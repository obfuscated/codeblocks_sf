/////////////////////////////////////////////////////////////////////////////
// Name:        propgrid.h
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_PROPGRID_H__
#define __WX_PROPGRID_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "propgrid.cpp"
#endif

/*

  -- IMPORTANT API NOTES --

  * Intermediate property classes that should only be inherited from
    have names that begin with wxPG (for example, wxPGProperty and
    wxPGPropertyWithChildren).

*/

#include <wx/dynarray.h>

#include <wx/hashmap.h>

#include <wx/variant.h>

#include <wx/tooltip.h>

//#include <wx/validate.h>

// Doxygen special
#if !defined(_WX_WINDOW_H_BASE_) && !defined(SWIG)
# include "pg_dox_mainpage.h"
#endif

// Custom version information (takes CVS(i.e. subrelease) into account)
#define wxPG_WX_VERSION ((wxMAJOR_VERSION*100)+(wxMINOR_VERSION*10)+(wxRELEASE_NUMBER+wxSUBRELEASE_NUMBER))

// -----------------------------------------------------------------------


//
// Here are some platform dependent defines
// NOTE: More in propertygrid.cpp
//

#ifndef SWIG
#if defined(__WXMSW__)
    // tested

    #define wxPG_XBEFORETEXT            5 // space between vertical line and value text
    #define wxPG_XBEFOREWIDGET          1 // space between vertical line and value editor control

    #define wxPG_ICON_WIDTH             9 // comment to use bitmap buttons
    #define wxPG_USE_RENDERER_NATIVE    0 // 1 if wxRendererNative should be employed

# if defined(__WXWINCE__)
    #define wxPG_DOUBLE_BUFFER          0 // 1 to use double-buffer that guarantees flicker-free painting
# else
    #define wxPG_DOUBLE_BUFFER          1 // 1 to use double-buffer that guarantees flicker-free painting
# endif

    #define wxPG_HEAVY_GFX              1 // 1 for prettier appearance

#ifndef wxPG_USE_CUSTOM_CONTROLS
    #define wxPG_USE_CUSTOM_CONTROLS    0 // 1 to use wxCustomControls instead native ones (usually better)
#endif

    #define wxPG_SUPPORT_TOOLTIPS       1 // Enable tooltips

#elif defined(__WXGTK__)
    // tested

    #define wxPG_XBEFORETEXT            6 // space between vertical line and value text
    #define wxPG_XBEFOREWIDGET          1 // space between vertical line and value editor control

    #define wxPG_ICON_WIDTH             9 // comment to use bitmap buttons
    #define wxPG_USE_RENDERER_NATIVE    0 // 1 if wxRendererNative should be employed

    #define wxPG_DOUBLE_BUFFER          1 // 1 to use double-buffer that guarantees flicker-free painting

    #define wxPG_HEAVY_GFX              1 // 1 for prettier appearance

#ifndef wxPG_USE_CUSTOM_CONTROLS
    #define wxPG_USE_CUSTOM_CONTROLS    0 // 1 to use wxCustomControls instead native ones (usually better)
#endif

    #define wxPG_SUPPORT_TOOLTIPS       1  // Enable tooltips

#elif defined(__WXMAC__)
    // *not* tested

    #define wxPG_XBEFORETEXT            5 // space between vertical line and value text
    #define wxPG_XBEFOREWIDGET          1 // space between vertical line and value editor widget

    #define wxPG_ICON_WIDTH             11  // comment to use bitmap buttons
    #define wxPG_USE_RENDERER_NATIVE    1 // 1 if wxRendererNative should be employed

    #define wxPG_DOUBLE_BUFFER          0 // 1 to use double-buffer that guarantees flicker-free painting

    #define wxPG_HEAVY_GFX              1 // 1 for prettier appearance

#ifndef wxPG_USE_CUSTOM_CONTROLS
    #define wxPG_USE_CUSTOM_CONTROLS    0 // 1 to use wxCustomControls instead native ones (usually better)
#endif

    #define wxPG_SUPPORT_TOOLTIPS       1  // Enable tooltips

#else
    // defaults
    // tested on: none.

    #define wxPG_XBEFORETEXT            6 // space between vertical line and value text
    #define wxPG_XBEFOREWIDGET          1 // space between vertical line and value editor widget

    #define wxPG_ICON_WIDTH             9 // comment to use bitmap buttons
    #define wxPG_USE_RENDERER_NATIVE    0 // 1 if wxRendererNative should be employed

    #define wxPG_DOUBLE_BUFFER          0 // 1 to use double-buffer that guarantees flicker-free painting

    #define wxPG_HEAVY_GFX              0 // 1 for prettier appearance

#ifndef wxPG_USE_CUSTOM_CONTROLS
    #define wxPG_USE_CUSTOM_CONTROLS    1 // 1 to use wxCustomControls instead native ones (usually better)
#endif

    #define wxPG_SUPPORT_TOOLTIPS       0  // Enable tooltips

#endif

#endif // SWIG

// Undefine wxPG_ICON_WIDTH to use supplied xpm bitmaps instead
// (for tree buttons)
//#undef wxPG_ICON_WIDTH

// Need to force disable tooltips?
#if !wxUSE_TOOLTIPS
# undef wxPG_SUPPORT_TOOLTIPS
# define wxPG_SUPPORT_TOOLTIPS       0
#endif

// Set 1 to include basic properties ( ~48k in 0.9.9.2 )
#define wxPG_INCLUDE_BASICPROPS         1

// Set 1 to include advanced properties (wxFontProperty, wxColourProperty, etc.) ( ~32k in 0.9.9.2 )
#define wxPG_INCLUDE_ADVPROPS           1

// Set 1 include wxPropertyGridManager ( ~36k in 0.9.9.1 )
//#define wxPG_INCLUDE_MANAGER            1

// Set 1 to include checkbox editor class ( ~4k in 0.9.9.1 )
#define wxPG_INCLUDE_CHECKBOX           1

// 1 to allow user data for each property
#define wxPG_USE_CLIENT_DATA            1

// 1 to allow per-property validators
#define wxPG_USE_VALIDATORS             0

// NOTE: Currently advantages granted by wxPG_EMBED_VARIANT are probably minimal.
#define wxPG_EMBED_VARIANT              0 // 1 if wxVariant is in wxPGProperty (should be better if used a lot)

// NOTE: Use this only if you really need wxDynamicCast etc. Property class name
//   can be acquired with wxPropertyGrid::GetPropertyClassName() anyway.
// ALSO NOTE: Causes quite a code size increase (~20k in main object file) considering what
//   it does.
#define wxPG_INCLUDE_WXOBJECT           0 // 1 if inherit wxPGProperty from wxObject - always 1 if wxPG_EMBED_VARIANT


#define wxPG_USE_GENERIC_TEXTCTRL       0 // This at 1 is deprecated feature


#if wxPG_EMBED_VARIANT
# error "wxPG_EMBED_VARIANT is currently broken."
# undef wxPG_INCLUDE_WXOBJECT
# define wxPG_INCLUDE_WXOBJECT           1
#endif

//
// To have same native colour scheme calculation as in version
// 1.0.3 and earlier, set this to 0.
#define wxPG_NEW_COLOUR_SCHEME           1


// -----------------------------------------------------------------------

// Our very custom dynamic object macros. Should only be used
// directly in an abstract (typeless etc.) base property classes.
#if wxPG_INCLUDE_WXOBJECT
# error "wxPG_INCLUDE_WXOBJECT is not currently supported (as of 1.0.0b)."
//# define _WX_PG_DECLARE_PROPERTY_CLASS(A)       DECLARE_CLASS(A)
//# define _WX_PG_IMPLEMENT_PROPERTY_CLASS(A,B)   IMPLEMENT_CLASS(A,B)
# define WX_PG_DECLARE_GETCLASSNAME()
#else
//# define _WX_PG_DECLARE_PROPERTY_CLASS(A)
//# define _WX_PG_IMPLEMENT_PROPERTY_CLASS(A,B)
# define WX_PG_DECLARE_GETCLASSNAME()          virtual const wxChar* GetClassName() const;
#endif

// -----------------------------------------------------------------------

#ifdef WXMAKINGDLL_PROPGRID
    #define WXDLLIMPEXP_PG WXEXPORT
//#elif defined(WXUSINGDLL)
//    #define WXDLLIMPEXP_PG WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_PG
#endif

// -----------------------------------------------------------------------

#if wxPG_USE_CUSTOM_CONTROLS
# include "wx/propgrid/custctrl.h"
# define wxPGCtrlClass wxCustomControl
#else
# define wxPGCtrlClass wxWindow
# if !wxPG_USE_GENERIC_TEXTCTRL
#  define wxCCustomTextCtrl wxTextCtrl
# else
#  include "wx/propgrid/custctrl.h"
#  define wxCCustomTextCtrl wxGenericTextCtrl
# endif
//# define wxCCustomComboBox wxComboBox
class WXDLLEXPORT wxPGOwnerDrawnComboBox;
# define wxCCustomComboBox wxPGOwnerDrawnComboBox
# define wxCCustomButton wxButton
#endif

// -----------------------------------------------------------------------

#ifndef SWIG
class  WXDLLIMPEXP_PG wxPGVariant;
class  WXDLLIMPEXP_PG wxPGValueType;
class  WXDLLIMPEXP_PG wxPGEditor;
class  WXDLLIMPEXP_PG wxPGProperty;
class  WXDLLIMPEXP_PG wxPGPropertyWithChildren;
class  WXDLLIMPEXP_PG wxPropertyCategoryClass;
class  WXDLLIMPEXP_PG wxPGConstants;
class  WXDLLIMPEXP_PG wxPropertyGridState;
class  WXDLLIMPEXP_PG wxPropertyGrid;
class  WXDLLIMPEXP_PG wxPropertyGridEvent;
class  WXDLLIMPEXP_PG wxPropertyGridManager;
#endif

struct wxPGPaintData;

extern WXDLLIMPEXP_PG const wxChar *wxPropertyGridNameStr;

/** @defgroup miscellaneous wxPropertyGrid Miscellanous
    This section describes some miscellanous values, types and macros.
    @{
*/

/** Used to tell wxPGProperty to use label as name as well. */
#define wxPG_LABEL (*((const wxString*)NULL))

/** Convert Red, Green and Blue to a single 32-bit value.
*/
#define wxPG_COLOUR(R,G,B) ((wxUint32)(R+(G<<8)+(B<<16)))

/** Return this in GetImageSize() to indicate that the property
    custom paint is flexible. That is, it will paint (dropdown)
    list items with PREFWID,PREFHEI size, and in with required
    default size in the value cell.
*/
#define wxPG_FLEXIBLE_SIZE(PREFWID,PREFHEI) wxSize(-(PREFWID),-(PREFHEI))

/** This callback function is used by atleast wxCustomProperty
    to facilitiate easy custom action on button press.
    \param propGrid
    related wxPropertyGrid
    \param property
    related wxPGProperty
    \param ctrl
    If not NULL (for example, not selected), a wxWindow* or equivalent
    \param data
    Value depends on the context.
    \retval
    True if changed value of the property.
*/
typedef bool (*wxPropertyGridCallback)(wxPropertyGrid* propGrid,
                                       wxPGProperty* property,
                                       wxPGCtrlClass* ctrl,
                                       int data);

/** This callback function is used by atleast wxCustomProperty
    to facilitiate drawing items in drop down list.

    Works very much like wxPGProperty::OnCustomPaint.
*/
typedef void (*wxPGPaintCallback)(wxPGProperty* property,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  wxPGPaintData& paintdata);


/** @}
*/

// -----------------------------------------------------------------------

/** @defgroup wndflags wxPropertyGrid Window Styles
    SetWindowStyleFlag method can be used to modify some of these at run-time.
    @{
*/
/** This will cause Sort() automatically after an item is added.
    When inserting a lot of items in this mode, it may make sense to
    use Freeze() before operations and Thaw() afterwards to increase
    performance.
*/
#define wxPG_AUTO_SORT              0x00000010

/** Categories are not initially shown (even if added).
    IMPORTANT NOTE: If you do not plan to use categories, then this
    style will result in waste of resources.
    This flag can also be changed using wxPropertyGrid::EnableCategories method.
*/
#define wxPG_HIDE_CATEGORIES        0x00000020

/* This style combines non-categoric mode and automatic sorting.
*/
#define wxPG_ALPHABETIC_MODE        (wxPG_HIDE_CATEGORIES|wxPG_AUTO_SORT)

/** Modified values are shown in bold font. Changing this requires Refresh()
    to show changes.
*/
#define wxPG_BOLD_MODIFIED          0x00000040

/** When wxPropertyGrid is resized, splitter moves to the center. This
    behaviour stops once the user manually moves the splitter.
*/
#define wxPG_SPLITTER_AUTO_CENTER   0x00000080

/** Display tooltips for cell text that cannot be shown completely. If
    wxUSE_TOOLTIPS is 0, then this doesn't have any effect.
*/
#define wxPG_TOOLTIPS               0x00000100

/** Disables margin and hides all expand/collapse buttons that would appear
    outside the margin (for sub-properties). Toggling this style automatically
    expands all collapsed items.
*/
#define wxPG_HIDE_MARGIN            0x00000200

/** This style prevents user from moving the splitter.
*/
#define wxPG_STATIC_SPLITTER        0x00000400

/** Combination of other styles that make it impossible for user to modify
    the layout.
*/
#define wxPG_STATIC_LAYOUT          (wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER)

/** Disables wxTextCtrl based editors for properties which
    can be edited in another way. Equals calling wxPropertyGrid::LimitPropertyEditing
    for all added properties.
*/
#define wxPG_LIMITED_EDITING        0x00000800

/** wxPropertyGridManager only: Show toolbar for mode and page selection. */
#define wxPG_TOOLBAR                0x00001000

/** wxPropertyGridManager only: Show adjustable text box showing description
    or help text, if available, for currently selected property.
*/
#define wxPG_DESCRIPTION            0x00002000

/** wxPropertyGridManager only: Show compactor button that toggles hidden
    state of low-priority properties.
*/
#define wxPG_COMPACTOR              0x00004000

/** Speeds up switching to wxPG_HIDE_CATEGORIES mode. Initially, if wxPG_HIDE_CATEGORIES
    is not defined, the non-categorized data storage is not activated, and switching
    the mode first time becomes somewhat slower. wxPG_EX_INIT_NOCAT activates the
    non-categorized data storage right away. IMPORTANT NOTE: If you do plan not
    switching to non-categoric mode, or if you don't plan to use categories at
    all, then using this style will result in waste of resources.
*/
#define wxPG_EX_INIT_NOCAT          0x00001000

/** Extended window style that sets wxPropertyGridManager toolbar to not
    use flat style.
*/
#define wxPG_EX_NO_FLAT_TOOLBAR     0x00002000

/** Classic spacing uses spacing that most resembles .NET propertygrid.
*/
#define wxPG_EX_CLASSIC_SPACING     0x00004000

/** Hides alphabetic/categoric mode buttons from toolbar.
*/
#define wxPG_EX_NO_MODE_BUTTONS     0x00008000

/** Combines various styles.
*/
#define wxPG_DEFAULT_STYLE	        (wxNO_FULL_REPAINT_ON_RESIZE)

/** Combines various styles.
*/
#define wxPGMAN_DEFAULT_STYLE	    (wxNO_FULL_REPAINT_ON_RESIZE)

/** @}
*/


//
// Valid constants for wxPG_UINT_BASE attribute
// (long because of wxVariant constructor)
#define wxPG_BASE_OCT                       (long)8
#define wxPG_BASE_DEC                       (long)10
#define wxPG_BASE_HEX                       (long)16
#define wxPG_BASE_HEXL                      (long)32

//
// Valid constants for wxPG_UINT_PREFIX attribute
#define wxPG_PREFIX_NONE                    (long)0
#define wxPG_PREFIX_0x                      (long)1
#define wxPG_PREFIX_DOLLAR_SIGN             (long)2


/** Flags for wxPropertyGrid::GetPropertyValues and wxPropertyGridManager::GetPropertyValues. */
#ifndef wxKEEP_STRUCTURE
# define wxKEEP_STRUCTURE               0x00000010
#endif

/** Flags for wxPropertyGrid::SetPropertyAttribute etc */
#ifndef wxRECURSE
# define wxRECURSE                      0x00000020
#endif

// -----------------------------------------------------------------------

// Property priorities
#define wxPG_LOW                    1
#define wxPG_HIGH                   2

// -----------------------------------------------------------------------

// Misc argument flags.
#define wxPG_FULL_VALUE             0x00000001 // Get/Store full value instead of displayed value.
#define wxPG_REPORT_ERROR           0x00000002

// -----------------------------------------------------------------------

// Simple class to hold the wxPGProperty pointer.
class WXDLLIMPEXP_PG wxPGId
{
public:
    inline wxPGId() { m_ptr = (wxPGProperty*) NULL; }
    inline wxPGId ( wxPGProperty* ptr ) { m_ptr = ptr; }
    inline wxPGId ( wxPGProperty& ref ) { m_ptr = &ref; }
    ~wxPGId() {}

    bool IsOk () const { return ( m_ptr != NULL ); }

    bool operator == (const wxPGId& other)
    {
        return m_ptr == other.m_ptr;
    }

    operator wxPGProperty* ()
    {
        return m_ptr;
    }

    wxPGProperty& GetProperty () const { return *m_ptr; }
    wxPGProperty* GetPropertyPtr () const { return m_ptr; }
private:
    wxPGProperty* m_ptr;
};

#define wxPGIdGen(PTR) wxPGId(PTR)
#define wxPGIdToPtr(ID) ID.GetPropertyPtr()
#define wxPGIdIsOk(ID) ID.IsOk()

// -----------------------------------------------------------------------

WXDLLIMPEXP_PG void wxPGTypeOperationFailed ( const wxPGProperty* p, const wxChar* typestr, const wxChar* op );
WXDLLIMPEXP_PG void wxPGGetFailed ( const wxPGProperty* p, const wxChar* typestr );

// -----------------------------------------------------------------------

/*
#define wxPG_CURSOR_FROM_FILE       0xFFFFFE

#define wxPG_CURSOR_FROM_RESOURCE   0xFFFFFF
*/

// -----------------------------------------------------------------------

/** @defgroup propflags wxPGProperty Flags
    @{
*/

// NOTE: Do not change order of these, and if you add
//   any, remember also to update gs_property_flag_to_string
//   in propgrid.cpp.

/** Indicates bold font.
*/
#define wxPG_PROP_MODIFIED          0x0001

/** Disables ('greyed' text and editor does not activate) property.
*/
#define wxPG_PROP_DISABLED          0x0002

/** Hider button will hide this property.
*/
#define wxPG_PROP_HIDEABLE          0x0004

/** This property has custom paint image just in front of its value.
    If property only draws custom images into a popup list, then this
    flag should not be set.
*/
#define wxPG_PROP_CUSTOMIMAGE       0x0008

/** Do not create text based editor for this property (but button-triggered
    dialog and choice are ok).
*/
#define wxPG_PROP_NOEDITOR          0x0010

/** Value is unspecified.
*/
#define wxPG_PROP_UNSPECIFIED       0x0020

/** Indicates the bit useable by derived properties.
*/
#define wxPG_PROP_CLASS_SPECIFIC_1  0x0040

/** Indicates the bit useable by derived properties.
*/
#define wxPG_PROP_CLASS_SPECIFIC_2  0x0080

/** @}
*/

// -----------------------------------------------------------------------

/** @defgroup attrids wxPropertyGrid Property Attribute Identifiers
    wxPropertyGrid::SetPropertyAttribute accepts one of these as
    attrid argument when used with one of the built-in property classes.
    @{
*/

/** wxBoolProperty specific, int, default 0. When 1 sets bool property to
    use checkbox instead of choice.
*/
#define wxPG_BOOL_USE_CHECKBOX              64

/** wxBoolProperty specific, int, default 0. When 1 sets bool property value
    to cycle on double click (instead of showing the popup listbox).
*/
#define wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING  65

/** wxFloatProperty (and similar) specific, int, default -1. Sets the (max) precision
    used when floating point value is rendered as text. The default -1 means infinite
    precision.
*/
#define wxPG_FLOAT_PRECISION                66

/** wxFileProperty/wxImageFileProperty specific, wxChar*, default is detected/varies.
    Sets the wildcard used in the triggered wxFileDialog. Format is the
    same.
*/
#define wxPG_FILE_WILDCARD                  67

/** wxFileProperty/wxImageFileProperty specific, int, default 1.
    When 0, only the file name is shown (i.e. drive and directory are hidden).
*/
#define wxPG_FILE_SHOW_FULL_PATH            68

/** Define base used by a wxUIntProperty. Valid constants are
    wxPG_BASE_OCT, wxPG_BASE_DEC, wxPG_BASE_HEX and wxPG_BASE_HEXL
    (lowercase characters).
*/
#define wxPG_UINT_BASE                      69

/** Define prefix rendered to wxUIntProperty. Accepted constants
    wxPG_PREFIX_NONE, wxPG_PREFIX_0x, and wxPG_PREFIX_DOLLAR_SIGN.
    <b>Note:</b> Only wxPG_PREFIX_NONE works with Decimal and Octal
    numbers.
*/
#define wxPG_UINT_PREFIX                    70

/** wxCustomProperty specific, wxPGEditor*. Set editor control. Editor pointer is stored
    in variable named wxPGEditor_EDITORNAME. So built-in editors are at
    wxPGEditor_TextCtrl, wxPGEditor_Choice, wxPGEditor_CheckBox,
    wxPGEditor_TextCtrlAndButton, and wxPGEditor_ChoiceAndButton.
*/
#define wxPG_CUSTOM_EDITOR                  128

/** wxCustomProperty specific, wxBitmap*. Sets a small bitmap. Value must be given as
    pointer and it is then copied. If you give it wxNullBitmap, then the current
    image (if any) is deleted.
*/
#define wxPG_CUSTOM_IMAGE                   129

/** wxCustomProperty specific, void*. Sets callback function (of type wxPropertyGridCallback)
    that is called whenever button is pressed.
*/
#define wxPG_CUSTOM_CALLBACK                130

