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
#include <vtkMutexLock.h>
#include <vtkMultiThreader.h>


#include "msvEntity.h"

#include <vector>
#include <list>
#include <exception>

using std::string;
using std::vector;
using std::list;
using std::exception;

struct msvEntityInfoEntry
{
    vtkMutexLock* AccessLock;
    msvEntity* Entity;
    int dataObjectType;
    vtkSmartPointer<vtkMultipleDataReader> multipleDataReader;
    vtkSmartPointer<vtkTemporalDataSetTimeStepProvider> TSProvider;
    vtkSmartPointer<vtkMultiThreader> multiThreader;
    int firstTimeStepToLoad;
    int timeStepsToLoad;
    bool timeStepsLoaded;
    bool timeStepAssigned;
};

typedef struct
{
    vtkMutexLock* Lock;
    msvEntityMgrImpl* entityMgr;
    msvEntityInfoEntry* entityInfoEntry;
    int firstFrameToLoad;
    int framesToLoad;
    int threadID;
} msvEntityMgrThreadData;

#if 0
#define FLUSHED_MESSAGE( m ) cout << vtkMultiThreader::GetCurrentThreadID() << ": " << m << endl; cout.flush();
#define FLUSHED_MESSAGE2( threadID, m ) cout << vtkMultiThreader::GetCurrentThreadID() << "(" << threadID << "): " << m << endl; cout.flush();
#else
#define FLUSHED_MESSAGE( m )
#define FLUSHED_MESSAGE2( threadID, m )
#endif
////////////////////////////
// Private Implementor class
////////////////////////////

class msvEntityMgrImpl : public vtkObject
{
public:
    // Description:
    // Constructor and destructor
    msvEntityMgrImpl( msvEntityMgr* publicInterface );
    ~msvEntityMgrImpl();

    msvEntity* CreateEntityFromDirectory( const std::string& DirectoryName, const std::string& FileWildcard = "*" );

    void SetRenderer( vtkRenderer* assignedRenderer );

    virtual void PrintSelf( ostream& os, vtkIndent indent );

    //const std::vector<msvEntity*>& GetEntitiesVector() const;
    // Updates having into account the elapsed time, in microseconds
    void Tick( long elapsedTime );

    // Fills a petition to provide new time steps to the entity that asks for it
    void RequestSequentialTimeSteps( msvEntity* requesterEntity, int firstTimeStepRequested, int timeStepsRequested );
    void RequestNextSequentialTimeStep( msvEntity* requesterEntity );

    //
    void TerminatLoadFrameThread( msvEntityMgrThreadData* threadData );

    void QueueFulfilledRequest( msvEntityMgrThreadData* threadData, int threadID = -1 );
    void LoadTimeSteps( msvEntityInfoEntry* entityInfoEntry, int firstFrameToLoad, int framesToLoad, bool fromMainThread = true, int threadID = -1 );

private:
    friend class msvEntityMgr;

    msvEntity* CreateEntityFromSeriesOfStructuredPoints( vtkStringArray* series );
    msvEntity* CreateEntityFromSeries( vtkStringArray* series );
    msvEntityInfoEntry* CreateEntityInfoEntryFromSeries( vtkStringArray* series );

    // Finds and gets an EntityInfoEntry (or null) from EntityInfoEntries
    msvEntityInfoEntry* GetEntityInfoEntry( msvEntity* holdedEntity );

    // Sets the correct Current Time Step for the data entity
    void SetEntityCurrentTimeStep( msvEntityInfoEntry* entityInfoEntry, int currentTimeStep, int cachedTimeStep );

    // Launches a thread to load frames from the data source into the entity
    void PreloadTimeSteps( msvEntityInfoEntry* entityInfoEntry, int firstFrameToPreload, int framesToPreload );

    void RequestSequentialTimeSteps( msvEntityInfoEntry* entityInfoEntry );
    void FulfillTimeStepRequests();

    bool GetEntityNewDataAvailable();
    void SetEntityNewDataAvailable( bool available = true );

private:

