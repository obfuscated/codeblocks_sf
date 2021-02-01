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
    #include "manager.h"
#endif

#include <wx/progdlg.h>

#include "sc_utils.h"
#include "sc_typeinfo_all.h"

namespace ScriptBindings
{
    class ProgressDialog : public wxProgressDialog
    {
        public:
            ProgressDialog()
                : wxProgressDialog(_("Progress"),
                                    _("Please wait while operation is in progress..."),
                                    100, Manager::Get()->GetAppWindow(),
                                    wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT)
            {
            }

            ~ProgressDialog() override
            {
            }

            ProgressDialog& operator=(const ProgressDialog&) = delete;
            ProgressDialog(const ProgressDialog&) = delete;
    };

    SQInteger ProgressDialog_DoUpdate(HSQUIRRELVM v)
    {
        // this, value, newmsg
        ExtractParams3<ProgressDialog*, SQInteger, const wxString *> extractor(v);
        if (!extractor.Process("ProgressDialog::DoUpdate"))
            return extractor.ErrorMessage();

        sq_pushbool(v, extractor.p0->Update(extractor.p1, *extractor.p2, nullptr));
        return 1;
    }

    void Register_ProgressDialog(HSQUIRRELVM v)
    {
        PreserveTop preserveTop(v);
        sq_pushroottable(v);

        {
            // Register ProgressDialog
            const SQInteger classDecl = CreateClassDecl<ProgressDialog>(v, _SC("ProgressDialog"));
            BindEmptyCtor<ProgressDialog>(v);
            BindMethod(v, _SC("DoUpdate"), ProgressDialog_DoUpdate,
                       _SC("ProgressDialog::DoUpdate"));

            BindDefaultInstanceCmp<ProgressDialog>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        sq_pop(v, 1); // pop root table
    }
} // namespace ScriptBindings
