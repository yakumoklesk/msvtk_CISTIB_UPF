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


#ifndef __MSVENTITYMGR_H__
#define __MSVENTITYMGR_H__

#include <vtkProp3D.h>
#include <vtkSmartPointer.h>

#include <vector>

class vtkRenderer;

class msvEntity;
struct msvEntityInfoEntry;

class msvEntityMgrImpl;

class msvEntityMgr : public vtkObject
{
public:
    vtkTypeMacro( msvEntityMgr, vtkObject );
    // Description:
    // Construct object
    static msvEntityMgr* New();


    msvEntity* CreateEntityFromDirectory( const std::string& DirectoryName, const std::string& FileWildcard = "*" );

    void SetRenderer( vtkRenderer* assignedRenderer );

    virtual void PrintSelf( ostream& os, vtkIndent indent );

    // Fills a petition to provide new time steps to the entity that asks for it
    void RequestSequentialTimeSteps( msvEntity* requesterEntity, int firstTimeStepRequested = 0, int timeStepsRequested = 1 );
    void RequestNextSequentialTimeStep( msvEntity* requesterEntity );

    // Updates having into account the elapsed time, in microseconds
    void Tick( long elapsedTime );

protected:
    msvEntityMgr();
    ~msvEntityMgr();

private:

    msvEntityMgrImpl*                   Impl;
};

typedef vtkSmartPointer<msvEntityMgr>  msvEntityMgrSP;

#endif  // #ifndef __MSVENTITYMGR_H__

