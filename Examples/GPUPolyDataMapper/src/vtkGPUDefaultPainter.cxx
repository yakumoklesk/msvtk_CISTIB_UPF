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
#include "vtkGPUDefaultPainter.h"

#include "vtkActor.h"
#include "vtkClipPlanesPainter.h"
#include "vtkCoincidentTopologyResolutionPainter.h"
#include "vtkCompositePainter.h"
#include "vtkDisplayListPainter.h"
#include "vtkGarbageCollector.h"
#include "vtkLightingPainter.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkRepresentationPainter.h"
#include "vtkScalarsToColorsPainter.h"

vtkStandardNewMacro(vtkGPUDefaultPainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, DefaultPainterDelegate, vtkPainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, ScalarsToColorsPainter, 
                     vtkScalarsToColorsPainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, ClipPlanesPainter,
                     vtkClipPlanesPainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, DisplayListPainter,
                     vtkDisplayListPainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, CompositePainter, vtkCompositePainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, CoincidentTopologyResolutionPainter,
                     vtkCoincidentTopologyResolutionPainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, LightingPainter, vtkLightingPainter);
vtkCxxSetObjectMacro(vtkGPUDefaultPainter, RepresentationPainter, vtkRepresentationPainter);
//-----------------------------------------------------------------------------
vtkGPUDefaultPainter::vtkGPUDefaultPainter()
{
    this->ScalarsToColorsPainter = 0;
    this->ClipPlanesPainter = 0;
    this->DisplayListPainter = 0;
    this->CompositePainter = 0;
    this->CoincidentTopologyResolutionPainter = 0;
    this->LightingPainter = 0;
    this->RepresentationPainter = 0;
    this->DefaultPainterDelegate = 0;

    vtkScalarsToColorsPainter* scp = vtkScalarsToColorsPainter::New();
    this->SetScalarsToColorsPainter(scp);
    scp->Delete();

    vtkClipPlanesPainter* cp = vtkClipPlanesPainter::New();
    this->SetClipPlanesPainter(cp);
    cp->Delete();

    vtkDisplayListPainter* dlp = vtkDisplayListPainter::New();
    this->SetDisplayListPainter(dlp);
    dlp->Delete();

    vtkCompositePainter* cpp = vtkCompositePainter::New();
    this->SetCompositePainter(cpp);
    cpp->Delete();

    vtkCoincidentTopologyResolutionPainter* ctrp =
        vtkCoincidentTopologyResolutionPainter::New();
    this->SetCoincidentTopologyResolutionPainter(ctrp);
    ctrp->Delete();

    vtkLightingPainter* lp = vtkLightingPainter::New();
    this->SetLightingPainter(lp);
    lp->Delete();

    vtkRepresentationPainter* vp = vtkRepresentationPainter::New();
    this->SetRepresentationPainter(vp);
    vp->Delete();
}

//-----------------------------------------------------------------------------
vtkGPUDefaultPainter::~vtkGPUDefaultPainter()
{
    this->SetScalarsToColorsPainter(0);
    this->SetClipPlanesPainter(0);
    this->SetDisplayListPainter(0);
    this->SetCompositePainter(0);
    this->SetCoincidentTopologyResolutionPainter(0);
    this->SetLightingPainter(0);
    this->SetRepresentationPainter(0);
    this->SetDefaultPainterDelegate(0);
}

//-----------------------------------------------------------------------------
void vtkGPUDefaultPainter::BuildPainterChain()
{
    vtkPainter* headPainter = 0;
    vtkPainter* prevPainter = 0;
    vtkPainter* painter = 0;

/*
    painter = this->GetScalarsToColorsPainter();
    if (painter)
    {
        if (prevPainter)
        {
            prevPainter->SetDelegatePainter(painter);
        }
        prevPainter = painter;
        headPainter = (headPainter)? headPainter : painter;
    }

    painter = this->GetClipPlanesPainter();
    if (painter)
    {
        if (prevPainter)
        {
            prevPainter->SetDelegatePainter(painter);
        }
        prevPainter = painter;
        headPainter = (headPainter)? headPainter : painter;
    }
*/
    painter = this->GetDisplayListPainter();
    if (painter)
    {
        if (prevPainter)
        {
            prevPainter->SetDelegatePainter(painter);
        }
        prevPainter = painter;
        headPainter = (headPainter)? headPainter : painter;
    }
/*
    // We are always putting in the composite painter since it does not have any
    // significant overhead for non-composite datasets.
    painter = this->GetCompositePainter();
    if (painter)
    {
        if (prevPainter)
        {
            prevPainter->SetDelegatePainter(painter);
        }
        prevPainter = painter;
        headPainter = (headPainter)? headPainter : painter;
    }

    painter = this->GetLightingPainter();
    if (painter)
    {
        if (prevPainter)
        {
            prevPainter->SetDelegatePainter(painter);
        }
        prevPainter = painter;
        headPainter = (headPainter)? headPainter : painter;
    }

    painter = this->GetRepresentationPainter();
    if (painter)
    {
        if (prevPainter)
        {
            prevPainter->SetDelegatePainter(painter);
        }
        prevPainter = painter;
        headPainter = (headPainter)? headPainter : painter;
    }

    painter = this->GetCoincidentTopologyResolutionPainter();
    if (painter)
    {
        if (prevPainter)
        {
            prevPainter->SetDelegatePainter(painter);
        }
        prevPainter = painter;
        headPainter = (headPainter)? headPainter : painter;
    }
*/

    // this will set in internal delegate painter.
    this->Superclass::SetDelegatePainter(headPainter);
    if (prevPainter)
    {
        prevPainter->SetDelegatePainter(this->DefaultPainterDelegate);
    }
}
//-----------------------------------------------------------------------------
void vtkGPUDefaultPainter::Render(vtkRenderer* renderer, vtkActor* actor, 
                               unsigned long typeflags, bool forceCompileOnly)
{
    if (this->ChainBuildTime < this->MTime)
    {
        this->BuildPainterChain();
        this->ChainBuildTime.Modified();
    }
    this->Superclass::Render(renderer, actor, typeflags,forceCompileOnly);
}

