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
    #include <wx/intl.h>
    #include <wx/string.h>
    #include "cbeditor.h"
    #include "manager.h"
    #endif
#include "tidycmt.h"
#include "cbstyledtextctrl.h"

#define SCI_SETUNDOCOLLECTION 2012

const unsigned int len = 112;

namespace
{
    PluginRegistrant<TidyCmt> reg(_T("tidycmt"));
}

void TidyCmt::OnAttach()
{
	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_BEFORE_SAVE, new cbEventFunctor<TidyCmt, CodeBlocksEvent>(this, &TidyCmt::OnSave));
}

void TidyCmt::OnSave(CodeBlocksEvent& event)
{
	cbEditor* ed = (cbEditor*) event.GetEditor();
	cbStyledTextCtrl* ctrl = ed->GetControl();

	unsigned int n = ctrl->GetLineCount();

	int pos = ctrl->GetCurrentPos();
	ctrl->SendMsg(SCI_SETUNDOCOLLECTION, 0, 0);

	for(unsigned int i = 0; i < n; ++i)
	{
		int a = ctrl->GetLineIndentPosition(i);
		int b = ctrl->GetLineEndPosition(i);

		wxString s = ctrl->GetTextRange(a,b);

		if(s.StartsWith(_T("//--")))
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
		if(s.StartsWith(_T("/*--")) && s.EndsWith(_T("*/")))
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
