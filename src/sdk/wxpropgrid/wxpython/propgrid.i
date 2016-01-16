/////////////////////////////////////////////////////////////////////////////
// Name:        propgrid.i
// Purpose:     Wrappers for the wxPropertyGrid.
//
// Author:      Jaakko Salli
//
// Created:     17-Feb-2005
// RCS-ID:      $Id:
// Copyright:   (c) 2005 by Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"The `PropertyGrid` provides a specialized two-column grid for editing
properties such as strings, numbers, colours, and string lists."
%enddef

%module(package="wx", docstring=DOCSTRING) propgrid
//%module(directors="1") propgrid

%import windows.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

%ignore DECLARE_DYNAMIC_CLASS;
%ignore DECLARE_ABSTRACT_CLASS;
%ignore WX_PG_DECLARE_PROPERTY_CLASS;
%ignore WX_PG_DECLARE_EVENT_METHODS;
%ignore WX_PG_DECLARE_PARENTAL_METHODS;
%ignore WX_PG_DECLARE_BASIC_TYPE_METHODS;
%ignore WX_PG_DECLARE_ATTRIBUTE_METHODS;
%ignore WX_PG_DECLARE_CUSTOM_PAINT_METHODS;
%ignore WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR_WITH_DECL;
%ignore wxPG_DECLARE_CREATECONTROLS;

#define DECLARE_DYNAMIC_CLASS(foo);
#define DECLARE_ABSTRACT_CLASS(foo);
#define WX_PG_DECLARE_PROPERTY_CLASS(foo)

