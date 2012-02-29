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


#include "MainApp.h"

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkCommand.h>

#include "msvObjectFactory.h"

#include <string>
#include <ios>

using namespace std;

MSVTK_IMPLEMENT_APP( MainApp )

#define USE_MAIN_FRAME 0

class ExitCommand : public vtkCommand
{
public:
    static ExitCommand* New();

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *callData );
protected:

    ExitCommand();
    virtual ~ExitCommand();
};

ExitCommand::ExitCommand()
{}

ExitCommand::~ExitCommand()
{}

ExitCommand* ExitCommand::New()
{
    return new ExitCommand;
}

void ExitCommand::Execute( vtkObject *caller, unsigned long eventId, void *callData )
{
    msvGetApp().Exit();
}


MainApp::MainApp()
: m_pmsvObjectFactory( 0 )
{

}

MainApp::MainApp( int &argc, char **argv )
: msvApp( argc, argv )
, m_pmsvObjectFactory( 0 )
{
}

// 'Main program' equivalent: the program execution "starts" here
bool MainApp::OnInit()
{
    msvApp::OnInit();

    m_pmsvObjectFactory = msvObjectFactory::New();

    vtkObjectFactory::RegisterFactory( m_pmsvObjectFactory );

    m_pRendererSP = vtkRendererSP::New();
    m_pRenderWindowSP = vtkRenderWindowSP::New();
    m_RenderWindowInteractorSP = vtkRenderWindowInteractorSP::New();

    m_RenderWindowInteractorSP->SetRenderWindow( m_pRenderWindowSP );
    m_pRenderWindowSP->SetMultiSamples( 0 );
    m_pRenderWindowSP->AddRenderer( m_pRendererSP );

    CreateSceneData();

    m_pRenderWindowSP->Render();

    m_ExitCommand = 0;
    m_ExitCommand = ExitCommand::New();
    m_RenderWindowInteractorSP->AddObserver( vtkCommand::ExitEvent, m_ExitCommand );

    return TRUE;
}

int MainApp::OnExit()
{
    if( m_ExitCommand )
    {
        m_ExitCommand->Delete();
        m_ExitCommand = 0;
    }

    if( m_pmsvObjectFactory != 0 )
    {
        vtkObjectFactory::UnRegisterFactory( m_pmsvObjectFactory );
        m_pmsvObjectFactory->Delete();
        m_pmsvObjectFactory = 0;
    }
    
    return 0;
}

void MainApp::OnIdle()
{
    if( m_ElapsedTimeMillis > 40 )
    {
        ResetElapsedTime();

        //cout << setw( 5 ) << fixed << setprecision( 3 ) << right << vtkTimerLog::GetCPUTime() << ": Tick" << endl;
    }
    m_pRenderWindowSP->Render();
}

string MainApp::GetResouceFolderPath()
{
#if defined( BUILD_LOCATION )
  #if( BUILD_LOCATION == BUILD_FROM_WORK )
    string resFolder = "F:/dev/prj/UsingVTK/res/";
  #elif( BUILD_LOCATION == BUILD_FROM_HOME )
    string resFolder = "C:/dev/prj/job/UsingVTK/res/";
  #endif
#else
    string resFolder = "H:/dev/prj/UsingVTK/res/";
#endif  // #ifdef( BUILD_LOCATION )
    return resFolder;
}

void MainApp::CreateSceneData()
{
}

