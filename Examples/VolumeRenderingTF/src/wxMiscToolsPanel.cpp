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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxMiscToolsPanel.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkAnimationScene.h>


#include <string>

using namespace std;

/*
#include "wx/Utils.h"
const wxWindowID wxID_COMBO_BLEND_MODE                   = wxNewId();
const wxWindowID wxID_CB_INDEPENDENT_COMPONENTS          = wxNewId();
const wxWindowID wxID_COMBO_INTERPOLATION_TYPE_MODE      = wxNewId();
const wxWindowID wxID_CB_SHADE                           = wxNewId();
const wxWindowID wxID_TB_AMBIENT_LIGHTING                = wxNewId();
const wxWindowID wxID_SLD_AMBIENT_LIGHTING               = wxNewId();
const wxWindowID wxID_TB_DIFFUSE_LIGHTING                = wxNewId();
const wxWindowID wxID_SLD_DIFFUSE_LIGHTING               = wxNewId();
const wxWindowID wxID_TB_SPECULAR                        = wxNewId();
const wxWindowID wxID_SLD_SPECULAR                       = wxNewId();
const wxWindowID wxID_TB_SPECULAR_POWER                  = wxNewId();
const wxWindowID wxID_SLD_SPECULAR_POWER                 = wxNewId();
const wxWindowID wxID_TB_SCALAR_OPACITY_UNIT_DISTANCE    = wxNewId();
const wxWindowID wxID_SLD_SCALAR_OPACITY_UNIT_DISTANCE   = wxNewId();
*/


wxMiscToolsPanel::wxMiscToolsPanel( wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style )
: wxMiscToolsPanelUI( parent, id, pos, size, style )
, m_pCurrentAnimationScene( NULL )
, m_pCurrentRenderer( NULL )
{

}

wxMiscToolsPanel::~wxMiscToolsPanel()
{

}

void wxMiscToolsPanel::SetCurrentAnimationScene( vtkAnimationScene* pCurrentAnimationScene )
{
    m_pCurrentAnimationScene = pCurrentAnimationScene;
}

void wxMiscToolsPanel::SetCurrentRenderer( vtkRenderer* pCurrentRenderer )
{
    m_pCurrentRenderer = pCurrentRenderer;
}

void wxMiscToolsPanel::OnStartAnimationPressed( wxCommandEvent& event )
{
    if( m_pCurrentAnimationScene )
    {
        m_pCurrentAnimationScene->Play();
    }
}

void wxMiscToolsPanel::OnStopAnimationPressed( wxCommandEvent& event )
{
    if( m_pCurrentAnimationScene )
    {
        m_pCurrentAnimationScene->Stop();
    }
}

void wxMiscToolsPanel::UpdateRenderer()
{
    if( m_pCurrentRenderer )
    {
        //m_pCurrentRenderer->Render();
        m_pCurrentRenderer->GetRenderWindow()->Render();
    }
}

