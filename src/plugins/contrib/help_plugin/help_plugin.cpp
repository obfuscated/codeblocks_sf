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
#include "man2html.h"
#include "bzip2/bzlib.h"
#include "zlib/zlib.h"

#include <wx/process.h>
#include <wx/intl.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <globals.h> // cbMessageBox
#include <manager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <messagemanager.h>
#include <projectmanager.h>
#include <cbeditor.h>
#include <cbproject.h>
#include <macrosmanager.h>
#include <vector>

#include <wx/help.h> //(wxWindows chooses the appropriate help controller class)
#include <wx/helpbase.h> //(wxHelpControllerBase class)
#include <wx/helpwin.h> //(Windows Help controller)

#ifdef __WXMSW__
#include <wx/msw/helpchm.h> // used in case we fail to load the OCX module (it could fail too)
#include <wx/thread.h>
#endif

#include <wx/generic/helpext.h> //(external HTML browser controller)
#include <wx/html/helpctrl.h> //(wxHTML based help controller: wxHtmlHelpController)
#include <wx/wxhtml.h> //(man pages)

// 20 wasn't enough
#define MAX_HELP_ITEMS 32

int idHelpMenus[MAX_HELP_ITEMS];

// Register the plugin
namespace
{
    PluginRegistrant<HelpPlugin> reg(_T("HelpPlugin"));

    class MANFrame : public wxFrame
    {
        private:
            wxHtmlWindow *m_htmlWindow;

        public:
            MANFrame(wxWindow *parent = 0, wxWindowID id = wxID_ANY, const wxString &title = wxEmptyString)
            : wxFrame(parent, id, title)
            {
                m_htmlWindow = new wxHtmlWindow(this);
            }

            void SetPage(const wxString &contents)
            {
                m_htmlWindow->SetPage(contents);
            }

            void OnClose(wxCloseEvent &event)
            {
                Show(false);
                event.Veto(true);
            }

        DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(MANFrame, wxFrame)
        EVT_CLOSE(MANFrame::OnClose)
    END_EVENT_TABLE()
};

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

      link.cbStruct     = sizeof(cbHH_AKLINK);
      link.fReserved    = FALSE;
      link.pszKeywords  = m_keyword.c_str();
      link.pszUrl       = NULL;
      link.pszMsgText   = NULL;
      link.pszMsgTitle  = NULL;
      link.pszWindow    = NULL;
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
: m_pMenuBar(0), m_LastId(0), m_manFrame(0)
{
  //ctor
    if(!Manager::LoadResource(_T("help_plugin.zip")))
    {
        NotifyMissingFile(_T("help_plugin.zip"));
    }

  // initialize IDs for Help and popup menu
  for (int i = 0; i < MAX_HELP_ITEMS; ++i)
  {
    idHelpMenus[i] = wxNewId();

    // dynamically connect the events
    Connect(idHelpMenus[i], -1, wxEVT_COMMAND_MENU_SELECTED,
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

  if (m_manFrame)
  {
    m_manFrame->Destroy();
  }
}

void HelpPlugin::OnAttach()
{
  // load configuration (only saved in our config dialog)
  HelpCommon::LoadHelpFilesVector(m_Vector);

  if (!m_manFrame)
  {
    m_manFrame = new MANFrame(0, wxID_ANY, wxEmptyString);
  }
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
	m_manFrame->Destroy();
	m_manFrame = 0;
}

void HelpPlugin::BuildMenu(wxMenuBar *menuBar)
{
  if (!IsAttached())
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
  if (!menu || !IsAttached() || !m_Vector.size())
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
      AddToPopupMenu(sub_menu, idHelpMenus[counter++], it->first);
    }

    wxMenuItem *locate_in_menu = new wxMenuItem(0, wxID_ANY, _("&Locate in"), _T(""), wxITEM_NORMAL);
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

HelpCommon::HelpFileAttrib HelpPlugin::HelpFileFromId(int id)
{
  int counter = 0;
  HelpCommon::HelpFilesVector::iterator it;

  for (it = m_Vector.begin(); it != m_Vector.end(); ++it, ++counter)
  {
    if (idHelpMenus[counter] == id)
    {
      return it->second;
    }
  }

  return HelpCommon::HelpFileAttrib();
}

void HelpPlugin::LaunchHelp(const wxString &c_helpfile, bool isExecutable, const wxString &keyword)
{
  const static wxString http_prefix(_T("http://"));
  const static wxString man_prefix(_T("man:"));
  wxString helpfile(c_helpfile);

  helpfile.Replace(_T("$(keyword)"), keyword);
  Manager::Get()->GetMacrosManager()->ReplaceMacros(helpfile);

  if (isExecutable)
  {
    Manager::Get()->GetMessageManager()->DebugLog(_T("Executing %s"), helpfile.c_str());
    wxExecute(helpfile);
    return;
  }

  // Operate on help http (web) links
  if (helpfile.Mid(0, http_prefix.size()).CmpNoCase(http_prefix) == 0)
  {
    Manager::Get()->GetMessageManager()->DebugLog(_T("Launching %s"), helpfile.c_str());
    wxLaunchDefaultBrowser(helpfile);
    return;
  }

  // Operate on man pages
  if (helpfile.Mid(0, man_prefix.size()).CmpNoCase(man_prefix) == 0)
  {
    if (keyword.IsEmpty())
    {
        return;
    }

    wxString man_page = GetManPage(c_helpfile, keyword);

    if (man_page.IsEmpty())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("Couldn't find man page"));
        return;
    }

    Manager::Get()->GetMessageManager()->DebugLog(_T("Launching man page"));
    reinterpret_cast<MANFrame *>(m_manFrame)->SetPage(cbC2U(man2html_buffer(man_page.mb_str())));
    m_manFrame->SetTitle(keyword);
    m_manFrame->Show(true);
    return;
  }

  wxFileName the_helpfile = wxFileName(helpfile);
  Manager::Get()->GetMessageManager()->DebugLog(_T("Help File is %s"), helpfile.c_str());

  if (!(the_helpfile.FileExists()))
  {
    wxString msg;
    msg << _("Couldn't find the help file:\n")
        << the_helpfile.GetFullPath() << _("\n")
        << _("Do you want to run the associated program anyway?");
    if (!(cbMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO | wxNO_DEFAULT) == wxID_YES));
        return;
  }

