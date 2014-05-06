/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <scrollingdialog.h>
#include <manager.h>
#include <configmanager.h>
#include <pluginmanager.h>
#include <cbplugin.h>
#include <globals.h>

// Custom window to shutdown the app when closed.
// used for batch builds only.
class BatchLogWindow : public wxScrollingDialog
{
    public:
        BatchLogWindow(wxWindow *parent, const wxString &title)
            : wxScrollingDialog(parent, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)
        {
            wxSize size;
            size.SetWidth(Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/batch_build_log/width"), wxDefaultSize.GetWidth()));
            size.SetHeight(Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/batch_build_log/height"), wxDefaultSize.GetHeight()));
            SetSize(size);
        }
        void EndModal(int retCode)
        {
            // allowed to close?
            // find compiler plugin
            PluginsArray arr = Manager::Get()->GetPluginManager()->GetCompilerOffers();
            if (arr.GetCount() != 0)
            {
                cbCompilerPlugin* compiler = static_cast<cbCompilerPlugin*>(arr[0]);
                if (compiler && compiler->IsRunning())
                {
                    if (cbMessageBox(_("The build is in progress. Are you sure you want to abort it?"),
                                    _("Abort build?"),
                                    wxICON_QUESTION | wxYES_NO, this) == wxID_YES)
                    {
                        compiler->KillProcess();
                        while (compiler->IsRunning())
                        {
                            wxMilliSleep(100);
                            Manager::Yield();
                        }
                    }
                    return;
                }
            }

            Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/batch_build_log/width"), (int)GetSize().GetWidth());
            Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/batch_build_log/height"), (int)GetSize().GetHeight());
            wxScrollingDialog::EndModal(retCode);
        }
};
