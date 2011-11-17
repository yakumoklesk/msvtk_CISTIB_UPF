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
#include "vtkTemporalDataSetTimeStepProvider.h"


#include <string>

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
//, m_CurrentTimeMillis( 0 )
//, m_ElapsedTimeMillis( 0 )
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

    m_RenderWindowInteractorSP = vtkRenderWindowInteractorSP::New();
    // If use msvRTRenderWindowInteractor, then on exit there is an error sound
    //m_RenderWindowInteractorSP = msvRTRenderWindowInteractor::New();


    m_pRendererSP = vtkRendererSP::New();
    m_pRenderWindowSP = vtkRenderWindowSP::New();

    m_RenderWindowInteractorSP->SetRenderWindow( m_pRenderWindowSP );
    m_pRenderWindowSP->SetMultiSamples( 0 );
    m_pRenderWindowSP->AddRenderer( m_pRendererSP );


    m_pmsvEntityMgr = msvEntityMgr::New();

    ConstructVTK();
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
    DestroyVTK();

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

string MainApp::GetCWD()
{
    return string();
    //return wxGetCwd().mb_str();
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

void MainApp::DestroyVTK()
{
    //http://www.vtk.org/pipermail/vtkusers/2003-September/019894.html
}

void MainApp::ConstructVTK()
{
}

#if( USE_CUSTOM_WINDOW_INTERACTOR )
void MainFrame::ConfigureVTK()
{

}

#else

void MainApp::ConfigureVTK()
{
    // connect the render window and wxVTK window
#if( USE_WXVTK )
    m_pRenderWindowSP = m_pwxVTKWindowInteractor->GetRenderWindow();

    // connect renderer and render window and configure render window
    m_pRenderWindowSP->AddRenderer( m_pRendererSP );
#endif
}
#endif

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

#define SHOW_VOLUME 0
#define SHOW_PIECEDPOLYDATA 1

    string resFolder = GetResouceFolderPath();
    string heartModelMRI_VOLUME = resFolder + "HeartModel/MRI";

#if( SHOW_PIECEDPOLYDATA )
    string heartPolyData = resFolder + "LVComplete.vtk";

    vtkSmartPointer<vtkMultipleStructuredPointsReader> multipleStructuredPointsReaderSP = vtkSmartPointer<vtkMultipleStructuredPointsReader>::New();
    multipleStructuredPointsReaderSP->SetDirectoryName( heartModelMRI_VOLUME.c_str() );
    multipleStructuredPointsReaderSP->SetWildCard( "*.vtk" );
/*
    //double array1[] = { 0, 1, 2 };
    //multipleStructuredPointsReaderSP->SetRequestedTimesTeps( array1, 3 );
    double array1[] = { 0 };
    multipleStructuredPointsReaderSP->SetRequestedTimesTeps( array1, 1 );
    multipleStructuredPointsReaderSP->UpdateInformation();
    multipleStructuredPointsReaderSP->Update();
    double array2[] = { 1, 2, 3 };
    multipleStructuredPointsReaderSP->SetRequestedTimesTeps( array2, 3 );
*/
    multipleStructuredPointsReaderSP->Update();


    vtkSmartPointer<vtkTemporalDataSetTimeStepProvider> temporalDataSetTimeStepProviderSP = vtkSmartPointer<vtkTemporalDataSetTimeStepProvider>::New();
    temporalDataSetTimeStepProviderSP->SetInputConnection( multipleStructuredPointsReaderSP->GetOutputPort() );
    temporalDataSetTimeStepProviderSP->SetCacheSize( 3 );
    temporalDataSetTimeStepProviderSP->SetNextTimeStep( 0 );
    temporalDataSetTimeStepProviderSP->Update();

    vtkTemporalDataSet* temporalDataSet = temporalDataSetTimeStepProviderSP->GetOutput();

    temporalDataSetTimeStepProviderSP->SetNextTimeStep( 1 );
    temporalDataSetTimeStepProviderSP->Update();


    //vtkStructuredPoints* structuredPoints = vtkStructuredPoints::SafeDownCast( multipleStructuredPointsReaderSP->GetOutput()->GetTimeStep( 0 ) );
    vtkStructuredPoints* structuredPoints = vtkStructuredPoints::SafeDownCast( temporalDataSetTimeStepProviderSP->GetOutput()->GetTimeStep( 0 ) );
    if( structuredPoints )
    {
        m_EntitySP = vtkSmartPointer<msvEntity>::New();
        m_EntitySP->AddDataObject( structuredPoints );
        m_EntitySP->SetRenderer( m_pRendererSP );
    }

    vtkSmartPointer<vtkPolyDataReader> polyDataReaderSP = vtkSmartPointer<vtkPolyDataReader>::New();
    polyDataReaderSP->SetFileName( heartPolyData.c_str() );
    polyDataReaderSP->Update();
    
    m_PolyDataSP = vtkSmartPointer<vtkPolyData>::New();
    m_PolyDataSP->DeepCopy( polyDataReaderSP->GetOutput() );
    
    m_PolyDataMapperSP = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_PolyDataMapperSP->SetInput( m_PolyDataSP );
    
    m_PolyDataActorSP = vtkSmartPointer<vtkActor>::New();
    m_PolyDataActorSP->SetMapper( m_PolyDataMapperSP );
    
    m_pRendererSP->AddActor( m_PolyDataActorSP );


#endif

#if( SHOW_VOLUME )
    
    string heartModel3DUS = resFolder + "HeartModel/3DUS_images";
    string heartModelMRI_VOLUME = resFolder + "HeartModel/MRI";
    string heartModelMRI_POLYDATA = resFolder + "HeartModel/MRI_fitting_LV";

    if( m_pmsvEntityMgr != 0 )
    {
        m_pmsvEntityMgr->SetRenderer( m_pRendererSP );
        msvEntity* loadedEntity;
        loadedEntity = m_pmsvEntityMgr->CreateEntityFromDirectory( heartModelMRI_VOLUME, "*.vtk" );
    }
#endif

    //CreateAddInputVolumeActor();
    //CreateAnimation();
}

