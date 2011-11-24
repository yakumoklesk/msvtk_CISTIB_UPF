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

#include "vtkMultipleDataReader.h"

#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkErrorCode.h>
#include <vtkDataReader.h>
#include <vtkStdString.h>
#include <vtkStringArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkGlobFileNames.h>
#include <vtkDataObjectTypes.h>
#include <vtkTemporalDataSet.h>

using namespace std;

vtkStandardNewMacro( vtkMultipleDataReader );

vtkMultipleDataReader::vtkMultipleDataReader()
: DirectoryName( 0 )
, WildCard( 0 )
, FileNames( 0 )
, CachedTimeSteps( 0 )
, m_RequestedTimeSteps( 0 )
, m_NumRequestedTimeSteps( 0 )
, m_DataObjectType( VTK_DATA_OBJECT )
, m_NumAvailableTimeSteps( 0 )
{
    this->SetNumberOfInputPorts( 0 );
}

vtkMultipleDataReader::~vtkMultipleDataReader()
{
    if( this->DirectoryName )
    {
        delete[] DirectoryName; 
    }
    if( this->WildCard )
    {
        delete[] WildCard; 
    }
    if( this->FileNames )
    {
        this->FileNames->Delete();
    }
    if( this->m_RequestedTimeSteps )
    {
        delete this->m_RequestedTimeSteps;
    }
}

void vtkMultipleDataReader::SetRequestedTimesTeps( double* timeSteps, int numTimeSteps )
{
    // TODO?: Compare first?
    if( this->m_RequestedTimeSteps )
    {
        delete this->m_RequestedTimeSteps;
    }
    m_NumRequestedTimeSteps = numTimeSteps;
    m_RequestedTimeSteps = new double[m_NumRequestedTimeSteps];
    for( int indexTimeStep = 0; indexTimeStep < m_NumRequestedTimeSteps; indexTimeStep++ )
    {
        m_RequestedTimeSteps[indexTimeStep] = timeSteps[indexTimeStep];
    }
    this->Modified();
}

double* vtkMultipleDataReader::GetRequestedTimesTeps()
{
    if( this->m_RequestedTimeSteps )
    {
        double* result = new double[m_NumRequestedTimeSteps];
        for( int indexTimeStep = 0; indexTimeStep < m_NumRequestedTimeSteps; indexTimeStep++ )
        {
            result[indexTimeStep] = m_RequestedTimeSteps[indexTimeStep];
        }
        return result;
    }
    return 0;
}

int vtkMultipleDataReader::ProcessRequest( vtkInformation* request,
                                  vtkInformationVector** inputVector,
                                  vtkInformationVector* outputVector )
{
    // generate the data
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
        return this->RequestData(request, inputVector, outputVector);
    }

    if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
    {
        return this->RequestUpdateExtent(request, inputVector, outputVector);
    }

    // execute information
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
    {
        return this->RequestInformation(request, inputVector, outputVector);
    }

    return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
// Default method performs Update to get information
int vtkMultipleDataReader::RequestInformation(
    vtkInformation *,
    vtkInformationVector **,
    vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // We don't want that pipeline set the UPDATE_TIME_STEPS to { 0.0 }, so we init
    // the extents ourselves, set our UPDATE_TIME_STEPS to lengh 0 and mark 
    // UPDATE_EXTEND_INITIALIZED
    vtkStreamingDemandDrivenPipeline *sddp = 
        vtkStreamingDemandDrivenPipeline::SafeDownCast( this->GetExecutive() );
    if( sddp )
    {
        vtkDebugMacro(<<"Setting UPDATE_TIME_STEPS length to 0...");

        sddp->SetUpdateExtentToWholeExtent( outInfo );
        double dummy[] = { 0 };
        outInfo->Set( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS(), dummy, 0 );
        outInfo->Set( vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT_INITIALIZED(), 1 );
    }

    return this->ReadMetaData(outInfo);
}

