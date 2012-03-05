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
#include "vtkExtTypes.h"

class msvObjectFactory;
class vtkGPUPolyDataMapper;

// Define a new application type, each program should derive a class from wxApp
class MainApp: public msvApp
{
public:
    MainApp();
    MainApp( int &argc, char **argv );

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnExit();
    std::string GetResouceFolderPath();
    virtual void OnIdle();

protected:
    void CreateSceneData();

protected:
    vtkRendererSP                           m_pRendererSP;
    vtkRenderWindowSP                       m_pRenderWindowSP;
    vtkRenderWindowInteractorSP             m_RenderWindowInteractorSP;

    msvObjectFactory*                       m_pmsvObjectFactory;

    vtkActorSP                              m_SampleActorSP;
    vtkPolyDataSP                           m_SamplePolydataSP;
    vtkPolyDataMapperSP                     m_SamplePolyDataMapperSP;
    vtkSmartPointer<vtkGPUPolyDataMapper>   m_SampleGPUPolyDataMapperSP;
    vtkGPUPolyDataMapper*                   m_pSampleGPUPolyDataMapper;

private:
};


#endif  // #ifndef __MAINAPP_H__

