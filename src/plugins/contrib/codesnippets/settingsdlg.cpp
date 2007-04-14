/*
	This file is part of Code Snippets, a plugin for Code::Blocks
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

// ----------------------------------------------------------------------------
//  SettingsDlg.cpp                                         //(pecan 2006/9/12)
// ----------------------------------------------------------------------------
#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif

#include <wx/fileconf.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>

#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "settingsdlg.h"
#include "settingsdlgform.h"
#include "version.h"

   BEGIN_EVENT_TABLE(SettingsDlg, SettingsDlgForm)
    EVT_BUTTON(wxID_OK,            SettingsDlg::OnOk)
    EVT_BUTTON(ID_EXTEDITORBUTTON, SettingsDlg::OnExtEditorButton)
    EVT_BUTTON(ID_SNIPPETFILEBUTTON, SettingsDlg::OnSnippetFileButton)
   END_EVENT_TABLE()

// ----------------------------------------------------------------------------
SettingsDlg::SettingsDlg(wxWindow* parent)
// ----------------------------------------------------------------------------
    :SettingsDlgForm(parent ,-1,wxT("User Settings"))
{
    // move dialog into the parents frame space
    //wxPoint mousePosn = ::wxGetMousePosition();
    //this->Move(mousePosn.x, mousePosn.y);
    GetConfig()->CenterChildOnParent(this);

    // Initialize the properties fields
    m_ExtEditorTextCtrl-> SetValue( wxT("Enter filename of external editor") );
    m_SnippetFileTextCtrl->SetValue(wxT("Enter filename of Snippet XML file") );

	// Put the old external editor filename into the textCtrl
    if ( not GetConfig()->SettingsExternalEditor.IsEmpty() )
        m_ExtEditorTextCtrl-> SetValue( GetConfig()->SettingsExternalEditor );

	// Put the old Snippet XML filename into the textCtrl
    if ( not GetConfig()->SettingsSnippetsXmlFullPath.IsEmpty() )
        m_SnippetFileTextCtrl-> SetValue( GetConfig()->SettingsSnippetsXmlFullPath );

}
// ----------------------------------------------------------------------------
void SettingsDlg::OnOk(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    wxString filename = m_ExtEditorTextCtrl->GetValue();
    if ( not filename.IsEmpty() )
        GetConfig()->SettingsExternalEditor = filename;
    else
        GetConfig()->SettingsExternalEditor = wxEmptyString;

    filename = m_SnippetFileTextCtrl->GetValue();
    if ( not filename.IsEmpty() )
        GetConfig()->SettingsSnippetsXmlFullPath = filename;
    else
        GetConfig()->SettingsSnippetsXmlFullPath = wxEmptyString;

    this->EndModal(wxID_OK);
    OnSettingsSave( event);
}
// ----------------------------------------------------------------------------
void SettingsDlg::OnExtEditorButton(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Ask user for filename of editor program
    wxString newFileName;
    GetFileName(newFileName);

    if (not newFileName.IsEmpty())
        m_ExtEditorTextCtrl-> SetValue( newFileName );
}
// ----------------------------------------------------------------------------
void SettingsDlg::OnSnippetFileButton(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Ask user for filename of editor program
    wxString newFileName;
    newFileName = AskForPathName();

    if (not newFileName.IsEmpty())
        m_SnippetFileTextCtrl-> SetValue( newFileName+wxFILE_SEP_PATH+wxT("sniplist.xml") );
}
// ----------------------------------------------------------------------------
void SettingsDlg::GetFileName(wxString& newFileName)
// ----------------------------------------------------------------------------
{
    newFileName = wxEmptyString;

    // Ask user for filename
    wxFileDialog dlg(this,                      //parent  window
                 _T("Select file "),             //message
                 wxEmptyString,                 //default directory
                 wxEmptyString,                 //default file
                 wxT("*.*"),                    //wildcards
                 wxOPEN | wxFILE_MUST_EXIST );  //style

   // move dialog into the parents frame space
    wxPoint mousePosn = ::wxGetMousePosition();
    (&dlg)->Move(mousePosn.x, mousePosn.y);

    if (dlg.ShowModal() != wxID_OK) return;
    newFileName = dlg.GetPath();

    #ifdef LOGGING
     LOGIT( _T("New filename[%s]"), newFileName.GetData() );
    #endif //LOGGING;

}
// ----------------------------------------------------------------------------
wxString SettingsDlg::AskForPathName()       //(pecan 2006/10/06)
// ----------------------------------------------------------------------------
{
    wxString newPathName = wxEmptyString;

    // Ask user for filename
    wxDirDialog dlg(::wxGetTopLevelParent(0),   //parent  window
                 _T("Select path "),             //message
                 ::wxGetCwd(),                  //default directory
                 wxDD_DEFAULT_STYLE );          //style

   // move dialog into the parents frame space
    wxPoint mousePosn = ::wxGetMousePosition();
    (&dlg)->Move(mousePosn.x, mousePosn.y);

    if (dlg.ShowModal() != wxID_OK) return wxEmptyString;
    return newPathName = dlg.GetPath();
}
// ----------------------------------------------------------------------------
void SettingsDlg::OnSettingsSave(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
        // file will be saved in $HOME/sniplist.ini

    wxFileConfig cfgFile(wxEmptyString, // appname
                        wxEmptyString,      // vendor
                        GetConfig()->SettingsSnippetsCfgFullPath,    // local filename
                        wxEmptyString,                  // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Write( wxT("ExternalEditor"),  m_ExtEditorTextCtrl->GetValue() );
	cfgFile.Write( wxT("SnippetFile"),     m_SnippetFileTextCtrl->GetValue() );
	cfgFile.Flush();
}
// ----------------------------------------------------------------------------
SettingsDlg::~SettingsDlg()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------