%{

#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/props.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/editors.h>
#include <wx/propgrid/extras.h>

#if !defined(__WXMSW__) && !defined(OutputDebugString)
  #define OutputDebugString(A)
#endif

// Change to '#if 0' inorder to remove debug messages from build
#if 1
  #define MySWIGOutputDebugString OutputDebugString
#else
  #define MySWIGOutputDebugString(A)
#endif

#ifndef Py_RETURN_NONE
    #define Py_RETURN_NONE return Py_INCREF(Py_None), Py_None
#endif

wxPGProperty* NewPropertyCategory( const wxString& label, const wxString& name )
{
    return new wxPropertyCategory( label, name );
}

wxPGProperty* NewStringProperty( const wxString& label, const wxString& name, const wxString& value )
{
    return new wxStringProperty( label, name, value );
}

wxPGProperty* NewIntProperty( const wxString& label, const wxString& name, long value )
{
    return new wxIntProperty( label, name, value );
}

wxPGProperty* NewUIntProperty( const wxString& label, const wxString& name, unsigned long value )
{
    return new wxUIntProperty( label, name, value );
}

/*wxPGProperty* NewIntProperty( const wxString& label, const wxString& name, wxInt64 value )
{
    return new wxIntProperty( label, name, wxLongLong(value) );
}

wxPGProperty* NewUIntProperty( const wxString& label, const wxString& name, wxUint64 value )
{
    return new wxUIntProperty( label, name, wxULongLong(value) );
}*/

wxPGProperty* NewFloatProperty( const wxString& label, const wxString& name, double value )
{
    return new wxFloatProperty( label, name, value );
}

wxPGProperty* NewBoolProperty( const wxString& label, const wxString& name, bool value )
{
    return new wxBoolProperty( label, name, value );
}

wxPGProperty* NewEnumProperty( const wxString& label, const wxString& name,
        const wxArrayString& labels, const wxArrayInt& values,
        int value )
{
    return new wxEnumProperty( label, name, labels, values, value );
}

wxPGProperty* NewEditEnumProperty( const wxString& label, const wxString& name,
        const wxArrayString& labels, const wxArrayInt& values,
        const wxString& value )
{
    return new wxEditEnumProperty( label, name, labels, values, value );
}

wxPGProperty* NewFlagsProperty( const wxString& label, const wxString& name,
        const wxArrayString& labels, const wxArrayInt& values,
        int value )
{
    return new wxFlagsProperty( label, name, labels, values, value );
}

wxPGProperty* NewFileProperty( const wxString& label, const wxString& name, const wxString& value )
{
    return new wxFileProperty( label, name, value );
}

wxPGProperty* NewLongStringProperty( const wxString& label, const wxString& name, const wxString& value )
{
    return new wxLongStringProperty( label, name, value );
}

wxPGProperty* NewDirProperty( const wxString& label, const wxString& name, const wxString& value )
{
    return new wxDirProperty( label, name, value );
}

wxPGProperty* NewArrayStringProperty( const wxString& label, const wxString& name, const wxArrayString& value )
{
    return new wxArrayStringProperty( label, name, value );
}

wxPGProperty* NewFontProperty( const wxString& label, const wxString& name, const wxFont& value )
{
    return new wxFontProperty( label, name, value );
}

wxPGProperty* NewSystemColourProperty( const wxString& label,
                            const wxString& name,
                            const wxColourPropertyValue& value )
{
    return new wxSystemColourProperty( label, name, value );
}

wxPGProperty* NewColourProperty( const wxString& label,
                            const wxString& name,
                            const wxColour& value )
{
    return new wxColourProperty( label, name, value );
}

wxPGProperty* NewCursorProperty( const wxString& label,
                      const wxString& name,
                      int value )
{
    return new wxCursorProperty( label, name, value );
}

wxPGProperty* NewImageFileProperty( const wxString& label,
                         const wxString& name,
                         const wxString& value )
{
    return new wxImageFileProperty( label, name, value );
}

wxPGProperty* NewMultiChoiceProperty( const wxString& label,
                           const wxString& name,
                           const wxArrayString& choices,
                           const wxArrayString& value )
{
    return new wxMultiChoiceProperty( label, name, choices, value );
}

wxPGProperty* NewDateProperty( const wxString& label,
                    const wxString& name,
                    const wxDateTime& value )
{
    return new wxDateProperty( label, name, value );
}

wxPGProperty* NewFontDataProperty( const wxString& label,
                    const wxString& name,
                    const wxFontData& value )
{
    return new wxFontDataProperty( label, name, value );
}

wxPGProperty* NewSizeProperty( const wxString& label, const wxString& name,
                    const wxSize& value )
{
    return new wxSizeProperty( label, name, value );
}

wxPGProperty* NewPointProperty( const wxString& label, const wxString& name,
                 const wxPoint& value )
{
    return new wxPointProperty( label, name, value );
}

wxPGProperty* NewDirsProperty( const wxString& label, const wxString& name, const wxArrayString& value )
{
    return new wxDirsProperty( label, name, value );
}

wxPGProperty* NewArrayDoubleProperty( const wxString& label,
                           const wxString& name,
                           const wxArrayDouble& value )
{
    return new wxArrayDoubleProperty( label, name, value );
}


void RegisterEditor( wxPGEditor* editor, const wxString& name )
{
    wxPropertyGrid::RegisterEditorClass(editor, name);
}


#include <datetime.h>

bool PyObject_to_wxVariant( PyObject* input, wxVariant* v )
{
    PyDateTime_IMPORT;

    if ( input == Py_None )
    {
        v->MakeNull();
        return true;
    }
    else if ( PyBool_Check(input) )
    {
        *v = (bool) PyInt_AsLong(input);
        return true;
    }
    else if ( PyInt_Check(input) )
    {
        *v = (long) PyInt_AsLong(input);
        return true;
    }
    else if ( PyString_Check(input) || PyUnicode_Check(input) )
    {
        wxString* sptr = wxString_in_helper(input);
        if (sptr == NULL) return false;
        *v = *sptr;
        delete sptr;
        return true;
    }
    else if ( PyFloat_Check(input) )
    {
        *v = PyFloat_AsDouble(input);
        return true;
    }
    else if ( PyDate_Check(input) )
    {
        // Both date and datetime have these
        int year = PyDateTime_GET_YEAR(input);
        // Month is enumeration, make sure to match its first entry
        int month = PyDateTime_GET_MONTH(input) - 1 + (int) wxDateTime::Jan;
        int day = PyDateTime_GET_DAY(input);

        // Only datetime.datetime has the following
        int hour = 0;
        int minute = 0;
        int second = 0;
        int microsecond = 0;
        if ( PyDateTime_Check(input) )
        {
            hour = PyDateTime_DATE_GET_HOUR(input);
            minute = PyDateTime_DATE_GET_MINUTE(input);
            second = PyDateTime_DATE_GET_SECOND(input);
            microsecond = PyDateTime_DATE_GET_MICROSECOND(input);
        }

        wxDateTime wx_dateTime(day, (wxDateTime::Month)month, year,
                               hour, minute, second,
                               microsecond/1000);  // wx uses milliseconds
        *v = wx_dateTime;
        return true;
    }
    else if ( PyTuple_CheckExact(input) || PyList_CheckExact(input) )
    {
        int len = PySequence_Length(input);

        if ( len )
        {
            int i;
            PyObject* item = PySequence_GetItem(input, 0);
            bool failed = false;
            if ( PyString_CheckExact(item) || PyUnicode_CheckExact(item) )
            {
                wxArrayString arr;
                for (i=0; i<len; i++)
                {
                    item = PySequence_GetItem(input, i);
                    wxString* s = wxString_in_helper(item);
                    if ( PyErr_Occurred() )
                    {
                        delete s;
                        failed = true;
                        break;
                    }
                    arr.Add(*s);
                    delete s;
                    Py_DECREF(item);
                }

                if ( !failed )
                {
                    *v = arr;
                    return true;
                }
            }
            else if ( PyInt_CheckExact(item) || PyLong_CheckExact(item) )
            {
                wxArrayInt arr;
                for (i=0; i<len; i++)
                {
                    item = PySequence_GetItem(input, i);
                    long val;
                    if ( PyInt_CheckExact(item) )
                    {
                        val = PyInt_AS_LONG(item);
                    }
                    else if ( PyLong_CheckExact(item) )
                    {
                        val = PyLong_AsLong(item);
                    }
                    else
                    {
                        failed = true;
                        break;
                    }
                    arr.Add(val);
                    Py_DECREF(item);
                }

                if ( !failed )
                {
                    *v = WXVARIANT(arr);
                    return true;
                }
            }
        }
        else
        {
            *v = wxArrayString();
            return true;
        }
    }
    else if ( wxPySwigInstance_Check(input) )
    {
        // First try if it is a wxColour
        wxColour* col_ptr;
        if ( wxPyConvertSwigPtr(input, (void **)&col_ptr, wxT("wxColour")))
        {
            *v << *col_ptr;
            return true;
        }

        // Then wxPoint
        wxPoint* pt_ptr;
        if ( wxPyConvertSwigPtr(input, (void **)&pt_ptr, wxT("wxPoint")))
        {
            *v << *pt_ptr;
            return true;
        }

        // Then wxSize
        wxSize* sz_ptr;
        if ( wxPyConvertSwigPtr(input, (void **)&sz_ptr, wxT("wxSize")))
        {
            *v << *sz_ptr;
            return true;
        }

        // Then wxFont
        wxFont* font_ptr;
        if ( wxPyConvertSwigPtr(input, (void **)&font_ptr, wxT("wxFont")))
        {
            *v << *font_ptr;
            return true;
        }

        // Then wxColourPropertyValue
        wxColourPropertyValue* cpv_ptr;
        if ( wxPyConvertSwigPtr(input, (void **)&cpv_ptr,
                                wxT("wxColourPropertyValue")))
        {
            *v << *cpv_ptr;
            return true;
        }
    }

    //Py_TrackObject(input);
    // Last ditch - let's convert it to a wxVariant containing an arbitrary
    // PyObject
    wxVariant tempVariant = PyObjectToVariant(input);
    wxVariantData* vd = tempVariant.GetData();
    vd->IncRef();
    v->SetData(vd);

    return true;
}

PyObject* wxVariant_to_PyObject( const wxVariant* v )
{
    if ( !v || v->IsNull() )
        Py_RETURN_NONE;

    wxString variantType = v->GetType();
    //OutputDebugString(variantType.c_str());
    if ( variantType == wxT("long") )
    {
        return PyInt_FromLong(v->GetLong());
    }
    else if ( variantType == wxT("string") )
    {
        wxString _wxvar_str = v->GetString();
#if wxUSE_UNICODE
        return PyUnicode_FromWideChar(_wxvar_str.c_str(), _wxvar_str.Len());
#else
        return PyString_FromStringAndSize(_wxvar_str.c_str(), _wxvar_str.Len());
#endif
    }
    else if ( variantType == wxT("double") )
    {
        return PyFloat_FromDouble(v->GetDouble());
    }
    else if ( variantType == wxT("bool") )
    {
        return PyBool_FromLong((long)v->GetBool());
    }
    else if ( variantType == wxT("arrstring") )
    {
        wxArrayString arr = v->GetArrayString();
        PyObject* list = PyList_New(arr.GetCount());
        unsigned int i;

        for ( i=0; i<arr.GetCount(); i++ )
        {
            const wxString& str = arr.Item(i);
    #if wxUSE_UNICODE
            PyObject* item = PyUnicode_FromWideChar
            (str.c_str(), str.Len());
    #else
            PyObject* item = PyString_FromStringAndSize(str.c_str(), str.Len());
    #endif

            // PyList_SetItem steals reference, so absence of Py_DECREF is ok
            PyList_SetItem(list, i, item);
        }

        return list;
    }
    else if ( variantType == wxT("wxArrayInt") )
    {
        const wxArrayInt& arr = wxArrayIntFromVariant(*v);
        PyObject* list = PyList_New(arr.GetCount());
        unsigned int i;

        for ( i=0; i<arr.GetCount(); i++ )
        {
            PyObject* item = PyInt_FromLong((long)arr.Item(i));
            // PyList_SetItem steals reference, so absence of Py_DECREF is ok
            PyList_SetItem(list, i, item);
        }

        return list;
    }
    else if ( variantType == wxT("datetime") )
    {
        wxDateTime dt = v->GetDateTime();
        int year = dt.GetYear();
        // Month is enumeration, make sure to match its first entry
        int month = dt.GetMonth() + 1 - (int) wxDateTime::Jan;
        int day = dt.GetDay();
        int hour = dt.GetHour();
        int minute = dt.GetMinute();
        int second = dt.GetSecond();
        int millisecond = dt.GetMillisecond();
        return PyDateTime_FromDateAndTime(year, month, day,
                                          hour, minute, second,
                                          millisecond*1000);
    }
    else if ( variantType == wxT("wxColour") )
    {
        wxColour col;
        col << *v;
        return SWIG_NewPointerObj(SWIG_as_voidptr(new wxColour(col)),
                                  SWIGTYPE_p_wxColour,
                                  SWIG_POINTER_OWN | 0 );
    }
    else if ( variantType == wxT("wxPoint") )
    {
        const wxPoint& point = wxPointFromVariant(*v);
        return SWIG_NewPointerObj(SWIG_as_voidptr(new wxPoint(point)),
                                  SWIGTYPE_p_wxPoint,
                                  SWIG_POINTER_OWN | 0 );
    }
    else if ( variantType == wxT("wxSize") )
    {
        const wxSize& size = wxSizeFromVariant(*v);
        return SWIG_NewPointerObj(SWIG_as_voidptr(new wxSize(size)),
                                  SWIGTYPE_p_wxSize,
                                  SWIG_POINTER_OWN | 0 );
    }
    else if ( variantType == wxT("PyObject*") )
    {
        // PyObjectFromVariant already increments the reference count
        PyObject* o = PyObjectFromVariant(*v);
        //Py_TrackObject(o);
        if ( !o )
            Py_RETURN_NONE;
        return o;
    }
    else if ( variantType == wxT("wxFont") )
    {
        wxFont font;
        font << *v;
        return SWIG_NewPointerObj(SWIG_as_voidptr(new wxFont(font)),
                                  SWIGTYPE_p_wxFont,
                                  SWIG_POINTER_OWN | 0 );
    }
    else if ( variantType == wxT("wxColourPropertyValue") )
    {
        wxColourPropertyValue cpv;
        cpv << *v;
        return SWIG_NewPointerObj(
            SWIG_as_voidptr(new wxColourPropertyValue(cpv)),
            SWIGTYPE_p_wxColourPropertyValue,
            SWIG_POINTER_OWN | 0 );
    }
    else if ( variantType.StartsWith(wxT("wx")) )
    {
        // As a last desperate measure, go ahead and assume it is
        // wxObject ptr (such as wxFontData), if the variant type name
        // begins with 'wx'. Note that this should still correctly
        // return NULL if variant is really not wxObject ptr.
        wxObject* result;
        if ( wxPGVariantToWxObjectPtr(*v, &result) )
        {
            return wxPyMake_wxObject(result, (bool)0);
        }
    }

    return NULL;
}

PyObject* wxPGVariantAndBool_to_PyObject( const wxPGVariantAndBool& vab )
{
    PyObject* tuple = PyTuple_New(2);

    PyObject* value;
    if ( vab.m_valueValid )
    {
        value = wxVariant_to_PyObject(&vab.m_value);
    }
    else
    {
        Py_INCREF(Py_None);
        value = Py_None;
    }

    PyTuple_SetItem(tuple, 0, PyInt_FromLong((long)vab.m_result));
    PyTuple_SetItem(tuple, 1, value);

    return tuple;
}

bool PyObject_to_wxPGVariantAndBool( PyObject* input,
                                     wxPGVariantAndBool& vab )
{
    PyObject* resObj = NULL;
    PyObject* valueObj = NULL;

    if ( PySequence_Check(input) && PySequence_Length(input) == 2 )
    {
        resObj = PySequence_GetItem(input, 0);
        if (PyErr_Occurred()) return false;
        valueObj = PySequence_GetItem(input, 1);
        if (PyErr_Occurred()) return false;
    }
    else
    {
        resObj = input;
    }

    // Also checks for bool, which is subclass of int
    if ( PyInt_Check(resObj) )
    {
        vab.Init((bool) PyInt_AsLong(resObj));
    }
    else if ( PyLong_Check(resObj) )
    {
        vab.Init((bool) PyLong_AsLong(resObj));
    }
    else
    {
        return false;
    }

    if ( valueObj )
    {
        // If valueObj is valid, then we can assume resObj was acquired from
        // sequence and must be decref'ed.
        Py_DECREF(resObj);

        if ( PyObject_to_wxVariant(valueObj, &vab.m_value) )
            vab.m_valueValid = true;
        else
            return false;

        Py_DECREF(valueObj);
    }

    return true;
}

bool PyObject_to_wxPGPropArgCls( PyObject* input, wxPGPropArgCls** v )
{
    if ( PyString_Check(input) || PyUnicode_Check(input) )
    {
        wxString* sptr = wxString_in_helper(input);
        if (sptr == NULL) return false;
        *v = new wxPGPropArgCls(sptr, true);
    }
    else if ( input == Py_None )
    {
        *v = new wxPGPropArgCls(NULL);
    }
    else
    {
        void* valp;
        int res = SWIG_ConvertPtr(input, &valp, SWIGTYPE_p_wxPGProperty,  0  | 0);
        if ( !SWIG_IsOK(res) ) return false;
        *v = new wxPGPropArgCls(reinterpret_cast< wxPGProperty * >(valp));
    }

    return true;
}

PyObject* wxPGPropArgCls_to_PyObject( const wxPGPropArgCls& v )
{
    if ( v.HasName() )
    {
        const wxString& _wxvar_str = v.GetName();
#if wxUSE_UNICODE
        return PyUnicode_FromWideChar(_wxvar_str.c_str(), _wxvar_str.Len());
#else
        return PyString_FromStringAndSize(_wxvar_str.c_str(), _wxvar_str.Len());
#endif
    }

    wxPGProperty* p = v.GetPtr();

    if ( !p )
        Py_RETURN_NONE;

    return SWIG_NewPointerObj(SWIG_as_voidptr(p), SWIGTYPE_p_wxPGProperty, 0 |  0 );
}

PyObject* wxPGAttributeStorage_to_PyObject( const wxPGAttributeStorage* attrs )
{
    wxPGAttributeStorage::const_iterator it = attrs->StartIteration();
    wxVariant v;

    PyObject* dict = PyDict_New();
    if ( !dict ) return dict;

    while ( attrs->GetNext( it, v ) )
    {
        const wxString& name = v.GetName();
#if wxUSE_UNICODE
        PyObject* pyStr = PyUnicode_FromWideChar(name.c_str(), name.Len());
#else
        PyObject* pyStr = PyString_FromStringAndSize(name.c_str(), name.Len());
#endif
        PyObject* pyVal = wxVariant_to_PyObject(&v);
        int res = PyDict_SetItem( dict, pyStr, pyVal );
    }

    return dict;
}

PyObject* wxPoint_to_PyObject( const wxPoint* p )
{
    if ( p->x == -1 || p->y == -1 )
        Py_RETURN_NONE;

    PyObject* tuple = PyTuple_New(2);
    // PyTuple_SetItem steals reference, so absence of Py_DECREF is ok
    PyTuple_SetItem(tuple, 0, PyInt_FromLong(p->x));
    PyTuple_SetItem(tuple, 1, PyInt_FromLong(p->y));
    return tuple;
}

PyObject* wxSize_to_PyObject( const wxSize* p )
{
    if ( p->x == -1 || p->y == -1 )
        Py_RETURN_NONE;

    PyObject* tuple = PyTuple_New(2);
    // PyTuple_SetItem steals reference, so absence of Py_DECREF is ok
    PyTuple_SetItem(tuple, 0, PyInt_FromLong(p->x));
    PyTuple_SetItem(tuple, 1, PyInt_FromLong(p->y));
    return tuple;
}

PyObject* wxPGWindowList_to_PyObject( const wxPGWindowList* p )
{
    PyObject* o1 = NULL;
    PyObject* o2 = NULL;

    if ( p->m_primary )
        o1 = wxPyMake_wxObject(p->m_primary, false);

    if ( p->m_secondary )
        o2 = wxPyMake_wxObject(p->m_secondary, false);

    if ( o1 )
    {
        if ( o2 )
        {
            PyObject* tuple = PyTuple_New(2);
            // PyTuple_SetItem steals reference, so absence of Py_DECREF is ok
            PyTuple_SetItem(tuple, 0, o1);
            PyTuple_SetItem(tuple, 1, o2);
            return tuple;
        }
        else
        {
            return o1;
        }
    }

    Py_RETURN_NONE;
}

bool PyObject_to_wxPGWindowList( PyObject* o, wxPGWindowList* p )
{
    if ( PySequence_Check(o) )
    {
        if ( PySequence_Size(o) != 2 )
            return false;

        bool res;

        PyObject* m1 = PySequence_GetItem(o, 0);
        res = wxPyConvertSwigPtr(m1, (void **)&p->m_primary, wxT("wxWindow"));
        Py_DECREF(m1);
        if ( !res )
            return false;

        PyObject* m2 = PySequence_GetItem(o, 1);
        res = wxPyConvertSwigPtr(m2, (void **)&p->m_secondary, wxT("wxWindow"));
        Py_DECREF(m2);
        if ( !res )
            return false;

        return true;
    }

    p->m_secondary = NULL;

    if ( !wxPyConvertSwigPtr(o, (void **)&p->m_primary, wxT("wxWindow")) )
        return false;

    return true;
}

// Returns allocated string that must be freed by the caller
char* GeneratePythonConvErrMsg(PyObject* obj,
                               const char* toType)
{
    PyObject* pyReprStr = PyObject_Str(obj);
    char* reprStr = PyString_AsString(pyReprStr);
    const char* templ = "'%s' cannot be converted to %s";
    char* s = (char*) malloc(strlen(templ) +
                             strlen(reprStr) +
                             strlen(toType) + 1);
    sprintf(s, templ, reprStr, toType);
    Py_DECREF(pyReprStr);
    return s;
}

%}

