///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxSliderDouble.h"

///////////////////////////////////////////////////////////////////////////

wxSliderDoubleAndValue::wxSliderDoubleAndValue( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_RootSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* SliderDoubleSizer;
	SliderDoubleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_SliderLabelsSizer = new wxGridSizer( 1, 3, 0, 0 );
	
	m_lblMinValue = new wxStaticText( this, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblMinValue->Wrap( -1 );
	m_SliderLabelsSizer->Add( m_lblMinValue, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 0 );
	
	m_lblCurrentValue = new wxStaticText( this, wxID_ANY, wxT("0.5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblCurrentValue->Wrap( -1 );
	m_SliderLabelsSizer->Add( m_lblCurrentValue, 0, wxALIGN_CENTER, 0 );
	
	m_lblRightValue = new wxStaticText( this, wxID_ANY, wxT("1.0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblRightValue->Wrap( -1 );
	m_SliderLabelsSizer->Add( m_lblRightValue, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0 );
	
	SliderDoubleSizer->Add( m_SliderLabelsSizer, 0, wxEXPAND, 5 );
	
	m_sldSlider = new wxSlider( this, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	SliderDoubleSizer->Add( m_sldSlider, 1, wxEXPAND, 0 );
	
	bSizer4->Add( SliderDoubleSizer, 2, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_RootSizer->Add( bSizer4, 0, wxALL|wxEXPAND, 4 );
	
	this->SetSizer( m_RootSizer );
	this->Layout();
	
	// Connect Events
	m_sldSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxSliderDoubleAndValue::OnScrollChanged ), NULL, this );
}

wxSliderDoubleAndValue::~wxSliderDoubleAndValue()
{
	// Disconnect Events
	m_sldSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxSliderDoubleAndValue::OnScrollChanged ), NULL, this );
	
}
