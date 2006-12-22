#ifndef SCRIPTSECURITYWARNINGDLG_H
#define SCRIPTSECURITYWARNINGDLG_H

#include <wx/dialog.h>

enum ScriptSecurityResponse
{
    ssrAllow = 0,
    ssrAllowAll,
    ssrDeny,
    ssrTrust,
    ssrTrustPermanently
};

class ScriptSecurityWarningDlg : public wxDialog
{
    public:
        ScriptSecurityWarningDlg(wxWindow* parent, const wxString& operation, const wxString& command);
        ~ScriptSecurityWarningDlg();

        ScriptSecurityResponse GetResponse();
        void EndModal(int retCode);
    protected:
    private:
};



#endif // SCRIPTSECURITYWARNINGDLG_H
