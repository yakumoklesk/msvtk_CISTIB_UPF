///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxMiscToolsPanelUI.h"

///////////////////////////////////////////////////////////////////////////

wxMiscToolsPanelUI::wxMiscToolsPanelUI( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bttnStartAnimation = new wxButton( this, wxID_ANY, wxT("Start Animation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_bttnStartAnimation, 0, wxALL, 5 );
	
	m_bttnStopAnimation = new wxButton( this, wxID_ANY, wxT("Stop Animation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_bttnStopAnimation, 0, wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_bttnStartAnimation->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMiscToolsPanelUI::OnStartAnimationPressed ), NULL, this );
	m_bttnStopAnimation->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMiscToolsPanelUI::OnStopAnimationPressed ), NULL, this );
}

wxMiscToolsPanelUI::~wxMiscToolsPanelUI()
{
	// Disconnect Events
	m_bttnStartAnimation->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMiscToolsPanelUI::OnStartAnimationPressed ), NULL, this );
	m_bttnStopAnimation->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMiscToolsPanelUI::OnStopAnimationPressed ), NULL, this );
	
}
