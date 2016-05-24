
#include <scripting/bindings/sc_cb_vm.h>
#include <globals.h>
#include <cbexception.h>
#include <logmanager.h>
#include <wx/msgdlg.h>


#include <scripting/bindings/sq_wx/sq_wx.h>

#include <scripting/squirrel/sqstdblob.h>
#include <scripting/squirrel/sqstdio.h>
#include <scripting/squirrel/sqstdmath.h>
#include <scripting/squirrel/sqstdsystem.h>
#include <scripting/squirrel/sqstdstring.h>



namespace ScriptBindings
{

CBScriptException::CBScriptException(wxString msg) : m_message(msg)
{

}

CBScriptException::~CBScriptException()
{

}

wxString CBScriptException::Message()
{
    return m_message;
}


template<> CBsquirrelVMManager* Mgr<CBsquirrelVMManager>::instance = nullptr;
template<> bool  Mgr<CBsquirrelVMManager>::isShutdown = false;

CBsquirrelVMManager::CBsquirrelVMManager()
{

}

CBsquirrelVMManager::~CBsquirrelVMManager()
{

}

void CBsquirrelVMManager::AddVM(CBsquirrelVM* vm)
{
    wxCriticalSectionLocker locker(cs);
    m_map[vm->GetSqVM()] = vm;
}

CBsquirrelVM* CBsquirrelVMManager::GetVM(HSQUIRRELVM vm)
{
    wxCriticalSectionLocker locker(cs);
    if(m_map.find(vm) == m_map.end())
    {
        // VM not found...
        // Should we throw an exception?
        return nullptr;
    }
    return m_map[vm];
}

void CBsquirrelVMManager::RemoveVM(CBsquirrelVM* vm)
{
    wxCriticalSectionLocker locker(cs);
    VMHashMap::iterator itr = m_map.begin();
    for(;itr != m_map.end();itr++)
    {
        if(itr->second == vm)
        {
            m_map.erase(itr);
        }
    }
}

void CBsquirrelVMManager::RemoveVM(HSQUIRRELVM vm)
{
    wxCriticalSectionLocker locker(cs);
    m_map.erase(vm);
}

CBsquirrelVM::CBsquirrelVM(HSQUIRRELVM vm,bool close) : m_vm(vm)
                    , m_rootTable(new Sqrat::RootTable(m_vm))
                    , m_script(new Sqrat::Script(m_vm))
                    , m_lastErrorMsg()
                    , m_shutdwon(false)
                    , m_close(close)
{
    CBsquirrelVMManager::Get()->AddVM(this);
    m_lib_loaded = 0;

    sq_enabledebuginfo(m_vm,true);

    SetErrorHandler(runtimeErrorHandler,compilerErrorHandler);

    m_lastErrorMsg.clear();
}

CBsquirrelVM::CBsquirrelVM(int initialStackSize,const uint32_t library_to_load) : m_vm(sq_open(initialStackSize))
        , m_rootTable(new Sqrat::RootTable(m_vm))
        , m_script(new Sqrat::Script(m_vm))
        , m_lastErrorMsg()
        , m_shutdwon(false)
{
    //Register VM in the Manager
    CBsquirrelVMManager::Get()->AddVM(this);

    //Register std libs
    sq_pushroottable(m_vm);     // The register functions aspects a Table on the stack

    if(library_to_load & VM_LIB_IO)
        sqstd_register_iolib(m_vm);
    if(library_to_load & VM_LIB_BLOB)
        sqstd_register_bloblib(m_vm);
    if(library_to_load & VM_LIB_MATH)
        sqstd_register_mathlib(m_vm);
    if(library_to_load & VM_LIB_SYST)
        sqstd_register_systemlib(m_vm);
    if(library_to_load & VM_LIB_STR)
        sqstd_register_stringlib(m_vm);
    sq_pop(m_vm, 1);    // Pop the root table

    m_lib_loaded = library_to_load;

    sq_enabledebuginfo(m_vm,true);

    SetErrorHandler(runtimeErrorHandler,compilerErrorHandler);

    m_lastErrorMsg.clear();

    // FIXME (bluehazzard#1#): temporary set me as default vm, if CB supports multi vm this should be removed!!!!
    Sqrat::DefaultVM::Set(m_vm);
}

CBsquirrelVM::~CBsquirrelVM()
{
    if(!m_shutdwon)
        Shutdown();
}

void CBsquirrelVM::Shutdown()
{
    CBsquirrelVMManager::Get()->RemoveVM(m_vm); // Remove this vm from the managed list
    delete m_script;
    delete m_rootTable;
    if(m_close)
        sq_close(m_vm);

    m_shutdwon = true;
}

CBSquirrelThread* CBsquirrelVM::CreateThread()
{
    try
    {
        CBSquirrelThread* ret = new CBSquirrelThread(this->GetSqVM());
        return ret;
    } catch(CBScriptException &e)
    {
        Manager::Get()->GetLogManager()->LogError(_("CBsquirrelVM::CreateThread(): ")+ e.Message());
        return nullptr;
    }
    return nullptr;
}

void CBsquirrelVM::LoadLibrary(const uint32_t library_to_load)
{
    sq_pushroottable(m_vm); // The register functions aspects a Table on the stack
    if(library_to_load & VM_LIB_IO)
        sqstd_register_iolib(m_vm);
    if(library_to_load & VM_LIB_BLOB)
        sqstd_register_bloblib(m_vm);
    if(library_to_load & VM_LIB_MATH)
        sqstd_register_mathlib(m_vm);
    if(library_to_load & VM_LIB_SYST)
        sqstd_register_systemlib(m_vm);
    if(library_to_load & VM_LIB_STR)
        sqstd_register_stringlib(m_vm);
    sq_pop(m_vm, 1);    // Pop the root table

    m_lib_loaded |= library_to_load;
}

SQInteger CBsquirrelVM::runtimeErrorHandler(HSQUIRRELVM v)
{
    CBsquirrelVM* sq_vm = CBsquirrelVMManager::Get()->GetVM(v);
    StackHandler sa(v);
    if(sq_vm == NULL)
    {
        // Something funny happened.
        Manager::Get()->GetLogManager()->LogError(_T("Could not find squirrel VM in runtimeErrorHandler! This is a Program Error. Please report it to the developers!"));
        return SQ_ERROR;
    }
    const SQChar *sErr = 0;
    wxString ErrorMessage(_T("Squirrel Runtime Error:\n"));
    ErrorMessage.Append(sa.CreateStackInfo());
    ErrorMessage.Append(_T("\nError:\n"));
    if(sq_gettop(v) >= 1)
    {
        if(SQ_SUCCEEDED(sq_getstring(v, 2, &sErr)))
        {
            ErrorMessage.Append(wxString::FromUTF8(sErr));
        }
        else
        {
            ErrorMessage.Append(_T("An Unknown RuntimeError Occurred."));
        }
    }
    // Append local variables of the last function
    ErrorMessage.Append(_T("\n\nLocal Variables:\n"));
    ErrorMessage.Append(sa.CreateFunctionLocalInfo(1));

    sq_vm->m_lastErrorMsg = ErrorMessage.ToUTF8();
    return SQ_ERROR;
}

// Default Error Handler
void CBsquirrelVM::compilerErrorHandler(HSQUIRRELVM v,
                            const SQChar* desc,
                            const SQChar* source,
                            SQInteger line,
                            SQInteger column)
{
    int buffer_size = 128;
    SQChar *tmp_buffer;
    for(;;buffer_size*=2)
    {
        tmp_buffer = new SQChar [buffer_size];
        int retvalue = snprintf(tmp_buffer,buffer_size, _SC("\nSource: %s\nline: %d\ncolumn:%d\n%s"), source, (int) line, (int) column, desc);
        if(retvalue < buffer_size)
        {
            // Buffersize was large enough
            CBsquirrelVM* sq_vm = CBsquirrelVMManager::Get()->GetVM(v);
            if(sq_vm == NULL)
            {
                // Something funny happened.
                // In reality this shouldn't happen never...
                Manager::Get()->GetLogManager()->LogError(_("Could not find squirrel VM in compilerErrorHandler! This is a Program Error. Please report it to the developers!"));
                return;
            }
            sq_vm->m_lastErrorMsg = tmp_buffer;

            delete[] tmp_buffer;
            break;
        }
        // Buffer size was not enough
        delete[] tmp_buffer;
    }
}

void CBsquirrelVM::SetPrintFunc(SQPRINTFUNCTION printFunc, SQPRINTFUNCTION errFunc)
{
    sq_setprintfunc(m_vm, printFunc, errFunc);
}

void CBsquirrelVM::GetPrintFunc(SQPRINTFUNCTION& printFunc, SQPRINTFUNCTION& errFunc)
{
    printFunc   = sq_getprintfunc(m_vm);
    errFunc     = sq_geterrorfunc(m_vm);
}

void CBsquirrelVM::SetErrorHandler(SQFUNCTION runErr, SQCOMPILERERROR comErr)
{
    sq_newclosure(m_vm, runErr, 0);
    sq_seterrorhandler(m_vm);
    sq_setcompilererrorhandler(m_vm, comErr);
}

SC_ERROR_STATE CBsquirrelVM::doString(const Sqrat::string& str,const Sqrat::string& name)
{
    Sqrat::string msg;
    m_lastErrorMsg.clear();
    if(!m_script->CompileString(str, msg,name))
    {
        if(m_lastErrorMsg.empty())
        {
            m_lastErrorMsg = msg;
        }
        return SC_COMPILE_ERROR;
    }
    if(!m_script->Run(msg))
    {
        if(m_lastErrorMsg.empty())
        {
            m_lastErrorMsg = msg;
        }
        return SC_RUNTIME_ERROR;
    }
    return SC_NO_ERROR;
}

SC_ERROR_STATE CBsquirrelVM::doFile(const wxString& file)
{
    return doFile(Sqrat::string(file.mb_str()));
}

SC_ERROR_STATE CBsquirrelVM::doFile(const Sqrat::string& file)
{
    Sqrat::string msg;
    m_lastErrorMsg.clear();
    if(!m_script->CompileFile(file, msg))
    {
        if(m_lastErrorMsg.empty())
        {
            m_lastErrorMsg = msg;
        }
        return SC_COMPILE_ERROR;
    }
    if(!m_script->Run(msg))
    {
        if(m_lastErrorMsg.empty())
        {
            m_lastErrorMsg = msg;
        }
        return SC_RUNTIME_ERROR;
    }
    return SC_NO_ERROR;
}


SC_ERROR_STATE CBsquirrelVM::doString(const wxString str, const wxString name)
{
    return doString(Sqrat::string(str.ToUTF8()),Sqrat::string(name.ToUTF8()));
}

void CBsquirrelVM::SetMeDefault()
{
    if(m_vm == nullptr)
        cbThrow(_("Cant set nullptr as default vm"));
    Sqrat::DefaultVM::Set(m_vm);
}

wxString CBsquirrelVM::getLastErrorMsg()
{
    wxString ErrorMessage;
    ErrorMessage.Append(wxString(m_lastErrorMsg.c_str(),wxConvUTF8));
    m_lastErrorMsg.clear();

    if(Sqrat::Error::Occurred(m_vm))
    {
        ErrorMessage.Append(_("\nSqrat Error:\n"));
        ErrorMessage.Append(wxString::FromUTF8(Sqrat::Error::Message(m_vm).c_str()));
        Sqrat::Error::Clear(m_vm);
    }
    return ErrorMessage;
};

bool CBsquirrelVM::HasError()
{
    if(!m_lastErrorMsg.empty() ||  Sqrat::Error::Occurred(m_vm))
        return true;
    else
        return false;
}


/******************************************************************/
/* CBSquirrelThread                                               */
/******************************************************************/

CBSquirrelThread::CBSquirrelThread(HSQUIRRELVM vm) : m_parent_vm(vm)
{
    create_thread();
}

CBSquirrelThread::~CBSquirrelThread()
{
    destroy_thread();
}

bool CBSquirrelThread::create_thread()
{
    if(m_parent_vm == nullptr)
    {
        throw CBScriptException(_("CBSquirrelThread::create_thread(): m_parent_vm == nullptr"));
    }

    HSQUIRRELVM thread = sq_newthread(m_parent_vm,1024);

    if(thread == nullptr)
        return false;

    sq_resetobject(&m_thread_obj);
    if(SQ_FAILED(sq_getstackobj(m_parent_vm,-1,&m_thread_obj)))
    {
        throw CBScriptException(_("CBSquirrelThread::create_thread(): SQ_FAILED(sq_getstackobj(m_parent_vm,-1,&thread))"));
    }
    else
    {
        sq_addref(m_parent_vm,&m_thread_obj);
        sq_pop(m_parent_vm,1);
    }

    // Create a isolated vm, with its own environment
    HSQOBJECT environment;
    sq_resetobject(&environment);
    sq_newtable(thread);

    // store the table in environment object
    if(sq_getstackobj(thread, -1, &environment) < 0)
    {
        throw CBScriptException(_("CBSquirrelThread::create_thread(): if(sq_getstackobj(thread, -1, &environment)<0)"));
    }

    //increment the ref counter of the object
    sq_addref(thread, &environment);
    sq_pop(thread, 1);

    // set old roottable as delegate of environment
    sq_pushobject(thread, environment);
    sq_pushroottable(thread);
    sq_setdelegate(thread, -2);
    sq_pop(thread, 1);

    // set environment as new roottable
    // if we want to isolate the enviroment even more, we should overwrite the _set metamethod
    sq_pushobject(thread, environment);
    sq_setroottable(thread);

    sq_release(thread, &environment);

    m_vm = new CBsquirrelVM(thread,false);

    return true;
}

bool CBSquirrelThread::destroy_thread()
{
    if(m_vm)
    {
        m_vm->Shutdown();
        delete m_vm;
        m_vm = nullptr;
    }

    // We close the VM in m_vm->Shutdown(), so also this object should get released...
    sq_release(m_parent_vm,&m_thread_obj);
    return true;
}

SC_ERROR_STATE CBSquirrelThread::doString(const wxString script,const wxString name)
{
    return m_vm->doString(script,name);
}


/******************************************************************/
/* Stack Handler                                                  */
/******************************************************************/


StackHandler::StackHandler(HSQUIRRELVM vm) : m_vm(vm)
{

}

StackHandler::~StackHandler()
{

}

int StackHandler::GetParamCount()
{
    return sq_gettop(m_vm);
}

SQObjectType StackHandler::GetType(int pos)
{
    return sq_gettype(m_vm,pos);
}

SQInteger StackHandler::ThrowError(const SQChar* error)
{
    return sq_throwerror(m_vm,error);
}


SQInteger StackHandler::ThrowError(const wxString error)
{

    wxString tmp = _("Stack Handler: ") + error + _("\n")+ CreateStackInfo() + _("\n Local variables:\n") + CreateFunctionLocalInfo(1);

    return sq_throwerror(m_vm,tmp.ToUTF8().data());
}

wxString StackHandler::CreateStackInfo()
{
    SQStackInfos si;
    int stack = 1; // lvl 0 is the sq_stackinfos itself
    wxString stack_string(_("Call Stack: \n"));
    while(SQ_SUCCEEDED(sq_stackinfos(m_vm,stack,&si)))
    {
        stack_string << wxString::Format(_("[%02i] "),stack) << wxString(si.funcname,wxConvUTF8)
                     << _T("\t(") << wxString(si.source,wxConvUTF8) << _(":") << si.line << _T(")\n");
        stack++;
    }
    return stack_string;
}

wxString StackHandler::CreateFunctionLocalInfo(int stack_lvl)
{
    SQStackInfos    si;
    wxString stack_string;
    int seq=0;
    const SQChar *name=0;
    const SQChar *s=0;
    SQInteger i;
    SQFloat f;
    SQBool b;

    if(SQ_SUCCEEDED(sq_stackinfos(m_vm,stack_lvl,&si)))
    {
            stack_string << _T("FUNCTION: ") << wxString(si.funcname,wxConvUTF8)
                         <<_(" (") << wxString(si.source,wxConvUTF8) << _(":") << si.line << _T(")\n");
    }

    while((name = sq_getlocal(m_vm,stack_lvl,seq)))
    {
        seq++;
        stack_string << _("\t") << wxString(name,wxConvUTF8) << _("\t");
        switch(sq_gettype(m_vm,-1))
        {
        case OT_NULL:
            stack_string  << _(":(NULL)\n");
            break;
        case OT_INTEGER:
            sq_getinteger(m_vm,-1,&i);
            stack_string << _(":(int)\t= ") << i << _("\n");
            break;
        case OT_FLOAT:
            sq_getfloat(m_vm,-1,&f);
            stack_string << _(":(float)\t= ") << f << _("\n");
            break;
        case OT_USERPOINTER:
            stack_string << _(":(userpointer)\n");
            break;
        case OT_STRING:
            sq_getstring(m_vm,-1,&s);
            stack_string << _(":(string)\t= ") << wxString(s,wxConvUTF8) << _("\n");
            break;
        case OT_TABLE:
            stack_string << _(":(table)\n");
            break;
        case OT_ARRAY:
            stack_string << _(":(array)\n");
            break;
        case OT_CLOSURE:
            stack_string << _(":(closure)\n");
            break;
        case OT_NATIVECLOSURE:
            stack_string << _(":(native closure)\n");
            break;
        case OT_GENERATOR:
            stack_string << _(":(generator)\n");
            break;
        case OT_USERDATA:
            stack_string << _(":(user data)\n");
            break;
        case OT_THREAD:
            stack_string << _(":(thread)\n");
            break;
        case OT_CLASS:
            stack_string << _(":(class)\n");
            break;
        case OT_INSTANCE:
            stack_string << _(":(instance)\n");
            break;
        case OT_WEAKREF:
            stack_string << _(":(weak ref)\n");
            break;
        case OT_BOOL:
            sq_getbool(m_vm,-1,&b);
            stack_string << _(":(bool)\t= ") << (b?_("true"):_("false")) << _("\n");
        break;
        default:
            stack_string << _("\t") << _T("Stack error at lvl: ") << seq << _("\n");
            break;
        }
        sq_pop(m_vm,1);
    }
    return stack_string;
}

wxString StackHandler::GetError(bool del)
{
    if(!Sqrat::Error::Occurred(m_vm))
        return wxEmptyString;

    wxString stack_string = CreateStackInfo();
    stack_string += _("\nSqrat Error\n");
    stack_string += wxString::FromUTF8(Sqrat::Error::Message(m_vm).c_str());

    if(del)
        Sqrat::Error::Clear(m_vm);

    return stack_string;
}

bool StackHandler::HasError()
{
    return Sqrat::Error::Occurred(m_vm);
}

} // namespace ScriptBindings











