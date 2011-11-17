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

#include "vtkTemporalDataSetTimeStepProvider.h"

#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkDataSet.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkStreamingDemandDrivenPipeline.h>

using namespace std;

vtkStandardNewMacro( vtkTemporalDataSetTimeStepProvider );

vtkTemporalDataSetTimeStepProvider::vtkTemporalDataSetTimeStepProvider()
: NextTimeStep( 0 )
{
    this->SetNumberOfInputPorts( 1 );
    this->SetNumberOfInputPorts( 1 );
}

vtkTemporalDataSetTimeStepProvider::~vtkTemporalDataSetTimeStepProvider()
{

}

/*
void vtkTemporalDataSetTimeStepProvider::SetNextTimeStep( int NextTimeStep )
{
    this->NextTimeStep = NextTimeStep;
    // We don't want to modify the algorithm time, because we will only change
    // the UPDATE_TIME_STEPS
}
*/

//----------------------------------------------------------------------------
int vtkTemporalDataSetTimeStepProvider
::RequestUpdateExtent (vtkInformation* request,
                       vtkInformationVector **inputVector,
                       vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

    int numUpdateTimeSteps = this->CacheSize;
    if( numUpdateTimeSteps > 0 )
    {
        // TODO:
        // Watch that this->NextTimeStep is valid
        double* updateTimeSteps = new double[numUpdateTimeSteps];
        for( int indexCache = 0; indexCache < this->CacheSize; indexCache++ )
        {
            updateTimeSteps[indexCache] = ( this->NextTimeStep + indexCache );
        }
        outInfo->Set( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS(), updateTimeSteps, numUpdateTimeSteps );

        // We don't need time steps information any longer
        delete[] updateTimeSteps;
    }
    else
    {
        vtkDebugMacro(<<"CacheSize size is zero, returning...");
    }

    // From vtkTemporalDataSetCache
    CacheType::iterator pos;
    vtkstd::vector<double> reqTimeSteps;
    if( outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS()) )
    {
        double *upTimes = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS());
        int numTimes = outInfo->Length(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS());
        int i;
        for( i = 0; i < numTimes; ++i )
        {
            // do we have this time step?
            pos = this->Cache.find( upTimes[i] );
            if( pos == this->Cache.end() )
            {
                reqTimeSteps.push_back( upTimes[i] );
            }
        }

        // if we need any data
        if( reqTimeSteps.size() )
        {
            inInfo->Set( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS(),
                &reqTimeSteps[0],static_cast<int>(reqTimeSteps.size()) );
        }
        // otherwise leave the input with what it already has 
        else
        {
            vtkDataObject *dobj = inInfo->Get(vtkDataObject::DATA_OBJECT());
            if( dobj )
            {
                double *its = dobj->GetInformation()->Get( vtkDataObject::DATA_TIME_STEPS() );
                int itsSize = dobj->GetInformation()->Length( vtkDataObject::DATA_TIME_STEPS() );
                inInfo->Set( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS(), its, itsSize );
            }
        }
    }

    //return this->Superclass::RequestUpdateExtent( request, inputVector, outputVector );
    return 1;
}

int vtkTemporalDataSetTimeStepProvider::FillOutputPortInformation(
    int vtkNotUsed(port), vtkInformation* info)
{
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkTemporalDataSet");
    return 1;
}

void vtkTemporalDataSetTimeStepProvider::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
}
