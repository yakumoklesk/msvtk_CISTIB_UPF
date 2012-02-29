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
#ifndef __MSVWXAPP_H__
#define __MSVWXAPP_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// Define a new application type, each program should derive a class from wxApp
class msvWxApp : public wxApp, public msvAppBase
//class msvtkAppWx: public msvtkAppBase, public wxAppConsole
{
public:
    msvWxApp();
    msvWxApp( int &argc, char **argv );

    virtual std::string GetCWD();
    virtual bool OnInit();
    virtual int OnExit();
    virtual void Exit();
    void OnIdlePlatformDependent( wxIdleEvent& event );
    virtual void OnIdle();
    virtual void ResetElapsedTime();
    //virtual int OnRun();

protected:

protected:

private:
    wxLongLong                      m_CurrentTimeMillisInternal;
    wxLongLong                      m_ElapsedTimeMillisInternal;

    ExitCommand*                    m_ExitCommand;
};



#endif  // #ifndef __MSVWXAPP_H__