// Suppress warning 511 (kwargs not supported for overloaded functions)
#pragma SWIG nowarn=511

//%feature("director") wxPGProperty;
//%feature("director") wxPGEditor;

//
// If these are not used by the wxPython DLLs, we're in trouble.
%define wxUSE_VALIDATORS        1
%enddef
%define wxUSE_DATETIME          1
%enddef
%define wxUSE_DATEPICKCTRL      1
%enddef
%define wxUSE_SPINCTRL          1
%enddef
%define wxUSE_TOOLTIPS          1
%enddef

%ignore wxPropertyGridHitTestResult::property;
%ignore wxPropertyGridHitTestResult::splitter;
%ignore wxPropertyGridHitTestResult::column;
%ignore wxPropertyGridHitTestResult::splitterHitOffset;

%extend wxPropertyGridHitTestResult {
    %pythoncode {
        column = property(GetColumn)
        splitter = property(GetSplitter)
        splitterHitOffset = property(GetSplitterHitOffset)
        # Following must be last, I think
        property = property(GetProperty)
    }
}

%extend wxPGMultiButton {
    %pythonAppend wxPGMultiButton
    {
        self._setOORInfo(self)
    }

    void AddBitmapButton( const wxBitmap& bitmap, int id = -2 )
    {
        return self->Add(bitmap, id);
    }
    %pythoncode {
        def AddButton(self, *args, **kwargs):
            return self.Add(*args, **kwargs)
    }
}

