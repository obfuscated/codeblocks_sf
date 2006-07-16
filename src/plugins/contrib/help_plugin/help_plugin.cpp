/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "help_plugin.h"

#include <wx/process.h>
#include <wx/intl.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <manager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <messagemanager.h>
#include <projectmanager.h>
#include <cbeditor.h>
#include <cbproject.h>
#include <licenses.h>

#include <wx/help.h> //(wxWindows chooses the appropriate help controller class)
#include <wx/helpbase.h> //(wxHelpControllerBase class)
#include <wx/helpwin.h> //(Windows Help controller)

#ifdef __WXMSW__
#include <wx/msw/helpchm.h> // used in case we fail to load the OCX module (it could fail too)
#include <wx/thread.h>
#endif

#include <wx/generic/helpext.h> //(external HTML browser controller)
#include <wx/html/helpctrl.h> //(wxHTML based help controller: wxHtmlHelpController)

// 20 wasn't enough
#define MAX_HELP_ITEMS 32

int idHelpMenus[MAX_HELP_ITEMS];
int idPopupMenus[MAX_HELP_ITEMS];

CB_IMPLEMENT_PLUGIN(HelpPlugin, "Help plugin");

BEGIN_EVENT_TABLE(HelpPlugin, cbPlugin)
  // we hook the menus dynamically
END_EVENT_TABLE()

#ifdef __WXMSW__
namespace
{
#ifndef UNICODE
  typedef HWND (WINAPI *HTMLHELP)(HWND, LPCSTR, UINT, DWORD);
  #define HTMLHELP_NAME "HtmlHelpA"
#else // ANSI
  typedef HWND (WINAPI *HTMLHELP)(HWND, LPCWSTR, UINT, DWORD);
  #define HTMLHELP_NAME "HtmlHelpW"
#endif

  // ocx symbol handle
  HTMLHELP fp_htmlHelp = 0;
  HMODULE ocx_module = 0;

  // it's used to search by keyword
  struct cbHH_AKLINK
  {
    int      cbStruct;
    BOOL     fReserved;
    LPCTSTR  pszKeywords;
    LPCTSTR  pszUrl;
    LPCTSTR  pszMsgText;
    LPCTSTR  pszMsgTitle;
    LPCTSTR  pszWindow;
    BOOL     fIndexOnFail;
  };

  // the command to search by keyword
  const UINT cbHH_KEYWORD_LOOKUP = 0x000D;

  // This little class helps to fix a problem when the help file is CHM and the
  // keyword throws many results
  class LaunchCHMThread : public wxThread
  {
    private:
      wxCHMHelpController m_helpctl;
      wxString m_filename;
      wxString m_keyword;

    public:
      LaunchCHMThread(const wxString &file, const wxString &keyword);
      ExitCode Entry();
  };

  LaunchCHMThread::LaunchCHMThread(const wxString &file, const wxString &keyword)
  : m_filename(file), m_keyword(keyword)
  {
    m_helpctl.Initialize(file);
  }

  wxThread::ExitCode LaunchCHMThread::Entry()
  {
    if (fp_htmlHelp) // do it our way if we can
    {
      cbHH_AKLINK link;

      link.cbStruct =     sizeof(cbHH_AKLINK);
      link.fReserved =    FALSE;
      link.pszKeywords =  m_keyword.c_str();
      link.pszUrl =       NULL;
      link.pszMsgText =   NULL;
      link.pszMsgTitle =  NULL;
      link.pszWindow =    NULL;
      link.fIndexOnFail = TRUE;

      fp_htmlHelp(0L, (const wxChar*)m_filename, cbHH_KEYWORD_LOOKUP, (DWORD)&link);
    }
    else // do it the wx way then (which is the same thing, except for the 0L in the call to fp_htmlHelp)
    {
      m_helpctl.KeywordSearch(m_keyword);
    }

    return 0;
  }
}
#endif

