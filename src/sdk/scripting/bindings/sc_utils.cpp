/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <sdk_precomp.h>
#ifndef CB_PRECOMP
#endif // CB_PRECOMP

#include "scripting/bindings/sc_utils.h"
#include "squirrel.h"

namespace ScriptBindings
{

static std::string GetItemString(HSQUIRRELVM vm, SQInteger stackIdx)
{
/*    const void *ptr;
    sq_getvoidptr(vm, stackIdx, &ptr);
    std::stringstream s;
    s << std::hex << ptr;
    return s.str();*/
    return "unknown value";
}

std::string MakeStringFromSquirrelValue(HSQUIRRELVM vm, const int stackIdx)
{
    std::string line;
    switch (sq_gettype(vm, stackIdx))
    {
        case OT_NULL:
            line+="Null";
            break;

        case OT_INTEGER:
        {
            SQInteger value;
            sq_getinteger(vm, stackIdx, &value);
            line+=std::to_string(value);
            line+="; Integer";
            break;
        }
        case OT_FLOAT:
        {
            SQFloat value;
            sq_getfloat(vm, stackIdx, &value);
            line+=std::to_string(value);
            line+="; Float";
            break;
        }
        case OT_BOOL:
        {
            SQBool value;
            sq_getbool(vm, stackIdx, &value);
            line+=(value ? "True" : "False");
            line+="; Bool";
            break;
        }
        case OT_STRING:
        {
            const SQChar *value;
            sq_getstring(vm, stackIdx, &value);
            if (value != nullptr)
            {
                line+="'";
                line+=value;
            }
            else
                line+="'<n/a>";
            line+="'; String";
            break;
        }
        case OT_TABLE:
        {
            const void *ptr;
            //sq_getvoidptr(vm, stackIdx, &ptr);
            ptr = (void*)0x01020304;
            line+=GetItemString(vm, stackIdx);
            line+="; Table";
            break;
        }
        case OT_ARRAY:
        {
            const void *ptr;
            //sq_getvoidptr(vm, stackIdx, &ptr);
            ptr = (void*)0x01020304;
            line+=GetItemString(vm, stackIdx);
            line+="; Array";
            break;
        }
        case OT_USERDATA:
            line+="UserData";
            break;
        case OT_CLOSURE:
            line+="Closure";
            break;
        case OT_NATIVECLOSURE:
            line+="NativeClosure";
            break;
        case OT_GENERATOR:
            line+="Generator";
            break;
        case OT_USERPOINTER:
            line+="UserPointer";
            break;
        case OT_THREAD:
            line+="Thread";
            break;
        case OT_FUNCPROTO:
            line+="FuncProto";
            break;
        case OT_CLASS:
            line+="Class";
            break;
        case OT_INSTANCE:
        {
            line+=GetItemString(vm, stackIdx);
            line+="; Instance";
            break;
        }
        case OT_WEAKREF:
            line+="WeakRef";
            break;
        default:
            line+="<Unknown>";
    }
    return line;
}

DLLIMPORT void PrintStack(HSQUIRRELVM vm, const char *title, SQInteger oldTop)
{
    SQInteger top = sq_gettop(vm);
    printf("debug: stack [%s] top=%lld oldTop=%lld\n", title, (long long)top, (long long)oldTop);

    std::string line;
    for (int ii = 0; ii < top; ++ii)
    {
        line="debug: [";
        line+=std::to_string(top-ii);
        line+=":";
        line+=std::to_string(-1-ii);
        line+="]: ";

        const SQInteger stackIdx=-1-ii;
        line+=MakeStringFromSquirrelValue(vm, stackIdx);
        puts(line.c_str());
    }
    puts("");
}

DLLIMPORT wxString ExtractLastSquirrelError(HSQUIRRELVM vm, bool canBeEmpty)
{
    const SQChar *s;
    sq_getlasterror(vm);
    sq_getstring(vm, -1, &s);
    wxString errorMsg;
    if (s)
        errorMsg = wxString(s);
    else if (!canBeEmpty)
        errorMsg = "Unknown error!";
    sq_pop(vm, 1);
    return errorMsg;
}

DLLIMPORT void PrintSquirrelToWxString(wxString& msg, const SQChar* s, va_list& vl)
{
    SQChar localBuffer[2048];

    va_list tmpVL;
    va_copy(tmpVL, vl);
    const int retvalue = scvsprintf(localBuffer, cbCountOf(localBuffer), s, tmpVL);
    va_end(tmpVL);
    cbAssert(retvalue != -1);

    if (retvalue < cbCountOf(localBuffer))
        msg = cbC2U(localBuffer);
    else
    {
        // The buffer is not large enough, so we need to allocate new one on the heap.
        const int bufferSize = retvalue + 20;
        std::unique_ptr<SQChar[]> tmpBuffer(new SQChar[bufferSize]);

        va_list tmpVL;
        va_copy(tmpVL, vl);
        const int retvalue2 = scvsprintf(tmpBuffer.get(), bufferSize, s, tmpVL);
        va_end(tmpVL);
        cbAssert(retvalue2 != -1);
        if (retvalue2 >= bufferSize)
        {
            cbAssert(false);
            return;
        }

        msg = cbC2U(tmpBuffer.get());
    }
}

DLLIMPORT int ExtractParamsBase::ErrorMessage()
{
    wxString fullMessage(m_errorMessage);
    fullMessage += "\n\nCALLSTACK:\n";

    SQInteger level = 0;
    SQStackInfos si;
    while (SQ_SUCCEEDED(sq_stackinfos(m_vm, level, &si)))
    {
        const wxString fn(si.funcname ? si.funcname : _SC("unknown"));
        const wxString src(si.source ? si.source : _SC("unknown"));

        fullMessage += wxString::Format("%d: Function '%s' at %s:%d\n", int(level), fn, src, int(si.line));

        {
            // Extract the arguments for the function. Arguments for native closures/functions
            // cannot be extracted using this API.
            wxString args;
            const SQChar *name = nullptr;
            SQInteger seq = 0;
            while ((name = sq_getlocal(m_vm, level, seq)) != nullptr)
            {
                seq++;
                args += "      ";
                args += name;
                args += ": ";
                args += MakeStringFromSquirrelValue(m_vm, -1);
                args += "\n";
                sq_pop(m_vm, 1);
            }
            fullMessage += args;
        }
        level++;
    }

    const wxScopedCharBuffer &utf8Buffer = fullMessage.utf8_str();
    return sq_throwerror(m_vm, utf8Buffer.data());
}

} // namespace ScriptBindings
