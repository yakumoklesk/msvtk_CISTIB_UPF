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

#include "msvEntity.h"

#include "msvEntityMgr.h"

#include <vtkObjectFactory.h>
#include <vtkStructuredPoints.h>
#include <vtkPolyData.h>
//#include <vtkRenderer.h>
#include <vtkThreadSafeRenderer.h>
#include <vtkActor.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkTimerLog.h>


#include <vector>

//using namespace std;
using std::numeric_limits;

////////////////////////////
// Private Implementor class
////////////////////////////

class msvEntityImpl
{
public:
    // Description:
    // Construct object
    msvEntityImpl( msvEntity* publicInterface );
    ~msvEntityImpl();

    // Sets the Entity Manager
    void SetEntityMgr( msvEntityMgr* entityMgr );

    // Add a frame
    void AddDataObject( vtkStructuredPoints* dataObject );
    void AddDataObject( vtkPolyData* dataObject );

    // Set current timestep
    void SetCurrentTimeStepData( vtkStructuredPoints* dataObject, int timeStepNumber = numeric_limits<int>::min() );
    void SetCurrentTimeStepData( vtkPolyData* dataObject, int timeStepNumber = numeric_limits<int>::min() );

    // Returns if has any frame
    bool HasAnyTimeStep();

    // Set the renderer
    void SetRenderer( vtkThreadSafeRenderer* assignedRenderer );
    void SetRenderer( vtkRenderer* assignedRenderer );

    // Set the provider of time steps
    void SetTimeStepProvider( vtkTemporalDataSetTimeStepProvider* timeStepProvider );

    // Updates having into account the elapsed time, in microseconds
    void Tick( long elapsedTime );

    // Description:
    // Get the bounds for this Prop3D as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
    virtual double* GetBounds() { return 0; };
    virtual void PrintSelf( ostream& os, vtkIndent indent );

private:
    void DeleteTimeStepData();
    void ReplaceCurrentTimeStepData( vtkDataSet* newTimeStepData, vtkProp* newTimeStepProp,
        vtkAbstractMapper3D* newtTimeStepMapper, vtkVolumeProperty* newTimeStepVolumeProperty );
    void AddViewPropToRenderer( vtkProp* prop );
    void RemoveViewPropToRenderer( vtkProp* prop );

private:

    msvEntity*                          PublicInterface;
    vtkRenderer*                        AssignedRenderer;
    vtkDataSet*                         CurrentTimeStepData;
    vtkProp*                            CurrentTimeStepProp;
    vtkAbstractMapper3D*                CurrentTimeStepMapper;
    vtkVolumeProperty*                  CurrentTimeStepVolumeProperty;

    bool                                FirstTimeShown;
    int                                 CurrentTimeStep;
    long                                AcumElapsedTime;
    msvEntityMgr*                       EntityMgr;
    vtkTemporalDataSetTimeStepProvider* TimeStepProvider;

    /*
    vector<vtkDataSet*>            CurrentTimeStepData;
    vector<vtkProp*>               CurrentTimeStepProp;
    vector<vtkAbstractMapper3D*>   Mappers;
    vector<vtkVolumeProperty*>     VolumeProperties;
    */
};


msvEntityImpl::msvEntityImpl( msvEntity* publicInterface )
: PublicInterface( publicInterface )
, AssignedRenderer( 0 )
, CurrentTimeStepData( 0 )
, CurrentTimeStepProp( 0 )
, CurrentTimeStepMapper( 0 )
, CurrentTimeStepVolumeProperty( 0 )
, FirstTimeShown( false )
, CurrentTimeStep( 0 )
, AcumElapsedTime( 0 )
, EntityMgr( 0 )
, TimeStepProvider( 0 )
{

}

msvEntityImpl::~msvEntityImpl()
{
    // First test if we have a renderer, and if so, if we have some
    // volume or actor added to it
    if( this->AssignedRenderer && this->CurrentTimeStepData )
    {
        if( this->AssignedRenderer->HasViewProp( this->CurrentTimeStepProp ) )
        {
            this->RemoveViewPropToRenderer( this->CurrentTimeStepProp );
        }
    }
    DeleteTimeStepData();
}

void msvEntityImpl::DeleteTimeStepData()
{
    if( CurrentTimeStepData != 0 )
    {
        CurrentTimeStepData->Delete();
    }
    if( CurrentTimeStepProp != 0 )
    {
        CurrentTimeStepProp->Delete();
    }
    if( CurrentTimeStepMapper != 0 )
    {
        CurrentTimeStepMapper->Delete();
    }
    if( CurrentTimeStepVolumeProperty != 0 )
    {
        CurrentTimeStepVolumeProperty->Delete();
    }
}


void msvEntityImpl::SetEntityMgr( msvEntityMgr* entityMgr )
{
    this->EntityMgr = entityMgr;
}

