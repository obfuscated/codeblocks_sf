#ifndef SQ_WX_TYPE_HANDLER_H_INCLUDED
#define SQ_WX_TYPE_HANDLER_H_INCLUDED

#include <wx/string.h>
#include <scripting/sqrat.h>

namespace Sqrat
{
template<>
struct Var<wxString> {
    wxString value;
    Var(HSQUIRRELVM v, SQInteger idx) {
        if (!Sqrat::Error::Occurred(v)) {
            wxString* ptr = ClassType<wxString>::GetInstance(v, idx);
            if (ptr != NULL) {
                value = *ptr;
            } else {
                Sqrat::Error::Clear(v);
                switch(sq_gettype(v,idx))
                {
                case OT_INTEGER:    // It is a const char ''
                    {
                        SQInteger val;
                        sq_getinteger(v,idx,&val);
                        value.Printf(wxT("%c"),static_cast<char>(val));
                        break;
                    }
                default:
                    {
                        const SQChar* str;
                        sq_tostring(v, idx);
                        sq_getstring(v, -1, &str);
                        value = wxString::FromUTF8(str);
                        sq_pop(v, 1);
                        break;
                    }
                }
            }
        }
    }

    static void push(HSQUIRRELVM v, const wxString& value) {
        ClassType<wxString>::PushInstanceCopy(v, value);
    }
};

template<>
struct Var<const wxString> {
    wxString value;
    Var(HSQUIRRELVM v, SQInteger idx) {
        if (!Sqrat::Error::Occurred(v)) {
            wxString* ptr = ClassType<wxString>::GetInstance(v, idx);
            if (ptr != NULL) {
                value = *ptr;
            } else {
                Sqrat::Error::Clear(v);
                switch(sq_gettype(v,idx))
                {
                case OT_INTEGER:    // It is a const char ''
                    {
                        SQInteger val;
                        sq_getinteger(v,idx,&val);
                        value.Printf(wxT("%c"),static_cast<char>(val));
                        break;
                    }
                default:
                    {
                        const SQChar* str;
                        sq_tostring(v, idx);
                        sq_getstring(v, -1, &str);
                        value = wxString::FromUTF8(str);
                        sq_pop(v, 1);
                        break;
                    }
                }
            }
        }
    }

    static void push(HSQUIRRELVM v, const wxString value) {
        ClassType<wxString>::PushInstanceCopy(v, value);
    }
};


template<>
struct Var<wxString&> {

    wxString value;

    Var(HSQUIRRELVM v, SQInteger idx) {
        if (!Sqrat::Error::Occurred(v)) {
            wxString* ptr = ClassType<wxString>::GetInstance(v, idx);
            if (ptr != NULL) {
                value = *ptr;
            } else {
                Sqrat::Error::Clear(v);
                const SQChar* str;
                sq_tostring(v, idx);
                sq_getstring(v, -1, &str);
                value = wxString::FromUTF8(str);
                sq_pop(v, 1);
            }
        }
    }
    static void push(HSQUIRRELVM v, const wxString& value) {
        ClassType<wxString>::PushInstanceCopy(v, value);
    }
};

template<>
struct Var<const wxString&> {

    wxString value;

    Var(HSQUIRRELVM v, SQInteger idx) {
        if (!Sqrat::Error::Occurred(v)) {
            wxString* ptr = ClassType<wxString>::GetInstance(v, idx);
            if (ptr != NULL) {
                value = *ptr;
            } else {
                Sqrat::Error::Clear(v);
                const SQChar* str;
                sq_tostring(v, idx);
                sq_getstring(v, -1, &str);
                value = wxString::FromUTF8(str);
                sq_pop(v, 1);
            }
        }
    }

    static void push(HSQUIRRELVM v, const wxString& value) {
        ClassType<wxString>::PushInstanceCopy(v, value);
    }
};


template<>
struct Var<wxString*> {
    wxString* value;
    Var(HSQUIRRELVM v, SQInteger idx){
        if (!Sqrat::Error::Occurred(v)) {
            value = ClassType<wxString>::GetInstance(v, idx);
            if (value != NULL) {
                return;
            } else {
                Sqrat::Error::Clear(v);
                Sqrat::Error::Throw(v,"Tried to get an wxString* from a string");
            }
        }
    }
    static void push(HSQUIRRELVM v, wxString* value) {
        ClassType<wxString>::PushInstanceCopy(v, *value);
    }
};


/*
template<class T>
struct Var<T* const> {
    T* const value;
    Var(HSQUIRRELVM vm, SQInteger idx) : value(ClassType<T>::GetInstance(vm, idx)) {
    }
    static void push(HSQUIRRELVM vm, T* const value) {
        ClassType<T>::PushInstance(vm, value);
    }
};

template<class T>
struct Var<const T*> {
    const T* value;
    Var(HSQUIRRELVM vm, SQInteger idx) : value(ClassType<T>::GetInstance(vm, idx)) {
    }
    static void push(HSQUIRRELVM vm, const T* value) {
        ClassType<T>::PushInstance(vm, const_cast<T*>(value));
    }
};

template<class T>
struct Var<const T* const> {
    const T* const value;
    Var(HSQUIRRELVM vm, SQInteger idx) : value(ClassType<T>::GetInstance(vm, idx)) {
    }
    static void push(HSQUIRRELVM vm, const T* const value) {
        ClassType<T>::PushInstance(vm, const_cast<T*>(value));
    }
};*/

}

#endif // SQ_WX_TYPE_HANDLER_H_INCLUDED
