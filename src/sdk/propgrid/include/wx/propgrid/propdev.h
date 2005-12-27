/////////////////////////////////////////////////////////////////////////////
// Name:        propdev.h
// Purpose:     wxPropertyGrid Internal/Property Developer Header
// Author:      Jaakko Salli
// Modified by:
// Created:     Nov-23-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_PROPDEV_H_
#define _WX_PROPGRID_PROPDEV_H_

// -----------------------------------------------------------------------

#ifdef _WX_PROPGRID_ADVPROPS_H_
# error "propdev.h must be included *before* advprops.h"
#endif

//#if wxPG_USE_CUSTOM_CONTROLS
//# include "wx/propertygrid/custctrl.h"
//#endif

// -----------------------------------------------------------------------

// wxWidgets < 2.5.4 compatibility.
// TODO: After officially included in library, remove this section.
#if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 3 )
# if wxUSE_UNICODE
    #include <string.h> //for mem funcs

    inline wxChar* wxTmemcpy(wxChar* szOut, const wxChar* szIn, size_t len)
    {
        return (wxChar*) memcpy(szOut, szIn, len * sizeof(wxChar));
    }
# else
    #define wxTmemcpy memcpy
# endif
# ifndef wxFIXED_MINSIZE
#  define wxFIXED_MINSIZE    0
# endif
#elif ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 4 )
    #define wxTmemcpy wxMemcpy
#endif

// -----------------------------------------------------------------------


// This is required for sharing common global variables.
// TODO: Automatic locking mechanism?
class WXDLLIMPEXP_PG wxPGGlobalVarsClass
{
public:

    wxPGGlobalVarsClass();
    ~wxPGGlobalVarsClass();

    wxString            m_pDefaultImageWildcard; // Used by advprops, but here to make things easier.

    wxArrayPtrVoid      m_arrEditorClasses; // List of editor class instances.

    wxPGHashMapS2P      m_dictValueType; // Valuename -> Value type object instance.

    wxPGHashMapS2P      m_dictPropertyClassInfo; // PropertyName -> ClassInfo

    // void* so we don't have to declare class in headers
    void*               m_dictConstants;

    wxString            m_boolChoices[2]; // default is ["False", "True"]

    wxPGConstants       m_emptyConstants;

    bool                m_autoGetTranslation; // If true then some things are automatically translated

    int                 m_offline; // > 0 if errors cannot or should not be shown in statusbar etc.
};

extern WXDLLIMPEXP_PG wxPGGlobalVarsClass* wxPGGlobalVars;

#define wxPGUnRefChoices(PCHOICES) \
    wxASSERT ( PCHOICES ); \
    if ( PCHOICES != &wxPGGlobalVars->m_emptyConstants && PCHOICES->UnRef() ) \
        delete PCHOICES

// -----------------------------------------------------------------------
// wxPGEditor class.

/** \class wxPGEditor
    \ingroup classes
    \brief Base for property editor classes.
    See propgrid.cpp for how builtin editors works (starting from wxPGTextCtrlEditor).
*/
class WXDLLIMPEXP_PG wxPGEditor
{
public:

    /** Destructor. */
    virtual ~wxPGEditor() = 0;

    /** Instantiates editor controls. */
    virtual wxPGCtrlClass* CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxPGCtrlClass** psecondary ) const = 0;

    /** Queries preferred value from property to the control. */
    virtual void UpdateControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const = 0;

    /** Used to draw the value when control is hidden. Default is to draw a string.
        Note that some margin above and below has been taken into account, to make
        drawing text easier without giving loads parameters. If Pen is changed,
        it must be returned to *wxTRANSPARENT_PEN.
    */
    virtual void DrawValue ( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const;

    /** Handles events. Returns TRUE if processed and value modified
        (same retval and args as in wxPGProperty::OnEvent).
    */
    virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property,
        wxPGCtrlClass* wnd_primary, wxEvent& event ) const = 0;

    /** Copies value from ctrl to property's internal storage.
        Returns TRUE if value was different.
    */
    virtual bool CopyValueFromControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const = 0;

    /** Sets value in control to unspecified. */
    virtual void SetValueToUnspecified ( wxPGCtrlClass* ctrl ) const = 0;

    /** Sets control's value specifically from string. */
    virtual void SetControlStringValue ( wxPGCtrlClass* ctrl, const wxString& txt ) const;

    /** Sets control's value specifically from int (applies to choice etc.). */
    virtual void SetControlIntValue ( wxPGCtrlClass* ctrl, int value ) const;

    /** Appends item to a existing control. Default implementation
        does nothing. Returns index of item added.
    */
    virtual int AppendItem ( wxPGCtrlClass* ctrl, const wxString& label ) const;

    /** Returns TRUE if control itself can contain the custom image. Default is
        to return FALSE.
    */
    virtual bool CanContainCustomImage () const;

protected:
};

#define WX_PG_IMPLEMENT_EDITOR_CLASS_CONSTFUNC(EDITOR,CLASSNAME,PARENTCLASS) \
wxPGEditor* wxPGEditor_##EDITOR = (wxPGEditor*) NULL; \
wxPGEditor* wxPGConstruct##EDITOR##EditorClass() \
{ \
    wxASSERT ( !wxPGEditor_##EDITOR ); \
    return new CLASSNAME(); \
}

#define WX_PG_IMPLEMENT_EDITOR_CLASS_STD_METHODS() \
virtual wxPGCtrlClass* CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property, \
    const wxPoint& pos, const wxSize& sz, wxPGCtrlClass** psecondary ) const; \
virtual void UpdateControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const; \
virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property, \
    wxPGCtrlClass* primary, wxEvent& event ) const; \
virtual bool CopyValueFromControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const; \
virtual void SetValueToUnspecified ( wxPGCtrlClass* ctrl ) const;

#define WX_PG_IMPLEMENT_EDITOR_CLASS(EDITOR,CLASSNAME,PARENTCLASS) \
class CLASSNAME; \
class CLASSNAME : public PARENTCLASS \
{ \
public: \
    virtual ~CLASSNAME(); \
    WX_PG_IMPLEMENT_EDITOR_CLASS_STD_METHODS() \
}; \
WX_PG_IMPLEMENT_EDITOR_CLASS_CONSTFUNC(EDITOR,CLASSNAME,PARENTCLASS)


// -----------------------------------------------------------------------
// Variant setter macros.
// TODO: When sure this is feature is out, remove these.

#if wxPG_EMBED_VARIANT
# define wxPG_SetVariantValue(VALUE) \
    ((wxVariant&)*this) = VALUE
# define wxPG_SetVariantWxObjectValue() \
    ((wxVariant&)*this) = wxVariant((wxObject*)DoGetValue().GetRawPtr(),m_name);
# define wxPG_SetVariantValueVoidPtr() \
    ((wxVariant&)*this) = wxPGVariantToVoidPtr(DoGetValue())
#else
# define wxPG_SetVariantValue(VALUE)            { }
# define wxPG_SetVariantWxObjectValue()         { }
# define wxPG_SetVariantValueVoidPtr()          { }
#endif

