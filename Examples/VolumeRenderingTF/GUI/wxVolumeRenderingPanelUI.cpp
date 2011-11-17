///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxVolumeRenderingPanelUI.h"

///////////////////////////////////////////////////////////////////////////

wxVolumeRenderingPanelUI::wxVolumeRenderingPanelUI( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_RootSizer = new wxBoxSizer( wxVERTICAL );
	
	m_HorizontalMarginSizer = new wxBoxSizer( wxVERTICAL );
	
	m_VolumeRenderingSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Volume Rendering") ), wxVERTICAL );
	
	m_VolumeMapperSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Volume Mapper") ), wxHORIZONTAL );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Blend Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_VolumeMapperSizer->Add( m_staticText9, 0, wxALL, 5 );
	
	wxString m_choBlendModeChoices[] = { wxT("COMPOSITE"), wxT("MAXIMUN INTENSITY") };
	int m_choBlendModeNChoices = sizeof( m_choBlendModeChoices ) / sizeof( wxString );
	m_choBlendMode = new wxChoice( this, wxID_COMBO_BLEND_MODE, wxDefaultPosition, wxDefaultSize, m_choBlendModeNChoices, m_choBlendModeChoices, 0 );
	m_choBlendMode->SetSelection( 0 );
	m_VolumeMapperSizer->Add( m_choBlendMode, 1, wxALL, 5 );
	
	m_VolumeRenderingSizer->Add( m_VolumeMapperSizer, 0, wxEXPAND, 5 );
	
	m_VolumePropertiesSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Volume Properties") ), wxVERTICAL );
	
	m_chkIndependentComponents = new wxCheckBox( this, wxID_CB_INDEPENDENT_COMPONENTS, wxT("Independent Components"), wxDefaultPosition, wxDefaultSize, 0 );
	m_VolumePropertiesSizer->Add( m_chkIndependentComponents, 0, wxALL, 5 );
	
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("Interpolation Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer8->Add( m_staticText10, 0, wxALL, 5 );
	
	wxString m_choInterpolationTypeChoices[] = { wxT("NEAREST"), wxT("LINEAR") };
	int m_choInterpolationTypeNChoices = sizeof( m_choInterpolationTypeChoices ) / sizeof( wxString );
	m_choInterpolationType = new wxChoice( this, wxID_COMBO_INTERPOLATION_TYPE_MODE, wxDefaultPosition, wxDefaultSize, m_choInterpolationTypeNChoices, m_choInterpolationTypeChoices, 0 );
	m_choInterpolationType->SetSelection( 0 );
	bSizer8->Add( m_choInterpolationType, 1, wxALL|wxEXPAND, 5 );
	
	m_VolumePropertiesSizer->Add( bSizer8, 0, wxEXPAND, 5 );
	
	m_chkShade = new wxCheckBox( this, wxID_CB_SHADE, wxT("Shade"), wxDefaultPosition, wxDefaultSize, 0 );
	m_VolumePropertiesSizer->Add( m_chkShade, 0, wxALL, 5 );
	
	m_pAmbientSliderDouble = new wxSliderDoubleWithTextControl( this );
	m_VolumePropertiesSizer->Add( m_pAmbientSliderDouble, 0, wxALL|wxEXPAND, 5 );
	
	m_pDiffuseSliderDouble = new wxSliderDoubleWithTextControl( this );
	m_VolumePropertiesSizer->Add( m_pDiffuseSliderDouble, 0, wxALL|wxEXPAND, 5 );
	
	m_pSpecularSliderDouble = new wxSliderDoubleWithTextControl( this );
	m_VolumePropertiesSizer->Add( m_pSpecularSliderDouble, 0, wxALL|wxEXPAND, 5 );
	
	m_pSpecularPowerSliderDouble = new wxSliderDoubleWithTextControl( this );
	m_VolumePropertiesSizer->Add( m_pSpecularPowerSliderDouble, 0, wxALL|wxEXPAND, 5 );
	
	m_pScalarOpacityUnitDistanceSliderDouble = new wxSliderDoubleWithTextControl( this );
	m_VolumePropertiesSizer->Add( m_pScalarOpacityUnitDistanceSliderDouble, 0, wxALL|wxEXPAND, 5 );
	
	m_VolumeRenderingSizer->Add( m_VolumePropertiesSizer, 1, wxEXPAND, 5 );
	
	m_HorizontalMarginSizer->Add( m_VolumeRenderingSizer, 1, wxEXPAND, 5 );
	
	m_RootSizer->Add( m_HorizontalMarginSizer, 1, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	this->SetSizer( m_RootSizer );
	this->Layout();
	
	// Connect Events
	m_choBlendMode->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnBlendModeChanged ), NULL, this );
	m_chkIndependentComponents->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnIndepenenComponentsCheckChanged ), NULL, this );
	m_choInterpolationType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnInterpolationTypeChanged ), NULL, this );
	m_chkShade->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnShadeCheckChanged ), NULL, this );
}

wxVolumeRenderingPanelUI::~wxVolumeRenderingPanelUI()
{
	// Disconnect Events
	m_choBlendMode->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnBlendModeChanged ), NULL, this );
	m_chkIndependentComponents->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnIndepenenComponentsCheckChanged ), NULL, this );
	m_choInterpolationType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnInterpolationTypeChanged ), NULL, this );
	m_chkShade->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxVolumeRenderingPanelUI::OnShadeCheckChanged ), NULL, this );
	
}