    msvEntityMgr*                       PublicInterface;
    vtkMutexLock*                       TimeStepRequestQueueLock;
    vtkMutexLock*                       FulfilledTimeStepRequestsLock;
    vtkMutexLock*                       AlgorithmLock;
    vtkRenderer*                        AssignedRenderer;
    int                                 NewEntityPreloadTimeSteps;
    //std::vector<msvEntity*>             Entities;
    vector<msvEntityInfoEntry*>         EntityInfoEntries;
    list<msvEntityInfoEntry*>           TimeStepRequestQueue;
    list<msvEntityMgrThreadData*>       FulfilledTimeStepRequests;
    bool                                EntityNewDataAvailable;
};

msvEntityMgrImpl::msvEntityMgrImpl( msvEntityMgr* publicInterface )
: PublicInterface( publicInterface )
, TimeStepRequestQueueLock( vtkMutexLock::New() )
, FulfilledTimeStepRequestsLock( vtkMutexLock::New() )
, AlgorithmLock( vtkMutexLock::New() )
, AssignedRenderer( 0 )
//, NewEntityPreloadTimeSteps( 1 )
, NewEntityPreloadTimeSteps( 3 )
//, NewEntityPreloadTimeSteps( 34 )
, EntityNewDataAvailable( false )
{
    //this->TimeStepRequestQueueLock->Register( this );
    //this->AlgorithmLock->Register( this );
    //this->TimeStepRequestQueueLock->DebugOn();
    //this->AlgorithmLock->DebugOn();
}

msvEntityMgrImpl::~msvEntityMgrImpl()
{
    this->FulfilledTimeStepRequestsLock->Lock();
    try
    {
        FLUSHED_MESSAGE( "  Locked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::~msvEntityMgrImpl" );
        if( this->FulfilledTimeStepRequests.size() > 0 )
        {
            // We are consuming, then lock
            list<msvEntityMgrThreadData*>::iterator it;
            for( it = this->FulfilledTimeStepRequests.begin(); it != this->FulfilledTimeStepRequests.end(); it++ )
            {
                msvEntityMgrThreadData* threadData = *it; 
                msvEntityInfoEntry* entityInfoEntry = threadData->entityInfoEntry;

                entityInfoEntry->AccessLock->Lock();
                try
                {
                    //CRASH when active breakpoint
                    if( entityInfoEntry->multiThreader->IsThreadActive( threadData->threadID ) )
                    {
                        FLUSHED_MESSAGE( "Going to terminate thread in msvEntityMgrImpl::~msvEntityMgrImpl" );
                        entityInfoEntry->multiThreader->TerminateThread( threadData->threadID );
                        FLUSHED_MESSAGE( "Thread terminated in msvEntityMgrImpl::~msvEntityMgrImpl" );
                    }
                    entityInfoEntry->AccessLock->Unlock();
                }
                catch( exception& )
                {
                    FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::~msvEntityMgrImpl(LOOP)" );
                    entityInfoEntry->AccessLock->Unlock();
                }

                delete threadData;
            }
            this->FulfilledTimeStepRequests.clear();
        }
        this->FulfilledTimeStepRequestsLock->Unlock();
        FLUSHED_MESSAGE( "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::~msvEntityMgrImpl" );
    }
    catch( exception e )
    {
        FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::~msvEntityMgrImpl" );
        this->FulfilledTimeStepRequestsLock->Unlock();
        FLUSHED_MESSAGE( "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::~msvEntityMgrImpl(CATCH)" );
    }

    vector<msvEntityInfoEntry*>::size_type index;
    for( index = 0; index < this->EntityInfoEntries.size(); index++ )
    {
        this->EntityInfoEntries[index]->AccessLock->Lock();
        try
        {
            this->EntityInfoEntries[index]->Entity->Delete();
            this->EntityInfoEntries[index]->Entity = 0;
            //this->EntityInfoEntries[index]->multipleDataReader->Delete();
            //this->EntityInfoEntries[index]->multipleDataReader = 0;
            //this->EntityInfoEntries[index]->TSProvider->Delete();
            //this->EntityInfoEntries[index]->TSProvider = 0;
            this->EntityInfoEntries[index]->AccessLock->Unlock();
        }
        catch( exception e )
        {
            this->EntityInfoEntries[index]->AccessLock->Unlock();
        }
        this->EntityInfoEntries[index]->AccessLock->Delete();
        delete this->EntityInfoEntries[index];
    }

    if( this->TimeStepRequestQueueLock != 0 )
    {
        this->TimeStepRequestQueueLock->Delete();
    }
    if( this->FulfilledTimeStepRequestsLock != 0 )
    {
        this->FulfilledTimeStepRequestsLock->Delete();
    }
    if( this->AlgorithmLock != 0 )
    {
        this->AlgorithmLock->Delete();
    }
}

