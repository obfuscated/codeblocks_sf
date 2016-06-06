#ifndef SC_BINDING_UTIL_H
#define SC_BINDING_UTIL_H


// helper macros to bind constants
#define BIND_INT_CONSTANT(a) Sqrat::ConstTable(vm).Const(_SC(#a), a)
#define BIND_INT_CONSTANT_NAMED(a,n) Sqrat::ConstTable(vm).Const(_SC(n), a)
// NOTE (bluehazzard#1#): This can break the API, but the old API was wrong, because constants should be constants and don't get modified...
#define BIND_WXSTR_CONSTANT_NAMED(a,n) Sqrat::ConstTable(vm).Const(_SC(n),a.ToUTF8())

#define SC_RETURN_FAILED    -1
#define SC_RETURN_OK        -0
#define SC_RETURN_VALUE      1
#include <scripting/bindings/sc_cb_vm.h>

namespace ScriptBindings
{


template <typename U> int GetValueFromTable(Sqrat::Table table,const SQChar* name,U& output,bool exc = false)
{
    if(!table.HasKey(name))
        return -1;

    Sqrat::SharedPtr<U> ptr = table.GetValue<U>(name);
    if(ptr == nullptr)
    {
        if(exc)
        {
            CBScriptException ex(wxString::Format(_("Could not Find Value with name: %s"),name) + wxString(_(" from table "))); //+ wxString::FromUTF8(table.getName()))
            throw ex;
        }

        return -2;
    }


    output = *ptr.Get();
    return 0;
}

}
#endif // SC_BINDING_UTIL_H
