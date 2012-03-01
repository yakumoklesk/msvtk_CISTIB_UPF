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

#include <qtimer.h>
#include <Qtime>
#include <QDir>

#include <string>

using std::string;

int g_ArgCount;
char** g_ArgVector = 0;

msvQApp::msvQApp()
: QApplication( g_ArgCount, g_ArgVector )
{
}

msvQApp::msvQApp( int &argc, char **argv )
: QApplication( argc, argv )
, timer( 0 )
{
}

/*
~virtual msvQApp()
{

}
*/
/*
int msvQApp::OnRun()
{
    return 0;
}
*/

void msvQApp::Exit()
{
    disconnect( this->timer, SIGNAL( timeout() ), this, SLOT( OnIdlePlatformDependent() ) );
    delete this->startingTime;
    delete this->timer;

    QApplication::exit();
}

string msvQApp::GetCWD()
{
    QString CWDPath = QDir::currentPath();

    return CWDPath.toStdString();
}

bool msvQApp::OnInit()
{
    msvAppBase::OnInit();

    QTime currentTime = QTime::currentTime();
    this->startingTime = new QTime( currentTime.hour(), currentTime.minute(), currentTime.second(), currentTime.msec() );

    // Activate on idle
    this->timer = new QTimer( this );
    this->connect( this->timer, SIGNAL( timeout() ), this, SLOT( OnIdlePlatformDependent() ) );
    this->timer->start( 0 );

/*
    m_CurrentTimeMillisInternal = ::wxGetLocalTimeMillis();
    Connect( wxEVT_IDLE, wxIdleEventHandler( msvWxApp::OnIdlePlatformDependent ), 0, this );
*/

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

int msvQApp::OnExit()
{
    return msvAppBase::OnExit();
}

void msvQApp::OnIdle()
{

}

void msvQApp::OnIdlePlatformDependent()
{
    QTime currentTime = QTime::currentTime();
    m_CurrentTimeMillisInternal = this->startingTime->msecsTo( currentTime );

    m_ElapsedTimeMillis = static_cast<long>( m_CurrentTimeMillisInternal );

    OnIdle();
}

void msvQApp::ResetElapsedTime()
{
    //m_ElapsedTimeMillisInternal = 0;
    m_ElapsedTimeMillis = 0;
}

int QEntry( int argc, char **argv )
{
    msvQApp* app = QCreateApp( argc, argv );
    app->OnInit();

    // Like wxWidgets does
    class CallOnExit
    {
    public:
        CallOnExit( msvQApp* app ) { this->app = app; }
        ~CallOnExit() { app->OnExit(); delete app; }
    msvQApp* app;
    } callOnExit( app );

    return app->exec();
}

