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
#include <wx/msw/helpchm.h> //(MS HTML Help controller)
#endif

#include <wx/generic/helpext.h> //(external HTML browser controller)
#include <wx/html/helpctrl.h> //(wxHTML based help controller: wxHtmlHelpController)

// max 20 help items (it should be sufficient)
#define MAX_HELP_ITEMS 20

int idHelpMenus[MAX_HELP_ITEMS];
int idPopupMenus[MAX_HELP_ITEMS];

CB_IMPLEMENT_PLUGIN(HelpPlugin);

BEGIN_EVENT_TABLE(HelpPlugin, cbPlugin)
  // we hook the menus dynamically
END_EVENT_TABLE()

HelpPlugin::HelpPlugin()
: m_pMenuBar(0), m_LastId(0)
{
  //ctor
  wxString resPath = ConfigManager::GetDataFolder();
  wxXmlResource::Get()->Load(resPath + _T("/help_plugin.zip#zip:*.xrc"));

  m_PluginInfo.name = _T("HelpPlugin");
  m_PluginInfo.title = _T("Help plugin");
  m_PluginInfo.version = _T("0.1");
  m_PluginInfo.description = _T("Code::Blocks Help plugin");
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
}

HelpPlugin::~HelpPlugin()
{
  //dtor
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
  if (!menu || !m_IsAttached)
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

    for (it = m_Vector.begin(); it != m_Vector.end(); ++it)
    {
      AddToPopupMenu(menu, idPopupMenus[counter++], it->first);
    }
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
  wxString ext = wxFileName(helpfile).GetExt();
  Manager::Get()->GetMessageManager()->DebugLog(_("Help File is %s"), helpfile.c_str());

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
      wxCHMHelpController HelpCtl;
      HelpCtl.Initialize(helpfile);
      HelpCtl.KeywordSearch(keyword);
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
