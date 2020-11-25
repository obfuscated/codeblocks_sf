/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CB_SC_UTILS_H
#define CB_SC_UTILS_H

#include <assert.h>
#include <squirrel.h>
#include <string>
#include <sstream>
#include <stdarg.h>

// FIXME (squirrel) Explain how things below actually work.

namespace ScriptBindings
{

void PrintStack(HSQUIRRELVM vm, const char *title);

struct PreserveTop
{
    PreserveTop(HSQUIRRELVM vm) : m_vm(vm), m_top(sq_gettop(vm)) {}
    ~PreserveTop()
    {
        if (m_checkAtDtor)
            check();
        sq_settop(m_vm, m_top);
    }

    void check()
    {
#if !defined(NDEBUG)
        if (sq_gettop(m_vm) != m_top)
            PrintStack(m_vm, "PreserveTop::check failure!");
#endif
        assert(sq_gettop(m_vm) == m_top);
    }
    void noCheck() { m_checkAtDtor = false; }
private:
    HSQUIRRELVM m_vm;
    SQInteger m_top;
    bool m_checkAtDtor = true;
};

inline bool GetRootTableField(HSQUIRRELVM vm, const SQChar *fieldName)
{
    sq_pushroottable(vm);
    sq_pushstring(vm, fieldName, -1);

    //gets the field 'fieldName' from the root table
    const bool result = SQ_SUCCEEDED(sq_get(vm, -2));
    sq_remove(vm, -2); // remove the root table
    return result;
}

template<typename T>
struct TypeInfo {};

enum class InstanceAllocationMode : uint32_t
{
    InstanceIsInline,
    InstanceIsOwnedPtr,
    InstanceIsNonOwnedPtr
};

constexpr size_t smax(size_t a, size_t b)
{
    return a < b ? b : a;
}

template<typename UserType>
struct UserDataForType
{
    InstanceAllocationMode mode;
    /// Make it possible to store either whole object or a pointer to an object.
    /// Make sure the data is properly aligned.

    union {
        typename std::aligned_storage<
            smax(sizeof(UserType), sizeof(UserType*)),
            smax(alignof(UserType), alignof(UserType*))
        >::type userdata;
        UserType *userptr;
    };
};

template<typename UserType>
SQInteger ReleaseHook(SQUserPointer ptr, cb_unused SQInteger size)
{
    UserDataForType<UserType> &data = *reinterpret_cast<UserDataForType<UserType>*>(ptr);

    switch (data.mode)
    {
    case InstanceAllocationMode::InstanceIsInline:
        {
            UserType *p = reinterpret_cast<UserType*>(&data.userdata);
            p->~UserType();
            break;
        }
    default:
        assert(false);
    }

    return 0;
}

template<typename UserType, InstanceAllocationMode mode>
UserDataForType<UserType>* SetupUserPointer(HSQUIRRELVM vm, SQInteger idx)
{
    SQUserPointer ptr = nullptr;
    if (SQ_FAILED(sq_getinstanceup(vm, idx, &ptr,
                                   SQUserPointer(TypeInfo<UserType>::typetag))))
    {
        sq_throwerror(vm, _SC("SetupUserPointer: Invalid type tag!"));
        assert(false);
        return nullptr;
    }

    UserDataForType<UserType> *data = reinterpret_cast<UserDataForType<UserType>*>(ptr);
    data->mode = mode;
    // FIXME (squirrel) C++17 feature?!!! Could be replaced with a trait, but should we?
    if constexpr (mode == InstanceAllocationMode::InstanceIsInline)
        sq_setreleasehook(vm, idx, ReleaseHook<UserType>);
    return data;
}

template<typename UserType>
inline bool ExtractUserPointer(UserType *&self, HSQUIRRELVM vm, SQInteger idx, uint32_t tag)
{
    self = nullptr;
    SQUserPointer ptr = nullptr;
    if (SQ_FAILED(sq_getinstanceup(vm, idx, &ptr, SQUserPointer(uint64_t(tag)))))
    {
        assert(false);
        return false;
    }

    UserDataForType<UserType> &data = *reinterpret_cast<UserDataForType<UserType>*>(ptr);

    switch (data.mode)
    {
    case InstanceAllocationMode::InstanceIsInline:
        self = reinterpret_cast<UserType*>(&data.userdata);
        return self != nullptr;
    case InstanceAllocationMode::InstanceIsOwnedPtr:
    case InstanceAllocationMode::InstanceIsNonOwnedPtr:
        //self = *(UserType**)(ptr + sizeof(InstanceAllocationMode));
        self = data.userptr;
        assert(data.mode!=InstanceAllocationMode::InstanceIsOwnedPtr);
        return self != nullptr;
    }

    return self != nullptr;
}

struct SkipParam {};

struct ExtractParamsBase
{
    ExtractParamsBase(HSQUIRRELVM vm) : m_vm(vm) {}