// wxPropertyGrid specific special code
%{
// We need these proxies or SWIG will fail (it has somewhat incomplete
// C++ syntax support, it seems).
static wxString& wxString_wxPG_LABEL = *((wxString*)NULL);
static wxArrayInt& wxArrayInt_wxPG_EMPTY = *((wxArrayInt*)NULL);
static wxArrayString& wxArrayString_wxPG_EMPTY = *((wxArrayString*)NULL);
static wxColourPropertyValue& wxCPV_wxPG_EMPTY = *((wxColourPropertyValue*)NULL);
#define wxFONT_wxPG_NORMAL_FONT *wxNORMAL_FONT
#define wxColour_BLACK          *wxBLACK
#define wxBitmap_NULL           wxNullBitmap
%}

//---------------------------------------------------------------------------

%{
const wxChar* wxPyPropertyGridNameStr = wxPropertyGridNameStr;
const wxChar* wxPyPropertyGridManagerNameStr = wxPropertyGridManagerNameStr;
const wxChar* PropertyGridNameStr = wxPropertyGridNameStr;
%}

%pythoncode {
PropertyGridNameStr = "wxPropertyGrid"
PropertyGridManagerNameStr = "wxPropertyGridManager"
}


%module outarg

// This tells SWIG to treat an double * argument with name 'OutValue' as
// an output value.  We'll append the value to the current result which
// is guaranteed to be a List object by SWIG.