void msvEntityImpl::AddDataObject( vtkStructuredPoints* dataObject )
{
    vtkStructuredPoints* dataObjectCopy = vtkStructuredPoints::New();
    dataObjectCopy->DeepCopy( dataObject );
    vtkVolumeMapper* volumeMapper = vtkGPUVolumeRayCastMapper::New();
    volumeMapper->SetInput( dataObject );
    vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
    vtkVolume* volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( volumeProperty );

/*
    this->VolumeProperties.push_back( volumeProperty );
    this->Frames.push_back( dataObjectCopy );
    this->Mappers.push_back( volumeMapper );
    this->Props.push_back( volume );
*/
    //this->Modified();
}

void msvEntityImpl::AddDataObject( vtkPolyData* dataObject )
{
    vtkPolyData* dataObjectCopy = vtkPolyData::New();
    dataObjectCopy->DeepCopy( dataObject );
    vtkPolyDataMapper* polyDataMapper = vtkPolyDataMapper::New();
    polyDataMapper->SetInput( dataObject );
    vtkActor* actor = vtkActor::New();
    actor->SetMapper( polyDataMapper );

/*
    this->Frames.push_back( dataObjectCopy );
    this->Mappers.push_back( polyDataMapper );
    this->Props.push_back( actor );
*/
    //this->Modified();
}

bool msvEntityImpl::HasAnyTimeStep()
{
    return ( this->CurrentTimeStep != 0 );
}

void msvEntityImpl::SetRenderer( vtkThreadSafeRenderer* assignedRenderer )
{
    this->AssignedRenderer = assignedRenderer;

    if( this->CurrentTimeStepProp )
    {
        this->AddViewPropToRenderer( this->CurrentTimeStepProp );
    }
}

void msvEntityImpl::SetRenderer( vtkRenderer* assignedRenderer )
{
    this->AssignedRenderer = assignedRenderer;

    if( !this->CurrentTimeStepProp )
        return;

    vtkThreadSafeRenderer* tsr = dynamic_cast<vtkThreadSafeRenderer*>( this->AssignedRenderer );
    if( tsr )
    {
        SetRenderer( tsr );
    }
    else
    {
        AddViewPropToRenderer( this->CurrentTimeStepProp );
    }

    //this->Modified();
}

void msvEntityImpl::SetTimeStepProvider( vtkTemporalDataSetTimeStepProvider* timeStepProvider )
{
    this->TimeStepProvider = timeStepProvider;
}

void msvEntityImpl::AddViewPropToRenderer( vtkProp* prop )
{
    if( vtkActor::SafeDownCast( prop ) != 0 )
    {
        this->AssignedRenderer->AddActor( prop );
    }
    else if( vtkVolume::SafeDownCast( prop ) != 0 )
    {
        this->AssignedRenderer->AddVolume( prop );
    }
}

void msvEntityImpl::RemoveViewPropToRenderer( vtkProp* prop )
{
    if( vtkActor::SafeDownCast( prop ) != 0 )
    {
        this->AssignedRenderer->RemoveActor( prop );
    }
    else if( vtkVolume::SafeDownCast( prop ) != 0 )
    {
        this->AssignedRenderer->RemoveVolume( prop );
    }
}

void msvEntityImpl::ReplaceCurrentTimeStepData(
    vtkDataSet* newTimeStepData, vtkProp* newTimeStepProp,
    vtkAbstractMapper3D* newtTimeStepMapper, vtkVolumeProperty* newTimeStepVolumeProperty )
{
    vtkDataSet* oldTimeStepData = this->CurrentTimeStepData;
    vtkProp* oldTimeStepProp = this->CurrentTimeStepProp;
    vtkAbstractMapper3D* oldTimeStepMapper = this->CurrentTimeStepMapper;
    vtkVolumeProperty* oldTimeStepVolumeProperty = this->CurrentTimeStepVolumeProperty;

    this->CurrentTimeStepData = newTimeStepData;
    this->CurrentTimeStepProp = newTimeStepProp;
    this->CurrentTimeStepMapper = newtTimeStepMapper;
    this->CurrentTimeStepVolumeProperty = newTimeStepVolumeProperty;

    if( this->AssignedRenderer )
    {
        if( this->AssignedRenderer->HasViewProp( oldTimeStepProp ) )
        {
            this->RemoveViewPropToRenderer( oldTimeStepProp );
        }
        AddViewPropToRenderer( this->CurrentTimeStepProp );
    }
    if( oldTimeStepData != 0 )
    {
        oldTimeStepData->Delete();
    }
    if( oldTimeStepProp != 0 )
    {
        oldTimeStepProp->Delete();
    }
    if( oldTimeStepMapper != 0 )
    {
        oldTimeStepMapper->Delete();
    }
    if( oldTimeStepVolumeProperty != 0 )
    {
        oldTimeStepVolumeProperty->Delete();
    }
}