/** wxCustomProperty specific, void*. Sets callback function (of type wxPGPaintCallback)
    that is called whenever image in front of property needs to be repainted. This attribute
    takes precedence over bitmap set with wxPG_CUSTOM_IMAGE, and it is only proper way
    to draw images to wxCustomProperty's drop down choices list.
    \remarks
    Callback must handle measure calls (i.e. when rect.x < 0, set paintdata.m_drawnHeight to
    height of item in question).
*/
#define wxPG_CUSTOM_PAINT_CALLBACK          131

/** wxCustomProperty specific, int, default 0. Setting to 1 makes children private, similar to other
    properties with children.
    \remarks
    - Children must be added <b>when this attribute has value 0</b>. Otherwise
      there will be an assertion failure.
*/
#define wxPG_CUSTOM_PRIVATE_CHILDREN        132

/** First attribute id that is guaranteed not to be used built-in
    properties.
*/
#define wxPG_USER_ATTRIBUTE                 192

/** @}
*/

// -----------------------------------------------------------------------
// Value type.

// Value type declarer, with optional declaration part (with creator function).
#define WX_PG_DECLARE_VALUE_TYPE_WITH_DECL(VALUETYPE,DECL) \
    extern DECL const wxPGValueType *wxPGValueType_##VALUETYPE; \
    extern DECL wxPGValueType* wxPGNewVT##VALUETYPE();

// Value type declarer (with creator function).
#define WX_PG_DECLARE_VALUE_TYPE(VALUETYPE) \
    extern const wxPGValueType *wxPGValueType_##VALUETYPE; \
    wxPGValueType* wxPGNewVT##VALUETYPE();

// Value type declarer, with optional declaration part.
#define WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(VALUETYPE,DECL) \
    extern DECL const wxPGValueType *wxPGValueType_##VALUETYPE;

// Value type accessor.
#define wxPG_VALUETYPE(T)       wxPGValueType_##T

// Like wxPG_VALUETYPE, but casts pointer to exact class.
#define wxPG_VALUETYPE_EXACT(T) ((wxPGValueType##VALUETYPE##Class)wxPGValueType##T)

// Declare builtin value types.
WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(none,WXDLLIMPEXP_PG)
WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(wxString,WXDLLIMPEXP_PG)
WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(long,WXDLLIMPEXP_PG)
WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(bool,WXDLLIMPEXP_PG)
WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(double,WXDLLIMPEXP_PG)
WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(void,WXDLLIMPEXP_PG)
WX_PG_DECLARE_VALUE_TYPE_BUILTIN_WITH_DECL(wxArrayString,WXDLLIMPEXP_PG)

#define WX_PG_DECLARE_VALUE_TYPE_VDC(VALUETYPE) \
wxVariantData_##VALUETYPE : public wxPGVariantDataWxObj \
{ \
    DECLARE_DYNAMIC_CLASS(wxVariantData_##VALUETYPE) \
protected: \
    VALUETYPE   m_value; \
public: \
    wxVariantData_##VALUETYPE(); \
    wxVariantData_##VALUETYPE(const VALUETYPE& value); \
    virtual void Copy(wxVariantData& data); \
    virtual bool Eq(wxVariantData& data) const; \
    virtual wxString GetType() const; \
    virtual void* GetValuePtr(); \
    inline const VALUETYPE& GetValue () const { return m_value; }

// Value type declarer for void* that need auto-generated .
#define WX_PG_DECLARE_VALUE_TYPE_VOIDP_WITH_DECL(VALUETYPE,DECL) \
class DECL WX_PG_DECLARE_VALUE_TYPE_VDC(VALUETYPE) \
}; \
extern DECL const wxPGValueType *wxPGValueType_##VALUETYPE;

#define WX_PG_DECLARE_VALUE_TYPE_VOIDP(VALUETYPE) \
class WX_PG_DECLARE_VALUE_TYPE_VDC(VALUETYPE) \
}; \
WX_PG_DECLARE_VALUE_TYPE(VALUETYPE)

/** \class wxPGVariantDataWxObj
    \ingroup classes
    \brief Identical to wxVariantDataWxObjectPtr except that it deletes the
       ptr on destruction.
*/
class WXDLLIMPEXP_PG wxPGVariantDataWxObj : public wxVariantData
{
public:
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
    virtual bool Read(wxString& str);

    virtual void* GetValuePtr() = 0;
};

// -----------------------------------------------------------------------
// Editor class.

// Editor accessor.
#define wxPG_EDITOR(T)          wxPGEditor_##T

// Declare editor class, with optional part.
#define WX_PG_DECLARE_EDITOR_CLASS_WITH_DECL(EDITOR,DECL) \
extern DECL wxPGEditor* wxPGEditor_##EDITOR; \
extern DECL wxPGEditor* wxPGConstruct##EDITOR##EditorClass();

// Declare editor class.
#define WX_PG_DECLARE_EDITOR_CLASS(EDITOR) \
extern wxPGEditor* wxPGEditor_##EDITOR; \
extern wxPGEditor* wxPGConstruct##EDITOR##EditorClass();

// Declare builtin editor classes.
WX_PG_DECLARE_EDITOR_CLASS_WITH_DECL(TextCtrl,WXDLLIMPEXP_PG)
WX_PG_DECLARE_EDITOR_CLASS_WITH_DECL(Choice,WXDLLIMPEXP_PG)
WX_PG_DECLARE_EDITOR_CLASS_WITH_DECL(TextCtrlAndButton,WXDLLIMPEXP_PG)
#if wxPG_INCLUDE_CHECKBOX
WX_PG_DECLARE_EDITOR_CLASS_WITH_DECL(CheckBox,WXDLLIMPEXP_PG)
#endif
WX_PG_DECLARE_EDITOR_CLASS_WITH_DECL(ChoiceAndButton,WXDLLIMPEXP_PG)

// -----------------------------------------------------------------------

/** \class wxPGValueType
	\ingroup classes
    \brief wxPGValueType is base class for property value types.
*/

class WXDLLIMPEXP_PG wxPGValueType
{
public:

    virtual ~wxPGValueType() = 0;

    /** Returns type name. If there is wxVariantData for this type, then name should
    be the same that the class uses (otherwise wxT("void*")). */
    virtual const wxChar* GetTypeName() const = 0;

    /** Returns custom type name. If this is base for a type, should not be overridden,
        as the default implementation already does good thing and calls GetTypeName.
        Otherwise, should be an unique string, such as the class name etc.
    */
    virtual const wxChar* GetCustomTypeName() const;

    /** Returns default value.
    */
    virtual wxPGVariant GetDefaultValue () const = 0;

    /** Creates wxVariant with supplied value and name.
    */
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const = 0;

    /** Creates new property instance with "proper" class. Initial value is set
        to default.
    */
    virtual wxPGProperty* GenerateProperty ( const wxString& label, const wxString& name ) const = 0;

    /** Sets property value from wxVariant.
    */
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const = 0;

    /** Returns type that can be passed to CreatePropertyByType.
    */
    inline const wxChar* GetType() const
    {
        return GetCustomTypeName();
    }

protected:
};

// -----------------------------------------------------------------------

#if wxPG_USE_VALIDATORS

/** \class wxPropertyValidator
    \ingroup Classes
    \brief Classes derived from this one can be used to validate values
    edited for a property.

    wxIntPropertyValidator and wxFloatPropertyValidator are built-in for
    setting minimum and maximum values for a property that has matching
    value type.

    Example of use:

    \code

        // Limit value to -100,100 range.
        wxIntPropertyValidator int_validator(-100,100);

        wxPGId pid = pg->Append ( wxIntProperty( wxT("Value (-100 - 100)"), wxPG_LABEL ) );

        pg->SetPropertyValidator ( pid, int_validator );

    \endcode

    Also, if property is of array type, it can use validator if type of
    all its entries match type handled by a validator (so, for example,
    sample wxArrayDoubleProperty can use wxFloatPropertyValidator to
    validate on per-item basis).

    Note that validators can only check if value is bad - they cannot
    change it. Also, if you do a custom property class take note that
    properties do not use validators automatically. See following
    source code (in propgrid.cpp and propgridsample.cpp) for example
    of implementation:
    wxStringPropertyClass::SetValueFromString:
    wxIntPropertyClass::SetValueFromString:
    wxFloatPropertyClass::SetValueFromString:
    wxArrayDoublePropertyClass::SetValueFromString

    As can be noted from the method location of validation (i.e.
    not in DoSetValue), value is not checked when set programmatically
    (if you really need this, then make a feature request).

*/
class WXDLLIMPEXP_PG wxPropertyValidator
{
public:
    wxPropertyValidator();
    virtual ~wxPropertyValidator();

    /** Must be implemented to create clone of this object.
    */
    virtual wxPropertyValidator* Clone() const = 0;

    /** Validates value. If not valid, will be modified to best
        possible value.
        \param value
        Value to be validated.
        Beware, this is not typesafe nor persistent variant, but should do
        since custom properties that need to use this are not common.
        \param showmsg
        Fill this if you want to show a custom error message (shown usually
        as a tooltip or statusbar text).
        \retval
        Returns TRUE if validation was succesfull (i.e. value is ok).
    */
    virtual bool Validate ( wxPGVariant& value, wxString& showmsg ) const = 0;

#ifdef __WXDEBUG__
    virtual void AssertDataType ( const wxChar* typestr ) const;
#else
    /** Not necessary to override. Virtual out-of-inline method in debug mode only
        (__WXDEBUG__ defined). Does not do anything in release mode. When implemented,
        must cause assertion failure if type given as argument is not supported.
    */
    inline void AssertDataType ( const wxChar* ) { }
#endif

    /** For reference counting (no need to call in user code).
    */
    wxPropertyValidator* Ref();

    /** For reference counting (no need to call in user code).
    */
    bool UnRef();

private:
    wxPropertyValidator* m_refObject;
    int m_refCount;
};

/** \class wxStringPropertyValidator
    \ingroup Classes
    \brief Use as validator for wxStringProperty (and maybe some others,
    such as wxArrayStringProperty).
    Implements character exclusion.
*/
class WXDLLIMPEXP_PG wxStringPropertyValidator : public wxPropertyValidator
{
public:
    /** \param excludeList
        List of characters that are not allowed.
    */
    wxStringPropertyValidator( const wxString& excludeList );
    virtual ~wxStringPropertyValidator();

    virtual wxPropertyValidator* Clone() const;
    virtual bool Validate ( wxPGVariant& value, wxString& showmsg ) const;
#ifdef __WXDEBUG__
    virtual void AssertDataType ( const wxChar* typestr ) const;
#endif

protected:
    wxString m_excludeList;
};

/** \class wxIntPropertyValidator
    \ingroup Classes
    \brief Use as validator for wxIntProperty (and maybe some others).
    Implements minimum and maximum value checking.
*/
class WXDLLIMPEXP_PG wxIntPropertyValidator : public wxPropertyValidator
{
public:
    wxIntPropertyValidator( long min, long max );
    virtual ~wxIntPropertyValidator();

    virtual wxPropertyValidator* Clone() const;
    virtual bool Validate ( wxPGVariant& value, wxString& showmsg ) const;
#ifdef __WXDEBUG__
    virtual void AssertDataType ( const wxChar* typestr ) const;
#endif

protected:
    long m_min, m_max;
};

/** \class wxFloatPropertyValidator
    \ingroup Classes
    \brief Use as validator for wxFloatProperty (and maybe some others).
    Implements minimum and maximum value checking.
*/
class WXDLLIMPEXP_PG wxFloatPropertyValidator : public wxPropertyValidator
{
public:
    wxFloatPropertyValidator( double min, double max );
    virtual ~wxFloatPropertyValidator();

    virtual wxPropertyValidator* Clone() const;
    virtual bool Validate ( wxPGVariant& value, wxString& showmsg ) const;
#ifdef __WXDEBUG__
    virtual void AssertDataType ( const wxChar* typestr ) const;
#endif

protected:
    double m_min, m_max;
};

#endif

// -----------------------------------------------------------------------

union wxPGVariantUnion
{
    long        m_long;
    void*       m_ptr;
    bool        m_bool;
};

// Very simple value wrapper.
class wxPGVariant
{
public:

    /** Constructor for none. */
    wxPGVariant ()
    {
        m_v.m_ptr = (void*)NULL;
    }
#ifndef SWIG
    /** Constructor for long integer. */
    wxPGVariant ( long v_long )
    {
        m_v.m_long = v_long;
    }
    /** Constructor for integer. */
    wxPGVariant ( int v_long )
    {
        m_v.m_long = v_long;
    }
    /** Constructor for bool. */
    wxPGVariant ( bool value )
    {
        m_v.m_bool = value;
    }
    /** Constructor for float. */
    wxPGVariant ( const double& v_ptr )
    {
        m_v.m_ptr = (void*)&v_ptr;
    }
    /** Constructor for wxString*. */
    wxPGVariant ( const wxString& v_ptr )
    {
        m_v.m_ptr = (void*)&v_ptr;
    }
    /** Constructor for wxArrayString*. */
    wxPGVariant ( const wxArrayString& v_ptr )
    {
        m_v.m_ptr = (void*)&v_ptr;
    }
    /** Constructor for wxObject&. */
    wxPGVariant ( const wxObject& v_ptr )
    {
        m_v.m_ptr = (void*)&v_ptr;
    }
    /** Constructor for wxObject*. */
    wxPGVariant ( const wxObject* v_ptr )
    {
        m_v.m_ptr = (void*)v_ptr;
    }
    /** Constructor for void*. */
    wxPGVariant ( void* v_ptr )
    {
        m_v.m_ptr = v_ptr;
    }

    /** Returns value as long integer. */
    inline long GetLong () const
    {
        return m_v.m_long;
    }
    /** Returns value as boolean integer. */
    inline bool GetBool () const
    {
        return m_v.m_bool;
    }
    /** Returns value as floating point number. */
    inline double GetDouble () const
    {
        return *((double*)m_v.m_ptr);
    }
    /** Returns value as floating point number ptr. */
    inline double* GetDoublePtr () const
    {
        return (double*) m_v.m_ptr;
    }
    /** Returns value as a wxString. */
    inline const wxString& GetString () const
    {
        return *((const wxString*)m_v.m_ptr);
    }
    /** Returns value as a reference to a wxArrayString. */
    inline wxArrayString& GetArrayString () const
    {
        wxArrayString* temp = (wxArrayString*)m_v.m_ptr;
        return *temp;
    }

    inline const wxObject& GetWxObject() const
    {
        return *((const wxObject*)m_v.m_ptr);
    }

    inline wxObject* GetWxObjectPtr() const
    {
        return (wxObject*)m_v.m_ptr;
    }

    /** Returns value as void*. */
    inline void* GetVoidPtr () const
    {
        return m_v.m_ptr;
    }
#endif

    /** Returns value as long integer without type checking. */
    inline long GetRawLong () const { return m_v.m_long; }

    /** Returns value as void* without type checking. */
    inline void* GetRawPtr () const { return m_v.m_ptr; }

#undef wxPG_ASSERT_VARIANT_GET

    /** Value in portable format. */
    wxPGVariantUnion          m_v;
};

#define wxPGVariantToString(A)      A.GetString()
#define wxPGVariantToLong(A)        A.GetLong()
#define wxPGVariantToBool(A)        A.GetBool()
#define wxPGVariantToDouble(A)      A.GetDouble()
#define wxPGVariantToArrayString(A) A.GetArrayString()
#define wxPGVariantToWxObject(A)        A.GetWxObject()
#define wxPGVariantToWxObjectPtr(A,B)   wxDynamicCast((wxObject*)A.GetRawPtr(),B);
#define wxPGVariantToVoidPtr(A)     A.GetVoidPtr()

#define wxPGVariantFromString(A)        A
#define wxPGVariantFromLong(A)          A
#define wxPGVariantFromDouble(A)        A
#define wxPGVariantFromArrayString(A)   A
#define wxPGVariantFromBool(A)          A
#define wxPGVariantFromWxObject(A)      *((const wxObject*)A)

// -----------------------------------------------------------------------

//
// Property class declaration helper macros
// (wxPGRootPropertyClass and wxPropertyCategory require this).
//

#define WX_PG_DECLARE_PROPERTY_CLASS() \
public: \
    virtual const wxPGValueType* GetValueType () const; \
    virtual const wxPGEditor* GetEditorClass () const; \
    WX_PG_DECLARE_GETCLASSNAME() \
private:

// -----------------------------------------------------------------------
// wxPGPropertyClassInfo


typedef wxPGProperty* (*wxPGPropertyConstructor) (const wxString&,const wxString&);

/** \class wxPGPropertyClassInfo
	\ingroup classes
    \brief Class info structure for wxPGProperty derivatives (may be deprecated
    in a future release).
*/
struct wxPGPropertyClassInfo
{
    // One returned by GetPropertyClassName
    const wxChar*               m_name;

    // Simple property constructor function.
    wxPGPropertyConstructor     m_constructor;
};