void MainApp::CreateAddInputVolumeActor()
{
    string geometryFileName = "../res/skull.v3d";
    string resFolder = "H:/dev/prj/UsingVTK/thirdParty/WXVTK-1.3/wxVTK/VolumeRenderingTF/res/";
    string volumeFileName = "skull.vtk";
    volumeFileName = "VTK-0-VolumeCropped_Double.vtk";;
    volumeFileName = "VTK-0-VolumeCropped_Double_small.vtk";;

    string volumeFilePath = "";
    volumeFilePath += GetCWD();
    volumeFilePath += "\\VolumeRenderingTF";
    volumeFilePath += "\\res\\skull.vtk";
    volumeFilePath = resFolder + volumeFileName;
    string heartModel3DUS = resFolder + "HeartModel/3DUS_images";
    string heartModelMRI = resFolder + "HeartModel/MRI";

    // Create geometry
    //m_VolumeReaderSP = vtkVolumeReaderSP::New();
    m_VolumeReaderSP = vtkStructuredPointsReaderSP::New();
    m_VolumeReaderSP->SetFileName( volumeFilePath.c_str() );
    m_VolumeReaderSP->Update();

    vtkStructuredPointsSP structuredPointsSP = m_VolumeReaderSP->GetOutput();

#define USE_TEXTUREMAPPER 0

    m_VolumeTextureMapper3DSP = vtkVolumeTextureMapper3DSP::New();
    m_VolumeTextureMapper3DSP->SetInput( structuredPointsSP );
    m_VolumeTextureMapper3DSP->SetBlendModeToComposite();
    //m_VolumeTextureMapper3DSP->SetBlendModeToMaximumIntensity();

    m_GPUVolumeMapperSP = vtkGPUVolumeRayCastMapperSP::New();
    //m_GPUVolumeMapperSP->SetInput( static_cast<vtkImageData*>( m_VolumeReaderSP->GetOutput() ) );
    m_GPUVolumeMapperSP->SetInput( structuredPointsSP );
    m_GPUVolumeMapperSP->SetBlendModeToComposite();
    //m_GPUVolumeMapperSP->SetBlendModeToMaximumIntensity();


    m_SalarOpacityTransferFunctionSP = vtkPiecewiseFunctionSP::New();
    m_ColorTransferFunctionSP = vtkColorTransferFunctionSP::New();
    m_GradientOpacityTransferFunctionSP = vtkPiecewiseFunctionSP::New();

    double scalarRange[2];
    structuredPointsSP->GetScalarRange( scalarRange );
    int min = scalarRange[0];
    int max = scalarRange[1];
    int range = max - min;
    m_SalarOpacityTransferFunctionSP->Initialize();
    //m_ScalarOpacityFunction->AddPoint( m_Min,0.0 );
    m_SalarOpacityTransferFunctionSP->AddPoint( min, 0.0 );
    m_SalarOpacityTransferFunctionSP->AddPoint( range*0.33333, 0.0 );
    m_SalarOpacityTransferFunctionSP->AddPoint( range*0.87541, 1.0 );
    m_SalarOpacityTransferFunctionSP->AddPoint( max, 1.0 );

    m_ColorTransferFunctionSP->RemoveAllPoints();
    m_ColorTransferFunctionSP->AddRGBPoint( min, 0, 0, 0 );
    m_ColorTransferFunctionSP->AddRGBPoint( max, 1, 1, 0 );

    m_GradientOpacityTransferFunctionSP->Initialize();
    m_GradientOpacityTransferFunctionSP->AddPoint( 0, 1, 0.5, 0 );
    m_GradientOpacityTransferFunctionSP->AddPoint( 255, 1, 0.5, 0);


    m_VolumePropertySP = vtkVolumePropertySP::New();
    m_VolumePropertySP->SetIndependentComponents( 1 );
    //m_VolumePropertySP->SetIndependentComponents( 0 );
    m_VolumePropertySP->SetColor( m_ColorTransferFunctionSP );
    m_VolumePropertySP->SetScalarOpacity( m_SalarOpacityTransferFunctionSP );
    m_VolumePropertySP->SetGradientOpacity( m_GradientOpacityTransferFunctionSP );
    m_VolumePropertySP->SetInterpolationTypeToLinear();
    //m_VolumePropertySP->SetInterpolationTypeToNearest();
    //m_VolumePropertySP->SetComponentWeight( 0, 1.0 );

    //m_VolumePropertySP->ShadeOn();
    m_VolumePropertySP->ShadeOff();
#if 0
    //m_VolumePropertySP->SetAmbient( 0.0 );
    //m_VolumePropertySP->SetDiffuse( 1.0 );
    //m_VolumePropertySP->SetSpecular( 0.0 );
    //m_VolumePropertySP->SetSpecularPower( 0.0 );
#else
    m_VolumePropertySP->SetAmbient( 1.0 );
    m_VolumePropertySP->SetDiffuse( 1.0 );
    m_VolumePropertySP->SetSpecular( 1.0 );
    m_VolumePropertySP->SetSpecularPower( 1.0 );
#endif
    //m_VolumePropertySP->SetScalarOpacityUnitDistance( 0.8919 );
    m_VolumePropertySP->SetScalarOpacityUnitDistance( 0.7 );


    m_VolumeSP = vtkVolumeSP::New();
    m_VolumeSP->SetProperty( m_VolumePropertySP );
#if( USE_TEXTUREMAPPER )
    m_VolumeSP->SetMapper( m_VolumeTextureMapper3DSP );
#else
    m_VolumeSP->SetMapper( m_GPUVolumeMapperSP );
#endif

    m_pRendererSP->AddVolume( m_VolumeSP );
}