//----------------------------------------------------------------------------
int vtkMultipleDataReader::ReadMetaData( vtkInformation *outInfo )
{
    this->SetErrorCode( vtkErrorCode::NoError );

    vtkDebugMacro(<<"Retrieving number of timesteps available...");

    int numDataTimeSteps( 0 );
    if( !this->FileNames )
    {
        this->FileNames = this->GetFileNames();
        if( ( !this->FileNames ) || ( ( numDataTimeSteps = this->FileNames->GetNumberOfTuples() ) <= 0 ) )
        {
            vtkErrorMacro(<< "No filenames were found in " << this->DirectoryName << " with pattern " << this->WildCard);
            return 0;
        }
    }

    if( this->GetSeriesDataObjectType( this->FileNames ) != m_DataObjectType )
    {
        vtkErrorMacro(<< "Input data is not of type " << vtkDataObjectTypes::GetClassNameFromTypeId( m_DataObjectType ) );
        return 0;
    }


    m_NumRequestedTimeSteps = numDataTimeSteps;
    double* dataTimeSteps = new double[numDataTimeSteps];
    for( int timeStep = 0; timeStep < numDataTimeSteps; timeStep++ )
    {
        dataTimeSteps[timeStep] = static_cast<double>( timeStep );
    }

    outInfo->Set( vtkStreamingDemandDrivenPipeline::TIME_STEPS(), dataTimeSteps, numDataTimeSteps );

    // Also put time range If not, vtkStreamingDemandDrivenPipeline::NeedToExecuteBasedOnTime will
    // return prematurely
    // At the moment, we don't have a metadata place where this range is stored, but there should be
    static double timeRange[] = { 0, 1 };
    outInfo->Set( vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2 );

    delete[] dataTimeSteps;

    return 1;
}

int vtkMultipleDataReader
::RequestUpdateExtent( vtkInformation* request,
                       vtkInformationVector **inputVector,
                       vtkInformationVector *outputVector )
{
    // get the info objects
    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    // First test if we have already requested time steps from another filter
    if( !outInfo->Has( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS() ) || 
        ( outInfo->Length( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS() ) <= 0 ) )
    {
        // If not, then fill the user requested time steps
        if( m_NumRequestedTimeSteps > 0 )
        {
            outInfo->Set( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS(), m_RequestedTimeSteps, m_NumRequestedTimeSteps );
        }
        else
        {
            vtkDebugMacro(<<"There are not requested time steps...");
        }
    }
    else
    {
        int numUpdatedTimeSteps( outInfo->Length( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS() ) );
        double* updateTimeSteps( outInfo->Get( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS() ) );
        int a = 1;
    }

    return 1;
}

