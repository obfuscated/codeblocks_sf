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
