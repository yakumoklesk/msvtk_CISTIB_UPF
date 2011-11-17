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

#ifndef __MSVVTKRTRENDERWINDOWINTERACTOR_H__
#define __MSVVTKRTRENDERWINDOWINTERACTOR_H__

#include <vtkWin32RenderWindowInteractor.h>

class msvVTKRTRenderWindowInteractor : public vtkWin32RenderWindowInteractor
{
public:
    vtkTypeMacro( msvVTKRTRenderWindowInteractor, vtkWin32RenderWindowInteractor );
    // Description:
    // Construct object
    static msvVTKRTRenderWindowInteractor *New();

    virtual void Start();
    void PrintSelf( ostream& os, vtkIndent indent );

protected:
    msvVTKRTRenderWindowInteractor();
    virtual ~msvVTKRTRenderWindowInteractor();

};

#endif	// #ifndef __MSVVTKRTRENDERWINDOWINTERACTOR_H__

