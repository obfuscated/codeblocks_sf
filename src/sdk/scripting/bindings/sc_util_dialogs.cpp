#include <sdk_precomp.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <cbexception.h>
    #include <wx/string.h>
#endif

#include "sc_base_types.h"

#include <editarrayfiledlg.h>
#include <editarrayorderdlg.h>
#include <editarraystringdlg.h>
#include <editpairdlg.h>
#include <editpathdlg.h>
#include <genericmultilinenotesdlg.h>

DECLARE_ENUM_TYPE(EditPairDlg::BrowseMode);

namespace ScriptBindings
{
    SQInteger EditArrayFileDlg_Dtor(SQUserPointer up, SQInteger size)
    {
      SQ_DELETE_CLASS(EditArrayFileDlg);
    }

    SQInteger EditArrayFileDlg_Ctor(HSQUIRRELVM v)
    {
        //    (wxWindow* parent,
        //    wxArrayString& array,
        //    bool useRelativePaths = false,
        //    const wxString& basePath = _T(""))
        StackHandler sa(v);
        EditArrayFileDlg* dlg = 0;

        if (sa.GetParamCount() > 3)
        {
            dlg = new EditArrayFileDlg(0,
                                        *SqPlus::GetInstance<wxArrayString>(v, 2),
                                        sa.GetBool(3),
                                        *SqPlus::GetInstance<wxString>(v, 4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new EditArrayFileDlg(0,
                                        *SqPlus::GetInstance<wxArrayString>(v, 2),
                                        sa.GetBool(3));
        }
        else if (sa.GetParamCount() > 1)
        {
            dlg = new EditArrayFileDlg(0,
                                        *SqPlus::GetInstance<wxArrayString>(v, 2));
        }
        else
            return sa.ThrowError("EditArrayFileDlg needs at least one argument");

        return SqPlus::PostConstruct<EditArrayFileDlg>(v, dlg, EditArrayFileDlg_Dtor);
    }

    SQInteger EditArrayOrderDlg_Dtor(SQUserPointer up, SQInteger size)
    {
      SQ_DELETE_CLASS(EditArrayOrderDlg);
    }

    SQInteger EditArrayOrderDlg_Ctor(HSQUIRRELVM v)
    {
        //    (wxWindow* parent, const wxArrayString& array = 0L)
        StackHandler sa(v);
        EditArrayOrderDlg* dlg = 0;

        if (sa.GetParamCount() == 1)
            dlg = new EditArrayOrderDlg(0);
        else if (sa.GetParamCount() == 2)
            dlg = new EditArrayOrderDlg(0, *SqPlus::GetInstance<wxArrayString>(v, 2));
        else
            return sa.ThrowError("EditArrayOrderDlg needs at most one argument");

        return SqPlus::PostConstruct<EditArrayOrderDlg>(v, dlg, EditArrayOrderDlg_Dtor);
    }

    SQInteger EditArrayStringDlg_Dtor(SQUserPointer up, SQInteger size)
    {
      SQ_DELETE_CLASS(EditArrayStringDlg);
    }

    SQInteger EditArrayStringDlg_Ctor(HSQUIRRELVM v)
    {
        //    (wxWindow* parent, const wxArrayString& array)
        StackHandler sa(v);
        EditArrayStringDlg* dlg = 0;

        if (sa.GetParamCount() == 2)
            dlg = new EditArrayStringDlg(0, *SqPlus::GetInstance<wxArrayString>(v, 2));
        else
            return sa.ThrowError("EditArrayStringDlg needs one argument");

        return SqPlus::PostConstruct<EditArrayStringDlg>(v, dlg, EditArrayStringDlg_Dtor);
    }

    SQInteger EditPairDlg_Dtor(SQUserPointer up, SQInteger size)
    {
      SQ_DELETE_CLASS(EditPairDlg);
    }

    SQInteger EditPairDlg_Ctor(HSQUIRRELVM v)
    {
        //    (wxWindow* parent,
        //    wxString& key,
        //    wxString& value,
        //    const wxString& title = _("Edit pair"),
        //    BrowseMode allowBrowse = bmDisable);
        StackHandler sa(v);
        EditPairDlg* dlg = 0;

        if (sa.GetParamCount() > 4)
        {
            dlg = new EditPairDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3),
                                    *SqPlus::GetInstance<wxString>(v, 4),
                                    (EditPairDlg::BrowseMode)sa.GetInt(5));
        }
        else if (sa.GetParamCount() > 3)
        {
            dlg = new EditPairDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3),
                                    *SqPlus::GetInstance<wxString>(v, 4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new EditPairDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3));
        }
        else
            return sa.ThrowError("EditPairDlg needs at least two arguments");

        return SqPlus::PostConstruct<EditPairDlg>(v, dlg, EditPairDlg_Dtor);
    }

    SQInteger EditPathDlg_Dtor(SQUserPointer up, SQInteger size)
    {
      SQ_DELETE_CLASS(EditPathDlg);
    }

    SQInteger EditPathDlg_Ctor(HSQUIRRELVM v)
    {
        //    (wxWindow* parent,
        //    const wxString& path,       // initial path
        //    const wxString& basepath,   // for relative pathes
        //    const wxString& title = _("Edit Path"),      // title of the dialog
        //    const wxString& message = _T(""),    // message displayed in the dialogs
        //    const bool wantDir = true,  // whether to open a dir or a file dialog
        //    const bool allowMultiSel = false,  // whether to allow for multiple files selection
        //    const wxString& filter = _("All files(*)|*"));  // wildcard for files
        StackHandler sa(v);
        EditPathDlg* dlg = 0;

        if (sa.GetParamCount() > 7)
        {
            dlg = new EditPathDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3),
                                    *SqPlus::GetInstance<wxString>(v, 4),
                                    *SqPlus::GetInstance<wxString>(v, 5),
                                    sa.GetBool(6),
                                    sa.GetBool(7),
                                    *SqPlus::GetInstance<wxString>(v, 8));
        }
        else if (sa.GetParamCount() > 6)
        {
            dlg = new EditPathDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3),
                                    *SqPlus::GetInstance<wxString>(v, 4),
                                    *SqPlus::GetInstance<wxString>(v, 5),
                                    sa.GetBool(6),
                                    sa.GetBool(7));
        }
        else if (sa.GetParamCount() > 5)
        {
            dlg = new EditPathDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3),
                                    *SqPlus::GetInstance<wxString>(v, 4),
                                    *SqPlus::GetInstance<wxString>(v, 5),
                                    sa.GetBool(6));
        }
        else if (sa.GetParamCount() > 4)
        {
            dlg = new EditPathDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3),
                                    *SqPlus::GetInstance<wxString>(v, 4),
                                    *SqPlus::GetInstance<wxString>(v, 5));
        }
        else if (sa.GetParamCount() > 3)
        {
            dlg = new EditPathDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3),
                                    *SqPlus::GetInstance<wxString>(v, 4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new EditPathDlg(0,
                                    *SqPlus::GetInstance<wxString>(v, 2),
                                    *SqPlus::GetInstance<wxString>(v, 3));
        }
        else
            return sa.ThrowError("EditPathDlg needs at least two arguments");

        return SqPlus::PostConstruct<EditPathDlg>(v, dlg, EditPathDlg_Dtor);
    }

    SQInteger GenericMultiLineNotesDlg_Dtor(SQUserPointer up, SQInteger size)
    {
      SQ_DELETE_CLASS(GenericMultiLineNotesDlg);
    }

    SQInteger GenericMultiLineNotesDlg_Ctor(HSQUIRRELVM v)
    {
        //    (wxWindow* parent,
        //    const wxString& caption = _("Notes"),
        //    const wxString& notes = wxEmptyString,
        //    bool readOnly = true);
        StackHandler sa(v);
        GenericMultiLineNotesDlg* dlg = 0;

        if (sa.GetParamCount() > 3)
        {
            dlg = new GenericMultiLineNotesDlg(0,
                                            *SqPlus::GetInstance<wxString>(v, 2),
                                            *SqPlus::GetInstance<wxString>(v, 3),
                                            sa.GetBool(4));
        }
        else if (sa.GetParamCount() > 2)
        {
            dlg = new GenericMultiLineNotesDlg(0,
                                            *SqPlus::GetInstance<wxString>(v, 2),
                                            *SqPlus::GetInstance<wxString>(v, 3));
        }
        else if (sa.GetParamCount() > 1)
        {
            dlg = new GenericMultiLineNotesDlg(0,
                                            *SqPlus::GetInstance<wxString>(v, 2));
        }
        else
            dlg = new GenericMultiLineNotesDlg(0);

        return SqPlus::PostConstruct<GenericMultiLineNotesDlg>(v, dlg, GenericMultiLineNotesDlg_Dtor);
    }

    SQInteger ShowModalForDialogs(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        SQUserPointer up = 0;
        sq_getinstanceup(v, 1, &up, 0);
        wxDialog* dlg = (wxDialog*)up;
        return sa.Return(static_cast<SQInteger>(dlg->ShowModal()));
    }

    void Register_UtilDialogs()
    {
        SqPlus::BindConstant<SQInteger>(EditPairDlg::bmBrowseForDirectory, "bmBrowseForDirectory");
        SqPlus::BindConstant<SQInteger>(EditPairDlg::bmBrowseForFile, "bmBrowseForFile");
        SqPlus::BindConstant<SQInteger>(EditPairDlg::bmDisable, "bmDisable");

        SqPlus::SQClassDef<EditArrayFileDlg>("EditArrayFileDlg").
                staticFuncVarArgs(&EditArrayFileDlg_Ctor, "constructor", "*").
                staticFunc(&ShowModalForDialogs, "ShowModal");

        SqPlus::SQClassDef<EditArrayOrderDlg>("EditArrayOrderDlg").
                staticFuncVarArgs(&EditArrayOrderDlg_Ctor, "constructor", "*").
                staticFunc(&ShowModalForDialogs, "ShowModal").
                func(&EditArrayOrderDlg::SetArray, "SetArray").
                func(&EditArrayOrderDlg::GetArray, "GetArray");

        SqPlus::SQClassDef<EditArrayStringDlg>("EditArrayStringDlg").
                staticFuncVarArgs(&EditArrayStringDlg_Ctor, "constructor", "*").
                staticFunc(&ShowModalForDialogs, "ShowModal");

        SqPlus::SQClassDef<EditPairDlg>("EditPairDlg").
                staticFuncVarArgs(&EditPairDlg_Ctor, "constructor", "*").
                staticFunc(&ShowModalForDialogs, "ShowModal");

        SqPlus::SQClassDef<EditPathDlg>("EditPathDlg").
                staticFuncVarArgs(&EditPathDlg_Ctor, "constructor", "*").
                staticFunc(&ShowModalForDialogs, "ShowModal").
                func(&EditPathDlg::GetPath, "GetPath");

        SqPlus::SQClassDef<GenericMultiLineNotesDlg>("GenericMultiLineNotesDlg").
                staticFuncVarArgs(&GenericMultiLineNotesDlg_Ctor, "constructor", "*").
                staticFunc(&ShowModalForDialogs, "ShowModal").
                func(&GenericMultiLineNotesDlg::GetNotes, "GetNotes");
    }
} // namespace ScriptBindings