// Use this macro to register your custom property classes.
#define wxPGRegisterPropertyClass(NAME) \
    wxPropertyGrid::RegisterPropertyClass(wxT(#NAME),&NAME##ClassInfo)


// -----------------------------------------------------------------------


// Structure for relaying choice/list info.
struct wxPGChoiceInfo
{
    const wxChar**  m_arrWxChars;
    wxString*       m_arrWxString;
    wxPGConstants** m_constants;
    int             m_itemCount;
};


/** \class wxPGProperty
	\ingroup classes
    \brief wxPGProperty is base class for properties. Information here is
    provided primarily for anyone who creates new properties, since <b>all operations
    on properties should be done via wxPropertyGrid's or wxPropertyGridManager's
    methods</b>.

    \remarks
    - When changing name of a property, it is essential to use wxPropertyGrid::SetPropertyName
      (that's why there is no SetName method).
*/

#if wxPG_EMBED_VARIANT
class WXDLLIMPEXP_PG wxPGProperty : public wxVariant
#elif wxPG_INCLUDE_WXOBJECT
class WXDLLIMPEXP_PG wxPGProperty : public wxObject
#else
class WXDLLIMPEXP_PG wxPGProperty
#endif
{
    friend class wxPGPropertyWithChildren;
    friend class wxPropertyGrid;
    friend class wxPropertyGridState;
    //_WX_PG_DECLARE_PROPERTY_CLASS(wxPGProperty)
public:

    /** Basic constructor. Should not be necessary to override.
    */
    wxPGProperty();

    /** Constructor.
        Real used property classes should have constructor of this style:

        \code

        // If MyValueType is a class, then it should be a constant reference
        // (e.g. const MyValueType& ) instead.
        wxMyProperty ( const wxString& label, const wxString& name,
            MyValueType value ) : wxPGProperty
        {
            // Only required if MyValueType is not built-in default
            // (wxString, long, double, bool, and wxArrayString are;
            // wxFont, wxColour, etc. are not).
            wxPG_INIT_REQUIRED_TYPE(MyValueType)
            DoSetValue(value); // Generally recommended way to set the initial value.

            // If has child properties (i.e. wxPGPropertyWithChildren is used
            // as the parent class), then create children here. For example:
            //     AddChild( new wxStringProperty( wxT("Subprop 1"), wxPG_LABEL, value.GetSubProp1() ) );
        }

        \endcode

        Of course, in this example, wxPGProperty could also be wxPGPropertyWithChildren
        (if it has sub-properties) or actually any other property class.
    */
    wxPGProperty( const wxString& label, const wxString& name );

    /** Virtual destructor. It is customary for derived properties to override this. */
    virtual ~wxPGProperty();

    /** Sets property's internal value.
        \param value
        Simple container with GetString(), GetLong() etc. methods. Currently recommended
        means to extract value is to use wxPGVariantToXXX(value) macro.
        \remarks
        Example pseudo-implementation with comments:
        \code

        void wxMyProperty::DoSetValue ( wxPGVariant value )
        {
            // A) Get value. For example
            const wxMyValueType* pvalue = wxPGVariantToWxObjectPtr(value,wxMyValueType);
            //    or:
            const wxString& str = wxPGVariantToString(value);
            //    or:
            long val = wxPGVariantToLong(value);

            // B) If value is wxObject or void based with NULL default, then handle that:
            if ( pvalue )
                m_value = *pvalue;
            else
                pmyvalue->SetToDefault();

            // Otherwise
            // m_value = *pvalue;
            // is sufficient.

            // C) If has children, this should be here (before displaying in control).
            RefreshChildren();

        }

        \endcode
    */
    virtual void DoSetValue ( wxPGVariant value );

    /** Returns properly constructed wxPGVariant.
    */
    virtual wxPGVariant DoGetValue () const;

#if wxPG_EMBED_VARIANT
    /** Returns value as reference to a wxVariant.
    */
    inline const wxVariant& GetValueAsVariant () const
    {
        return *this;
    }
#else
    /** Returns value as wxVariant.
    */
    inline wxVariant GetValueAsVariant () const;
#endif

    /** Returns text representation of property's value.
        \param arg_flags
        If wxPG_FULL_VALUE is set, returns complete, storable value instead of displayable
        one (they may be different).
    */
    virtual wxString GetValueAsString ( int arg_flags = 0 ) const;

    /** Converts string to a value, and if succesfull, calls DoSetValue() on it.
        Default behaviour is to do nothing.
        \param text
        String to get the value from.
        \param report_error
        If TRUE, invalid string will be reported (prefer to use wxLogError).
        \retval
        TRUE if value was changed.
    */
    virtual bool SetValueFromString ( const wxString& text, int flags = 0 );

    /** Converts integer to a value, and if succesfull, calls DoSetValue() on it.
        Default behaviour is to do nothing.
        \param value
        Int to get the value from.
        \param flags
        If has wxPG_FULL_VALUE, then the value given is a actual value and not an index.
        \retval
        TRUE if value was changed.
    */
    virtual bool SetValueFromInt ( long value, int flags = 0 );

    /** Returns size of the custom paint image in front of property.
        Returning -1 for either dimension means using default value.
        Default behaviour is to return wxSize(0,0), which means no image.
    */
    virtual wxSize GetImageSize() const;

    /** Events received by editor widgets are processed here. Note that editor class
        usually processes most events. Some, such as button press events of
        TextCtrlAndButton class, should be handled here. Also, if custom handling
        for regular events is desired, then that can also be done (for example,
        wxSystemColourProperty custom handles wxEVT_COMMAND_CHOICE_SELECTED).
        \param event
        Associated wxEvent. This may be NULL, which indicates focus is leaving control.
        Property should get the edited value from wnd_primary and store it to the
        internals.
        \retval
        Should return TRUE if any changes in value should be reported. This is case,
        for example, when enter is pressed in wxTextCtrl.\n
        parent->EditorsValueWasModified() should be called on any change,
        regardless if it is not large enough to be reported. This is case,
        for example, when text in wxTextCtrl changes.
        \remarks
        Note that event is pointer, not a reference.
    */
    virtual bool OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* wnd_primary, wxEvent& event );

#if wxPG_INCLUDE_WXOBJECT
    inline const wxChar* GetClassName () const
    {
        return GetClassInfo()->GetClassName();
    }
#else
    /** Returns classname (for example, "wxStringProperty" for wxStringProperty)
        of a property class.
    */
    virtual const wxChar* GetClassName () const = 0;
#endif

    /** Returns string that identified the type of property's value.
        Since pointer-level identification is used for basic types,
        wxPGTypeXXX string pointers should be used as often as possible.
        Rest should return CLASSINFO(valueclass)->GetClassName().
        \sa @link typestrings wxPGTypeXXX string pointers @endlink
    */

    /** Returns pointer to the object that has methods related to
        the value type of this property. Keep atleast this method
        abstract so the class is kept abstract.
    */
    virtual const wxPGValueType* GetValueType () const = 0;

    /** Returns pointer to an instance of editor class.
    */
    virtual const wxPGEditor* GetEditorClass () const;

    /** Returns 0 for normal items. 1 for categories, -1 for other properties with children,
        -2 for wxCustomProperty (mostly like -1 ones but with few expections).
        \remarks
        Should not be overridden by new custom properties.
    */
    //virtual int GetParentingType() const;
    inline signed char GetParentingType() const { return m_parentingType; }

    /** Returns current value's index to the choice control. May also return,
        through pointer arguments, strings that should be inserted to that control.
        Irrelevant to classes which do not employ wxPG_EDITOR(Choice) or similar.
        \remarks
        - If returns -1 in choiceinfo->m_itemCount, then in that case this
          class be derived from wxBaseEnumPropertyClass (see propdev.h) and
          GetEntry is used to fill an array (slower, but handier if you don't
          store your labels as arrays of strings).
        - Must not crash even if property's set of choices is uninitialized
          (i.e. it points to wxPGGlobalVars->m_emptyConstants).
    */
    virtual int GetChoiceInfo ( wxPGChoiceInfo* choiceinfo );

    /** Override to paint an image in front of the property value text or drop-down
        list item.
        If property's GetImageSize() returns size that has height != 0 but less than
        row height ( < 0 has special meanings), wxPropertyGrid calls this method to
        draw a custom image in a limited area in front of the editor control or
        value text/graphics, and if control has drop-down list, then the image is
        drawn there as well (even in the case GetImageSize() returned higher height
        than row height).

        NOTE: Following applies when GetImageSize() returns a "flexible" height (
        using wxPG_FLEXIBLE_SIZE(W,H) macro), which implies variable height items:
        If rect.x is < 0, then this is a measure item call, which means that
        dc is invalid and only thing that should be done is to set paintdata.m_drawnHeight
        to the height of the image of item at index paintdata.m_choiceItem. This call
        may be done even as often as once every drop-down popup show.
        \param dc
        wxDC to paint on.
        \param rect
        Box reserved for custom graphics. Includes surrounding rectangle, if any.
        If x is < 0, then this is a measure item call (see above).
        \param paintdata
        wxPGPaintData structure with much useful data.
        \remarks
            - You can actually exceed rect width, but if you do so then paintdata.m_drawnWidth
              must be set to the full width drawn in pixels.
            - Due to technical reasons, rect's height will be default even if custom height
              was reported during measure call.
            - Font and text colour should not be modified.
            - Brush is guaranteed to be default background colour. It has been already used to
              clear the background of area being painted. It can be modified.
            - Pen is guaranteed to be 1-wide 'black' (or whatever is the proper colour) pen for
              drawing framing rectangle. It can be changed as well.
        \sa @link GetValueAsString @endlink
    */
    virtual void OnCustomPaint ( wxDC& dc,
        const wxRect& rect, wxPGPaintData& paintdata );

    /** Sets an attribute of this property. This is quite property class specific,
        and there are no common attributes. Note that attribute may be specific
        to a property instance, or it may affect all properties of its class.
        \param id
        Identifier of attribute
        \param value
        Value for that attribute.
    */
    virtual void SetAttribute ( int id, wxVariant value );

    /** Returns property's label. */
    inline const wxString& GetLabel() const { return m_label; }

    /** Returns wxPropertyGridState in which this property belongs. */
    wxPropertyGridState* GetParentState() const;

#if wxPG_EMBED_VARIANT
    /** Returns property's name (alternate way to access property). */
    //inline const wxString& GetName() const { return wxVariant::GetName(); }
    inline void DoSetName(const wxString& str) { SetName(str); }
#else
    /** Returns property's name (alternate way to access property). */
    inline const wxString& GetName() const { return m_name; }
    inline void DoSetName(const wxString& str) { m_name = str; }
#endif

    /** Gets pre-calculated top y coordinate of property graphics.
        This cannot be relied on all times (wxPropertyGrid knows when :) ),
        and value is -1 if property is not visible.
    */
    inline int GetY() const { return m_y; }

    void UpdateControl ( wxPGCtrlClass* primary );

    inline wxString GetDisplayedString () const
    {
        return GetValueAsString(0);
    }

    /** Returns property id. */
    inline wxPGId GetId() { return wxPGIdGen(this); }

    /** Return parent of property */
    inline wxPGPropertyWithChildren* GetParent() const { return m_parent; }

    /** Returns true if property is valid and wxPropertyGrid methods
        can operate on it safely.
    */
    inline bool IsOk() const
    {
        return (( m_y >= -1 )?TRUE:FALSE);
    }

    inline bool IsFlagSet( unsigned char flag ) const
    {
        return ( m_flags & flag ) ? TRUE : FALSE;
    }

    /** Returns true if extra children can be added for this property
        (i.e. it is wxPropertyCategory or wxCustomProperty)
    */
    inline bool CanHaveExtraChildren() const
    {
        return ( m_parentingType == 1 || m_parentingType == -2 );
    }

    inline unsigned int GetFlags() const
    {
        return (unsigned int)m_flags;
    }

    /** Returns type name of property that is compatible with CreatePropertyByType.
        and wxVariant::GetType.
    */
    inline const wxChar* GetType() const
    {
        return GetValueType()->GetType();
    }

    /** Returns TRUE if this is a sub-property. */
    inline bool IsSubProperty() const
    {
        wxPGProperty* parent = (wxPGProperty*)m_parent;
        if ( parent && parent->GetParentingType() < 0 && parent->m_y > -2 )
            return TRUE;
        return FALSE;
    }

    /** Returns number of children (always 0 for normal properties). */
    size_t GetChildCount() const;

    inline unsigned int GetArrIndex() const { return m_arrIndex; }

    inline unsigned int GetDepth() const { return (unsigned int)m_depth; }

    /** Returns position in parent's array. */
    inline unsigned int GetIndexInParent() const
    {
        return (unsigned int)m_arrIndex;
    }

    /** If property has choices and they are not yet private, new such copy
        of them will be created.
    */
    void SetChoicesPrivate();

    inline void SetFlag( unsigned char flag ) { m_flags |= flag; }

    inline void SetHelpString ( const wxString& help_string ) { m_helpString = help_string; }

    inline void SetLabel( const wxString& label ) { m_label = label; }

#if wxPG_USE_VALIDATORS
    /** Sets wxValidator for a property*/
    void SetValidator ( wxPropertyValidator& validator );

    /** Gets assignable version of property's validator. */
    wxPropertyValidator& GetValidator () const;

    /** Does std validation process and if ok calls DoSetValue,
        otherwise shows the reported validation error.
    */
    bool StdValidationProcedure( wxPGVariant value );

#else

    inline bool StdValidationProcedure( wxPGVariant value )
    {
        DoSetValue( value );
        return TRUE;
    }

#endif

    /** Updates property value in case there were last minute
        changes. If value was unspecified, it will be set to default.
        Use only for properties that have TextCtrl-based editor.
        \remarks
        If you have code similar to
        \code
            // Update the value in case of last minute changes
            if ( primary && propgrid->IsEditorsValueModified() )
                 GetEditorClass()->CopyValueFromControl( this, primary );
        \endcode
        in wxPGProperty::OnEvent wxEVT_COMMAND_BUTTON_CLICKED handler,
        then replace it with call to this method.
        \retval
        True if value changed.
    */
    bool PrepareValueForDialogEditing( wxPropertyGrid* propgrid );

#if wxPG_USE_CLIENT_DATA
    inline void* GetClientData () const { return m_clientData; }
    inline void SetClientData ( void* clientData ) { m_clientData = clientData; }
#endif

    /** If is non-zero and found, then set choices to refer to that.
        If not, then creates new set of choices with given id.
        Returns id of created wxPGConstants.
    */
    size_t SetChoices ( size_t id, const wxArrayString& labels, const wxArrayInt& values );

    inline const wxString& GetHelpString () const { return m_helpString; }

    inline void ClearFlag( unsigned char flag ) { m_flags &= ~(flag); }

    // Use, for example, to detect if item is inside collapsed section.
    bool IsSomeParent ( wxPGProperty* candidate_parent ) const;

    // Shows error as a tooltip or something similar (depends on platform).
    void ShowError ( const wxString& msg );

protected:

    // Called in constructors.
    void Init ();

    wxString                    m_label;
//#if wxPG_EMBED_VARIANT
//    wxVariant                   m_variant;
//#else
    wxString                    m_name;
    wxPGPropertyWithChildren*   m_parent;
    wxString                    m_helpString; // Help shown in statusbar or help box.
#if wxPG_USE_CLIENT_DATA
    void*                       m_clientData;
#endif
#if wxPG_USE_VALIDATORS
    wxPropertyValidator*        m_validator;
#endif
    unsigned int                m_arrIndex; // Index in parent.
    int                         m_y; // This could be short int.
    unsigned char               m_flags; // This could be short int.

    // 1 = category
    // 0 = no children
    // -1 = has fixed-set of sub-properties
    // -2 = this is wxCustomProperty (sub-properties can be added)
    signed char                 m_parentingType;

    unsigned char               m_depth; // Root has 0, categories etc. at that level 1, etc.

    // m_depthBgCol indicates width of background colour between margin and item
    // (essentially this is category's depth, if none then equals m_depth).
    unsigned char               m_depthBgCol;

    unsigned char               m_bgColIndex; // Cell background brush index.

};

//
// wxPGId comparison operators.
// TODO: Are these really used?
//

inline bool operator==(const wxPGId& id, const wxString& b)
{
    wxASSERT (wxPGIdIsOk(id));
    const wxString& a = id.GetProperty().GetName();
    return (a.Len() == b.Len()) && (a.Cmp(b) == 0);
}

inline bool operator==(const wxPGId& id, const wxChar* b)
{
    wxASSERT (wxPGIdIsOk(id));
    return id.GetProperty().GetName().Cmp(b) == 0;
}


// For dual-pointer-usage reasons, we need to use this trickery
// instead of wxObjArray. wxPGValueType hash map is declared
// in propdev.h.
typedef wxArrayPtrVoid wxPGArrayProperty;

// It was reported that the alternative does not work with GCC 4.1
#if wxUSE_UNICODE || wxCHECK_GCC_VERSION(4,0) || wxUSE_STL

// Always use wxString based hashmap with unicode
WX_DECLARE_STRING_HASH_MAP_WITH_DECL(void*,
                                     wxPGHashMapS2P,
                                     class WXDLLIMPEXP_PG);

# define wxPGNameStr            const wxString&
# define wxPGNameConv(STR)      STR

#else

WX_DECLARE_HASH_MAP_WITH_DECL(wxChar*,     // type of the keys
                              void*,                 // type of the values
                              wxStringHash,          // hasher
                              wxStringEqual,         // key equality predicate
                              wxPGHashMapS2P,       // name of the class
                              class WXDLLIMPEXP_PG);

# define wxPGNameStr         const wxChar*       // Should be same as hashmap type
# define wxPGNameConv(STR)   ((char * const)STR.c_str())

#endif

// -----------------------------------------------------------------------

// Hash map for int-to-int mapping
WX_DECLARE_HASH_MAP_WITH_DECL(size_t,
                              size_t,
                              wxIntegerHash,
                              wxIntegerEqual,
                              wxPGHashMapI2I,
                              class WXDLLIMPEXP_PG);

// -----------------------------------------------------------------------

#if !wxPG_EMBED_VARIANT
inline wxVariant wxPGProperty::GetValueAsVariant () const
{
    wxPGVariant value = DoGetValue();
    const wxPGValueType* typeclass = GetValueType();
    wxASSERT_MSG ( typeclass, wxT("Did you forgot to use wxPG_INIT_REQUIRED_TYPE(T) in constructor?") );
    return typeclass->GenerateVariant(value,m_name);
}
#endif

// -----------------------------------------------------------------------

/** \class wxPGPropertyWithChildren
    \ingroup classes
    \brief Base class for new properties that have sub-properties. For
    example, wxFontProperty and wxFlagsProperty descend from this class.
*/
class WXDLLIMPEXP_PG wxPGPropertyWithChildren : public wxPGProperty
{
    friend class wxPGProperty;
    friend class wxPropertyGridState;
    friend class wxPropertyGrid;
public:

    /** Special constructor only used in special cases. */
    wxPGPropertyWithChildren();

    /** When new class is derived, call this constructor.
        \param label
        Label for the property.
    */
    wxPGPropertyWithChildren( const wxString& label, const wxString& name );

    /** Destructor. */
    virtual ~wxPGPropertyWithChildren();

    //virtual int GetParentingType() const;

    /** Advanced variant of GetValueAsString() that forms a string that
        contains sequence of text representations of sub-properties.
    */
    // Advanced version that gives property list and index to this item
    virtual wxString GetValueAsString ( int arg_flags = 0 ) const;

    /** This overridden version converts comma or semicolon separated
        tokens into child values.
    */
    virtual bool SetValueFromString ( const wxString& text, int flags );

    /** Refresh values of child properties.
    */
    virtual void RefreshChildren();

    /** Called after child property p has been altered.
        The value of this parent property should now be updated accordingly.
    */
    virtual void ChildChanged ( wxPGProperty* p );

    /** Returns number of sub-properties. */
    inline size_t GetCount() const { return m_children.GetCount(); }

    /** Returns sub-property at index i. */
    inline wxPGProperty* Item ( size_t i ) const { return (wxPGProperty*)m_children.Item(i); }

    /** Returns last sub-property. */
    inline wxPGProperty* Last () const { return (wxPGProperty*)m_children.Last(); }

    /** Returns index of given sub-property. */
    inline int Index ( const wxPGProperty* p ) const { return m_children.Index((void*)p); }

    /** Deletes all sub-properties. */
    void Empty();

    inline bool IsExpanded() const
    {
        return ( m_expanded > 0 ) ? TRUE : FALSE;
    }

    wxPropertyGridState* GetParentState() const { return m_parentState; }

    // Puts correct indexes to children
    void FixIndexesOfChildren ( size_t starthere = 0 );

    wxPGProperty* GetItemAtY ( unsigned int y, unsigned int lh );

    void AddChild ( wxPGProperty* prop, int index = -1, bool correct_mode = TRUE );

    inline void SetParentState ( wxPropertyGridState* pstate ) { m_parentState = pstate; }

protected:

    wxPropertyGridState*    m_parentState;

    wxPGArrayProperty   m_children;
    unsigned char       m_expanded;
};


// -----------------------------------------------------------------------

/** \class wxPGRootPropertyClass
    \ingroup classes
    \brief Root parent property.
*/
class WXDLLIMPEXP_PG wxPGRootPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    /** Constructor. */
    wxPGRootPropertyClass();
    virtual ~wxPGRootPropertyClass();

    /** Override this to return 1 (just in case). */
    //virtual int GetParentingType() const;

protected:
};

// -----------------------------------------------------------------------

/** \class wxPropertyCategoryClass
    \ingroup classes
    \brief Category property.
*/
class WXDLLIMPEXP_PG wxPropertyCategoryClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    /** Special constructor only used in special cases. */
    wxPropertyCategoryClass();

    /** Construct.
        \param label
        Label for the category.
        \remarks
        All non-category properties appended will have most recently
        added category.
    */
    wxPropertyCategoryClass( const wxString& label, const wxString& name = wxPG_LABEL );
    ~wxPropertyCategoryClass();

    /** Must be overridden with function that doesn't do anything. */
    virtual wxString GetValueAsString ( int arg_flags ) const;

    //virtual int GetParentingType() const;

    inline int GetTextExtent() const { return m_textExtent; }

    void CalculateTextExtent ( wxWindow* wnd, wxFont& font );

protected:
    int m_textExtent; // pre-calculated length of text
};

// -----------------------------------------------------------------------

// Used to indicate wxPGConstants::Add etc that the value shall not be added
#define wxPG_INVALID_VALUE      2147483647

/** \class wxPGConstants
    \ingroup classes
    \brief Helper class for managing constant (key=value) sequences.
*/
class WXDLLIMPEXP_PG wxPGConstants
{
public:

    /** Basic constructor. */
    wxPGConstants();

    /** Constructor. */
    wxPGConstants( const wxChar** labels, const long* values = NULL, unsigned int itemcount = 0 );

    /** Constructor. */
    wxPGConstants( const wxArrayString& labels, const wxArrayInt& values = *((const wxArrayInt*)NULL) );

    /** Adds to current. If did not have own copies, creates them now. If was empty,
        identical to set except that creates copies.
    */
    void Add ( const wxChar** labels, const long* values = NULL, unsigned int itemcount = 0 );

    /** Version that works with wxArrayString. */
    void Add ( const wxArrayString& arr, const long* values = NULL );

    /** Version that works with wxArrayString and wxArrayInt. */
    void Add ( const wxArrayString& arr, const wxArrayInt& arrint );

    /** Adds single item. */
    void Add ( const wxChar* label, int value = wxPG_INVALID_VALUE );

    /** Returns reference to wxArrayString of labels for you to modify.
    */
    inline wxArrayString& GetLabels()
    {
        wxASSERT ( m_refCount != 0xFFFFFFF );
        return m_arrLabels;
    }

    /** Returns reference to wxArrayInt of values for you to modify.
    */
    inline wxArrayInt& GetValues()
    {
        wxASSERT ( m_refCount != 0xFFFFFFF );
        return m_arrValues;
    }

    /** Returns false if this is a constant empty set of choices,
        which should not be modified.
    */
    bool IsOk () const
    {
        return ( m_refCount != 0xFFFFFFF );
    }

    /** Returns TRUE if this is a temporary in-stack set of choices.
    */
    bool IsTemporary () const
    {
        return ( m_refCount == 0 );
    }

    /** Gets a unsigned number identifying this list. */
    size_t GetId() const { return m_id; };

    /** Does not create copies for itself. */
    void Set ( const wxChar** labels, const long* values = NULL, unsigned int itemcount = 0 )
    {
        Free();
        Add(labels,values,itemcount);
    }

    /** Version that works with wxArrayString.
        TODO: Deprecate this.
    */
    void Set ( wxArrayString& arr, const long* values = (const long*) NULL )
    {
        Free();
        Add(arr,values);
    }

    /** Version that works with wxArrayString and wxArrayInt. */
    void Set ( const wxArrayString& labels, const wxArrayInt& values = *((const wxArrayInt*)NULL) )
    {
        Free();
        if ( &values )
            Add(labels,values);
        else
            Add(labels);
    }

    /** If previous id was valid (i.e. non-zero), then clears it.
        Then sets this wxPGConstants to new id.
    */
    void SetId( size_t id );

    void SetupId( size_t id ) { m_id = id; }

    inline const wxString& GetLabel( size_t ind )
    {
        return m_arrLabels[ind];
    }
    inline const wxArrayString& GetLabels() const { return m_arrLabels; }

    inline size_t GetCount () { return m_arrLabels.GetCount(); }

    inline int GetValue( size_t ind ) { return m_arrValues[ind]; }
    inline const wxArrayInt& GetValues() const { return m_arrValues; }

    inline int GetRefCount() const { return m_refCount; }
    inline void SetRefCount(int refcount) { m_refCount = refcount; }
    inline void Ref()
    {
        wxASSERT ( m_refCount != 0xFFFFFFF );
        m_refCount++;
        /*
        const wxChar* l1 = wxT("<no_items>");
        if ( GetCount() )
            l1 = m_arrLabels[0].c_str();
        wxLogDebug(wxT("%X: %s... Ref: refcount is now %i"),(unsigned int)this,l1,(int)m_refCount);
        */
    }
    inline bool UnRef()
    {
        wxASSERT ( m_refCount != 0xFFFFFFF );
        m_refCount--;

        /*
        const wxChar* l1 = wxT("<no_items>");
        if ( GetCount() )
            l1 = m_arrLabels[0].c_str();
        wxLogDebug(wxT("%X: %s... UnRef: refcount is now %i"),(unsigned int)this,l1,(int)m_refCount);
        */

        return ( m_refCount < 1 );
    }

    inline void AddString( const wxString& str ) { m_arrLabels.Add(str); }
    inline void AddInt( int val ) { m_arrValues.Add(val); }

    inline void SetLabels( wxArrayString& arr ) { m_arrLabels = arr; }
    inline void SetLabels( const wxArrayString& arr ) { m_arrLabels = arr; }
    inline void SetValues( wxArrayInt& arr ) { m_arrValues = arr; }
    inline void SetValues( const wxArrayInt& arr ) { m_arrValues = arr; }

