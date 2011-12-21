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

#include "vtkMultipleStructuredPointsReader.h"

#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkErrorCode.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkTemporalDataSet.h>
#include <vtkStringArray.h>

// Debug -->
#include <vtkInformationDataObjectKey.h>
// Debug <--

#include <math.h>

using namespace std;

vtkStandardNewMacro( vtkMultipleStructuredPointsReader );

vtkMultipleStructuredPointsReader::vtkMultipleStructuredPointsReader()
{
    this->DataObjectType = VTK_STRUCTURED_POINTS;
}

vtkMultipleStructuredPointsReader::~vtkMultipleStructuredPointsReader()
{
}


//----------------------------------------------------------------------------
void vtkMultipleStructuredPointsReader::SetOutput( vtkTemporalDataSet* output )
{
    this->GetExecutive()->SetOutputData( 0, output );
}

//----------------------------------------------------------------------------
vtkTemporalDataSet* vtkMultipleStructuredPointsReader::GetOutput()
{
    return this->GetOutput(0);
}

//----------------------------------------------------------------------------
vtkTemporalDataSet* vtkMultipleStructuredPointsReader::GetOutput(int idx)
{
    return vtkTemporalDataSet::SafeDownCast(this->GetOutputDataObject(idx));
}

vtkDataReader* vtkMultipleStructuredPointsReader::GetConcreteReaderInstance()
{
    return vtkStructuredPointsReader::New();
}

void vtkMultipleStructuredPointsReader::SetOutputTimeStep( vtkDataReader* pDataReader, vtkTemporalDataSet *outputData, int timeStep )
{
    vtkStructuredPointsReader* pStructuredPointsReader = vtkStructuredPointsReader::SafeDownCast( pDataReader );
    if( pStructuredPointsReader )
    {
        vtkStructuredPoints* pStructuredPoints = pStructuredPointsReader->GetOutput();
        outputData->SetTimeStep( timeStep, pStructuredPoints );
    }
}

void vtkMultipleStructuredPointsReader::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
}

