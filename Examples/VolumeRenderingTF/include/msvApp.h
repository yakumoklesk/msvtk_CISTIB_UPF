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

#ifndef __MSVTKAPP_H__
#define __MSVTKAPP_H__

#include "BuildConfig.h"
#include "vtkExtTypes.h"

#include <vector>

class ActorAnimator;
class ExitCommand;
class vtkStringArray;
class vtkStdString;
class msvEntity;
class msvEntityMgr;
class msvRTRenderWindowInteractor;
class msvObjectFactory;

// Define a new application type, each program should derive a class from wxApp
class msvAppBase
{
public:
    msvAppBase();

    virtual void Exit();

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnExit();
    virtual std::string GetCWD();
    std::string GetResouceFolderPath();
    virtual void OnIdle();

protected:

protected:

private:

    //ExitCommand*                    m_ExitCommand;
};

#if( GUI_API == GUI_API_WXWIDGETS )
  #include "msvWxApp.h"
  #define msvApp msvWxApp
  #define msvGetApp wxGetApp
  #define MSVTK_IMPLEMENT_APP IMPLEMENT_APP
#endif


#endif  // #ifndef __MSVTKAPP_H__

