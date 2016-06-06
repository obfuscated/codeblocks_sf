
#ifndef SC_CB_VM_H
#define SC_CB_VM_H

#include <scripting/squirrel/squirrel.h>
#include <scripting/sqrat.h>
#include "manager.h"
#include <wx/hashmap.h>
#include <wx/string.h>

namespace ScriptBindings
{

class CBsquirrelVMManager;
class CBSquirrelThread;


/** \brief A helper class to manage Exceptions thrown from the script environment
 */
class CBScriptException //: public Sqrat::Exception
{
public:

    /** \brief Public constructor to use it with wxWidgets Strings
     *
     * \param msg wxString The Message
     *
     */
    CBScriptException(wxString msg);
    ~CBScriptException();

    /** \brief Get the message as a wxString
     *
     * \return wxString The error message transported with this exception
     *
     */
    wxString Message();

private:
    wxString m_message;
};

/** \brief Flags for standard libraries to load.
 *
 * This flags can be used with \ref CBsquirrelVM::CBsquirrelVM, \ref CBsquirrelVM::LoadLibrary and \ref CBsquirrelVM::GetLoadedLibraries
 *  \defgroup vm_library_flags VM Library Flags
 *  \relates CBsquirrelVM
 */
 /** \addtogroup vm_library_flags
 @{
 */
const uint32_t VM_LIB_ALL   = 0xFFFFFFFF;   /**< Load all std libraries */
const uint32_t VM_LIB_IO    = 0x00000001;   /**< Load the file I/O library */
const uint32_t VM_LIB_BLOB  = 0x00000002;   /**< Load the blob library*/
const uint32_t VM_LIB_MATH  = 0x00000004;   /**< Load the Math library (sin, cos, etc...) */
const uint32_t VM_LIB_SYST  = 0x00000008;   /**< Load the system library (clock(), date(), etc...) */
const uint32_t VM_LIB_STR   = 0x00000010;   /**< Load the string library */
/**@}*/



/** \brief Returns the success/fail of the run/compile
    *
    * This are the return values from \ref doString and \ref doFile
*/
    enum SC_ERROR_STATE
    {
        SC_NO_ERROR,       /**< No error occurred */
        SC_COMPILE_ERROR,  /**< A compilation error occurred. Call \ref getLastErrorMsg() to get a detailed description of the error: file and line */
        SC_RUNTIME_ERROR   /**< A runtime error occurred. Call \ref getLastErrorMsg() to get a detailed description of the error */
    };

/** \brief A class to manage the squirrel VM.
 *
 * Based on SqratVM
 */
class CBsquirrelVM
{

private:
    static void compilerErrorHandler(HSQUIRRELVM v,const SQChar* desc,const SQChar* source,SQInteger line,SQInteger column);
    static SQInteger runtimeErrorHandler(HSQUIRRELVM v);


public:



    /** \brief Initialize a squirrel VM and load the specified standard libraries.
     *
     *  This VM catches compile-time and run-time errors. They can be retrieved with GetLastError()
     * \param[in]  int initialStackSize = 1024
     * The size of the initial stack
     * \param[in] const uint32_t library_to_load = VM_LIB_ALL
     * The libraries to be loaded.
     *
     * Libraries can be combined with | operator.  Example:
     * \code CBsquirrelVM(1024,VM_LIB_SYST|VM_LIB_MATH); \endcode \n
     * For a list with flags see \ref vm_library_flags
     */
    CBsquirrelVM(int initialStackSize = 1024,const uint32_t library_to_load = VM_LIB_ALL);
    CBsquirrelVM(HSQUIRRELVM vm, bool close = true);
    ~CBsquirrelVM();

    /** \brief Shuts down the vm
     *
     * Removes the vm from the internal list and closes it in squirrel.
     * \return void
     *
     */
    void Shutdown();

    CBSquirrelThread* CreateThread();

    /** \brief Loads additional std libraries
     * \param[in] library_to_load const uint32_t
     * Libraries to load. Combined with | \n For a list with flags see \ref vm_library_flags
     */
    void LoadLibrary(const uint32_t library_to_load);

    /** \brief Returns all loaded libraries as Flags.
     * For a list with flags see \ref vm_library_flags
     * \return uint32_t Loaded libraries
     */
    uint32_t GetLoadedLibraries()                       {return m_lib_loaded;};

    /** \brief Set the compiler and runtime error handler functions for this vm
     * \param[in] runErr SQFUNCTION      The runtime error handler Function with the signature: SQInteger (*SQFUNCTION)(HSQUIRRELVM);
     * \param[in] comErr SQCOMPILERERROR The compile error handler Function with the signature: typedef void (*SQCOMPILERERROR)(HSQUIRRELVM v,const SQChar * desc,const SQChar *source,SQInteger line,SQInteger column);
     */
    void SetErrorHandler(SQFUNCTION runErr, SQCOMPILERERROR comErr);

