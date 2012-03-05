/*==============================================================================

  Library: MSVTK

  Copyright (c) Computational Image and Simulation Technologies in Biomedicine (CISTIB),
                Universitat Pompeu Fabra (UPF), Barcelona, Spain

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#include "vtkGPUPainterPolyDataMapper.h"

#include "vtkChooserPainter.h"
#include "vtkClipPlanesPainter.h"
#include "vtkCoincidentTopologyResolutionPainter.h"
#include "vtkCommand.h"
#include "vtkGPUDefaultPainter.h"
#include "vtkDisplayListPainter.h"
#include "vtkGarbageCollector.h"
#include "vtkGenericVertexAttributeMapping.h"
#include "vtkHardwareSelectionPolyDataPainter.h"
#include "vtkInformation.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkPlaneCollection.h"
#include "vtkPolyData.h"
#include "vtkPrimitivePainter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkScalarsToColorsPainter.h"
#include "vtkStandardPolyDataPainter.h"
#include "vtkgl.h"

vtkStandardNewMacro(vtkPainterGPUPolyDataMapper);
//-----------------------------------------------------------------------------
class vtkPainterGPUPolyDataMapperObserver : public vtkCommand
{
public:
    static vtkPainterGPUPolyDataMapperObserver* New()
    { return new vtkPainterGPUPolyDataMapperObserver; }

    virtual void Execute(vtkObject* caller, unsigned long event, void*)
    {
        vtkPainter* p = vtkPainter::SafeDownCast(caller);
        if (this->Target && p && event == vtkCommand::ProgressEvent)
        {
            this->Target->UpdateProgress(p->GetProgress());
        }
    }
    vtkPainterGPUPolyDataMapperObserver()
    {
        this->Target = 0;
    }
    vtkPainterGPUPolyDataMapper* Target;
};


//-----------------------------------------------------------------------------
vtkPainterGPUPolyDataMapper::vtkPainterGPUPolyDataMapper()
{
    this->Painter = 0;

    this->PainterInformation = vtkInformation::New();

    this->Observer = vtkPainterGPUPolyDataMapperObserver::New();
    this->Observer->Target = this;

    vtkGPUDefaultPainter* gdp = vtkGPUDefaultPainter::New();
    this->SetPainter(gdp);
    gdp->Delete();

    vtkChooserPainter* cp = vtkChooserPainter::New();
    this->Painter->SetDelegatePainter(cp);
    cp->Delete();

    this->SelectionPainter = 0;
    vtkPainter* selPainter = vtkHardwareSelectionPolyDataPainter::New();
    this->SetSelectionPainter(selPainter);
    selPainter->Delete();
}

//-----------------------------------------------------------------------------
vtkPainterGPUPolyDataMapper::~vtkPainterGPUPolyDataMapper()
{
    this->SetPainter(NULL);
    this->SetSelectionPainter(0);
    this->Observer->Target = NULL;
    this->Observer->Delete();
    this->PainterInformation->Delete();
}

//---------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::MapDataArrayToVertexAttribute(
    const char* vertexAttributeName,
    const char* dataArrayName, 
    int field,
    int componentno)
{
    vtkGenericVertexAttributeMapping* mappings = 0;
    if( this->PainterInformation->Has(
        vtkPrimitivePainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()) )
    {
        mappings = vtkGenericVertexAttributeMapping::SafeDownCast(
            this->PainterInformation->Get(
            vtkPolyDataPainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()));
    }

    if (mappings==NULL)
    {
        mappings = vtkGenericVertexAttributeMapping::New();
        this->PainterInformation->Set(
            vtkPolyDataPainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE(), mappings);
        mappings->Delete();
    }

    mappings->AddMapping(
        vertexAttributeName, dataArrayName, field, componentno);
}

//---------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::MapDataArrayToMultiTextureAttribute(
    int unit,
    const char* dataArrayName, 
    int field,
    int componentno)
{
    vtkGenericVertexAttributeMapping* mappings = 0;
    if( this->PainterInformation->Has(
        vtkPrimitivePainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()) )
    {
        mappings = vtkGenericVertexAttributeMapping::SafeDownCast(
            this->PainterInformation->Get(
            vtkPolyDataPainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()));
    }

    if (mappings==NULL)
    {
        mappings = vtkGenericVertexAttributeMapping::New();
        this->PainterInformation->Set(
            vtkPolyDataPainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE(), mappings);
        mappings->Delete();
    }

    mappings->AddMapping(
        unit, dataArrayName, field, componentno);
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::RemoveAllVertexAttributeMappings()
{
    vtkGenericVertexAttributeMapping* mappings = 0;
    if( this->PainterInformation->Has(
        vtkPrimitivePainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()) )
    {
        mappings = vtkGenericVertexAttributeMapping::SafeDownCast(
            this->PainterInformation->Get(
            vtkPolyDataPainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()));
        mappings->RemoveAllMappings();
    }
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::RemoveVertexAttributeMapping(
    const char* vertexAttributeName)
{
    vtkGenericVertexAttributeMapping* mappings = 0;
    if( this->PainterInformation->Has(
        vtkPrimitivePainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()) )
    {
        mappings = vtkGenericVertexAttributeMapping::SafeDownCast(
            this->PainterInformation->Get(
            vtkPolyDataPainter::DATA_ARRAY_TO_VERTEX_ATTRIBUTE()));
        mappings->RemoveMapping(vertexAttributeName);
    }
}


//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::SetPainter(vtkPainter* p)
{
    if (this->Painter)
    {
        this->Painter->RemoveObservers(vtkCommand::ProgressEvent, this->Observer);
        this->Painter->SetInformation(0);
    }
    vtkSetObjectBodyMacro(Painter, vtkPainter, p);

    if (this->Painter)
    {
        this->Painter->AddObserver(vtkCommand::ProgressEvent, this->Observer);
        this->Painter->SetInformation(this->PainterInformation);
    }
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::SetSelectionPainter(vtkPainter* p)
{
    if (this->SelectionPainter)
    {
        this->SelectionPainter->SetInformation(0);
        this->SelectionPainter->RemoveObservers(vtkCommand::ProgressEvent, this->Observer);
    }
    vtkSetObjectBodyMacro(SelectionPainter, vtkPainter, p);
    if (this->SelectionPainter)
    {
        this->SelectionPainter->AddObserver(vtkCommand::ProgressEvent, this->Observer);
        this->SelectionPainter->SetInformation(this->PainterInformation);
    }
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::ReportReferences(vtkGarbageCollector *collector)
{
    this->Superclass::ReportReferences(collector);
    vtkGarbageCollectorReport(collector, this->Painter, "Painter");
    vtkGarbageCollectorReport(collector, this->SelectionPainter, "SelectionPainter");
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::ReleaseGraphicsResources(vtkWindow *w)
{
    if (this->Painter)
    {
        this->Painter->ReleaseGraphicsResources(w);
    }
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::UpdatePainterInformation()
{
    vtkInformation* info = this->PainterInformation;

    info->Set(vtkPainter::STATIC_DATA(), this->Static);

    info->Set(vtkScalarsToColorsPainter::USE_LOOKUP_TABLE_SCALAR_RANGE(),
        this->GetUseLookupTableScalarRange());
    info->Set(vtkScalarsToColorsPainter::SCALAR_RANGE(), 
        this->GetScalarRange(), 2);
    info->Set(vtkScalarsToColorsPainter::SCALAR_MODE(), this->GetScalarMode());
    info->Set(vtkScalarsToColorsPainter::COLOR_MODE(), this->GetColorMode());
    info->Set(vtkScalarsToColorsPainter::INTERPOLATE_SCALARS_BEFORE_MAPPING(),
        this->GetInterpolateScalarsBeforeMapping());
    info->Set(vtkScalarsToColorsPainter::LOOKUP_TABLE(), this->LookupTable);
    info->Set(vtkScalarsToColorsPainter::SCALAR_VISIBILITY(), 
        this->GetScalarVisibility());
    info->Set(vtkScalarsToColorsPainter::ARRAY_ACCESS_MODE(), 
        this->ArrayAccessMode);
    info->Set(vtkScalarsToColorsPainter::ARRAY_ID(), this->ArrayId);
    info->Set(vtkScalarsToColorsPainter::ARRAY_NAME(), this->ArrayName);
    info->Set(vtkScalarsToColorsPainter::ARRAY_COMPONENT(), this->ArrayComponent);
    info->Set(vtkScalarsToColorsPainter::SCALAR_MATERIAL_MODE(), 
        this->GetScalarMaterialMode());

    info->Set(vtkClipPlanesPainter::CLIPPING_PLANES(), this->ClippingPlanes);

    info->Set(vtkCoincidentTopologyResolutionPainter::RESOLVE_COINCIDENT_TOPOLOGY(),
        this->GetResolveCoincidentTopology());
    info->Set(vtkCoincidentTopologyResolutionPainter::Z_SHIFT(),
        this->GetResolveCoincidentTopologyZShift());
    double p[2];
    this->GetResolveCoincidentTopologyPolygonOffsetParameters(p[0], p[1]);
    info->Set(vtkCoincidentTopologyResolutionPainter::POLYGON_OFFSET_PARAMETERS(),
        p, 2);
    info->Set(vtkCoincidentTopologyResolutionPainter::POLYGON_OFFSET_FACES(),
        this->GetResolveCoincidentTopologyPolygonOffsetFaces());

    int immr = (this->ImmediateModeRendering || 
        vtkMapper::GetGlobalImmediateModeRendering());
    info->Set(vtkDisplayListPainter::IMMEDIATE_MODE_RENDERING(), immr);
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::RenderPiece(vtkRenderer* ren, vtkActor* act)
{
    vtkDataObject *input= this->GetInputDataObject(0, 0);

    vtkStandardPolyDataPainter * painter =
        vtkStandardPolyDataPainter::SafeDownCast(this->Painter);
    if (painter != NULL && vtkPolyData::SafeDownCast(input))
    {
        // FIXME: This is not supported currently for composite datasets.
        vtkInformationVector *inArrayVec =
            this->Information->Get(INPUT_ARRAYS_TO_PROCESS());
        int numArrays = inArrayVec->GetNumberOfInformationObjects();

        for(int i = 0; i < numArrays; i++)
        {
            painter->AddMultiTextureCoordsArray(this->GetInputArrayToProcess(i,input));
        }
    }

    //
    // make sure that we've been properly initialized
    //
    if (ren->GetRenderWindow()->CheckAbortStatus())
    {
        return;
    }

    if ( input == NULL )
    {
        vtkErrorMacro(<< "No input!");
        return;
    }
    else
    {
        this->InvokeEvent(vtkCommand::StartEvent,NULL);
        if (!this->Static)
        {
            input->Update();
        }
        this->InvokeEvent(vtkCommand::EndEvent,NULL);

        // This check is unnecessary since the mapper will be cropped out by culling
        // if it returns invalid bounds which is what will happen when input has no
        // points.
        // vtkIdType numPts = input->GetNumberOfPoints();
        // if (numPts == 0)
        //   {
        //   vtkDebugMacro(<< "No points!");
        //   return;
        //   }
    }

    // Update Painter information if obsolete.
    if (this->PainterUpdateTime < this->GetMTime())
    {
        this->UpdatePainterInformation();
        this->PainterUpdateTime.Modified();
    }

    // make sure our window is current
    ren->GetRenderWindow()->MakeCurrent();
    this->TimeToDraw = 0.0;

    // If we are rendering in selection mode, then we use the selection painter
    // instead of the standard painter.
    if (this->SelectionPainter && ren->GetSelector())
    {
        this->SelectionPainter->SetInput(input);
        this->SelectionPainter->Render(ren, act, 0xff,
            (this->ForceCompileOnly==1));
        this->TimeToDraw = this->SelectionPainter->GetTimeToDraw();
    }
    else if (this->SelectionPainter && this->SelectionPainter != this->Painter)
    {
        this->SelectionPainter->ReleaseGraphicsResources(ren->GetRenderWindow());
    }

    if (this->Painter && ren->GetSelector() == 0)
    {
        // Pass polydata.
        this->Painter->SetInput(input);
        this->Painter->Render(ren, act, 0xff,this->ForceCompileOnly==1);
        this->TimeToDraw = this->Painter->GetTimeToDraw();
    }

    // If the timer is not accurate enough, set it to a small
    // time so that it is not zero
    if ( this->TimeToDraw == 0.0 )
    {
        this->TimeToDraw = 0.0001;
    }

    this->UpdateProgress(1.0);
}

//-------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::ComputeBounds()
{
    this->GetInput()->GetBounds(this->Bounds);

    // if the mapper has a painter, update the bounds in the painter
    vtkPainter *painter = this->GetPainter();
    if (painter)
    {
        // Update Painter information if obsolete.
        if (this->PainterUpdateTime < this->GetMTime())
        {
            this->UpdatePainterInformation();
            this->PainterUpdateTime.Modified();
        }
        painter->UpdateBounds(this->Bounds);
    }
}

//-----------------------------------------------------------------------------
void vtkPainterGPUPolyDataMapper::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Painter: " ;
    if (this->Painter)
    {
        os << endl;
        this->Painter->PrintSelf(os, indent.GetNextIndent());
    }
    else
    {
        os << indent << "(none)" << endl;
    }
    os << indent << "SelectionPainter: " << this->SelectionPainter << endl;
}