    /** Destructor. */
    ~wxPGConstants();

protected:

    wxArrayString   m_arrLabels;
    wxArrayInt      m_arrValues;

    size_t          m_id; // usually pointer to a wxChar* array

#ifdef __WXDEBUG__
public:
    // For pairing validation
    const long*     m_origValueArray;
protected:
#endif

    // So that multiple properties can use the same set
    int             m_refCount;

    void Init ()
    {
        m_id = 0;
        m_refCount = 0;
    }
    void Free ();

};

// -----------------------------------------------------------------------
// Property declaration.

#define wxPG_CONSTFUNC(PROP) PROP
#define wxPG_PROPCLASS(PROP) PROP##Class

// Macro based constructor.
#define wxPG_NEWPROPERTY(PROP,LABEL,NAME,VALUE) wx##PROP##Property(LABEL,NAME,VALUE)

#define wxPG_DECLARE_PROPERTY_CLASSINFO(NAME) \
    extern wxPGPropertyClassInfo NAME##ClassInfo;

#define wxPG_DECLARE_PROPERTY_CLASSINFO_WITH_DECL(NAME,DECL) \
    extern DECL wxPGPropertyClassInfo NAME##ClassInfo;

#define WX_PG_DECLARE_PROPERTY_WITH_DECL(NAME,VALARG,DEFVAL,DECL) \
    extern DECL wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, VALARG = DEFVAL ); \
    extern DECL wxPGPropertyClassInfo NAME##ClassInfo;

#define WX_PG_DECLARE_PROPERTY(NAME,VALARG,DEFVAL) \
    extern wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, VALARG = DEFVAL ); \
    wxPG_DECLARE_PROPERTY_CLASSINFO(NAME)

//
// Specific macro-based declarations.
//

#define WX_PG_DECLARE_STRING_PROPERTY_WITH_DECL(NAME,DECL) \
extern DECL wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxString& = wxEmptyString ); \
extern DECL wxPGPropertyClassInfo NAME##ClassInfo;

#define WX_PG_DECLARE_STRING_PROPERTY(NAME) \
extern wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxString& = wxEmptyString ); \
wxPG_DECLARE_PROPERTY_CLASSINFO(NAME)

#define WX_PG_DECLARE_CUSTOM_FLAGS_PROPERTY_WITH_DECL(NAME,DECL) \
WX_PG_DECLARE_PROPERTY_WITH_DECL(NAME,long,-1,DECL)

#define WX_PG_DECLARE_CUSTOM_FLAGS_PROPERTY(NAME) \
WX_PG_DECLARE_PROPERTY(NAME,long,-1)

#define WX_PG_DECLARE_CUSTOM_ENUM_PROPERTY_WITH_DECL(NAME,DECL) \
WX_PG_DECLARE_PROPERTY_WITH_DECL(NAME,int,-1,DECL)

#define WX_PG_DECLARE_CUSTOM_ENUM_PROPERTY(NAME) \
WX_PG_DECLARE_PROPERTY(NAME,int,-1)

#define WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(NAME,DECL) \
extern DECL wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxArrayString& = wxArrayString() ); \
extern DECL wxPGPropertyClassInfo NAME##ClassInfo;

#define WX_PG_DECLARE_ARRAYSTRING_PROPERTY(NAME) \
extern wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxArrayString& = wxArrayString() ); \
wxPG_DECLARE_PROPERTY_CLASSINFO(NAME)

// Declare basic property classes.
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxStringProperty,const wxString&,wxEmptyString,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxIntProperty,long,0,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxUIntProperty,unsigned long,0,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxFloatProperty,double,0.0,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxBoolProperty,bool,FALSE,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxLongStringProperty,const wxString&,wxEmptyString,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxFileProperty,const wxString&,wxEmptyString,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxArrayStringProperty,const wxArrayString&,wxArrayString(),WXDLLIMPEXP_PG)

WX_PG_DECLARE_STRING_PROPERTY_WITH_DECL(wxDirProperty,WXDLLIMPEXP_PG)

// Enum and Flags Properties require special attention.
extern WXDLLIMPEXP_PG wxPGProperty* wxEnumProperty( const wxString&, const wxString&, const wxChar** labels = (const wxChar**) NULL,
    const long* values = NULL, unsigned int itemcount = 0, int value = 0 );
extern WXDLLIMPEXP_PG wxPGProperty* wxEnumProperty( const wxString& label, const wxString& name,
    wxPGConstants& constants, int value = 0 );
extern WXDLLIMPEXP_PG wxPGPropertyClassInfo wxEnumPropertyClassInfo;

extern WXDLLIMPEXP_PG wxPGProperty* wxFlagsProperty( const wxString&, const wxString&, const wxChar** labels = (const wxChar**) NULL,
    const long* values = NULL, unsigned int itemcount = 0, int value = 0 );
extern WXDLLIMPEXP_PG wxPGProperty* wxFlagsProperty( const wxString& label, const wxString& name,
    wxPGConstants& constants, int value = 0 );
extern WXDLLIMPEXP_PG wxPGPropertyClassInfo wxFlagsPropertyClassInfo;

// wxCustomProperty doesn't have value argument.
extern WXDLLIMPEXP_PG wxPGProperty* wxCustomProperty( const wxString& label, const wxString& name = wxPG_LABEL );
extern WXDLLIMPEXP_PG wxPGPropertyClassInfo wxCustomPropertyClassInfo;

// wxParentProperty doesn't have value argument.
extern WXDLLIMPEXP_PG wxPGProperty* wxParentProperty( const wxString& label, const wxString& name );
extern WXDLLIMPEXP_PG wxPGPropertyClassInfo wxParentPropertyClassInfo;

// wxPropertyCategory doesn't have value argument.
extern WXDLLIMPEXP_PG wxPGProperty* wxPropertyCategory ( const wxString& label, const wxString& name = wxPG_LABEL );
extern WXDLLIMPEXP_PG wxPGPropertyClassInfo wxPropertyCategoryClassInfo;

// FIXME: Should this be out-of-inline?
inline wxObject* wxPG_VariantToWxObject( wxVariant& variant, wxClassInfo* classInfo )
{
    if ( !variant.IsValueKindOf(classInfo) )
        return (wxObject*) NULL;
    wxVariantData* vdata = variant.GetData();

    if ( vdata->IsKindOf( &wxPGVariantDataWxObj::ms_classInfo ) )
         return (wxObject*) ((wxPGVariantDataWxObj*)vdata)->GetValuePtr();

    return variant.GetWxObjectPtr();
}

//
// Redefine wxGetVariantCast to also take propertygrid variantdata
// classes into account.
// TODO: Remove after persistent wxObject classes added (i.e.
//   GetWxObjectPtr works for all).
//
#undef wxGetVariantCast
#define wxGetVariantCast(var,classname) (classname*)wxPG_VariantToWxObject(var,&classname::ms_classInfo)

// TODO: After a while, remove this.
#define WX_PG_VARIANT_TO_WXOBJECT(VARIANT,CLASSNAME) (CLASSNAME*)wxPG_VariantToWxObject(VARIANT,&CLASSNAME::ms_classInfo)
//#define WX_PG_VARIANT_TO_WXOBJECT(VARIANT,CLASSNAME) wxGetVariantCast(VARIANT,CLASSNAME)

// -----------------------------------------------------------------------

/** \class wxPropertyGridState
	\ingroup classes
    \brief
    Contains information of a single wxPropertyGrid page.
*/
// BM_STATE
class WXDLLIMPEXP_PG wxPropertyGridState
{
    friend class wxPGProperty;
    friend class wxPropertyGrid;
    friend class wxPropertyGridManager;
public:

    /** Constructor. */
    wxPropertyGridState();

    /** Destructor. */
    ~wxPropertyGridState();

    /** Base append. */
    wxPGId Append ( wxPGProperty* property );

    wxPGId AppendIn ( wxPGPropertyWithChildren* pwc, const wxString& label, const wxString& propname, wxVariant& value );

    /** Returns property by its name. */
    wxPGId BaseGetPropertyByName ( wxPGNameStr name ) const;

    /** Called in, for example, wxPropertyGrid::Clear. */
    void Clear ();

    void ClearModifiedStatus ( wxPGProperty* p );

    static void ClearPropertyAndChildrenFlags ( wxPGProperty* p, long flags );
    static void SetPropertyAndChildrenFlags ( wxPGProperty* p, long flags );

    bool ClearPropertyValue ( wxPGProperty* p );

    bool Collapse ( wxPGProperty* p );

    void Delete ( wxPGProperty* item );

    bool EnableCategories ( bool enable );

    /** Enables or disables given property and its subproperties. */
    bool EnableProperty ( wxPGProperty* p, bool enable );

    bool Expand ( wxPGProperty* p );

    void ExpandAll ( unsigned char do_expand );

    /** Returns id of first item, whether it is a category or property. */
    inline wxPGId GetFirst () const
    {
        wxPGProperty* p = (wxPGProperty*) NULL;
        if ( m_properties->GetCount() )
            p = m_properties->Item(0);
        return wxPGIdGen(p);
    }

    wxPGId GetFirstCategory () const;

    wxPGId GetFirstProperty () const;

    wxPropertyGrid* GetGrid () const { return m_pPropGrid; }

    wxPGId GetNextCategory ( wxPGId id ) const;

    wxPGId GetNextProperty ( wxPGId id ) const;

    static wxPGId GetNextSibling ( wxPGId id );

    static wxPGId GetPrevSibling ( wxPGId id );

    wxPGId GetPrevProperty ( wxPGId id ) const;

    wxPGId GetPropertyByLabel ( const wxString& name, wxPGPropertyWithChildren* parent  = (wxPGPropertyWithChildren*) NULL ) const;

    wxVariant GetPropertyValues ( const wxString& listname, wxPGId baseparent, long flags ) const;

    /** Base insert. */
    wxPGId Insert ( wxPGPropertyWithChildren* parent, int index, wxPGProperty* property );

    inline bool IsDisplayed() const;

    inline bool IsInNonCatMode() const { return (bool)(m_properties == m_abcArray); }

    /** Only inits arrays, doesn't migrate things or such. */
    void InitNonCatMode ();

    void LimitPropertyEditing ( wxPGProperty* p, bool limit = TRUE );

    wxPGId ReplaceProperty( wxPGProperty* replaced, wxPGProperty* property );

    void SetPropertyLabel( wxPGProperty* p, const wxString& newlabel );

    void SetPropertyPriority ( wxPGProperty* p, int priority );

    void SetPropVal( wxPGProperty* p, wxPGVariant value );

    bool SetPropertyValue ( wxPGProperty* p, const wxPGValueType* typeclass, wxPGVariant value );

    bool SetPropertyValue ( wxPGProperty* p, const wxChar* typestring, wxPGVariant value );

    bool SetPropertyValue ( wxPGProperty* p, const wxString& value );

    bool SetPropertyValue ( wxPGProperty* p, wxVariant& value );

    bool SetPropertyValue ( wxPGProperty* p, wxObject* value );

    /** Sets value (long integer) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, long value )
    {
        SetPropertyValue ( p, wxPG_VALUETYPE(long), wxPGVariantFromLong(value) );
    }
    /** Sets value (integer) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, int value )
    {
        SetPropertyValue ( p, wxPG_VALUETYPE(long), wxPGVariantFromLong((long)value) );
    }
    /** Sets value (floating point) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, double value )
    {
        SetPropertyValue ( p, wxPG_VALUETYPE(double), wxPGVariantFromDouble(value) );
    }
    /** Sets value (bool) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, bool value )
    {
        SetPropertyValue ( p, wxPG_VALUETYPE(bool), wxPGVariantFromLong(value?1:0) );
    }
    /** Sets value (wxArrayString) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, const wxArrayString& value )
    {
        SetPropertyValue ( p, wxPG_VALUETYPE(wxArrayString), wxPGVariantFromArrayString(value) );
    }
    /** Sets value (void*) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, void* value )
    {
        SetPropertyValue ( p, wxPG_VALUETYPE(void), value );
    }
    /** Sets value (wxPoint&) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, const wxPoint& value )
    {
        wxASSERT ( p );
        //wxASSERT ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),) == 0 );
        //SetPropertyValue ( p, wxPG_VALUETYPE(void), (void*)&value );
        SetPropertyValue ( p, wxT("wxPoint"), (void*)&value );
    }
    /** Sets value (wxSize&) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, const wxSize& value )
    {
        wxASSERT ( p );
        //wxASSERT ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),wxT("wxSize")) == 0 );
        //SetPropertyValue ( p, wxPG_VALUETYPE(void), (void*)&value );
        SetPropertyValue ( p, wxT("wxSize"), (void*)&value );
    }
    /** Sets value (wxArrayInt&) of a property. */
    inline void SetPropertyValue ( wxPGProperty* p, const wxArrayInt& value )
    {
        wxASSERT ( p );
        //wxASSERT ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),wxT("wxArrayInt")) == 0 );
        //SetPropertyValue ( p, wxPG_VALUETYPE(void), (void*)&value );
        SetPropertyValue ( p, wxT("wxArrayInt"), (void*)&value );
    }

    void SetPropertyValues ( const wxList& list, wxPGId default_category );

    void SetPropertyValueUnspecified ( wxPGProperty* p );

    void Sort ( wxPGProperty* p );
    void Sort ();

#ifdef _WX_WINDOW_H_BASE_
protected:

    int PrepareToAddItem ( wxPGProperty* property, wxPGPropertyWithChildren* scheduled_parent );

    /** If visible, then this is pointer to wxPropertyGrid. Otherwise NULL. */
    wxPropertyGrid*             m_pPropGrid;

    /** Pointer to currently used array. */
    wxPGPropertyWithChildren*   m_properties;

    /** Array for categoric mode. */
    wxPGRootPropertyClass           m_regularArray;

    /** Array for root of non-categoric mode. */
    wxPGRootPropertyClass*          m_abcArray;

    /** Dictionary for name-based access. */
    wxPGHashMapS2P                  m_dictName;

    /** Most recently added category. */
    wxPropertyCategoryClass*         m_currentCategory;

    /** Pointer to selected property. */
    wxPGProperty*               m_selected;

    /** 1 if m_lastCaption is also the bottommost caption. */
    unsigned char               m_lastCaptionBottomnest;
    /** 1 items appended/inserted, so stuff needs to be done before drawing;
        If m_bottomy == 0, then calcylatey's must be done.
        Otherwise just sort.
    */
    unsigned char               m_itemsAdded;

    /** 1 if any value is modified. */
    unsigned char               m_anyModified;

#endif
};

// -----------------------------------------------------------------------

/** \class wxPropertyContainerMethods
    \ingroup classes
    \brief In order to have most same base methods, both wxPropertyGrid and
    wxPropertyGridManager must derive from this.
*/
class WXDLLIMPEXP_PG wxPropertyContainerMethods
// BM_METHODS
{
public:

    // needed because this class has virtual functions
    virtual ~wxPropertyContainerMethods(){}

    /** Adds choice to a property that can accept one.
        \remarks
        - If you need to make sure that you modify only the set of choices of
          a single property (and not also choices of other properties with initially
          identical set), call wxPropertyGrid::SetPropertyChoicesPrivate.
        - This usually only works for wxEnumProperty and derivatives (wxFlagsProperty
          can get accept new items but its items may not get updated).
        - If property modified is selected, the editor control will not get updated.
    */
    void AddPropertyChoice( wxPGId id, const wxString& label, int value = wxPG_INVALID_VALUE );
    inline void AddPropertyChoice( wxPGNameStr name, const wxString& label, int value = wxPG_INVALID_VALUE )
    {
        AddPropertyChoice(GetPropertyByName(name),label,value);
    }

    /** Constructs a property. Class used is given as the first
        string argument. It may be either normal property class
        name, such as "wxIntProperty" or a short one such as
        "Int".
    */
    static wxPGProperty* CreatePropertyByClass(const wxString &classname,
                                               const wxString &label,
                                               const wxString &name);

    /** Constructs a property. Value type name used is given as the first
        string argument. It may be "string", "long", etc. Any value returned
        by wxVariant::GetType fits there.

        Otherwise, this is similar as CreatePropertyByClass.
        \remarks
        <b>Cannot</b> generate property category.
    */
    static wxPGProperty* CreatePropertyByType(const wxString &valuetype,
                                              const wxString &label,
                                              const wxString &name);

    /** Returns next item under the same parent. */
    inline wxPGId GetNextSibling ( wxPGId id )
    {
        return wxPropertyGridState::GetNextSibling(id);
    }
    inline wxPGId GetNextSibling ( wxPGNameStr name )
    {
        return wxPropertyGridState::GetNextSibling(GetPropertyByName(name));
    }

    /** Returns comma-delimited string with property's attributes (both
        pseudo-attributes such as "Disabled" and "Modified" and real
        attributes such as "BoolUseCheckbox" - actual names may vary).
        \param flagmask
        Combination of property flags that should be included (in addition
        to any other attributes). For example, to avoid adding Modified
        attribute use ~(wxPG_PROP_MODIFIED).
        \remarks
        Atleast in 1.0.0rc this does not return complete list of attributes
        (for example, no floating point precision) and some attributes have
        generic names (such as "Special1" instead of "UseCheckbox" etc)
    */
    static wxString GetPropertyAttributes( wxPGId id, unsigned int flagmask = 0xFFFF );

    /** Sets attributes from a string generated by GetPropertyAttributes.
        \remarks
        Performance may not be top-notch.
    */
    static void SetPropertyAttributes( wxPGId id, const wxString& attributes );

    /** Returns id of property with given name (case-sensitive). If there is no
        property with such name, returned property id is invalid ( i.e. it will return
        FALSE with IsOk method).
    */
    wxPGId GetPropertyByName( wxPGNameStr name ) const;

    /** Returns writable reference to property's list of choices (and relevant
        values). If property does not have any choices, will return reference
        to an invalid set of choices that will return false on IsOk call.
    */
    wxPGConstants& GetPropertyChoices( wxPGId id );
    inline wxPGConstants& GetPropertyChoices( wxPGNameStr name )
    {
        return GetPropertyChoices(GetPropertyByName(name));
    }

    /** Gets name of property's constructor function. */
    inline const wxChar* GetPropertyClassName ( wxPGId id ) const
    {
        return wxPGIdToPtr(id)->GetClassName();
    }

    /** Gets name of property's constructor function. */
    inline const wxChar* GetPropertyClassName ( wxPGNameStr name ) const
    {
        return wxPGIdToPtr(GetPropertyByNameWithAssert(name))->GetClassName();
    }

#if wxPG_USE_CLIENT_DATA
    /** Returns client data (void*) of a property. */
    inline void* GetPropertyClientData ( wxPGId id ) const
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetClientData();
    }
    /** Returns client data (void*) of a property. */
    inline void* GetPropertyClientData ( wxPGNameStr name ) const
    {
        return GetPropertyClientData(GetPropertyByName(name));
    }
#endif

    /** Returns property's position under its parent. */
    inline unsigned int GetPropertyIndex ( wxPGId id )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetIndexInParent();
    }

    /** Returns property's position under its parent. */
    inline unsigned int GetPropertyIndex ( wxPGNameStr name )
    {
        return GetPropertyIndex ( GetPropertyByName(name) );
    }

    /** Returns label of a property. */
    inline const wxString& GetPropertyLabel ( wxPGId id )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetLabel();
    }
    inline const wxString& GetPropertyLabel ( wxPGNameStr name )
    {
        return GetPropertyLabel ( GetPropertyByName(name) );
    }

    /** Returns name of a property. */
    inline const wxString& GetPropertyName ( wxPGId id )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetName();
    }

    /** Returns parent item of a property. */
    inline wxPGId GetPropertyParent ( wxPGId id )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdGen(wxPGIdToPtr(id)->GetParent());
    }

    /** Returns parent item of a property. */
    inline wxPGId GetPropertyParent ( wxPGNameStr name )
    {
        return GetPropertyParent ( GetPropertyByName(name) );
    }

    /** Returns priority of a property (wxPG_HIGH or wxPG_LOW). */
    inline int GetPropertyPriority ( wxPGId id )
    {
        wxPGProperty* p = wxPGIdToPtr(id);
        if ( p && p->IsFlagSet(wxPG_PROP_HIDEABLE) )
            return wxPG_LOW;
        return wxPG_HIGH;
    }

    /** Returns priority of a property (wxPG_HIGH or wxPG_LOW). */
    inline int GetPropertyPriority( wxPGNameStr name )
    {
        return GetPropertyPriority(GetPropertyByNameWithAssert(name));
    }

    /** Returns help string associated with a property. */
    inline const wxString& GetPropertyHelpString ( wxPGId id ) const
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetHelpString();
    }

    /** Returns help string associated with a property. */
    inline const wxString& GetPropertyHelpString ( wxPGNameStr name ) const
    {
        return GetPropertyHelpString(GetPropertyByNameWithAssert(name));
    }

    /** Returns short name for property's class. For example,
        "wxPropertyCategory" translates to "Category" and "wxIntProperty"
        to "Int".
    */
    static wxString GetPropertyShortClassName( wxPGId id );

#if wxPG_USE_VALIDATORS
    /** Returns validator of a property as a reference, which you
        can pass to any number of SetPropertyValidator.
    */
    inline wxPropertyValidator& GetPropertyValidator ( wxPGId id )
    {
        wxASSERT( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetValidator();
    }
    inline wxPropertyValidator& GetPropertyValidator ( wxPGNameStr name )
    {
        return GetPropertyValidator(GetPropertyByName(name));
    }
#endif

    /** Returns value as wxVariant. To get wxObject pointer from it,
        you will have to use WX_PG_VARIANT_TO_WXOBJECT(VARIANT,CLASSNAME) macro.
    */
#if wxPG_EMBED_VARIANT
    inline const wxVariant& GetPropertyValue ( wxPGId id )
#else
    inline wxVariant GetPropertyValue ( wxPGId id )
#endif
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetValueAsVariant();
    }

    /** Returns value as wxVariant. To get wxObject pointer from it,
        you will have to use WX_PG_VARIANT_TO_WXOBJECT(VARIANT,CLASSNAME) macro.
    */
