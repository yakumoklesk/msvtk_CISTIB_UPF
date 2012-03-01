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
#ifndef __MSVQAPP_H__
#define __MSVQAPP_H__

// Qt includes
#include <QApplication>
#include "msvApp.h"

// Define a new application type, each program should derive a class from wxApp
class msvQApp : public QApplication, public msvAppBase
{
    Q_OBJECT
public:
    msvQApp();
    msvQApp( int &argc, char **argv );
    //~virtual msvQApp();

    virtual std::string GetCWD();
    virtual bool OnInit();
    virtual int OnExit();
    virtual void Exit();
    virtual void OnIdle();
    virtual void ResetElapsedTime();
    //virtual int OnRun();
public slots:
    void OnIdlePlatformDependent();

protected:

protected:

private:
    int                               m_CurrentTimeMillisInternal;

    QTimer *timer;
    QTime* startingTime;
};

msvQApp* QCreateApp( int argc, char **argv );

int QEntry( int argc, char **argv );

#define IMPLEMENT_QT_APP

#define IMPLEMENT_QTWIN_MAIN_CONSOLE(appname) \
    int main(int argc, char **argv) { return QEntry( argc, argv ); }

// port-specific header could have defined it already in some special way
#ifndef IMPLEMENT_QTWIN_MAIN
#define IMPLEMENT_QTWIN_MAIN IMPLEMENT_QTWIN_MAIN_CONSOLE
#endif // defined(IMPLEMENT_QTWIN_MAIN)
/*
// Use this macro if you want to define your own main() or WinMain() function
// and call wxEntry() from there.
#define IMPLEMENT_APP_NO_MAIN(appname)                                      \
	wxAppConsole *wxCreateApp()                                             \
{                                                                       \
	wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,         \
	"your program");                    \
	return new appname;                                                 \
}                                                                       \
	wxAppInitializer                                                        \
	wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);        \
	DECLARE_APP(appname)                                                    \
	appname& wxGetApp() { return *wx_static_cast(appname*, wxApp::GetInstance()); }
*/
#define IMPLEMENT_QTAPP_NO_MAIN(appname) \
  msvQApp* QCreateApp( int argc, char **argv ) \
  { \
    return new appname( argc, argv ); \
  } \
  DECLARE_APP(appname)                                                    \
  appname& QGetApp() { return *static_cast<appname*>( QCoreApplication::instance() ); }


/*
// Same as IMPLEMENT_APP() normally but doesn't include themes support in
// wxUniversal builds
#define IMPLEMENT_APP_NO_THEMES(appname)    \
	IMPLEMENT_APP_NO_MAIN(appname)          \
	IMPLEMENT_WXWIN_MAIN

// Use this macro exactly once, the argument is the name of the wxApp-derived
// class which is the class of your application.
#define IMPLEMENT_APP(appname)              \
	IMPLEMENT_APP_NO_THEMES(appname)        \
	IMPLEMENT_WX_THEME_SUPPORT
*/

// Same as IMPLEMENT_APP(), but for console applications.
#define IMPLEMENT_QTAPP_CONSOLE(appname)      \
	IMPLEMENT_QTAPP_NO_MAIN(appname)          \
	IMPLEMENT_QTWIN_MAIN_CONSOLE(appname)

// this macro can be used multiple times and just allows you to use wxGetApp()
// function
#define DECLARE_APP(appname) extern appname& QGetApp();

#endif  // #ifndef __MSVQAPP_H__
