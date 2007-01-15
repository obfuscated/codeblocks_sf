#include <sdk_precomp.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <cbexception.h>
    #include <manager.h>
    #include <configmanager.h>
    #include <messagemanager.h>
    #include <wx/string.h>
#endif

#include <wx/xrc/xmlres.h>
#include <wx/dialog.h>

#include "sc_base_types.h"

namespace ScriptBindings
{
    class XrcDialog : public wxDialog
    {
            DECLARE_EVENT_TABLE()
            wxString m_CallBack;
        public:
            XrcDialog(wxWindow* parent, const wxString& dlgName, const wxString& callback)
                : m_CallBack(callback)
            {
                if (!wxXmlResource::Get()->LoadDialog(this, parent, dlgName))
                {
                    cbThrow(wxEmptyString);
                }
            }
            ~XrcDialog(){ }
            void OnButton(wxCommandEvent& event);
    };

    XrcDialog* s_ActiveDialog = 0;

    BEGIN_EVENT_TABLE(XrcDialog, wxDialog)
        EVT_CHOICE(-1, XrcDialog::OnButton)
        EVT_COMBOBOX(-1, XrcDialog::OnButton)
        EVT_CHECKBOX(-1, XrcDialog::OnButton)
        EVT_LISTBOX(-1, XrcDialog::OnButton)
        EVT_RADIOBOX(-1, XrcDialog::OnButton)
        EVT_BUTTON(-1, XrcDialog::OnButton)
    END_EVENT_TABLE()

    void XrcDialog::OnButton(wxCommandEvent& event)
    {
        // VERY important, or else the dialog will not be dismissed with
        // standard event IDs like wxID_OK/wxID_CANCEL/etc.
        event.Skip(true);

        try
        {
//            DBGLOG(_T("Script dialog event: %d"), event.GetId());
            SqPlus::SquirrelFunction<void> cb(cbU2C(m_CallBack));
            if (cb.func.IsNull())
                return;
            cb(event.GetId());
        }
        catch (SquirrelError& e)
        {
            Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        }
    }

    int ShowDialog(const wxString& xrc, const wxString& dlgName, const wxString& callback)
    {
        wxString actual = ConfigManager::LocateDataFile(xrc, sdScriptsUser | sdScriptsGlobal);
//        DBGLOG(_T("Original parameter is: ") + xrc);
        DBGLOG(_T("Loading XRC: ") + actual);
        if (wxXmlResource::Get()->Load(actual))
        {
            XrcDialog* old = s_ActiveDialog;
            try
            {
                s_ActiveDialog = new XrcDialog(0, dlgName, callback);
                int ret = s_ActiveDialog->ShowModal();
                delete s_ActiveDialog;
                s_ActiveDialog = old;
                wxXmlResource::Get()->Unload(actual);
                return ret;
            }
            catch (cbException& e)
            {
                cbMessageBox(wxString::Format(_("Dialog \"%s\" not found...\n\nActual resource: \"%s\"\nOriginal resource: \"%s\""),
                                            dlgName.c_str(),
                                            actual.c_str(),
                                            xrc.c_str()),
                            _("Error"), wxICON_ERROR);
            }
        }
        return -1;
    }

    void EndModal(int retCode)
    {
        // valid only while in a ShowDialog() call
        if (s_ActiveDialog)
        {
            s_ActiveDialog->EndModal(retCode);
            return;
        }
        cbMessageBox(_("EndModal() only valid while inside a ShowDialog() call..."), _("Error"), wxICON_ERROR);
    }

    SQInteger XrcId(HSQUIRRELVM v)
    {
        // here we simulate XRCID by using wxWindow::FindWindowByName().
        // XRCID() doesn't seem to work on its own inside here...
        StackHandler sa(v);
        if (!s_ActiveDialog)
        {
            cbMessageBox(_("XRCID() only valid while inside a ShowDialog() call..."), _("Error"), wxICON_ERROR);
            return sa.Return((SQInteger)-1);
        }

        wxWindow* win = 0;
        if (sa.GetType(2) == OT_STRING)
            win = wxWindow::FindWindowByName(cbC2U(sa.GetString(2)), s_ActiveDialog);
        else
            win = wxWindow::FindWindowByName(*SqPlus::GetInstance<wxString>(v, 2), s_ActiveDialog);
        return sa.Return((SQInteger)(win ? win->GetId() : -1));
    }

    void Register_Dialog()
    {
        SqPlus::RegisterGlobal(ShowDialog, "ShowDialog");
        SqPlus::RegisterGlobal(EndModal, "EndModal");
        SquirrelVM::CreateFunctionGlobal(XrcId, "XRCID", "*");
    }
} // namespace ScriptBindings