    /** \brief Set the compiler and runtime error handler functions for this vm
     * \param[in] printFunc SQPRINTFUNCTION     The print() handler Function with the prototype: printfunc(HSQUIRRELVM v,const SQChar *s,...)
     * \param[in] errFunc SQPRINTFUNCTION       The error() handler Function with the prototype: printfunc(HSQUIRRELVM v,const SQChar *s,...)
     */
    void SetPrintFunc(SQPRINTFUNCTION printFunc, SQPRINTFUNCTION errFunc);

    /** \brief Get the compiler and runtime error handler functions for this vm
     * \param[out] printFunc SQPRINTFUNCTION     The print() handler Function with the prototype: printfunc(HSQUIRRELVM v,const SQChar *s,...)
     * \param[out] errFunc SQPRINTFUNCTION       The error() handler Function with the prototype: printfunc(HSQUIRRELVM v,const SQChar *s,...)
     */
    void GetPrintFunc(SQPRINTFUNCTION& printFunc, SQPRINTFUNCTION& errFunc);

    /** \brief Returns the HSQUIRRELVM object managed by this CBsquirrelVM
     * \return HSQUIRRELVM The vm Object
     */
    HSQUIRRELVM GetSqVM()                 {   return m_vm;    };

    /** \brief Gets the root table
     * \return Sqrat::RootTable& Reference to the rootTable from this vm.
     */
    Sqrat::RootTable&   GetRootTable()  {   return *m_rootTable;    };

    /** \brief Gets the current running script
     * \return Sqrat::Script& Reference to the compiled/running script
     */
    Sqrat::Script&      GetScript()     {   return *m_script;       };

    /** \brief Gets the last error
     *
     * The error is returned in an readable format. It is not differenced between compiler or tuntime error
     * \return wxString The error string
     */
    wxString getLastErrorMsg();

    /** \brief Reports if a error occurred on this vm
     *
     * The detailed error can be retrieved with getLastErrorMsg()
     * \return bool true if a error occurred
     */
    bool HasError();

    /** \brief Sets the last error
     * \param str const Sqrat::string& The error message
     */
    void setLastErrorMsg(const Sqrat::string& str)    { m_lastErrorMsg = str;   };

    /** \name Compiles and runs an string
     */
    /** @{ */
        /**
         * \param[in] str Sqrat::string& A sqrat string to compile and run
         * \return See \ref ERROR_STATE for the return values
        */
    SC_ERROR_STATE doString(const Sqrat::string& str,const Sqrat::string& name);
        /**
         * \param[in] str wxString A wxWidgets string to compile and run
         * \return See \ref ERROR_STATE for the return values
        */
    SC_ERROR_STATE doString(const wxString str,const wxString name);
    /** @} */

    /** \brief Compiles and runs a file
     * \param file const Sqrat::string& Path to a file that has to be compiled and run
     * \return ERROR_STATE See \ref ERROR_STATE for the return values
     */
    SC_ERROR_STATE doFile(const Sqrat::string& file);
    SC_ERROR_STATE doFile(const wxString& file);

    /** \brief Set this vm as the default vm used with sqrat
     *
     * Warning the old vm is lost.
     */
    void SetMeDefault();

private:
    HSQUIRRELVM m_vm;
    Sqrat::RootTable* m_rootTable;
    Sqrat::Script* m_script;
    Sqrat::string m_lastErrorMsg;
    uint32_t m_lib_loaded;
    bool m_shutdwon;
    bool m_close;
};

/** \brief A hash map to save the different vms (not for public use)
 */

WX_DECLARE_HASH_MAP(HSQUIRRELVM,
                    CBsquirrelVM*,
                    wxPointerHash,
                    wxPointerEqual,
                    VMHashMap);



class CBSquirrelThread
{
public:
    CBSquirrelThread(HSQUIRRELVM parent);
    ~CBSquirrelThread();


    SC_ERROR_STATE doString(const wxString script,const wxString name);

    CBsquirrelVM* GetVM()   {return m_vm;};


private:

    bool create_thread();
    bool destroy_thread();

    CBsquirrelVM* m_vm;

    HSQUIRRELVM m_parent_vm;
    //HSQUIRRELVM m_thread;
    HSQOBJECT m_thread_obj;

};

/** \brief A helper class to Manage different squirrel VMs
 *
 * This is almost only needed for the Error handler and will eventually removed...
 */
class CBsquirrelVMManager : public Mgr<CBsquirrelVMManager>
{
        friend class Mgr<CBsquirrelVMManager>;
        wxCriticalSection cs;
    public:

        /** \brief Add a vm to Manage
         * \param[in] vm CBsquirrelVM* The vm which has to be managed
         */
        void AddVM(CBsquirrelVM* vm);

