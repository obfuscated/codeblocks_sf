/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: editsnippetdlg.cpp 32 2007-04-02 17:51:48Z Pecan $

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
    #include <wx/textctrl.h>
    #include <wx/sizer.h>
#endif

#include <wx/fileconf.h>
#include <wx/dnd.h>

#if defined(BUILDING_PLUGIN)
    #include "sdk.h"
    #ifndef CB_PRECOMP
        #include "manager.h"
        #include "configmanager.h"
    #endif
#endif //defined(BUILDING_PLUGIN)


#include "codesnippetswindow.h"
#include "editsnippetdlg.h"
#include "snippetsconfig.h"
#include "edit.h"
#include "version.h"

BEGIN_EVENT_TABLE(EditSnippetDlg, wxDialog)
	EVT_BUTTON(wxID_OK,     EditSnippetDlg::OnOK)
	EVT_BUTTON(wxID_CANCEL, EditSnippetDlg::OnCancel)
	EVT_BUTTON(wxID_HELP,   EditSnippetDlg::OnHelp)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
class EditSnippetDropTarget : public wxTextDropTarget
// ----------------------------------------------------------------------------
{
    // Drop target used to place dragged data into Properties dialog

	public:
		EditSnippetDropTarget(EditSnippetDlg* window) : m_Window(window) {}
		~EditSnippetDropTarget() {}
		bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	private:
		EditSnippetDlg* m_Window;
};
// ----------------------------------------------------------------------------
bool EditSnippetDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
// ----------------------------------------------------------------------------
{
    // Put dragged text into SnippetTextCtrl
    #ifdef LOGGING
     LOGIT( _T("Dragged Data[%s]"), data.GetData() );
    #endif //LOGGING
    //m_Window->m_SnippetEditCtrl->WriteText(data);
    m_Window->m_SnippetEditCtrl->AddText(data);
    return true;

} // end of OnDropText

// ----------------------------------------------------------------------------
EditSnippetDlg::EditSnippetDlg(const wxString& snippetName, const wxString& snippetText,
                            wxSemaphore* pWaitSem, wxString fileName)