    template<typename Arg>
    bool ProcessParam(Arg &arg, int stackIndex, const char *funcStr)
    {
        using NoPtrAndCVType = typename std::remove_cv<typename std::remove_pointer<Arg>::type>::type;

        arg = nullptr;
        if (!ExtractUserPointer(arg, m_vm, stackIndex, TypeInfo<NoPtrAndCVType>::typetag))
        {
#if SQUIRREL_VERSION_NUMBER>=300
            scsprintf(m_errorMessage, 500, _SC("Extracting '%s' in '%s' failed for index %d"),
                      typeid(Arg).name(), funcStr, stackIndex);
#else
            scsprintf(m_errorMessage, _SC("Extracting '%s' in '%s' failed for index %d"),
                      typeid(Arg).name(), funcStr, stackIndex);
#endif
            return false;
        }

        return true;
    }

    bool ProcessParam(float &f, int stackIndex, const char *funcStr)
    {
        SQFloat temp;
        if (SQ_FAILED(sq_getfloat(m_vm, stackIndex, &temp)))
        {
#if SQUIRREL_VERSION_NUMBER>=300
            scsprintf(m_errorMessage, 500, _SC("Extract float in '%s' failed for index %d"),
                      funcStr, stackIndex);
#else
            scsprintf(m_errorMessage, _SC("Extract float in '%s' failed for index %d"),
                      funcStr, stackIndex);
#endif
            return false;
        }
        f = temp;
        return true;
    }

    bool ProcessParam(SQInteger &i, int stackIndex, const char *funcStr)
    {
        SQInteger temp;
        if (SQ_FAILED(sq_getinteger(m_vm, stackIndex, &temp)))
        {
#if SQUIRREL_VERSION_NUMBER>=300
            scsprintf(m_errorMessage, 500, _SC("Extract int in '%s' failed for index %d"),
                      funcStr, stackIndex);
#else
            scsprintf(m_errorMessage, _SC("Extract int in '%s' failed for index %d"),
                      funcStr, stackIndex);
#endif
            return false;
        }
        i = temp;
        return true;
    }

    bool ProcessParam(bool &b, int stackIndex, const char *funcStr)
    {
        SQBool temp;
        if (SQ_FAILED(sq_getbool(m_vm, stackIndex, &temp)))
        {
#if SQUIRREL_VERSION_NUMBER>=300
            scsprintf(m_errorMessage, 500, _SC("Extract bool in '%s' failed for index %d"),
                      funcStr, stackIndex);
#else
            scsprintf(m_errorMessage, _SC("Extract bool in '%s' failed for index %d"),
                      funcStr, stackIndex);
#endif
            return false;
        }
        b = temp;
        return true;
    }

    bool ProcessParam(const SQChar *&c, int stackIndex, const char *funcStr)
    {
        const SQChar *temp;
        if (SQ_FAILED(sq_getstring(m_vm, stackIndex, &temp)))
        {
#if SQUIRREL_VERSION_NUMBER>=300
            scsprintf(m_errorMessage, 500, _SC("Extract string in '%s' failed for index %d"),
                      funcStr, stackIndex);
#else
            scsprintf(m_errorMessage, _SC("Extract string in '%s' failed for index %d"),
                      funcStr, stackIndex);
#endif
            return false;
        }

        c = temp;
        return true;
    }

    bool ProcessParam(cb_unused SkipParam &v, cb_unused int stackIndex,
                      cb_unused const char *funcStr)
    {
        // This allows us to skip the param for some reason.
        return true;
    }

    bool CheckNumArguments(int expected, const char *funcStr)
    {
        const int numArgs = sq_gettop(m_vm);
        if (numArgs != expected)
        {
#if SQUIRREL_VERSION_NUMBER>=300
            scsprintf(m_errorMessage, 500,
                      _SC("Wrong number of arguments to '%s' - expected %d given %d (often one argument is an implicit 'this' table)!"),
                      funcStr, expected, numArgs);
#else
            scsprintf(m_errorMessage,
                      _SC("Wrong number of arguments to '%s' - expected %d given %d (often one argument is an implicit 'this' table)!"),
                      funcStr, expected, numArgs);
#endif
            return false;
        }
        return true;
    }

