/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
  #include <wx/filename.h>
  #include <wx/fs_zip.h>
  #include <wx/intl.h>
  #include <wx/menuitem.h>
  #include <wx/process.h>
  #include <wx/xrc/xmlres.h>
  #include "cbeditor.h"
  #include "cbproject.h"
  #include "configmanager.h"
  #include "editormanager.h"
  #include "globals.h"
  #include "logmanager.h"
  #include "macrosmanager.h"
  #include "manager.h"
  #include "projectmanager.h"
#endif

#include <vector>

#include <wx/textdlg.h>
#include <wx/mimetype.h>
#include <wx/help.h> //(wxWindows chooses the appropriate help controller class)
#include <wx/helpbase.h> //(wxHelpControllerBase class)
#include <wx/helpwin.h> //(Windows Help controller)
#include <wx/generic/helpext.h> //(external HTML browser controller)
#include <wx/html/helpctrl.h> //(wxHTML based help controller: wxHtmlHelpController)

#ifdef __WXMSW__
#include <wx/msw/helpchm.h> // used in case we fail to load the OCX module (it could fail too)
#include <wx/thread.h>
#endif

#include <cbstyledtextctrl.h>
#include <sc_base_types.h>
#include <sqplus.h>


#include "help_plugin.h"
#include "MANFrame.h"



// 20 wasn't enough
#define MAX_HELP_ITEMS 32

int idHelpMenus[MAX_HELP_ITEMS];

// Register the plugin
namespace
{
    PluginRegistrant<HelpPlugin> reg(_T("HelpPlugin"));
    int idViewMANViewer = wxNewId();
};

BEGIN_EVENT_TABLE(HelpPlugin, cbPlugin)
	EVT_MENU(idViewMANViewer, HelpPlugin::OnViewMANViewer)
	EVT_UPDATE_UI(idViewMANViewer, HelpPlugin::OnUpdateUI)
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
}

void HelpPlugin::OnAttach()
{
    // load configuration (only saved in our config dialog)
    HelpCommon::LoadHelpFilesVector(m_Vector);

    const wxString man_prefix = _T("man:");
    wxString all_man_dirs(man_prefix);

    for (HelpCommon::HelpFilesVector::const_iterator i = m_Vector.begin(); i != m_Vector.end(); ++i)
    {
        if (i->second.name.Mid(0, man_prefix.size()).CmpNoCase(man_prefix) == 0)
        {
            // only add ; if a dir is already set
            if (all_man_dirs.Length() > man_prefix.Length())
            {
                all_man_dirs += _T(";");
            }

            all_man_dirs += i->second.name.Mid(man_prefix.Length());
        }
    }

    wxBitmap zoominbmp = wxXmlResource::Get()->LoadBitmap(_T("ZoomInBitmap"));
    wxBitmap zoomoutbmp = wxXmlResource::Get()->LoadBitmap(_T("ZoomOutBitmap"));

    m_manFrame = new MANFrame(Manager::Get()->GetAppWindow(), wxID_ANY, zoominbmp, zoomoutbmp);
    m_manFrame->SetDirs(all_man_dirs);
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("MANViewer");
    evt.title = _("Man/Html pages viewer");
    evt.pWindow = m_manFrame;
    evt.dockSide = CodeBlocksDockEvent::dsRight;
    evt.desiredSize.Set(320, 240);
    evt.floatingSize.Set(320, 240);
    evt.minimumSize.Set(240, 160);
    Manager::Get()->ProcessEvent(evt);

    int baseFont = Manager::Get()->GetConfigManager(_T("help_plugin"))->ReadInt(_T("/base_font_size"), 0);

    if (baseFont > 0)
    {
        m_manFrame->SetBaseFontSize(baseFont);
    }

    if (Manager::Get()->GetConfigManager(_T("help_plugin"))->ReadBool(_T("/show_man_viewer"), false))
    {
        ShowMANViewer();
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
    BuildHelpMenu();
}

void HelpPlugin::OnRelease(bool /*appShutDown*/)
{
    Manager::Get()->GetConfigManager(_T("help_plugin"))->Write(_T("/base_font_size"), m_manFrame->GetBaseFontSize());
    CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
    evt.pWindow = m_manFrame;
    Manager::Get()->ProcessEvent(evt);
	m_manFrame->Destroy();
	m_manFrame = 0;
}

void HelpPlugin::BuildHelpMenu()
{
    int counter = 0;
    HelpCommon::HelpFilesVector::iterator it;

    for (it = m_Vector.begin(); it != m_Vector.end(); ++it, ++counter)
    {
        if (counter == HelpCommon::getDefaultHelpIndex())
        {
            AddToHelpMenu(idHelpMenus[counter], it->first + _T("\tF1"), it->second.readFromIni);
        }
        else
        {
            AddToHelpMenu(idHelpMenus[counter], it->first, it->second.readFromIni);
        }
    }

    m_LastId = idHelpMenus[0] + counter;
}

void HelpPlugin::BuildMenu(wxMenuBar *menuBar)
{
    if (!IsAttached())
    {
        return;
    }

    m_pMenuBar = menuBar;

    BuildHelpMenu();

    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* view = menuBar->GetMenu(idx);
        wxMenuItemList& items = view->GetMenuItems();
        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                view->InsertCheckItem(i, idViewMANViewer, _("Man pages viewer"), _("Toggle displaying the man pages viewer"));
                return;
            }
        }
        // not found, just append
        view->AppendCheckItem(idViewMANViewer, _("Man pages viewer"), _("Toggle displaying the man pages viewer"));
    }
}

