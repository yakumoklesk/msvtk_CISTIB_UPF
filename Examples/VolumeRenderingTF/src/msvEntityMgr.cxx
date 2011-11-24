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

#include "msvEntityMgr.h"

#include <vtkObjectFactory.h>
#include <vtkGlobFileNames.h>
#include "vtkMultipleDataReader.h"
#include "vtkMultipleStructuredPointsReader.h"
#include "vtkTemporalDataSetTimeStepProvider.h"
#include "vtkTemporalDataSet.h"
#include "vtkMultiplePolyDataReader.h"
#include <vtkStructuredPoints.h>
#include <vtkPolyData.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStdString.h>
#include <vtkStringArray.h>
#include <vtkRenderer.h>

#include "msvEntity.h"

#include <vector>

using namespace std;

struct msvEntityInfoEntry
{
    msvEntity* Entity;
    vtkSmartPointer<vtkMultipleDataReader> multipleDataReader;
    vtkTemporalDataSetTimeStepProvider* TSProvider;
};

vtkStandardNewMacro( msvEntityMgr );

msvEntityMgr::msvEntityMgr()
: m_AssignedRenderer( 0 )
{
    
}

msvEntityMgr::~msvEntityMgr()
{
    vector<msvEntityInfoEntry*>::size_type index;
    for( index = 0; index < m_EntityInfoEntries.size(); index++ )
    {
        //m_EntityInfoEntries[index]->Delete();
        //delete m_EntityInfoEntries[index];
        m_EntityInfoEntries[index]->Entity->Delete();
        m_EntityInfoEntries[index]->multipleDataReader->Delete();
        m_EntityInfoEntries[index]->TSProvider->Delete();
        delete m_EntityInfoEntries[index];
    }
}

msvEntity* msvEntityMgr::CreateEntityFromSeriesOfStructuredPoints( vtkStringArray* series )
{
#define LOCAL_USE_SP 1
    msvEntity* resultEntity = 0;
    resultEntity = msvEntity::New();
#if( LOCAL_USE_SP )
    vtkSmartPointer<vtkStructuredPointsReader> structuredPointsReaderSP = vtkSmartPointer<vtkStructuredPointsReader>::New();
#else
    vtkStructuredPointsReader* structuredPointsReader = 0;
#endif
    vtkStructuredPoints* dataObject = 0;
    for( vtkIdType indexFile = 0; indexFile < series->GetNumberOfValues(); indexFile++ )
    {
        vtkStdString& currentFileName = series->GetValue( indexFile );
#if( LOCAL_USE_SP )
        structuredPointsReaderSP->SetFileName( currentFileName );
        structuredPointsReaderSP->Update();
        dataObject = structuredPointsReaderSP->GetOutput();
        resultEntity->AddDataObject( dataObject );
#else
        structuredPointsReader = vtkStructuredPointsReader::New();

        structuredPointsReader->SetFileName( currentFileName );
        structuredPointsReader->Update();
        dataObject = structuredPointsReader->GetOutput();
        resultEntity->AddDataObject( dataObject );

        structuredPointsReader->Delete();
#endif
    }
#undef LOCAL_USE_SP 

    return resultEntity;
}

msvEntity* msvEntityMgr::CreateEntityFromSeries( vtkStringArray* series )
{
    msvEntity* resultEntity( 0 );
    msvEntityInfoEntry* resultEntityInfoEntry( 0 );
    resultEntityInfoEntry = CreateEntityInfoEntryFromSeries( series );
    if( resultEntityInfoEntry != 0 )
    {
        resultEntity = resultEntityInfoEntry->Entity;
    }

    return resultEntity;
}

