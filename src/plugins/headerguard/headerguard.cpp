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

#include "headerguard.h"
#include "cbstyledtextctrl.h"

namespace
{
    PluginRegistrant<HeaderGuard> reg(_T("Header Guard"));
}

void HeaderGuard::OnAttach()
{
	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_BEFORE_SAVE, new cbEventFunctor<HeaderGuard, CodeBlocksEvent>(this, &HeaderGuard::OnSave));
}

void HeaderGuard::OnSave(CodeBlocksEvent& event)
{
	cbEditor* ed = (cbEditor*) event.GetEditor();

	wxString n = ed->GetFilename();

	if(!n.Right(2).Lower().IsSameAs(_T(".h")) && !n.Right(3).Lower().IsSameAs(_T(".hpp")) && !n.Right(3).Lower().IsSameAs(_T(".hxx")))
		return;

	// Header guards should appear before any comments or code to ensure the compiler can perform header guard optimisation,
	// therefore we don't really need to scan the full text, checking the first line is good enough and much faster, both here and in build.
	// It also doesn't choke on external header guards that some people still use.
	// We're omitting a check for "#pragma once", as that's not standard compliant. It doesn't hurt to add guards around "#pragma once", either.
	const wxString s(ed->GetControl()->GetLine(0).Trim(true).Trim(false));
	if(s.StartsWith(_T("#ifndef")) && ( s.Contains(_T("HEADER")) || s.Right(2).IsSameAs(_T("_H")) || s.Contains(_T(" H_")) || s.Contains(_T("_H_")) || s.Contains(_T("_INCLUDED")) ))
		return;

	unsigned int s1 = 257, s2 = 123, b = 9737333, c = 174440041;
	for(unsigned int i = 0; i < n.length(); ++i)
	{
		b = (b*33) + (s1 += n[i]);
		(c *= 47) += s2;
		s2 += s1;
	}
	b ^= (((s1%255)<<10) | (s2&255));
	c ^= (c << 14); b ^= (b << 21); c ^= (c >> 5);
	b ^= (b >> 17); c ^= (c << 11); b ^= (b << 9);

	n.Printf(_T("#ifndef HEADER_%X%X\n#define HEADER_%X%X\n\n"), b, c, b, c);

	ed->GetControl()->BeginUndoAction();
	ed->GetControl()->InsertText (0, n);
	ed->GetControl()->InsertText (ed->GetControl()->GetLength() - 1, _T("\n#endif // header guard \n"));
	ed->GetControl()->EndUndoAction();
}
