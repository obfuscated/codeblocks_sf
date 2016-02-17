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

#include <scripting/squirrel/squirrel.h>
#include <scripting/squirrel/sqvm.h>
#include "scripting/bindings/sc_base_types.h"

#include <editarrayfiledlg.h>
#include <editarrayorderdlg.h>
#include <editarraystringdlg.h>
#include <editpairdlg.h>
#include <editpathdlg.h>
#include <genericmultilinenotesdlg.h>



namespace ScriptBindings
{
    /*SQInteger EditArrayFileDlg_Dtor(SQUserPointer up, cb_unused SQInteger size)
    {
      SQ_DELETE_CLASS(EditArrayFileDlg);
    }*/

    SQInteger EditArrayFileDlg_Ctor(HSQUIRRELVM vm)
    {
        //    (wxWindow* parent,
        //    wxArrayString& array,
        //    bool useRelativePaths = false,
        //    const wxString& basePath = _T(""))
        StackHandler sa(vm);
        EditArrayFileDlg* dlg = nullptr;

        if (sa.GetParamCount() > 3)
        {
            dlg = new EditArrayFileDlg(nullptr,
                                        *sa.GetInstance<wxArrayString>( 2),
                                        sa.GetValue<bool>(3),
                                        *sa.GetInstance<wxString>( 4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new EditArrayFileDlg(nullptr,
                                        *sa.GetInstance<wxArrayString>( 2),
                                        sa.GetValue<bool>(3));
        }
        else if (sa.GetParamCount() > 1)
        {
            dlg = new EditArrayFileDlg(nullptr,
                                        *sa.GetInstance<wxArrayString>( 2));
        }
        else
            return sa.ThrowError(_("EditArrayFileDlg needs at least one argument"));


        sq_setinstanceup(vm, 1, dlg);
        sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<EditArrayFileDlg>::Delete);
        return SC_RETURN_OK;
    }

    /*SQInteger EditArrayOrderDlg_Dtor(SQUserPointer up, cb_unused SQInteger size)
    {
      SQ_DELETE_CLASS(EditArrayOrderDlg);
    }*/

    SQInteger EditArrayOrderDlg_Ctor(HSQUIRRELVM vm)
    {
        //    (wxWindow* parent, const wxArrayString& array = 0L)
        StackHandler sa(vm);
        EditArrayOrderDlg* dlg = nullptr;

        if (sa.GetParamCount() == 1)
            dlg = new EditArrayOrderDlg(nullptr, wxArrayString());
        else if (sa.GetParamCount() == 2)
            dlg = new EditArrayOrderDlg(nullptr, *sa.GetInstance<wxArrayString>( 2));
        else
            return sa.ThrowError(_("EditArrayOrderDlg needs at most one argument"));

        sq_setinstanceup(vm, 1, dlg);
        sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<EditArrayOrderDlg>::Delete);
        return SC_RETURN_OK;
        //return SqPlus::PostConstruct<EditArrayOrderDlg>(v, dlg, EditArrayOrderDlg_Dtor);
    }

    /*SQInteger EditArrayStringDlg_Dtor(SQUserPointer up, cb_unused SQInteger size)
    {
      SQ_DELETE_CLASS(EditArrayStringDlg);
    }*/

