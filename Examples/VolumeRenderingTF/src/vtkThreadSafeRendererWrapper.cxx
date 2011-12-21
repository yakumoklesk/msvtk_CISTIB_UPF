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

#include "vtkThreadSafeRendererWrapper.h"

#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkMutexLock.h>


#include <vector>

using namespace std;

vtkStandardNewMacro( vtkThreadSafeRendererWrapper );

vtkThreadSafeRendererWrapper::vtkThreadSafeRendererWrapper()
: WrappedRenderer( 0 )
, RendererMutex( 0 )
, LockOwnerThreadID( -1 )
{
    this->RendererMutex = vtkMutexLock::New();
    this->WrappedRenderer = vtkRenderer::New();
}

vtkThreadSafeRendererWrapper::~vtkThreadSafeRendererWrapper()
{
    this->RendererMutex->Unlock();
    this->RendererMutex->Delete();
    this->WrappedRenderer->Delete();
}

void vtkThreadSafeRendererWrapper::LockRenderer()
{
    vtkMultiThreaderIDType threadID = vtkMultiThreader::GetCurrentThreadID();
    this->RendererMutex->Lock();
    this->LockOwnerThreadID = threadID;
}

void vtkThreadSafeRendererWrapper::UnlockRenderer()
{
    this->RendererMutex->Unlock();
    this->LockOwnerThreadID = -1;
}

vtkRenderer* vtkThreadSafeRendererWrapper::SafeGetRenderer()
{
    return this->WrappedRenderer;
}
//void msvEntity::SetRenderer( vtkRenderer* assignedRenderer )
//{
//    m_WrappedRenderer = assignedRenderer;
//    this->Modified();
//}

void vtkThreadSafeRendererWrapper::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
}
