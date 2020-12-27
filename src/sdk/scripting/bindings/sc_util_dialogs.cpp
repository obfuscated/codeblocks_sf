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
    #include "globals.h"
    #include "cbexception.h"
    #include <wx/string.h>
#endif

#include "editarrayfiledlg.h"
#include "editarrayorderdlg.h"
#include "editarraystringdlg.h"
#include "editpairdlg.h"
#include "editpathdlg.h"
#include "genericmultilinenotesdlg.h"

#include "sc_utils.h"
#include "sc_typeinfo_all.h"


namespace ScriptBindings
{
    // Local type traits.
    // This won't allow other bound functions to extract these types from Squirrel values.

    template<>
    struct TypeInfo<EditArrayFileDlg> {
        static const uint32_t typetag = uint32_t(TypeTag::EditArrayFileDlg);
        static constexpr const SQChar *className = _SC("EditArrayFileDlg");
        using baseClass = void;
    };

    template<>
    struct TypeInfo<EditArrayOrderDlg> {
        static const uint32_t typetag = uint32_t(TypeTag::EditArrayOrderDlg);
        static constexpr const SQChar *className = _SC("EditArrayOrderDlg");
        using baseClass = void;
    };

    template<>
    struct TypeInfo<EditArrayStringDlg> {
        static const uint32_t typetag = uint32_t(TypeTag::EditArrayStringDlg);
        static constexpr const SQChar *className = _SC("EditArrayStringDlg");
        using baseClass = void;
    };

    template<>
    struct TypeInfo<EditPairDlg> {
        static const uint32_t typetag = uint32_t(TypeTag::EditPairDlg);
        static constexpr const SQChar *className = _SC("EditPairDlg");
        using baseClass = void;
    };

    template<>
    struct TypeInfo<EditPathDlg> {
        static const uint32_t typetag = uint32_t(TypeTag::EditPathDlg);
        static constexpr const SQChar *className = _SC("EditPathDlg");
        using baseClass = void;
    };

    template<>
    struct TypeInfo<GenericMultiLineNotesDlg> {
        static const uint32_t typetag = uint32_t(TypeTag::GenericMultiLineNotesDlg);
        static constexpr const SQChar *className = _SC("GenericMultiLineNotesDlg");
        using baseClass = void;
    };

    SQInteger EditArrayFileDlg_Ctor(HSQUIRRELVM v)
    {
        // env table, array, useRelativePaths = false, basePath = _T("")
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(2, 4, "EditArrayFileDlg::contructor"))
            return extractor.ErrorMessage();
        const int numArgs = sq_gettop(v);

        wxString basePath;
        if (numArgs == 4)
        {
            const wxString *value;
            if (!extractor.ProcessParam(value, 4, "EditArrayFileDlg::contructor"))
                return extractor.ErrorMessage();
            basePath = *value;
        }
        bool useRelativePaths = false;
        if (numArgs >= 3)
        {
            if (!extractor.ProcessParam(useRelativePaths, 3, "EditArrayFileDlg::contructor"))
                return extractor.ErrorMessage();
        }
        wxArrayString *array;
        if (!extractor.ProcessParam(array, 2, "EditArrayFileDlg::contructor"))
            return extractor.ErrorMessage();

