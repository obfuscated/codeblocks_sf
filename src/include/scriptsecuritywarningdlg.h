/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

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
