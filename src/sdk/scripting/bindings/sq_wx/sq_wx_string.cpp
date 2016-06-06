/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <wx/wx.h>
#include <wx/string.h>
#include <limits>

#include "scripting/bindings/sq_wx/sq_wx.h"
#include "scripting/bindings/sc_binding_util.h"
#include "scripting/bindings/sc_cb_vm.h"

// We need at least version 2.8.5
#if !wxCHECK_VERSION(2, 8, 5)
#error Minimal wxWidgets version is 2.8.5
#endif

namespace ScriptBindings
{
namespace SQ_WX_binding
{

/** \defgroup sq_wxstring wxString binding to Squirrel
 *  \ingroup Squirrel
 *  \brief The binding of the wxString class to Squirrel
 *
 *  ### Attention
 *  The wxString binding is UTF8 capable, so all characters should work. But if you use unicode you can't use
 *  the single quotes ('') for constants in wxString::Replace or wxString::AfterFirst because squirrel uses
 *  internally only char for constants and so they have not enough space. You have to use double quotes ("")
 *  instead.
 */

 /**
 *  \ingroup sq_wxstring
 *  \brief Function bound to squirrel:
 *
 *  ### wxString Function bound to squirrel
 *   | Name            | parameter                     | description               | info       |
 *   | :-------------- | :---------------------------  | :------------------------ | :--------- |
 *   | wxString()      | string; wxString; SQchar; int; float |  create a wxString object |        x   |
 *   | Append          |                          x    |                        x  |        x   |
 *   | IsEmpty         |                          x    |                        x  |        x   |
 *   | Length          |                          x    |                        x  |        x   |
 *   | len             |                          x    |                        x  |        x   |
 *   | size            |                          x    |                        x  |        x   |
 *   | Lower           |                          x    |                        x  |        x   |
 *   | LowerCase       |                          x    |                        x  |        x   |
 *   | MakeLower       |                          x    |                        x  |        x   |
 *   | Upper           |                          x    |                        x  |        x   |
 *   | MakeUpper       |                          x    |                        x  |        x   |
 *   | Mid             |                          x    |                        x  |        x   |
 *   | Remove          |                          x    |                        x  |        x   |
 *   | RemoveLast      |                          x    |                        x  |        x   |
 *   | Replace         |                          x    |                        x  |        x   |
 *   | AfterFirst      |                          x    |                        x  |        x   |
 *   | BeforeFirst     |                          x    |                        x  |        x   |
 *   | BeforeLast      |                          x    |                        x  |        x   |
 *   | Right           |                          x    |                        x  |        x   |
 *   | Matches         |                          x    |                        x  |        x   |
 *   | GetChar         |                          x    |                        x  |        x   |
 *   | AddChar         |                          x    |                        x  |        x   |
 *   | Find            |                          x    |                        x  |        x   |
 *   | insert          |  size_t nPos, string          |                        x  |        x   |
 *   | replace         |  size_t nStart, size_t nLen, string   x  |             x  |        x   |
 *   | ToInt           |  int base                     | returns a table with two entries: the _value_ and a bool _success_ which is true on success    |   x   |
 *   | ToDouble        |                               | returns a table with two entries: the _value_ and a bool _success_ which is true on success    |   x   |
 *
 * ### Global function for wxSting
 *   | Name            | parameter              | description     | info       |
 *   | :-------------- | :--------------------  | :-------------- | :--------- |
 *   | _T()            | string                 | converts a normal string to a wxString object. Needed only for compatibility |   x   |
 *   | wxT()           | string                 | same as _T      |   x   |
 *   | _()             | string                 | search for the translation of the string and return a wxString object.  |   x   |
 *
 * ### Example for ToInt/ToDouble
 *
 * ~~~~~~~~~
 *  local string = wxString("42");
 *  local ret_val = string.ToInt(10);
 *  if(!ret_val.success)
 *      error("Could not convert string to int");
 *
 *  print("String to int result: " + ret_val.value +"\n" );
 * ~~~~~~~~~
 *
 */



/** \brief  Converts a wxString to a sqrat/squirrel string.
 *          Takes care of the UNICODE conversion. Scripts use internally UTF8, wxString uses the system specified Encoding
 *
 * \param input wxString Input String
 * \return Sqrat::string A squirrel string (char)
 *
 */
Sqrat::string cWxToStdString(wxString input)
{
    return Sqrat::string( input.ToUTF8());
}

/** \brief Converts a squirrel String to a wxString.
 *          Takes care of the UNICODE conversion
 *
 * \param input SQChar* Pointer to the inout string
 * \return wxString     A wxString object
 *
 */
wxString StdToWxString(SQChar* input)
{
    return wxString::FromUTF8(input);
}

/** \brief Converts a squirrel String to a translated wxString.
 *          Takes care of the UNICODE conversion
 *
 * \param input SQChar* Pointer to the inout string
 * \return wxString     A wxString object
 *
 */
wxString StdToWxStringTranslated(SQChar* input)
{
    return wxGetTranslation(wxString::FromUTF8(input));
}

/** \brief The constructor function which squirrel uses for creating a wxWidgets object from various other types
 *
 *  types supported: wxString, native string, int, float, empty
 * \param vm HSQUIRRELVM
 * \return SQInteger
 *
 */
static SQInteger wxString_constructor(HSQUIRRELVM vm)
{
    StackHandler sa(vm);
    try
    {
    if (sa.GetParamCount() == 1)      //Empty constructor....
    {
        wxString* instance = new wxString();
        Sqrat::DefaultAllocator<wxString>::SetInstance(vm,1,instance);
        //sq_setinstanceup(vm, 1, instance);
        //sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<wxString>::Delete);
        return SC_RETURN_OK;
    }
    else if (sa.GetParamCount() == 2)     // 1 Parameter
    {
        // Lets check which type it is
        //copy Constructor?
        Sqrat::Var<const wxString&> copy(vm, 2);
        if (!Sqrat::Error::Occurred(vm))
        {
            wxString* instance = new wxString(copy.value);
            Sqrat::DefaultAllocator<wxString>::SetInstance(vm,1,instance);
            //sq_setinstanceup(vm, 1, instance);
            //sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<wxString>::Delete);
            return SC_RETURN_OK;
        }
        // it was not the copy ctr
        // lets test if it is a squirrel string
        Sqrat::Error::Clear(vm);
        Sqrat::Var<SQChar*> char_arr(vm, 2);
        if (!Sqrat::Error::Occurred(vm))
        {
            wxString* instance = new wxString(wxString::FromUTF8(char_arr.value));
            Sqrat::DefaultAllocator<wxString>::SetInstance(vm,1,instance);
            //sq_setinstanceup(vm, 1, instance);
            //sq_setreleasehook(vm, 1, &Sqrat::DefaultAllocator<wxString>::Delete);
            return SC_RETURN_OK;
        }
        // Wrong ctr parameter
        Sqrat::Error::Clear(vm);
        return sa.ThrowError(Sqrat::FormatTypeError(vm, 2, Sqrat::ClassType<wxString>::ClassName() + _SC("|SQChar*")).c_str());
    }
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::constructor:") + e.Message());
    }

    return sa.ThrowError(_T("wxString::constructor: wrong number of parameters"));
}

/** \brief The operator+ for wxString in squirrel
 *
 * \param vm HSQUIRRELVM
 * \return SQInteger
 *
 */
static SQInteger wxString_add(HSQUIRRELVM vm)
{
    StackHandler sa(vm);
    try
    {
    if (sa.GetParamCount() == 1)      //no parameter to add
    {
        return sa.ThrowError(_T("wxString operator+: wrong number of parameters"));
    }
    else if (sa.GetParamCount() == 2)     // 1 Parameter
    {
        // Lets check which type it is

        //First get the "this"
        wxString* instance = Sqrat::ClassType<wxString>::GetInstance(vm, 1);
        if(instance == NULL)
        {
            return sa.ThrowError(_T("wxString operator+: no base"));
        }

        wxString ret(*instance); // create the return value

        // Test if we add wxString
        Sqrat::Var<wxString> copy(vm, 2);
        if (!Sqrat::Error::Occurred(vm))
        {
            ret.Append(copy.value);
            Sqrat::Var<wxString>::push(vm, ret);
            return SC_RETURN_VALUE;
        }
        // Test if we add squirrel string
        Sqrat::Error::Clear(vm);
        Sqrat::Var<SQChar*> char_arr(vm, 2);
        if (!Sqrat::Error::Occurred(vm))
        {
            ret.Append(wxString::FromUTF8(char_arr.value));
            Sqrat::Var<wxString>::push(vm, ret);
            return SC_RETURN_VALUE;
        }
        // Test if we add a int (this probably will never happen, because squirrel transforms it to a string?)
        Sqrat::Error::Clear(vm);
        Sqrat::Var<SQInteger> int_val(vm, 2);
        if (!Sqrat::Error::Occurred(vm))
        {
            ret.Append(wxString::Format(wxT("%d"),int_val.value));
            Sqrat::Var<wxString>::push(vm, ret);
            return SC_RETURN_VALUE;
        }
        // Test if we add a floating point number (this probably will never happen, because squirrel transforms it to a string?)
        Sqrat::Error::Clear(vm);
        Sqrat::Var<SQFloat> float_val(vm, 2);
        if (!Sqrat::Error::Occurred(vm))
        {
            ret.Append(wxString::Format(wxT("%f"),int_val.value));
            Sqrat::Var<wxString>::push(vm, ret);
            return SC_RETURN_VALUE;
        }
        // Wrong type of parameter
        wxString error_msg;
        return sa.ThrowError(Sqrat::FormatTypeError(vm, 2, Sqrat::ClassType<wxString>::ClassName() + _SC("|SQChar*|int|float")).c_str());
    }
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString operator+:") + e.Message());
    }
    return sa.ThrowError(_T("wxString operator+: wrong number of parameters"));
}

/** \brief Compare two strings. This function is only used for > and <. For == you have to use a function like wxString::Compare()
 *
 * \param vm HSQUIRRELVM
 * \return SQInteger
 *
 */
static SQInteger wxString_OpCmp(HSQUIRRELVM vm)
{
    StackHandler sa(vm);
    try
    {
    int ret = 0;
    if (sa.GetParamCount() < 2)
    {
        sa.ThrowError(_T("wxString operator cmp (<,>): wrong number of parameters"));
    }
    //First get the "this"
    wxString* lhs = sa.GetInstance<wxString>(1);
    wxString rhs;
    if(lhs == NULL)
    {
        sa.ThrowError(_T("wxString cmp (<,>): have no base"));
    }
    Sqrat::Var<const wxString&> str_val(vm, 2);
    if (!Sqrat::Error::Occurred(vm))
    {
        ret = lhs->CompareTo(str_val.value);
        Sqrat::Var<int>::push(vm, ret);
        return SC_RETURN_VALUE;
    }
    Sqrat::Error::Clear(vm);
    Sqrat::Var<SQChar*> char_arr(vm, 2);
    if (!Sqrat::Error::Occurred(vm))
    {
        ret = lhs->CompareTo(wxString::Format(wxT("%s"),char_arr.value));
        Sqrat::Var<int>::push(vm, ret);
        return SC_RETURN_VALUE;
    }
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString operator cmp (<,>): ") + e.Message());
    }
    return sa.ThrowError(_T("wxString operator cmp (<,>): wrong number of parameters"));
}

static SQInteger wxString_Replace(HSQUIRRELVM vm)
{
    StackHandler sa(vm);
    try
    {
    if (sa.GetParamCount() < 3)
    {
        sa.ThrowError(_T("wxString::Replace: wrong number of parameters"));
    }
    //First get the "this"
    wxString* self = Sqrat::ClassType<wxString>::GetInstance(vm, 1);
    if(self == NULL)
    {
        sa.ThrowError(_T("wxString::Replace: have no base"));
    }

    bool all = true;
    Sqrat::Var<wxString> old_str(vm,2);
    Sqrat::Var<wxString> new_str(vm,3);
    if(Sqrat::Error::Occurred(vm))
    {
        return sa.ThrowError(Sqrat::FormatTypeError(vm, 2, Sqrat::ClassType<wxString>::ClassName()).c_str());
    }

    if(sa.GetParamCount() == 4)
    {
        Sqrat::Var<bool> all_val(vm,4);
        if(Sqrat::Error::Occurred(vm))
        {
            return sa.ThrowError(Sqrat::FormatTypeError(vm, 2, _SC("bool")).c_str());
        }

        all = all_val.value;
    }
    Sqrat::Var<int>::push(vm, self->Replace(old_str.value, new_str.value, all));
    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::Replace: ") + e.Message());
    }
}

SQInteger wxString_AfterFirst(HSQUIRRELVM vm)
{
    StackHandler sa(vm);
    if (sa.GetParamCount() < 2)
    {
        return sa.ThrowError(_T("wxString::AfterFirst: wrong number of parameters"));
    }
    try
    {
    //First get the "this"
    wxString* self = Sqrat::ClassType<wxString>::GetInstance(vm, 1);
    if(self == NULL)
    {
        return sa.ThrowError(_T("wxString::AfterFirst: have no base"));
    }

    wxString search_char= sa.GetValue<wxString>(2);

    sa.PushValue<wxString>(self->AfterFirst(search_char[0]));
    //Sqrat::Var<wxString>::push(vm, self->AfterFirst(search_char[0]));
    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::AfterFirst: ") + e.Message());
    }
}

SQInteger wxString_AfterLast(HSQUIRRELVM vm)
{
    StackHandler sa(vm);
    if (sa.GetParamCount() < 2)
    {
        return sa.ThrowError(_T("wxString::AfterLast: wrong number of parameters"));
    }
    try
    {
    //First get the "this"
    wxString* self = sa.GetInstance<wxString>(1); //Sqrat::ClassType<wxString>::GetInstance(vm, 1);
    if(self == NULL)
    {
        return sa.ThrowError(_T("wxString::AfterLast: have no base"));
    }

    wxString search_char = sa.GetValue<wxString>(2);


    sa.PushValue<wxString>(self->AfterLast(search_char[0]));
    //Sqrat::Var<wxString>::push(vm, self->AfterLast(search_char[0]));
    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::AfterLast: ") + e.Message());
    }
}

SQInteger wxString_BeforeFirst(HSQUIRRELVM vm)
{
    StackHandler sa(vm);
    if (sa.GetParamCount() < 2)
    {
        return sa.ThrowError(_T("wxString::BeforeFirst: wrong number of parameters"));
    }
    try
    {
    //First get the "this"
    wxString* self = Sqrat::ClassType<wxString>::GetInstance(vm, 1);
    if(self == NULL)
    {
        return sa.ThrowError(_T("wxString::BeforeFirst: have no base"));
    }
    wxString search_char = sa.GetValue<wxString>(2);


    sa.PushValue<wxString>(self->BeforeFirst(search_char[0]));

    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::BeforeFirst:") + e.Message());
    }
}

SQInteger wxString_BeforeLast(HSQUIRRELVM vm)
{
    StackHandler sa(vm);

    if (sa.GetParamCount() < 2)
    {
        return sa.ThrowError(_T("wxString::BeforeLast: wrong number of parameters"));
    }
    try
    {
    //First get the "this"
    wxString* self = sa.GetInstance<wxString>(1);
    if(self == NULL)
    {
        return sa.ThrowError(_T("wxString::BeforeLast: have no base"));
    }

    wxString search_char = sa.GetValue<wxString>(2);

    sa.PushValue<wxString>(self->BeforeLast(search_char[0]));

    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::BeforeLast:") + e.Message());
    }
}


SQInteger wxString_Matches(HSQUIRRELVM v)
{
    StackHandler sa(v);
    try
    {

    wxString& self = *sa.GetInstance<wxString>(1);
    wxString other = sa.GetValue<wxString>(2);
    sa.PushValue<SQInteger>(self.Matches(other));

    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::Matches: ") + e.Message());
    }
}

SQInteger wxString_GetChar(HSQUIRRELVM v)
{
    StackHandler sa(v);
    try
    {

    wxString& self = *sa.GetInstance<wxString>(1);
    unsigned int pos = sa.GetValue<unsigned int>(2);
    if(pos >= self.length())
        return SC_RETURN_FAILED;

    SQChar tmp = self.To8BitData()[pos];
    sa.PushValue<SQInteger>(static_cast<SQInteger>(tmp));
    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::GetChar: ") + e.Message());
    }
}

SQInteger wxString_AddChar(HSQUIRRELVM v)
{
    StackHandler sa(v);
    try
    {
    wxString& self = *sa.GetInstance<wxString>(1);
    wxString character = sa.GetValue<wxString>(2);
    sa.PushValue<wxString&>(self.Append(character));
    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::AddChar: ") + e.Message());
    }
}

SQInteger wxString_Find(HSQUIRRELVM v)
{
    StackHandler sa(v);
    try
    {
    wxString& self = *sa.GetInstance<wxString>(1);
    wxString other = sa.GetValue<wxString>(2);
    sa.PushValue<int>(self.Find(other));
    return SC_RETURN_VALUE;
    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::Find: ") + e.Message());
    }
}


SQInteger wxString_ToInt(HSQUIRRELVM v)
{
    StackHandler sa(v);
    try
    {
    wxString& self = *sa.GetInstance<wxString>(1);
    int base = 10;
    if(sa.GetParamCount() == 2)
        base = sa.GetValue<int>(2);

    long tmp = 0;
    bool ret = self.ToLong(&tmp,base);

    Sqrat::Table ret_table(v);

    // TODO (bluehazzard#1#): Inform squirrel user about overflow, underflow?
    if(ret && tmp <= std::numeric_limits<int>::max() && tmp >= std::numeric_limits<int>::min())
    {
        ret_table.SetValue<bool>("success",true);
        ret_table.SetValue<int>("value",tmp);
    }
    else
    {
        ret_table.SetValue<bool>("success", false);
        ret_table.SetValue<int>("value", 0);
    }
    sa.PushValue(ret_table);

    return SC_RETURN_VALUE;

    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::ToInt: ") + e.Message());
    }
}

SQInteger wxString_ToDouble(HSQUIRRELVM v)
{
    StackHandler sa(v);
    try
    {
    wxString& self = *sa.GetInstance<wxString>(1);


    double tmp = 0;
    bool ret = self.ToDouble(&tmp);

    Sqrat::Table ret_table(v);
    ret_table.SetValue<bool>("success",ret);
    if(ret)
    {
        if(tmp <= std::numeric_limits<float>::max() && tmp >= std::numeric_limits<float>::min()) // don't make an overflow
            ret_table.SetValue<float>("value",(float) tmp);
        else
            ret_table.SetValue<float>("value",(float) std::numeric_limits<float>::infinity());
    }
    else
        ret_table.SetValue<float>("value", std::numeric_limits<float>::quiet_NaN());

    sa.PushValue(ret_table);
    return SC_RETURN_VALUE;

    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::ToDouble: ") + e.Message());
    }
}

SQInteger wxString_ToLong(HSQUIRRELVM v)
{
    StackHandler sa(v);
    try
    {
    wxString& param = *sa.GetInstance<wxString>(2);


    long tmp = 0;
    bool ret = param.ToLong(&tmp);

    Sqrat::Table ret_table(v);
    // TODO (bluehazzard#1#): Inform squirrel user about overflow, underflow?
    if(ret && tmp <= std::numeric_limits<long>::max() && tmp >= std::numeric_limits<long>::min())
    {
        ret_table.SetValue<bool>("success",true);
        ret_table.SetValue<long>("value",tmp);
    }
    else
    {
        ret_table.SetValue<bool>("success", false);
        ret_table.SetValue<long>("value", 0);
    }
    sa.PushValue(ret_table);
    return SC_RETURN_VALUE;

    }
    catch(CBScriptException &e)
    {
        return sa.ThrowError(_("wxString::ToLong: ") + e.Message());
    }
}

/** \brief Bind wxString to the Squirrel vm
 *
 * \param vm HSQUIRRELVM vm to which wxString should be bound
 * \return void
 *
 */
void bind_wxString(HSQUIRRELVM vm)
{
    using namespace Sqrat;

    Class<wxString> bwxString(vm,"wxString");
    bwxString.SquirrelFunc("constructor",&wxString_constructor)
    .SquirrelFunc(_SC("_cmp"),&wxString_OpCmp)
    .SquirrelFunc("_add", &wxString_add)
    .GlobalFunc(_SC("_tostring"),&cWxToStdString)

    .Func<wxString& (wxString::*)(const wxString&)>("Append",&wxString::Append)
    .Func("IsEmpty",&wxString::IsEmpty)
    .Func("Length", &wxString::Len)
    .Func("length", &wxString::Len)
    .Func("len",    &wxString::Len)
    .Func("size",   &wxString::Len)
    .Func("Lower",  &wxString::Lower)
    .Func("LowerCase",  &wxString::LowerCase)
    .Func("MakeLower",  &wxString::MakeLower)
    .Func("Upper",      &wxString::Upper)
    .Func("UpperCase",  &wxString::UpperCase)
    .Func("MakeUpper",  &wxString::MakeUpper)
    .Func("Mid",    &wxString::Mid)
    .Func<wxString& (wxString::*) (size_t pos, size_t len)>("Remove",    &wxString::Remove)
    .Func("RemoveLast",         &wxString::RemoveLast)
    .Func<wxString& (wxString::*) (size_t , const wxString&)>("insert",&wxString::insert)
    .Func<int (wxString::*) (const wxString&) const>("Cmp",&wxString::Cmp)
    .Func<int (wxString::*) (const wxString&) const>("CmpNoCase",&wxString::CmpNoCase)
    .Overload<wxString& (wxString::*) (size_t, size_t, const wxString&)>("replace",&wxString::replace)
    .Overload<wxString& (wxString::*) (size_t,size_t,const wxString&,size_t,size_t)>("replace",&wxString::replace)
    .SquirrelFunc("Replace",    &wxString_Replace)
    .SquirrelFunc("AfterFirst", &wxString_AfterFirst)
    .SquirrelFunc("AfterLast",  &wxString_AfterLast)
    .SquirrelFunc("BeforeFirst",&wxString_BeforeFirst)
    .SquirrelFunc("BeforeLast", &wxString_BeforeLast)
    .Func("Right",   &wxString::Right)
    // TODO (bluehazzard#1#): In wx2.9 this is wxString not wxChar
    .SquirrelFunc("Matches",&wxString_Matches)
    .SquirrelFunc("GetChar",&wxString_GetChar)
    .SquirrelFunc("AddChar",&wxString_AddChar)
    .SquirrelFunc("Find",   &wxString_Find)
    .SquirrelFunc("ToInt",  &wxString_ToInt)
    .SquirrelFunc("ToDouble",&wxString_ToDouble)
    ;
    RootTable(vm).Bind(_SC("wxString"),bwxString);


    RootTable(vm).Func(_SC("_T"),&StdToWxString);
    RootTable(vm).Func<wxString (*) (SQChar *)>(_SC("wxT"),&StdToWxString);
    RootTable(vm).Func<wxString (*) (SQChar *)>(_SC("_"),&StdToWxStringTranslated);

     Sqrat::RootTable(vm).SquirrelFunc("wxString_ToLong",&wxString_ToLong);
}


} // namespace SQ_WX_binding

} // namespace ScriptBinding
