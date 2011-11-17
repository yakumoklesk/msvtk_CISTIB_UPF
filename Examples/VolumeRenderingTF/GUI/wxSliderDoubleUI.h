///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SliderDoubleUI__
#define __SliderDoubleUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_TEXTCTRL_VALUE 8000
#define wxID_SLIDER_VALUE 8001

///////////////////////////////////////////////////////////////////////////////
/// Class wxSliderDoubleWithTextControlUI
///////////////////////////////////////////////////////////////////////////////
class wxSliderDoubleWithTextControlUI : public wxPanel 
{
	private:
		wxBoxSizer* m_RootSizer;
		wxGridSizer* m_SliderLabelsSizer;
	
	protected:
		wxStaticText* m_txtLabel;
		wxTextCtrl* m_tcValue;
		wxStaticText* m_lblMinValue;
		wxStaticText* m_lblCurrentValue;
		wxStaticText* m_lblMaxValue;
		wxSlider* m_sldSlider;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnValueLeaveWindow( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnValueEntered( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSliderScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnSliderScrollChanged( wxScrollEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxSliderDoubleWithTextControlUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 798,57 ), long style = wxTAB_TRAVERSAL );
		~wxSliderDoubleWithTextControlUI();
	
};

#endif //__SliderDoubleUI__