    bool CheckNumArguments(int expectedMin, int expectedMax, const char *funcStr)
    {
        const int numArgs = sq_gettop(m_vm);
        if (numArgs < expectedMin || numArgs > expectedMax)
        {
#if SQUIRREL_VERSION_NUMBER>=300
            scsprintf(m_errorMessage, 500,
                      _SC("Wrong number of arguments to '%s' - expected [%d;%d] given %d (often one argument is an implicit 'this' table)!"),
                      funcStr, expectedMin, expectedMax, numArgs);
#else
            scsprintf(m_errorMessage,
                      _SC("Wrong number of arguments to '%s' - expected [%d;%d] given %d (often one argument is an implicit 'this' table)!"),
                      funcStr, expectedMin, expectedMax, numArgs);
#endif
            return false;
        }
        return true;
    }

    int ErrorMessage()
    {
        return sq_throwerror(m_vm, m_errorMessage);
    }

protected:
    HSQUIRRELVM m_vm;
    SQChar m_errorMessage[500];
};

template<typename Arg0>
struct ExtractParams1 : ExtractParamsBase
{
    ExtractParams1(HSQUIRRELVM vm) : ExtractParamsBase(vm) {}

    bool Process(const char *funcStr)
    {
        if (!CheckNumArguments(1, funcStr))
            return false;
        if (!ProcessParam(p0, 1, funcStr))
            return false;
        return true;
    }

    Arg0 p0;
};

template<typename Arg0, typename Arg1>
struct ExtractParams2 : ExtractParamsBase
{
    ExtractParams2(HSQUIRRELVM vm) : ExtractParamsBase(vm) {}

    bool Process(const char *funcStr)
    {
        if (!CheckNumArguments(2, funcStr))
            return false;
        if (!ProcessParam(p0, 1, funcStr))
            return false;
        if (!ProcessParam(p1, 2, funcStr))
            return false;
        return true;
    }

    Arg0 p0;
    Arg1 p1;
};

template<typename Arg0, typename Arg1, typename Arg2>
struct ExtractParams3 : ExtractParamsBase
{
    ExtractParams3(HSQUIRRELVM vm) : ExtractParamsBase(vm) {}

    bool Process(const char *funcStr)
    {
        if (!CheckNumArguments(3, funcStr))
            return false;
        if (!ProcessParam(p0, 1, funcStr))
            return false;
        if (!ProcessParam(p1, 2, funcStr))
            return false;
        if (!ProcessParam(p2, 3, funcStr))
            return false;
        return true;
    }

    Arg0 p0;
    Arg1 p1;
    Arg2 p2;
};

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
struct ExtractParams4 : ExtractParamsBase
{
    ExtractParams4(HSQUIRRELVM vm) : ExtractParamsBase(vm) {}

    bool Process(const char *funcStr)
    {
        if (!CheckNumArguments(4, funcStr))
            return false;
        if (!ProcessParam(p0, 1, funcStr))
            return false;
        if (!ProcessParam(p1, 2, funcStr))
            return false;
        if (!ProcessParam(p2, 3, funcStr))
            return false;
        if (!ProcessParam(p3, 4, funcStr))
            return false;
        return true;
    }

    Arg0 p0;
    Arg1 p1;
    Arg2 p2;
    Arg3 p3;
};

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
struct ExtractParams5 : ExtractParamsBase
{
    ExtractParams5(HSQUIRRELVM vm) : ExtractParamsBase(vm) {}

    bool Process(const char *funcStr)
    {
        if (!CheckNumArguments(5, funcStr))
            return false;
        if (!ProcessParam(p0, 1, funcStr))
            return false;
        if (!ProcessParam(p1, 2, funcStr))
            return false;
        if (!ProcessParam(p2, 3, funcStr))
            return false;
        if (!ProcessParam(p3, 4, funcStr))
            return false;
        if (!ProcessParam(p4, 5, funcStr))
            return false;
        return true;
    }

