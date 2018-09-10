#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/string.h>
    #include <wx/regex.h>

    #include "cbeditor.h"
    #include "manager.h"
#endif
#include "rndgen.h"
#include "cbstyledtextctrl.h"

namespace
{
    PluginRegistrant<RndGen> reg(_T("rndgen"));
}

void RndGen::OnAttach()
{
	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_BEFORE_SAVE, new cbEventFunctor<RndGen, CodeBlocksEvent>(this, &RndGen::OnSave));
}

void RndGen::OnSave(CodeBlocksEvent& event)
{
	cbEditor* ed = (cbEditor*) event.GetEditor();
	cbStyledTextCtrl* ctrl = ed->GetControl();

	if (ctrl->GetText().Contains(_T("RANDGEN:")) == false)
		return;

	const int Pos = ctrl->GetCurrentPos();
	ctrl->SetUndoCollection(false);

	wxRegEx int_re(_T("([0-9]+)\\ *;?\\ */\\*(\\ *RANDGEN:INT\\((.*))\\*/"));
//	wxRegEx alnum_re(_T("\\\"([^\"]+)\\\"\\ *;?\\ */\\*(\\ *RANDGEN:ALNUM\\((.*))\\*/"));
	wxRegEx alnum_re(_T("\\\"([^\\\"]+)\\\"\\ *;?\\ */\\*(\\ *RANDGEN:(ALNUM|DIGITS|CHARS|UPPERCHARS|LOWERCHARS)\\((.*))\\*/"));

	const wxString Chars(_T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));

	assert(alnum_re.IsValid() && int_re.IsValid());

	const unsigned int n = ctrl->GetLineCount();
	for (unsigned int i = 0; i < n; ++i)
	{
		const int a = ctrl->GetLineIndentPosition(i);
		const int b = ctrl->GetLineEndPosition(i);
		wxString s(ctrl->GetTextRange(a, b));

		// This is rather crude and will only match a single hit per line.
		// However, that should be good enough for any sensible use, as you will not want more than that anyway.
		// Still, if more correct matching is needed, feel free to do this as homework :-)
		if (int_re.Matches(s))
		{
			const wxString Search(int_re.GetMatch(s, 1));
			long Arg;
			int_re.GetMatch(s, 3).ToLong(&Arg);
			wxString Replace;
			Replace.Printf(_T("%u"), RandGen() % (Arg+1));
			s.Replace(Search, Replace, false);

			ctrl->SetTargetStart(a);
			ctrl->SetTargetEnd(b);
			ctrl->ReplaceTarget(s);
		}
		else if (alnum_re.Matches(s))
		{
			const wxString Search(alnum_re.GetMatch(s, 1));
			const wxString What(alnum_re.GetMatch(s, 3));
			long Arg;
			alnum_re.GetMatch(s, 4).ToLong(&Arg);
			wxString Replace;
			if (What == _T("ALNUM"))
			{
				for (int j = 0; j < Arg; ++j)
					Replace += Chars[RandGen() % Chars.length()];
			}
			else if (What == _T("DIGITS"))
			{
				for (int j = 0; j < Arg; ++j)
					Replace += Chars[RandGen() % 10];
			}
			else if (What == _T("CHARS"))
			{
				for (int j = 0; j < Arg; ++j)
					Replace += Chars[10+ RandGen() % (Chars.length() - 10)];
			}
			else if (What == _T("UPPERCHARS"))
			{
				for (int j = 0; j < Arg; ++j)
					Replace += Chars[36 + RandGen() % 26];
			}
			else if(What == _T("LOWERCHARS"))
			{
				for (int j = 0; j < Arg; ++j)
					Replace += Chars[10 + RandGen() % 26];
			}

			s.Replace(Search, Replace, false);

			ctrl->SetTargetStart(a);
			ctrl->SetTargetEnd(b);
			ctrl->ReplaceTarget(s);
		}
	}

	ctrl->SetUndoCollection(true);
	ctrl->SetCurrentPos(Pos);
}
