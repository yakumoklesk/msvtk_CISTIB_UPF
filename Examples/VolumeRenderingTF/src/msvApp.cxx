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

#include "msvApp.h"

#include <vtkCommand.h>

#include <string>

using namespace std;

#define USE_MAIN_FRAME 0


msvAppBase::msvAppBase()
{

}

void msvAppBase::Exit()
{

}

// 'Main program' equivalent: the program execution "starts" here
bool msvAppBase::OnInit()
{
    return TRUE;
}

int msvAppBase::OnExit()
{
    return 0;
}

void msvAppBase::OnIdle()
{

}

string msvAppBase::GetCWD()
{
    return string();
    //return wxGetCwd().mb_str();
}

string msvAppBase::GetResouceFolderPath()
{
#if defined( BUILD_LOCATION )
  #if( BUILD_LOCATION == BUILD_FROM_WORK )
    string resFolder = "F:/dev/prj/UsingVTK/thirdParty/WXVTK-1.3/wxVTK/VolumeRenderingTF/res/";
  #elif( BUILD_LOCATION == BUILD_FROM_HOME )
    string resFolder = "C:/dev/prj/job/UsingVTK/thirdParty/WXVTK-1.3/wxVTK/VolumeRenderingTF/res/";
  #endif
#else
    string resFolder = "H:/dev/prj/UsingVTK/thirdParty/WXVTK-1.3/wxVTK/VolumeRenderingTF/res/";
#endif	// #ifdef( BUILD_LOCATION )
    return resFolder;
}

