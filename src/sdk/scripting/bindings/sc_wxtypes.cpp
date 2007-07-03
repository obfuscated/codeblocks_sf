#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include <globals.h>
#endif
#include <wx/filename.h>
#include <wx/colour.h>

#include "sc_base_types.h"

namespace ScriptBindings
{
    // the _T() function for scripts
    wxString static_T(const SQChar* str)
    {
        return cbC2U(str);
    }

    // the _() function for scripts
    wxString static_(const SQChar* str)
    {
        return wxGetTranslation(cbC2U(str));
    }

    // wxString operator+
    SQInteger wxString_OpAdd(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString result;
        wxString& str1 = *SqPlus::GetInstance<wxString>(v, 1);
        if (sa.GetType(2) == OT_INTEGER)
            result.Printf(_T("%s%d"), str1.c_str(), sa.GetInt(2));
        else if (sa.GetType(2) == OT_FLOAT)
            result.Printf(_T("%s%f"), str1.c_str(), sa.GetFloat(2));
        else if (sa.GetType(2) == OT_USERPOINTER)
            result.Printf(_T("%s%p"), str1.c_str(), sa.GetUserPointer(2));
        else if (sa.GetType(2) == OT_STRING)
            result.Printf(_T("%s%s"), str1.c_str(), cbC2U(sa.GetString(2)).c_str());
        else
            result = str1 + *SqPlus::GetInstance<wxString>(v, 2);
        return SqPlus::ReturnCopy(v, result);
    }

    SQInteger wxString_OpCmp(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& str1 = *SqPlus::GetInstance<wxString>(v, 1);
        if (sa.GetType(2) == OT_STRING)
            return sa.Return((SQInteger)str1.Cmp(cbC2U(sa.GetString(2))));
        return sa.Return((SQInteger)str1.Cmp(*SqPlus::GetInstance<wxString>(v, 2)));
    }

    SQInteger wxString_OpToString(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        return sa.Return((const SQChar*)self.mb_str(wxConvUTF8));
    }

    SQInteger wxString_AddChar(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        int idx = sa.GetInt(2);
        char tmp[8] = {};
        sprintf(tmp, "%c", idx);
        self += cbC2U(tmp);
        return sa.Return();
    }
    SQInteger wxString_GetChar(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        int idx = sa.GetInt(2);
        return sa.Return((SQInteger)(((const char*)cbU2C(self))[idx]));
    }
    SQInteger wxString_Matches(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        wxString& other = *SqPlus::GetInstance<wxString>(v, 2);
        return sa.Return(self.Matches(other));
    }
    SQInteger wxString_AfterFirst(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        return SqPlus::ReturnCopy(v, self.AfterFirst((wxChar)sa.GetInt(2)));
    }
    SQInteger wxString_AfterLast(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        return SqPlus::ReturnCopy(v, self.AfterLast((wxChar)sa.GetInt(2)));
    }
    SQInteger wxString_BeforeFirst(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        return SqPlus::ReturnCopy(v, self.BeforeFirst((wxChar)sa.GetInt(2)));
    }
    SQInteger wxString_BeforeLast(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        return SqPlus::ReturnCopy(v, self.BeforeLast((wxChar)sa.GetInt(2)));
    }
    SQInteger wxString_Replace(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString& self = *SqPlus::GetInstance<wxString>(v, 1);
        wxString from = *SqPlus::GetInstance<wxString>(v, 2);
        wxString to = *SqPlus::GetInstance<wxString>(v, 3);
        bool all = true;
        if (sa.GetParamCount() == 4)
            all = sa.GetBool(4);
        return sa.Return((SQInteger)self.Replace(from, to, all));
    }

////////////////
// wxFileName //
////////////////

    SQInteger wxFileName_OpToString(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxFileName& self = *SqPlus::GetInstance<wxFileName>(v, 1);
        return sa.Return((const SQChar*)self.GetFullPath().mb_str(wxConvUTF8));
    }


///////////////////
// wxArrayString //
///////////////////

    SQInteger wxArrayString_Index(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxArrayString& self = *SqPlus::GetInstance<wxArrayString>(v, 1);
        wxString inpstr = *SqPlus::GetInstance<wxString>(v, 2);
        bool chkCase = true;
        bool frmEnd = false;
        if (sa.GetParamCount() >= 3)
            chkCase = sa.GetBool(3);
        if (sa.GetParamCount() == 4)
            frmEnd = sa.GetBool(4);
        return sa.Return((SQInteger)self.Index(inpstr.c_str(), chkCase, frmEnd));
    }


////////////////
// wxColour //
////////////////

    SQInteger wxColour_OpToString(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxColour& self = *SqPlus::GetInstance<wxColour>(v, 1);
        wxString str = wxString::Format(_T("[r=%d, g=%d, b=%d]"), self.Red(), self.Green(), self.Blue());
        return sa.Return((const SQChar*)str.mb_str(wxConvUTF8));
    }


////////////////////////////////////////////////////////////////////////////////

