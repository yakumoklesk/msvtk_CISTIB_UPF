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

#ifndef __MAINAPP_H__
#define __MAINAPP_H__

#include "BuildConfig.h"
#include "msvApp.h"

#include <vector>

class ActorAnimator;
class ExitCommand;
class vtkStringArray;
class vtkStdString;
class msvEntity;
class msvEntityMgr;
class msvRTRenderWindowInteractor;
class msvObjectFactory;
class vtkThreadSafeRendererWrapper;
class vtkThreadSafeRenderer;
class vtkThreadSafeRenderWindow;
class vtkThreadSafeRenderWindowInteractor;

#define USE_THREADSAFE_RENDERER 0

// Define a new application type, each program should derive a class from wxApp
class MainApp: public msvApp
{
public:
    MainApp();

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnExit();
    virtual std::string GetCWD();
    std::string GetResouceFolderPath();
    virtual void OnIdle();

protected:
    void CreateAddInputVolumeActor();
    void CreateWindowAndGUI();
    void ConfigureGUIData();
    void DestroyVTK();
    void ConstructVTK();
    void ConfigureVTK();
    void CreateSceneData();
    void CreateAnimation();

protected:
    vtkThreadSafeRendererWrapper*   m_RendererWrapper;
    //vtkSmartPointer<msvRTRenderWindowInteractor> m_RenderWindowInteractorSP;
#if( USE_THREADSAFE_RENDERER )
    vtkThreadSafeRenderer*                  m_pRendererSP;
    vtkThreadSafeRenderWindow*              m_pRenderWindowSP;
    vtkThreadSafeRenderWindowInteractor*    m_RenderWindowInteractorSP;
#else
    vtkRendererSP                   m_pRendererSP;
    vtkRenderWindowSP               m_pRenderWindowSP;
    vtkRenderWindowInteractorSP     m_RenderWindowInteractorSP;
#endif
    

    msvEntityMgr*                   m_pmsvEntityMgr;
    msvObjectFactory*               m_pmsvObjectFactory;

    vtkSmartPointer<vtkPolyData>    m_PolyDataSP;
    vtkSmartPointer<vtkPolyDataMapper>    m_PolyDataMapperSP;
    vtkSmartPointer<vtkActor>       m_PolyDataActorSP;
    vtkSmartPointer<msvEntity>      m_EntitySP;
    vtkSmartPointer<msvEntity>      m_EntityPolyDataSP;



    //vtkVolumeReaderSP               m_VolumeReaderSP;
    vtkStructuredPointsReaderSP     m_VolumeReaderSP;
    vtkStructuredPointsReaderSP     m_StructuredPointsReader;
    vtkVolumeSP                     m_VolumeSP;
    vtkVolumeMapperSP               m_VolumeMapperSP;
    vtkActorSP                      m_VolumeActorSP;
    //vtkGPUVolumeRayCastMapperSP     m_GPUVolumeRayCastMapperSP;
    vtkVolumeTextureMapper3DSP      m_VolumeTextureMapper3DSP;
    vtkGPUVolumeRayCastMapperSP     m_GPUVolumeMapperSP;
    vtkColorTransferFunctionSP      m_ColorTransferFunctionSP;
    vtkPiecewiseFunctionSP          m_SalarOpacityTransferFunctionSP;
    vtkPiecewiseFunctionSP          m_GradientOpacityTransferFunctionSP;
    vtkVolumePropertySP             m_VolumePropertySP;

    vtkAnimationSceneSP             m_AnimationSceneSP;

    //ActorAnimator*                  m_SphereAnimator;
    //ActorAnimator*                  m_ConeAnimator;
private:

    ExitCommand*                    m_ExitCommand;
};


#endif  // #ifndef __MAINAPP_H__

