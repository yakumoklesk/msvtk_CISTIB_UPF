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

#ifndef __WXMAINFRAME_H__
#define __WXMAINFRAME_H__

#include "BuildConfig.h"

#include "wxMainFrameUI.h"
#include "VtkExtTypes.h"

#define USE_CUSTOM_WINDOW_INTERACTOR 1

class VolumeRenderingPanel;
#if( USE_WXVTK )
class wxVTKRenderWindowInteractor;
#endif
class ActorAnimator;

// Define a new frame type: this is going to be our main frame
class wxMainFrame : public wxMainFrameUI
{
public:
    // ctor(s)
    wxMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~wxMainFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    virtual void OnSize( wxSizeEvent& event );

protected:
    void CreateWindowAndGUI();
    void ConfigureGUIData();
    void ConfigureVTK();

private:
    

private:

    VolumeRenderingPanel*           m_VolumeRenderingPanel;

#if( USE_WXVTK )
    wxVTKRenderWindowInteractor*    m_pwxVTKWindowInteractor;
#endif
    vtkRenderWindowInteractorSP     m_RenderWindowInteractorSP;


    // vtk classes
    vtkRendererSP                   m_pRendererSP;
    vtkRenderWindowSP               m_pRenderWindowSP;
    vtkPolyDataMapper*              pConeMapper;
    vtkActor*                       pConeActor;
    vtkConeSource*                  pConeSource;

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

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif  // #ifndef __WXMAINFRAME_H__