    Arg0 p0;
    Arg1 p1;
    Arg2 p2;
    Arg3 p3;
    Arg4 p4;
};

template<typename UserType>
inline SQInteger CreateClassDecl(HSQUIRRELVM v, const SQChar *className,
                                 const SQChar *baseClassName = nullptr)
{
    const SQInteger tableStackIdx = sq_gettop(v);
    sq_pushstring(v, className, -1);
    if (baseClassName)
    {
        sq_pushstring(v, baseClassName, -1);
        if (SQ_FAILED(sq_get(v, -3)))
        {
            assert(false);
            return -1;
        }
    }
    sq_newclass(v, ((baseClassName != nullptr) ? SQTrue : SQFalse));
    sq_settypetag(v, -1, SQUserPointer(TypeInfo<UserType>::typetag));
    // Add some memory to the class. We over-allocate in cases where we store only a pointer.
    sq_setclassudsize(v, -1, sizeof(UserDataForType<UserType>));

    return tableStackIdx;
}

/// Creates a squirrel UserType instance and pushes it on the stack.
/// The C++ object/memory is not initialized. The caller must do it before proceeding.
template<typename UserType>
inline UserDataForType<UserType>* CreateInlineInstance(HSQUIRRELVM v)
{
    if (!GetRootTableField(v, TypeInfo<UserType>::className))
    {
        assert(false);
        sq_throwerror(v, _SC("CreateInlineInstance: Getting class name failed!"));
        return nullptr;
    }
    sq_createinstance(v, -1);
    sq_remove(v, -2); // remove class object

    return SetupUserPointer<UserType, InstanceAllocationMode::InstanceIsInline>(v, -1);
}

/// Create an instance of a given type which references some native memory.
/// This creates a unique squirrel object every time the method is called.
/// The C++ memory is the same for all object and it allows modifications.
template<typename UserType>
inline UserDataForType<UserType>* CreateNonOwnedPtrInstance(HSQUIRRELVM v, UserType *value)
{
    if (!GetRootTableField(v, TypeInfo<UserType>::className))
    {
        assert(false);
        sq_throwerror(v, _SC("CreateNonOwnedPtrInstance: Getting class name failed!"));
        return nullptr;
    }
    sq_createinstance(v, -1);
    sq_remove(v, -2); // remove class object

    UserDataForType<UserType> *data;
    data = SetupUserPointer<UserType, InstanceAllocationMode::InstanceIsNonOwnedPtr>(v, -1);
    if (data)
        data->userptr = value;
    return data;
}

/// Get a reference to an object in the middle of the stack and copy it at the top.
/// Hopefully this handles internal references correctly.
inline void DuplicateInstance(HSQUIRRELVM v, SQInteger idx)
{
    HSQOBJECT obj;
    sq_resetobject(&obj);
    sq_getstackobj(v, idx, &obj);
    // Skipping addref and release. Seems to work without them. I hope it is reliable.
    sq_pushobject(v, obj);
}

/// This expects the class to be on the stack.
inline void BindMethod(HSQUIRRELVM v, const SQChar *name, SQFUNCTION func, const SQChar *humanName)
{
    sq_pushstring(v, name, -1);
    sq_newclosure(v, func, 0);
    if (humanName)
        sq_setnativeclosurename(v, -1, humanName);
    sq_newslot(v, -3, SQFalse);
}

/// This expects the class to be on the stack.
inline void BindStaticMethod(HSQUIRRELVM v, const SQChar *name, SQFUNCTION func, const SQChar *humanName)
{
    sq_pushstring(v, name, -1);
    sq_newclosure(v, func, 0);
    if (humanName)
        sq_setnativeclosurename(v, -1, humanName);
    sq_newslot(v, -3, SQTrue);
}

template<typename UserType>
SQInteger Generic_DefaultCtor(HSQUIRRELVM v)
{
    const int numArgs = sq_gettop(v);
    if (numArgs != 1)
        return sq_throwerror(v, _SC("Generic_DefaultCtor: Incorrect number of arguments!"));

    UserDataForType<UserType> *data;
    data = SetupUserPointer<UserType, InstanceAllocationMode::InstanceIsInline>(v, 1);
    if (!data)
        return -1; // SetupUserPointer should have called sq_throwerror!
    new (&(data->userdata)) UserType();

    return 0;
}

template<typename UserType>
void BindEmptyCtor(HSQUIRRELVM v)
{
    BindMethod(v, _SC("constructor"), Generic_DefaultCtor<UserType>, nullptr);
}

template<typename ReturnType, typename ClassType, ReturnType (ClassType::*func)() const>
SQInteger NoParamGetterInt(HSQUIRRELVM v)
{
    ExtractParams1<const ClassType*> extractor(v);
    if (!extractor.Process("NoParamGetter"))
        return extractor.ErrorMessage();
    sq_pushinteger(v, (extractor.p0->*func)());
    return 1;
}

inline SQInteger ThrowIndexNotFound(HSQUIRRELVM v)
{
#if SQUIRREL_VERSION_NUMBER >= 300
    sq_pushnull(v);
    sq_throwobject(v); // no throw object in 2.x, but it seems we don't need it!?
#endif
    return -1;
}

} // namespace ScriptBindings

#endif // CB_SC_UTILS_H