%typemap(argout) wxWindow ** {
    PyObject *o;
    o = wxPyConstructObject((void*)*$1, wxT("wxWindow"), 0);
    PyObject* _tuple = PyTuple_New(2);
    // PyTuple_SetItem steals reference, so absence of Py_DECREF is ok
    PyTuple_SetItem(_tuple, 0, $result);
    PyTuple_SetItem(_tuple, 1, o);
    $result = _tuple;
}


// We'll need some new 'out' types for our custom directors
//%typemap(out) wxRect& { $result = wxPyConstructObject((void*)$1, wxT("wxRect"), 0); }
//py_rect = SWIG_NewPointerObj((void*)&rect, SWIGTYPE_p_wxRect, 0);
%typemap(out) wxPoint& { $result = wxPoint_to_PyObject($1); }
%typemap(out) wxSize& { $result = wxSize_to_PyObject($1); }
%typemap(out) const wxChar* {
%#if wxUSE_UNICODE
    $result = PyUnicode_FromWideChar($1, wxStrlen($1));
%#else
    $result = PyString_FromStringAndSize($1, wxStrlen($1));
%#endif
}

//
// wxVariant typemap
//
%typemap(in) wxVariant {
    if ( !PyObject_to_wxVariant($input, &$1) )
    {
        char* s = GeneratePythonConvErrMsg($input, "wxVariant");
        PyErr_SetString(PyExc_TypeError, s);
        free(s);
        SWIG_fail;
    }
}