HelpPlugin::HelpPlugin()
: m_pMenuBar(0), m_LastId(0)
{
  //ctor
    if(!Manager::LoadResource(_T("help_plugin.zip")))
    {
        NotifyMissingFile(_T("help_plugin.zip"));
    }

  m_PluginInfo.name = _T("HelpPlugin");
  m_PluginInfo.title = _T("Help plugin");
  m_PluginInfo.version = _T("1.0");
  m_PluginInfo.description = _T("This plugin is used to add a list of help files to the Help menu ")
                             _T("so you can have them handy to launch.\n")
                             _T("\n")
                             _T("You can also set one of the help files as the \"default help file\" ")
                             _T("and that way it'll be launched when you press F1.\n")
                             _T("NOTE: you can add any file you want and it'll be launched with the ")
                             _T("associated application.\n")
                             _T("\n")
                             _T("It'll also integrate in the context menu of the editor (right click) ")
                             _T("enabling you to search the word under the cursor in any of the help ")
                             _T("files.\n")
                             _T("NOTE: it's only meaningful under Windows for help files with extension ")
                             _T("hlp or chm. It's also meaningul for any platform when the help file is ")
                             _T("an URL (read below).\n")
                             _T("\n")
                             _T("A recent addition allows you to add an URL as a help file. To do that ")
                             _T("Add a new help file BUT when prompted for the location of the file ")
                             _T("dismiss the file dialog, be sure the new help file is selected and ")
                             _T("enter the URL in the text entry (the one followed by the ... button).\n")
                             _T("\n")
                             _T("For URLs you can add $(keyword) in any place of the address, any ")
                             _T("number of times, and it'll get replaced by the word you selected to ")
                             _T("locate in that help file.\n")
                             _T("\n")
                             _T("The configuration dialog for this plugin can be found clicking on ")
                             _T("Settings -> Environment (Help files).");
  m_PluginInfo.author = _T("Bourricot | Ceniza (maintainer)");
  m_PluginInfo.authorEmail = _T("titi37fr@yahoo.fr | ceniza@gda.utp.edu.co");
  m_PluginInfo.authorWebsite = _T("www.codeblocks.org");
  m_PluginInfo.thanksTo = _T("Codeblocks dev team !\nBourricot for the initial version");
  m_PluginInfo.license = LICENSE_GPL;

  // initialize IDs for Help and popup menu
  for (int i = 0; i < MAX_HELP_ITEMS; ++i)
  {
    idHelpMenus[i] = wxNewId();
    idPopupMenus[i] = wxNewId();

    // dynamically connect the events
    Connect(idHelpMenus[i], -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &HelpPlugin::OnHelp);
    Connect(idPopupMenus[i], -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &HelpPlugin::OnFindItem);
  }

  m_LastId = idHelpMenus[0];

#ifdef __WXMSW__
  ocx_module = LoadLibrary(_T("HHCTRL.OCX"));

  if (ocx_module)
  {
    fp_htmlHelp = (HTMLHELP)GetProcAddress(ocx_module, HTMLHELP_NAME);
  }
#endif
}

HelpPlugin::~HelpPlugin()
{
#ifdef __WXMSW__
  if (ocx_module)
  {
    FreeLibrary(ocx_module);
  }
#endif
}

void HelpPlugin::OnAttach()
{
  // load configuration (only saved in our config dialog)
  HelpCommon::LoadHelpFilesVector(m_Vector);
}

cbConfigurationPanel* HelpPlugin::GetConfigurationPanel(wxWindow* parent)
{
  return new HelpConfigDialog(parent, this);
}

void HelpPlugin::Reload()
{
    // remove entries from help menu
    int counter = m_LastId - idHelpMenus[0];
    HelpCommon::HelpFilesVector::iterator it;

    for (it = m_Vector.begin(); it != m_Vector.end(); ++it)
    {
      RemoveFromHelpMenu(idHelpMenus[--counter], it->first);
    }

    // reload configuration (saved in the config dialog)
    HelpCommon::LoadHelpFilesVector(m_Vector);
    BuildMenu(m_pMenuBar);
}

void HelpPlugin::OnRelease(bool appShutDown)
{
	//empty
}

void HelpPlugin::BuildMenu(wxMenuBar *menuBar)
{
  if (!m_IsAttached)
  {
    return;
  }

  m_pMenuBar = menuBar;

  // add entries in help menu
  int counter = 0;
  HelpCommon::HelpFilesVector::iterator it;

  for (it = m_Vector.begin(); it != m_Vector.end(); ++it, ++counter)
  {
    if (counter == HelpCommon::getDefaultHelpIndex())
    {
      AddToHelpMenu(idHelpMenus[counter], it->first + _T("\tF1"));
    }
    else
    {
      AddToHelpMenu(idHelpMenus[counter], it->first);
    }
  }

  m_LastId = idHelpMenus[0] + counter;
}

void HelpPlugin::BuildModuleMenu(const ModuleType type, wxMenu *menu, const FileTreeData* data)
{
  if (!menu || !m_IsAttached || !m_Vector.size())
  {
    return;
  }

  if (type == mtEditorManager)
  {
    if (m_Vector.size() != 0)
    {
      menu->AppendSeparator();
    }

    // add entries in popup menu
    int counter = 0;
    HelpCommon::HelpFilesVector::iterator it;
    wxMenu *sub_menu = new wxMenu;

    for (it = m_Vector.begin(); it != m_Vector.end(); ++it)
    {
      AddToPopupMenu(sub_menu, idPopupMenus[counter++], it->first);
    }

    wxMenuItem *locate_in_menu = new wxMenuItem(0, wxID_ANY, _("&Locate in"), _(""), wxITEM_NORMAL);
    locate_in_menu->SetSubMenu(sub_menu);

    menu->Append(locate_in_menu);
  }
}