//-----------------------------------------------------------------------------
void vtkGPUDefaultPainter::ReleaseGraphicsResources(vtkWindow *window)
{
    if (this->DefaultPainterDelegate)
    {
        this->DefaultPainterDelegate->ReleaseGraphicsResources(window);
    }
    if (this->ScalarsToColorsPainter)
    {
        this->ScalarsToColorsPainter->ReleaseGraphicsResources(window);
    }
    this->Superclass::ReleaseGraphicsResources(window);
}

//-----------------------------------------------------------------------------
void vtkGPUDefaultPainter::SetDelegatePainter(vtkPainter* painter)
{
    this->SetDefaultPainterDelegate(painter);
}

//-----------------------------------------------------------------------------
void vtkGPUDefaultPainter::ReportReferences(vtkGarbageCollector* collector)
{
    this->Superclass::ReportReferences(collector);
    vtkGarbageCollectorReport(collector, this->ScalarsToColorsPainter,
        "ScalarsToColors Painter");
    vtkGarbageCollectorReport(collector, this->DisplayListPainter,
        "DisplayListPainter");
    vtkGarbageCollectorReport(collector, this->ClipPlanesPainter,
        "ClipPlanes Painter");
    vtkGarbageCollectorReport(collector, this->CompositePainter,
        "Composite Painter");
    vtkGarbageCollectorReport(collector, 
        this->CoincidentTopologyResolutionPainter,
        "CoincidentTopologyResolution Painter");
    vtkGarbageCollectorReport(collector, this->LightingPainter,
        "Lighting Painter");
    vtkGarbageCollectorReport(collector, this->RepresentationPainter,
        "Wireframe Painter");
    vtkGarbageCollectorReport(collector, this->DefaultPainterDelegate,
        "DefaultPainter Delegate");
}

//-------------------------------------------------------------------------
void vtkGPUDefaultPainter::UpdateBounds(double bounds[6])
{
    // need the superclass to start with the first painter in the chain
    vtkPainter *painter = this->Superclass::GetDelegatePainter();

    if( painter )
    {
        // delegate the task of updating the bounds
        painter->UpdateBounds(bounds);
    }
    else
    {
        // no painter in the chain. let's build the chain if needed.
        if (this->ChainBuildTime < this->MTime)
        {
            // build the chain of painters
            this->BuildPainterChain();
            this->ChainBuildTime.Modified();
        }

        // try again to get the first painter in the chain
        painter = this->Superclass::GetDelegatePainter();

        if( painter )
        {
            //delegate the task of updating the bounds
            painter->UpdateBounds(bounds);
        }
    }
}

//-----------------------------------------------------------------------------
void vtkGPUDefaultPainter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "ScalarsToColorsPainter: " ;
    if (this->ScalarsToColorsPainter)
    {
        os << endl ;
        this->ScalarsToColorsPainter->PrintSelf(os, indent.GetNextIndent());
    }
    else
    {
        os << "(none)" << endl;
    }

    os << indent << "ClipPlanesPainter: " ;
    if (this->ClipPlanesPainter)
    {
        os << endl ;
        this->ClipPlanesPainter->PrintSelf(os, indent.GetNextIndent());
    }
    else
    {
        os << "(none)" << endl;
    }

    os << indent << "DisplayListPainter: " ;
    if (this->DisplayListPainter)
    {
        os << endl;
        this->DisplayListPainter->PrintSelf(
            os, indent.GetNextIndent());
    }
    else
    {
        os << "(none)" << endl;
    }

    os << indent << "CompositePainter: ";
    if (this->CompositePainter)
    {
        os << endl;
        this->CompositePainter->PrintSelf(os, indent.GetNextIndent());
    }
    else
    {
        os << "(none)" << endl;
    }

    os << indent << "CoincidentTopologyResolutionPainter: " ;
    if (this->CoincidentTopologyResolutionPainter)
    {
        os << endl;
        this->CoincidentTopologyResolutionPainter->PrintSelf(
            os, indent.GetNextIndent());
    }
    else
    {
        os << "(none)" << endl;
    }

    os << indent << "LightingPainter: " ;
    if (this->LightingPainter)
    {
        os << endl ;
        this->LightingPainter->PrintSelf(os, indent.GetNextIndent());
    }
    else
    {
        os << "(none)" << endl;
    }

    os << indent << "RepresentationPainter: " ;
    if (this->RepresentationPainter)
    {
        os << endl ;
        this->RepresentationPainter->PrintSelf(os, indent.GetNextIndent());
    }
    else
    {
        os << "(none)" << endl;
    }
}