msvEntityInfoEntry* msvEntityMgr::CreateEntityInfoEntryFromSeries( vtkStringArray* series )
{
    msvEntityInfoEntry* resultEntityInfoEntry( 0 );
    vtkSmartPointer<vtkMultipleDataReader> multipleDataReaderSP = vtkSmartPointer<vtkMultipleDataReader>::New();
    int dataObjectType = multipleDataReaderSP->GetSeriesDataObjectType( series );
    if( dataObjectType >= 0 )
    {
        switch( dataObjectType )
        {
            case VTK_POLY_DATA:
            {
                vtkSmartPointer<vtkMultiplePolyDataReader> multiplePolyDataReaderSP = vtkSmartPointer<vtkMultiplePolyDataReader>::New();
                multiplePolyDataReaderSP->SetFileNames( series );
                vtkSmartPointer<vtkTemporalDataSetTimeStepProvider> temporalDataSetTimeStepProviderSP = vtkSmartPointer<vtkTemporalDataSetTimeStepProvider>::New();
                temporalDataSetTimeStepProviderSP->SetInputConnection( multiplePolyDataReaderSP->GetOutputPort() );
                temporalDataSetTimeStepProviderSP->SetCacheSize( 3 );
                temporalDataSetTimeStepProviderSP->SetNextTimeStep( 0 );
                temporalDataSetTimeStepProviderSP->Update();
                vtkPolyData* polyData = vtkPolyData::SafeDownCast( temporalDataSetTimeStepProviderSP->GetOutput()->GetTimeStep( 0 ) );
                if( polyData )
                {
                    resultEntityInfoEntry = new msvEntityInfoEntry;
                    resultEntityInfoEntry->Entity = msvEntity::New();
                    resultEntityInfoEntry->Entity->AddDataObject( polyData );
                    resultEntityInfoEntry->multipleDataReader = multiplePolyDataReaderSP;
                    resultEntityInfoEntry->TSProvider = temporalDataSetTimeStepProviderSP;
                    m_EntityInfoEntries.push_back( resultEntityInfoEntry );
                }
                break;
            }
            case VTK_STRUCTURED_POINTS:
            {
                //resultEntity = CreateEntityFromSeriesOfStructuredPoints( series );
                vtkSmartPointer<vtkMultipleStructuredPointsReader> multipleStructuredPointsReaderSP = vtkSmartPointer<vtkMultipleStructuredPointsReader>::New();
                multipleStructuredPointsReaderSP->SetFileNames( series );
                vtkSmartPointer<vtkTemporalDataSetTimeStepProvider> temporalDataSetTimeStepProviderSP = vtkSmartPointer<vtkTemporalDataSetTimeStepProvider>::New();
                temporalDataSetTimeStepProviderSP->SetInputConnection( multipleStructuredPointsReaderSP->GetOutputPort() );
                temporalDataSetTimeStepProviderSP->SetCacheSize( 3 );
                temporalDataSetTimeStepProviderSP->SetNextTimeStep( 0 );
                temporalDataSetTimeStepProviderSP->Update();
                vtkStructuredPoints* structuredPoints = vtkStructuredPoints::SafeDownCast( temporalDataSetTimeStepProviderSP->GetOutput()->GetTimeStep( 0 ) );
                if( structuredPoints )
                {
                    resultEntityInfoEntry = new msvEntityInfoEntry;
                    resultEntityInfoEntry->Entity = msvEntity::New();
                    resultEntityInfoEntry->Entity->AddDataObject( structuredPoints );
                    resultEntityInfoEntry->multipleDataReader = multipleStructuredPointsReaderSP;
                    resultEntityInfoEntry->TSProvider = temporalDataSetTimeStepProviderSP;
                    m_EntityInfoEntries.push_back( resultEntityInfoEntry );
                }
                break;
            }
            case VTK_STRUCTURED_GRID:
            {
                break;
            }
            case VTK_RECTILINEAR_GRID:
            {
                break;
            }
            case VTK_UNSTRUCTURED_GRID:
            {
                break;
            }
        }
    }

    return resultEntityInfoEntry;
}

msvEntity* msvEntityMgr::CreateEntityFromDirectory( const string& DirectoryName, const string& FileWildcard )
{
    msvEntity* resultEntity = 0 ;
    vtkSmartPointer<vtkGlobFileNames> globFileNamesSP = vtkSmartPointer<vtkGlobFileNames>::New();
    globFileNamesSP->SetDirectory( DirectoryName.c_str() );
    globFileNamesSP->RecurseOff();
    if( globFileNamesSP->AddFileNames( FileWildcard.c_str() ) )
    {
        int a = globFileNamesSP->GetNumberOfFileNames();
        vtkStringArray* foundFiles = globFileNamesSP->GetFileNames();
        for( vtkIdType indexFile = 0; indexFile < foundFiles->GetNumberOfValues(); indexFile++ )
        {
            vtkStdString& currentFileName = foundFiles->GetValue( indexFile );
            cout << currentFileName << endl;
        }
        if( foundFiles->GetNumberOfValues() > 0 )
        {
            resultEntity = CreateEntityFromSeries( foundFiles );
        }
    }

    if( resultEntity != 0 )
    {
        // Maybe here should observe the delete event
        //m_Entities.push_back( resultEntity );
        // Assign the renderer if whe already have one
        if( m_AssignedRenderer != 0 )
        {
            resultEntity->SetRenderer( m_AssignedRenderer );
        }
    }

    return resultEntity;
}

void msvEntityMgr::SetRenderer( vtkRenderer* assignedRenderer )
{
    //m_AssignedRenderer->Register( this );
    m_AssignedRenderer = assignedRenderer;

    vector<msvEntityInfoEntry*>::iterator it;
    for( it = m_EntityInfoEntries.begin(); it != m_EntityInfoEntries.end(); it++ )
    {
        //(*it)->SetRenderer( assignedRenderer );
        (*it)->Entity->SetRenderer( assignedRenderer );
    }
}

//const vector<msvEntity*>& msvEntityMgr::GetEntitiesVector() const
//{
//    return m_Entities;
//}

void msvEntityMgr::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
}
