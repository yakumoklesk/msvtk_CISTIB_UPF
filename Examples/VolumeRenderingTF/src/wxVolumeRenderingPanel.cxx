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

#include "wxVolumeRenderingPanel.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkVolume.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkVolumeProperty.h"

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


wxVolumeRenderingPanel::wxVolumeRenderingPanel( wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style )
: wxVolumeRenderingPanelUI( parent, id, pos, size, style )
, m_pCurrentVolume( NULL )
{
    m_pAmbientSliderDouble->SetLabel( "Ambient intensity:" );
    //m_pAmbientSliderDouble->SetMinValue( 0.0 );
    //m_pAmbientSliderDouble->SetMaxValue( 1.0 );
    m_pDiffuseSliderDouble->SetLabel( "Diffuse intensity:" );
    m_pSpecularSliderDouble->SetLabel( "Specular:" );
    m_pSpecularPowerSliderDouble->SetLabel( "Specular power:" );
    m_pScalarOpacityUnitDistanceSliderDouble->SetLabel( "Scalar Opacity Unit Distance:" );

    m_pAmbientSliderDouble->Connect( EVT_VALUE_CHANGED, wxCommandEventHandler( wxVolumeRenderingPanel::OnAmbientIntensityChanged ), NULL, this );
    m_pDiffuseSliderDouble->Connect( EVT_VALUE_CHANGED, wxCommandEventHandler( wxVolumeRenderingPanel::OnDiffuseIntensityChanged ), NULL, this );
    m_pSpecularSliderDouble->Connect( EVT_VALUE_CHANGED, wxCommandEventHandler( wxVolumeRenderingPanel::OnSpecularChanged ), NULL, this );
    m_pSpecularPowerSliderDouble->Connect( EVT_VALUE_CHANGED, wxCommandEventHandler( wxVolumeRenderingPanel::OnSpecularPowerChanged ), NULL, this );
    m_pScalarOpacityUnitDistanceSliderDouble->Connect( EVT_VALUE_CHANGED, wxCommandEventHandler( wxVolumeRenderingPanel::OnScalarOpacityUnitDistanceChanged ), NULL, this );
}

wxVolumeRenderingPanel::~wxVolumeRenderingPanel()
{

}

void wxVolumeRenderingPanel::SetCurrentVolume( vtkVolume* pCurrentVolume )
{
    m_pCurrentVolume = pCurrentVolume;

    vtkGPUVolumeRayCastMapper* pVolumeRayCastMapper = GetGPUVolumeRayCastMapper( m_pCurrentVolume );
    if( pVolumeRayCastMapper )
    {
        
        if( pVolumeRayCastMapper->GetBlendMode() == vtkGPUVolumeRayCastMapper::COMPOSITE_BLEND )
        {
            m_choBlendMode->SetSelection( 0 );
        }
        else
        {
            m_choBlendMode->SetSelection( 1 );
        }
    }
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        m_chkIndependentComponents->SetValue( pVolumeProperty->GetIndependentComponents()?true:false );
        if( pVolumeProperty->GetInterpolationType() == VTK_NEAREST_INTERPOLATION )
        {
            m_choInterpolationType->SetSelection( 0 );
        }
        else
        {
            m_choInterpolationType->SetSelection( 1 );
        }
        m_chkShade->SetValue( pVolumeProperty->GetShade()?true:false );
/*
        double value;
        wxString valueString;
        value = pVolumeProperty->GetAmbient();
        valueString = wxString::Format( wxT( "%g" ), value );
        m_tbAmbientLighting->SetValue( valueString );
*/
    }
}

void wxVolumeRenderingPanel::SetCurrentRenderer( vtkRenderer* pCurrentRenderer )
{
    m_pCurrentRenderer = pCurrentRenderer;
}

