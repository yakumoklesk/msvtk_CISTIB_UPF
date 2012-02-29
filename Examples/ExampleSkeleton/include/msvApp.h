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

class ExitCommand;

#include <string>

// Define a new application type, each program should derive a class from wxApp
class msvAppBase
{
public:
    msvAppBase();
    msvAppBase( int &argc, char **argv );

    virtual void Exit();

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnExit();
    virtual std::string GetCWD() = 0;
    std::string GetResouceFolderPath();
    virtual void OnIdle();
    virtual void ResetElapsedTime() = 0;

protected:

protected:
    long                            m_CurrentTimeMillis;
    long                            m_ElapsedTimeMillis;
    ExitCommand*                    m_ExitCommand;

private:

};

#if( GUI_API == GUI_API_NONE )
#elif( GUI_API == GUI_API_QT )
  #include "msvQApp.h"
  #define msvApp msvQApp
  #define msvGetApp QGetApp
  #define MSVTK_IMPLEMENT_APP IMPLEMENT_QTAPP_CONSOLE
#elif( GUI_API == GUI_API_WXWIDGETS )
  #include "msvWxApp.h"
  #define msvApp msvWxApp
  #define msvGetApp wxGetApp
  #if( APPLICATION_EXECUTABLE_TYPE == APPLICATION_EXECUTABLE_TYPE_CONSOLE )
    #define MSVTK_IMPLEMENT_APP IMPLEMENT_APP_CONSOLE
  #else
    #define MSVTK_IMPLEMENT_APP IMPLEMENT_APP
  #endif
#endif


#endif  // #ifndef __MSVTKAPP_H__

