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
//#include "MainFrame.h"

#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkVolumeMapper.h>
#include <vtkVolumeReader.h>
#include <vtkActor.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>

#include <vtkCommand.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkErrorCode.h>
#include <vtkObjectFactory.h>
#include <vtkTimerLog.h>

//#include <vtkInteractorObserver.h>
//
#include <vtkPolyDataReader.h>
//


#include "msvVTKRTRenderWindowInteractor.h"
#include "msvEntityMgr.h"
#include "msvEntity.h"
#include "msvObjectFactory.h"
#include <vtkTemporalDataSet.h>
#include "vtkMultipleStructuredPointsReader.h"
#include "vtkMultiplePolyDataReader.h"
#include "vtkTemporalDataSetTimeStepProvider.h"
#include "vtkThreadSafeRendererWrapper.h"
#include "vtkThreadSafeRenderer.h"
#include "vtkThreadSafeRenderWindow.h"
#include "vtkThreadSafeRenderWindowInteractor.h"


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
: m_VolumeReaderSP( 0 )
, m_StructuredPointsReader( 0 )
, m_VolumeSP( 0 )
, m_VolumeMapperSP( 0 )
, m_VolumeActorSP( 0 )
, m_VolumeTextureMapper3DSP( 0 )
, m_GPUVolumeMapperSP( 0 )
, m_ColorTransferFunctionSP( 0 )
, m_SalarOpacityTransferFunctionSP( 0 )
, m_GradientOpacityTransferFunctionSP( 0 )
, m_VolumePropertySP( 0 )
, m_AnimationSceneSP( 0 )
, m_pmsvEntityMgr( 0 )
, m_pmsvObjectFactory( 0 )
{

}

MainApp::MainApp( int &argc, char **argv )
: msvApp( argc, argv )
, m_VolumeReaderSP( 0 )
, m_StructuredPointsReader( 0 )
, m_VolumeSP( 0 )
, m_VolumeMapperSP( 0 )
, m_VolumeActorSP( 0 )
, m_VolumeTextureMapper3DSP( 0 )
, m_GPUVolumeMapperSP( 0 )
, m_ColorTransferFunctionSP( 0 )
, m_SalarOpacityTransferFunctionSP( 0 )
, m_GradientOpacityTransferFunctionSP( 0 )
, m_VolumePropertySP( 0 )
, m_AnimationSceneSP( 0 )
, m_pmsvEntityMgr( 0 )
, m_pmsvObjectFactory( 0 )
{
}

// 'Main program' equivalent: the program execution "starts" here
bool MainApp::OnInit()
{
    msvApp::OnInit();

#if( USE_MAIN_FRAME )
    // create the main application window
    MainFrame *frame = new MainFrame(_T("wxWindows-VTK App"),
        wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

#else
    m_pmsvObjectFactory = msvObjectFactory::New();

    vtkObjectFactory::RegisterFactory( m_pmsvObjectFactory );

    // If use msvRTRenderWindowInteractor, then on exit there is an error sound
    //m_RenderWindowInteractorSP = msvRTRenderWindowInteractor::New();

#if( USE_THREADSAFE_RENDERER )
    m_pRendererSP = vtkThreadSafeRenderer::New();
    m_pRenderWindowSP = vtkThreadSafeRenderWindow::New();
    m_RenderWindowInteractorSP = vtkThreadSafeRenderWindowInteractor::New();
#else
    m_pRendererSP = vtkRendererSP::New();
    m_pRenderWindowSP = vtkRenderWindowSP::New();
    m_RenderWindowInteractorSP = vtkRenderWindowInteractorSP::New();
#endif
    

    m_RenderWindowInteractorSP->SetRenderWindow( m_pRenderWindowSP );
    m_pRenderWindowSP->SetMultiSamples( 0 );
    m_pRenderWindowSP->AddRenderer( m_pRendererSP );


    m_pmsvEntityMgr = msvEntityMgr::New();

    CreateSceneData();

    m_pRenderWindowSP->Render();

    m_ExitCommand = 0;
    m_ExitCommand = ExitCommand::New();
    m_RenderWindowInteractorSP->AddObserver( vtkCommand::ExitEvent, m_ExitCommand );

#endif

    return TRUE;
}

int MainApp::OnExit()
{
    if( m_ExitCommand )
    {
        m_ExitCommand->Delete();
        m_ExitCommand = 0;
    }

    if( m_pmsvEntityMgr != 0 )
    {
        m_pmsvEntityMgr->Delete();
        m_pmsvEntityMgr = 0;
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
        if( m_pmsvEntityMgr )
        {
            m_pmsvEntityMgr->Tick( m_ElapsedTimeMillis );
        }
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
    // configure renderer
    /*
    m_pRenderer->SetBackground(1.0,0.333333,0.5);
    m_pRenderer->GetActiveCamera()->Elevation(30.0);
    m_pRenderer->GetActiveCamera()->Azimuth(30.0);
    m_pRenderer->GetActiveCamera()->Zoom(1.0);
    m_pRenderer->GetActiveCamera()->SetClippingRange(1,1000);
    */

    string resFolder = GetResouceFolderPath();
    string heartModelMRI_VOLUME = resFolder + "HeartModel/MRI";
    string heartModelMRI_POLYDATA = resFolder + "HeartModel/MRI_fitting_LV";
    string heartModel3DUS = resFolder + "HeartModel/3DUS_images";
    string numbers = resFolder + "Numbers";
    string heartNumbers = resFolder + "HeartNumbers";

    if( m_pmsvEntityMgr != 0 )
    {
        m_pmsvEntityMgr->SetRenderer( m_pRendererSP );
        //msvEntity* structuredPointsEntity;
        msvEntity* polyDataEntity;
        //structuredPointsEntity = m_pmsvEntityMgr->CreateEntityFromDirectory( heartModelMRI_VOLUME, "*.vtk" );
        //polyDataEntity = m_pmsvEntityMgr->CreateEntityFromDirectory( heartModelMRI_POLYDATA, "*.vtk" );
        //polyDataEntity = m_pmsvEntityMgr->CreateEntityFromDirectory( numbers, "*.vtk" );
        polyDataEntity = m_pmsvEntityMgr->CreateEntityFromDirectory( heartNumbers, "*.vtk" );
    }
}