msvEntity* msvEntityMgrImpl::CreateEntityFromSeriesOfStructuredPoints( vtkStringArray* series )
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

msvEntity* msvEntityMgrImpl::CreateEntityFromSeries( vtkStringArray* series )
{
    msvEntity* resultEntity( 0 );
    msvEntityInfoEntry* resultEntityInfoEntry( 0 );
    resultEntityInfoEntry = CreateEntityInfoEntryFromSeries( series );
    if( resultEntityInfoEntry != 0 )
    {
        resultEntity = resultEntityInfoEntry->Entity;
        // Assign the entity manager so the entity can ask for new frames
        resultEntity->SetEntityMgr( this->PublicInterface );
        resultEntity->SetRenderer( this->AssignedRenderer );
    }
    PreloadTimeSteps( resultEntityInfoEntry, 0, this->NewEntityPreloadTimeSteps );

    return resultEntity;
}

msvEntityInfoEntry* msvEntityMgrImpl::CreateEntityInfoEntryFromSeries( vtkStringArray* series )
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
                vtkSmartPointer<vtkTemporalDataSetTimeStepProvider> temporalDataSetTimeStepProviderSP = vtkSmartPointer<vtkTemporalDataSetTimeStepProvider>::New();
                multiplePolyDataReaderSP->SetFileNames( series );
                multiplePolyDataReaderSP->TimeStepWrapOn();
                temporalDataSetTimeStepProviderSP->SetInputConnection( multiplePolyDataReaderSP->GetOutputPort() );
                temporalDataSetTimeStepProviderSP->TimeStepWrapOn();

                resultEntityInfoEntry = new msvEntityInfoEntry;
                resultEntityInfoEntry->Entity = msvEntity::New();
                resultEntityInfoEntry->AccessLock = vtkMutexLock::New();
                resultEntityInfoEntry->multipleDataReader = multiplePolyDataReaderSP;
                resultEntityInfoEntry->TSProvider = temporalDataSetTimeStepProviderSP;
                resultEntityInfoEntry->multiThreader = vtkSmartPointer<vtkMultiThreader>::New();
                // Initially there was not time step request, so the condition is set to true to be able to launch a request
                resultEntityInfoEntry->timeStepsLoaded = true;
                resultEntityInfoEntry->timeStepAssigned = true;
                this->EntityInfoEntries.push_back( resultEntityInfoEntry );
                break;
            }
            case VTK_STRUCTURED_POINTS:
            {
                vtkSmartPointer<vtkMultipleStructuredPointsReader> multipleStructuredPointsReaderSP = vtkSmartPointer<vtkMultipleStructuredPointsReader>::New();
                multipleStructuredPointsReaderSP->SetFileNames( series );
                multipleStructuredPointsReaderSP->TimeStepWrapOn();
                vtkSmartPointer<vtkTemporalDataSetTimeStepProvider> temporalDataSetTimeStepProviderSP = vtkSmartPointer<vtkTemporalDataSetTimeStepProvider>::New();
                temporalDataSetTimeStepProviderSP->SetInputConnection( multipleStructuredPointsReaderSP->GetOutputPort() );

                resultEntityInfoEntry = new msvEntityInfoEntry;
                resultEntityInfoEntry->Entity = msvEntity::New();
                resultEntityInfoEntry->AccessLock = vtkMutexLock::New();
                resultEntityInfoEntry->multipleDataReader = multipleStructuredPointsReaderSP;
                resultEntityInfoEntry->TSProvider = temporalDataSetTimeStepProviderSP;
                resultEntityInfoEntry->multiThreader = vtkSmartPointer<vtkMultiThreader>::New();
                // Initially there was not time step request, so the condition is set to true to be able to launch a request
                resultEntityInfoEntry->timeStepsLoaded = true;
                resultEntityInfoEntry->timeStepAssigned = true;
                this->EntityInfoEntries.push_back( resultEntityInfoEntry );
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
        if( resultEntityInfoEntry != 0 )
        {
            resultEntityInfoEntry->dataObjectType = dataObjectType;
        }
    }

    return resultEntityInfoEntry;
}

