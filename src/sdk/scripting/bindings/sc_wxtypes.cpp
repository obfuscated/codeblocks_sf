#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include <globals.h>
#endif

#include "sc_base_types.h"

namespace ScriptBindings
{
    // the _T() function for scripts
    wxString static_T(const SQChar* str)
    {
        return cbC2U(str);
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

    void Register_wxTypes()
    {
        SqPlus::RegisterGlobal(&static_T, "_T");

        typedef int(wxString::*WXSTR_FIRST_STR)(const wxString&)const;
        typedef wxString&(wxString::*WXSTR_REMOVE_2)(size_t pos, size_t len);

        SqPlus::SQClassDef<wxString>("wxString").
                emptyCtor().
                staticFuncVarArgs(&wxString_OpAdd, "_add", "*").
                staticFuncVarArgs(&wxString_OpCmp, "_cmp", "*").
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

        SqPlus::SQClassDef<wxArrayString>("wxArrayString").
                emptyCtor().
                func(&wxArrayString::Add, "Add").
                func(&wxArrayString::Clear, "Clear").
//                func(&wxArrayString::Index, "Index").
                func(&wxArrayString::GetCount, "GetCount").
                func(&wxArrayString::Item, "Item");
    }
};
