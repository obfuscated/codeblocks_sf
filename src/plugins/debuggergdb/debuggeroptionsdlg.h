/*
* This file is part of Code::Blocks, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*/

#ifndef DEBUGGEROPTIONSDLG_H
#define DEBUGGEROPTIONSDLG_H

#include <wx/dialog.h>

class DebuggerOptionsDlg : public wxDialog
{
	public:
		DebuggerOptionsDlg(wxWindow* parent);
		virtual ~DebuggerOptionsDlg();
		
		void EndModal(int retCode);
	protected:
	private:
};

#endif // DEBUGGEROPTIONSDLG_H
