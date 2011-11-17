///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WXVOLUMERENDERINGPANELUI_H__
#define __WXVOLUMERENDERINGPANELUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include "wxSliderDoubleWidget.h"
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_COMBO_BLEND_MODE 8000
#define wxID_CB_INDEPENDENT_COMPONENTS 8001
#define wxID_COMBO_INTERPOLATION_TYPE_MODE 8002
#define wxID_CB_SHADE 8003
#define wxID_SLIDERDOUBLE_AMBIENT 8004
#define wxID_SLIDERDOUBLE_DIFFUSE 8005
#define wxID_SLIDERDOUBLE_SPECULAR 8006
#define wxID_SLIDERDOUBLE_SPECULAR_POWER 8007
#define wxID_SLIDERDOUBLE_SCALAR_OPACITY_UNIT_DISTANCE 8008

///////////////////////////////////////////////////////////////////////////////
/// Class wxVolumeRenderingPanelUI
///////////////////////////////////////////////////////////////////////////////
class wxVolumeRenderingPanelUI : public wxPanel 
{
	private:
		wxBoxSizer* m_RootSizer;
		wxBoxSizer* m_HorizontalMarginSizer;
		wxStaticBoxSizer* m_VolumeRenderingSizer;
		wxStaticBoxSizer* m_VolumeMapperSizer;
		wxStaticBoxSizer* m_VolumePropertiesSizer;
		wxBoxSizer* bSizer8;
	
	protected:
		wxStaticText* m_staticText9;
		wxChoice* m_choBlendMode;
		wxCheckBox* m_chkIndependentComponents;
		wxStaticText* m_staticText10;
		wxChoice* m_choInterpolationType;
		wxCheckBox* m_chkShade;
		wxSliderDoubleWithTextControl* m_pAmbientSliderDouble;
		wxSliderDoubleWithTextControl* m_pDiffuseSliderDouble;
		wxSliderDoubleWithTextControl* m_pSpecularSliderDouble;
		wxSliderDoubleWithTextControl* m_pSpecularPowerSliderDouble;
		wxSliderDoubleWithTextControl* m_pScalarOpacityUnitDistanceSliderDouble;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBlendModeChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnIndepenenComponentsCheckChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInterpolationTypeChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShadeCheckChanged( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxVolumeRenderingPanelUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,441 ), long style = wxTAB_TRAVERSAL ); 
		~wxVolumeRenderingPanelUI();
	
};

#endif //__WXVOLUMERENDERINGPANELUI_H__
