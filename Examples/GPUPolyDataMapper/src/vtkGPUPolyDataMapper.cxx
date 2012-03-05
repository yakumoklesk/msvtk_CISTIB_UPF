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

#include "vtkGPUPolyDataMapper.h"

#include "vtkExecutive.h"
#include "vtkGraphicsFactory.h"
#include "vtkInformation.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkRenderWindow.h"


//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkGPUPolyDataMapper);

//----------------------------------------------------------------------------
// return the correct type of PolyDataMapper
vtkGPUPolyDataMapper *vtkGPUPolyDataMapper::New()
{
    // First try to create the object from the vtkObjectFactory
    vtkObject* ret = vtkGraphicsFactory::CreateInstance("vtkGPUPolyDataMapper");
    //vtkObject* ret = vtkGraphicsFactory::CreateInstance("vtkPolyDataMapper");
    return static_cast<vtkGPUPolyDataMapper *>(ret);
}

//----------------------------------------------------------------------------
vtkGPUPolyDataMapper::vtkGPUPolyDataMapper()
{
    this->Piece = 0;
    this->NumberOfPieces = 1;
    this->NumberOfSubPieces = 1;
    this->GhostLevel = 0;
}

//----------------------------------------------------------------------------
void vtkGPUPolyDataMapper::Render(vtkRenderer *ren, vtkActor *act)
{
    if (this->Static)
    {
        this->RenderPiece(ren,act);
        return;
    }

    int currentPiece, nPieces;
    vtkDataObject *input = this->GetInputDataObject(0, 0);

    if (input == NULL)
    {
        vtkErrorMacro("Mapper has no input.");
        return;
    }

    nPieces = this->NumberOfPieces * this->NumberOfSubPieces;

    for(int i=0; i<this->NumberOfSubPieces; i++)
    {
        // If more than one pieces, render in loop.
        currentPiece = this->NumberOfSubPieces * this->Piece + i;
        input->SetUpdateExtent(currentPiece, nPieces, this->GhostLevel);
        this->RenderPiece(ren, act);
    }
}

//----------------------------------------------------------------------------
void vtkGPUPolyDataMapper::SetInput(vtkPolyData *input)
{
    if(input)
    {
        this->SetInputConnection(0, input->GetProducerPort());
    }
    else
    {
        // Setting a NULL input removes the connection.
        this->SetInputConnection(0, 0);
    }
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
vtkPolyData *vtkGPUPolyDataMapper::GetInput()
{
    return vtkPolyData::SafeDownCast(
        this->GetExecutive()->GetInputData(0, 0));
}

// Update the network connected to this mapper.
void vtkGPUPolyDataMapper::Update()
{
    if (this->Static)
    {
        return;
    }

    int currentPiece, nPieces = this->NumberOfPieces;
    vtkPolyData* input = this->GetInput();

    // If the estimated pipeline memory usage is larger than
    // the memory limit, break the current piece into sub-pieces.
    if (input)
    {
        currentPiece = this->NumberOfSubPieces * this->Piece;
        input->SetUpdateExtent(currentPiece, this->NumberOfSubPieces*nPieces,
            this->GhostLevel);
    }

    this->vtkMapper::Update();
}

// Get the bounds for the input of this mapper as
// (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *vtkGPUPolyDataMapper::GetBounds()
{
    // do we have an input
    if ( ! this->GetNumberOfInputConnections(0))
    {
        vtkMath::UninitializeBounds(this->Bounds);
        return this->Bounds;
    }
    else
    {
        if (!this->Static)
        {
            // For proper clipping, this would be this->Piece, this->NumberOfPieces .
            // But that removes all benefites of streaming.
            // Update everything as a hack for paraview streaming.
            // This should not affect anything else, because no one uses this.
            // It should also render just the same.
            // Just remove this lie if we no longer need streaming in paraview :)
            //this->GetInput()->SetUpdateExtent(0, 1, 0);
            //this->GetInput()->Update();

            this->Update();
        }
        this->ComputeBounds();

        // if the bounds indicate NAN and subpieces are being used then
        // return NULL
        if (!vtkMath::AreBoundsInitialized(this->Bounds)
            && this->NumberOfSubPieces > 1)
        {
            return NULL;
        }
        return this->Bounds;
    }
}

void vtkGPUPolyDataMapper::ComputeBounds()
{
    this->GetInput()->GetBounds(this->Bounds);
}

void vtkGPUPolyDataMapper::ShallowCopy(vtkAbstractMapper *mapper)
{
    vtkGPUPolyDataMapper *m = vtkGPUPolyDataMapper::SafeDownCast(mapper);
    if ( m != NULL )
    {
        this->SetInput(m->GetInput());
        this->SetGhostLevel(m->GetGhostLevel());
        this->SetNumberOfPieces(m->GetNumberOfPieces());
        this->SetNumberOfSubPieces(m->GetNumberOfSubPieces());
    }

    // Now do superclass
    this->vtkMapper::ShallowCopy(mapper);
}

void vtkGPUPolyDataMapper::MapDataArrayToVertexAttribute(
    const char* vtkNotUsed(vertexAttributeName),
    const char* vtkNotUsed(dataArrayName),
    int vtkNotUsed(fieldAssociation),
    int vtkNotUsed(componentno)
    )
{
    vtkErrorMacro("Not impmlemented at this level...");
}

void vtkGPUPolyDataMapper::MapDataArrayToMultiTextureAttribute(
    int vtkNotUsed(unit),
    const char* vtkNotUsed(dataArrayName),
    int vtkNotUsed(fieldAssociation),
    int vtkNotUsed(componentno)
    )
{
    vtkErrorMacro("Not impmlemented at this level...");
}


void vtkGPUPolyDataMapper::RemoveVertexAttributeMapping(const char* vtkNotUsed(vertexAttributeName))
{
    vtkErrorMacro("Not impmlemented at this level...");
}


void vtkGPUPolyDataMapper::RemoveAllVertexAttributeMappings()
{
    vtkErrorMacro("Not impmlemented at this level...");
}


void vtkGPUPolyDataMapper::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "Piece : " << this->Piece << endl;
    os << indent << "NumberOfPieces : " << this->NumberOfPieces << endl;
    os << indent << "GhostLevel: " << this->GhostLevel << endl;
    os << indent << "Number of sub pieces: " << this->NumberOfSubPieces
        << endl;
}

//----------------------------------------------------------------------------
int vtkGPUPolyDataMapper::FillInputPortInformation(
    int vtkNotUsed( port ), vtkInformation* info)
{
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
}

