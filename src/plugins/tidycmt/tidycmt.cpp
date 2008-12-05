#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/intl.h>
#include <wx/string.h>
#include "cbeditor.h"
#include "manager.h"
#endif
#include "tidycmt.h"
#include "cbstyledtextctrl.h"

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

			ctrl->SetSelection(a, b);  // this must be the most stupid way on earth of doing it,
			ctrl->ReplaceSelection(s); // but there seems to be no SetTextRange or similar function
		}
		if(s.StartsWith(_T("/*--")) && s.EndsWith(_T("*/")))
		{
			s.RemoveLast().RemoveLast();
			unsigned int from = s.find_first_not_of(_T("/*- \t\r\n"));
			unsigned int to   = s.find_last_not_of(_T("/*- \t\r\n")) + 1;
			s = s.Mid(from, to - from);
			unsigned int pad = len - s.length() - 10 - ctrl->GetLineIndentation(i);
			s = _T("/*---- ") + s + _T(' ') + wxString(_T('-'), pad) + _T("*/");

			ctrl->SetSelection(a, b);
			ctrl->ReplaceSelection(s);
		}
	}
	ctrl->SetSelection(0,0);
}