        /** \brief Get the CBsquirrelVM from a HSQUIRRELVM object
         * \param[in] vm HSQUIRRELVM The vm from which the CBsquirrelVM is needed
         * \return CBsquirrelVM* A pointer to the CBsquirrelVM object. nullptr if not found
         */
        CBsquirrelVM* GetVM(HSQUIRRELVM vm);


        /** \name Remove a VM from the list
         */
        /** @{ */
            /**
             * \param[in] vm CBsquirrelVM* A pointer to a CBsquirrelVM that will be removed
             */
        void RemoveVM(CBsquirrelVM* vm);
            /**
             * \param[in] vm HSQUIRRELVM A pointer to a Squirrelvm that will be removed
             */
        void RemoveVM(HSQUIRRELVM vm);
        /** @} */

    protected:


    private:
        CBsquirrelVMManager();
        ~CBsquirrelVMManager();
        CBsquirrelVMManager(cb_unused const CBsquirrelVMManager& rhs); // prevent copy construction
        VMHashMap m_map;

};




/** \brief A class to manage the VM stack easily
 */
class StackHandler
{
public:
    /** \brief Initialize the StackHandler with a SquirrelVM
     * \param vm HSQUIRRELVM Squirrel VM from which the Stack get handled
     */
    StackHandler(HSQUIRRELVM vm);
    ~StackHandler();

    /** \brief Get the parameter count from the stack
     * \return int Returns the Number of parameters on the stack
     */
    int GetParamCount();

    /** \brief Get the variable type from stack on position pos
     * \param pos int
     * \return SQObjectType The type of the object
     */
    SQObjectType GetType(int pos);

    /** \brief Get the object from stack
     * \param pos int
     * \return template<RETURN_VALUE> RETURN_VALUE
     */
    template<typename RETURN_VALUE> RETURN_VALUE GetValue(int pos)
    {
        // Clear all old errors
        Sqrat::Error::Clear(m_vm);

        Sqrat::Var<RETURN_VALUE> val(m_vm,pos);
        if(Sqrat::Error::Occurred(m_vm))
            throw CBScriptException(_("Script Error: GetValue<> failed\n"));
        return val.value;
    }

    /** \brief Get an instance of an object from stack
     * \param pos int
     * \return INSTANCE_TYPE*
     */
    template<typename INSTANCE_TYPE> INSTANCE_TYPE* GetInstance(int pos)
    {
        // Clear all old errors
        Sqrat::Error::Clear(m_vm);
        INSTANCE_TYPE* ret = Sqrat::ClassType<INSTANCE_TYPE>::GetInstance(m_vm, pos);
        if(Sqrat::Error::Occurred(m_vm))
            throw CBScriptException(_("Script Error: GetInstance<> failed\n"));
        return ret;
    }

    /** \brief Push a value to the stack
     *
     * \param value VALUE_TYPE The Value
     * \return template<typename VALUE_TYPE> void
     *
     * For example
     * \code
     * sa.PushValue<int>(55);
     * \endcode
     */
    template<typename VALUE_TYPE> void PushValue(VALUE_TYPE value)
    {
        Sqrat::Var<VALUE_TYPE>::push(m_vm, value);
    }

    template<typename VALUE_TYPE> void PushInstanceCopy(VALUE_TYPE value)
    {
        Sqrat::ClassType<VALUE_TYPE>::PushInstanceCopy(m_vm, value);
    }

    template<typename VALUE_TYPE> void PushInstance(VALUE_TYPE* value)
    {
        Sqrat::ClassType<VALUE_TYPE>::PushInstance(m_vm, value);
    }

    /** \brief Throws an squirrel error
     * \param error SQChar a error description
     * \return SQInteger
     */
    SQInteger ThrowError(const SQChar* error);

    /** \brief Throws an squirrel error
     * \param error SQChar a error description
     * \return SQInteger
     */
    SQInteger ThrowError(const wxString error);

    /** \brief Returns the error message of the last occurred error or an empty string
     *
     * \param del bool
     *
     * This function deletes the error from the Sqrat::Error queue if del is true. other vise it will be kept in the queue
     * \return wxString Error message of the string or an empty string if no error occurred
     */
    wxString GetError(bool del = true);

    /** \brief Returns true if a error occurred
     *
     * \return bool True if a error occurred
     */
    bool HasError();

    /** \brief Returns a backtrace of the current call stack
     *
     * \return wxString
     *
     */
    wxString CreateStackInfo();
    wxString CreateFunctionLocalInfo(int stack_lvl);

    HSQUIRRELVM GetVM()     {return m_vm;};

protected:
private:
    HSQUIRRELVM m_vm;

};

}

#endif // SC_CB_VM_H
