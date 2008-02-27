/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h" // Code::Blocks SDK
#ifndef CB_PRECOMP
  #include <wx/intl.h>
  #include <wx/string.h>
  #include <wx/utils.h> // wxLaunchDefaultBrowser
  #include "globals.h"
  #include "manager.h"
  #include "editormanager.h"
  #include "cbeditor.h"
#endif
#include "cbstyledtextctrl.h"

#include "cb_koders.h"
#include "kodersdialog.h"

// Register the plugin
namespace
{
    PluginRegistrant<CB_Koders> reg(_T("CB_Koders"));
};

const int idSearchKoders = wxNewId();

BEGIN_EVENT_TABLE(CB_Koders, cbToolPlugin)
  EVT_MENU(idSearchKoders, CB_Koders::OnSearchKoders)
END_EVENT_TABLE()

// constructor
CB_Koders::CB_Koders() :
  TheDialog(0)
{
}

// destructor
CB_Koders::~CB_Koders()
{
  if (TheDialog)
    TheDialog->Destroy();
}

void CB_Koders::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void CB_Koders::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}

int CB_Koders::Execute()
{
  if (IsReady() && TheDialog->ShowModal()==wxID_OK)
  {
    const wxString search = TheDialog->GetSearch();
    if (search.IsEmpty())
    {
      cbMessageBox(_("Cannot search for an empty expession."), _("Error"), wxICON_ERROR);
    }
    else
    {
      const wxString language = TheDialog->GetLanguage();
      const wxString license  = TheDialog->GetLicense();

      wxString query;
      query.Printf(_("http://www.koders.com/?S=%s&btnSearch=Search&la=%s&li=%s"),
                   search.c_str(), language.c_str(), license.c_str());
      if (!wxLaunchDefaultBrowser(query))
        cbMessageBox(_("Could not launch the default browser of your system."), _("Error"), wxICON_ERROR);
    }
  }

	return 0;
}

void CB_Koders::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
	if (!menu || !IsAttached())
		return;

	if (type == mtEditorManager)
	{
		menu->AppendSeparator();
		menu->Append(idSearchKoders, _("Search at Koders..."), _("Search keyword at Koders webpage..."));
	}
}

bool CB_Koders::IsReady()
{
  if (!IsAttached())
    return false;

  if (!TheDialog)
    TheDialog = new KodersDialog(Manager::Get()->GetAppWindow());

  if (TheDialog)
    return true;
  else
    cbMessageBox(_("Could not initialise CB_Koders plugin."), _("Error"), wxICON_ERROR);

  return false;
}

void CB_Koders::OnSearchKoders(wxCommandEvent& event)
{
  if (IsReady())
  {
    wxString search(_T("")); // the word to search for (if any)
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

    if (ed)
    {
      // check if there is any text selected
      cbStyledTextCtrl *control = ed->GetControl();
      search = control->GetSelectedText();

      // if no selection, take the word under the cursor
      if (search.IsEmpty())
      {
        int origPos = control->GetCurrentPos();
        int start = control->WordStartPosition(origPos, true);
        int end = control->WordEndPosition(origPos, true);
        search = control->GetTextRange(start, end);
      }
    }

    TheDialog->SetSearch(search);
    Execute();
  }
}
