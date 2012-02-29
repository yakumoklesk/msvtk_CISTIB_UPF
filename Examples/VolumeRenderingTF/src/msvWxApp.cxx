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

#include <string>

using namespace std;


msvWxApp::msvWxApp()
{
}

msvWxApp::msvWxApp( int &argc, char **argv )
{
}

/*
int MainAppWx::OnRun()
{
    return 0;
}
*/

void msvWxApp::Exit()
{
    wxApp::Exit();
}

string msvWxApp::GetCWD()
{
    string cwd = wxGetCwd().mb_str();

    return cwd;
}

bool msvWxApp::OnInit()
{
    msvAppBase::OnInit();

    m_CurrentTimeMillisInternal = ::wxGetLocalTimeMillis();
    Connect( wxEVT_IDLE, wxIdleEventHandler( msvWxApp::OnIdlePlatformDependent ), 0, this );

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

int msvWxApp::OnExit()
{
    return msvAppBase::OnExit();
}

void msvWxApp::OnIdle()
{

}

void msvWxApp::OnIdlePlatformDependent( wxIdleEvent& event )
{
    //wxApp::OnIdle( event );

    wxLongLong currentTimeMillis = ::wxGetLocalTimeMillis();
    m_ElapsedTimeMillisInternal = m_ElapsedTimeMillisInternal + ( currentTimeMillis - m_CurrentTimeMillisInternal );

    m_ElapsedTimeMillis = static_cast<long>( m_ElapsedTimeMillisInternal.ToLong() );

    m_CurrentTimeMillisInternal = currentTimeMillis;

    OnIdle();

    event.RequestMore();
}

void msvWxApp::ResetElapsedTime()
{
    m_ElapsedTimeMillisInternal = 0;
    m_ElapsedTimeMillis = 0;
}

