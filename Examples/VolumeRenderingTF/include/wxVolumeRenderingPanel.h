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

#ifndef __WXVOLUMERENDERINGPANEL_H__
#define __WXVOLUMERENDERINGPANEL_H__

#include "wxVolumeRenderingPanelUI.h"

class vtkVolume;
class vtkGPUVolumeRayCastMapper;
class vtkVolumeProperty;
class vtkRenderer;

class wxVolumeRenderingPanel : public wxVolumeRenderingPanelUI
{
// public methods
public:
    wxVolumeRenderingPanel( wxWindow* parent, int id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0 );
    virtual ~wxVolumeRenderingPanel();

    void SetCurrentVolume( vtkVolume* pCurrentVolume );
    void SetCurrentRenderer( vtkRenderer* pCurrentRenderer );


    virtual void OnAmbientIntensityChanged( wxCommandEvent& event );
    virtual void OnDiffuseIntensityChanged( wxCommandEvent& event );
    virtual void OnSpecularChanged( wxCommandEvent& event );
    virtual void OnSpecularPowerChanged( wxCommandEvent& event );
    virtual void OnScalarOpacityUnitDistanceChanged( wxCommandEvent& event );


    virtual void OnBlendModeChanged( wxCommandEvent& event );
    virtual void OnIndepenenComponentsCheckChanged( wxCommandEvent& event );
    virtual void OnInterpolationTypeChanged( wxCommandEvent& event );
    virtual void OnShadeCheckChanged( wxCommandEvent& event );

	/*
    virtual void OnAmbientLightingTextChanged( wxCommandEvent &event );
    virtual void OnAmbientLightingEndScroll( wxScrollEvent &event );
    virtual void OnDiffuseLightingTextChanged( wxCommandEvent &event );
    virtual void OnDiffuseLightingEndScroll( wxScrollEvent &event );
    virtual void OnSpecularTextChanged( wxCommandEvent &event );
    virtual void OnSpecularEndScroll( wxScrollEvent &event );
    virtual void OnSpecularPowerTextChanged( wxCommandEvent &event );
    virtual void OnSpecularPowerEndScroll( wxScrollEvent &event );
    virtual void OnScalarOpacityUnitDistanceTextChanged( wxCommandEvent &event );
    virtual void OnScalarOpacityUnitDistanceEndScroll( wxScrollEvent &event );
	*/

// private methods
private:
    vtkGPUVolumeRayCastMapper* GetGPUVolumeRayCastMapper( vtkVolume* pVolume );
    vtkVolumeProperty* GetVolumeProperty( vtkVolume* pVolume );
    double GetClampedDouble( wxString& doubleString, double min, double max );
    void UpdateRenderer();
    

// private members
private:
    vtkVolume*      m_pCurrentVolume;
    vtkRenderer*    m_pCurrentRenderer;

};

#endif  // #ifndef __WXVOLUMERENDERINGPANEL_H__

