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

#ifndef __WXMISCTOOLSPANEL_H__
#define __WXMISCTOOLSPANEL_H__

#include "wxMiscToolsPanelUI.h"

class vtkVolume;
class vtkGPUVolumeRayCastMapper;
class vtkVolumeProperty;
class vtkRenderer;
class vtkAnimationScene;

class wxMiscToolsPanel : public wxMiscToolsPanelUI
{
// public methods
public:
    wxMiscToolsPanel( wxWindow* parent, int id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0 );
    virtual ~wxMiscToolsPanel();

    void SetCurrentAnimationScene( vtkAnimationScene* pCurrentAnimationScene );
    void SetCurrentRenderer( vtkRenderer* pCurrentRenderer );

    virtual void OnStartAnimationPressed( wxCommandEvent& event );
    virtual void OnStopAnimationPressed( wxCommandEvent& event );

// private methods
private:
    void UpdateRenderer();

// private members
private:
    vtkAnimationScene* m_pCurrentAnimationScene;
    vtkRenderer*    m_pCurrentRenderer;

};

#endif  // #ifndef __WXMISCTOOLSPANEL_H__

