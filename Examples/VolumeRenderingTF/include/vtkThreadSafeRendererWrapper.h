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


#ifndef __VTKTHREADSAFERENDERERWRAPPER_H__
#define __VTKTHREADSAFERENDERERWRAPPER_H__

#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkMultiThreader.h>

#include <vector>

class vtkRenderer;
class vtkMutexLock;

class vtkThreadSafeRendererWrapper : public vtkObject
{
public:
    vtkTypeMacro( vtkThreadSafeRendererWrapper, vtkObject );
    // Description:
    // Construct object
    static vtkThreadSafeRendererWrapper* New();

    void LockRenderer();
    void UnlockRenderer();
    vtkRenderer* SafeGetRenderer();

    virtual void PrintSelf( ostream& os, vtkIndent indent );

protected:
    vtkThreadSafeRendererWrapper();
    ~vtkThreadSafeRendererWrapper();

private:

    vtkRenderer*                        WrappedRenderer;
    vtkMutexLock*                       RendererMutex;
    vtkMultiThreaderIDType              LockOwnerThreadID;

};

typedef vtkSmartPointer<vtkThreadSafeRendererWrapper>  vtkThreadSafeRendererWrapperSP;

#endif	// #ifndef __VTKTHREADSAFERENDERERWRAPPER_H__

