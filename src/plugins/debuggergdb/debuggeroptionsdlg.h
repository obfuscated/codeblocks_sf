/*
* This file is part of Code::Blocks, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*/

#ifndef DEBUGGEROPTIONSDLG_H
#define DEBUGGEROPTIONSDLG_H

#include <cbplugin.h>

class DebuggerOptionsDlg : public cbConfigurationPanel
{
	public:
		DebuggerOptionsDlg(wxWindow* parent);
		virtual ~DebuggerOptionsDlg();

        virtual wxString GetTitle(){ return _T("Debugger settings"); }
        virtual wxString GetBitmapBaseName(){ return _T("debugger"); }
        virtual void OnApply();
        virtual void OnCancel(){}
	protected:
	private:
};

#endif // DEBUGGEROPTIONSDLG_H
