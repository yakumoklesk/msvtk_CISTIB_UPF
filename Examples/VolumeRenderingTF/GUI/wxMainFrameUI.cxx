///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxMainFrameUI.h"

///////////////////////////////////////////////////////////////////////////

wxMainFrameUI::wxMainFrameUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_RootSizer = new wxBoxSizer( wxVERTICAL );
	
	m_HorizontalMarginSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel8 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 5,-1 ), wxCLIP_CHILDREN|wxTAB_TRAVERSAL );
	m_HorizontalMarginSizer->Add( m_panel8, 0, wxEXPAND, 5 );
	
	m_VerticalSizer = new wxBoxSizer( wxVERTICAL );
	
	m_panel5 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,5 ), wxTAB_TRAVERSAL );
	m_VerticalSizer->Add( m_panel5, 0, wxEXPAND, 5 );
	
	m_HorizontalSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_VerticalSizer->Add( m_HorizontalSizer, 1, wxEXPAND, 5 );
	
	m_HorizontalMarginSizer->Add( m_VerticalSizer, 1, wxEXPAND, 5 );
	
	m_panel9 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 5,-1 ), wxCLIP_CHILDREN|wxTAB_TRAVERSAL );
	m_HorizontalMarginSizer->Add( m_panel9, 0, wxEXPAND, 5 );
	
	m_RootSizer->Add( m_HorizontalMarginSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( m_RootSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

wxMainFrameUI::~wxMainFrameUI()
{
}
