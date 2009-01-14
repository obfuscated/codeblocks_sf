#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/intl.h>
#include <wx/string.h>
#include "cbeditor.h"
#include "manager.h"
#endif
#include "RndGen.h"
#include "cbstyledtextctrl.h"

// already done by wxWidgets
// #include <stdlib.h>

const unsigned int len = 112;

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
	srand(time(0));
	cbEditor* ed = (cbEditor*) event.GetEditor();
	cbStyledTextCtrl* ctrl = ed->GetControl();

	wxString quicktest = ctrl->GetText();
	if(quicktest.Contains(_T("RANDGEN:")) == false)
		return;

	wxRegEx int_re(_T("([0-9]+)\\ *;?\\ */\\*(\\ *RANDGEN:INT\\((.*))\\*/"));
//	wxRegEx alnum_re(_T("\\\"([^\"]+)\\\"\\ *;?\\ */\\*(\\ *RANDGEN:ALNUM\\((.*))\\*/"));
	wxRegEx alnum_re(_T("\\\"([^\\\"]+)\\\"\\ *;?\\ */\\*(\\ *RANDGEN:(ALNUM|DIGITS|CHARS|UPPERCHARS|LOWERCHARS)\\((.*))\\*/"));

	wxString c(_T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));

	assert(alnum_re.IsValid() && int_re.IsValid());

	unsigned int n = ctrl->GetLineCount();

	for(unsigned int i = 0; i < n; ++i)
	{
		int a = ctrl->GetLineIndentPosition(i);
		int b = ctrl->GetLineEndPosition(i);

		wxString s = ctrl->GetTextRange(a,b);

		// This is rather crude and will only match a single hit per line.
		// However, that should be good enough for any sensible use, as you will not want more than that anyway.
		// Still, if more correct matching is needed, feel free to do this as homework :-)
		if(int_re.Matches(s))
		{
			wxString search = int_re.GetMatch(s, 1);
			long arg;
			int_re.GetMatch(s, 3).ToLong(&arg);
			wxString replace;
			int rnd = rand() % (arg+1);
			replace.Printf(_T("%u"), rnd);
			s.Replace(search, replace, false);

			ctrl->SetTargetStart(a);
			ctrl->SetTargetEnd(b);
			ctrl->ReplaceTarget(s);
		}
		else if(alnum_re.Matches(s))
		{
			wxString search = alnum_re.GetMatch(s, 1);
			long arg;
			wxString what = alnum_re.GetMatch(s, 3);
			alnum_re.GetMatch(s, 4).ToLong(&arg);
			wxString replace;
			if(what == _T("ALNUM"))
			{
				for(int i = 0; i<arg; ++i)
					replace += c[rand() % c.length()];
			}
			if(what == _T("DIGITS"))
			{
				for(int i = 0; i<arg; ++i)
					replace += c[rand() % 10];
			}
			if(what == _T("CHARS"))
			{
				for(int i = 0; i<arg; ++i)
					replace += c[10+ rand() % (c.length() - 10)];
			}
			if(what == _T("UPPERCHARS"))
			{
				for(int i = 0; i<arg; ++i)
					replace += c[36 + rand() % 26];
			}
			if(what == _T("LOWERCHARS"))
			{
				for(int i = 0; i<arg; ++i)
					replace += c[10 + rand() % 26];
			}
			s.Replace(search, replace, false);

			ctrl->SetTargetStart(a);
			ctrl->SetTargetEnd(b);
			ctrl->ReplaceTarget(s);
		}
	}
}
