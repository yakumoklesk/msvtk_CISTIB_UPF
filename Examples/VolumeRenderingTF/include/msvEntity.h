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


#ifndef __MSVENTITY_H__
#define __MSVENTITY_H__

#include <vtkProp3D.h>
#include <vtkSmartPointer.h>

#include <vector>
#include <limits>

class vtkStructuredPoints;
class vtkPolyData;
class vtkRenderer;
class vtkThreadSafeRenderer;
class vtkTemporalDataSetTimeStepProvider;
class msvEntityMgr;

class msvEntityImpl;

class msvEntity : public vtkProp3D
{
public:
    vtkTypeMacro( msvEntity, vtkProp3D );
    // Description:
    // Construct object
    static msvEntity* New();

    // Sets the Entity Manager
    void SetEntityMgr( msvEntityMgr* entityMgr );

    // Add a frame
    void AddDataObject( vtkStructuredPoints* dataObject );
    void AddDataObject( vtkPolyData* dataObject );

    // Set current timestep
    void SetCurrentTimeStepData( vtkStructuredPoints* dataObject, int timeStepNumber = std::numeric_limits<int>::min() );
    void SetCurrentTimeStepData( vtkPolyData* dataObject, int timeStepNumber = std::numeric_limits<int>::min() );

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

protected:
    msvEntity();
    ~msvEntity();

private:
    msvEntityImpl*                      Impl;

};

typedef vtkSmartPointer<msvEntity>  msvEntitySP;

#endif	// #ifndef __MSVENTITY_H__

