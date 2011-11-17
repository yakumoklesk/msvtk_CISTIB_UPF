///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WXMISCTOOLSPANELUI_H__
#define __WXMISCTOOLSPANELUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class wxMiscToolsPanelUI
///////////////////////////////////////////////////////////////////////////////
class wxMiscToolsPanelUI : public wxPanel 
{
	private:
	
	protected:
		wxButton* m_bttnStartAnimation;
		wxButton* m_bttnStopAnimation;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnStartAnimationPressed( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStopAnimationPressed( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxMiscToolsPanelUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~wxMiscToolsPanelUI();
	
};

#endif //__WXMISCTOOLSPANELUI_H__
