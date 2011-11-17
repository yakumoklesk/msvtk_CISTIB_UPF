///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WXSLIDERDOUBLE_H__
#define __WXSLIDERDOUBLE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class wxSliderDoubleAndValue
///////////////////////////////////////////////////////////////////////////////
class wxSliderDoubleAndValue : public wxPanel 
{
	private:
		wxBoxSizer* m_RootSizer;
		wxGridSizer* m_SliderLabelsSizer;
	
	protected:
		wxStaticText* m_lblMinValue;
		wxStaticText* m_lblCurrentValue;
		wxStaticText* m_lblRightValue;
		wxSlider* m_sldSlider;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnScrollChanged( wxScrollEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxSliderDoubleAndValue( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 798,61 ), long style = wxTAB_TRAVERSAL ); 
		~wxSliderDoubleAndValue();
	
};

#endif //__WXSLIDERDOUBLE_H__
