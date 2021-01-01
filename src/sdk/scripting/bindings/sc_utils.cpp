/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <sdk_precomp.h>
#ifndef CB_PRECOMP
#endif // CB_PRECOMP

#include "sc_utils.h"
#include "squirrel.h"

namespace ScriptBindings
{

// FIXME (squirrel) Remove this?!
/*
static const char * GetStackItemTypeStr(HSQUIRRELVM vm, SQInteger idx)
{
    switch (sq_gettype(vm, idx))
    {
        case OT_NULL: return "Null";
        case OT_INTEGER: return "Integer";
        case OT_FLOAT: return "Float";
        case OT_BOOL: return "Bool";
        case OT_STRING: return "String";
        case OT_TABLE: return "Table";
        case OT_ARRAY: return "Array";
        case OT_USERDATA: return "UserData";
        case OT_CLOSURE: return "Closure";
        case OT_NATIVECLOSURE: return "NativeClosure";
        case OT_GENERATOR: return "Generator";
        case OT_USERPOINTER: return "UserPointer";
        case OT_THREAD: return "Thread";
        case OT_FUNCPROTO: return "FuncProto";
        case OT_CLASS: return "Class";
        case OT_INSTANCE: return "Instance";
        case OT_WEAKREF: return "WeakRef";
        default: return "<Unknown>";
    }
}
*/

static std::string GetItemString(HSQUIRRELVM vm, SQInteger stackIdx)
{
/*    const void *ptr;
    sq_getvoidptr(vm, stackIdx, &ptr);
    std::stringstream s;
    s << std::hex << ptr;
    return s.str();*/
    return "unknown value";
}

void PrintStack(HSQUIRRELVM vm, const char *title, SQInteger oldTop)
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
        //printf("debug: [%d]: type %s\n", ii, GetStackItemTypeStr(vm, -1-ii));
    }
    puts("");
}

} // namespace ScriptBindings