int vtkMultipleDataReader::RequestData(
    vtkInformation *,
    vtkInformationVector **,
    vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    this->SetErrorCode( vtkErrorCode::NoError );
    int done=0;
    vtkTemporalDataSet *outputData = vtkTemporalDataSet::SafeDownCast(
        outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // ImageSource superclass does not do this.
    //outputData->ReleaseData();

    int numUpdateTimeSteps( 0 );
    double* updateTimeSteps( 0 );
    if( outInfo->Has( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS() ) )
    {
        numUpdateTimeSteps = outInfo->Length( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS() );
        updateTimeSteps = outInfo->Get( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS() );
    }
    else
    {
        // No timesteps wanted, then return;
        vtkDebugMacro(<<"No timesteps wanted, skipping...");
        return 1;
    }

    if( numUpdateTimeSteps )
    {
        outputData->SetNumberOfTimeSteps( numUpdateTimeSteps );

        vtkDebugMacro(<<"Reading vtk structured time steps...");

        vtkSmartPointer<vtkDataReader> dataReaderSP;
        dataReaderSP.TakeReference( this->GetConcreteReaderInstance() );
        int indexTimeStep = 0;
        int leftTimeSteps = numUpdateTimeSteps;
        while( leftTimeSteps )
        {
            int timeStepToLoad = updateTimeSteps[indexTimeStep];

            dataReaderSP->SetFileName( this->FileNames->GetValue( timeStepToLoad ) );
            dataReaderSP->Update();

            // Assign the timesteps indexed from zero
            this->SetOutputTimeStep( dataReaderSP, outputData, indexTimeStep );

            indexTimeStep++;
            leftTimeSteps--;
        }

        outputData->GetInformation()->Set( vtkDataObject::DATA_TIME_STEPS(), 
            updateTimeSteps, numUpdateTimeSteps );
    }

    return 1;
}

vtkDataReader* vtkMultipleDataReader::GetConcreteReaderInstance()
{
    vtkErrorMacro(<< "GetConcreteReaderInstance called from base class vtkMultipleDataReader" );
    return NULL;
}

void vtkMultipleDataReader::SetOutputTimeStep( vtkDataReader* pDataReader, vtkTemporalDataSet *outputData, int timeStep )
{
    vtkErrorMacro(<< "SetOutputTimeStep called from base class vtkMultipleDataReader" );
}

int vtkMultipleDataReader::GetDataObjectType( const vtkStdString& fileName )
{
    int resultType = -1;
    char line[1024];
    vtkSmartPointer<vtkDataReader> dataReaderSP = vtkSmartPointer<vtkDataReader>::New();
    dataReaderSP->SetFileName( fileName.c_str() );
    if( dataReaderSP->OpenVTKFile() && dataReaderSP->ReadHeader() )
    {
        // From vtkDataReader::IsFileValid
        if( !dataReaderSP->ReadString( line ) )
        {
            //vtkErrorWithObjectMacro( dataReaderSP, <<"Data file ends prematurely!" );
            dataReaderSP->CloseVTKFile();
            //dataReaderSP->SetErrorCode( vtkErrorCode::PrematureEndOfFileError );
            return -1;
        }
        if( !strncmp( dataReaderSP->LowerCase( line ), "dataset", (unsigned long)7) )
        {
            if( !dataReaderSP->ReadString( line ) )
            {
                //vtkErrorWithObjectMacro( dataReaderSP, <<"Data file ends prematurely!" );
                dataReaderSP->CloseVTKFile();
                //dataReaderSP->SetErrorCode( vtkErrorCode::PrematureEndOfFileError );
                return -1;
            } 
            char* lowerCaseLine = dataReaderSP->LowerCase( line );
            if( !strncmp( lowerCaseLine, "polydata" , strlen( "polydata" ) ) )
            {
                resultType = VTK_POLY_DATA;
            }
            else if( !strncmp( lowerCaseLine, "structured_points" , strlen( "structured_points" ) ) )
            {
                resultType = VTK_STRUCTURED_POINTS;
            }
            else if( !strncmp( lowerCaseLine, "structured_grid" , strlen( "structured_grid" ) ) )
            {
                resultType = VTK_STRUCTURED_GRID;
            }
            else if( !strncmp( lowerCaseLine, "rectilinear_grid" , strlen( "rectilinear_grid" ) ) )
            {
                resultType = VTK_RECTILINEAR_GRID;
            }
            else if( !strncmp( lowerCaseLine, "unstructured_grid" , strlen( "unstructured_grid" ) ) )
            {
                resultType = VTK_UNSTRUCTURED_GRID;
            }
            else
            {
                resultType = -1;
            }
            dataReaderSP->CloseVTKFile();
        }
    }

    return resultType;
}

int vtkMultipleDataReader::GetSeriesDataObjectType( vtkStringArray* series )
{
    int resultType = -1;
    int auxResultType = -1;

    vtkStdString& firstFileName = series->GetValue( 0 );
    resultType = GetDataObjectType( firstFileName );
    if( ( ( resultType >= VTK_POLY_DATA ) && ( resultType <= VTK_UNSTRUCTURED_GRID ) ) )
    {
        auxResultType = resultType;
        vtkIdType indexFile = 1;
        while( indexFile < series->GetNumberOfValues() && ( auxResultType == resultType ) )
        {
            vtkStdString& currentFileName = series->GetValue( indexFile );
            auxResultType = GetDataObjectType( currentFileName );
            indexFile++;
        }
    }

    return ( auxResultType == resultType )?resultType:-1;
}

void vtkMultipleDataReader::SetFileNames( vtkStringArray* fileNames )
{
    // Notimg passed? Return;
    if( !fileNames )
    {
        return;
    }
    // Have something?
    if( this->FileNames )
    {
        // Same object? Return;
        if( this->FileNames == fileNames )
        {
            return;
        }
        // TODO: Test every entry? Maybe more expensive than Delete + DeepCopy
        this->FileNames->Delete();
    }
    this->FileNames = vtkStringArray::New();
    this->FileNames->DeepCopy( fileNames );
}

vtkStringArray* vtkMultipleDataReader::GetFileNames()
{
    //if( this->FileNames )
    //{
    //    return this->FileNames;
    //}

    vtkSmartPointer<vtkGlobFileNames> globFileNamesSP = vtkSmartPointer<vtkGlobFileNames>::New();
    globFileNamesSP->SetDirectory( this->DirectoryName );
    globFileNamesSP->RecurseOff();
    if( globFileNamesSP->AddFileNames( this->WildCard ) )
    {
        int a = globFileNamesSP->GetNumberOfFileNames();
        vtkStringArray* foundFiles = vtkStringArray::New();
        //foundFiles->Register( this );
        foundFiles->DeepCopy( globFileNamesSP->GetFileNames() );

        return foundFiles;
    }

    return 0;
}

int vtkMultipleDataReader::GetAvailableTimeSteps()
{
    // Update the information. If we are already up to date, it will not execute
    this->UpdateInformation();

    return m_NumAvailableTimeSteps;
}


void vtkMultipleDataReader::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );

    os << indent << "DirectoryName: " << this->DirectoryName << "\n";
    os << indent << "WildCard: " << this->WildCard << "\n";

    //vtkStringArray* FileNames;
    //int CachedTimeSteps;	// 0 means all
    //double* m_RequestedTimeSteps;
    
    os << indent << "m_RequestedTimeSteps: " << this->m_RequestedTimeSteps << "\n";
    os << indent << "m_DataObjectType: " << this->m_DataObjectType << "\n";
    os << indent << "m_NumAvailableTimeSteps: " << this->m_NumAvailableTimeSteps << "\n";
}

