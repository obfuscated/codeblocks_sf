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
        line+="]: type=";

        const SQInteger stackIdx=-1-ii;

        switch (sq_gettype(vm, stackIdx))
        {
            case OT_NULL:
                line+="Null";
                break;

            case OT_INTEGER:
            {
                line+="Integer=";
                SQInteger value;
                sq_getinteger(vm, stackIdx, &value);
                line+=std::to_string(value);
                break;
            }
            case OT_FLOAT:
            {
                line+="Float ";
                SQFloat value;
                sq_getfloat(vm, stackIdx, &value);
                line+=std::to_string(value);
                break;
            }
            case OT_BOOL:
            {
                line+="Bool ";
                SQBool value;
                sq_getbool(vm, stackIdx, &value);
                line+=(value ? "True" : "False");
                break;
            }
            case OT_STRING:
            {
                line+="String '";
                const SQChar *value;
                sq_getstring(vm, stackIdx, &value);
                if (value != nullptr)
                {
                    line+=value;
                    line+="'";
                }
                else
                    line+="<n/a>'";
                break;
            }
            case OT_TABLE:
            {
                line+="Table ";
                const void *ptr;
                //sq_getvoidptr(vm, stackIdx, &ptr);
                ptr = (void*)0x01020304;
                line+=GetItemString(vm, stackIdx);
                break;
            }
            case OT_ARRAY:
            {
                line+="Array ";
                const void *ptr;
                //sq_getvoidptr(vm, stackIdx, &ptr);
                ptr = (void*)0x01020304;
                line+=GetItemString(vm, stackIdx);
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
                line+="Instance ";
                line+=GetItemString(vm, stackIdx);
                break;
            }
            case OT_WEAKREF:
                line+="WeakRef";
                break;
            default: line+="<Unknown>";
        }

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

} // namespace ScriptBindings