// -----------------------------------------------------------------------
// Value type registeration macros

// -----------------------------------------------------------------------
// Value type registeration macros

#define wxPGRegisterValueType(TYPENAME) \
    if ( wxPGValueType_##TYPENAME == (wxPGValueType*) NULL ) \
    { \
        wxPGValueType_##TYPENAME = wxPropertyGrid::RegisterValueType( wxPGNewVT##TYPENAME() ); \
    }

// Use this in RegisterDefaultValues.
#define wxPGRegisterDefaultValueType(TYPENAME) \
    if ( wxPGValueType_##TYPENAME == (wxPGValueType*) NULL ) \
    { \
        wxPGValueType_##TYPENAME = wxPropertyGrid::RegisterValueType( new wxPGValueType##TYPENAME##Class, true ); \
    }

#define wxPG_INIT_REQUIRED_TYPE(T) \
    wxPGRegisterValueType(T)

// Use this with 'simple' value types (derived)
#define wxPG_INIT_REQUIRED_TYPE2(T) \
    if ( wxPGValueType_##T == (wxPGValueType*) NULL ) \
    { \
        wxPGValueType_##T = wxPropertyGrid::RegisterValueType( new wxPGValueType##T##Class ); \
    }

// -----------------------------------------------------------------------
// Editor class registeration macros

#define wxPGRegisterEditorClass(EDITOR) \
    if ( wxPGEditor_##EDITOR == (wxPGEditor*) NULL ) \
    { \
        wxPGEditor_##EDITOR = wxPropertyGrid::RegisterEditorClass( wxPGConstruct##EDITOR##EditorClass() ); \
    }

// Use this in RegisterDefaultEditors.
#define wxPGRegisterDefaultEditorClass(EDITOR) \
if ( wxPGEditor_##EDITOR == (wxPGEditor*) NULL ) \
    { \
        wxPGEditor_##EDITOR = wxPropertyGrid::RegisterEditorClass( wxPGConstruct##EDITOR##EditorClass(), TRUE ); \
    }

#define wxPG_INIT_REQUIRED_EDITOR(T) \
    wxPGRegisterEditorClass(T)

// -----------------------------------------------------------------------

#define WX_PG_IMPLEMENT_SUBTYPE(VALUETYPE,CVALUETYPE,DEFPROPERTY,TYPESTRING,GETTER,DEFVAL) \
const wxPGValueType *wxPGValueType_##VALUETYPE = (wxPGValueType*) NULL; \
class wxPGValueType##VALUETYPE##Class : public wxPGValueType \
{ \
public: \
    virtual const wxChar* GetTypeName() const { return TYPESTRING; } \
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant(m_default); } \
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const \
    { return wxVariant ( value.GETTER(), name ); } \
    virtual wxPGProperty* GenerateProperty ( const wxString& label, const wxString& name ) const \
    { \
        return wxPG_CONSTFUNC(DEFPROPERTY)(label,name); \
    } \
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const \
    { \
        wxASSERT_MSG( wxStrcmp(GetTypeName(),value.GetType().c_str()) == 0, \
            wxT("SetValueFromVariant: wxVariant type mismatch.") ); \
        property->DoSetValue(value.GETTER()); \
    } \
    wxPGValueType##VALUETYPE##Class(); \
    virtual ~wxPGValueType##VALUETYPE##Class(); \
protected: \
    CVALUETYPE m_default; \
}; \
wxPGValueType##VALUETYPE##Class::wxPGValueType##VALUETYPE##Class() { m_default = DEFVAL; } \
wxPGValueType##VALUETYPE##Class::~wxPGValueType##VALUETYPE##Class() { }

#define WX_PG_IMPLEMENT_VALUE_TYPE(VALUETYPE,DEFPROPERTY,TYPESTRING,GETTER,DEFVAL) \
WX_PG_IMPLEMENT_SUBTYPE(VALUETYPE,VALUETYPE,DEFPROPERTY,TYPESTRING,GETTER,DEFVAL)

//
// Implements wxVariantData for the type.
//
#define WX_PG_IMPLEMENT_VALUE_TYPE_VDC(VDCLASS,VALUETYPE) \
IMPLEMENT_DYNAMIC_CLASS(VDCLASS,wxVariantData) \
VDCLASS::VDCLASS() { } \
VDCLASS::VDCLASS(const VALUETYPE& value) \
{ \
    m_value = value; \
} \
void VDCLASS::Copy(wxVariantData& data) \
{ \
    wxASSERT_MSG( data.GetType() == GetType(), wxT(#VDCLASS) wxT("::Copy: Can't copy to this type of data") ); \
    VDCLASS& otherData = (VDCLASS&) data; \
    otherData.m_value = m_value; \
} \
wxString VDCLASS::GetType() const \
{ \
    return wxString(wxT(#VALUETYPE)); \
} \
bool VDCLASS::Eq(wxVariantData& data) const \
{ \
    wxASSERT_MSG( data.GetType() == GetType(), wxT(#VDCLASS) wxT("::Eq: argument mismatch") ); \
    VDCLASS& otherData = (VDCLASS&) data; \
    return otherData.m_value == m_value; \
} \
void* VDCLASS::GetValuePtr() { return (void*)&m_value; }

// TODO: When ready, remove GetValueClassInfo
#define WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ_BASE(VALUETYPE,DEFPROPERTY,DEFVAL) \
class WX_PG_DECLARE_VALUE_TYPE_VDC(VALUETYPE) \
    virtual wxClassInfo* GetValueClassInfo(); \
}; \
WX_PG_IMPLEMENT_VALUE_TYPE_VDC(wxVariantData_##VALUETYPE,VALUETYPE) \
wxClassInfo* wxVariantData_##VALUETYPE::GetValueClassInfo() \
{ \
    return m_value.GetClassInfo(); \
} \
const wxPGValueType *wxPGValueType_##VALUETYPE = (wxPGValueType*) NULL; \
class wxPGValueType##VALUETYPE##Class : public wxPGValueType \
{ \
public: \
    virtual const wxChar* GetTypeName() const { return CLASSINFO(VALUETYPE)->GetClassName(); } \
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const \
    { return wxVariant ( new wxVariantData_##VALUETYPE( (*(VALUETYPE*)value.GetRawPtr()) ), name ); } \
    virtual wxPGProperty* GenerateProperty ( const wxString& label, const wxString& name ) const \
    { \
        return wxPG_CONSTFUNC(DEFPROPERTY)(label,name); \
    } \
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const \
    { \
        const VALUETYPE* real_value; \
        wxASSERT_MSG( wxStrcmp(GetTypeName(),value.GetType().c_str()) == 0, \
            wxT("GetPtrFromVariant: wxVariant type mismatch.") ); \
        wxVariantData_##VALUETYPE* vd = (wxVariantData_##VALUETYPE*)value.GetData(); \
        if ( vd->IsKindOf(CLASSINFO(wxVariantData_##VALUETYPE)) ) \
            real_value = &vd->GetValue(); \
        else \
            real_value  = ((const VALUETYPE*)value.GetWxObjectPtr()); \
        property->DoSetValue( *real_value ); \
    }

// This should not be used by built-in types (advprops.cpp types should use it though)
#define WX_PG_IMPLEMENT_VALUE_TYPE_CREATOR(VALUETYPE) \
wxPGValueType* wxPGNewVT##VALUETYPE() { return new wxPGValueType##VALUETYPE##Class; }

#define WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ(VALUETYPE,DEFPROPERTY,DEFVAL) \
WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ_BASE(VALUETYPE,DEFPROPERTY,DEFVAL) \
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant(DEFVAL); } \
}; \
WX_PG_IMPLEMENT_VALUE_TYPE_CREATOR(VALUETYPE)

#define WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ_OWNDEFAULT(VALUETYPE,DEFPROPERTY,DEFVAL) \
WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ_BASE(VALUETYPE,DEFPROPERTY,DEFVAL) \
    wxPGValueType##VALUETYPE##Class() { m_default = DEFVAL; } \
    virtual ~wxPGValueType##VALUETYPE##Class() { } \
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant(&m_default); } \
protected: \
    VALUETYPE   m_default; \
}; \
WX_PG_IMPLEMENT_VALUE_TYPE_CREATOR(VALUETYPE)

//    virtual const wxChar* GetTypeName() const { return wxPGValueType_void->GetTypeName(); }

#define WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_BASE(VALUETYPE,DEFPROPERTY,DEFVAL) \
const wxPGValueType *wxPGValueType_##VALUETYPE = (wxPGValueType*)NULL; \
class wxPGValueType##VALUETYPE##Class : public wxPGValueType \
{ \
protected: \
    VALUETYPE   m_default; \
public: \
    virtual const wxChar* GetTypeName() const { return wxT(#VALUETYPE); } \
    virtual const wxChar* GetCustomTypeName() const { return wxT(#VALUETYPE); } \
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant((void*)&m_default); } \
    virtual wxPGProperty* GenerateProperty ( const wxString& label, const wxString& name ) const \
    { \
        return wxPG_CONSTFUNC(DEFPROPERTY)(label,name); \
    } \
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const \
    { \
        wxASSERT_MSG( wxStrcmp(GetTypeName(),value.GetType().c_str()) == 0, \
            wxT("SetValueFromVariant: wxVariant type mismatch.") ); \
        wxVariantData_##VALUETYPE* vd = (wxVariantData_##VALUETYPE*)value.GetData(); \
        wxASSERT_MSG( vd->IsKindOf(CLASSINFO(wxVariantData_##VALUETYPE)), \
            wxT("SetValueFromVariant: wxVariantData mismatch.")); \
        property->DoSetValue((void*)&vd->GetValue() ); \
    } \
    wxPGValueType##VALUETYPE##Class() { m_default = DEFVAL; } \
    virtual ~wxPGValueType##VALUETYPE##Class() { }

#define WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_SIMPLE(VALUETYPE,DEFPROPERTY,DEFVAL) \
WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_BASE(VALUETYPE,DEFPROPERTY,DEFVAL) \
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const \
    { \
        void* ptr = (void*)value.GetRawPtr(); \
        wxASSERT ( ptr ); \
        return wxVariant ( ptr, name ); \
    } \
}; \
WX_PG_IMPLEMENT_VALUE_TYPE_CREATOR(VALUETYPE)

#define WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_CVD(VALUETYPE,DEFPROPERTY,DEFVAL,VDCLASS) \
WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_BASE(VALUETYPE,DEFPROPERTY,DEFVAL) \
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const \
    { \
        void* ptr = (void*)value.GetRawPtr(); \
        wxASSERT ( ptr ); \
        return wxVariant ( new VDCLASS(*((VALUETYPE*)ptr)), name ); \
    } \
}; \
WX_PG_IMPLEMENT_VALUE_TYPE_CREATOR(VALUETYPE)

#define WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP2(VALUETYPE,DEFPROPERTY,DEFVAL,VDCLASS) \
WX_PG_IMPLEMENT_VALUE_TYPE_VDC(VDCLASS,VALUETYPE) \
WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP_CVD(VALUETYPE,DEFPROPERTY,DEFVAL,VDCLASS)

#define WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(VALUETYPE,DEFPROPERTY,DEFVAL) \
WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP2(VALUETYPE,DEFPROPERTY,DEFVAL,wxVariantData_##VALUETYPE)

//
// Use this to create a new type with a different default value.
#define WX_PG_IMPLEMENT_DERIVED_TYPE(VALUETYPE,PARENTVT,DEFVAL) \
const wxPGValueType *wxPGValueType_##VALUETYPE = (wxPGValueType*) NULL; \
class wxPGValueType##VALUETYPE##Class : public wxPGValueType \
{ \
protected: \
    const wxPGValueType*    m_parentClass; \
    PARENTVT                m_default; \
public: \
    virtual const wxChar* GetTypeName() const { return m_parentClass->GetTypeName(); } \
    virtual const wxChar* GetCustomTypeName() const { return wxT(#VALUETYPE); } \
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant(m_default); } \
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const \
    { return m_parentClass->GenerateVariant(value,name); } \
    virtual wxPGProperty* GenerateProperty ( const wxString& label, const wxString& name ) const \
    { return m_parentClass->GenerateProperty(label,name); } \
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const \
    { m_parentClass->SetValueFromVariant(property,value); } \
    wxPGValueType##VALUETYPE##Class(); \
    virtual ~wxPGValueType##VALUETYPE##Class(); \
}; \
wxPGValueType##VALUETYPE##Class::wxPGValueType##VALUETYPE##Class() \
{ \
    m_default = DEFVAL; \
    m_parentClass = wxPGValueType_##PARENTVT; \
    wxASSERT ( m_parentClass != (wxPGValueType*) NULL); \
} \
wxPGValueType##VALUETYPE##Class::~wxPGValueType##VALUETYPE##Class() { }

// -----------------------------------------------------------------------

//
// Additional property class declaration helper macros
//

#define WX_PG_DECLARE_DERIVED_PROPERTY_CLASS() \
public: \
    WX_PG_DECLARE_GETCLASSNAME() \
private:

// -----------------------------------------------------------------------

//
// Property class implementation helper macros.
//

#define WX_PG_DECLARE_PARENTAL_TYPE_METHODS() \
    virtual void DoSetValue ( wxPGVariant value ); \
    virtual wxPGVariant DoGetValue () const;

#define WX_PG_DECLARE_BASIC_TYPE_METHODS() \
    virtual void DoSetValue ( wxPGVariant value ); \
    virtual wxPGVariant DoGetValue () const; \
    virtual wxString GetValueAsString ( int arg_flags = 0 ) const; \
    virtual bool SetValueFromString ( const wxString& text, int flags = 0 );

// class WXDLLIMPEXP_PG
#define wxPG_BEGIN_PROPERTY_CLASS_BODY2(CLASSNAME,UPCLASS,T,INTERNAL_T,T_AS_ARG,DECL) \
DECL CLASSNAME : public UPCLASS \
{ \
    WX_PG_DECLARE_PROPERTY_CLASS() \
protected: \
    INTERNAL_T      m_value; \
public: \
    CLASSNAME( const wxString& label, const wxString& name, T_AS_ARG value ); \
    virtual ~CLASSNAME();

#define wxPG_BEGIN_PROPERTY_CLASS_BODY(NAME,UPCLASS,T,T_AS_ARG) \
wxPG_BEGIN_PROPERTY_CLASS_BODY2(wxPG_PROPCLASS(NAME),UPCLASS,T,T,T_AS_ARG,class)

#define wxPG_BEGIN_PROPERTY_CLASS_BODY_WITH_DECL(NAME,UPCLASS,T,T_AS_ARG,DECL) \
wxPG_BEGIN_PROPERTY_CLASS_BODY2(wxPG_PROPCLASS(NAME),UPCLASS,T,T,T_AS_ARG,class DECL)

#define wxPG_END_PROPERTY_CLASS_BODY() \
};

#define WX_PG_DECLARE_CHOICE_METHODS() \
    virtual bool SetValueFromInt ( long value, int flags = 0 ); \
    virtual int GetChoiceInfo ( wxPGChoiceInfo* choiceinfo );

#define WX_PG_DECLARE_EVENT_METHODS() \
    virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary, wxEvent& event );

#define WX_PG_DECLARE_PARENTAL_METHODS() \
    virtual void ChildChanged ( wxPGProperty* p ); \
    virtual void RefreshChildren ();

#define WX_PG_DECLARE_CUSTOM_PAINT_METHODS() \
    virtual wxSize GetImageSize() const; \
    virtual void OnCustomPaint ( wxDC& dc, const wxRect& rect, wxPGPaintData& paintdata );

#define WX_PG_DECLARE_ATTRIBUTE_METHODS() \
    virtual void SetAttribute ( int id, wxVariant value );

#if wxPG_INCLUDE_WXOBJECT
# define wxPG_GETCLASSNAME_IMPLEMENTATION(PROPNAME)
#else
# define wxPG_GETCLASSNAME_IMPLEMENTATION(PROPNAME) \
    static const wxChar* PROPNAME##_ClassName = wxT(#PROPNAME); \
    const wxChar* PROPNAME##Class::GetClassName() const { return PROPNAME##_ClassName; }
#endif

// Implements sans constructor function. Also, first arg is class name, not property name.
#define WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(PROPNAME,T,EDITOR) \
wxPG_GETCLASSNAME_IMPLEMENTATION(PROPNAME) \
const wxPGValueType* PROPNAME##Class::GetValueType () const \
{ \
    return wxPGValueType_##T; \
} \
const wxPGEditor* PROPNAME##Class::GetEditorClass () const \
{ \
    return wxPGEditor_##EDITOR; \
}

#define WX_PG_IMPLEMENT_CLASSINFO(NAME) \
static wxPGProperty* NAME##B (const wxString& label, const wxString& name) \
{ \
    return wxPG_CONSTFUNC(NAME)(label,name); \
} \
wxPGPropertyClassInfo NAME##ClassInfo = {wxT(#NAME),&NAME##B};


// second constfunc and classinfo is for creating properties w/o value from text
// (should be changed to simpler scheme in 1.2)
#define WX_PG_IMPLEMENT_CONSTFUNC(NAME,T_AS_ARG) \
wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString& label, const wxString& name, T_AS_ARG value ) \
{ \
    return new wxPG_PROPCLASS(NAME)(label,name,value); \
}

// Adds constructor function as well.
#define WX_PG_IMPLEMENT_PROPERTY_CLASS2(NAME,CLASSNAME,T,T_AS_ARG,EDITOR) \
WX_PG_IMPLEMENT_CONSTFUNC(NAME,T_AS_ARG) \
WX_PG_IMPLEMENT_CLASSINFO(NAME) \
WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(NAME,T,EDITOR)

// A regular property
#define WX_PG_IMPLEMENT_PROPERTY_CLASS(NAME,T,T_AS_ARG,EDITOR) \
WX_PG_IMPLEMENT_PROPERTY_CLASS2(NAME,wxPG_PROPCLASS(NAME),T,T_AS_ARG,EDITOR)

// Derived property class is one that inherits from an existing working property
// class, but assumes same value and editor type.
#define WX_PG_IMPLEMENT_DERIVED_PROPERTY_CLASS(NAME,T_AS_ARG) \
WX_PG_IMPLEMENT_CONSTFUNC(NAME,T_AS_ARG) \
WX_PG_IMPLEMENT_CLASSINFO(NAME) \
wxPG_GETCLASSNAME_IMPLEMENTATION(NAME)

// -----------------------------------------------------------------------

#define WX_PG_IMPLEMENT_STRING_PROPERTY(NAME) \
class wxPG_PROPCLASS(NAME) : public wxLongStringPropertyClass \
{ \
    WX_PG_DECLARE_DERIVED_PROPERTY_CLASS() \
public: \
    wxPG_PROPCLASS(NAME)( const wxString& name, const wxString& label, const wxString& value ); \
    virtual ~wxPG_PROPCLASS(NAME)(); \
    virtual bool OnButtonClick ( wxPropertyGrid* propgrid, wxString& value ); \
}; \
WX_PG_IMPLEMENT_DERIVED_PROPERTY_CLASS(NAME,const wxString&) \
wxPG_PROPCLASS(NAME)::wxPG_PROPCLASS(NAME)( const wxString& name, const wxString& label, const wxString& value ) \
  : wxLongStringPropertyClass(name,label,value) \
{ } \
wxPG_PROPCLASS(NAME)::~wxPG_PROPCLASS(NAME)() { }

// -----------------------------------------------------------------------

// This will create interface for wxFlagsProperty derived class
// named CLASSNAME.
#define WX_PG_IMPLEMENT_CUSTOM_FLAGS_PROPERTY2(NAME,CLASSNAME,LABELS,VALUES,ITEMCOUNT,DEFVAL) \
class CLASSNAME : public wxFlagsPropertyClass \
{ \
    WX_PG_DECLARE_PROPERTY_CLASS() \
public: \
    CLASSNAME ( const wxString& label, const wxString& name, long value ); \
    virtual ~CLASSNAME(); \
}; \
WX_PG_IMPLEMENT_DERIVED_TYPE(long_##NAME,long,DEFVAL) \
WX_PG_IMPLEMENT_PROPERTY_CLASS(NAME,long_##NAME,long,TextCtrl) \
CLASSNAME::CLASSNAME ( const wxString& label, const wxString& name, long value ) \
    : wxFlagsPropertyClass(label,name,LABELS,VALUES,ITEMCOUNT,value!=-1?value:DEFVAL) \
{ \
    wxPG_INIT_REQUIRED_TYPE2(long_##NAME) \
    m_flags |= wxPG_PROP_STATIC_CHOICES; \
} \
CLASSNAME::~CLASSNAME() { }

#define WX_PG_IMPLEMENT_CUSTOM_FLAGS_PROPERTY(NAME,LABELS,VALUES,ITEMCOUNT,DEFVAL) \
WX_PG_IMPLEMENT_CUSTOM_FLAGS_PROPERTY2(NAME,wxPG_PROPCLASS(NAME),LABELS,VALUES,ITEMCOUNT,DEFVAL)

// -----------------------------------------------------------------------

// This will create interface for Enum property derived class
// named CLASSNAME.
#define WX_PG_IMPLEMENT_CUSTOM_ENUM_PROPERTY2(NAME,CLASSNAME,LABELS,VALUES,ITEMCOUNT,DEFVAL) \
class CLASSNAME : public wxEnumPropertyClass \
{ \
    WX_PG_DECLARE_PROPERTY_CLASS() \
public: \
    CLASSNAME ( const wxString& label, const wxString& name, int value ); \
    virtual ~CLASSNAME(); \
}; \
WX_PG_IMPLEMENT_DERIVED_TYPE(long_##NAME,long,DEFVAL) \
WX_PG_IMPLEMENT_PROPERTY_CLASS(NAME,long_##NAME,int,Choice) \
CLASSNAME::CLASSNAME ( const wxString& label, const wxString& name, int value ) \
    : wxEnumPropertyClass(label,name,LABELS,VALUES,ITEMCOUNT,value!=-1?value:DEFVAL) \
{ \
    wxPG_INIT_REQUIRED_TYPE2(long_##NAME) \
    m_flags |= wxPG_PROP_STATIC_CHOICES; \
} \
CLASSNAME::~CLASSNAME() { }

#define WX_PG_IMPLEMENT_CUSTOM_ENUM_PROPERTY(NAME,LABELS,VALUES,ITEMCOUNT,DEFVAL) \
WX_PG_IMPLEMENT_CUSTOM_ENUM_PROPERTY2(NAME,wxPG_PROPCLASS(NAME),LABELS,VALUES,ITEMCOUNT,DEFVAL)

// -----------------------------------------------------------------------

// Implementation for user wxColour editor property

#define WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY2(NAME,CLASSNAME,LABELS,VALUES,COLOURS,ITEMCOUNT) \
class CLASSNAME : public wxPG_PROPCLASS(wxSystemColourProperty) \
{ \
    WX_PG_DECLARE_DERIVED_PROPERTY_CLASS() \
public: \
    CLASSNAME( const wxString& label, const wxString& name, \
        const wxColourPropertyValue& value ); \
    virtual ~CLASSNAME (); \
    virtual long GetColour ( int index ); \
}; \
WX_PG_IMPLEMENT_DERIVED_PROPERTY_CLASS(NAME,const wxColourPropertyValue&) \
CLASSNAME::CLASSNAME( const wxString& label, const wxString& name, \
    const wxColourPropertyValue& value ) \
    : wxPG_PROPCLASS(wxSystemColourProperty)(label,name,LABELS,VALUES,ITEMCOUNT,value ) \
{ \
    wxPG_INIT_REQUIRED_TYPE(wxColourPropertyValue) \
    m_flags |= wxPG_PROP_TRANSLATE_CUSTOM; \
    DoSetValue ( &m_value ); \
} \
CLASSNAME::~CLASSNAME () { } \
long CLASSNAME::GetColour ( int index ) \
{ \
    const wxArrayInt& values = GetValues(); \
    if ( !values.GetCount() ) \
    { \
        wxASSERT ( index < (int)m_constants->GetCount() ); \
        return COLOURS[index]; \
    } \
    return COLOURS[values[index]]; \
}

#define WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY(NAME,LABELS,VALUES,COLOURS,ITEMCOUNT) \
    WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY2(NAME,wxPG_PROPCLASS(NAME),LABELS,VALUES,COLOURS,ITEMCOUNT)

// -----------------------------------------------------------------------

#define WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR2(NAME,CLASSNAME,LABELS,VALUES,COLOURS,ITEMCOUNT) \
class CLASSNAME : public wxPG_PROPCLASS(wxSystemColourProperty) \
{ \
    WX_PG_DECLARE_PROPERTY_CLASS() \
public: \
    CLASSNAME( const wxString& label, const wxString& name, \
        const wxColour& value ); \
    virtual ~CLASSNAME (); \
    virtual void DoSetValue ( wxPGVariant value ); \
    virtual wxPGVariant DoGetValue () const; \
    virtual long GetColour ( int index ); \
}; \
WX_PG_IMPLEMENT_PROPERTY_CLASS(NAME,wxColour,const wxColour&,Choice) \
CLASSNAME::CLASSNAME( const wxString& label, const wxString& name, const wxColour& value ) \
    : wxPG_PROPCLASS(wxSystemColourProperty)(label,name,LABELS,VALUES,ITEMCOUNT,value ) \
{ \
    wxPG_INIT_REQUIRED_TYPE(wxColour) \
    m_flags |= wxPG_PROP_TRANSLATE_CUSTOM; \
    DoSetValue(&m_value.m_colour); \
} \
CLASSNAME::~CLASSNAME () { } \
void CLASSNAME::DoSetValue ( wxPGVariant value ) \
{ \
    wxASSERT ( value.GetRawPtr() ); \
    wxColour* pval = wxPGVariantToWxObjectPtr(value,wxColour); \
    m_value.m_type = wxPG_COLOUR_CUSTOM; \
    if ( m_flags & wxPG_PROP_TRANSLATE_CUSTOM ) \
    { \
        int found_ind = ColToInd(*pval); \
        if ( found_ind != wxNOT_FOUND ) m_value.m_type = found_ind; \
    } \
    m_value.m_colour = *pval; \
    wxPG_SetVariantWxObjectValue(); \
    if ( m_value.m_type < wxPG_COLOUR_WEB_BASE ) \
        wxPG_PROPCLASS(wxEnumProperty)::DoSetValue ( (long)m_value.m_type ); \
    else \
        m_index = GetItemCount()-1; \
} \
wxPGVariant CLASSNAME::DoGetValue () const \
{ \
    return wxPGVariant(&m_value.m_colour); \
} \
long CLASSNAME::GetColour ( int index ) \
{ \
    const wxArrayInt& values = GetValues(); \
    if ( !values.GetCount() ) \
    { \
        wxASSERT ( index < (int)GetItemCount() ); \
        return COLOURS[index]; \
    } \
    return COLOURS[values[index]]; \
}

#define WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(NAME,LABELS,VALUES,COLOURS,ITEMCOUNT) \
    WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR2(NAME,wxPG_PROPCLASS(NAME),LABELS,VALUES,COLOURS,ITEMCOUNT)

// -----------------------------------------------------------------------

// These may be useful for external properties (I think these could be any two values)
#define wxPG_SUBID1                     11485
#define wxPG_SUBID2                     11486

// -----------------------------------------------------------------------

/** \class wxPGPaintData
    \ingroup classes
    \brief Contains information relayed to property's OnCustomPaint.
*/
struct wxPGPaintData
{
    /** wxPropertyGrid. */
    wxPropertyGrid* m_parent;

    /** Normally -1, otherwise index to drop-down list item that has to be drawn. */
    int             m_choiceItem;

    /** Set to drawn width in OnCustomPaint (optional). */
    int             m_drawnWidth;

    /** In a measure item call, set this to the height of item at m_choiceItem index. */
    int             m_drawnHeight;

};

#if defined(_WX_WINDOW_H_BASE_) // DOXYGEN


// -----------------------------------------------------------------------
// Some property class definitions (these should be useful to inherit from).
// -----------------------------------------------------------------------

// This provides base for wxEnumPropertyClass and any custom
// "dynamic" enum property classes.
class WXDLLIMPEXP_PG wxBaseEnumPropertyClass : public wxPGProperty
{
public:
    wxBaseEnumPropertyClass( const wxString& label, const wxString& name );

    virtual void DoSetValue ( wxPGVariant value );
    virtual wxPGVariant DoGetValue () const;
    virtual wxString GetValueAsString ( int arg_flags ) const;
    virtual bool SetValueFromString ( const wxString& text, int arg_flags );
    virtual bool SetValueFromInt ( long value, int arg_flags );

    //
    // Additional virtuals

    // This must be overridden to have non-index based value
    virtual int GetIndexForValue( int value ) const;

    // This returns string and value for index
    // Returns NULL if beyond last item
    // pvalue is never NULL - always set it.
    virtual const wxString* GetEntry( size_t index, int* pvalue ) const = 0;

protected:

    int                     m_index;
};

// -----------------------------------------------------------------------

// If set, then selection of choices is static and should not be
// changed (i.e. returns NULL in GetPropertyChoices).
#define wxPG_PROP_STATIC_CHOICES    wxPG_PROP_CLASS_SPECIFIC_1

class WXDLLIMPEXP_PG wxEnumPropertyClass : public wxBaseEnumPropertyClass // wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxEnumPropertyClass ( const wxString& label, const wxString& name, const wxChar** labels,
        const long* values = NULL, unsigned int itemcount = 0, int value = 0 );
    wxEnumPropertyClass ( const wxString& label, const wxString& name, 
        wxPGConstants& constants, int value = 0 );
    virtual ~wxEnumPropertyClass ();

    virtual int GetChoiceInfo ( wxPGChoiceInfo* choiceinfo );
    virtual int GetIndexForValue( int value ) const;
    virtual const wxString* GetEntry( size_t index, int* pvalue ) const;

    inline size_t GetItemCount () const { return m_constants->GetCount(); }
    inline const wxArrayInt& GetValues() const { return m_constants->GetValues(); }

    inline wxPGConstants& GetChoices() const { return *m_constants; }

protected:
    wxPGConstants*          m_constants;
};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_PG wxFlagsPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxFlagsPropertyClass ( const wxString& label, const wxString& name, const wxChar** labels,
        const long* values = NULL, unsigned int itemcount = 0, long value = 0 );
    wxFlagsPropertyClass ( const wxString& label, const wxString& name, 
        wxPGConstants& constants, long value = 0 );
    virtual ~wxFlagsPropertyClass ();

    virtual void DoSetValue ( wxPGVariant value );
    virtual wxPGVariant DoGetValue () const;
    virtual wxString GetValueAsString ( int arg_flags ) const;
    virtual bool SetValueFromString ( const wxString& text, int flags );
    virtual void ChildChanged ( wxPGProperty* p );
    virtual void RefreshChildren();

    // this is necessary for conveying m_constants
    virtual int GetChoiceInfo ( wxPGChoiceInfo* choiceinfo );

    // helpers
    inline size_t GetItemCount() const { return m_constants->GetCount(); }
    inline const wxArrayInt& GetValues() const { return m_constants->GetValues(); }
    inline const wxString& GetLabel( size_t ind ) const { return m_constants->GetLabel(ind); }

protected:
    wxPGConstants*          m_constants;

    long                    m_value;

    // Converts string id to a relevant bit.
    long IdToBit ( const wxString& id ) const;

    // Creates children and sets their value.
    void Init ();

};

// -----------------------------------------------------------------------

#include <wx/filename.h>

// Indicates first bit useable by derived properties.
#define wxPG_PROP_SHOW_FULL_FILENAME  wxPG_PROP_CLASS_SPECIFIC_1

class WXDLLIMPEXP_PG wxFilePropertyClass : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxFilePropertyClass ( const wxString& label, const wxString& name = wxPG_LABEL,
        const wxString& value = wxEmptyString );
    virtual ~wxFilePropertyClass ();

    virtual void DoSetValue ( wxPGVariant value );
    virtual wxPGVariant DoGetValue () const;
    virtual wxString GetValueAsString ( int arg_flags ) const;
    virtual bool SetValueFromString ( const wxString& text, int flags );
    virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* wnd_primary, wxEvent& event );

    virtual void SetAttribute ( int id, wxVariant value );

protected:
    wxString    m_wildcard;
    wxString    m_fnstr; // needed for return value
    wxFileName  m_filename; // used as primary storage
    int         m_indFilter; // index to the selected filter
};

// -----------------------------------------------------------------------

//
// In wxTextCtrl, strings a space delimited C-like strings. For example:
// "String 1" "String 2" "String 3"
//
// To have " in a string, use \".
// To have \ in a string, use \\.
//
class WXDLLIMPEXP_PG wxLongStringPropertyClass : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxLongStringPropertyClass ( const wxString& label, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString );
    virtual ~wxLongStringPropertyClass ();

    virtual void DoSetValue ( wxPGVariant value );
    virtual wxPGVariant DoGetValue () const;
    virtual wxString GetValueAsString ( int arg_flags = 0 ) const;
    virtual bool SetValueFromString ( const wxString& text, int flags );
    virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* wnd_primary, wxEvent& event );

    //  Shows string editor dialog. Value to be edited should be read from value, and
    //  if dialog is not cancelled, it should be stored back and TRUE should be returned
    //  if that was the case.
    virtual bool OnButtonClick ( wxPropertyGrid* propgrid, wxString& value );

protected:
    wxString    m_value;
};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_PG wxArrayStringPropertyClass : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxArrayStringPropertyClass ( const wxString& label, const wxString& name,
        const wxArrayString& value );
    virtual ~wxArrayStringPropertyClass ();

    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()
    virtual void GenerateValueAsString ();

    //  Shows string editor dialog. Value to be edited should be read from value, and
    //  if dialog is not cancelled, it should be stored back and TRUE should be returned
    //  if that was the case.
    virtual bool OnCustomStringEdit ( wxWindow* parent, wxString& value );

    // Helper.
    bool OnButtonClick ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
        const wxChar* cbt );

protected:
    wxArrayString   m_value;
    wxString        m_display; // Cache for displayed text.
};

#define WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY(PROPNAME,DELIMCHAR,CUSTBUTTXT) \
class wxPG_PROPCLASS(PROPNAME) : public wxPG_PROPCLASS(wxArrayStringProperty) \
{ \
    WX_PG_DECLARE_PROPERTY_CLASS() \
public: \
    wxPG_PROPCLASS(PROPNAME) ( const wxString& label, const wxString& name, const wxArrayString& value ); \
    ~wxPG_PROPCLASS(PROPNAME) (); \
    virtual void GenerateValueAsString (); \
    virtual bool SetValueFromString ( const wxString& text, int ); \
    virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary, wxEvent& event ); \
    virtual bool OnCustomStringEdit ( wxWindow* parent, wxString& value ); \
}; \
WX_PG_IMPLEMENT_PROPERTY_CLASS(PROPNAME,wxArrayString,const wxArrayString&,TextCtrlAndButton) \
wxPG_PROPCLASS(PROPNAME)::wxPG_PROPCLASS(PROPNAME) ( const wxString& label, const wxString& name, const wxArrayString& value ) \
    : wxPG_PROPCLASS(wxArrayStringProperty)(label,name,value) \
{ \
} \
wxPG_PROPCLASS(PROPNAME)::~wxPG_PROPCLASS(PROPNAME)() { } \
void wxPG_PROPCLASS(PROPNAME)::GenerateValueAsString () \
{ \
    wxChar delim_char = DELIMCHAR; \
    if ( delim_char == wxT('"') ) \
        wxPG_PROPCLASS(wxArrayStringProperty)::GenerateValueAsString(); \
    else \
        wxPropertyGrid::ArrayStringToString(m_display,m_value,0,DELIMCHAR,0); \
} \
bool wxPG_PROPCLASS(PROPNAME)::SetValueFromString ( const wxString& text, int ) \
{ \
    wxChar delim_char = DELIMCHAR; \
    if ( delim_char == wxT('"') ) \
        return wxPG_PROPCLASS(wxArrayStringProperty)::SetValueFromString(text,0); \
    \
    m_value.Empty(); \
    WX_PG_TOKENIZER1_BEGIN(text,DELIMCHAR) \
        m_value.Add ( token ); \
    WX_PG_TOKENIZER1_END() \
    GenerateValueAsString(); \
    return TRUE; \
} \
bool wxPG_PROPCLASS(PROPNAME)::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary, wxEvent& event ) \
{ \
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED ) \
        return OnButtonClick(propgrid,primary,(const wxChar*) CUSTBUTTXT); \
    return FALSE; \
}


// -----------------------------------------------------------------------
// wxArrayEditorDialog
// -----------------------------------------------------------------------

#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>

#define wxAEDIALOG_STYLE \
    (wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxOK | wxCANCEL | wxCENTRE)

class WXDLLIMPEXP_PG wxArrayEditorDialog : public wxDialog
{
public:
    wxArrayEditorDialog();

    void Init();

    wxArrayEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              long style = wxAEDIALOG_STYLE,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& sz = wxDefaultSize );

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                long style = wxAEDIALOG_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& sz = wxDefaultSize );

    /*
    inline void SetCustomButton ( const wxChar* custBtText, wxArrayStringPropertyClass* pcc )
    {
        m_custBtText = custBtText;
        m_pCallingClass = pcc;
    }*/

    // Returns TRUE if array was actually modified
    bool IsModified() const { return m_modified; }

    //const wxArrayString& GetStrings() const { return m_array; }

    // implementation from now on
    void OnUpdateClick(wxCommandEvent& event);
    void OnAddClick(wxCommandEvent& event);
    void OnDeleteClick(wxCommandEvent& event);
    void OnListBoxClick(wxCommandEvent& event);
    void OnUpClick(wxCommandEvent& event);
    void OnDownClick(wxCommandEvent& event);
    //void OnCustomEditClick(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);

protected:
    wxTextCtrl*     m_edValue;
    wxListBox*      m_lbStrings;

    wxButton*       m_butAdd;       // Button pointers
    wxButton*       m_butCustom;    // required for disabling/enabling changing.
    wxButton*       m_butUpdate;
    wxButton*       m_butRemove;
    wxButton*       m_butUp;
    wxButton*       m_butDown;

    //wxArrayString   m_array;

    const wxChar*   m_custBtText;
    //wxArrayStringPropertyClass*     m_pCallingClass;

    bool            m_modified;

    unsigned char   m_curFocus;

    // These must be overridden - must return TRUE on success.
    virtual wxString ArrayGet( size_t index ) = 0;
    virtual size_t ArrayGetCount() = 0;
    virtual bool ArrayInsert( const wxString& str, int index ) = 0;
    virtual bool ArraySet( size_t index, const wxString& str ) = 0;
    virtual void ArrayRemoveAt( int index ) = 0;
    virtual void ArraySwap( size_t first, size_t second ) = 0;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxArrayEditorDialog)
    DECLARE_EVENT_TABLE()
};

// -----------------------------------------------------------------------

/** This is a simple property which holds sub-properties. Has default editing
    textctrl based editing capability. In essence, it is a category that has
    look and feel of a property, and which children can be edited via the textctrl.
*/
class wxParentPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxParentPropertyClass ( const wxString& label, const wxString& name = wxPG_LABEL );
    virtual ~wxParentPropertyClass ();

    virtual void DoSetValue ( wxPGVariant value );
    virtual wxPGVariant DoGetValue () const;
    virtual void ChildChanged ( wxPGProperty* p );

protected:
    wxString    m_string;
};

// -----------------------------------------------------------------------

/** \class wxCustomPropertyClass
    \ingroup classes
    \brief This is a rather inefficient but very versatile property class.

   Base class offers the following:
     - Add any properties as children (i.e. like the old wxParentProperty)
     - Editor control can be set at run-time.
     - By default has string value type.
     - Has capacity to have choices.
     - Can have custom-paint bitmap.

   Also note:
     - Has m_parentingType of -2 (technical detail).
*/
class wxCustomPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxCustomPropertyClass ( const wxString& label, const wxString& name = wxPG_LABEL );
    virtual ~wxCustomPropertyClass ();

    virtual void DoSetValue ( wxPGVariant value );
    virtual wxPGVariant DoGetValue () const;
    virtual bool SetValueFromString ( const wxString& text, int flags );
    virtual wxString GetValueAsString ( int arg_flags ) const;

    virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary, wxEvent& event );

    WX_PG_DECLARE_CUSTOM_PAINT_METHODS()

    virtual bool SetValueFromInt ( long value, int );
    virtual int GetChoiceInfo ( wxPGChoiceInfo* choiceinfo );

    virtual void SetAttribute ( int id, wxVariant value );

protected:
    wxPGEditor*             m_editor;
    wxPGConstants*          m_constants;
    wxBitmap*               m_bitmap;
    wxPropertyGridCallback  m_callback;
    wxPGPaintCallback       m_paintCallback;

    wxString                m_value;
};

// -----------------------------------------------------------------------

//
// Tokenizer macros.
// NOTE: I have made two versions - worse ones (performance and consistency
//   wise) use wxStringTokenizer and better ones (may have unfound bugs)
//   use custom code.
//

#include <wx/tokenzr.h>

// TOKENIZER1 can be done with wxStringTokenizer
#define WX_PG_TOKENIZER1_BEGIN(WXSTRING,DELIMITER) \
    wxStringTokenizer tkz(WXSTRING,DELIMITER,wxTOKEN_RET_EMPTY); \
    while ( tkz.HasMoreTokens() ) \
    { \
        wxString token = tkz.GetNextToken(); \
        token.Trim(TRUE); \
        token.Trim(FALSE);

#define WX_PG_TOKENIZER1_END() \
    }


/*#define WX_PG_TOKENIZER1_BEGIN(WXSTRING,DELIMITER) \
    const wxChar* ptr = WXSTRING.c_str(); \
    wxString token; \
    const wxChar* token_start = NULL; \
    wxChar a = 0; \
    do \
    { \
        a = *ptr; \
        while ( a == ' ' ) { ptr++; a = *ptr; } \
        token_start = ptr; \
        while ( a != DELIMITER && a != 0 ) { ptr++; a = *ptr; } \
        if ( ptr > token_start ) \
        { \
            unsigned int str_len = ptr-token_start; \
            wxChar* store_ptr = token.GetWriteBuf ( str_len+1 ); \
            wxTmemcpy ( store_ptr, token_start, str_len ); \
            store_ptr[str_len] = 0; \
            token.UngetWriteBuf ( str_len ); \
            token.Trim(); \
        } \
        else \
            token.Empty();

#define WX_PG_TOKENIZER1_END() \
        ptr++; \
    } while ( a );*/

/*

#define WX_PG_TOKENIZER2_BEGIN(WXSTRING,DELIMITER) \
    wxStringTokenizer tkz(WXSTRING,DELIMITER,wxTOKEN_RET_EMPTY); \
    int phase = 0; \
    while ( tkz.HasMoreTokens() ) \
    { \
        wxString token = tkz.GetNextToken(); \
        if ( phase != 0 ) \
        {

#define WX_PG_TOKENIZER2_END() \
            phase = -1; \
        } \
        phase += 1; \
    }

*/

//
// 2nd version: tokens are surrounded by DELIMITERs (for example, C-style strings).
// TOKENIZER2 must use custom code (a class) for full compliancy
// with " surrounded strings with \" inside.
//
// class implementation is in propgrid.cpp
//

class WXDLLIMPEXP_PG wxPGStringTokenizer
{
public:
    wxPGStringTokenizer(const wxString& str,
                        wxChar delimeter);
    ~wxPGStringTokenizer();

    bool HasMoreTokens(); // not const so we can do some stuff in it
    wxString GetNextToken();

protected:

    const wxString* m_str;
    const wxChar*   m_curPos;
#if wxUSE_STL
    //wxString        m_buffer;
#endif
    wxString        m_readyToken;
    wxChar          m_delimeter;
};

#define WX_PG_TOKENIZER2_BEGIN(WXSTRING,DELIMITER) \
    wxPGStringTokenizer tkz(WXSTRING,DELIMITER); \
    while ( tkz.HasMoreTokens() ) \
    { \
        wxString token = tkz.GetNextToken();

#define WX_PG_TOKENIZER2_END() \
    }

/*
#if wxUSE_STL

// 2nd version: tokens are surrounded by DELIMITERs (for example, C-style strings).
// TOKENIZER2 must use custom code for full compliancy
// with " surrounded strings with \" inside.
#define WX_PG_TOKENIZER2_BEGIN(WXSTRING,DELIMITER) \
    const wxChar* ptr = WXSTRING.c_str(); \
    wxString token; \
    wxStringBuffer strbuf(token,2048); \
    wxChar* store_ptr_start = NULL; \
    wxChar* store_ptr = NULL; \
    wxChar a = *ptr; \
    wxChar prev_a = 0; \
    while ( a ) \
    { \
        if ( !store_ptr_start ) \
        { \
            if ( a == DELIMITER ) \
            { \
                store_ptr_start = store_ptr = strbuf; \
                prev_a = 0; \
            } \
        } \
        else \
        { \
            if ( prev_a != wxT('\\') ) \
            { \
                if ( a != DELIMITER ) \
                { \
                    if ( a != wxT('\\') ) \
                    { \
                        *store_ptr = a; \
                        store_ptr++; \
                    } \
                } \
                else \
                { \
                    *store_ptr = 0; \
                    wxASSERT ( (store_ptr-store_ptr_start) < 2048 );

#define WX_PG_TOKENIZER2_END() \
                    store_ptr_start = NULL; \
                } \
                prev_a = a; \
            } \
            else \
            { \
                *store_ptr = a; \
                store_ptr++; \
                prev_a = 0; \
            } \
        } \
        ptr++; \
        a = *ptr; \
    }


#else // wxUSE_STL

//
// NON USE_STL COMPLIANT VERSION
//
// 2nd version: tokens are surrounded by DELIMITERs (for example, C-style strings).
// TOKENIZER2 must use custom code for full compliancy
// with " surrounded strings with \" inside.
#define WX_PG_TOKENIZER2_BEGIN(WXSTRING,DELIMITER) \
    const wxChar* ptr = WXSTRING.c_str(); \
    const wxChar* ptr_end = &ptr[WXSTRING.length()]; \
    wxString token; \
    wxChar* store_ptr_start = NULL; \
    wxChar* store_ptr = NULL; \
    wxChar a = *ptr; \
    wxChar prev_a = 0; \
    while ( a ) \
    { \
        if ( !store_ptr_start ) \
        { \
            if ( a == DELIMITER ) \
            { \
                store_ptr_start = store_ptr = token.GetWriteBuf ( ptr_end-ptr+1 ); \
                prev_a = 0; \
            } \
        } \
        else \
        { \
            if ( prev_a != wxT('\\') ) \
            { \
                if ( a != DELIMITER ) \
                { \
                    if ( a != wxT('\\') ) \
                    { \
                        *store_ptr = a; \
                        store_ptr++; \
                    } \
                } \
                else \
                { \
                    *store_ptr = 0; \
                    token.UngetWriteBuf ( store_ptr-store_ptr_start ); \

#define WX_PG_TOKENIZER2_END() \
                    store_ptr_start = NULL; \
                } \
                prev_a = a; \
            } \
            else \
            { \
                *store_ptr = a; \
                store_ptr++; \
                prev_a = 0; \
            } \
        } \
        ptr++; \
        a = *ptr; \
    } \
    if ( store_ptr_start ) \
        token.UngetWriteBuf ( store_ptr-store_ptr_start );

#endif // !wxUSE_STL

*/
// -----------------------------------------------------------------------

#endif // !DOXYGEN

#endif // _WX_PROPGRID_PROPDEV_H_