  wxString ext = the_helpfile.GetExt();

#ifdef __WXMSW__
  // Operate on help files with keyword search (windows only)
  if (!keyword.IsEmpty())
  {
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
  }
#endif

  // Just call it with the associated program
  wxFileType *filetype = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);

  if (!filetype)
  {
    cbMessageBox(_("Couldn't find an associated program to open:\n") +
      the_helpfile.GetFullPath(), _("Warning"), wxOK | wxICON_EXCLAMATION);
    return;
  }

  wxExecute(filetype->GetOpenCommand(helpfile));
  delete filetype;
}

void HelpPlugin::OnFindItem(wxCommandEvent &event)
{
  wxString text; // save here the word to lookup... if any
  cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

  if (ed)
  {
    cbStyledTextCtrl *control = ed->GetControl();
    text = control->GetSelectedText();

    if (text.IsEmpty())
    {
      int origPos = control->GetCurrentPos();
      int start = control->WordStartPosition(origPos, true);
      int end = control->WordEndPosition(origPos, true);
      text = control->GetTextRange(start, end);
    }
  }

  int id = event.GetId();
  HelpCommon::HelpFileAttrib hfa = HelpFileFromId(id);
  LaunchHelp(hfa.name, hfa.isExecutable, text);
}

bool HelpPlugin::Decompress(const wxString& filename, const wxString& tmpfile)
{
    // open file
    FILE* f = fopen(filename.mb_str(), "rb");
    if (!f)
    {
        return false;
    }

    // open BZIP2 stream
    int bzerror;
    BZFILE* bz = BZ2_bzReadOpen(&bzerror, f, 0, 0, 0L, 0);
    if (!bz || bzerror != BZ_OK)
    {
        fclose(f);
        return false;
    }

    // open output file
    FILE* fo = fopen(tmpfile.mb_str(), "wb");
    if (!fo)
    {
        fclose(f);
        return false;
    }

    // read stream writing to uncompressed file
    char buffer[2048];
    while (bzerror != BZ_STREAM_END)
    {
        BZ2_bzRead(&bzerror, bz, buffer, 2048);
        if (bzerror != BZ_OK && bzerror != BZ_STREAM_END)
        {
            BZ2_bzReadClose(&bzerror, bz);
            fclose(fo);
            fclose(f);
            return false;
        }
        fwrite(buffer, 2048, 1, fo);
    }

    BZ2_bzReadClose(&bzerror, bz);


    fclose(fo);
    fclose(f);
    return true;
}

wxString HelpPlugin::GetManPage(const wxString &dirs, const wxString &keyword)
{
    std::vector<wxString> dirs_vect;
    size_t start_pos = 4; // len("man:")

    while (true)
    {
        size_t next_semi = dirs.find(_T(';'), start_pos);

        if ((int)next_semi == wxNOT_FOUND)
        {
            next_semi = dirs.Length();
        }

        dirs_vect.push_back(dirs.SubString(start_pos, next_semi - 1));

        if (next_semi == dirs.Length())
        {
            break;
        }

        start_pos = next_semi + 1;
    }

    if (dirs_vect.empty())
    {
        return wxString();
    }

    for (std::vector<wxString>::iterator i = dirs_vect.begin(); i != dirs_vect.end(); ++i)
    {
        wxString filename = wxDir::FindFirst(*i, keyword + _T(".*"));

        if (filename.IsEmpty())
        {
            continue;
        }

        if (filename.EndsWith(_T(".bz2")))
        {
            wxString tmpfile = wxFileName::CreateTempFileName(_T("manbz2"));

            if (!Decompress(filename, tmpfile))
            {
                wxRemoveFile(tmpfile);
                continue;
            }

            filename = tmpfile; // tmpfile isn't removed
        }
        else if (filename.EndsWith(_T(".gz")))
        {
            gzFile f = gzopen(filename.mb_str(), "rb");

            if (!f)
            {
                continue;
            }

            char buffer[4096];
            wxString ret;
            int read_bytes = -1;

            while (true)
            {
                read_bytes = gzread(f, buffer, sizeof(buffer));

                if (read_bytes <= 0) // -1 = error, 0 = eof
                {
                    break;
                }

                ret += wxString(buffer, wxConvLocal, read_bytes);
            }

            gzclose(f);

            if (read_bytes == -1)
            {
                continue;
            }

            return ret;
        }

        wxStringOutputStream sos;
        wxFileInputStream f(filename);

        if (!f.IsOk())
        {
            continue;
        }

        f.Read(sos);

        return sos.GetString();
    }

    return wxString();
}
