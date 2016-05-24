/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk_precomp.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <cbexception.h>
    #include <wx/string.h>
#endif

#include <wx/progdlg.h>

#include "scripting/bindings/sc_base_types.h"

class ProgressDialog : public wxProgressDialog
{
    public:
        ProgressDialog()
            : wxProgressDialog(_("Progress"),
                                _("Please wait while operation is in progress..."),
                                100, nullptr,
                                wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT)
        {
        }

        ProgressDialog(wxString title, wxString Message,int max)
            : wxProgressDialog(title,
                                Message,
                                max, nullptr,
                                wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT)
        {
        }

        ~ProgressDialog()
        {
        }

        ProgressDialog& operator=(const ProgressDialog&)
        {
            cbThrow(_T("ProgressDialog copy constructor should never be called!"));
        }

        bool DoUpdate(int value, const wxString& newmsg)
        {
            return wxProgressDialog::Update(value, newmsg, nullptr);
        }
};


namespace ScriptBindings
{
    void Register_ProgressDialog(HSQUIRRELVM vm)
    {


    /** \ingroup sq_dialogs
     *### ProgressDialog()
     * Create an Progress Dialog with the Title _"Progress"_ and the message _"Please wait while operation is in progress..."_.
     * The Dialog can be updated with the member function _DoUpdate_
     *
     *### ProgressDialog(title,Message,max)
     * Create an Progress Dialog with the Title _title_ and the message _Message_.
     * The Dialog can be updated with the member function _DoUpdate_
     *
     *  - __title__     The title for the window [wxString]
     *  - __Message__   The Message which gets displayed to inform the user [wxString]
     *  - __max__       The value on which the progressbar reaches 100% [int]
     *
     *
     */

    /** \ingroup sq_dialogs
     *### DoUpdate(value,newmsg)
     * This member function updates the progressbar with the _value_ and the message _newmsg_. If _value_ is >= _max_. The dialogue will be closed.
     *
     *  - __value__     The title for the window [int]
     *  - __newmsg__   The Message which gets displayed to inform the user [wxString]
     *
     */
        Sqrat::Class<ProgressDialog,Sqrat::NoCopy<ProgressDialog> > progress_dialog(vm,"ProgressDialog");
                progress_dialog.
                //Ctor().
                Ctor<wxString,wxString,int>().
                Func("DoUpdate",    &ProgressDialog::DoUpdate);
        Sqrat::RootTable(vm).Bind("ProgressDialog",progress_dialog);
    }
} // namespace ScriptBindings