void wxVolumeRenderingPanel::OnAmbientIntensityChanged( wxCommandEvent& WXUNUSED(event) )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        pVolumeProperty->SetAmbient( m_pAmbientSliderDouble->GetValue() );
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnDiffuseIntensityChanged( wxCommandEvent& event )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        pVolumeProperty->SetDiffuse( m_pDiffuseSliderDouble->GetValue() );
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnSpecularChanged( wxCommandEvent& event )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        pVolumeProperty->SetSpecular( m_pSpecularSliderDouble->GetValue() );
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnSpecularPowerChanged( wxCommandEvent& event )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        pVolumeProperty->SetSpecularPower( m_pSpecularPowerSliderDouble->GetValue() );
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnScalarOpacityUnitDistanceChanged( wxCommandEvent& event )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        pVolumeProperty->SetScalarOpacityUnitDistance( m_pScalarOpacityUnitDistanceSliderDouble->GetValue() );
        // Nothing here seems to work but moving interacting with the window
        m_pCurrentVolume->Update();
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnBlendModeChanged( wxCommandEvent& WXUNUSED(event) )
{
    vtkGPUVolumeRayCastMapper* pVolumeRayCastMapper = GetGPUVolumeRayCastMapper( m_pCurrentVolume );
    if( pVolumeRayCastMapper )
    {
        if( m_choBlendMode->GetSelection() == 0 )
        {
            pVolumeRayCastMapper->SetBlendModeToComposite();
        }
        else if( m_choBlendMode->GetSelection() == 1 )
        {
            pVolumeRayCastMapper->SetBlendModeToMaximumIntensity();
        }
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnIndepenenComponentsCheckChanged( wxCommandEvent& WXUNUSED(event) )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        if( m_chkIndependentComponents->IsChecked() )
        {
            pVolumeProperty->SetIndependentComponents( 1 );
        }
        else
        {
            pVolumeProperty->SetIndependentComponents( 0 );
        }
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnInterpolationTypeChanged( wxCommandEvent& WXUNUSED(event) )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        if( m_choInterpolationType->GetSelection() == 0 )
        {
            pVolumeProperty->SetInterpolationTypeToNearest();
        }
        else if( m_choInterpolationType->GetSelection() == 1 )
        {
            pVolumeProperty->SetInterpolationTypeToLinear();
        }
        // This does not update, no change is made
        m_pCurrentVolume->Update();
        UpdateRenderer();
    }
}

void wxVolumeRenderingPanel::OnShadeCheckChanged( wxCommandEvent& WXUNUSED(event) )
{
    vtkVolumeProperty* pVolumeProperty = GetVolumeProperty( m_pCurrentVolume );
    if( pVolumeProperty )
    {
        if( m_chkShade->IsChecked() )
        {
            pVolumeProperty->ShadeOn();
        }
        else
        {
            pVolumeProperty->ShadeOff();
        }
        UpdateRenderer();
    }
}

vtkGPUVolumeRayCastMapper* wxVolumeRenderingPanel::GetGPUVolumeRayCastMapper( vtkVolume* pVolume )
{
    vtkGPUVolumeRayCastMapper* pVolumeRayCastMapper = NULL;

    if( pVolume )
    {
        vtkAbstractVolumeMapper* pVolumeMapper = pVolume->GetMapper();
        pVolumeRayCastMapper = dynamic_cast<vtkGPUVolumeRayCastMapper*>( pVolumeMapper );
    }

    return pVolumeRayCastMapper;
}

vtkVolumeProperty* wxVolumeRenderingPanel::GetVolumeProperty( vtkVolume* pVolume )
{
    vtkVolumeProperty* pVolumeProperty = NULL;

    if( pVolume )
    {
        pVolumeProperty = pVolume->GetProperty();
    }

    return pVolumeProperty;
}

double wxVolumeRenderingPanel::GetClampedDouble( wxString& doubleString, double min, double max )
{
    double value;
    if( doubleString.ToDouble( &value ) )
    {
        if( value < min )
        {
            value = min;
        }
        if( value > max )
        {
            value = max;
        }
        return value;
    }

    return min;
}

//SetTextControlValue( )

void wxVolumeRenderingPanel::UpdateRenderer()
{
    if( m_pCurrentRenderer )
    {
        //m_pCurrentRenderer->Render();
        m_pCurrentRenderer->GetRenderWindow()->Render();
    }
}

