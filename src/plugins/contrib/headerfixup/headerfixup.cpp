/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <wx/window.h>

#include <sdk.h>
#include <cbproject.h>
#include <globals.h>
#include <manager.h>
#include <projectmanager.h>

#include "headerfixup.h"
#include "configuration.h"
#include "execution.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

namespace
{
  PluginRegistrant<HeaderFixup> reg(_T("HeaderFixup"));
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

HeaderFixup::HeaderFixup()
{
  if ( !Manager::LoadResource(_T("headerfixup.zip")) )
    NotifyMissingFile(_T("headerfixup.zip"));
}// HeaderFixup

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

HeaderFixup::~HeaderFixup()
{
}// ~HeaderFixup

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void HeaderFixup::OnAttach()
{
}// OnAttach

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void HeaderFixup::OnRelease(bool /*appShutDown*/)
{
}// OnRelease

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

int HeaderFixup::Configure()
{
  cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _("Header Fixup Config"));
  cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
  if (panel)
  {
    dlg.AttachConfigurationPanel(panel);
    PlaceWindow(&dlg);
    return dlg.ShowModal() == wxID_OK ? 0 : -1;
  }
  return 1;
}// Configure

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

int HeaderFixup::Execute()
{
  // if not attached, exit
  if ( !IsAttached() )
    return -1;

  // if no project is opened -> inform the user and do not operate
  const cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
  if (!prj)
  {
    cbMessageBox(_("You need to open a project/workspace before using this plugin!"),
                 _T("Header Fixup"), wxICON_ERROR | wxOK);
    return -1;
  }

  Execution Dlg(NULL);
  Dlg.ShowModal();
  return 0;
}// Execute

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

cbConfigurationPanel* HeaderFixup::GetConfigurationPanel(wxWindow* parent)
{
  return new Configuration(parent);
}// GetConfigurationPanel