msvEntity* msvEntityMgrImpl::CreateEntityFromDirectory( const string& DirectoryName, const string& FileWildcard )
{
    msvEntity* resultEntity = 0 ;
    vtkSmartPointer<vtkGlobFileNames> globFileNamesSP = vtkSmartPointer<vtkGlobFileNames>::New();
    globFileNamesSP->SetDirectory( DirectoryName.c_str() );
    globFileNamesSP->RecurseOff();
    if( globFileNamesSP->AddFileNames( FileWildcard.c_str() ) )
    {
        int a = globFileNamesSP->GetNumberOfFileNames();
        vtkStringArray* foundFiles = globFileNamesSP->GetFileNames();
#if 0
        for( vtkIdType indexFile = 0; indexFile < foundFiles->GetNumberOfValues(); indexFile++ )
        {
            vtkStdString& currentFileName = foundFiles->GetValue( indexFile );
            cout << currentFileName << endl;
        }
#endif
        if( foundFiles->GetNumberOfValues() > 0 )
        {
            resultEntity = CreateEntityFromSeries( foundFiles );
        }
    }

    if( resultEntity != 0 )
    {
        // Maybe here should observe the delete event
        //m_Entities.push_back( resultEntity );
        // Assign the renderer if we already have one
        if( this->AssignedRenderer != 0 )
        {
            resultEntity->SetRenderer( this->AssignedRenderer );
        }
    }

    return resultEntity;
}

void msvEntityMgrImpl::SetRenderer( vtkRenderer* assignedRenderer )
{
     this->AssignedRenderer = assignedRenderer;

    vector<msvEntityInfoEntry*>::iterator it;
    for( it = this->EntityInfoEntries.begin(); it != this->EntityInfoEntries.end(); it++ )
    {
        (*it)->Entity->SetRenderer( this->AssignedRenderer );
    }
}

