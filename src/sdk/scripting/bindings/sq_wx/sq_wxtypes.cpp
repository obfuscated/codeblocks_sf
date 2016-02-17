/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"
#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include <globals.h>
#endif
#include <wx/filename.h>
#include <wx/colour.h>

#include "scripting/bindings/sc_binding_util.h"
#include "scripting/bindings/sc_base_types.h"
#include "scripting/bindings/sc_cb_vm.h"

// FIXME (bluehazzard#1#): Error Handling has to be improved...

namespace ScriptBindings
{
    ///////////////////
    // wxArrayString //
    ///////////////////
    SQInteger wxArrayString_Index(HSQUIRRELVM v)
    {
        CompileTimeAssertion<wxMinimumVersion<2,8>::eval>::Assert();
        StackHandler sa(v);
        try
        {
            wxArrayString& self = *sa.GetInstance<wxArrayString>(1);
            wxString inpstr = sa.GetValue<wxString>(2);

            bool chkCase = true;
            bool frmEnd = false;
            if (sa.GetParamCount() >= 3)
                chkCase = sa.GetValue<bool>(3);
            if (sa.GetParamCount() == 4)
                frmEnd = sa.GetValue<bool>(4);
            sa.PushValue<SQInteger>(self.Index(inpstr.c_str(), chkCase, frmEnd));
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    SQInteger wxArrayString_SetItem(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            if (sa.GetParamCount() != 3)
                return sa.ThrowError(wxT("wxArrayString::SetItem wrong number of parameters!"));

            wxArrayString& self = *sa.GetInstance<wxArrayString>(1);
            int index = sa.GetValue<int>(2);
            if (index < 0 || size_t(index) >= self.GetCount())
                return sa.ThrowError(wxT("wxArrayString::SetItem index out of bounds!"));

            const wxString &value = *sa.GetInstance<wxString>(3);
            self[index] = value;
            return SC_RETURN_OK;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    SQInteger wxArrayString_Item(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
            wxArrayString* inst = sa.GetInstance<wxArrayString>(1);
            int pos = sa.GetValue<int>(2);

            if(inst->GetCount() == 0 || inst->GetCount() < (size_t) pos)
                return sa.ThrowError("wxArrayString: GetCount < pos");

            sa.PushValue<wxString>(inst->Item(pos));
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    //////////////
    // wxColour //
    //////////////
    SQInteger wxColour_OpToString(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxColour& self = *sa.GetInstance<wxColour>(1);
            wxString str = wxString::Format(_T("[r=%d, g=%d, b=%d]"), self.Red(), self.Green(), self.Blue());
            sa.PushValue<const SQChar*>(str.mb_str(wxConvUTF8).data());
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    SQInteger wxColour_IsOk(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxColour& self = *sa.GetInstance<wxColour>(1);
            sa.PushValue<bool>(self.IsOk());
            return SC_RETURN_VALUE;

        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    ////////////////
    // wxFileName //
    ////////////////
    SQInteger wxFileName_OpToString(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        try
        {
            wxFileName& self = *sa.GetInstance<wxFileName>(1);
            sa.PushValue<const SQChar*>(self.GetFullPath().mb_str(wxConvUTF8));
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    SQInteger wxFileName_GetPathSeparator(HSQUIRRELVM vm)
    {
         StackHandler sa(vm);
        try
        {
            wxPathFormat  format = sa.GetValue<wxPathFormat>(2);
            sa.PushValue<wxString>(wxString(wxFileName::GetPathSeparator(format)));
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }


    /////////////
    // wxPoint //
    /////////////

    // wxPoint operator >= <= > <
    SQInteger wxPoint_OpCmp(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxPoint& self = *sa.GetInstance<wxPoint>(1);
            wxPoint& other = *sa.GetInstance<wxPoint>(2);
            int ret = 0;
            if(self != other)
                ret = 1;
            sa.PushValue<SQInteger>(ret);
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    // wxPoint compare
    SQInteger wxPoint_Cmp(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxPoint& self = *sa.GetInstance<wxPoint>(1);
            wxPoint& other = *sa.GetInstance<wxPoint>(2);
            sa.PushValue<bool>(self==other);
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }


    SQInteger wxPoint_OpTostring(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxPoint& self = *sa.GetInstance<wxPoint>(1);
            wxString output;
            output.Printf(_("[%d,%d]"),self.x,self.y);
            sa.PushValue<const SQChar*>(output.mb_str(wxConvUTF8));
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

#if wxCHECK_VERSION(3, 0, 0)
        typedef const wxString& (wxArrayString::*WXARRAY_STRING_ITEM)(size_t nIndex) const;
#else
        typedef wxString& (wxArrayString::*WXARRAY_STRING_ITEM)(size_t nIndex) const;
#endif

    SQInteger wxPoint_x(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxPoint& self = *sa.GetInstance<wxPoint>(1);
            sa.PushValue<SQInteger>(self.x);
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }
    SQInteger wxPoint_y(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxPoint& self = *sa.GetInstance<wxPoint>(1);
            sa.PushValue<SQInteger>(self.y);
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    SQInteger wxSize_OpTostring(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxSize& self = *sa.GetInstance<wxSize>(1);
            wxString output;
            output.Printf(_("[%d,%d]"),self.GetX(),self.GetY());
            sa.PushValue<const SQChar*>(output.mb_str(wxConvUTF8));
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }

    SQInteger wxSize_Cmp(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        try
        {
            wxSize& self  = *sa.GetInstance<wxSize>(1);
            wxSize& other = *sa.GetInstance<wxSize>(2);
            sa.PushValue<bool>(self==other);
            return SC_RETURN_VALUE;
        } catch(CBScriptException &e)
        {
            return sa.ThrowError(e.Message());
        }
    }
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////

    void Register_wxTypes(HSQUIRRELVM vm)
    {

        ///////////////////
        // wxArrayString //
        ///////////////////
        Sqrat::Class<wxArrayString> array_string(vm,"wxArrayString");
                array_string
                //emptyCtor().
                //Ctor<>().
                .Func("Add",     &wxArrayString::Add )
                .Func("Clear",   &wxArrayString::Clear )
                .SquirrelFunc("Index",   &wxArrayString_Index)
                .Func("GetCount",        &wxArrayString::GetCount)
                .SquirrelFunc("Item",    &wxArrayString_Item)
                .SquirrelFunc("SetItem",    &wxArrayString_SetItem);

        Sqrat::RootTable(vm).Bind("wxArrayString",array_string);

        //////////////
        // wxColour //
        //////////////
        typedef void(wxColour::*WXC_SET)(const unsigned char, const unsigned char, const unsigned char, const unsigned char);
        Sqrat::Class<wxColour> wx_colour(vm,"wxColour");
                wx_colour.
                Ctor<unsigned char,unsigned char,unsigned char>().
                //emptyCtor().
                SquirrelFunc("_tostring",   &wxColour_OpToString).
                Func("Blue",    &wxColour::Blue).
                Func("Green",   &wxColour::Green).
                Func("Red",     &wxColour::Red).
                SquirrelFunc("IsOk", &wxColour_IsOk).
                Func<WXC_SET>("Set",    &wxColour::Set);
        Sqrat::RootTable(vm).Bind("wxColour",wx_colour);

        ////////////////
        // wxFileName //
        ////////////////
        typedef void(wxFileName::*WXFN_ASSIGN_FN)(const wxFileName&);
        typedef void(wxFileName::*WXFN_ASSIGN_STR)(const wxString&, wxPathFormat);
        typedef wxString(wxFileName::*WXFN_GETPATH)(int, wxPathFormat)const;
#if wxCHECK_VERSION(3, 0, 0)
        typedef bool(wxFileName::*WXFN_SETCWD)()const;
#else
        typedef bool(wxFileName::*WXFN_SETCWD)();
#endif
        typedef bool(wxFileName::*WXFN_ISFILEWRITEABLE)()const;

        Sqrat::Class<wxFileName> wx_filename(vm,"wxFileName");
                wx_filename.
                //emptyCtor().
                SquirrelFunc("_tostring",       &wxFileName_OpToString).
                Func<WXFN_ASSIGN_FN>("Assign",  &wxFileName::Assign).
                Func<WXFN_ASSIGN_STR>("Assign", &wxFileName::Assign).
                Func("AssignCwd",       &wxFileName::AssignCwd).
                Func("AssignDir",       &wxFileName::AssignDir).
                Func("AssignHomeDir",   &wxFileName::AssignHomeDir).
                Func("Clear",           &wxFileName::Clear).
                Func("ClearExt",        &wxFileName::ClearExt).
/// TODO (bluehazzard#1#): GetCwd missing
//                Func("GetCwd",&wxFileName::GetCwd).
                Func("GetDirCount",     &wxFileName::GetDirCount).
                Func("GetDirs",         &wxFileName::GetDirs).
                Func("GetExt",          &wxFileName::GetExt).
                Func("GetFullName",     &wxFileName::GetFullName).
                Func("GetFullPath",     &wxFileName::GetFullPath).
                Func("GetLongPath",     &wxFileName::GetLongPath).
                Func("GetName",         &wxFileName::GetName).
                Func<WXFN_GETPATH>("GetPath",&wxFileName::GetPath).
                Func("GetShortPath",    &wxFileName::GetShortPath).
                Func("GetVolume",       &wxFileName::GetVolume).
                Func("HasExt",          &wxFileName::HasExt).
                Func("HasName",         &wxFileName::HasName).
                Func("HasVolume",       &wxFileName::HasVolume).
                Func("InsertDir",       &wxFileName::InsertDir).
                Func("IsAbsolute",      &wxFileName::IsAbsolute).
                Func("IsOk",            &wxFileName::IsOk).
                Func("IsRelative",      &wxFileName::IsRelative).
                Func("IsDir",           &wxFileName::IsDir).
                Func("MakeAbsolute",    &wxFileName::MakeAbsolute).
                Func("MakeRelativeTo",  &wxFileName::MakeRelativeTo).
                Func("Normalize",       &wxFileName::Normalize).
                Func("PrependDir",      &wxFileName::PrependDir).
                Func("RemoveDir",       &wxFileName::RemoveDir).
                Func("RemoveLastDir",   &wxFileName::RemoveLastDir).
                Func("SameAs",          &wxFileName::SameAs).
                Func<WXFN_SETCWD>("SetCwd",&wxFileName::SetCwd).
                Func("SetExt",          &wxFileName::SetExt).
                Func("SetEmptyExt",     &wxFileName::SetEmptyExt).
                Func("SetFullName",     &wxFileName::SetFullName).
                Func("SetName",         &wxFileName::SetName).
                Func("SetVolume",       &wxFileName::SetVolume).
                Func<WXFN_ISFILEWRITEABLE>("IsFileWritable",&wxFileName::IsFileWritable).
                StaticFunc("GetPathSeparators", &wxFileName::GetPathSeparators).
                SquirrelFunc("GetPathSeparator", &wxFileName_GetPathSeparator);

        Sqrat::RootTable(vm).Bind("wxFileName",wx_filename);

        /////////////
        // wxPoint //
        /////////////
        Sqrat::Class<wxPoint> wx_point(vm,"wxPoint");
        wx_point.
            Ctor().
            Ctor<int,int>().
            SquirrelFunc("_cmp",      &wxPoint_OpCmp).
            SquirrelFunc("Cmp",       &wxPoint_Cmp).
            SquirrelFunc("_tostring", &wxPoint_OpTostring).
            Var("x",    &wxPoint::x).
            Var("y",    &wxPoint::y);
        Sqrat::RootTable(vm).Bind("wxPoint",wx_point);

        ////////////
        // wxSize //
        ////////////
        typedef void(wxSize::*WXS_SET)(int, int);
        typedef void(wxSize::*WXS_SETH)(int);
        typedef void(wxSize::*WXS_SETW)(int);
        Sqrat::Class<wxSize> wx_size(vm,"wxSize");
        wx_size.
                Ctor().
                Ctor<int,int>().
                SquirrelFunc("_tostring", &wxSize_OpTostring).
                SquirrelFunc("Cmp", &wxSize_Cmp).
                Func("GetWidth",    &wxSize::GetWidth).
                Func("GetHeight",   &wxSize::GetHeight).
                Func<WXS_SET>("Set",&wxSize::Set).
                Func<WXS_SETH>("SetHeight", &wxSize::SetHeight).
                Func<WXS_SETW>("SetWidth",  &wxSize::SetWidth);
        Sqrat::RootTable(vm).Bind("wxSize",wx_size);

    }
};