#if wxPG_EMBED_VARIANT
    inline const wxVariant& GetPropertyValue ( wxPGNameStr name )
#else
    inline wxVariant GetPropertyValue ( wxPGNameStr name )
#endif
    {
        wxPGId id = GetPropertyByName(name);
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetValueAsVariant();
    }

    static wxString GetPropertyValueAsString ( wxPGId id );
    static long GetPropertyValueAsLong ( wxPGId id );
    static inline int GetPropertyValueAsInt ( wxPGId id ) { return (int)GetPropertyValueAsLong(id); }
    static bool GetPropertyValueAsBool ( wxPGId id );
    static double GetPropertyValueAsDouble ( wxPGId id );
    static const wxArrayString& GetPropertyValueAsArrayString ( wxPGId id );
    static const wxObject* GetPropertyValueAsWxObjectPtr ( wxPGId id );
    static void* GetPropertyValueAsVoidPtr ( wxPGId id );

    static inline const wxPoint& GetPropertyValueAsPoint ( wxPGId id )
    {
        wxPGProperty* p = wxPGIdToPtr(id);
        wxASSERT ( p );

        if ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),wxT("wxPoint")) != 0 )
        {
            wxPGGetFailed(p,wxT("wxPoint"));
            return *((const wxPoint*)NULL);
        }
        return *((const wxPoint*)wxPGVariantToVoidPtr(p->DoGetValue()));
    }

    static inline const wxSize& GetPropertyValueAsSize ( wxPGId id )
    {
        wxPGProperty* p = wxPGIdToPtr(id);
        wxASSERT ( p );

        if ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),wxT("wxSize")) != 0 )
        {
            wxPGGetFailed(p,wxT("wxSize"));
            return *((const wxSize*)NULL);
        }
        return *((const wxSize*)wxPGVariantToVoidPtr(p->DoGetValue()));
    }

    static inline const wxArrayInt& GetPropertyValueAsArrayInt ( wxPGId id )
    {
        wxPGProperty* p = wxPGIdToPtr(id);
        wxASSERT ( p );

        if ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),wxT("wxArrayInt")) != 0 )
        {
            wxPGGetFailed(p,wxT("wxArrayInt"));
            return *((const wxArrayInt*)NULL);
        }
        return *((const wxArrayInt*)wxPGVariantToVoidPtr(p->DoGetValue()));
    }

    inline wxString GetPropertyValueAsString ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsString ( GetPropertyByName(name) );
    }
    inline long GetPropertyValueAsLong ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsLong ( GetPropertyByName(name) );
    }
    inline int GetPropertyValueAsInt ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsInt ( GetPropertyByName(name) );
    }
    inline bool GetPropertyValueAsBool ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsBool ( GetPropertyByName(name) );
    }
    inline double GetPropertyValueAsDouble ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsDouble ( GetPropertyByName(name) );
    }
    inline const wxArrayString& GetPropertyValueAsArrayString ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsArrayString ( GetPropertyByName(name) );
    }
    inline const wxObject* GetPropertyValueAsWxObjectPtr ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsWxObjectPtr ( GetPropertyByName(name) );
    }
    inline const wxPoint& GetPropertyValueAsPoint ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsPoint ( GetPropertyByName(name) );
    }
    inline const wxSize& GetPropertyValueAsSize ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsSize ( GetPropertyByName(name) );
    }
    inline const wxArrayInt& GetPropertyValueAsArrayInt ( wxPGNameStr name ) const
    {
        return GetPropertyValueAsArrayInt ( GetPropertyByName(name) );
    }

    /** Returns a wxPGValueType class instance that describes
        the property's data type.
    */
    const wxPGValueType* GetPropertyValueType ( wxPGId id )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetValueType();
    }
    const wxPGValueType* GetPropertyValueType ( wxPGNameStr name )
    {
        return wxPGIdToPtr( GetPropertyByNameWithAssert(name) )->GetValueType();
    }

    inline wxPropertyGridState* GetState() const { return m_pState; }

    /** Returns value type class fro given value type name.
    */
    static wxPGValueType* GetValueType( const wxString &type );

#if wxPG_INCLUDE_ADVPROPS
    /** Initializes *all* property types. Causes references to all property
        classes in the library, so calling this may cause significant increase
        in executable size when linking with static library.
    */
    static void InitAllTypeHandlers ();
#else
    static inline void InitAllTypeHandlers () { }
#endif

    /** Returns TRUE if property is enabled. */
    inline bool IsPropertyEnabled ( wxPGId id ) const
    {
        return (!(wxPGIdToPtr(id)->GetFlags() & wxPG_PROP_DISABLED))?TRUE:FALSE;
    }

    /** Returns TRUE if property is enabled. */
    inline bool IsPropertyEnabled ( wxPGNameStr name ) { return IsPropertyEnabled(GetPropertyByName(name)); }

    /** Returns true if property's value type has name typestr. */
    inline bool IsPropertyValueType ( wxPGId id, const wxChar* typestr )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return (wxStrcmp(wxPGIdToPtr(id)->GetValueType()->GetTypeName(),typestr) == 0);
    }

    /** Returns true if property's value type is valuetype */
    inline bool IsPropertyValueType ( wxPGId id, const wxPGValueType* valuetype )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return ( wxPGIdToPtr(id)->GetValueType() == valuetype );
    }

    /** Returns true if property's value type has same name as a class. */
    inline bool IsPropertyValueType ( wxPGId id, const wxClassInfo* classinfo )
    {
        return IsPropertyValueType(id,classinfo->GetClassName());
    }

    /** Returns true if property's value type has name typestr. */
    inline bool IsPropertyValueType ( wxPGNameStr name, const wxChar* typestr )
    {
        return IsPropertyValueType(GetPropertyByName(name),typestr);
    }

    /** Returns true if property's value type is valuetype */
    inline bool IsPropertyValueType ( wxPGNameStr name, const wxPGValueType* valuetype )
    {
        return IsPropertyValueType(GetPropertyByName(name),valuetype);
    }

    /** Returns true if property's value type has same name as a class. */
    inline bool IsPropertyValueType ( wxPGNameStr name, const wxClassInfo* classinfo )
    {
        return IsPropertyValueType(GetPropertyByName(name),classinfo);
    }

    /** Returns TRUE if given property is expanded. Naturally, always returns FALSE
        for properties that cannot be expanded.
    */
    static bool IsPropertyExpanded ( wxPGId id );
    inline bool IsPropertyExpanded ( wxPGNameStr name ) { return IsPropertyExpanded(GetPropertyByName(name)); }

    /** Returns TRUE if property has been modified after value set or modify flag
        clear by software.
    */
    inline bool IsModified ( wxPGId id ) const
    {
        wxPGProperty* p = wxPGIdToPtr ( id );
        if ( !p ) return FALSE;
        return ( (p->GetFlags() & wxPG_PROP_MODIFIED) ? TRUE : FALSE );
    }

    inline bool IsModified ( wxPGNameStr name ) { return IsModified(GetPropertyByName(name)); }

    /** Returns true if property is a category. */
    inline bool IsPropertyCategory ( wxPGId id ) const
    {
        return (wxPGIdToPtr(id)->GetParentingType()>0)?TRUE:FALSE;
    }
    inline bool IsPropertyCategory ( wxPGNameStr name ) { return IsPropertyCategory(GetPropertyByName(name)); }

    /** Returns TRUE if property has been modified after value set or modify flag
        clear by software.
    */
    inline bool IsPropertyModified ( wxPGId id ) const
    {
        wxPGProperty* p = wxPGIdToPtr(id);
        if ( !p ) return FALSE;
        return ( (p->GetFlags() & wxPG_PROP_MODIFIED) ? TRUE : FALSE );
    }
    inline bool IsPropertyModified ( wxPGNameStr name ) { return IsPropertyModified(GetPropertyByName(name)); }

    /** Returns TRUE if property value is set to unspecified.
    */
    inline bool IsPropertyValueUnspecified ( wxPGId id ) const
    {
        wxPGProperty* p = wxPGIdToPtr(id);
        if ( !p ) return FALSE;
        return ( (p->GetFlags() & wxPG_PROP_UNSPECIFIED) ? TRUE : FALSE );
    }
    inline bool IsPropertyValueUnspecified ( wxPGNameStr name ) { return IsPropertyValueUnspecified(GetPropertyByName(name)); }

    /** Basic property classes are registered by the default, but this
        registers advanced ones as well.
    */
    static void RegisterAdvancedPropertyClasses();

    /** Registers property class info with specific name. Preferably use
        wxPGRegisterPropertyClass(PROPERTYNAME) macro.
    */
    static bool RegisterPropertyClass ( const wxChar* name, wxPGPropertyClassInfo* classinfo );

    /** Replaces property with id with newly created property. For example,
        this code replaces existing property named "Flags" with one that
        will have different set of items:
        \code
            pg->ReplaceProperty(wxT("Flags"),
                wxFlagsProperty(wxT("Flags"),wxPG_LABEL,newItems))
        \endcode
        For more info, see wxPropertyGrid::Insert.
    */
    wxPGId ReplaceProperty( wxPGId id, wxPGProperty* property )
    {
        return m_pState->ReplaceProperty(id,property);
    }
    wxPGId ReplaceProperty( wxPGNameStr name, wxPGProperty* property )
    {
        return m_pState->ReplaceProperty(GetPropertyByName(name),property);
    }

    /** Lets user to set the strings listed in the choice dropdown of a wxBoolProperty.
        Defaults are "True" and "False", so changing them to, say, "Yes" and "No" may
        be useful in some less technical applications.
    */
    static void SetBoolChoices ( const wxChar* true_choice, const wxChar* false_choice );

    /** Set choices of a property to specified set of labels and values.
    */
    static void SetPropertyChoices(wxPGId id, wxPGConstants& choices);

    /** Set choices of a property to specified set of labels and values.
    */
    inline void SetPropertyChoices(wxPGNameStr name, wxPGConstants& choices)
    {
        SetPropertyChoices(GetPropertyByName(name),choices);
    }

    /** If property's set of choices is shared, then calling this method converts
        it to private.
    */
    inline void SetPropertyChoicesPrivate( wxPGId id )
    {
        wxASSERT( wxPGIdIsOk(id) );
        wxPGIdToPtr(id)->SetChoicesPrivate();
    }
    inline void SetPropertyChoicesPrivate( wxPGNameStr name )
    {
        wxPGId id = GetPropertyByName(name);
        wxASSERT( wxPGIdIsOk(id) );
        wxPGIdToPtr(id)->SetChoicesPrivate();
    }

    /** Sets an attribute of a property. Ids and relevants values are totally
        specific to property classes and may affect either the given instance
        or all instances of that class. See \ref attrids for list of built-in
        attributes.
        \param arg_flags
        Optional. Use wxRECURSE to set the attribute to child properties
        as well.
        \remarks
        wxVariant doesn't have int constructor (as of 2.5.4), so <b>you will
        need to cast int values (including most numeral constants) to long</b>.
    */
    static void SetPropertyAttribute ( wxPGId id, int attrid, wxVariant value, long arg_flags = 0 );
    inline void SetPropertyAttribute ( wxPGNameStr name, int attrid, wxVariant value, long arg_flags = 0  )
    {
        SetPropertyAttribute(GetPropertyByName(name),attrid,value,arg_flags);
    }

#if wxPG_USE_CLIENT_DATA
    /** Sets client data (void*) of a property. */
    inline void SetPropertyClientData ( wxPGId id, void* clientData )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        wxPGIdToPtr(id)->SetClientData(clientData);
    }
    /** Sets client data (void*) of a property. */
    inline void SetPropertyClientData ( wxPGNameStr name, void* clientData )
    {
        SetPropertyClientData(GetPropertyByName(name),clientData);
    }
#endif

    inline void SetPropertyHelpString ( wxPGId id, const wxString& help_string )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        wxPGIdToPtr(id)->SetHelpString(help_string);
    }

    inline void SetPropertyHelpString ( wxPGNameStr name, const wxString& help_string )
    {
        SetPropertyHelpString(GetPropertyByName(name),help_string);
    }

#if wxPG_USE_VALIDATORS
    /** Sets validator of a property. For example
        \code
          // Allow property's value range from -100 to 100
          wxIntPropertyValidator validator(-100,100);
          wxPGId id = pg->Append( wxIntProperty(wxT("Value 1",wxPG_LABEL,0)) );
          pg->SetPropertyValidator( id, validator );
        \endcode
    */
    inline void SetPropertyValidator ( wxPGId id, wxPropertyValidator& validator )
    {
        wxASSERT( wxPGIdIsOk(id) );
        wxPGIdToPtr(id)->SetValidator(validator);
    }
    inline void SetPropertyValidator ( wxPGNameStr name, wxPropertyValidator& validator )
    {
        SetPropertyValidator(GetPropertyByName(name),validator);
    }
#endif

protected:

    //virtual void DrawItemAndChildren( wxPGProperty* p ) = 0;

    virtual wxPGId DoGetPropertyByName( wxPGNameStr name ) const = 0;

    inline wxPGId GetPropertyByNameWithAssert ( wxPGNameStr name ) const
    {
        wxPGId id = GetPropertyByName(name);
        wxASSERT ( wxPGIdIsOk(id) );
        return id;
    }

    // Deriving classes must set this (it must be only or current page).
    wxPropertyGridState*         m_pState;

};

// -----------------------------------------------------------------------

// Internal flags
#define wxPG_FL_INITIALIZED                 0x0001
#define wxPG_FL_ACTIVATION_BY_CLICK         0x0002 // Set when creating editor controls if it was clicked on.
#define wxPG_FL_DONT_CENTER_SPLITTER        0x0004
#define wxPG_FL_FOCUSED                     0x0008
#define wxPG_FL_MOUSE_CAPTURED              0x0010
#define wxPG_FL_MOUSE_INSIDE                0x0020
#define wxPG_FL_VALUE_MODIFIED              0x0040
#define wxPG_FL_PRIMARY_FILLS_ENTIRE        0x0080 // don't clear background of m_wndPrimary
#define wxPG_FL_CUR_USES_CUSTOM_IMAGE       0x0100 // currently active editor uses custom image
#define wxPG_FL_HIDE_STATE                  0x0200 // set when hideable properties should be hidden
#define wxPG_FL_SCROLLED                    0x0400
#define wxPG_FL_ADDING_HIDEABLES            0x0800 // set when all added/inserted properties get hideable flag
#define wxPG_FL_NOSTATUSBARHELP             0x1000 // Disables showing help strings on statusbar.
#define wxPG_FL_CREATEDSTATE                0x2000 // Marks that we created the state, so we have to destroy it too.
#define wxPG_FL_SCROLLBAR_DETECTED          0x4000 // Set if scrollbar's existence was detected in last onresize.
#define wxPG_FL_DESC_REFRESH_REQUIRED       0x8000 // Set if wxPGMan requires redrawing of description text box.
#define wxPG_FL_SELECTED_IS_PAINT_FLEXIBLE  0x00010000 // Set if selected has flexible imagesize
#define wxPG_FL_IN_MANAGER                  0x00020000 // Set if contained in wxPropertyGridManager
#define wxPG_FL_GOOD_SIZE_SET               0x00040000 // Set after wxPropertyGrid is shown in its initial good size
#define wxPG_FL_IGNORE_NEXT_NAVKEY          0x00080000
#define wxPG_FL_NAVIGATING_OUT              0x00100000 // For nav key event to know whether to skip event or not
#define wxPG_FL_STRING_IN_STATUSBAR         0x00200000 // Set when help string is shown in status bar

// -----------------------------------------------------------------------

#define wxPG_USE_STATE  m_pState

/** \class wxPropertyGrid
	\ingroup classes
    \brief
    wxPropertyGrid is a specialized two-column grid for editing properties
    such as strings, numbers, flagsets, fonts, and colours. wxPropertySheet
    used to do the very same thing, but it hasn't been updated for a while
    and it is currently deprecated.

    wxPropertyGrid is modeled after .NET propertygrid (hence the name),
    and thus features are similar. However, inorder to keep the widget lightweight,
    it does not (and will not) have toolbar for mode and page selection, nor the help
    text box. wxAdvancedPropertyGrid (or something similarly named) is planned to have
    these features in some distant future.

    <h4>Derived from</h4>

    wxPropertyContainerMethods\n
    wxScrolledWindow\n
    wxPanel\n
    wxWindow\n
    wxEvtHandler\n
    wxObject\n

    <h4>Include files</h4>

    <wx/propertygrid/propertygrid.h>

    <h4>Window styles</h4>

    @link wndflags Additional Window Styles@endlink

    <h4>Event handling</h4>

    To process input from a propertygrid control, use these event handler macros to
    direct input to member functions that take a wxPropertyGridEvent argument.

    <table>
    <tr><td>EVT_PG_SELECTED (id, func)</td><td>Property is selected.</td></tr>
    <tr><td>EVT_PG_CHANGED (id, func)</td><td>Property value is modified.</td></tr>
    <tr><td>EVT_PG_HIGHLIGHTED (id, func)</td><td>Mouse moves over property. Event's property is NULL if hovered on area that is not a property.</td></tr>
    </table>

    \sa @link wxPropertyGridEvent wxPropertyGridEvent@endlink

    \remarks

    - Following functions do not automatically update the screen: Append. You
      will need to explicitly call Refresh() <b>if</b> you called one of these
      functions outside parent window constructor.

    - Use Freeze() and Thaw() respectively to disable and enable drawing. This
      will also delay sorting etc. miscellaneous calculations to the last moment.

    - Most methods have two versions - one which accepts property id (faster) and
      another that accepts property name (which additional does hashmap lookup).

    For code examples, see the main page.

*/
class WXDLLIMPEXP_PG wxPropertyGrid : public wxScrolledWindow, public wxPropertyContainerMethods
{
    friend class wxPropertyGridState;
    friend class wxPropertyGridManager;

	DECLARE_CLASS(wxPropertyGrid)

public:
	/** Two step constructor. Call Create when this constructor is called to build up the
	    wxPropertyGrid
	*/
    wxPropertyGrid();

#ifdef SWIG
    %pythonAppend wxPropertyGrid   "self._setOORInfo(self)"
    %pythonAppend wxPropertyGrid() ""

    wxPropertyGrid(wxWindow *parent, wxWindowID id = -1,
               			const wxPoint& pos = wxDefaultPosition,
               			const wxSize& size = wxDefaultSize,
               			long style = wxPG_DEFAULT_STYLE,
               			const wxChar* name = wxPropertyGridNameStr);
    %name(PrePropertyGrid) wxPropertyGrid();

#else
    /** The default constructor. The styles to be used are styles valid for
        the wxWindow and wxScrolledWindow.
        \sa @link wndflags Additional Window Styles@endlink
    */
    wxPropertyGrid(wxWindow *parent, wxWindowID id = -1,
               			const wxPoint& pos = wxDefaultPosition,
               			const wxSize& size = wxDefaultSize,
               			long style = wxPG_DEFAULT_STYLE,
               			//const wxValidator &validator = wxDefaultValidator,
               			const wxChar* name = wxPropertyGridNameStr);

    /** Destructor */
    virtual ~wxPropertyGrid();
#endif

    /** Appends property to the list. wxPropertyGrid assumes ownership of the object.
        Becomes child of most recently added category.
        \remarks
        - wxPropertyGrid takes the ownership of the property pointer.
        - If appending a category with name identical to a category already in the
          wxPropertyGrid, then newly created category is deleted, and most recently
          added category (under which properties are appended) is set to the one with
          same name. This allows easier adding of items to same categories in multiple
          passes.
        - Does not automatically redraw the control, so you may need to call Refresh
          when calling this function after control has been shown for the first time.
    */
    wxPGId Append ( wxPGProperty* property );

    inline wxPGId AppendCategory ( const wxString& label, const wxString& name = wxPG_LABEL )
    {
        return Append ( new wxPropertyCategoryClass(label,name) );
    }

#if wxPG_INCLUDE_BASICPROPS
    inline wxPGId Append ( const wxString& label, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString )
    {
        return Append ( wxStringProperty(label,name,value) );
    }

    inline wxPGId Append ( const wxString& label, const wxString& name = wxPG_LABEL, int value = 0 )
    {
        return Append ( wxIntProperty(label,name,value) );
    }

    inline wxPGId Append ( const wxString& label, const wxString& name = wxPG_LABEL, double value = 0.0 )
    {
        return Append ( wxFloatProperty(label,name,value) );
    }

    inline wxPGId Append ( const wxString& label, const wxString& name = wxPG_LABEL, bool value = FALSE )
    {
        return Append ( wxBoolProperty(label,name,value) );
    }
#endif

    inline wxPGId AppendIn ( const wxPGId id, wxPGProperty* property )
    {
        return Insert(id,-1,property);
    }

    inline wxPGId AppendIn ( wxPGNameStr name, wxPGProperty* property )
    {
        return Insert(GetPropertyByName(name),-1,property);
    }

    inline wxPGId AppendIn ( const wxPGId id, const wxString& label, const wxString& propname, wxVariant& value )
    {
        return m_pState->AppendIn( (wxPGPropertyWithChildren*)wxPGIdToPtr(id), label, propname, value );
    }

    inline wxPGId AppendIn ( wxPGNameStr name, const wxString& label, const wxString& propname, wxVariant& value )
    {
        return m_pState->AppendIn( (wxPGPropertyWithChildren*)wxPGIdToPtr(GetPropertyByName(name)),
            label, propname, value );
    }

    /** This static function enables or disables automatic use of wxGetTranslation for
        following strings: wxEnumProperty list labels, wxFlagsProperty sub-property
        labels.
        Default is FALSE.
    */
    static void AutoGetTranslation ( bool enable );

    /** Centers the splitter. If argument is TRUE, automatic splitter centering is
        enabled (only applicapple if style wxPG_SPLITTERAUTOCENTER was defined).
    */
    void CenterSplitter ( bool enable_auto_centering );