void msvEntityMgrImpl::SetEntityCurrentTimeStep( msvEntityInfoEntry* entityInfoEntry, int currentTimeStep, int cachedTimeStep )
{
    // entityInfoEntry->AccessLock->Lock();
    // this->AlgorithmLock->Lock();
    // supposed to be done before calling...

    int nextCachedTimeStep = cachedTimeStep;

    switch( entityInfoEntry->dataObjectType )
    {
        case VTK_POLY_DATA:
        {
            vtkPolyData* polyData = vtkPolyData::SafeDownCast( entityInfoEntry->TSProvider->GetOutput()->GetTimeStep( nextCachedTimeStep ) );
            if( polyData )
            {
                entityInfoEntry->Entity->SetCurrentTimeStepData( polyData, currentTimeStep );
            }
            break;
        }
        case VTK_STRUCTURED_POINTS:
        {
            vtkStructuredPoints* structuredPoints = vtkStructuredPoints::SafeDownCast( entityInfoEntry->TSProvider->GetOutput()->GetTimeStep( 0 ) );
            if( structuredPoints )
            {
                entityInfoEntry->Entity->SetCurrentTimeStepData( structuredPoints );
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

msvEntityInfoEntry* msvEntityMgrImpl::GetEntityInfoEntry( msvEntity* holdedEntity )
{
    msvEntityInfoEntry* entityInfoEntry( 0 );
    vector<msvEntityInfoEntry*>::iterator it;
    for( it = this->EntityInfoEntries.begin(); it != this->EntityInfoEntries.end(); it++ )
    {
        if( (*it)->Entity == holdedEntity )
        {
            entityInfoEntry = *it;
            break;
        }
    }

    return entityInfoEntry;
}

void msvEntityMgrImpl::RequestSequentialTimeSteps( msvEntityInfoEntry* entityInfoEntry )
{
    // Put the request on the Queue
    this->TimeStepRequestQueueLock->Lock();
    FLUSHED_MESSAGE( "  Locked TimeStepRequestQueueLock in msvEntityMgrImpl::RequestSequentialTimeSteps" );
    this->TimeStepRequestQueue.push_back( entityInfoEntry );
    this->TimeStepRequestQueueLock->Unlock();
    FLUSHED_MESSAGE( "UNLocked TimeStepRequestQueueLock in msvEntityMgrImpl::RequestSequentialTimeSteps" );
}

void msvEntityMgrImpl::RequestSequentialTimeSteps( msvEntity* requesterEntity, int firstTimeStepRequested, int timeStepsRequested )
{
    msvEntityInfoEntry* entityInfoEntry( GetEntityInfoEntry( requesterEntity ) );
    // We dont want to asign the time steps requested before the requester thread has updated that data
    // and released the lock
    entityInfoEntry->AccessLock->Lock();
    try
    {
        if( entityInfoEntry != 0 )
        {
            entityInfoEntry->firstTimeStepToLoad = firstTimeStepRequested;
            entityInfoEntry->timeStepsToLoad = timeStepsRequested;

            RequestSequentialTimeSteps( entityInfoEntry );
        }
        entityInfoEntry->AccessLock->Unlock();
    }
    catch( exception e )
    {
        FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::RequestSequentialTimeSteps" );
        entityInfoEntry->AccessLock->Unlock();
    }
}

void msvEntityMgrImpl::RequestNextSequentialTimeStep( msvEntity* requesterEntity )
{
    cout << "RequestNextSequentialTimeStep" << endl;
    msvEntityInfoEntry* entityInfoEntry( GetEntityInfoEntry( requesterEntity ) );
    if( entityInfoEntry != 0 )
    {
        int availableTimeSteps = 34;
        int chacheSize;
        entityInfoEntry->AccessLock->Lock();
        try
        {
            this->AlgorithmLock->Lock();
            try
            {
                FLUSHED_MESSAGE( "  Locked AlgorithmLock in msvEntityMgrImpl::RequestNextSequentialTimeStep" );
                availableTimeSteps = entityInfoEntry->multipleDataReader->GetAvailableTimeSteps();
                chacheSize = entityInfoEntry->TSProvider->GetCacheSize();
                int nextTimeStep = entityInfoEntry->TSProvider->GetNextTimeStep();
                this->AlgorithmLock->Unlock();
                FLUSHED_MESSAGE( "UNLocked AlgorithmLock in msvEntityMgrImpl::RequestNextSequentialTimeStep" );
            }
            catch( exception e )
            {
                FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::RequestNextSequentialTimeStep" );
                this->AlgorithmLock->Unlock();
                FLUSHED_MESSAGE( "UNLocked AlgorithmLock in msvEntityMgrImpl::RequestNextSequentialTimeStep(CATCH)" );
            }

            entityInfoEntry->firstTimeStepToLoad++;
            if( entityInfoEntry->firstTimeStepToLoad >= availableTimeSteps )
            {
                entityInfoEntry->firstTimeStepToLoad = 0;
            }
            entityInfoEntry->AccessLock->Unlock();
        }
        catch( exception e )
        {
            FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::RequestNextSequentialTimeStep" );
            entityInfoEntry->AccessLock->Unlock();
        }
        RequestSequentialTimeSteps( entityInfoEntry );
    }
}

void msvEntityMgrImpl::TerminatLoadFrameThread( msvEntityMgrThreadData* threadData )
{
    threadData->entityInfoEntry->multiThreader->TerminateThread( threadData->threadID );
    delete threadData;
}

void msvEntityMgrImpl::QueueFulfilledRequest( msvEntityMgrThreadData* threadData, int threadID )
{
    // We are producing, then lock
    this->FulfilledTimeStepRequestsLock->Lock();
    try
    {
        FLUSHED_MESSAGE2( threadID, "  Locked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::QueueFulfilledRequest" );
        this->FulfilledTimeStepRequests.push_back( threadData );
        this->FulfilledTimeStepRequestsLock->Unlock();
        FLUSHED_MESSAGE2( threadID, "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::QueueFulfilledRequest" );
    }
    catch( exception e )
    {
        FLUSHED_MESSAGE2( threadID, "Exception occurred in msvEntityMgrImpl::QueueFulfilledRequest" );
        this->FulfilledTimeStepRequestsLock->Unlock();
        FLUSHED_MESSAGE2( threadID, "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::QueueFulfilledRequest" );
    }
}

void msvEntityMgrImpl::LoadTimeSteps( msvEntityInfoEntry* entityInfoEntry, int firstFrameToLoad, int framesToLoad, bool fromMainThread, int threadID )
{
    entityInfoEntry->AccessLock->Lock();
    try
    {
        firstFrameToLoad = entityInfoEntry->firstTimeStepToLoad;
        framesToLoad = entityInfoEntry->timeStepsToLoad;

        this->AlgorithmLock->Lock();
        try
        {
            FLUSHED_MESSAGE2( threadID, "  Locked AlgorithmLock in msvEntityMgrImpl::LoadTimeSteps" );
            int nextTimeStep = entityInfoEntry->TSProvider->GetNextTimeStep();
            entityInfoEntry->TSProvider->SetNextTimeStep( firstFrameToLoad );
            entityInfoEntry->TSProvider->SetCacheSize( framesToLoad );
        
            entityInfoEntry->TSProvider->Update();
            this->AlgorithmLock->Unlock();
            FLUSHED_MESSAGE2( threadID, "UNLocked AlgorithmLock in msvEntityMgrImpl::LoadTimeSteps" );
        }
        catch( exception e )
        {
            FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::LoadTimeSteps" );
            this->AlgorithmLock->Unlock();
            FLUSHED_MESSAGE2( threadID, "UNLocked AlgorithmLock in msvEntityMgrImpl::LoadTimeSteps(CATCH)" );
        }
        entityInfoEntry->timeStepsLoaded = true;
        bool hasAnyTimeStep = entityInfoEntry->Entity->HasAnyTimeStep();
        // Unlock as soon as possible
        entityInfoEntry->AccessLock->Unlock();

        if( !fromMainThread && !hasAnyTimeStep )
        {
            this->SetEntityNewDataAvailable();
        }
    }
    catch( exception e )
    {
        FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::LoadTimeSteps" );
        entityInfoEntry->AccessLock->Unlock();
    }
}

VTK_THREAD_RETURN_TYPE TimeStepsLoaderThread( void* arg )
{
    vtkMultiThreader::ThreadInfo* threadInfo = static_cast<vtkMultiThreader::ThreadInfo*>( arg );
    int threadID = threadInfo->ThreadID;
    msvEntityMgrThreadData* threadData = static_cast<msvEntityMgrThreadData*>( threadInfo->UserData );

    msvEntityMgrImpl* entityMgr = threadData->entityMgr;

    msvEntityInfoEntry* entityInfoEntry = threadData->entityInfoEntry;

    // We set the last parameter to false, because we are not calling from main thread
    entityMgr->LoadTimeSteps( entityInfoEntry, threadData->firstFrameToLoad, threadData->framesToLoad, false, threadID );

    entityMgr->QueueFulfilledRequest( threadData, threadID );

    // We don't need the user data anymore
    //delete threadData;
    //threadInfo->UserData = NULL;

    return VTK_THREAD_RETURN_VALUE;
}

void msvEntityMgrImpl::PreloadTimeSteps( msvEntityInfoEntry* entityInfoEntry, int firstTimeStepToPreload, int timeStepsToPreload )
{
    this->RequestSequentialTimeSteps( entityInfoEntry->Entity, firstTimeStepToPreload, timeStepsToPreload );
    this->FulfillTimeStepRequests();
}

void msvEntityMgrImpl::FulfillTimeStepRequests()
{
    // Launches a thread for every request
    this->TimeStepRequestQueueLock->Lock();
    try
    {
        FLUSHED_MESSAGE( "  Locked TimeStepRequestQueueLock in msvEntityMgrImpl::FulfillTimeStepRequests" );
        int requestsLoadedButNotAssigned = 0;
        while( ( this->TimeStepRequestQueue.size() > 0 ) && requestsLoadedButNotAssigned != this->TimeStepRequestQueue.size() ) 
        {
            bool requestLaunched = false;
            list<msvEntityInfoEntry*>::iterator it;
            for( it = this->TimeStepRequestQueue.begin(); it != this->TimeStepRequestQueue.end(); it++ )
            {
                msvEntityInfoEntry* entityInfoEntry = *it;

                int threadID;
                entityInfoEntry->AccessLock->Lock();
                try
                {
                    // The previous requested tim esteps where loaded, so we can launch a new one
                    // We have to be sure that the time step has been asigned also
                    if( entityInfoEntry->timeStepsLoaded )
                    {
                        if( entityInfoEntry->timeStepAssigned )
                        {
                            entityInfoEntry->timeStepsLoaded = false;
                            msvEntityMgrThreadData* threadData = new msvEntityMgrThreadData;
                            threadData->entityMgr = this;
                            threadData->entityInfoEntry = entityInfoEntry;
                            threadData->firstFrameToLoad = entityInfoEntry->firstTimeStepToLoad;
                            threadData->framesToLoad = 1;
                            vtkMultiThreader* multiThreader = entityInfoEntry->multiThreader;

                            FLUSHED_MESSAGE( "Going to swpawn thread in msvEntityMgrImpl::FulfillTimeStepRequests" );
                            threadID = multiThreader->SpawnThread( TimeStepsLoaderThread, threadData );
                            FLUSHED_MESSAGE( "Thread spawned in msvEntityMgrImpl::FulfillTimeStepRequests" );
                            threadData->threadID = threadID;
                            requestLaunched = true;
                            entityInfoEntry->AccessLock->Unlock();
                            break;
                        }
                        else
                        {
                            requestsLoadedButNotAssigned++;
                        }
                    }
                    entityInfoEntry->AccessLock->Unlock();
                }
                catch( exception& )
                {
                    FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::FulfillTimeStepRequests" );
                    entityInfoEntry->AccessLock->Unlock();
                }
            }
            if( requestLaunched )
            {
                this->TimeStepRequestQueue.erase( it );
            }
            //this->TimeStepRequestQueue.clear();
        }
        this->TimeStepRequestQueueLock->Unlock();
        FLUSHED_MESSAGE( "UNLocked TimeStepRequestQueueLock in msvEntityMgrImpl::FulfillTimeStepRequests" );
    }
    catch( exception e )
    {
        FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::FulfillTimeStepRequests" );
        this->TimeStepRequestQueueLock->Unlock();
        FLUSHED_MESSAGE( "UNLocked TimeStepRequestQueueLock in msvEntityMgrImpl::FulfillTimeStepRequests(CATCH)" );
    }
}

bool msvEntityMgrImpl::GetEntityNewDataAvailable()
{
    bool available;

    this->FulfilledTimeStepRequestsLock->Lock();
    FLUSHED_MESSAGE( "  Locked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::GetEntityNewDataAvailable" );
    available = this->EntityNewDataAvailable;
    this->FulfilledTimeStepRequestsLock->Unlock();
    FLUSHED_MESSAGE( "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::GetEntityNewDataAvailable" );

    return available;
}

void msvEntityMgrImpl::SetEntityNewDataAvailable( bool available )
{
    this->FulfilledTimeStepRequestsLock->Lock();
    FLUSHED_MESSAGE( "  Locked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::SetEntityNewDataAvailable" );
    this->EntityNewDataAvailable = available;
    this->FulfilledTimeStepRequestsLock->Unlock();
    FLUSHED_MESSAGE( "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::SetEntityNewDataAvailable" );
}

void msvEntityMgrImpl::Tick( long elapsedTime )
{
    this->FulfilledTimeStepRequestsLock->Lock();
    try
    {
        FLUSHED_MESSAGE( "  Locked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::Tick" );
        if( this->FulfilledTimeStepRequests.size() > 0 )
        {
            // We are consuming, then lock
            list<msvEntityMgrThreadData*>::iterator it;
            for( it = this->FulfilledTimeStepRequests.begin(); it != this->FulfilledTimeStepRequests.end(); it++ )
            {
                msvEntityMgrThreadData* threadData = *it; 
                msvEntityInfoEntry* entityInfoEntry = threadData->entityInfoEntry;
                entityInfoEntry->AccessLock->Lock();
                try
                {
                    bool frameSkip = false;
                    int nextTimeStep;
                    int cacheSize;
                    int effectiveTimeStep( 0 );
                    this->AlgorithmLock->Lock();
                    try
                    {
                        nextTimeStep = entityInfoEntry->TSProvider->GetNextTimeStep();
                        cacheSize = entityInfoEntry->TSProvider->GetCacheSize();
                        int nextCachedTimeStep = 0;
                        if( threadData->firstFrameToLoad < nextTimeStep )
                        {
                            // FrameSkip happened
                            cout << "Frame Skip!!" << endl;
                            frameSkip = true;
                            effectiveTimeStep = nextTimeStep;
                            nextCachedTimeStep;
                        }
                        else
                        {
                            effectiveTimeStep = threadData->firstFrameToLoad;
                        }
                        // Correct the frame step that was loaded
                        entityInfoEntry->firstTimeStepToLoad = effectiveTimeStep;
                        SetEntityCurrentTimeStep( entityInfoEntry, effectiveTimeStep, nextCachedTimeStep );
                        entityInfoEntry->timeStepAssigned = true;
                        this->AlgorithmLock->Unlock();
                    }
                    catch( exception e )
                    {
                        this->AlgorithmLock->Unlock();
                    }

                    //entityInfoEntry->timeStepsAssigned = true;
                    //CRASH when active breakpoint
                    if( entityInfoEntry->multiThreader->IsThreadActive( threadData->threadID ) )
                    {
                        FLUSHED_MESSAGE( "Going to terminate thread in msvEntityMgrImpl::Tick" );
                        entityInfoEntry->multiThreader->TerminateThread( threadData->threadID );
                        FLUSHED_MESSAGE( "Thread terminated in msvEntityMgrImpl::Tick" );
                    }
                    entityInfoEntry->AccessLock->Unlock();
                }
                catch( exception& )
                {
                    FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::Tick(LOOP)" );
                    entityInfoEntry->AccessLock->Unlock();
                }
                
                delete threadData;
            }
            this->FulfilledTimeStepRequests.clear();
        }
        this->FulfilledTimeStepRequestsLock->Unlock();
        FLUSHED_MESSAGE( "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::Tick" );
    }
    catch( exception e )
    {
        FLUSHED_MESSAGE( "Exception occurred in msvEntityMgrImpl::Tick" );
        this->FulfilledTimeStepRequestsLock->Unlock();
        FLUSHED_MESSAGE( "Unlocked FulfilledTimeStepRequestsLock in msvEntityMgrImpl::Tick(CATCH)" );
    }

    if( GetEntityNewDataAvailable() )
    {
        this->AssignedRenderer->ResetCamera();
        SetEntityNewDataAvailable( false );
    }

    vector<msvEntityInfoEntry*>::iterator it;
    for( it = this->EntityInfoEntries.begin(); it != this->EntityInfoEntries.end(); it++ )
    {
        (*it)->Entity->Tick( elapsedTime );
    }

    // Fulfill pending Requests
    FulfillTimeStepRequests();
}

void msvEntityMgrImpl::PrintSelf( ostream& os, vtkIndent indent )
{
}

/////////////////////////
// Public interface class
/////////////////////////

vtkStandardNewMacro( msvEntityMgr );

msvEntityMgr::msvEntityMgr()
{
    this->Impl = new msvEntityMgrImpl( this );
}

msvEntityMgr::~msvEntityMgr()
{
    this->Impl->Delete();
}

msvEntity* msvEntityMgr::CreateEntityFromDirectory( const string& DirectoryName, const string& FileWildcard )
{
    return this->Impl->CreateEntityFromDirectory( DirectoryName, FileWildcard );
}

void msvEntityMgr::SetRenderer( vtkRenderer* assignedRenderer )
{
    this->Impl->SetRenderer( assignedRenderer );
}

void msvEntityMgr::Tick( long elapsedTime )
{
    this->Impl->Tick( elapsedTime );
}

void msvEntityMgr::RequestSequentialTimeSteps( msvEntity* requesterEntity, int firstTimeStepRequested, int timeStepsRequested )
{
    this->Impl->RequestSequentialTimeSteps( requesterEntity, firstTimeStepRequested, timeStepsRequested );
}

void msvEntityMgr::RequestNextSequentialTimeStep( msvEntity* requesterEntity )
{
    this->Impl->RequestNextSequentialTimeStep( requesterEntity );
}

void msvEntityMgr::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Impl->PrintSelf( os, indent );
}