// ----------------------------------------------------------------------------
	//-: wxDialog(Manager::Get()->GetAppWindow(), wxID_ANY, _("Edit snippet"),
	//: wxDialog( GetConfig()->pMainFrame, wxID_ANY, _T("Edit snippet"),
	: wxDialog( GetConfig()->GetSnippetsWindow(), wxID_ANY, _T("Edit snippet"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER)
{
    pWaitingSemaphore = pWaitSem;
    m_EditFileName = fileName;

	InitDlg();

	m_SnippetNameCtrl->SetValue(snippetName);
	//m_SnippetTextCtrl->SetValue(snippetText);
	m_SnippetEditCtrl->SetText(snippetText);

	if (not fileName.IsEmpty())
                m_SnippetEditCtrl->LoadFile(fileName);

	// Load the window's size
    //	ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
    //	SetSize(cfgMan->ReadInt(_T("editdlg_w"), 500), cfgMan->ReadInt(_T("editdlg_h"), 400));
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        GetConfig()->SettingsSnippetsCfgFullPath,      // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

    cfgFile.Read( wxT("EditDlgXpos"),       &GetConfig()->nEditDlgXpos,20);
    cfgFile.Read( wxT("EditDlgYpos"),       &GetConfig()->nEditDlgYpos,20);
	cfgFile.Read( wxT("EditDlgWidth"),      &GetConfig()->nEditDlgWidth, 500 ) ;
	cfgFile.Read( wxT("EditDlgHeight"),     &GetConfig()->nEditDlgHeight, 400 ) ;
	cfgFile.Read( wxT("EditDlgMaximized"),  &GetConfig()->bEditDlgMaximized, false );
	//SetSize(GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight);
    LOGIT( _T("EditDlgPositinIN X[%d]Y[%d]Width[%d]Height[%d]"),
        GetConfig()->nEditDlgXpos,GetConfig()->nEditDlgYpos,
        GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight );
    SetSize(GetConfig()->nEditDlgXpos, GetConfig()->nEditDlgYpos, GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight);

//	if (( GetConfig()->IsPlugin()) && (GetConfig()->bEditDlgMaximized) )
//		Maximize(true);

	SetDropTarget(new EditSnippetDropTarget(this));
	m_SnippetEditCtrl->SetFocus();
}

// ----------------------------------------------------------------------------
EditSnippetDlg::~EditSnippetDlg()
// ----------------------------------------------------------------------------
{}
// ----------------------------------------------------------------------------
void EditSnippetDlg::EndSnippetDlg(int wxID_OKorCANCEL)
// ----------------------------------------------------------------------------
{
    // Called from EndModal() OnOk/OnCancel routines

	// Save the window's size
	//ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        GetConfig()->SettingsSnippetsCfgFullPath,      // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);


	//if (!IsMaximized())
	//{
		//wxSize windowSize = GetSize();
        //	cfgMan->Write(_T("editdlg_w"), windowSize.GetWidth());
        //	cfgMan->Write(_T("editdlg_h"), windowSize.GetHeight());
        int x,y,w,h;
        GetPosition(&x,&y); GetSize(&w,&h);
        cfgFile.Write( wxT("EditDlgXpos"),  x );
        cfgFile.Write( wxT("EditDlgYpos"),  y );
        cfgFile.Write( wxT("EditDlgWidth"),  w );
        cfgFile.Write( wxT("EditDlgHeight"), h );
         LOGIT( _T("EditDlgPositinOUT X[%d]Y[%d]Width[%d]Height[%d]"),x,y,w,h );

		//  cfgMan->Write(_T("editdlg_maximized"), false);
        cfgFile.Write( wxT("EditDlgMaximized"),  false );
	//}
	//else
	//{
	//	//  cfgMan->Write(_T("editdlg_maximized"), true);
	//	if (GetConfig()->IsPlugin())
    //       cfgFile.Write( wxT("EditDlgMaximized"),  true );
	//}

    // If this was an external file, save it
    if ( (not m_EditFileName.IsEmpty()) && (wxID_OKorCANCEL == wxID_OK) )
    {
        if (m_SnippetEditCtrl->GetModify())
            m_SnippetEditCtrl->SaveFile(m_EditFileName);
    }

    // If parent is waiting on us, post we're finished
	if (pWaitingSemaphore)
        pWaitingSemaphore->Post();
}

// ----------------------------------------------------------------------------
void EditSnippetDlg::InitDlg()
// ----------------------------------------------------------------------------
{
	wxBoxSizer* dlgSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* snippetDataSizer = new wxBoxSizer(wxVERTICAL);

	m_NameLbl = new wxStaticText(this, wxID_ANY, _T("&Name:"), wxDefaultPosition, wxDefaultSize, 0);
	snippetDataSizer->Add( m_NameLbl, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_SnippetNameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	snippetDataSizer->Add(m_SnippetNameCtrl, 0, wxEXPAND|wxALL, 5 );

	m_SnippetLbl = new wxStaticText(this, wxID_ANY, _T("&Snippet:"), wxDefaultPosition, wxDefaultSize, 0);
	snippetDataSizer->Add(m_SnippetLbl, 0, wxTOP|wxRIGHT|wxLEFT, 5);

	//-m_SnippetTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_DONTWRAP|wxTE_PROCESS_TAB);
	m_SnippetEditCtrl = new Edit(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_DONTWRAP|wxTE_PROCESS_TAB);
	//-snippetDataSizer->Add(m_SnippetTextCtrl, 1, wxALL|wxEXPAND, 5);
	snippetDataSizer->Add(m_SnippetEditCtrl, 1, wxALL|wxEXPAND, 5);

	dlgSizer->Add(snippetDataSizer, 1, wxEXPAND, 5);

	wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	m_OKBtn = new wxButton(this, wxID_OK, _T("&OK"), wxDefaultPosition, wxDefaultSize, 0);
	m_CancelBtn = new wxButton(this, wxID_CANCEL, _T("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	if (GetConfig()->IsPlugin())
        m_HelpBtn = new wxButton(this, wxID_HELP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);

   #ifdef __WXGTK__
        if (GetConfig()->IsPlugin())
        {
            wxBoxSizer* helpButtonSizer = new wxBoxSizer(wxHORIZONTAL);
            helpButtonSizer->Add(m_HelpBtn, 0, wxALL, 5);
            bottomSizer->Add(helpButtonSizer, 0, wxEXPAND, 5);
        }

        buttonsSizer->Add(m_CancelBtn, 0, wxALL, 5);
        buttonsSizer->Add(m_OKBtn, 0, wxALL, 5);

        bottomSizer->Add(buttonsSizer, 1, wxALIGN_RIGHT|wxSHAPED|wxEXPAND, 5);
   #else //not WXGTK
        buttonsSizer->Add(m_OKBtn, 0, wxALL, 5);
        buttonsSizer->Add(m_CancelBtn, 0, wxALL, 5);
        if (GetConfig()->IsPlugin())
            buttonsSizer->Add(m_HelpBtn, 0, wxALL, 5);

        //-bottomSizer->Add(buttonsSizer, 1, wxALIGN_RIGHT|wxSHAPED|wxEXPAND, 5);
        bottomSizer->Add(buttonsSizer, 1, wxALIGN_CENTER_HORIZONTAL|wxSHAPED|wxEXPAND, 5);
   #endif

	dlgSizer->Add(bottomSizer, 0, wxEXPAND, 5);

	SetSizer(dlgSizer);
	Layout();

    // Place window over parent window
    GetConfig()->GetSnippetsWindow()->CenterChildOnParent(this);

}

// ----------------------------------------------------------------------------
wxString EditSnippetDlg::GetName()
// ----------------------------------------------------------------------------
{
	return m_SnippetNameCtrl->GetValue();
}

// ----------------------------------------------------------------------------
wxString EditSnippetDlg::GetText()
// ----------------------------------------------------------------------------
{
	//return m_SnippetTextCtrl->GetValue();
	return m_SnippetEditCtrl->GetText();
}

// ----------------------------------------------------------------------------
void EditSnippetDlg::OnOK(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    EndSnippetDlg(wxID_OK);
	EndModal(wxID_OK);
}

// ----------------------------------------------------------------------------
void EditSnippetDlg::OnCancel(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    EndSnippetDlg(wxID_CANCEL);
	EndModal(wxID_CANCEL);
}

// ----------------------------------------------------------------------------
void EditSnippetDlg::OnHelp(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	// Link to the Wiki which contains information about the available macros
	wxLaunchDefaultBrowser(_T("http://wiki.codeblocks.org/index.php?title=Builtin_variables"));
}