    /** Two step creation. Whenever the control is created without any parameters, use Create to actually
        create it. Don't access the control's public methods before this is called
        \sa @link wndflags Additional Window Styles@endlink
    */
    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxPG_DEFAULT_STYLE,
                const wxChar* name = wxPropertyGridNameStr);

    /** Deletes all properties. Does not free memory allocated for arrays etc.
        This should *not* be called in wxPropertyGridManager.
    */
    void Clear ();

    /** Resets modified status of a property and all sub-properties.
    */
    inline void ClearModifiedStatus ( wxPGId id )
    {
        m_pState->ClearModifiedStatus(wxPGIdToPtr(id));
    }

    /** Resets modified status of all properties.
    */
    inline void ClearModifiedStatus ()
    {
        m_pState->ClearModifiedStatus(m_pState->m_properties);
        m_pState->m_anyModified = FALSE;
    }

    /** Resets value of a property to its default. */
    bool ClearPropertyValue ( wxPGId id );

    /** Resets value of a property to its default. */
    inline bool ClearPropertyValue ( wxPGNameStr name )
    {
        return ClearPropertyValue ( GetPropertyByName(name) );
    }

    /** Deselect current selection, if any. */
    void ClearSelection ();

    /** Collapses given category or property with children.
        Returns true if actually collapses.
    */
    inline bool Collapse ( wxPGId p ) { return Collapse ( &GetPropertyById(p)); }

    /** Collapses given category or property with children.
        Returns true if actually collapses.
    */
    inline bool Collapse ( wxPGNameStr name )
    {
        return Collapse ( wxPGIdToPtr ( GetPropertyByName(name) ) );
    }

    /** Collapses all items that can be collapsed.
    */
    inline void CollapseAll () { m_pState->ExpandAll(0); }

    /** Shows(arg = FALSE) or hides(arg = TRUE) all hideable properties. */
    void Compact( bool compact );

    /** Deletes a property by id. If category is deleted, all children are automatically deleted as well. */
    inline void Delete ( wxPGId id )
    {
        Delete ( wxPGIdToPtr(id) );
    }

    /** Deletes a property by name. */
    inline void Delete ( wxPGNameStr name )
    {
        Delete ( wxPGIdToPtr ( GetPropertyByName(name) ) );
    }

    /** Disables property. */
    inline bool Disable ( wxPGId id ) { return EnableProperty (id,FALSE); }

    /** Disables property. */
    inline bool Disable ( wxPGNameStr name ) { return EnableProperty (name,FALSE); }

    /** Disables property. */
    inline bool DisableProperty ( wxPGId id ) { return EnableProperty (id,FALSE); }

    /** Disables property. */
    inline bool DisableProperty ( wxPGNameStr name ) { return EnableProperty (name,FALSE); }

    /** Enables or disables (shows/hides) categories according to parameter enable. */
    bool EnableCategories ( bool enable );

    /** Enables or disables property, depending on whether enable is TRUE or FALSE. */
    /*inline bool Enable ( wxPGId id, bool enable = TRUE )
    {
        return EnableProperty ( id, enable );
    }*/

    /** Enables or disables property, depending on whether enable is TRUE or FALSE. */
    /*inline bool Enable ( wxPGNameStr name, bool enable = TRUE )
    {
        return EnableProperty ( name, enable );
    }*/

    /** Enables or disables property, depending on whether enable is TRUE or FALSE. */
    bool EnableProperty ( wxPGId id, bool enable = TRUE );

    /** Enables or disables property, depending on whether enable is TRUE or FALSE. */
    inline bool EnableProperty ( wxPGNameStr name, bool enable = TRUE )
    {
        return EnableProperty ( GetPropertyByName(name), enable );
    }

    /** Scrolls and/or expands items to ensure that the given item is visible.
        Returns TRUE if something was actually done.
    */
    bool EnsureVisible ( wxPGId id );

    /** Scrolls and/or expands items to ensure that the given item is visible.
        Returns TRUE if something was actually done.
    */
    inline bool EnsureVisible ( wxPGNameStr name )
    {
        return EnsureVisible ( GetPropertyByName(name) );
    }

    /** Expands given category or property with children.
        Returns true if actually expands.
    */
    inline bool Expand ( wxPGId id ) { return Expand ( &GetPropertyById(id) ); }

    /** Expands given category or property with children.
        Returns true if actually expands.
    */
    inline bool Expand ( wxPGNameStr name )
    {
        return Expand ( wxPGIdToPtr ( GetPropertyByName(name) ) );
    }

    /** Expands all items that can be expanded.
    */
    inline void ExpandAll () { m_pState->ExpandAll(1); }

    /** Returns a wxVariant list containing wxVariant versions of all
        property values. Order is not guaranteed, but generally it should
        match the visible order in the grid.
        \param flags
        Use wxKEEP_STRUCTURE to retain category structure; each sub
        category will be its own wxList of wxVariant.
        \remarks
    */
    wxVariant GetPropertyValues ( const wxString& listname = wxEmptyString,
        wxPGId baseparent = wxPGIdGen((wxPGProperty*)NULL), long flags = 0 ) const
    {
        return m_pState->GetPropertyValues(listname,baseparent,flags);
    }

    inline wxFont* GetCaptionFont() { return &m_captionFont; }

    /** Returns current category caption background colour. */
    inline wxColour GetCaptionBackroundColour() const { return m_colCapBack; }

    /** Returns current category caption text colour. */
    inline wxColour GetCaptionForegroundColour() const { return m_colCapFore; }

    /** Returns current cell background colour. */
    inline wxColour GetCellBackgroundColour() const { return m_colPropBack; }

    /** Returns current cell text colour. */
    inline wxColour GetCellTextColour() const { return m_colPropFore; }

    /** Returns number of children of the root property.
    */
    inline size_t GetChildrenCount ()
    {
        return GetChildrenCount( wxPGIdGen(m_pState->m_properties) );
    }

    /** Returns number of children for the property. */
    inline size_t GetChildrenCount ( wxPGId id ) const
    {
        wxASSERT ( wxPGIdIsOk(id) );
        return wxPGIdToPtr(id)->GetChildCount();
    }

    /** Returns number of children for the property. */
    inline size_t GetChildrenCount ( wxPGNameStr name ) { return GetChildrenCount( GetPropertyByName(name) ); }

    /** Returns id of first item, whether it is a category or property. */
    inline wxPGId GetFirst () const
    {
        return m_pState->GetFirst();
    }

    /** Returns id of first visible item, whether it is a category or property.
        Note that visible item means category, property, or sub-property which
        user can see when control is scrolled properly. It does not only mean
        items that are actually painted on the screen.
    */
    inline wxPGId GetFirstVisible () const
    {
        wxPGProperty* p = NULL;
        if ( m_pState->m_properties->GetCount() )
        {
            p = m_pState->m_properties->Item(0);
            if ( (m_iFlags & wxPG_FL_HIDE_STATE) && p->m_flags & wxPG_PROP_HIDEABLE )
                p = GetNeighbourItem ( p, TRUE, 1 );
        }
        return wxPGIdGen(p);
    }

    /** Returns height of highest characters of used font. */
    int GetFontHeight () const { return m_fontHeight; }

    /** Returns pointer to itself. Dummy function that enables same kind
        of code to use wxPropertyGrid and wxPropertyGridManager.
    */
    wxPropertyGrid* GetGrid() { return this; }

    /** Returns id of first category (from target page). */
    inline wxPGId GetFirstCategory () const
    {
        return m_pState->GetFirstCategory ();
    }

    /** Returns id of first child of given property.
        \remarks
        Does not return sub-properties!
    */
    inline wxPGId GetFirstChild ( wxPGId parent )
    {
        wxASSERT ( wxPGIdIsOk(parent) );
        wxPGPropertyWithChildren& p = (wxPGPropertyWithChildren&)parent.GetProperty();
        if ( p.GetParentingType()==0 || p.GetParentingType()==-1 || !p.GetCount() ) return wxPGIdGen(NULL);
        return wxPGIdGen(p.Item(0));
    }
    inline wxPGId GetFirstChild ( wxPGNameStr name ) { return GetFirstChild( GetPropertyByName(name) ); }

    /** Returns id of first property that is not a category. */
    inline wxPGId GetFirstProperty()
    {
        return m_pState->GetFirstProperty();
    }

    /** Returns size of the custom paint image in front of property.
        If no argument is given, returns preferred size.
    */
    wxSize GetImageSize ( wxPGId id = wxPGIdGen((wxPGProperty*)NULL) ) const;

    /** Returns property (or category) at given y coordinate (relative to control's
        top left).
    */
    wxPGId GetItemAtY ( int y ) { return wxPGIdGen(DoGetItemAtY(y)); }

    /** Returns id of last item. Ignores categories and sub-properties.
    */
    inline wxPGId GetLastProperty ()
    {
        if ( !m_pState->m_properties->GetCount() ) return wxPGIdGen((wxPGProperty*)NULL);
        wxPGProperty* p = GetLastItem (FALSE, FALSE);
        if ( p->GetParentingType() > 0 )
            return GetPrevProperty ( wxPGIdGen(p) );
        return wxPGIdGen(p);
    }

    /** Returns id of last child of given property.
        \remarks
        Returns even sub-properties.
    */
    inline wxPGId GetLastChild ( wxPGId parent )
    {
        wxASSERT ( wxPGIdIsOk(parent) );
        wxPGPropertyWithChildren& p = (wxPGPropertyWithChildren&)parent.GetProperty();
        if ( !p.GetParentingType() || !p.GetCount() ) return wxPGIdGen(NULL);
        return wxPGIdGen(p.Last());
    }
    inline wxPGId GetLastChild ( wxPGNameStr name ) { return GetLastChild ( GetPropertyByName(name) ); }

    /** Returns id of last visible item. Does <b>not</b> ignore categories sub-properties.
    */
    inline wxPGId GetLastVisible ()
    {
        return wxPGIdGen ( GetLastItem (TRUE, TRUE) );
    }

    /** Returns colour of lines between cells. */
    inline wxColour GetLineColour() const { return m_colLine; }

    /** Returns background colour of margin. */
    inline wxColour GetMarginColour() const { return m_colMargin; }

    /** Returns id of next property. This does <b>not</b> iterate to sub-properties
        or categories, unlike GetNextVisible.
    */
    inline wxPGId GetNextProperty ( wxPGId id )
    {
        return m_pState->GetNextProperty(id);
    }

    /** Returns id of next category after a given property (which does not have to be category). */
    inline wxPGId GetNextCategory ( wxPGId id ) const
    {
        return m_pState->GetNextCategory(id);
    }

    /** Returns id of next visible item.
        Note that visible item means category, property, or sub-property which
        user can see when control is scrolled properly. It does not only mean
        items that are actually painted on the screen.
    */
    inline wxPGId GetNextVisible ( wxPGId property ) const
    {
        return wxPGIdGen ( GetNeighbourItem( property.GetPropertyPtr(),
            TRUE, 1 ) );
    }

    /** Returns id of previous property. Unlike GetPrevVisible, this skips categories
        and sub-properties.
    */
    inline wxPGId GetPrevProperty ( wxPGId id )
    {
        return m_pState->GetPrevProperty(id);
    }

    /** Returns id of previous item under the same parent. */
    inline wxPGId GetPrevSibling ( wxPGId id )
    {
        return wxPropertyGridState::GetPrevSibling(id);
    }
    inline wxPGId GetPrevSibling ( wxPGNameStr name )
    {
        return wxPropertyGridState::GetPrevSibling(GetPropertyByName(name));
    }

    /** Returns id of previous visible property.
    */
    inline wxPGId GetPrevVisible ( wxPGId property )
    {
        wxASSERT ( wxPGIdIsOk(property) );
        return wxPGIdGen ( GetNeighbourItem( property.GetPropertyPtr(),
            TRUE, -1 ) );
    }

    /** Returns id of property's nearest parent category. If no category
        found, returns invalid wxPGId.
    */
    inline wxPGId GetPropertyCategory ( wxPGId id ) const
    {
        return wxPGIdGen ( GetPropertyCategory ( wxPGIdToPtr(id) ) );
    }
    inline wxPGId GetPropertyCategory ( wxPGNameStr name ) const
    {
        return GetPropertyCategory ( GetPropertyByName(name) );
    }

    /** Returns cell background colour of a property. */
    wxColour GetPropertyColour ( wxPGId id ) const;
    inline wxColour GetPropertyColour ( wxPGNameStr name ) const
    {
        return GetPropertyColour(GetPropertyByName(name));
    }

    /** Returns id of property with given label (case-sensitive). If there is no
        property with such label, returned property id is invalid ( i.e. it will return
        FALSE with IsOk method). If there are multiple properties with identical name,
        most recent added is returned.
    */
    inline wxPGId GetPropertyByLabel ( const wxString& name ) const
    {
        return m_pState->GetPropertyByLabel(name);
    }

    /** Returns "root property". It does not have name, etc. and it is not
        visible. It is only useful for accessing its children.
    */
    wxPGId GetRoot () const { return wxPGIdGen(m_pState->m_properties); }

    /** Returns height of a single grid row (in pixels). */
    int GetRowHeight () const { return m_lineHeight; }

    inline wxPGId GetSelectedProperty () const { return GetSelection(); }

    /** Returns currently selected property. */
    inline wxPGId GetSelection () const
    {
        return wxPGIdGen(m_selected);
    }

    /** Returns current selection background colour. */
    inline wxColour GetSelectionBackgroundColour() const { return m_colSelBack; }

    /** Returns current selection text colour. */
    inline wxColour GetSelectionForegroundColour() const { return m_colSelFore; }

    /** Returns current splitter x position. */
    inline int GetSplitterPosition () const { return m_splitterx; }

    /** Returns a binary copy of the current property state.
        NOTE: Too much work to implement, and uses would be few indeed.
    */
    //wxPropertyGridState* GetCopyOfState() const;

    /** Returns current vertical spacing. */
    inline int GetVerticalSpacing () const { return (int)m_vspacing; }

    /** Returns TRUE if a property is selected. */
    inline bool HasSelection() const { return ((m_selected!=(wxPGProperty*)NULL)?TRUE:FALSE); }

    /** Hides all low priority properties. */
    inline void HideLowPriority() { Compact ( TRUE ); }

    /** Inserts property to the list.

        \param priorthis
        New property is inserted just prior to this. Available only
        in the first variant. There are two versions of this function
        to allow this parameter to be either an id or name to
        a property.

        \param parent
        New property is inserted under this category. Available only
        in the second variant. There are two versions of this function
        to allow this parameter to be either an id or name to
        a property.

        \param index
        Index under category. Available only in the second variant.
        If index is < 0, property is appended in category.

        \param newproperty
        Pointer to the inserted property. wxPropertyGrid will take
        ownership of this object.

        \return
        Returns id for the property,

        \remarks

        - wxPropertyGrid takes the ownership of the property pointer.

        While Append may be faster way to add items,
        make note that when both data storages (categoric and
        non-categoric) are active, Insert becomes even more slow. This is
        especially true if current mode is non-categoric.

        Example of use:

        \code

            // append category
            wxPGId my_cat_id = propertygrid->Append( new wxPropertyCategoryClass (wxT("My Category")) );

            ...

            // insert into category - using second variant
            wxPGId my_item_id_1 = propertygrid->Insert( my_cat_id, 0, new wxStringProperty(wxT("My String 1")) );

            // insert before to first item - using first variant
            wxPGId my_item_id_2 = propertygrid->Insert ( my_item_id, new wxStringProperty(wxT("My String 2")) );

        \endcode

      */
    inline wxPGId Insert ( wxPGId priorthis, wxPGProperty* newproperty )
    {
        wxPGId res = Insert ( wxPGIdToPtr( priorthis ), newproperty );
        DrawItems ( newproperty, (wxPGProperty*) NULL );
        return res;
    }
    /** @link Insert wxPropertyGrid::Insert @endlink */
    inline wxPGId Insert ( wxPGNameStr name, wxPGProperty* newproperty )
    {
        wxPGId res = Insert ( wxPGIdToPtr ( GetPropertyByName(name) ), newproperty );
        DrawItems ( newproperty, (wxPGProperty*) NULL );
        return res;
    }

    /** @link Insert wxPropertyGrid::Insert @endlink */
    inline wxPGId Insert ( wxPGId id, int index, wxPGProperty* newproperty )
    {
        wxPGId res = Insert ( (wxPGPropertyWithChildren*)wxPGIdToPtr (id), index, newproperty );
        DrawItems ( newproperty, (wxPGProperty*) NULL );
        return res;
    }

    /** @link Insert wxPropertyGrid::Insert @endlink */
    inline wxPGId Insert ( wxPGNameStr name, int index, wxPGProperty* newproperty )
    {
        wxPGId res = Insert ( (wxPGPropertyWithChildren*)wxPGIdToPtr ( GetPropertyByName(name) ), index, newproperty );
        DrawItems ( newproperty, (wxPGProperty*) NULL );
        return res;
    }

    inline wxPGId InsertCategory ( wxPGId id, int index, const wxString& label, const wxString& name = wxPG_LABEL )
    {
        return Insert ( id, index, new wxPropertyCategoryClass(label,name) );
    }

#if wxPG_INCLUDE_BASICPROPS
    inline wxPGId Insert ( wxPGId id, int index, const wxString& label, const wxString& name, const wxString& value = wxEmptyString )
    {
        return Insert ( id, index, wxStringProperty(label,name,value) );
    }

    inline wxPGId Insert ( wxPGId id, int index, const wxString& label, const wxString& name, int value )
    {
        return Insert ( id, index, wxIntProperty(label,name,value) );
    }

    inline wxPGId Insert ( wxPGId id, int index, const wxString& label, const wxString& name, double value )
    {
        return Insert ( id, index, wxFloatProperty(label,name,value) );
    }

    inline wxPGId Insert ( wxPGId id, int index, const wxString& label, const wxString& name, bool value )
    {
        return Insert ( id, index, wxBoolProperty(label,name,value) );
    }
