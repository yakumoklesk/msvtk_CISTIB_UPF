/*==============================================================================

  Library: MSVTK

  Copyright (c) Computational Image and Simulation Technologies in Biomedicine (CISTIB),
                Universitat Pompeu Fabra (UPF), Barcelona, Spain

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxMainFrame.h"

#include <vtkSmartPointer.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkVolumeMapper.h>
#include <vtkVolumeReader.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkActor.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#if( USE_WXVTK )
  #include <wxVTKRenderWindowInteractor.h>
#else
  #include <vtkRenderWindowInteractor.h>
#endif
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>

#include <vtkCommand.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkInteractorObserver.h>


#include <string>

using namespace std;

// the application icon
#ifndef __WXMSW__
    #include "mondrian.xpm"
#endif

#include "wxVolumeRenderingPanel.h"
#include "wxMiscToolsPanel.h"




// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_About
};

#define MY_FRAME      101
#define MY_VTK_WINDOW 102

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE( wxMainFrame, wxMainFrameUI )
    EVT_MENU( Minimal_Quit,  wxMainFrame::OnQuit )
    EVT_MENU( Minimal_About, wxMainFrame::OnAbout )
END_EVENT_TABLE()


// frame constructor
wxMainFrame::wxMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxMainFrameUI((wxFrame *)NULL, -1, title, pos, size)
{
#if( USE_WXVTK )
    m_pwxVTKWindowInteractor= NULL;
#endif
    m_VolumeRenderingPanel  = NULL;
    pConeMapper             = NULL;
    pConeActor              = NULL;
    pConeSource             = NULL;



#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Minimal_About;
#endif

    int language = wxLocale::GetSystemLanguage();
    int language2 = wxLANGUAGE_SPANISH;
    int language3 = wxLANGUAGE_SPANISH_MODERN;
    int language4 = wxLANGUAGE_SPANISH_US;

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));

    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    //SetStatusText(_T("Drag the mouse here! (wxWindows 2.4.0)"));
#endif // wxUSE_STATUSBAR

    CreateWindowAndGUI();

    SetSize( 1200, 600 );

//    m_pVTKWindow->DebugOn();
    //ConfigureVTK();

    ConfigureGUIData();

    Connect( wxEVT_SIZE, wxSizeEventHandler( wxMainFrame::OnSize ) );
}

wxMainFrame::~wxMainFrame()
{
    // If not, when sizer is removed, the m_pwxVTKWindowInteractor
    // inside tries to get deleted when has been already deleted
    //GetSizer()->Replace( 0, static_cast<wxSizerItem*>( NULL ) );
    //m_HorizontalSizer->Remove( 0 );

#if( USE_WXVTK )
    if( m_pwxVTKWindowInteractor )
    {
        m_pwxVTKWindowInteractor->Delete();
    }
#endif
}

#if( USE_CUSTOM_WINDOW_INTERACTOR )
void wxMainFrame::CreateWindowAndGUI()
{
    m_RenderWindowInteractorSP = vtkRenderWindowInteractorSP::New();
    m_pRendererSP = vtkRendererSP::New();
    m_pRenderWindowSP = vtkRenderWindowSP::New();

#if( USE_WXVTK )
    m_RenderWindowInteractorSP->SetRenderWindow( m_pRenderWindowSP );
#endif
    m_pRenderWindowSP->SetMultiSamples( 0 );
    m_pRenderWindowSP->AddRenderer( m_pRendererSP );

    wxButton* pButton = new wxButton( this, wxID_ANY, wxT( "Hola" ) );
    m_HorizontalSizer->Add( pButton, 0, wxSizerFlags().Expand().GetFlags() );

    wxWindow* pAtachWindow = new wxWindow( this, wxID_ANY );
    m_HorizontalSizer->Add( pAtachWindow, 0, wxSizerFlags().Expand().GetFlags() );

    m_pRenderWindowSP->SetWindowId( pAtachWindow->GetHandle() );
    m_pRenderWindowSP->SetParentId( GetParent() );

#if( USE_WXVTK )
    m_RenderWindowInteractorSP->Render();
#endif
}

void wxMainFrame::ConfigureGUIData()
{

}

#else

void MainFrame::CreateWindowAndGUI()
{
#if( USE_WXVTK )
    m_pwxVTKWindowInteractor = new wxVTKRenderWindowInteractor( this, MY_VTK_WINDOW );
    m_pwxVTKWindowInteractor->SetWindowStyle( wxFULL_REPAINT_ON_RESIZE );
    m_pwxVTKWindowInteractor->UseCaptureMouseOn();
#endif

    //FFBWidget* pFFBWidget = new FFBWidget( this, wxID_ANY );
    VolumeRenderingPanel2* pVolumeRenderingPanel2 = new VolumeRenderingPanel2( this, wxID_ANY );
    m_HorizontalSizer->Add( m_pVTKWindow, 2, wxSizerFlags().Expand().GetFlags() );
    m_HorizontalSizer->Add( pVolumeRenderingPanel2, 1, wxSizerFlags().Expand().GetFlags() );

    MiscToolsPanel* pMiscToolsPanel = new MiscToolsPanel( this, wxID_ANY );
    m_VerticalSizer->Add( pMiscToolsPanel, 0, wxSizerFlags().Expand().GetFlags() );
}

void MainFrame::ConfigureGUIData()
{
    if( pVolumeRenderingPanel2 )
    {
        pVolumeRenderingPanel2->SetCurrentVolume( m_VolumeSP );
        pVolumeRenderingPanel2->SetCurrentRenderer( m_pRenderer );
    }

    if( pMiscToolsPanel )
    {
        pMiscToolsPanel->SetCurrentAnimationScene( m_AnimationSceneSP );
        pMiscToolsPanel->SetCurrentRenderer( m_pRenderer );
    }

}

#endif



// event handlers

void wxMainFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void wxMainFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of wx-vtk sample.\n"));

    wxMessageBox(msg, _T("About wx-vtk"), wxOK | wxICON_INFORMATION, this);
}


void wxMainFrame::OnSize( wxSizeEvent& event )
{
    std::cout << "aaaaaaaaaa" << std::endl;
    wxFrame::OnSize( event );
/*
    if( m_pVTKWindow )
    {
        m_pVTKWindow->Refresh();
    }
*/
}