    void Register_wxTypes()
    {
        SqPlus::RegisterGlobal(&static_T, "_T");
        SqPlus::RegisterGlobal(&static_, "_");

        typedef int(wxString::*WXSTR_FIRST_STR)(const wxString&)const;
        typedef wxString&(wxString::*WXSTR_REMOVE_2)(size_t pos, size_t len);

        SqPlus::SQClassDef<wxString>("wxString").
                emptyCtor().
                staticFuncVarArgs(&wxString_OpAdd, "_add", "*").
                staticFuncVarArgs(&wxString_OpCmp, "_cmp", "*").
                staticFuncVarArgs(&wxString_OpToString, "_tostring", "").
                func<WXSTR_FIRST_STR>(&wxString::First, "Find").
                staticFuncVarArgs(&wxString_Matches, "Matches", "*").
                staticFuncVarArgs(&wxString_AddChar, "AddChar", "n").
                staticFuncVarArgs(&wxString_GetChar, "GetChar", "n").
                func(&wxString::IsEmpty, "IsEmpty").
                func(&wxString::Length, "Length").
                func(&wxString::Length, "length").
                func(&wxString::Length, "len").
                func(&wxString::Length, "size").
                func(&wxString::Lower, "Lower").
                func(&wxString::LowerCase, "LowerCase").
                func(&wxString::MakeLower, "MakeLower").
                func(&wxString::Upper, "Upper").
                func(&wxString::UpperCase, "UpperCase").
                func(&wxString::MakeUpper, "MakeUpper").
                func(&wxString::Mid, "Mid").
                func<WXSTR_REMOVE_2>(&wxString::Remove, "Remove").
                func(&wxString::RemoveLast, "RemoveLast").
                staticFuncVarArgs(&wxString_Replace, "Replace", "*").
                func(&wxString::Right, "Right").
                staticFuncVarArgs(&wxString_AfterFirst, "AfterFirst", "*").
                staticFuncVarArgs(&wxString_AfterLast, "AfterLast", "*").
                staticFuncVarArgs(&wxString_BeforeFirst, "BeforeFirst", "*").
                staticFuncVarArgs(&wxString_BeforeLast, "BeforeLast", "*");

        typedef void(wxFileName::*WXFN_ASSIGN_FN)(const wxFileName&);
        typedef void(wxFileName::*WXFN_ASSIGN_STR)(const wxString&, wxPathFormat);
        typedef wxString(wxFileName::*WXFN_GETPATH)(int, wxPathFormat)const;
        typedef bool(wxFileName::*WXFN_SETCWD)();

        SqPlus::SQClassDef<wxFileName>("wxFileName").
                emptyCtor().
                staticFuncVarArgs(&wxFileName_OpToString, "_tostring", "").
                func<WXFN_ASSIGN_FN>(&wxFileName::Assign, "Assign").
                func<WXFN_ASSIGN_STR>(&wxFileName::Assign, "Assign").
                func(&wxFileName::AssignCwd, "AssignCwd").
                func(&wxFileName::AssignDir, "AssignDir").
                func(&wxFileName::AssignHomeDir, "AssignHomeDir").
                func(&wxFileName::Clear, "Clear").
                func(&wxFileName::ClearExt, "ClearExt").
//                func(&wxFileName::GetCwd, "GetCwd").
                func(&wxFileName::GetDirCount, "GetDirCount").
                func(&wxFileName::GetDirs, "GetDirs").
                func(&wxFileName::GetExt, "GetExt").
                func(&wxFileName::GetFullName, "GetFullName").
                func(&wxFileName::GetFullPath, "GetFullPath").
                func(&wxFileName::GetLongPath, "GetLongPath").
                func(&wxFileName::GetName, "GetName").
                func<WXFN_GETPATH>(&wxFileName::GetPath, "GetPath").
                func(&wxFileName::GetShortPath, "GetShortPath").
                func(&wxFileName::GetVolume, "GetVolume").
                func(&wxFileName::HasExt, "HasExt").
                func(&wxFileName::HasName, "HasName").
                func(&wxFileName::HasVolume, "HasVolume").
                func(&wxFileName::InsertDir, "InsertDir").
                func(&wxFileName::IsAbsolute, "IsAbsolute").
                func(&wxFileName::IsOk, "IsOk").
                func(&wxFileName::IsRelative, "IsRelative").
                func(&wxFileName::IsDir, "IsDir").
                func(&wxFileName::MakeAbsolute, "MakeAbsolute").
                func(&wxFileName::MakeRelativeTo, "MakeRelativeTo").
                func(&wxFileName::Normalize, "Normalize").
                func(&wxFileName::PrependDir, "PrependDir").
                func(&wxFileName::RemoveDir, "RemoveDir").
                func(&wxFileName::RemoveLastDir, "RemoveLastDir").
                func(&wxFileName::SameAs, "SameAs").
                func<WXFN_SETCWD>(&wxFileName::SetCwd, "SetCwd").
                func(&wxFileName::SetExt, "SetExt").
                func(&wxFileName::SetEmptyExt, "SetEmptyExt").
                func(&wxFileName::SetFullName, "SetFullName").
                func(&wxFileName::SetName, "SetName").
                func(&wxFileName::SetVolume, "SetVolume");

        SqPlus::SQClassDef<wxArrayString>("wxArrayString").
                emptyCtor().
                func(&wxArrayString::Add, "Add").
                func(&wxArrayString::Clear, "Clear").
//                func(&wxArrayString::Index, "Index").
                staticFuncVarArgs(&wxArrayString_Index, "Index", "*").
                func(&wxArrayString::GetCount, "GetCount").
                func(&wxArrayString::Item, "Item");

#if wxCHECK_VERSION(2, 8, 0)
        typedef void(wxColour::*WXC_SET)(const unsigned char, const unsigned char, const unsigned char, const unsigned char);
#else
        typedef void(wxColour::*WXC_SET)(const unsigned char, const unsigned char, const unsigned char);
#endif
        SqPlus::SQClassDef<wxColour>("wxColour").
                emptyCtor().
                staticFuncVarArgs(&wxColour_OpToString, "_tostring", "").
                func(&wxColour::Blue, "Blue").
                func(&wxColour::Green, "Green").
                func(&wxColour::Red, "Red").
                func(&wxColour::Ok, "Ok").
                func<WXC_SET>(&wxColour::Set, "Set");
    }
};
