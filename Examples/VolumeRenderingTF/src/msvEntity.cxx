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

#include <vtkObjectFactory.h>
#include <vtkStructuredPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkPolyDataMapper.h>

#include <vector>

using namespace std;

vtkStandardNewMacro( msvEntity );

msvEntity::msvEntity()
: m_AssignedRenderer( 0 )
, m_CurrentFrame( 0 )
{
    
}

msvEntity::~msvEntity()
{
    // First test if we have a renderer, and if so, if we have some
    // volume or actor added to it
    if( m_AssignedRenderer && ( m_CurrentFrame >= 0 ) )
    {
        m_AssignedRenderer->RemoveVolume( m_Props[m_CurrentFrame] );
    }

    vector<vtkStructuredPoints*>::size_type index;
    for( index = 0; index < m_Frames.size(); index++ )
    {
        m_Frames[index]->Delete();
    }
    m_Frames.clear();

    for( index = 0; index < m_VolumeProperties.size(); index++ )
    {
        m_VolumeProperties[index]->Delete();
    }
    m_VolumeProperties.clear();

    for( index = 0; index < m_Mappers.size(); index++ )
    {
        m_Mappers[index]->Delete();
    }
    m_Mappers.clear();

    for( index = 0; index < m_Props.size(); index++ )
    {
        m_Props[index]->Delete();
    }
    m_Props.clear();
}

void msvEntity::AddDataObject( vtkStructuredPoints* dataObject )
{
    vtkStructuredPoints* dataObjectCopy = vtkStructuredPoints::New();
    dataObjectCopy->DeepCopy( dataObject );
    vtkVolumeMapper* volumeMapper = vtkGPUVolumeRayCastMapper::New();
    volumeMapper->SetInput( dataObject );
    vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
    vtkVolume* volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( volumeProperty );

    m_Frames.push_back( dataObjectCopy );
    m_VolumeProperties.push_back( volumeProperty );
    m_Mappers.push_back( volumeMapper );
    m_Props.push_back( volume );

    this->Modified();
}

void msvEntity::AddDataObject( vtkPolyData* dataObject )
{
    vtkPolyData* dataObjectCopy = vtkPolyData::New();
    dataObjectCopy->DeepCopy( dataObject );
    vtkPolyDataMapper* polyDataMapper = vtkPolyDataMapper::New();
    polyDataMapper->SetInput( dataObject );
    vtkActor* actor = vtkActor::New();
    actor->SetMapper( polyDataMapper );

    m_Frames.push_back( dataObjectCopy );
    m_Mappers.push_back( polyDataMapper );
    m_Props.push_back( actor );

    this->Modified();
}

void msvEntity::SetRenderer( vtkRenderer* assignedRenderer )
{
    //m_AssignedRenderer->Register( this );
    m_AssignedRenderer = assignedRenderer;

    //vector<vtkStructuredPoints*>::iterator it;
    vector<vtkDataSet*>::iterator it;
    for( it = m_Frames.begin(); it != m_Frames.end(); it++ )
    {
        //m_AssignedRenderer->AddActor( *it );
        //m_AssignedRenderer->AddVolume( );
    }
    if( vtkActor::SafeDownCast( m_Props[m_CurrentFrame] ) != 0 )
    {
        m_AssignedRenderer->AddActor( m_Props[m_CurrentFrame] );
    }
    else if( vtkVolume::SafeDownCast( m_Props[m_CurrentFrame] ) != 0 )
    {
        m_AssignedRenderer->AddVolume( m_Props[m_CurrentFrame] );
    }

    this->Modified();
}

void msvEntity::Tick( long elapsedTime )
{
    //m_AssignedRenderer->RemoveVolume( m_Props[m_CurrentFrame] );
    m_Props[m_CurrentFrame]->SetVisibility( 0 );
    ++m_CurrentFrame;
    if( m_CurrentFrame >= static_cast<int>( m_Frames.size() ) )
    {
        m_CurrentFrame = 0;
    }
    m_AssignedRenderer->AddVolume( m_Props[m_CurrentFrame] );
    m_Props[m_CurrentFrame]->SetVisibility( 1 );

    this->Modified();
}

void msvEntity::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
}