    SQInteger EditArrayStringDlg_Ctor(HSQUIRRELVM vm)
    {
        //    (wxWindow* parent, const wxArrayString& array)
        StackHandler sa(vm);
        EditArrayStringDlg* dlg = nullptr;

        if (sa.GetParamCount() == 2)
            dlg = new EditArrayStringDlg(nullptr, *sa.GetInstance<wxArrayString>( 2));
        else
            return sa.ThrowError(_("EditArrayStringDlg needs one argument"));

        sq_setinstanceup(vm, 1, dlg);
        sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<EditArrayStringDlg>::Delete);
        return SC_RETURN_OK;
        //return SqPlus::PostConstruct<EditArrayStringDlg>(v, dlg, EditArrayStringDlg_Dtor);
    }

    /*SQInteger EditPairDlg_Dtor(SQUserPointer up, cb_unused SQInteger size)
    {
      SQ_DELETE_CLASS(EditPairDlg);
    }*/

    SQInteger EditPairDlg_Ctor(HSQUIRRELVM vm)
    {
        //    (wxWindow* parent,
        //    wxString& key,
        //    wxString& value,
        //    const wxString& title = _("Edit pair"),
        //    BrowseMode allowBrowse = bmDisable);
        StackHandler sa(vm);
        EditPairDlg* dlg = nullptr;

        if (sa.GetParamCount() > 4)
        {
            dlg = new EditPairDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3),
                                    *sa.GetInstance<wxString>( 4),
                                    (EditPairDlg::BrowseMode)sa.GetValue<SQInteger>(5));
        }
        else if (sa.GetParamCount() > 3)
        {
            dlg = new EditPairDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3),
                                    *sa.GetInstance<wxString>( 4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new EditPairDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3));
        }
        else
            return sa.ThrowError(_("EditPairDlg needs at least two arguments"));


        sq_setinstanceup(vm, 1, dlg);
        sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<EditPairDlg>::Delete);
        return SC_RETURN_OK;
        //return SqPlus::PostConstruct<EditPairDlg>(v, dlg, EditPairDlg_Dtor);
    }

    /*SQInteger EditPathDlg_Dtor(SQUserPointer up, cb_unused SQInteger size)
    {
      SQ_DELETE_CLASS(EditPathDlg);
    }*/

    SQInteger EditPathDlg_Ctor(HSQUIRRELVM vm)
    {
        //    (wxWindow* parent,
        //    const wxString& path,       // initial path
        //    const wxString& basepath,   // for relative pathes
        //    const wxString& title = _("Edit Path"),      // title of the dialog
        //    const wxString& message = _T(""),    // message displayed in the dialogs
        //    const bool wantDir = true,  // whether to open a dir or a file dialog
        //    const bool allowMultiSel = false,  // whether to allow for multiple files selection
        //    const wxString& filter = _("All files(*)|*"));  // wildcard for files
        StackHandler sa(vm);
        EditPathDlg* dlg = nullptr;

        if (sa.GetParamCount() > 7)
        {
            dlg = new EditPathDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3),
                                    *sa.GetInstance<wxString>( 4),
                                    *sa.GetInstance<wxString>( 5),
                                    sa.GetValue<bool>(6),
                                    sa.GetValue<bool>(7),
                                    *sa.GetInstance<wxString>( 8));
        }
        else if (sa.GetParamCount() > 6)
        {
            dlg = new EditPathDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3),
                                    *sa.GetInstance<wxString>( 4),
                                    *sa.GetInstance<wxString>( 5),
                                    sa.GetValue<bool>(6),
                                    sa.GetValue<bool>(7));
        }
        else if (sa.GetParamCount() > 5)
        {
            dlg = new EditPathDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3),
                                    *sa.GetInstance<wxString>( 4),
                                    *sa.GetInstance<wxString>( 5),
                                    sa.GetValue<bool>(6));
        }
        else if (sa.GetParamCount() > 4)
        {
            dlg = new EditPathDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3),
                                    *sa.GetInstance<wxString>( 4),
                                    *sa.GetInstance<wxString>( 5));
        }
        else if (sa.GetParamCount() > 3)
        {
            dlg = new EditPathDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3),
                                    *sa.GetInstance<wxString>( 4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new EditPathDlg(nullptr,
                                    *sa.GetInstance<wxString>( 2),
                                    *sa.GetInstance<wxString>( 3));
        }
        else
            return sa.ThrowError(_("EditPathDlg needs at least two arguments"));


        sq_setinstanceup(vm, 1, dlg);
        sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<EditPathDlg>::Delete);
        return SC_RETURN_OK;
    }

    /*SQInteger GenericMultiLineNotesDlg_Dtor(SQUserPointer up, cb_unused SQInteger size)
    {
      SQ_DELETE_CLASS(GenericMultiLineNotesDlg);
    }*/

    SQInteger GenericMultiLineNotesDlg_Ctor(HSQUIRRELVM vm)
    {
        //    (wxWindow* parent,
        //    const wxString& caption = _("Notes"),
        //    const wxString& notes = wxEmptyString,
        //    bool readOnly = true);
        StackHandler sa(vm);
        GenericMultiLineNotesDlg* dlg = nullptr;

        if (sa.GetParamCount() > 3)
        {
            dlg = new GenericMultiLineNotesDlg(nullptr,
                                            *sa.GetInstance<wxString>( 2),
                                            *sa.GetInstance<wxString>( 3),
                                            sa.GetValue<bool>(4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new GenericMultiLineNotesDlg(nullptr,
                                            *sa.GetInstance<wxString>( 2),
                                            *sa.GetInstance<wxString>( 3));
        }
        else if (sa.GetParamCount() > 1)
        {
            dlg = new GenericMultiLineNotesDlg(nullptr,
                                            *sa.GetInstance<wxString>( 2));
        }
        else
            dlg = new GenericMultiLineNotesDlg(nullptr);

        sq_setinstanceup(vm, 1, dlg);
        sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<GenericMultiLineNotesDlg>::Delete);
        return SC_RETURN_OK;
    }

    SQInteger ShowModalForDialogs(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        SQUserPointer up = nullptr;
        sq_getinstanceup(vm, 1, &up, nullptr);
        wxScrollingDialog* dlg = (wxScrollingDialog*)up;
        sa.PushValue<SQInteger>((dlg->ShowModal()));
        return SC_RETURN_VALUE;
    }

    void Register_UtilDialogs(HSQUIRRELVM vm)
    {
        Sqrat::ConstTable(vm).Const("bmBrowseForDirectory",  EditPairDlg::bmBrowseForDirectory);
        Sqrat::ConstTable(vm).Const("bmBrowseForFile",       EditPairDlg::bmBrowseForFile);
        Sqrat::ConstTable(vm).Const("bmDisable",             EditPairDlg::bmDisable);


        // FIXME (bluehazzard#1#): This is a possible error source, because we have a constructor...
        Sqrat::Class<EditArrayFileDlg,Sqrat::NoCopy<EditArrayFileDlg> > array_file_dlg(vm,"EditArrayFileDlg");
        array_file_dlg.
                SquirrelFunc("constructor", &EditArrayFileDlg_Ctor).
                StaticFunc("ShowModal",     &ShowModalForDialogs);
        Sqrat::RootTable(vm).Bind("EditArrayFileDlg",array_file_dlg);

        Sqrat::Class<EditArrayOrderDlg,Sqrat::NoCopy<EditArrayOrderDlg> > array_order_dlg(vm,"EditArrayOrderDlg");
        array_order_dlg.
                SquirrelFunc("constructor",&EditArrayOrderDlg_Ctor).
                StaticFunc("ShowModal", &ShowModalForDialogs).
                Func("SetArray",        &EditArrayOrderDlg::SetArray).
                Func("GetArray",        &EditArrayOrderDlg::GetArray);
        Sqrat::RootTable(vm).Bind("EditArrayOrderDlg",array_order_dlg);

        Sqrat::Class<EditArrayStringDlg,Sqrat::NoCopy<EditArrayStringDlg> > array_string_dlg(vm,"EditArrayStringDlg");
        array_string_dlg.
                SquirrelFunc("constructor", &EditArrayStringDlg_Ctor).
                StaticFunc("ShowModal",     &ShowModalForDialogs);
        Sqrat::RootTable(vm).Bind("EditArrayStringDlg",array_string_dlg);

        Sqrat::Class<EditPairDlg,Sqrat::NoCopy<EditPairDlg> >edit_pair_dlg(vm,"EditPairDlg");
                edit_pair_dlg.
                SquirrelFunc("constructor", &EditPairDlg_Ctor).
                StaticFunc("ShowModal",     &ShowModalForDialogs);

        Sqrat::Class<EditPathDlg,Sqrat::NoCopy<EditPathDlg> > edit_path_dlg(vm,"EditPathDlg");
        edit_path_dlg.  SquirrelFunc("constructor", &EditPathDlg_Ctor).
                        StaticFunc("ShowModal",     &ShowModalForDialogs).
                        Func("GetPath",             &EditPathDlg::GetPath);

        Sqrat::Class<GenericMultiLineNotesDlg,Sqrat::NoCopy<GenericMultiLineNotesDlg> > multi_line_note_dlg(vm,"GenericMultiLineNotesDlg");
        multi_line_note_dlg.
                SquirrelFunc("constructor", &GenericMultiLineNotesDlg_Ctor).
                StaticFunc("ShowModal",     &ShowModalForDialogs).
                Func("GetNotes",            &GenericMultiLineNotesDlg::GetNotes);
    }
} // namespace ScriptBindings