%typemap(in) wxVariant& {
    if ( $1 )
        SWIG_fail;
    $1 = new wxVariant();
    if ( !PyObject_to_wxVariant($input, $1) )
    {
        char* s = GeneratePythonConvErrMsg($input, "wxVariant");
        PyErr_SetString(PyExc_TypeError, s);
        free(s);
        SWIG_fail;
    }
}

%typemap(freearg) wxVariant& {
    delete $1;
}

%typemap(in) const wxVariant& {
    if ( $1 )
        SWIG_fail;
    $1 = new wxVariant();
    if ( !PyObject_to_wxVariant($input, $1) )
    {
        char* s = GeneratePythonConvErrMsg($input, "wxVariant");
        PyErr_SetString(PyExc_TypeError, s);
        free(s);
        SWIG_fail;
    }
}

%typemap(freearg) const wxVariant& {
    delete $1;
}

%typemap(out) wxVariant {
    $result = wxVariant_to_PyObject(&$1);
    if ( !$result ) {
        PyErr_SetString(PyExc_TypeError,
            "this wxVariant type cannot be converted to Python object");
        SWIG_fail;
    }
}

%typemap(out) wxVariant& {
    $result = wxVariant_to_PyObject($1);
    if ( !$result ) {
        PyErr_SetString(PyExc_TypeError,
            "this wxVariant type cannot be converted to Python object");
        SWIG_fail;
    }
}

%typemap(out) const wxVariant& {
    $result = wxVariant_to_PyObject($1);
    if ( !$result ) {
        PyErr_SetString(PyExc_TypeError,
            "this wxVariant type cannot be converted to Python object");
        SWIG_fail;
    }
}

%typecheck(SWIG_TYPECHECK_POINTER) wxVariant
{
    // We'll try to convert any Python object into wxVariant
    $1 = 1;
}

%typemap(in) const wxPGPropArgCls& {
    if ( !PyObject_to_wxPGPropArgCls($input, &$1) ) {
        char* s = GeneratePythonConvErrMsg($input, "wxPGPropArgCls");
        PyErr_SetString(PyExc_TypeError, s);
        SWIG_fail;
    }
}

%typemap(freearg) const wxPGPropArgCls& {
    delete $1;
}

%typemap(out) wxPGPropArgCls {
    $result = wxPGPropArgCls_to_PyObject($1);
}

%typemap(out) wxPGAttributeStorage {
    $result = wxPGAttributeStorage_to_PyObject($1);
}

