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

#include "msvObjectFactory.h"

#include <vtkVersion.h>
#include <vtkGraphicsFactory.h>
//#include <vtkGPUVolumeRayCastMapper.h>


// Win32 specific stuff
#ifdef _WIN32
  #ifndef VTK_USE_OGLR
    #include <vtkWin32OpenGLRenderWindow.h>
//    #include "msvVTKRTRenderWindowInteractor.h"
//#  include "vtkWin32RenderWindowInteractor.h"
    #define VTK_DISPLAY_WIN32_OGL
  #endif // VTK_USE_OGLR
#endif


vtkStandardNewMacro( msvObjectFactory );

msvObjectFactory::msvObjectFactory()
{
    /*
    RegisterOverride( 
    "vtkRenderWindowInteractor",
    "msvRTRenderWindowInteractor",
    "Overriding creation of window interactor",
    1,
    msvObjectFactoryCreatemsvRTRenderWindowInteractor );
    */
}

msvObjectFactory::~msvObjectFactory()
{

}

const char* msvObjectFactory::GetVTKSourceVersion()
{
    return VTK_SOURCE_VERSION;
}

const char* msvObjectFactory::GetDescription()
{
    return "msv Object Factory";

    //VTK_CREATE_CREATE_FUNCTION
}

vtkObject* msvObjectFactory::msvObjectFactoryCreatemsvRTRenderWindowInteractor()
{
    //return msvVTKRTRenderWindowInteractor::New();
    return 0;
}

vtkObject* msvObjectFactory::CreateObject( const char* className )
{
    const char *rl = vtkGraphicsFactory::GetRenderLibrary();
#ifdef VTK_DISPLAY_WIN32_OGL
    if ( !vtkGraphicsFactory::GetOffScreenOnlyMode() )
    {
        if(strcmp( className, "vtkRenderWindowInteractor") == 0)
        {
            //return msvVTKRTRenderWindowInteractor::New();
        }
    }
    if (!strcmp("Win32OpenGL",rl))
    {
        if(strcmp( className, "vtkRenderWindow") == 0)
        {
            return vtkWin32OpenGLRenderWindow::New();
        }
    }
#endif

    return NULL;
}