void MainApp::CreateAnimation()
{
    // Generate a sphere
    vtkSphereSourceSP sphereSource = vtkSphereSourceSP::New();
    vtkPolyDataMapperSP sphereMapper = vtkPolyDataMapperSP::New();
    sphereMapper->SetInputConnection( sphereSource->GetOutputPort());
    vtkActorSP sphere = vtkActorSP ::New();
    sphere->SetMapper( sphereMapper );
    m_pRendererSP->AddActor( sphere );

    // Generate a cone
    vtkConeSourceSP coneSource = vtkConeSourceSP::New();
    vtkPolyDataMapperSP coneMapper = vtkPolyDataMapperSP::New();
    coneMapper->SetInputConnection( coneSource->GetOutputPort());
    vtkActorSP cone = vtkActorSP::New();
    cone->SetMapper( coneMapper );
    m_pRendererSP->AddActor( cone );

    // Create an Animation Scene
    m_AnimationSceneSP = vtkAnimationSceneSP::New();

    //m_AnimationSceneSP->SetModeToRealTime();
    m_AnimationSceneSP->SetModeToSequence();

    m_AnimationSceneSP->SetLoop( 0 );
    m_AnimationSceneSP->SetFrameRate( 5 );
    m_AnimationSceneSP->SetStartTime( 0 );
    m_AnimationSceneSP->SetEndTime( 20 );
    /*
    AnimationSceneObserverSP sceneObserver = AnimationSceneObserverSP::New();
    sceneObserver->SetRenderWindow( m_pRenderWindow );
    m_AnimationSceneSP->AddObserver( vtkCommand::AnimationCueTickEvent, sceneObserver );

    // Create an Animation Cue for each actor
    vtkAnimationCueSP cue1 = vtkAnimationCueSP::New();
    cue1->SetStartTime( 5 );
    cue1->SetEndTime( 23 );
    m_AnimationSceneSP->AddCue( cue1 );

    vtkAnimationCueSP cue2 = vtkAnimationCueSP::New();
    cue2->SetStartTime( 1 );
    cue2->SetEndTime( 10 );
    m_AnimationSceneSP->AddCue( cue2 );

    // Create an ActorAnimator for each actor;
    m_SphereAnimator = new ActorAnimator();
    m_SphereAnimator->SetActor( sphere );
    m_SphereAnimator->AddObserversToCue( cue1 );

    m_ConeAnimator = new ActorAnimator();
    std::vector<double> endCone(3);
    endCone[0] = -1;
    endCone[1] = -1;
    endCone[2] = -1;
    m_ConeAnimator->SetEndPosition( endCone );
    m_ConeAnimator->SetActor( cone );
    m_ConeAnimator->AddObserversToCue( cue2 );
    */
}
