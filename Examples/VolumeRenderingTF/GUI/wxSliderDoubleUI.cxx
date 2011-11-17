///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxSliderDoubleUI.h"

///////////////////////////////////////////////////////////////////////////

wxSliderDoubleWithTextControlUI::wxSliderDoubleWithTextControlUI( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_RootSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_txtLabel = new wxStaticText( this, wxID_ANY, wxT("�����:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtLabel->Wrap( -1 );
	m_txtLabel->SetMinSize( wxSize( 60,-1 ) );
	m_txtLabel->SetMaxSize( wxSize( 60,-1 ) );
	
	bSizer4->Add( m_txtLabel, 1, wxALIGN_CENTER_VERTICAL, 0 );
	
	m_tcValue = new wxTextCtrl( this, wxID_TEXTCTRL_VALUE, wxT("000"), wxDefaultPosition, wxSize( 50,30 ), wxTE_PROCESS_ENTER|wxTE_RIGHT );
	m_tcValue->SetMinSize( wxSize( 50,30 ) );
	m_tcValue->SetMaxSize( wxSize( 50,30 ) );
	
	bSizer4->Add( m_tcValue, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4 );
	
	wxBoxSizer* SliderDoubleSizer;
	SliderDoubleSizer = new wxBoxSizer( wxVERTICAL );
	
	m_SliderLabelsSizer = new wxGridSizer( 1, 3, 0, 0 );
	
	m_lblMinValue = new wxStaticText( this, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblMinValue->Wrap( -1 );
	m_SliderLabelsSizer->Add( m_lblMinValue, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 0 );
	
	m_lblCurrentValue = new wxStaticText( this, wxID_ANY, wxT("0.5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblCurrentValue->Wrap( -1 );
	m_SliderLabelsSizer->Add( m_lblCurrentValue, 0, wxALIGN_CENTER, 0 );
	
	m_lblMaxValue = new wxStaticText( this, wxID_ANY, wxT("1.0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblMaxValue->Wrap( -1 );
	m_SliderLabelsSizer->Add( m_lblMaxValue, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0 );
	
	SliderDoubleSizer->Add( m_SliderLabelsSizer, 0, wxEXPAND, 5 );
	
	m_sldSlider = new wxSlider( this, wxID_SLIDER_VALUE, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	SliderDoubleSizer->Add( m_sldSlider, 1, wxEXPAND, 0 );
	
	bSizer4->Add( SliderDoubleSizer, 2, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_RootSizer->Add( bSizer4, 0, wxALL|wxEXPAND, 4 );
	
	this->SetSizer( m_RootSizer );
	this->Layout();
	
	// Connect Events
	m_tcValue->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( wxSliderDoubleWithTextControlUI::OnValueLeaveWindow ), NULL, this );
	m_tcValue->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( wxSliderDoubleWithTextControlUI::OnValueEntered ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScrollChanged ), NULL, this );
}

wxSliderDoubleWithTextControlUI::~wxSliderDoubleWithTextControlUI()
{
	// Disconnect Events
	m_tcValue->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( wxSliderDoubleWithTextControlUI::OnValueLeaveWindow ), NULL, this );
	m_tcValue->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( wxSliderDoubleWithTextControlUI::OnValueEntered ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScroll ), NULL, this );
	m_sldSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxSliderDoubleWithTextControlUI::OnSliderScrollChanged ), NULL, this );
	
}