%typemap(out) wxPGAttributeStorage& {
    $result = wxPGAttributeStorage_to_PyObject($1);
}

%typemap(out) const wxPGAttributeStorage& {
    $result = wxPGAttributeStorage_to_PyObject($1);
}

//
// wxPGVariantAndBool typemaps
//
%typemap(in) wxPGVariantAndBool {
    if ( !PyObject_to_wxPGVariantAndBool($input, $1) )
    {
        char* s = GeneratePythonConvErrMsg($input, "wxPGVariantAndBool");
        PyErr_SetString(PyExc_TypeError, s);
        SWIG_fail;
    }
}

%typemap(out) wxPGVariantAndBool {
    $result = wxPGVariantAndBool_to_PyObject($1);
}

//
// wxPGWindowList typemap (used by wxPGEditor::CreateControls)
//
%typemap(in) wxPGWindowList {
    if ( !PyObject_to_wxPGWindowList($input, &$1) ) {
        PyErr_SetString(PyExc_TypeError,
                        "expected wxWindow or tuple of two wxWindows");
        SWIG_fail;
    }
}

%typemap(out) wxPGWindowList {
    $result = wxPGWindowList_to_PyObject(&$1);
}

//
// This macro creates typemaps for arrays of arbitrary C++ objects. Array
// class must support basic std::vector API.
//
%define PG_MAKE_ARRAY_TYPEMAPS(ITEM_CLASS, ARR_CLASS)
    %{
    PyObject* ARR_CLASS ## ToPyObject(const ARR_CLASS* arr)
    {
        PyObject* pyArr = PyList_New(arr->size());
        for ( unsigned int i=0; i< (unsigned int) arr->size(); i++ )
        {
            PyObject* pyItem = SWIG_NewPointerObj((void*)(*arr)[i],
                                                  SWIGTYPE_p_ ## ITEM_CLASS,
                                                  0);
            if ( !pyItem )
                return NULL;
            PyList_SetItem(pyArr, i, pyItem);
        }
        return pyArr;
    }
    bool PyObjectTo ## ARR_CLASS(PyObject* pyArr, ARR_CLASS* arr)
    {
        if (! PySequence_Check(pyArr)) {
            PyErr_SetString(PyExc_TypeError, "Sequence expected.");
            return false;
        }
        int i, len = PySequence_Length(pyArr);
        for ( i=0; i<len; i++ )
        {
            PyObject* item = PySequence_GetItem(pyArr, i);
            int res1;
            void* voidPtr;
            res1 = SWIG_ConvertPtr(item, &voidPtr,
                                   SWIGTYPE_p_ ## ITEM_CLASS, 0 |  0 );
            if ( !SWIG_IsOK(res1) ) return false;
            ITEM_CLASS* itemPtr = reinterpret_cast<ITEM_CLASS*>(voidPtr);
            if ( PyErr_Occurred() ) return false;
            arr->push_back(itemPtr);
            Py_DECREF(item);
        }
        return true;
    }
    %}

    %typemap(in) ARR_CLASS& (bool temp=false) {
        $1 = new ARR_CLASS();
        temp = true;
        if ( !PyObjectTo ## ARR_CLASS($input, $1) ) SWIG_fail;
    }

    %typemap(freearg) ARR_CLASS& {
        if (temp$argnum) delete $1;
    }

    %typemap(out) ARR_CLASS& {
        $result = ARR_CLASS ## ToPyObject($1);
        if ( !$result ) SWIG_fail;
    }

%enddef

PG_MAKE_ARRAY_TYPEMAPS(wxPGProperty, wxArrayPGProperty)


%include _propgrid_rename.i

MustHaveApp(wxPropertyGrid);

//---------------------------------------------------------------------------
// Get all our defs from the REAL header file.

%include propgrid.h
%include editors.h
%include props.h
%include advprops.h
%include manager.h

// Property constructor functions
wxPGProperty* NewPropertyCategory( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL );
wxPGProperty* NewStringProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString );
wxPGProperty* NewUIntProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, unsigned long value = 0 );
wxPGProperty* NewIntProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, long value = 0 );
//wxPGProperty* NewIntProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, wxInt64 value = wxLL(0) );
//wxPGProperty* NewUIntProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, wxUint64 value = wxLL(0) );
wxPGProperty* NewFloatProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, double value = 0.0 );
wxPGProperty* NewBoolProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, bool value = false );
wxPGProperty* NewEnumProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
        const wxArrayString& labels = wxArrayString(), const wxArrayInt& values = wxArrayInt(),
        int value = 0 );
wxPGProperty* NewEditEnumProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
        const wxArrayString& labels = wxArrayString(), const wxArrayInt& values = wxArrayInt(),
        const wxString& value = wxEmptyString );
wxPGProperty* NewFlagsProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
        const wxArrayString& labels = wxArrayString(), const wxArrayInt& values = wxArrayInt(),
        int value = 0 );
wxPGProperty* NewLongStringProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString );
wxPGProperty* NewFileProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString );
wxPGProperty* NewDirProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString );
wxPGProperty* NewArrayStringProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxArrayString& value = wxArrayString() );
wxPGProperty* NewFontProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxFont& value = wxFont() );
wxPGProperty* NewSystemColourProperty( const wxString& label = wxPG_LABEL,
                            const wxString& name = wxPG_LABEL,
                            const wxColourPropertyValue& value = wxColourPropertyValue() );