bool HelpPlugin::BuildToolBar(wxToolBar *toolBar)
{
	return false;
}

void HelpPlugin::AddToHelpMenu(int id, const wxString &help)
{
  if (!m_pMenuBar)
  {
    return;
  }

  int pos = m_pMenuBar->FindMenu(_("Help"));

  if (pos != wxNOT_FOUND)
  {
    wxMenu *helpMenu = m_pMenuBar->GetMenu(pos);

    if (id == idHelpMenus[0])
    {
      helpMenu->AppendSeparator();
    }

    helpMenu->Append(id, help);
  }
}

void HelpPlugin::RemoveFromHelpMenu(int id, const wxString &help)
{
  if (!m_pMenuBar)
  {
    return;
  }

  int pos = m_pMenuBar->FindMenu(_("Help"));

  if (pos != wxNOT_FOUND)
  {
    wxMenu *helpMenu = m_pMenuBar->GetMenu(pos);
    wxMenuItem *mi = helpMenu->Remove(id);

    if (id)
    {
      delete mi;
    }

    // remove separator too (if it's the last thing left)
    mi = helpMenu->FindItemByPosition(helpMenu->GetMenuItemCount() - 1);

    if (mi && (mi->GetKind() == wxITEM_SEPARATOR || mi->GetText().IsEmpty()))
    {
      helpMenu->Remove(mi);
      delete mi;
    }
  }
}

void HelpPlugin::AddToPopupMenu(wxMenu *menu, int id, const wxString &help)
{
  wxString tmp;

  if (!help.IsEmpty())
  {
    tmp.Append(_("Locate in "));
    tmp.Append(help);
    menu->Append(id, tmp);
  }
}

wxString HelpPlugin::HelpFileFromId(int id)
{
  int counter = 0;
  HelpCommon::HelpFilesVector::iterator it;

  for (it = m_Vector.begin(); it != m_Vector.end(); ++it, ++counter)
  {
    if (idHelpMenus[counter] == id || idPopupMenus[counter] == id)
    {
      return it->second;
    }
  }

  return wxEmptyString;
}

void HelpPlugin::LaunchHelp(const wxString &helpfile, const wxString &keyword)
{
  const static wxString http_prefix(_T("http://"));

  if (helpfile.Mid(0, http_prefix.size()).CmpNoCase(http_prefix) == 0)
  {
    wxString the_url = helpfile;
    the_url.Replace(_T("$(keyword)"), keyword);
    Manager::Get()->GetMessageManager()->DebugLog(_T("Launching %s"), the_url.c_str());

    wxLaunchDefaultBrowser(the_url);
    return;
  }

  wxString ext = wxFileName(helpfile).GetExt();
  Manager::Get()->GetMessageManager()->DebugLog(_T("Help File is %s"), helpfile.c_str());

  if (!keyword.IsEmpty())
  {
#ifdef __WXMSW__
  	if (ext.CmpNoCase(_T("hlp")) == 0)
  	{
      wxWinHelpController HelpCtl;
      HelpCtl.Initialize(helpfile);
      HelpCtl.KeywordSearch(keyword);
      return;
  	}

  	if (ext.CmpNoCase(_T("chm")) == 0)
  	{
      LaunchCHMThread *p_thread = new LaunchCHMThread(helpfile, keyword);
      p_thread->Create();
      p_thread->Run();
      return;
  	}
#endif
  }

  // Just call it with the associated program
  wxFileType *filetype = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);

  if (!filetype)
  {
    wxMessageBox(_("Couldn't find an associated program to open ") + wxFileName(helpfile).GetFullName(), _("Warning"), wxOK | wxICON_EXCLAMATION);
    return;
  }

  wxExecute(filetype->GetOpenCommand(helpfile));
  delete filetype;
}

// events
void HelpPlugin::OnHelp(wxCommandEvent &event)
{
  int id = event.GetId();
  wxString help = HelpFileFromId(id);
  LaunchHelp(help);
}

void HelpPlugin::OnFindItem(wxCommandEvent &event)
{
  cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

  if (!ed)
  {
    return;
  }

  cbStyledTextCtrl *control = ed->GetControl();
  wxString text = control->GetSelectedText();

  if (text.IsEmpty())
  {
    int origPos = control->GetCurrentPos();
    int start = control->WordStartPosition(origPos, true);
    int end = control->WordEndPosition(origPos, true);
    text = control->GetTextRange(start, end);
  }

  int id = event.GetId();
  wxString help = HelpFileFromId(id);
  LaunchHelp(help, text);
}
