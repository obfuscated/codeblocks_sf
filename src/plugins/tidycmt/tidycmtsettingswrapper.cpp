/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "tidycmtsettingswrapper.h"

#include <manager.h>

#include "tidycmt.h"
#include "tidycmtsettings.h"

BEGIN_EVENT_TABLE(TidyCmtSettingsWrapper,wxPanel)
END_EVENT_TABLE()

TidyCmtSettingsWrapper::TidyCmtSettingsWrapper(wxWindow* parent, TidyCmt* plugin, const TidyCmtConfig& tcc) :
  m_Plugin(plugin),
  m_TidyCmtSettings(nullptr)
{
  // wxPanel creation
  Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
  m_TidyCmtSettings = new TidyCmtSettings(this, tcc);
  wxBoxSizer* tidycmtsettings_sizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(tidycmtsettings_sizer);
  tidycmtsettings_sizer->Add(m_TidyCmtSettings, 0, wxEXPAND);
  tidycmtsettings_sizer->Layout();
}

void TidyCmtSettingsWrapper::OnApply()
{
  m_Plugin->ConfigurePlugin(m_TidyCmtSettings->GetTidyCmtConfig());
}