wxPGProperty* NewColourProperty( const wxString& label = wxPG_LABEL,
                            const wxString& name = wxPG_LABEL,
                            const wxColour& value = wxColour() );
wxPGProperty* NewCursorProperty( const wxString& label= wxPG_LABEL,
                      const wxString& name= wxPG_LABEL,
                      int value = 0 );
wxPGProperty* NewImageFileProperty( const wxString& label = wxPG_LABEL,
                         const wxString& name = wxPG_LABEL,
                         const wxString& value = wxEmptyString);
wxPGProperty* NewMultiChoiceProperty( const wxString& label,
                           const wxString& name = wxPG_LABEL,
                           const wxArrayString& choices = wxArrayString(),
                           const wxArrayString& value = wxArrayString() );
wxPGProperty* NewDateProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    const wxDateTime& value = wxDateTime() );
wxPGProperty* NewFontDataProperty( const wxString& label = wxPG_LABEL,
                    const wxString& name = wxPG_LABEL,
                    const wxFontData& value = wxFontData() );
wxPGProperty* NewSizeProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                    const wxSize& value = wxSize() );
wxPGProperty* NewPointProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                 const wxPoint& value = wxPoint() );
wxPGProperty* NewDirsProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxArrayString& value = wxArrayString() );
wxPGProperty* NewArrayDoubleProperty( const wxString& label = wxPG_LABEL,
                           const wxString& name = wxPG_LABEL,
                           const wxArrayDouble& value = wxArrayDouble() );


%include "propgrid_cbacks.i"

void RegisterEditor( wxPGEditor* editor, const wxString& name );

//---------------------------------------------------------------------------
// Python functions to act like the event macros

%pythoncode {
EVT_PG_CHANGED = wx.PyEventBinder( wxEVT_PG_CHANGED, 1 )
EVT_PG_CHANGING = wx.PyEventBinder( wxEVT_PG_CHANGING, 1 )
EVT_PG_SELECTED = wx.PyEventBinder( wxEVT_PG_SELECTED, 1 )
EVT_PG_HIGHLIGHTED = wx.PyEventBinder( wxEVT_PG_HIGHLIGHTED, 1 )
EVT_PG_RIGHT_CLICK = wx.PyEventBinder( wxEVT_PG_RIGHT_CLICK, 1 )
EVT_PG_PAGE_CHANGED = wx.PyEventBinder( wxEVT_PG_PAGE_CHANGED, 1 )
EVT_PG_DOUBLE_CLICK = wx.PyEventBinder( wxEVT_PG_DOUBLE_CLICK, 1 )

LABEL_AS_NAME = "_LABEL_AS_NAME"
DEFAULT_IMAGE_SIZE = (-1,-1)
NO_IMAGE_SIZE = (0,0)

PG_BOOL_USE_CHECKBOX = "UseCheckbox"
PG_BOOL_USE_DOUBLE_CLICK_CYCLING = "UseDClickCycling"
PG_FLOAT_PRECISION = "Precision"
PG_STRING_PASSWORD = "Password"
PG_UINT_BASE = "Base"
PG_UINT_PREFIX = "Prefix"
PG_FILE_WILDCARD = "Wildcard"
PG_FILE_SHOW_FULL_PATH = "ShowFullPath"
PG_FILE_SHOW_RELATIVE_PATH = "ShowRelativePath"
PG_FILE_INITIAL_PATH = "InitialPath"
PG_FILE_DIALOG_TITLE = "DialogTitle"
PG_DIR_DIALOG_MESSAGE = "DialogMessage"
PG_DATE_FORMAT = "DateFormat"
PG_DATE_PICKER_STYLE = "PickerStyle"

}

//---------------------------------------------------------------------------

%init %{
    wxPGInitResourceModule();
%}


//---------------------------------------------------------------------------

%include "propgrid_cbacks.cpp"

%pythoncode {

PropertyCategory = NewPropertyCategory
StringProperty = NewStringProperty
IntProperty = NewIntProperty
UIntProperty = NewUIntProperty
FloatProperty = NewFloatProperty
BoolProperty = NewBoolProperty
EnumProperty = NewEnumProperty
EditEnumProperty = NewEditEnumProperty
FlagsProperty = NewFlagsProperty
LongStringProperty = NewLongStringProperty
FileProperty = NewFileProperty
DirProperty = NewDirProperty
ArrayStringProperty = NewArrayStringProperty
FontProperty = NewFontProperty
SystemColourProperty = NewSystemColourProperty
ColourProperty = NewColourProperty
CursorProperty = NewCursorProperty
ImageFileProperty = NewImageFileProperty
MultiChoiceProperty = NewMultiChoiceProperty
DateProperty = NewDateProperty
FontDataProperty = NewFontDataProperty
SizeProperty = NewSizeProperty
PointProperty = NewPointProperty
DirsProperty = NewDirsProperty
ArrayDoubleProperty = NewArrayDoubleProperty

}