        UserDataForType<EditArrayFileDlg> *data;
        data = SetupUserPointer<EditArrayFileDlg, InstanceAllocationMode::InstanceIsInline>(v, 1);
        if (!data)
            return -1; // SetupUserPointer should have called sq_throwerror!
        new (&(data->userdata)) EditArrayFileDlg(Manager::Get()->GetAppWindow(), *array,
                                                 useRelativePaths, basePath);
        return 0;
    }

    SQInteger EditArrayOrderDlg_Ctor(HSQUIRRELVM v)
    {
        // env table, array = nullptr
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(1, 2, "EditArrayOrderDlg::contructor"))
            return extractor.ErrorMessage();
        const int numArgs = sq_gettop(v);
        wxArrayString *array = nullptr;
        if (numArgs == 2)
        {
            if (!extractor.ProcessParam(array, 2, "EditArrayOrderDlg::contructor"))
                return extractor.ErrorMessage();
        }

        UserDataForType<EditArrayOrderDlg> *data;
        data = SetupUserPointer<EditArrayOrderDlg, InstanceAllocationMode::InstanceIsInline>(v, 1);
        if (!data)
            return -1; // SetupUserPointer should have called sq_throwerror!
        new (&(data->userdata)) EditArrayOrderDlg(Manager::Get()->GetAppWindow(),
                                                  (array ? *array : wxArrayString()));
        return 0;
    }

    SQInteger EditArrayOrderDlg_SetArray(HSQUIRRELVM v)
    {
        // this, array
        ExtractParams2<EditArrayOrderDlg*, const wxArrayString *> extractor(v);
        if (!extractor.Process("EditArrayOrderDlg::SetArray"))
            return extractor.ErrorMessage();

        extractor.p0->SetArray(*extractor.p1);
        return 0;
    }

    SQInteger EditArrayOrderDlg_GetArray(HSQUIRRELVM v)
    {
        // this, array
        ExtractParams1<EditArrayOrderDlg*> extractor(v);
        if (!extractor.Process("EditArrayOrderDlg::GetArray"))
            return extractor.ErrorMessage();

        // FIXME (squirrel) This doesn't matter much, because squirrel doesn't care for constness.
        wxArrayString &result = const_cast<wxArrayString&>(extractor.p0->GetArray());
        return ConstructAndReturnNonOwnedPtr(v, &result);
    }

    SQInteger EditArrayStringDlg_Ctor(HSQUIRRELVM v)
    {
        // env table, array
        ExtractParams2<SkipParam, wxArrayString *> extractor(v);
        if (!extractor.Process("EditArrayStringDlg::constructor"))
            return extractor.ErrorMessage();

        UserDataForType<EditArrayStringDlg> *data;
        data = SetupUserPointer<EditArrayStringDlg, InstanceAllocationMode::InstanceIsInline>(v, 1);
        if (!data)
            return -1; // SetupUserPointer should have called sq_throwerror!
        new (&(data->userdata)) EditArrayStringDlg(Manager::Get()->GetAppWindow(), *extractor.p1);
        return 0;
    }

    SQInteger EditPairDlg_Ctor(HSQUIRRELVM v)
    {
        // env table, key, value, title = _("Edit pair"), allowBrowse = bmDisable
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(3, 5, "EditPairDlg::contructor"))
            return extractor.ErrorMessage();
        const int numArgs = sq_gettop(v);

        wxString *key, *value;
        if (!extractor.ProcessParam(key, 2, "EditPairDlg::contructor"))
            return extractor.ErrorMessage();
        if (!extractor.ProcessParam(value, 3, "EditPairDlg::contructor"))
            return extractor.ErrorMessage();

        const wxString *title = nullptr;
        if (numArgs >= 4)
        {
            if (!extractor.ProcessParam(title, 4, "EditPairDlg::contructor"))
                return extractor.ErrorMessage();
        }

        EditPairDlg::BrowseMode allowBrowse = EditPairDlg::bmDisable;
        if (numArgs == 5)
        {
            SQInteger value;
            if (!extractor.ProcessParam(value, 5, "EditPairDlg::contructor"))
                return extractor.ErrorMessage();
            if (value < EditPairDlg::bmDisable || value > EditPairDlg::bmBrowseForDirectory)
                return sq_throwerror(v, _SC("EditPairDlg::contructor: Invalid value for parameter 'allowBrowse'"));
            allowBrowse = EditPairDlg::BrowseMode(value);
        }

        UserDataForType<EditPairDlg> *data;
        data = SetupUserPointer<EditPairDlg, InstanceAllocationMode::InstanceIsInline>(v, 1);
        if (!data)
            return -1; // SetupUserPointer should have called sq_throwerror!
        new (&(data->userdata)) EditPairDlg(Manager::Get()->GetAppWindow(), *key, *value,
                                            (title ? *title : _("Edit pair")), allowBrowse);
        return 0;
    }

    SQInteger EditPathDlg_Ctor(HSQUIRRELVM v)
    {
        // env table, path, basepath, title =_("Edit path"), message = "", wantDir = true, allowMultiSel = false, filter = _("All files(*)|*")
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(3, 8, "EditPathDlg::contructor"))
            return extractor.ErrorMessage();
        const int numArgs = sq_gettop(v);

        const wxString *filter = nullptr;
        if (numArgs == 8)
        {
            if (!extractor.ProcessParam(filter, 8, "EditPathDlg::contructor"))
                return extractor.ErrorMessage();
        }

        bool allowMultiSel = false;
        if (numArgs >= 7)
        {
            if (!extractor.ProcessParam(allowMultiSel, 7, "EditPathDlg::contructor"))
                return extractor.ErrorMessage();
        }

        bool wantDir = false;
        if (numArgs >= 6)
        {
            if (!extractor.ProcessParam(wantDir, 6, "EditPathDlg::contructor"))
                return extractor.ErrorMessage();
        }

        const wxString *message = nullptr;
        if (numArgs >= 5)
        {
            if (!extractor.ProcessParam(message, 5, "EditPathDlg::contructor"))
                return extractor.ErrorMessage();
        }

        const wxString *title = nullptr;
        if (numArgs >= 4)
        {
            if (!extractor.ProcessParam(title, 4, "EditPathDlg::contructor"))
                return extractor.ErrorMessage();
        }

        const wxString *basepath;
        if (!extractor.ProcessParam(basepath, 3, "EditPathDlg::contructor"))
            return extractor.ErrorMessage();
        const wxString *path;
        if (!extractor.ProcessParam(path, 2, "EditPathDlg::contructor"))
            return extractor.ErrorMessage();

        UserDataForType<EditPathDlg> *data;
        data = SetupUserPointer<EditPathDlg, InstanceAllocationMode::InstanceIsInline>(v, 1);
        if (!data)
            return -1; // SetupUserPointer should have called sq_throwerror!
        new (&(data->userdata)) EditPathDlg(Manager::Get()->GetAppWindow(), *path, *basepath,
                                            (title ? *title : _("Edit Path")),
                                            (message ? *message : wxString()), wantDir,
                                            allowMultiSel, (filter ? *filter: _("All files(*)|*")));
        return 0;
    }

    SQInteger EditPathDlg_GetPath(HSQUIRRELVM v)
    {
        // this, array
        ExtractParams1<EditPathDlg*> extractor(v);
        if (!extractor.Process("EditPathDlg::GetPath"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, extractor.p0->GetPath());
    }

    SQInteger GenericMultiLineNotesDlg_Ctor(HSQUIRRELVM v)
    {
        // env table, caption = _("Notes"), notes = wxString(), readOnly = true;
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(1, 4, "GenericMultiLineNotesDlg::contructor"))
            return extractor.ErrorMessage();
        const int numArgs = sq_gettop(v);

        bool readOnly = true;
        if (numArgs == 4)
        {
            if (!extractor.ProcessParam(readOnly, 4, "GenericMultiLineNotesDlg::contructor"))
                return extractor.ErrorMessage();
        }
        const wxString *notes = nullptr;
        if (numArgs >= 3)
        {
            if (!extractor.ProcessParam(notes, 3, "GenericMultiLineNotesDlg::contructor"))
                return extractor.ErrorMessage();
        }
        const wxString *caption = nullptr;
        if (numArgs >= 2)
        {
            if (!extractor.ProcessParam(caption, 2, "GenericMultiLineNotesDlg::contructor"))
                return extractor.ErrorMessage();
        }

        UserDataForType<GenericMultiLineNotesDlg> *data;
        data = SetupUserPointer<GenericMultiLineNotesDlg, InstanceAllocationMode::InstanceIsInline>(v, 1);
        if (!data)
            return -1; // SetupUserPointer should have called sq_throwerror!
        new (&(data->userdata)) GenericMultiLineNotesDlg(Manager::Get()->GetAppWindow(),
                                                         (caption ? *caption : _("Notes")),
                                                         (notes ? *notes : wxString()),
                                                         readOnly);
        return 0;
    }

    SQInteger GenericMultiLineNotesDlg_GetNotes(HSQUIRRELVM v)
    {
        // this, array
        ExtractParams1<GenericMultiLineNotesDlg*> extractor(v);
        if (!extractor.Process("GenericMultiLineNotesDlg::GetNotes"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, extractor.p0->GetNotes());
    }

    template<typename DialogType>
    SQInteger ShowModalForDialogs(HSQUIRRELVM v)
    {
        // this
        ExtractParams1<DialogType *> extractor(v);
        if (!extractor.Process("ShowModalForDialogs"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, SQInteger(extractor.p0->ShowModal()));
        return 1;
    }

    void Register_UtilDialogs(HSQUIRRELVM v)
    {
        PreserveTop preserveTop(v);
        sq_pushroottable(v);

// FIXME (squirrel) Bind these constants!
//        SqPlus::BindConstant<SQInteger>(EditPairDlg::bmBrowseForDirectory, "bmBrowseForDirectory");
//        SqPlus::BindConstant<SQInteger>(EditPairDlg::bmBrowseForFile, "bmBrowseForFile");
//        SqPlus::BindConstant<SQInteger>(EditPairDlg::bmDisable, "bmDisable");

        {
            // Register EditArrayFileDlg
            const SQInteger classDecl = CreateClassDecl<EditArrayFileDlg>(v, _SC("EditArrayFileDlg"));
            BindMethod(v, _SC("constructor"), EditArrayFileDlg_Ctor,
                       _SC("EditArrayFileDlg::constructor"));
            BindMethod(v, _SC("ShowModal"), ShowModalForDialogs<EditArrayFileDlg>,
                       _SC("EditArrayFileDlg::ShowModal"));

            BindDefaultInstanceCmp<EditArrayFileDlg>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditArrayOrderDlg
            const SQInteger classDecl = CreateClassDecl<EditArrayOrderDlg>(v, _SC("EditArrayOrderDlg"));
            BindMethod(v, _SC("constructor"), EditArrayOrderDlg_Ctor,
                       _SC("EditArrayOrderDlg::constructor"));
            BindMethod(v, _SC("ShowModal"), ShowModalForDialogs<EditArrayOrderDlg>,
                       _SC("EditArrayOrderDlg::ShowModal"));
            BindMethod(v, _SC("SetArray"), EditArrayOrderDlg_SetArray,
                       _SC("EditArrayOrderDlg::SetArray"));
            BindMethod(v, _SC("GetArray"), EditArrayOrderDlg_GetArray,
                       _SC("EditArrayOrderDlg::GetArray"));

            BindDefaultInstanceCmp<EditArrayOrderDlg>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditArrayStringDlg
            const SQInteger classDecl = CreateClassDecl<EditArrayStringDlg>(v, _SC("EditArrayStringDlg"));
            BindMethod(v, _SC("constructor"), EditArrayStringDlg_Ctor,
                       _SC("EditArrayStringDlg::constructor"));
            BindMethod(v, _SC("ShowModal"), ShowModalForDialogs<EditArrayStringDlg>,
                       _SC("EditArrayStringDlg::ShowModal"));

            BindDefaultInstanceCmp<EditArrayStringDlg>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditPairDlg
            const SQInteger classDecl = CreateClassDecl<EditPairDlg>(v, _SC("EditPairDlg"));
            BindMethod(v, _SC("constructor"), EditPairDlg_Ctor,
                       _SC("EditPairDlg::constructor"));
            BindMethod(v, _SC("ShowModal"), ShowModalForDialogs<EditPairDlg>,
                       _SC("EditPairDlg::ShowModal"));

            BindDefaultInstanceCmp<EditPairDlg>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register EditPathDlg
            const SQInteger classDecl = CreateClassDecl<EditPathDlg>(v, _SC("EditPathDlg"));
            BindMethod(v, _SC("constructor"), EditPathDlg_Ctor,
                       _SC("EditPathDlg::constructor"));
            BindMethod(v, _SC("ShowModal"), ShowModalForDialogs<EditPathDlg>,
                       _SC("EditPathDlg::ShowModal"));
            BindMethod(v, _SC("GetPath"), EditPathDlg_GetPath, _SC("EditPathDlg::GetPath"));

            BindDefaultInstanceCmp<EditPairDlg>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        {
            // Register GenericMultiLineNotesDlg
            const SQInteger classDecl = CreateClassDecl<GenericMultiLineNotesDlg>(v, _SC("GenericMultiLineNotesDlg"));
            BindMethod(v, _SC("constructor"), GenericMultiLineNotesDlg_Ctor,
                       _SC("GenericMultiLineNotesDlg::constructor"));
            BindMethod(v, _SC("ShowModal"), ShowModalForDialogs<GenericMultiLineNotesDlg>,
                       _SC("GenericMultiLineNotesDlg::ShowModal"));
            BindMethod(v, _SC("GetNotes"), GenericMultiLineNotesDlg_GetNotes,
                       _SC("GenericMultiLineNotesDlg::GetNotes"));

            BindDefaultInstanceCmp<EditPairDlg>(v);
            // Put the class in the root table. This must be last!
            sq_newslot(v, classDecl, SQFalse);
        }

        sq_pop(v, 1); // pop root table
    }
} // namespace ScriptBindings