void msvEntityImpl::SetCurrentTimeStepData( vtkStructuredPoints* dataObject, int timeStepNumber )
{
    vtkStructuredPoints* dataObjectCopy = vtkStructuredPoints::New();
    dataObjectCopy->DeepCopy( dataObject );
    vtkVolumeMapper* volumeMapper = vtkGPUVolumeRayCastMapper::New();
    volumeMapper->SetInput( dataObject );
    vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
    vtkVolume* volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( volumeProperty );

    this->ReplaceCurrentTimeStepData( dataObjectCopy, volume, volumeMapper, volumeProperty );
}

void msvEntityImpl::SetCurrentTimeStepData( vtkPolyData* dataObject, int timeStepNumber )
{
    //cout << this->CurrentTimeStep << endl;
    cout << timeStepNumber << endl;
    cout << "dataObject: " << dataObject << endl;

    vtkPolyData* dataObjectCopy = vtkPolyData::New();
    dataObjectCopy->DeepCopy( dataObject );
    vtkPolyDataMapper* polyDataMapper = vtkPolyDataMapper::New();
    polyDataMapper->SetInput( dataObject );
    vtkActor* actor = vtkActor::New();
    actor->SetMapper( polyDataMapper );

    this->ReplaceCurrentTimeStepData( dataObjectCopy, actor, polyDataMapper, 0 );
}

void msvEntityImpl::Tick( long elapsedTime )
{
    // No prop? Return
    if( !this->CurrentTimeStepProp )
        return;
    // No renderer? Return
    if( !this->AssignedRenderer )
        return;

    // If the entity is not yet assigned to the renderer, there is no need
    // to wait for the next frame period
    if( !this->FirstTimeShown && !this->AssignedRenderer->HasViewProp( this->CurrentTimeStepProp ) )
    {
        AddViewPropToRenderer( this->CurrentTimeStepProp );
        this->CurrentTimeStepProp->SetVisibility( 1 );
        FirstTimeShown = true;
    }
    else
    {
        AcumElapsedTime += elapsedTime;
        //long NextFramePeriod = 5000;
        //long NextFramePeriod = 1000;
        //long NextFramePeriod = 500;
        //long NextFramePeriod = 250;
        long NextFramePeriod = 100;
        //long NextFramePeriod = 50;
        //long NextFramePeriod = 25;
        //long NextFramePeriod = 15;
        if( AcumElapsedTime < NextFramePeriod )
            return;

        //cout << setw( 5 ) << fixed << setprecision( 3 ) << right << vtkTimerLog::GetCPUTime() << ": AcumElapsedTime " << AcumElapsedTime << endl;

        AcumElapsedTime -= NextFramePeriod;

        ++this->CurrentTimeStep;
        //cout << this->CurrentTimeStep << endl;
        if( this->CurrentTimeStep >= 34 )
        {
            this->CurrentTimeStep = 0;
        }

        // We have moved a frame, so request next to start being loaded
        this->EntityMgr->RequestNextSequentialTimeStep( this->PublicInterface );
    }

    //this->Modified();
}

void msvEntityImpl::PrintSelf( ostream& os, vtkIndent indent )
{
}

/////////////////////////
// Public interface class
/////////////////////////

vtkStandardNewMacro( msvEntity );

msvEntity::msvEntity()
{
    this->Impl = new msvEntityImpl( this );
}

msvEntity::~msvEntity()
{
    delete Impl;
}

void msvEntity::SetEntityMgr( msvEntityMgr* entityMgr )
{
    this->Impl->SetEntityMgr( entityMgr );
}

void msvEntity::AddDataObject( vtkStructuredPoints* dataObject )
{
    this->Impl->AddDataObject( dataObject );
}

void msvEntity::AddDataObject( vtkPolyData* dataObject )
{
    this->Impl->AddDataObject( dataObject );
}

void msvEntity::SetCurrentTimeStepData( vtkStructuredPoints* dataObject, int timeStepNumber )
{
    this->Impl->SetCurrentTimeStepData( dataObject, timeStepNumber );
}

void msvEntity::SetCurrentTimeStepData( vtkPolyData* dataObject, int timeStepNumber )
{
    this->Impl->SetCurrentTimeStepData( dataObject, timeStepNumber );
}

bool msvEntity::HasAnyTimeStep()
{
    return this->Impl->HasAnyTimeStep();
}

void msvEntity::SetRenderer( vtkThreadSafeRenderer* assignedRenderer )
{
    this->Impl->SetRenderer( assignedRenderer );
}

void msvEntity::SetRenderer( vtkRenderer* assignedRenderer )
{
    this->Impl->SetRenderer( assignedRenderer );
}

void msvEntity::SetTimeStepProvider( vtkTemporalDataSetTimeStepProvider* timeStepProvider )
{
    this->Impl->SetTimeStepProvider( timeStepProvider );
}

void msvEntity::Tick( long elapsedTime )
{
    this->Impl->Tick( elapsedTime );
}

void msvEntity::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Impl->PrintSelf( os, indent );
}

