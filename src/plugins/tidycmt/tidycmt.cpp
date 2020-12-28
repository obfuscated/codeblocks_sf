/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h> // Code::Blocks SDK

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/string.h>
    #include "cbeditor.h"
    #include "manager.h"
    #include "configmanager.h"
#endif

#include "cbstyledtextctrl.h"

#include "tidycmt.h"
#include "tidycmtsettingswrapper.h"

// Keep this in sync with Scintilla.h:
#define SCI_SETUNDOCOLLECTION 2012

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
  PluginRegistrant<TidyCmt> reg(_T("tidycmt"));
}

cbConfigurationPanel* TidyCmt::GetConfigurationPanel(wxWindow* parent)
{
  // Called by plugin manager to show config panel in global Settings Dialog
  if (!IsAttached())
    return nullptr;

  ConfigManager* cfgman = Manager::Get()->GetConfigManager(_T("tidycmt"));
  TidyCmtConfig tcc;
  tcc.enabled         = cfgman->ReadBool(_T("/enabled"),     false);
  tcc.length          = cfgman->ReadInt(_T("/length"),       80);
  tcc.single_line_cmt = cfgman->Read(_T("/single_line_cmt"), _T("//--"));
  tcc.multi_line_cmt  = cfgman->Read(_T("/multi_line_cmt"),  _T("/*--"));

  return new TidyCmtSettingsWrapper(parent, this, tcc);
}

void TidyCmt::OnAttach()
{
	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_BEFORE_SAVE, new cbEventFunctor<TidyCmt, CodeBlocksEvent>(this, &TidyCmt::OnSave));
}

void TidyCmt::OnRelease(cb_unused bool appShutDown)
{
  Manager::Get()->RemoveAllEventSinksFor(this);
}

void TidyCmt::OnSave(CodeBlocksEvent& event)
{
  if (!IsAttached())
    return;

  ConfigManager* cfgman = Manager::Get()->GetConfigManager(_T("tidycmt"));
  if (!cfgman->ReadBool(_T("/enabled"), false))
    return;

	EditorBase* eb = event.GetEditor();
  if (!eb->IsBuiltinEditor() || eb->IsReadOnly())
    return;

	cbEditor*         ed   = (cbEditor*)eb;
	cbStyledTextCtrl* ctrl = ed->GetControl();

  if (!ctrl)
    return;

	unsigned int len = cfgman->ReadInt(_T("/length"), 80);
  wxString     slc = cfgman->Read(_T("/single_line_cmt"), _T("//--"));
  wxString     mlc = cfgman->Read(_T("/multi_line_cmt"),  _T("/*--"));

	unsigned int n   = ctrl->GetLineCount();
	int          pos = ctrl->GetCurrentPos();
	ctrl->SendMsg(SCI_SETUNDOCOLLECTION, 0, 0);

	for (unsigned int i=0; i<n; ++i)
	{
		int a = ctrl->GetLineIndentPosition(i);
		int b = ctrl->GetLineEndPosition(i);

		wxString s = ctrl->GetTextRange(a,b);

		if (s.StartsWith(slc))
		{
			unsigned int from = s.find_first_not_of(_T("/- \t\r\n"));
			unsigned int to   = s.find_last_not_of(_T("/- \t\r\n")) + 1;
			s = s.Mid(from, to - from);

			unsigned int pad = len - s.length() - 8 - ctrl->GetLineIndentation(i);
			s = _T("//---- ") + s + _T(' ') + wxString(_T('-'), pad);

			ctrl->SetTargetStart(a);
			ctrl->SetTargetEnd(b);
			ctrl->ReplaceTarget(s);
		}
		if (s.StartsWith(mlc) && s.EndsWith(_T("*/")))
		{
			s.RemoveLast().RemoveLast();
			unsigned int from = s.find_first_not_of(_T("/*- \t\r\n"));
			unsigned int to   = s.find_last_not_of(_T("/*- \t\r\n")) + 1;
			s = s.Mid(from, to - from);
			unsigned int pad = len - s.length() - 10 - ctrl->GetLineIndentation(i);
			s = _T("/*---- ") + s + _T(' ') + wxString(_T('-'), pad) + _T("*/");

			ctrl->SetTargetStart(a);
			ctrl->SetTargetEnd(b);
			ctrl->ReplaceTarget(s);
		}
	}
	ctrl->SendMsg(SCI_SETUNDOCOLLECTION, 1, 0);
	ctrl->SetCurrentPos(pos);
}

void TidyCmt::ConfigurePlugin(const TidyCmtConfig& tcc)
{
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("tidycmt"));
  cfg->Write(_T("/enabled"),         tcc.enabled);
  cfg->Write(_T("/length"),          static_cast<int>(tcc.length));
  cfg->Write(_T("/single_line_cmt"), tcc.single_line_cmt);
  cfg->Write(_T("/multi_line_cmt"),  tcc.multi_line_cmt);
}