void HelpPlugin::BuildModuleMenu(const ModuleType type, wxMenu *menu, const FileTreeData* /*data*/)
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
      AddToPopupMenu(sub_menu, idHelpMenus[counter++], it->first, it->second.readFromIni);
    }

    wxMenuItem *locate_in_menu = new wxMenuItem(0, wxID_ANY, _("&Locate in"), _T(""), wxITEM_NORMAL);
    locate_in_menu->SetSubMenu(sub_menu);

    menu->Append(locate_in_menu);
  }
}

bool HelpPlugin::BuildToolBar(wxToolBar * /*toolBar*/)
{
	return false;
}

void HelpPlugin::AddToHelpMenu(int id, const wxString &help, bool
#ifdef __WXMSW__
	fromIni
#endif
)
{
  if (!m_pMenuBar)
  {
    return;
  }

  int pos = m_pMenuBar->FindMenu(_("&Help"));

  if (pos != wxNOT_FOUND)
  {
    wxMenu *helpMenu = m_pMenuBar->GetMenu(pos);

    if (id == idHelpMenus[0])
    {
      helpMenu->AppendSeparator();
    }

#ifdef __WXMSW__
    if (fromIni)
    {
      wxMenuItem *mitem = new wxMenuItem(0, id, help);
      wxFont &font = mitem->GetFont();
      font.SetWeight(wxFONTWEIGHT_BOLD);
      mitem->SetFont(font);
      helpMenu->Append(mitem);
    }
    else
#endif
    {
      helpMenu->Append(id, help);
    }
  }
}

void HelpPlugin::RemoveFromHelpMenu(int id, const wxString & /*help*/)
{
  if (!m_pMenuBar)
  {
    return;
  }

  int pos = m_pMenuBar->FindMenu(_("&Help"));

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

    #if wxCHECK_VERSION(2, 9, 0)
    if (mi && (mi->GetKind() == wxITEM_SEPARATOR || mi->GetItemLabelText().IsEmpty()))
    #else
    if (mi && (mi->GetKind() == wxITEM_SEPARATOR || mi->GetText().IsEmpty()))
    #endif
    {
      helpMenu->Remove(mi);
      delete mi;
    }
  }
}

