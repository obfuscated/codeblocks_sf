/*
* This file is part of Code::Blocks, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*/

#ifndef DEBUGGEROPTIONSDLG_H
#define DEBUGGEROPTIONSDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"

class DebuggerGDB;

class DebuggerOptionsDlg : public cbConfigurationPanel
{
    public:
        DebuggerOptionsDlg(wxWindow* parent, DebuggerGDB* plugin);
        virtual ~DebuggerOptionsDlg();

        virtual wxString GetTitle() const { return _("Debugger settings"); }
        virtual wxString GetBitmapBaseName() const { return _T("debugger"); }
        virtual void OnApply();
        virtual void OnCancel(){}
    protected:
        DebuggerGDB* m_pPlugin;
    private:
};

#endif // DEBUGGEROPTIONSDLG_H
