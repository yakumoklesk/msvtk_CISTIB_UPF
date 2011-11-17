///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxSliderDoubleWidget.h"

///////////////////////////////////////////////////////////////////////////

DEFINE_EVENT_TYPE( EVT_VALUE_CHANGED );

using namespace std;

wxSliderDoubleWithTextControl::wxSliderDoubleWithTextControl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxSliderDoubleWithTextControlUI( parent, id, pos, size, style )
{
    m_LastCorrectValue = 0.0;
    m_SubDivisions = 1000;
    m_MinValue = 0.0;
    m_MaxValue = 1.0;
    m_DefaultValue = 0.5;
    m_LastCorrectValue = 0.5;
    m_Value = 0.5;

    UpdateInterval();
    CorrectValues();
    UpdateControls();
}

wxSliderDoubleWithTextControl::~wxSliderDoubleWithTextControl()
{

}

void wxSliderDoubleWithTextControl::SetLabel( const char* label )
{
    m_LabelValue = label;
    wxString wxstring( m_LabelValue.c_str(), wxConvUTF8 );
    m_txtLabel->SetLabel( wxstring );
}

void wxSliderDoubleWithTextControl::SetLabel( string& label )
{
    m_LabelValue = label;
    wxString wxstring( m_LabelValue.c_str(), wxConvUTF8 );
    m_txtLabel->SetLabel( wxstring );
}

void wxSliderDoubleWithTextControl::SetLabel( wxString& label )
{
    m_LabelValue = label.mb_str();
    m_txtLabel->SetLabel( label );
}

string wxSliderDoubleWithTextControl::GetLabel()
{
    return m_LabelValue;
}

//wxString wxSliderDoubleWithTextControl::GetLabel()
//{
//    return wxString( m_Label );
//}

void wxSliderDoubleWithTextControl::SetResolution( int subdivisions )
{
    m_SubDivisions = subdivisions;
    UpdateInterval();
}

int wxSliderDoubleWithTextControl::GetResolution()
{
    return m_SubDivisions;
}

void wxSliderDoubleWithTextControl::SetMinValue( double minValue )
{
    m_MinValue = minValue;
    CorrectValues();
    UpdateControls();
}

double wxSliderDoubleWithTextControl::GetMinValue()
{
    return m_MinValue;
}

void wxSliderDoubleWithTextControl::SetMaxValue( double maxValue )
{
    m_MaxValue = maxValue;
    CorrectValues();
    UpdateControls();
}

double wxSliderDoubleWithTextControl::GetMaxValue()
{
    return m_MaxValue;
}

void wxSliderDoubleWithTextControl::SetDefaultValue( double defautlValue )
{
    m_DefaultValue = defautlValue;
}

double wxSliderDoubleWithTextControl::GetDefaultValue()
{
    return m_DefaultValue;
}

void wxSliderDoubleWithTextControl::SetValue( double value )
{
    m_Value = value;
}

double wxSliderDoubleWithTextControl::GetValue()
{
    return m_Value;
}

void wxSliderDoubleWithTextControl::OnValueLeaveWindow( wxMouseEvent& event )
{

}

void wxSliderDoubleWithTextControl::OnValueEntered( wxCommandEvent& event )
{
    wxString valueString = m_tcValue->GetValue();
    double val;
    if( valueString.ToDouble( &val ) )
    {
        m_Value =  val;
        if( m_Value > m_MaxValue )
        {
            m_Value = m_LastCorrectValue;
        }
        if( m_Value < m_MinValue )
        {
            m_Value = m_LastCorrectValue;
        }
    }
    else
    {
        m_Value = m_LastCorrectValue;
    }
    m_LastCorrectValue = m_Value;
    UpdateControls();

    wxCommandEvent theEvent( EVT_VALUE_CHANGED );
    wxPostEvent( this, theEvent );
}

void wxSliderDoubleWithTextControl::OnSliderScroll( wxScrollEvent& event )
{
    int sliderValue = m_sldSlider->GetValue();
    m_Value = ( sliderValue*m_Range ) / m_SubDivisions;
    m_LastCorrectValue = m_Value;
    UpdateControls();
}

void wxSliderDoubleWithTextControl::OnSliderScrollChanged( wxScrollEvent& event )
{
    int sliderValue = m_sldSlider->GetValue();
    m_Value = ( sliderValue*m_Range ) / m_SubDivisions;
    m_LastCorrectValue = m_Value;
    UpdateControls();

    wxCommandEvent theEvent( EVT_VALUE_CHANGED );
    wxPostEvent( this, theEvent );
}


void wxSliderDoubleWithTextControl::CorrectValues()
{
    if( m_Value > m_MaxValue )
    {
        m_Value = m_MaxValue;
    }
    if( m_Value < m_MinValue )
    {
        m_Value = m_MinValue;
    }

    UpdateInterval();
}

void wxSliderDoubleWithTextControl::UpdateInterval()
{
    int sliderValue;
    m_Range = m_MaxValue - m_MinValue;
    sliderValue = ( m_Value*m_SubDivisions )*m_Range;
    m_sldSlider->SetMin( 0 );
    m_sldSlider->SetMax( m_SubDivisions );
    m_sldSlider->SetValue( sliderValue );
}

void wxSliderDoubleWithTextControl::UpdateControls()
{
    UpdateInterval();
    m_lblMinValue->SetLabel( wxString::Format( wxT( "%.2f" ), m_MinValue ) );
    m_lblMaxValue->SetLabel( wxString::Format( wxT( "%.2f" ), m_MaxValue ) );
    
    wxString valueString = wxString::Format( wxT( "%.2f" ), m_Value );
    m_lblCurrentValue->SetLabel( valueString );
    m_tcValue->SetValue( valueString );
}