void HelpPlugin::AddToPopupMenu(wxMenu *menu, int id, const wxString &help, bool
#ifdef __WXMSW__
	fromIni
#endif
)
{
  if (!help.IsEmpty())
  {
#ifdef __WXMSW__
    if (fromIni)
    {
      wxMenuItem *mitem = new wxMenuItem(0, id, help);
      wxFont &font = mitem->GetFont();
      font.SetWeight(wxFONTWEIGHT_BOLD);
      mitem->SetFont(font);
      menu->Append(mitem);
    }
    else
#endif
    {
      menu->Append(id, help);
    }
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

void HelpPlugin::OnViewMANViewer(wxCommandEvent &event)
{
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_manFrame;
    Manager::Get()->ProcessEvent(evt);
}

void HelpPlugin::ShowMANViewer(bool show)
{
    CodeBlocksDockEvent evt(show ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_manFrame;
    Manager::Get()->ProcessEvent(evt);

    // update user prefs
    Manager::Get()->GetConfigManager(_T("help_plugin"))->Write(_T("/show_man_viewer"), show);
}

void HelpPlugin::OnUpdateUI(wxUpdateUIEvent& /*event*/)
{
    wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();

    // uncheck checkbox if window was closed
    if(m_manFrame && !IsWindowReallyShown(m_manFrame))
        pbar->Check(idViewMANViewer, false);
}

void HelpPlugin::LaunchHelp(const wxString &c_helpfile, bool isExecutable, bool openEmbeddedViewer, HelpCommon::StringCase keyCase, const wxString &defkeyword, const wxString &c_keyword)
{
  const static wxString http_prefix(_T("http://"));
  const static wxString man_prefix(_T("man:"));
  wxString helpfile(c_helpfile);

  // Patch by Yorgos Pagles: Use the new attributes to calculate the keyword
  wxString keyword = c_keyword.IsEmpty() ? defkeyword : c_keyword;

  if(keyCase == HelpCommon::UpperCase)
  {
    keyword.MakeUpper();
  }
  else if (keyCase == HelpCommon::LowerCase)
  {
    keyword.MakeLower();
  }

  helpfile.Replace(_T("$(keyword)"), keyword);
  Manager::Get()->GetMacrosManager()->ReplaceMacros(helpfile);

  if (isExecutable)
  {
    Manager::Get()->GetLogManager()->DebugLog(_T("Executing ") + helpfile);
    wxExecute(helpfile);
    return;
  }

  // Support C::B scripts
  if (wxFileName(helpfile).GetExt() == _T("script"))
  {
      if (Manager::Get()->GetScriptingManager()->LoadScript(helpfile))
      {
        // help scripts must contain a function with the following signature:
        // function SearchHelp(keyword)
        try
        {
            SqPlus::SquirrelFunction<void> f("SearchHelp");
            f(keyword);
        }
        catch (SquirrelError& e)
        {
            Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        }
      }
      else
      {
        Manager::Get()->GetLogManager()->DebugLog(_T("Couldn't run script"));
      }

      return;
  }

  // Operate on help html file links inside embedded viewer
  if (openEmbeddedViewer && wxFileName(helpfile).GetExt().Mid(0, 3).CmpNoCase(_T("htm")) == 0)
  {
    Manager::Get()->GetLogManager()->DebugLog(_T("Launching ") + helpfile);
    cbMimePlugin* p = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(helpfile);
    if (p)
    {
        p->OpenFile(helpfile);
    }
    else
    {
        reinterpret_cast<MANFrame *>(m_manFrame)->LoadPage(helpfile);
        ShowMANViewer();
    }
    return;
  }

  // Operate on help http (web) links
  if (helpfile.Mid(0, http_prefix.size()).CmpNoCase(http_prefix) == 0)
  {
    Manager::Get()->GetLogManager()->DebugLog(_T("Launching ") + helpfile);
    wxLaunchDefaultBrowser(helpfile);
    return;
  }

  // Operate on man pages
  if (helpfile.Mid(0, man_prefix.size()).CmpNoCase(man_prefix) == 0)
  {
    if (reinterpret_cast<MANFrame *>(m_manFrame)->SearchManPage(c_helpfile, keyword))
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("Couldn't find man page"));
    }
    else
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("Launching man page"));
    }

    ShowMANViewer();
    return;
  }

  wxFileName the_helpfile = wxFileName(helpfile);
  Manager::Get()->GetLogManager()->DebugLog(_T("Help File is ") + helpfile);

  if (!(the_helpfile.FileExists()))
  {
    wxString msg;
    msg << _("Couldn't find the help file:\n")
        << the_helpfile.GetFullPath() << _("\n")
        << _("Do you want to run the associated program anyway?");
    if (!(cbMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO | wxNO_DEFAULT) == wxID_YES))
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
  // Patch by Yorgos Pagles: Use the new keyword calculation
  LaunchHelp(hfa.name, hfa.isExecutable, hfa.openEmbeddedViewer, hfa.keywordCase, hfa.defaultKeyword, text);
}