#endif

    /** Returns true if any property has been modified by the user. */
    inline bool IsAnyModified () const { return (m_pState->m_anyModified>0); }

    /** Returns TRUE if updating is frozen (ie. Freeze() called but not yet Thaw() ). */
    inline bool IsFrozen () const { return (m_frozen>0)?TRUE:FALSE; }

    /** Returns TRUE if given property is selected. */
    inline bool IsPropertySelected ( wxPGId id ) const
    {
        return ( m_selected == wxPGIdToPtr( id ) ) ? TRUE : FALSE;
    }

    /** Returns TRUE if given property is selected. */
    inline bool IsPropertySelected ( wxPGNameStr name ) { return IsPropertySelected(GetPropertyByName(name)); }

    /** Disables (limit = TRUE) or enables (limit = FALSE) wxTextCtrl editor of a property,
        if it is not the sole mean to edit the value.
    */
    void LimitPropertyEditing ( wxPGId id, bool limit = TRUE );

    /** Disables (limit = TRUE) or enables (limit = FALSE) wxTextCtrl editor of a property,
        if it is not the sole mean to edit the value.
    */
    inline void LimitPropertyEditing ( wxPGNameStr name, bool limit = TRUE )
    {
        LimitPropertyEditing(GetPropertyByName(name),limit);
    }

    /** Redraws the entire window. Use this instead of Refresh to avoid flicker. */
    //void RedrawAllVisible ();

    /** Registers a new value type. Takes ownership of the object.
        \retval
        Pointer to the value type that should be used. If on with
        the same name already existed, then the first one will be used,
        and its pointer is returned instead.
    */
    static wxPGValueType* RegisterValueType ( wxPGValueType* valueclass, bool no_def_check = FALSE );

    /** Registers a new editor class.
        \retval
        Pointer to the editor class instance that should be used.
    */
    static wxPGEditor* RegisterEditorClass ( wxPGEditor* valueclass, bool no_def_check = FALSE );

    /** Resets all colours to the original system values.
    */
    void ResetColours();

    /** Sets the current category - Append will add non-categories under this one.
    */
    inline void SetCurrentCategory ( wxPGId id )
    {
        wxPropertyCategoryClass* pc = (wxPropertyCategoryClass*)wxPGIdToPtr(id);
#ifdef __WXDEBUG__
        if ( pc ) wxASSERT ( pc->GetParentingType() > 0 );
#endif
        m_pState->m_currentCategory = pc;
    }

    /** Sets the current category - Append will add non-categories under this one.
    */
    inline void SetCurrentCategory ( wxPGNameStr name = wxEmptyString )
    {
        SetCurrentCategory(GetPropertyByName(name));
    }

    /** Sets background colour - applies to margin background and separator lines. */
    //virtual bool SetBackgroundColour(const wxColour& col);

    /** Sets background colour of property and all its children. Background brush
        cache is optimized for often set colours to be set last.
    */
    void SetPropertyColour ( wxPGId id, const wxColour& col );
    inline void SetPropertyColour ( wxPGNameStr name, const wxColour& col )
    {
        SetPropertyColour ( GetPropertyByName(name), col );
    }

    /** Sets background colour of property and all its children to the default. */
    inline void SetPropertyColourToDefault ( wxPGId id )
    {
        SetColourIndex ( wxPGIdToPtr(id), 0 );
    }
    inline void SetPropertyColourToDefault ( wxPGNameStr name )
    {
        SetColourIndex ( wxPGIdToPtr(GetPropertyByName(name)), 0 );
    }

    /** Sets category caption background colour. */
    void SetCaptionBackgroundColour(const wxColour& col);

    /** Sets category caption text colour. */
    void SetCaptionForegroundColour(const wxColour& col);

    /** Sets default cell background colour - applies to property cells.
        Note that appearance of editor widgets may not be affected.
    */
    void SetCellBackgroundColour(const wxColour& col);

    /** Sets default cell text colour - applies to property name and value text.
        Note that appearance of editor widgets may not be affected.
    */
    void SetCellTextColour(const wxColour& col);

    /** Sets colour of lines between cells. */
    void SetLineColour(const wxColour& col);

    /** Sets background colour of margin. */
    void SetMarginColour(const wxColour& col);

    /** Sets selection background colour - applies to selected property name background. */
    void SetSelectionBackground(const wxColour& col);

    /** Sets selection foreground colour - applies to selected property name text. */
    void SetSelectionForeground(const wxColour& col);

    /** Sets x coordinate of the splitter. */
    void SetSplitterPosition ( int newxpos, bool refresh = TRUE );

    /** Selects a property. Editor widget is automatically created, but
        not focused unless focus is TRUE. This will generate wxEVT_PG_SELECT event.
        \param id
        Id to property to select.
        \sa wxPropertyGrid::Unselect
    */
    inline void SelectProperty ( wxPGId id, bool focus = FALSE )
    {
        SelectProperty(wxPGIdToPtr(id),focus);
    }
    inline void SelectProperty ( wxPGNameStr name, bool focus = FALSE )
    {
        SelectProperty(wxPGIdToPtr(GetPropertyByName(name)),focus);
    }

    /** Property is be hidden/shown when hider button is toggled or
        when wxPropertyGrid::Compact is called.
    */
    void SetPropertyPriority ( wxPGId id, int priority );

    /** Property is be hidden/shown when hider button is toggled or
        when wxPropertyGrid::Compact is called.
    */
    inline void SetPropertyPriority ( wxPGNameStr name, int priority )
    {
        SetPropertyPriority(GetPropertyByName(name),priority);
    }

    /** Mostly useful for page switching.
    */
    void SwitchState ( wxPropertyGridState* pNewState );

    /** Sets label of a property.
        \remarks
        This is the only way to set property's name. There is not
        wxPGProperty::SetLabel() method.
    */
    inline void SetPropertyLabel ( wxPGId id, const wxString& newproplabel )
    {
        wxPGProperty* p = &GetPropertyById( id );
        if ( p ) SetPropertyLabel ( p, newproplabel );
    }
    /** Sets label of a property.
        \remarks
        This is the only way to set property's label. There is no
        wxPGProperty::SetLabel() method.
    */
    inline void SetPropertyLabel ( wxPGNameStr name, const wxString& newproplabel )
    {
        wxPGProperty* p = wxPGIdToPtr ( GetPropertyByName(name) );
        if ( p ) SetPropertyLabel ( p, newproplabel );
    }

    /** Sets name of a property.
        \param id
        Id of a property.
        \param newname
        New name.
        \remarks
        This is the only way to set property's name. There is not
        wxPGProperty::SetName() method.
    */
    inline void SetPropertyName ( wxPGId id, const wxString& newname )
    {
        DoSetPropertyName ( wxPGIdToPtr(id), newname );
    }
    /** Sets name of a property.
        \param name
        Label of a property.
        \param newname
        New name.
        \remarks
        This is the only way to set property's name. There is not
        wxPGProperty::SetName() method.
    */
    inline void SetPropertyName ( wxPGNameStr name, const wxString& newname )
    {
        DoSetPropertyName ( wxPGIdToPtr ( GetPropertyByName(name) ), newname );
    }

    /** Sets value (long integer) of a property. */
    inline void SetPropertyValue ( wxPGId id, long value )
    {
        SetPropertyValue ( id, wxPG_VALUETYPE(long), wxPGVariantFromLong(value) );
    }
    /** Sets value (integer) of a property. */
    inline void SetPropertyValue ( wxPGId id, int value )
    {
        SetPropertyValue ( id, wxPG_VALUETYPE(long), wxPGVariantFromLong((long)value) );
    }
    /** Sets value (floating point) of a property. */
    inline void SetPropertyValue ( wxPGId id, double value )
    {
        SetPropertyValue ( id, wxPG_VALUETYPE(double), wxPGVariantFromDouble(value) );
    }
    /** Sets value (bool) of a property. */
    inline void SetPropertyValue ( wxPGId id, bool value )
    {
        SetPropertyValue ( id, wxPG_VALUETYPE(bool), wxPGVariantFromLong(value?1:0) );
    }

    /** Sets value (wxString) of a property.
        \remarks
        This method uses wxPGProperty::SetValueFromString, which all properties
        should implement. This means that there should not be a type error,
        and instead the string is converted to property's actual value type.
    */
    void SetPropertyValue ( wxPGId id, const wxString& value );

    inline void SetPropertyValue ( wxPGId id, const wxChar* value )
    {
        SetPropertyValue(id,wxString(value));
    }

    /** Sets value (wxArrayString) of a property. */
    inline void SetPropertyValue ( wxPGId id, const wxArrayString& value )
    {
        SetPropertyValue ( id, wxPG_VALUETYPE(wxArrayString), wxPGVariantFromArrayString(value) );
    }
    /** Sets value (void*) of a property. */
    inline void SetPropertyValue ( wxPGId id, void* value )
    {
        SetPropertyValue ( id, wxPG_VALUETYPE(void), value );
    }
    /** Sets value (wxObject*) of a property. */
    void SetPropertyValue ( wxPGId id, wxObject* value );
    inline void SetPropertyValue ( wxPGId id, wxObject& value )
    {
        SetPropertyValue(id,&value);
    }

    /** Sets value (wxVariant&) of a property. */
    void SetPropertyValue ( wxPGId id, wxVariant& value );

    /** Sets value (wxPoint&) of a property. */
    inline void SetPropertyValue ( wxPGId id, const wxPoint& value )
    {
        SetPropertyValue ( id, wxT("wxPoint"), (void*)&value );
        //wxASSERT ( wxStrcmp(wxPGIdToPtr(id)->GetValueType()->GetCustomTypeName(),wxT("wxPoint")) == 0 );
        //SetPropertyValue ( id, wxPG_VALUETYPE(void), (void*)&value );
    }
    /** Sets value (wxSize&) of a property. */
    inline void SetPropertyValue ( wxPGId id, const wxSize& value )
    {
        SetPropertyValue ( id, wxT("wxSize"), (void*)&value );
        //wxASSERT ( wxStrcmp(wxPGIdToPtr(id)->GetValueType()->GetCustomTypeName(),wxT("wxSize")) == 0 );
        //SetPropertyValue ( id, wxPG_VALUETYPE(void), (void*)&value );
    }
    /** Sets value (wxArrayInt&) of a property. */
    inline void SetPropertyValue ( wxPGId id, const wxArrayInt& value )
    {
        //wxASSERT ( wxStrcmp(wxPGIdToPtr(id)->GetValueType()->GetCustomTypeName(),wxT("wxArrayInt")) == 0 );
        //SetPropertyValue ( id, wxPG_VALUETYPE(void), (void*)&value );
        SetPropertyValue ( id, wxT("wxArrayInt"), (void*)&value );
    }

    /** Sets value (long integer) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, long value )
    {
        SetPropertyValue ( GetPropertyByName(name), wxPG_VALUETYPE(long), wxPGVariantFromLong(value) );
    }
    /** Sets value (integer) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, int value )
    {
        SetPropertyValue ( GetPropertyByName(name), wxPG_VALUETYPE(long), wxPGVariantFromLong(value) );
    }
    /** Sets value (floating point) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, double value )
    {
        SetPropertyValue ( GetPropertyByName(name), wxPG_VALUETYPE(double), wxPGVariantFromDouble(value) );
    }
    /** Sets value (bool) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, bool value )
    {
        SetPropertyValue ( GetPropertyByName(name), wxPG_VALUETYPE(bool), wxPGVariantFromLong(value?1:0) );
    }
    /** Sets value (wxString) of a property. For properties which value type is
        not string, calls wxPGProperty::SetValueFromString to translate the value.
    */
    inline void SetPropertyValue ( wxPGNameStr name, const wxString& value )
    {
        SetPropertyValue ( GetPropertyByName(name), value );
    }
    /** Sets value (wxString) of a property. For properties which value type is
        not string, calls wxPGProperty::SetValueFromString to translate the value.
    */
    inline void SetPropertyValue ( wxPGNameStr name, const wxChar* value )
    {
        SetPropertyValue ( GetPropertyByName(name), wxString(value) );
    }
    /** Sets value (void*) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, void* value )
    {
        SetPropertyValue ( GetPropertyByName(name), wxPG_VALUETYPE(void), value );
    }
    /** Sets value (wxArrayString) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, const wxArrayString& value )
    {
        SetPropertyValue ( GetPropertyByName(name), wxPG_VALUETYPE(wxArrayString), wxPGVariantFromArrayString(value) );
    }
    /** Sets value (wxObject*) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, wxObject* value )
    {
        SetPropertyValue ( GetPropertyByName(name), value );
    }
    inline void SetPropertyValue ( wxPGNameStr name, wxObject& value )
    {
        SetPropertyValue(name,&value);
    }
    /** Sets value (wxVariant&) of a property. */
    void SetPropertyValue ( wxPGNameStr name, wxVariant& value )
    {
        SetPropertyValue ( GetPropertyByName(name), value );
    }
    /** Sets value (wxPoint&) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, const wxPoint& value )
    {
        SetPropertyValue ( GetPropertyByName(name), value );
    }
    /** Sets value (wxSize&) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, const wxSize& value )
    {
        SetPropertyValue ( GetPropertyByName(name), value );
    }
    /** Sets value (wxArrayInt&) of a property. */
    inline void SetPropertyValue ( wxPGNameStr name, const wxArrayInt& value )
    {
        SetPropertyValue ( GetPropertyByName(name), value );
    }

    /** Sets property's value to unspecified. If it has children (it may be category),
        then the same thing is done to them.
    */
    void SetPropertyValueUnspecified ( wxPGId id );
    inline void SetPropertyValueUnspecified ( wxPGNameStr name )
    {
        SetPropertyValueUnspecified ( GetPropertyByName(name) );
    }

    /** Sets various property values from a list of wxVariants. If property with
        name is missing from the grid, new property is created under given default
        category (or root if omitted).
    */
    void SetPropertyValues ( const wxList& list, wxPGId default_category )
    {
        m_pState->SetPropertyValues(list,default_category);
    }

    inline void SetPropertyValues ( const wxVariant& list, wxPGId default_category )
    {
        SetPropertyValues (list.GetList(),default_category);
    }
    inline void SetPropertyValues ( const wxList& list, const wxString& default_category = wxEmptyString )
    {
        SetPropertyValues (list,GetPropertyByName(default_category));
    }
    inline void SetPropertyValues ( const wxVariant& list, const wxString& default_category = wxEmptyString )
    {
        SetPropertyValues (list.GetList(),GetPropertyByName(default_category));
    }

    /** Sets vertical spacing. Can be 1, 2, or 3 - a value relative to font
        height. Value of 2 should be default on most platforms.
        \remarks
        On wxMSW, wxComboBox, when used as property editor widget, will spill
        out with anything less than 3.
    */
    inline void SetVerticalSpacing ( int vspacing )
    {
        m_vspacing = (unsigned char)vspacing;
        CalculateFontAndBitmapStuff ( vspacing );
        if ( !m_pState->m_itemsAdded ) Refresh();
    }

    /** Shows all low priority properties. */
    inline void ShowLowPriority() { Compact ( FALSE ); }

    /** Shows an brief error message that is related to a property. */
    inline void ShowPropertyError( wxPGId id, const wxString& msg )
    {
        wxASSERT ( wxPGIdIsOk(id) );
        wxPGIdToPtr(id)->ShowError(msg);
    }
    inline void ShowPropertyError( wxPGNameStr name, const wxString& msg )
    {
        ShowPropertyError (GetPropertyByName(name), msg);
    }

    /** Sorts all items at all levels (except sub-properties). */
    void Sort ();

    /** Sorts children of a category.
    */
    void Sort ( wxPGId id );

    /** Sorts children of a category.
    */
    inline void Sort ( wxPGNameStr name )
    {
        Sort ( GetPropertyByName(name) );
    }

    /** Overridden function.
        \sa @link wndflags Additional Window Styles@endlink
    */
    virtual void SetWindowStyleFlag( long style );

    /** All properties added/inserted will have given priority by default.
        \param
        priority can be wxPG_HIGH (default) or wxPG_LOW.
    */
    inline void SetDefaultPriority( int priority )
    {
        if ( priority == wxPG_LOW )
            m_iFlags |= wxPG_FL_ADDING_HIDEABLES;
        else
            m_iFlags &= ~(wxPG_FL_ADDING_HIDEABLES);
    }

    /** Toggles priority of a property between wxPG_HIGH and wxPG_LOW.
    */
    inline void TogglePropertyPriority ( wxPGId id )
    {
        int priority = wxPG_LOW;
        if ( GetPropertyPriority(id) == wxPG_LOW )
            priority = wxPG_HIGH;
        SetPropertyPriority(id,priority);
    }

    /** Toggles priority of a property between wxPG_HIGH and wxPG_LOW.
    */
    inline void TogglePropertyPriority ( wxPGNameStr name )
    {
        TogglePropertyPriority(GetPropertyByName(name));
    }

    /** Same as SetDefaultPriority(wxPG_HIGH). */
    inline void ResetDefaultPriority()
    {
        SetDefaultPriority(wxPG_HIGH);
    }

    /** Property editor widget helper methods. */
    //@{
    /** Call when editor widget's contents is modified. For example, this is called
        when changes text in wxTextCtrl (used in wxStringProperty and wxIntProperty).
        \remarks
        This should only be called by properties.
        \sa @link wxPGProperty::OnEvent @endlink
    */
    inline void EditorsValueWasModified() { m_iFlags |= wxPG_FL_VALUE_MODIFIED; }
    /** Reverse of EditorsValueWasModified(). */
    inline void EditorsValueWasNotModified() { m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED); }
    /** Shortcut for creating dialog-caller button. Used, for example, by wxFontProperty.
        \remarks
        This should only be called by properties.
    */
    /** Returns true if editor's value was marked modified. */
    inline bool IsEditorsValueModified() const { return  ( m_iFlags & wxPG_FL_VALUE_MODIFIED ) ? TRUE : FALSE; }
    wxPGCtrlClass* GenerateEditorButton ( const wxPoint& pos, const wxSize& sz );
    /** Shortcut for creating text editor widget.
        \param pos
        Same as pos given for CreateEditor.
        \param sz
        Same as sz given for CreateEditor.
        \param value
        Initial text for wxTextCtrl.
        \param secondary
        If right-side control, such as button, also created, then create it first
        and pass it as this parameter.
        \remarks
        Note that this should generally be called only by new classes derived
        from wxPGProperty.
    */
    wxPGCtrlClass* GenerateEditorTextCtrl ( const wxPoint& pos,
        const wxSize& sz, const wxString& value, wxPGCtrlClass* secondary,
        const wxSize& imsz = wxDefaultSize );

    /* Generates both textctrl and button.
    */
    wxPGCtrlClass* GenerateEditorTextCtrlAndButton( const wxPoint& pos,
        const wxSize& sz, wxPGCtrlClass** psecondary, int limited_editing,
        wxPGProperty* property );

    /** Generates position for a widget editor dialog box.
        \param p
        Property for which dialog is positioned.
        \param sz
        Known or over-approximated size of the dialog.
        \retval
        Position for dialog.
    */
    wxPoint GetGoodEditorDialogPosition ( wxPGProperty* p,
        const wxSize& sz );

    // Converts escape sequences in src_str to newlines,
    // tabs, etc. and copies result to dst_str.
    static wxString& ExpandEscapeSequences ( wxString& dst_str, wxString& src_str );

    // Converts newlines, tabs, etc. in src_str to escape
    // sequences, and copies result to dst_str.
    static wxString& CreateEscapeSequences ( wxString& dst_str, wxString& src_str );

    /** Returns pointer to current active primary editor control (NULL if none).
    */
    inline wxPGCtrlClass* GetPrimaryEditor() const
    {
        return m_wndPrimary;
    }

    /** Generates contents for string dst based on the convetents of wxArrayString
        src. Format will be <preDelim>str1<postDelim> <preDelim>str2<postDelim>
        and so on. Set flags to 1 inorder to convert backslashes to double-back-
        slashes and "<preDelims>"'s to "\<preDelims>".
    */
    static void ArrayStringToString ( wxString& dst, const wxArrayString& src,
                                      wxChar preDelim, wxChar postDelim,
                                      int flags );
#if wxPG_USE_CUSTOM_CONTROLS
    inline wxCustomControlManager* GetCCManager() { return &m_ccManager; }
#endif

    /** Pass this function to Connect calls in propertyclass::CreateEditor.
    */
    void OnCustomEditorEvent ( wxEvent &event );
    /** Puts items into sl. Automatic wxGetTranslation is used if enabled. */
    void SLAlloc ( unsigned int itemcount, const wxChar** items );
    /** Returns sl. */
    inline wxArrayString& SLGet () { return m_sl; }
    //@}

    inline long GetInternalFlags () const { return m_iFlags; }
    inline void ClearInternalFlag ( long flag ) { m_iFlags &= ~(flag); }
    inline unsigned int GetBottomY () const { return m_bottomy; }
    inline void SetBottomY ( unsigned int y ) { m_bottomy = y; }

    /** Call after a property modified internally.
    */
    void PropertyWasModified ( wxPGProperty* p );

    //wxArrayPtrVoid* GetCatArray () { return &m_pState->m_catArray; }
#if !wxPG_USE_CUSTOM_CONTROLS
    void OnComboItemPaint (wxPGOwnerDrawnComboBox* pCb,int item,wxDC& dc,
                           wxRect& rect,int flags );
#endif

    static wxPGConstants* AddConstantsArray(const wxChar** labels,
                                            const long* values,
                                            int itemcount);
    static wxPGConstants* AddConstantsArray(const wxArrayString& labels,
                                            const wxArrayInt& values,
                                            bool acceptLabelsAsId);
    static wxPGConstants* AddConstantsArray(wxPGConstants& constants);
#ifdef __WXDEBUG__
    // Displays what dynamic arrays are allocated
    static void DumpAllocatedChoiceSets();
#endif
    /** Creates a "permanent" choices array. Has zero refcount,
        so Ref has to be called.
    */
    static wxPGConstants* CreateConstantsArray(size_t id);

    /** Returns wxPGConstants* matching the id. If not found,
        NULL is returned.
    */
    static wxPGConstants* GetConstantsArray(size_t id);

    /** Standard double-to-string conversion.
    */
    static void DoubleToString(wxString& target,
                               double value,
                               int precision,
                               bool removeZeroes,
                               wxString* precTemplate);

#ifdef _WX_WINDOW_H_BASE_

    /** Font change helper */
    void SetCurControlBoldFont();

// Simple way to make doxygen skip protected members
// Used wx/window.h include guard because had trouble
// getting doxygen definitions work.

    //
    // Overridden functions - no documentation required.
    //

    virtual wxSize DoGetBestSize() const;

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );

    virtual bool SetFont ( const wxFont& font );

    virtual void Freeze();

    virtual void SetExtraStyle ( long exStyle );

    virtual void Thaw();

protected:

    /** Includes property arrays etc. */
    //wxPropertyGridState         m_state;
    //wxPropertyGridState*         m_pState;

    /** 1 items appended/inserted, so stuff needs to be done before drawing;
        If m_bottomy == 0, then calcylatey's must be done.
        Otherwise just sort.
    */
    //unsigned char               m_itemsAdded;

    /** 1 if calling property event handler. */
    unsigned char               m_processingEvent;

#ifndef wxPG_ICON_WIDTH
	wxBitmap            *m_expandbmp, *m_collbmp;
#endif

    wxCursor            *m_cursor_sizewe;

    /** wxPGCtrlClass pointers to editor control(s). */
    wxPGCtrlClass       *m_wndPrimary;
    wxPGCtrlClass       *m_wndSecondary;

#if wxPG_DOUBLE_BUFFER
    wxBitmap            *m_doubleBuffer;
#endif

    /** Indicates bottom of drawn and clickable area on the control. Updated
        by CalculateYs. */
    unsigned int m_bottomy;

	/** Extra Y spacing between the items. */
	int                 m_spacingy;

    /** Control client area width; updated on resize. */
    int                 m_width;

    /** Control client area height; updated on resize. */
    int                 m_height;

    /** List of currently visible properties. */
    wxPGArrayProperty   m_arrVisible;

    /** Previously recorded scroll start position. */
    int                 m_prevVY;

    /** Necessary so we know when to re-calculate visibles on resize. */
    int                 m_calcVisHeight;

	/** The gutter spacing in front and back of the image. This determines the amount of spacing in front
	    of each item */
	int                 m_gutterwidth;

    /** Includes separator line. */
    int                 m_lineHeight;

    /** Gutter*2 + image width.
        */
    int                 m_marginwidth;

    int                 m_buttonSpacingY; // y spacing for expand/collapse button.

    /** Extra margin for expanded sub-group items. */
    int                 m_subgroup_extramargin;

	/** The image width of the [+] icon. This is also calculated in the gutter */
	int                 m_iconwidth;

#ifndef wxPG_ICON_WIDTH

	/** The image height of the [+] icon. This is calculated as minimal size and to align */
	int                 m_iconheight;
#endif

    /** Current cursor id. */
    int                 m_curcursor;

	/** This captionFont is made equal to the font of the wxScrolledWindow. As extra the bold face
	    is set on it when this is wanted by the user (see flags) */
	wxFont              m_captionFont;

#if !wxPG_HEAVY_GFX
    int                 m_splitterprevdrawnx;

    /** Pen used to draw splitter column when it is being dragged. */
    wxPen               m_splitterpen;

#endif

	int                 m_fontHeight;  // height of the font

    // Most recently added category.
    //wxPGPropertyWithChildren*   m_currentCategory;

    //
    // Temporary values
    //

    /** m_splitterx when drag began. */
    int                 m_startingSplitterX;

    /** 0 = not dragging, 1 = drag just started, 2 = drag in progress */
    unsigned char       m_dragStatus;

    /** x - m_splitterx. */
    signed char         m_dragOffset;

    /** 0 = margin, 1 = label, 2 = value.*/
    unsigned char       m_mouseSide;

    /** True when editor control is focused. */
    unsigned char       m_editorFocused;

    /** Bits are used to indicate which colours are customized. */
    unsigned char       m_coloursCustomized;

    /** 1 if m_latsCaption is also the bottommost caption. */
    //unsigned char       m_lastCaptionBottomnest;

    /** Set to 1 when graphics frozen. */
    unsigned char       m_frozen;

    unsigned char       m_vspacing;

    /** Internal flags - see wxPG_FL_XXX constants. */
    wxUint32            m_iFlags;

    /** When drawing next time, clear this many item slots at the end. */
    int                 m_clearThisMany;

    /** Pointer to selected property. Note that this is duplicated in
        m_state for better transiency between pages so that the selected
        item can be retained.
    */
    wxPGProperty*       m_selected;

    wxPGProperty*       m_propHover;    // pointer to property that has mouse on itself

    int                 m_splitterx; // x position for the vertical line dividing name and value

