///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WXSLIDERDOUBLE__
#define __WXSLIDERDOUBLE__

#include "wxSliderDoubleUI.h"

#include <string>
#include "wx/string.h"

DECLARE_LOCAL_EVENT_TYPE( EVT_VALUE_CHANGED, wxID_ANY )

///////////////////////////////////////////////////////////////////////////////
/// Class wxSliderDoubleWithTextControl
///////////////////////////////////////////////////////////////////////////////
class wxSliderDoubleWithTextControl : public wxSliderDoubleWithTextControlUI
{
public:

    wxSliderDoubleWithTextControl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 798,57 ), long style = wxTAB_TRAVERSAL );
    ~wxSliderDoubleWithTextControl();

    void SetLabel( const char* label );
    void SetLabel( std::string& label );
    void SetLabel( wxString& label );
    std::string GetLabel();
    //wxString GetLabel();
    void SetResolution( int subdivisions );
    int GetResolution();
    void SetMinValue( double minValue );
    double GetMinValue();
    void SetMaxValue( double maxValue );
    double GetMaxValue();
    void SetDefaultValue( double defautlValue );
    double GetDefaultValue();
    void SetValue( double value );
    double GetValue();


    // Virtual event handlers, overide them in your derived class
    virtual void OnValueLeaveWindow( wxMouseEvent& event );
    virtual void OnValueEntered( wxCommandEvent& event );
    virtual void OnSliderScroll( wxScrollEvent& event );
    virtual void OnSliderScrollChanged( wxScrollEvent& event );

private:
    void CorrectValues();
    void UpdateInterval();
//    void UpdateSliderValues();
    void UpdateControls();

private:
    std::string m_LabelValue;
    int m_SubDivisions;
    double m_MinValue;
    double m_MaxValue;
    double m_Range;
    double m_RangeInverse;
    double m_DefaultValue;
    double m_LastCorrectValue;
    double m_Value;
};

#endif //__WXSLIDERDOUBLE__