#if !wxPG_USE_CUSTOM_CONTROLS

    int                 m_ctrlXAdjust; // x relative to splitter (needed for resize).
#endif

    wxColour            m_colLine;     // lines between cells
    wxColour            m_colPropFore; // property names and texts are written in this color
    wxColour            m_colPropBack; // background for m_colPropFore
    wxColour            m_colCapFore;  // text color for captions
    wxColour            m_colCapBack;  // background color for captions
    wxColour            m_colSelFore;  // foreground for selected property
    wxColour            m_colSelBack;  // background for selected property (actually use background color when control out-of-focus)
    wxColour            m_colMargin;   // background colour for margin

    wxArrayPtrVoid      m_arrBgBrushes; // Array of background colour brushes.

#if wxPG_USE_CUSTOM_CONTROLS
    wxCustomControlManager  m_ccManager;
#endif

    wxArrayString       m_sl;           // string control helper

    //wxPGCtrlClass*      m_prevSelected; // delete this ASAP

protected:

    // Sets some members to defaults.
	void Init1();

    // Initializes some members.
	void Init2();

	void OnPaint (wxPaintEvent &event );

    // main event receivers
#if wxPG_USE_CUSTOM_CONTROLS
    void OnMouseEvent ( wxMouseEvent &event );
#else
    void OnMouseMove ( wxMouseEvent &event );
    void OnMouseClick ( wxMouseEvent &event );
    void OnMouseRightClick ( wxMouseEvent &event );
    void OnMouseUp ( wxMouseEvent &event );
#endif
    void OnKey ( wxKeyEvent &event );
#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )
    void OnNavigationKey ( wxNavigationKeyEvent& event );
#if !wxPG_USE_CUSTOM_CONTROLS
    void SendNavigationKeyEvent( int dir );
#endif
#endif
    void OnResize ( wxSizeEvent &event );
    //virtual void OnSize ( wxSizeEvent &event );

    // event handlers
    bool HandleMouseMove ( int x, unsigned int y, wxMouseEvent &event );
    bool HandleMouseClick ( int x, unsigned int y, wxMouseEvent &event );
    bool HandleMouseRightClick( int x, unsigned int y, wxMouseEvent &event );
    bool HandleMouseUp ( int x, unsigned int y, wxMouseEvent &event );
    void HandleKeyEvent( wxKeyEvent &event );
    bool HandleChildKey ( wxKeyEvent& event, bool canDestroy ); // Handle TAB and ESCAPE in control

    void OnMouseEntry ( wxMouseEvent &event );

    //void OnSetFocus ( wxFocusEvent &event );
    //void OnKillFocus ( wxFocusEvent &event );

#if !wxPG_USE_CUSTOM_CONTROLS

    bool OnMouseCommon ( wxMouseEvent &event, int* px, int *py );
    bool OnMouseChildCommon ( wxMouseEvent &event, int* px, int *py );

    // sub-control event handlers
    void OnMouseClickChild ( wxMouseEvent &event );
    void OnMouseRightClickChild ( wxMouseEvent &event );
    void OnMouseMoveChild ( wxMouseEvent &event );
    void OnMouseUpChild ( wxMouseEvent &event );
    void OnKeyChild ( wxKeyEvent &event );

    void OnCaptureChange ( wxMouseCaptureChangedEvent &event );
#endif // !wxPG_USE_CUSTOM_CONTROLS
    void OnFocusEvent ( wxFocusEvent &event );

    void OnScrollEvent ( wxScrollWinEvent &event );

    void OnSysColourChanged ( wxSysColourChangedEvent &event );

protected:
//private:

	/** Adjust the centering of the bitmap icons (collapse / expand) when the caption font changes. They need to
	    be centered in the middle of the font, so a bit of deltaY adjustment is needed.
        On entry, m_captionFont must be set to window font. It will be modified properly.
    */
	void CalculateFontAndBitmapStuff( int vspacing );

    //void ClearValueBackground ( wxDC& dc, wxPGProperty* p );

    inline wxRect GetEditorWidgetRect ( wxPGProperty* p );

    //inline wxSize GetEditorWidgetSize ( wxPGProperty* p );

    //void CorrectEditorWidgetSizeX ( int cx, int cw );
    void CorrectEditorWidgetSizeX ( int new_splitterx, int new_width );

    //void CorrectEditorWidgetSizeY ( int cy );

#ifdef __WXDEBUG__
    void _log_items ();
    void OnScreenNote ( const wxChar* format, ... );
#endif

    void Delete ( wxPGProperty* item );

    void SelectProperty ( wxPGProperty* p, bool focus = FALSE, bool forceswitch = FALSE, bool nonvisible = FALSE );

    void DoDelete ( wxPGProperty* item );

    void DoDrawItems ( wxDC& dc,
        wxPGProperty* first_item, wxPGProperty* last_item,
        const wxRect* clip_rect );

    virtual wxPGId DoGetPropertyByName ( wxPGNameStr name ) const;

    /** Draws items from topitemy to bottomitemy */
    void DrawItems ( wxDC& dc, unsigned int topitemy, unsigned int bottomitemy,
        const wxRect* clip_rect = NULL );

    void DrawItems ( wxPGProperty* p1, wxPGProperty* p2 );

    void DrawItem ( wxDC& dc, wxPGProperty* p );

    void DrawItemAndChildren ( wxDC& dc, wxPGProperty* p );

    inline void DrawItem ( wxPGProperty* p );

    virtual void DrawItemAndChildren ( wxPGProperty* p );

    /** Returns property reference for given property id. */
    inline wxPGProperty& GetPropertyById ( wxPGId id )
    {
        return id.GetProperty();
    }

    static wxPropertyCategoryClass* GetPropertyCategory ( wxPGProperty* p );

    void ImprovedClientToScreen ( int* px, int* py );

    wxPGId Insert ( wxPGProperty* priorthis, wxPGProperty* newproperty );

    inline wxPGId Insert ( wxPGPropertyWithChildren* parent, int index, wxPGProperty* newproperty )
    {
        return m_pState->Insert(parent,index,newproperty);
    }

    /** Reloads all non-customized colours from system settings. */
    void RegainColours ();

    wxPGProperty* DoGetItemAtY ( int y );

    inline wxPGProperty* DoGetItemAtY_Full ( int y )
    {
        wxASSERT ( y >= 0 );

        if ( (unsigned int)y >= m_bottomy )
            return NULL;

        return m_pState->m_properties->GetItemAtY ( y, m_lineHeight );
    }

    void DoPropertyChanged ( wxDC& dc, wxPGProperty* p );

    void DoSetPropertyPriority ( wxPGProperty* p, int priority );

    wxPGProperty* GetLastItem ( bool need_visible, bool allow_subprops = TRUE );

    void CalculateVisibles ( int vy, bool full_recalc );

    void CalculateYs ( wxPGPropertyWithChildren* startparent,
        int startindex );

    /** Forces updating the value of property from the editor control.
        Returns true if DoPropertyChanged was actually called.
    */
    bool CommitChangesFromEditor();

    bool Expand ( wxPGProperty* p );

    bool Collapse ( wxPGProperty* p );

    // Returns nearest paint visible property (such that will be painted unless
    // window is scrolled or resized). If given property is paint visible, then
    // it itself will be returned.
    wxPGProperty* GetNearestPaintVisible ( wxPGProperty* p );

    void NavigateOut ( int dir, bool isChildFocused = false ); // this will then call wxWindow::Navigate

    static void RegisterDefaultEditors();

    static void RegisterDefaultValues();

    // Sets m_bgColIndex to this property and all its children.
    void SetColourIndex ( wxPGProperty* p, int index );

    void SetPropertyLabel ( wxPGProperty* p, const wxString& newproplabel );

    void DoSetPropertyName ( wxPGProperty* p, const wxString& newname );

    void SetPropertyValue ( wxPGId id, const wxPGValueType* typeclass, wxPGVariant value );

    void SetPropertyValue ( wxPGId id, const wxChar* typestring, wxPGVariant value );

    // Setups event handling for child control
    void SetupEventHandling ( wxPGCtrlClass* wnd, int id );

    //void Sort ( bool recalcys );

    //void Sort ( wxPGProperty* p, bool recalcys );

    void CustSetCursor ( int type );

    void RecalculateVirtualSize();

    void PGAdjustScrollbars ( int y );

    /** When splitter is dragged to a new position, this is drawn. */
    void DrawSplitterDragColumn ( wxDC& dc, int x );

    /** If given index is -1, scans for item to either up (dir=0) or down (dir!=0) */
    //int GetNearestValidItem ( int index, int dir );
    wxPGProperty* GetNeighbourItem ( wxPGProperty* item, bool need_visible,
        int dir ) const;

    //inline int PrepareToAddItem( wxPGProperty* property, wxPGPropertyWithChildren* category );

    void PrepareAfterItemsAdded();

#endif // DOXYGEN_SHOULD_SKIP_THIS

private:
    DECLARE_EVENT_TABLE()
};

#undef wxPG_USE_STATE

// -----------------------------------------------------------------------

inline bool wxPropertyGridState::IsDisplayed() const
{
    return ( this == m_pPropGrid->GetState() );
}

// -----------------------------------------------------------------------

/** \class wxPropertyGridEvent
	\ingroup classes
    \brief A propertygrid event holds information about events associated with
    wxPropertyGrid objects.

    <h4>Derived from</h4>

    wxNotifyEvent\n
    wxCommandEvent\n
    wxEvent\n
    wxObject\n

    <h4>Include files</h4>

    <wx/propertygrid/propertygrid.h>
*/
class WXDLLIMPEXP_PG wxPropertyGridEvent : public wxCommandEvent
{
public:

    /** Constructor. */
    wxPropertyGridEvent(wxEventType commandType=0, int id=0);
#ifndef SWIG
    /** Copy constructor. */
    wxPropertyGridEvent(const wxPropertyGridEvent& event);
#endif
    /** Destructor. */
    ~wxPropertyGridEvent();

    /** Copyer. */
    virtual wxEvent* Clone() const;

    /** Enables property. */
    inline void EnableProperty( bool enable = TRUE )
    {
        m_pg->EnableProperty(wxPGIdGen(m_property),enable);
    }

    /** Disables property. */
    inline void DisableProperty()
    {
        m_pg->EnableProperty(wxPGIdGen(m_property),FALSE);
    }

    /** Returns id of associated property. */
    wxPGId GetProperty() const
    {
        return wxPGIdGen(m_property);
    }

    /** Returns pointer to associated property. */
    wxPGProperty* GetPropertyPtr() const
    {
        return m_property;
    }

    /** Returns label of associated property. */
    const wxString& GetPropertyLabel() const
    {
        wxASSERT ( m_property );
        return m_property->GetLabel();
    }

    /** Returns name of associated property. */
    const wxString& GetPropertyName() const
    {
        wxASSERT ( m_property );
        return m_property->GetName();
    }

#if wxPG_USE_CLIENT_DATA
    /** Returns client data of relevant property. */
    void* GetPropertyClientData() const
    {
        wxASSERT ( m_property );
        return m_property->GetClientData();
    }
#endif

    /** Returns value of relevant property. */
#if wxPG_EMBED_VARIANT
    const wxVariant& GetPropertyValue() const
#else
    wxVariant GetPropertyValue() const
#endif
    {
        wxASSERT ( m_property );
        return m_property->GetValueAsVariant();
    }

    inline wxString GetPropertyValueAsString () const
    {
        return m_pg->GetPropertyValueAsString( wxPGIdGen(m_property) );
    }
    inline long GetPropertyValueAsLong () const
    {
        return m_pg->GetPropertyValueAsLong( wxPGIdGen(m_property) );
    }
    inline int GetPropertyValueAsInt () { return (int)GetPropertyValueAsLong(); }
    inline long GetPropertyValueAsBool () const
    {
        return m_pg->GetPropertyValueAsBool( wxPGIdGen(m_property) );
    }
    inline double GetPropertyValueAsDouble () const
    {
        return m_pg->GetPropertyValueAsDouble( wxPGIdGen(m_property) );
    }
    inline const wxArrayString& GetPropertyValueAsArrayString () const
    {
        return m_pg->GetPropertyValueAsArrayString( wxPGIdGen(m_property) );
    }
    inline const wxObject* GetPropertyValueAsWxObjectPtr () const
    {
        return m_pg->GetPropertyValueAsWxObjectPtr( wxPGIdGen(m_property) );
    }
    inline void* GetPropertyValueAsVoidPtr () const
    {
        return m_pg->GetPropertyValueAsVoidPtr( wxPGIdGen(m_property) );
    }
    inline const wxPoint& GetPropertyValueAsPoint () const
    {
        return m_pg->GetPropertyValueAsPoint( wxPGIdGen(m_property) );
    }
    inline const wxSize& GetPropertyValueAsSize () const
    {
        return m_pg->GetPropertyValueAsSize( wxPGIdGen(m_property) );
    }
    inline const wxArrayInt& GetPropertyValueAsArrayInt () const
    {
        return m_pg->GetPropertyValueAsArrayInt( wxPGIdGen(m_property) );
    }

    /** Returns value type of relevant property. */
    const wxPGValueType* GetPropertyValueType() const
    {
        return m_pg->GetPropertyValueType( wxPGIdGen(m_property) );
    }

    /** Returns TRUE if event has associated property. */
    inline bool HasProperty() const { return ( m_property != (wxPGProperty*) NULL ); }

    inline bool IsPropertyEnabled() const
    {
        return m_pg->IsPropertyEnabled(wxPGIdGen(m_property));
    }

    /** Changes the associated property. */
    void SetProperty ( wxPGId id ) { m_property = wxPGIdToPtr(id); }

    /** Changes the associated property. */
    void SetProperty ( wxPGProperty* p ) { m_property = p; }

    void SetPropertyGrid ( wxPropertyGrid* pg ) { m_pg = pg; }

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS(wxPropertyGridEvent)

    wxPGProperty*       m_property;
    wxPropertyGrid*     m_pg;
#endif
};

#ifndef SWIG
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PG, wxEVT_PG_SELECTED,           1775)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PG, wxEVT_PG_CHANGED,            1776)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PG, wxEVT_PG_HIGHLIGHTED,        1777)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PG, wxEVT_PG_RIGHT_CLICK,        1778)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_PG, wxEVT_PG_PAGE_CHANGED,       1779)

END_DECLARE_EVENT_TYPES()
#else
    enum {
        wxEVT_PG_SELECTED = 1775,
        wxEVT_PG_CHANGED,
        wxEVT_PG_HIGHLIGHTED,
        wxEVT_PG_RIGHT_CLICK,
        wxEVT_PG_PAGE_CHANGED,
    };
#endif


#ifndef SWIG
typedef void (wxEvtHandler::*wxPropertyGridEventFunction)(wxPropertyGridEvent&);

#define EVT_PG_SELECTED(id, fn)              DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxPropertyGridEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PG_CHANGED(id, fn)               DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxPropertyGridEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PG_HIGHLIGHTED(id, fn)           DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_HIGHLIGHTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxPropertyGridEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PG_RIGHT_CLICK(id, fn)           DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxPropertyGridEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_PG_PAGE_CHANGED(id, fn)          DECLARE_EVENT_TABLE_ENTRY( wxEVT_PG_PAGE_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxPropertyGridEventFunction, & fn ), (wxObject *) NULL ),

#endif


// -----------------------------------------------------------------------


/** \class wxPropertyGridPopulator
    \ingroup classes
    \brief Allows populating wxPropertyGrid from arbitrary text source.
*/
class WXDLLIMPEXP_PG wxPropertyGridPopulator
{
public:
    /** Constructor.
        \param pg
        Property grid to populate.
        \param popRoot
        Base parent property. Default is root.
    */
    inline wxPropertyGridPopulator(wxPropertyGrid* pg = (wxPropertyGrid*) NULL,
                                   wxPGId popRoot = wxPGId())
    {
        Init(pg, popRoot);
    }

    /** Destructor. */
    ~wxPropertyGridPopulator();

    /** Adds a new set of choices with given id, labels and optional values.
        \remarks
        choicesId can be any id unique in source (so it does not conflict
        with sets of choices created before population process).
    */
    void AddChoices(size_t choicesId,
                    const wxArrayString& choiceLabels,
                    const wxArrayInt& choiceValues = *((const wxArrayInt*)NULL));

    /** Appends a property under current parent.
        \param classname
        Class name of a property. Understands both wxXXXProperty
        and XXX style names. Thus, for example, wxStringProperty
        could be created with class names "wxStringProperty", and
        "String". Short class name of wxPropertyCategory is
        "Category".
        \param label
        Label for property. Use as in constructor functions.
        \param name
        Name for property. Use as in constructor functions.
        \param value
        Value for property is interpreted from this string.
        \param attributes
        Attributes of a property (both pseudo-attributes like
        "Disabled" and "Modified" in addition to real ones
        like "Precision") are read from this string. Is intended
        for string like one generated by GetPropertyAttributes.
        \param choicesId
        If non-zero: Id for set of choices unique in source. Pass
        either id previously given to AddChoices or a new one.
        If new is given, then choiceLabels and choiceValues are
        loaded as the contents for the newly created set of choices.
        \param choiceLabels
        List of choice labels.
        \param choiceValues
        List of choice values.
    */
    wxPGId AppendByClass(const wxString& classname,
                         const wxString& label,
                         const wxString& name = wxPG_LABEL,
                         const wxString& value = wxEmptyString,
                         const wxString& attributes = wxEmptyString,
                         size_t choicesId = 0,
                         const wxArrayString& choiceLabels = *((const wxArrayString*)NULL),
                         const wxArrayInt& choiceValues = *((const wxArrayInt*)NULL));

    /** Appends a property under current parent. Works just as
        AppendByClass, except accepts value type name instead of
        class name (value type name of a property can be queried using
        wxPropertyGrid::GetPropertyValueType(property)->GetType()).

        \remarks
        <b>Cannot</b> generate property category.
    */
    wxPGId AppendByType(const wxString& valuetype,
                        const wxString& label,
                        const wxString& name = wxPG_LABEL,
                        const wxString& value = wxEmptyString,
                        const wxString& attributes = wxEmptyString,
                        size_t choicesId = 0,
                        const wxArrayString& choiceLabels = *((const wxArrayString*)NULL),
                        const wxArrayInt& choiceValues = *((const wxArrayInt*)NULL));

    /** Returns id of parent property for which children can currently be added. */
    inline wxPGId GetCurrentParent() const
    {
        return m_curParent;
    }

    /** Returns true if set of choices with given id has already been added. */
    bool HasChoices( size_t id ) const;

    /** Sets the property grid to be populated. */
    inline void SetGrid( wxPropertyGrid* pg )
    {
        m_propGrid = pg;
    }

    /** If possible, sets the property last added as current parent. */
    bool BeginChildren();

    /** Terminates current parent - sets its parent as the new current parent. */
    inline void EndChildren()
    {
        wxASSERT( m_curParent.IsOk() );
        m_curParent = wxPGIdGen(wxPGIdToPtr(m_curParent)->GetParent());
        m_lastProperty = wxPGId((wxPGProperty*)NULL);
    }

protected:

    wxPGId DoAppend(wxPGProperty* p,
                    const wxString& value,
                    const wxString& attributes,
                    size_t choicesId,
                    const wxArrayString& choiceLabels,
                    const wxArrayInt& choiceValues);

    void Init( wxPropertyGrid* pg, wxPGId popRoot );

    /** Used property grid. */
    wxPropertyGrid* m_propGrid;

    /** Population root. */
    wxPGId          m_popRoot;

    /** Parent of currently added properties. */
    wxPGId          m_curParent;

    /** Id of property last added. */
    wxPGId          m_lastProperty;

    /** Hashmap for source-choices-id to real-choices-id mapping. */
    wxPGHashMapI2I  m_dictIdChoices;
};

// -----------------------------------------------------------------------

//
// Undefine macros that are not needed outside propertygrid sources
//
#ifndef __wxPG_SOURCE_FILE__
# undef wxPG_FL_DESC_REFRESH_REQUIRED
# undef wxPG_FL_SCROLLBAR_DETECTED
# undef wxPG_FL_CREATEDSTATE
# undef wxPG_FL_NOSTATUSBARHELP
# undef wxPG_FL_SCROLLED
# undef wxPG_FL_HIDE_STATE
# undef wxPG_FL_FOCUS_INSIDE_CHILD
# undef wxPG_FL_FOCUS_INSIDE
# undef wxPG_FL_MOUSE_INSIDE_CHILD
# undef wxPG_FL_CUR_USES_CUSTOM_IMAGE
# undef wxPG_FL_PRIMARY_FILLS_ENTIRE
# undef wxPG_FL_VALUE_MODIFIED
# undef wxPG_FL_MOUSE_INSIDE
# undef wxPG_FL_FOCUSED
# undef wxPG_FL_MOUSE_CAPTURED
# undef wxPG_FL_INITIALIZED
# undef wxPG_FL_ACTIVATION_BY_CLICK
# undef wxPG_FL_DONT_CENTER_SPLITTER
# undef wxPG_SUPPORT_TOOLTIPS
# undef wxPG_USE_CUSTOM_CONTROLS
# undef wxPG_DOUBLE_BUFFER
# undef wxPG_HEAVY_GFX
# undef wxPG_ICON_WIDTH
# undef wxPG_USE_RENDERER_NATIVE
// Following are needed by the manager headers
//# undef wxPGIdGen
//# undef wxPGNameStr
//# undef wxPGIdToPtr
#endif

// Doxygen special
#if !defined(_WX_WINDOW_H_BASE_) && !defined(SWIG)
# include "manager.h"
#endif

// -----------------------------------------------------------------------

#endif // __WX_PROPGRID_H__
