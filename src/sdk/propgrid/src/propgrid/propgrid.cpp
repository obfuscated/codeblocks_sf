/////////////////////////////////////////////////////////////////////////////
// Name:        propgrid.cpp
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "propgrid.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/panel.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/button.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/choice.h"
    #include "wx/stattext.h"
    #include "wx/scrolwin.h"
    #include "wx/dirdlg.h"
    #include "wx/combobox.h"
    #include "wx/layout.h"
    #include "wx/sizer.h"
    #include "wx/textdlg.h"
    #include "wx/filedlg.h"
    #include "wx/statusbr.h"
    #include "wx/intl.h"
#endif

#include <wx/filename.h>

#include <wx/colordlg.h>

// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include <wx/propgrid/propgrid.h>

#include <wx/propgrid/propdev.h>

#if wxPG_USE_RENDERER_NATIVE
# include <wx/renderer.h>
#endif

#if !wxPG_USE_CUSTOM_CONTROLS
# include <wx/propgrid/odcombo.h>
#endif

#define wxPGComboBox wxPGOwnerDrawnComboBox

// Two pics for the expand / collapse buttons.
// Files are not supplied with this project (since it is
// recommended to use either custom or native rendering).
// If you want them, get wxTreeMultiCtrl by Jorgen Bodde,
// and copy xpm files from archive to wxPropertyGrid src directory
// (and also comment/undef wxPG_ICON_WIDTH in propertygrid.h
// and set wxPG_USE_RENDERER_NATIVE to 0).
#ifndef wxPG_ICON_WIDTH
  #if defined(__WXMAC__)
    #include "mac_collapse.xpm"
    #include "mac_expand.xpm"
  #elif defined(__WXGTK__)
    #include "linux_collapse.xpm"
    #include "linux_expand.xpm"
  #else
    #include "default_collapse.xpm"
    #include "default_expand.xpm"
  #endif
#endif

#define wxPG_ALLOW_CLIPPING             1 // If 1, GetUpdateRegion() in OnPaint event handler is not ignored

#define wxPG_GUTTER_DIV                 3 // gutter is max(iconwidth/gutter_div,gutter_min)
#define wxPG_GUTTER_MIN                 3 // gutter before and after image of [+] or [-]

#define wxPG_YSPACING_MIN               1

#define wxPG_BUTTON_SIZEDEC             0

#define wxPG_DEFAULT_VSPACING           2 // This matches .NET propertygrid's value,
                                          // but causes normal combobox to spill out under MSW

#define wxPG_OPTIMAL_WIDTH              200

// space between caption and selection rectangle, horizontally and vertically
#define wxPG_CAPRECTXMARGIN             2
#define wxPG_CAPRECTYMARGIN             1


#define wxPG_MIN_SCROLLBAR_WIDTH         10 // Smallest scrollbar width on any platform
                                            // Must be larger than largest control border
                                            // width * 2.


#define wxPG_DEFAULT_CURSOR wxNullCursor


#define RedrawAllVisible Refresh

//
// Here are some extra platform dependent defines.
//

#if defined(__WXMSW__)
    // tested

    #define wxPG_DEFAULT_SPLITTERX      110 // default splitter position

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    0 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   1  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X          0 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y          0 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY          1
    #define wxPG_NAT_BUTTON_BORDER_X            1
    #define wxPG_NAT_BUTTON_BORDER_Y            1

    #define wxPG_TEXTCTRLXADJUST                3 // position adjustment for wxTextCtrl
    #define wxPG_TEXTCTRLYADJUST                (m_spacingy+0)

    #define wxPG_CHOICEXADJUST                  (-1) // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0 // Extra pixels above wxChoice/ComboBox.

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 0 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 1
    #define wxPG_CHECKMARK_YADJ                 (-1)
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

#elif defined(__WXGTK__)
    // tested

    #define wxPG_DEFAULT_SPLITTERX      110

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    1 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   0  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X      3 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y      3 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY      1
    #define wxPG_NAT_BUTTON_BORDER_X        1
    #define wxPG_NAT_BUTTON_BORDER_Y        1

    #define wxPG_TEXTCTRLXADJUST            3 // position adjustment for wxTextCtrl
    #define wxPG_TEXTCTRLYADJUST            0

    #define wxPG_CHOICEXADJUST                  2 // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 1 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 (-1)
    #define wxPG_CHECKMARK_HADJ                 (-1)
    #define wxPG_CHECKMARK_DEFLATE              3

#elif defined(__WXMAC__)
    // *not* tested

    #define wxPG_DEFAULT_SPLITTERX      110

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    0 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   1  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X      0 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y      0 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLXADJUST            3 // position adjustment for wxTextCtrl
    #define wxPG_TEXTCTRLYADJUST            3

    #define wxPG_CHOICEXADJUST                  0 // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 0 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

#else
    // defaults

    #define wxPG_DEFAULT_SPLITTERX      110

    #define wxPG_CREATE_CONTROLS_HIDDEN 0 // 1 to create controls out of sight, hide them, and then move them into correct position

    #define wxPG_NO_CHILD_EVT_MOTION    1 // 1 if splitter drag detect margin and control cannot overlap

    #define wxPG_CUSTOM_IMAGE_WIDTH     20 // for wxColourProperty etc.

    #define wxPG_ALLOW_EMPTY_TOOLTIPS   0  // If 1, then setting empty tooltip actually hides it

    #define wxPG_NAT_TEXTCTRL_BORDER_X      0 // Unremovable border of native textctrl.
    #define wxPG_NAT_TEXTCTRL_BORDER_Y      0 // Unremovable border of native textctrl.

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLXADJUST            0 // position adjustment for wxTextCtrl
    #define wxPG_TEXTCTRLYADJUST            0

    #define wxPG_CHOICEXADJUST                  0 // Extra pixels next to wxChoice/ComboBox.
    #define wxPG_CHOICEYADJUST                  0

    #define wxPG_REFRESH_CONTROLS_AFTER_REPAINT 1 // If 1 then controls are refreshed after selected was drawn.

    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

#endif


#ifndef wxCC_CORRECT_CONTROL_POSITION
# define wxCC_CORRECT_CONTROL_POSITION 0
#endif


#if wxPG_NO_CHILD_EVT_MOTION

# define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
# define wxPG_SPLITTERX_DETECTMARGIN2    2 // this much on right
# define wxPG_CONTROL_MARGIN             0 // space between splitter and control

#else

# define wxPG_SPLITTERX_DETECTMARGIN1    3 // this much on left
# define wxPG_SPLITTERX_DETECTMARGIN2    3 // this much on right
# define wxPG_CONTROL_MARGIN             0 // space between splitter and control

#endif

// Use same values for some defines when custom controls are used
#if wxPG_USE_CUSTOM_CONTROLS

# undef wxPG_XBEFOREWIDGET
# define wxPG_XBEFOREWIDGET              0

# undef wxPG_XBEFORETEXT
# define wxPG_XBEFORETEXT                (wxCC_TEXTCTRL_XSPACING+1)

# undef wxPG_CHOICEXADJUST
# define wxPG_CHOICEXADJUST              0

# undef wxPG_CHOICEYADJUST
# define wxPG_CHOICEYADJUST              0

# undef wxPG_XBEFORETEXT
# define wxPG_XBEFORETEXT                (wxCC_TEXTCTRL_XSPACING+1)

# undef wxPG_NAT_CHOICE_BORDER_ANY
# undef wxPG_NAT_BUTTON_BORDER_ANY

#else


#ifndef wxPG_NAT_CHOICE_BORDER_ANY
# define wxPG_NAT_CHOICE_BORDER_ANY  0
#endif


#if wxPG_USE_GENERIC_TEXTCTRL

// Generic textctrl gets basic coordinate offset
# undef wxPG_TEXTCTRLXADJUST
# define wxPG_TEXTCTRLXADJUST 0
# undef wxPG_TEXTCTRLYADJUST
# define wxPG_TEXTCTRLYADJUST -1

#endif

# define wxCC_CUSTOM_IMAGE_MARGIN1          4  // before image
# define wxCC_CUSTOM_IMAGE_MARGIN2          5  // after image

#endif

#if (!wxPG_NAT_TEXTCTRL_BORDER_X && !wxPG_NAT_TEXTCTRL_BORDER_Y) || wxPG_USE_CUSTOM_CONTROLS
    #define wxPG_ENABLE_CLIPPER_WINDOW      0
#else
    #define wxPG_ENABLE_CLIPPER_WINDOW      1
#endif

// for odcombo
#undef wxPG_CHOICEXADJUST
#define wxPG_CHOICEXADJUST           0
#undef wxPG_CHOICEYADJUST
#define wxPG_CHOICEYADJUST           0

#define wxPG_DRAG_MARGIN                30

//#define wxPG_CUSTOM_IMAGE_MARGIN        1 // space between horizontal sides of a custom image
#define wxPG_CUSTOM_IMAGE_SPACINGY      1 // space between vertical sides of a custom image

// Use this macro to generate standard custom image height from
#define wxPG_STD_CUST_IMAGE_HEIGHT(LINEHEIGHT)  (LINEHEIGHT-3)

// How many pixels between textctrl and button (not used by custom controls)
#define wxPG_TEXTCTRL_AND_BUTTON_SPACING        2

#define wxPG_HIDER_BUTTON_HEIGHT        25

// m_expanded of wxPGPropertyWithChildren is set to this if children should
// not be deleted in destructor.
#define wxPG_EXP_OF_COPYARRAY           127

#define wxPG_PIXELS_PER_UNIT            m_lineHeight

#ifdef wxPG_ICON_WIDTH
  #define m_iconheight m_iconwidth
#endif

#define wxPG_TOOLTIP_DELAY              1000


// Colour for the empty but visible space below last property.
#define wxPG_SLACK_BACKROUND        m_colPropBack

#define __INTENSE_DEBUGGING__       0
#define __PAINT_DEBUGGING__         0
#define __MOUSE_DEBUGGING__         0

// -----------------------------------------------------------------------

#if wxUSE_INTL
void wxPropertyGrid::AutoGetTranslation ( bool enable )
{
    wxPGGlobalVars->m_autoGetTranslation = enable;
}
#else
void wxPropertyGrid::AutoGetTranslation ( bool ) { }
#endif

// -----------------------------------------------------------------------

// This was needed to make progress towards using wxPropertyGridState faster.
#define FROM_STATE(X) m_pState->X

// -----------------------------------------------------------------------

// DeviceContext Init Macros.

#define wxPG_CLIENT_DC_INIT() \
    wxClientDC dc(this); \
    PrepareDC(dc);

#define wxPG_CLIENT_DC_INIT_R(RETVAL) \
    wxClientDC dc(this); \
    PrepareDC(dc);

#define wxPG_PAINT_DC_INIT() \
    wxPaintDC dc(this); \
    PrepareDC(dc);

// -----------------------------------------------------------------------

#if !wxPG_USE_CUSTOM_CONTROLS
# define wxPG_SETFONT_ADDENDUM
#else
# define wxPG_SETFONT_ADDENDUM , &dc
#endif

// -----------------------------------------------------------------------

// For wxMSW cursor consistency, we must do mouse capturing even
// when using custom controls.

# define BEGIN_MOUSE_CAPTURE \
    if ( !(m_iFlags & wxPG_FL_MOUSE_CAPTURED) ) \
    { \
        CaptureMouse(); \
        m_iFlags |= wxPG_FL_MOUSE_CAPTURED; \
    }

# define END_MOUSE_CAPTURE \
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED ) \
    { \
        ReleaseMouse(); \
        m_iFlags &= ~(wxPG_FL_MOUSE_CAPTURED); \
    }

// -----------------------------------------------------------------------
// NOTES
// -----------------------------------------------------------------------

//
// v1.1 Design:
// * Temporary folder and file removal on install and uninstall.
// * Get rid of custom controls.
// * Make white margin bg default.
// * SetAttribute arg "wxVariant" to "const wxVariant&"
// * wxPGPropertyExtData (for easy value type, editor, validator etc setting)
// * Rename wxXXX constants to wxPG_XXX.
// * If it appears that names are almost always wxPG_LABEL, get rid of
//   them from property constructors. Maybe provide optional backwards
//   compatibility.
// * Escape allows canceling edited value.
// * Bring back property class inheritance info (inherit wxPGProperty
//   from wxObject).
// * Zero-argument constructor (pass name and string in constructor
//   function instead) to allow best use for wxClassInfo::CreateObject.
// * Use wxObject::m_refData as storage for client data.
// * wxVariant values for AddPropertyChoice.
// * Consider: variant values would allow really flexible system.
//

//
// API Questions:
//   - id.Ok instead of id.IsOk? Treectrlid has .IsOk so that is probably ok.
//

//
// -----------------------------------------------------------------------
// TODO
// -----------------------------------------------------------------------
//

//
// Nothing here any more -- all TODO's moved to 1.1.x's propgrid.cpp.
//

// -----------------------------------------------------------------------

const wxChar *wxPropertyGridNameStr = wxT("wxPropertyGrid");

const wxChar *wxPGTypeName_long = wxT("long");
const wxChar *wxPGTypeName_bool = wxT("bool");
const wxChar *wxPGTypeName_double = wxT("double");
const wxChar *wxPGTypeName_wxString = wxT("string");
const wxChar *wxPGTypeName_void = wxT("void*");
const wxChar *wxPGTypeName_wxArrayString = wxT("arrstring");

// -----------------------------------------------------------------------
// wxStringProperty
// -----------------------------------------------------------------------

wxPG_BEGIN_PROPERTY_CLASS_BODY(wxStringProperty,wxPGProperty,wxString,const wxString&)
    WX_PG_DECLARE_BASIC_TYPE_METHODS()
wxPG_END_PROPERTY_CLASS_BODY()

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxStringProperty,wxString,const wxString&,TextCtrl)

wxStringPropertyClass::wxStringPropertyClass ( const wxString& label, const wxString& name,
    const wxString& value ) : wxPGProperty(label,name)
{
    DoSetValue(value);
}

wxStringPropertyClass::~wxStringPropertyClass () { }

void wxStringPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = wxPGVariantToString(value);

    wxPG_SetVariantValue(m_value);
}

wxPGVariant wxStringPropertyClass::DoGetValue () const
{
    return wxPGVariant(m_value);
}

wxString wxStringPropertyClass::GetValueAsString ( int ) const
{
    return m_value;
}

bool wxStringPropertyClass::SetValueFromString ( const wxString& text, int )
{
    if ( m_value != text )
        return StdValidationProcedure(text);

    return FALSE;
}

// -----------------------------------------------------------------------
// wxIntProperty
// -----------------------------------------------------------------------

wxPG_BEGIN_PROPERTY_CLASS_BODY(wxIntProperty,wxPGProperty,long,long)
    WX_PG_DECLARE_BASIC_TYPE_METHODS()
wxPG_END_PROPERTY_CLASS_BODY()

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxIntProperty,long,long,TextCtrl)

wxIntPropertyClass::wxIntPropertyClass ( const wxString& label, const wxString& name,
    long value ) : wxPGProperty(label,name)
{
    DoSetValue(value);
}

wxIntPropertyClass::~wxIntPropertyClass () { }

void wxIntPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = wxPGVariantToLong(value);
    wxPG_SetVariantValue(m_value);
}

wxPGVariant wxIntPropertyClass::DoGetValue () const
{
    return wxPGVariant(m_value);
}

wxString wxIntPropertyClass::GetValueAsString ( int ) const
{
    wxString temp;
    temp.Printf(wxT("%li"),m_value);
    return temp;
}

bool wxIntPropertyClass::SetValueFromString ( const wxString& text, int arg_flags )
{
    wxString s;
    long value;

    if ( text.IsNumber() )
    {
        text.ToLong(&value,0); // we know its number, so need to check retval

        if ( m_value != value )
        {
            return StdValidationProcedure(value);
        }
    }
    else if ( arg_flags & wxPG_REPORT_ERROR )
    {
        s.Printf ( wxT("! %s: \"%s\" is not a number."), m_label.c_str(), text.c_str() );
        ShowError(s);
    }
    return FALSE;
}

// -----------------------------------------------------------------------
// wxUIntProperty
// -----------------------------------------------------------------------


#define wxPG_UINT_TEMPLATE_MAX 8

static const wxChar* gs_uintTemplates[wxPG_UINT_TEMPLATE_MAX] = {
    wxT("%x"),wxT("0x%x"),wxT("$%x"),
    wxT("%X"),wxT("0x%X"),wxT("$%X"),
    wxT("%u"),wxT("%o")
};

wxPG_BEGIN_PROPERTY_CLASS_BODY(wxUIntProperty,wxPGProperty,long,unsigned long)
    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_ATTRIBUTE_METHODS()
    virtual bool SetValueFromInt ( long value, int flags );
protected:
    wxByte      m_base;
    wxByte      m_realBase; // translated to 8,16,etc.
    wxByte      m_prefix;
wxPG_END_PROPERTY_CLASS_BODY()

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxUIntProperty,long,unsigned long,TextCtrl)

wxUIntPropertyClass::wxUIntPropertyClass ( const wxString& label, const wxString& name,
    unsigned long value ) : wxPGProperty(label,name)
{
    m_base = 6; // This is magic number for dec base (must be same as in setattribute)
    m_realBase = 10;
    m_prefix = wxPG_PREFIX_NONE;

    DoSetValue((long)value);
}

wxUIntPropertyClass::~wxUIntPropertyClass () { }

void wxUIntPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = wxPGVariantToLong(value);
}

wxPGVariant wxUIntPropertyClass::DoGetValue () const
{
    return wxPGVariant(m_value);
}

wxString wxUIntPropertyClass::GetValueAsString ( int ) const
{
    //return wxString::Format(wxPGGlobalVars->m_uintTemplate.c_str(),m_value);

    size_t index = m_base + m_prefix;
    if ( index >= wxPG_UINT_TEMPLATE_MAX )
        index = wxPG_BASE_DEC;

    return wxString::Format(gs_uintTemplates[index],m_value);
}

bool wxUIntPropertyClass::SetValueFromString ( const wxString& text, int WXUNUSED(arg_flags) )
{
    wxString s;
    long value = 0;

    const wxChar *start = text.c_str();
    if ( text[0] && !wxIsalnum(text[0]) )
        start++;

    wxChar *end;
    value = wxStrtoul(start, &end, (unsigned int)m_realBase);

    if ( m_value != value )
    {
        return StdValidationProcedure(value);
    }
    return false;
}

bool wxUIntPropertyClass::SetValueFromInt( long value, int WXUNUSED(flags) )
{
    if ( m_value != value )
    {
        m_value = value;
        return true;
    }
    return false;
}

void wxUIntPropertyClass::SetAttribute ( int id, wxVariant value )
{
    if ( id == wxPG_UINT_BASE )
    {
        int val = value.GetLong();

        m_realBase = (wxByte) val;
        if ( m_realBase > 16 )
            m_realBase = 16;

        //
        // Translate logical base to a template array index
        m_base = 7; // oct
        if ( val == wxPG_BASE_HEX )
            m_base = 3;
        else if ( val == wxPG_BASE_DEC )
            m_base = 6;
        else if ( val == wxPG_BASE_HEXL )
            m_base = 0;
    }
    else if ( id == wxPG_UINT_PREFIX )
        m_prefix = (wxByte) value.GetLong();
}

// -----------------------------------------------------------------------
// wxFloatProperty
// -----------------------------------------------------------------------

wxPG_BEGIN_PROPERTY_CLASS_BODY(wxFloatProperty,wxPGProperty,double,double)
    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_ATTRIBUTE_METHODS()
protected:
    int m_precision;
wxPG_END_PROPERTY_CLASS_BODY()

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxFloatProperty,double,double,TextCtrl)

wxFloatPropertyClass::wxFloatPropertyClass ( const wxString& label, const wxString& name,
    double value ) : wxPGProperty(label,name)
{
    m_precision = -1;
    DoSetValue(value);
}

wxFloatPropertyClass::~wxFloatPropertyClass () { }

void wxFloatPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = wxPGVariantToDouble(value);

    wxPG_SetVariantValue(m_value);
}

wxPGVariant wxFloatPropertyClass::DoGetValue () const
{
    return wxPGVariant(m_value);
}

// This helper method provides standard way for floating point-using
// properties to convert values to string.
void wxPropertyGrid::DoubleToString(wxString& target,
                                    double value,
                                    int precision,
                                    bool removeZeroes,
                                    wxString* precTemplate)
{
    if ( precision >= 0 )
    {
        wxString text1;
        if (!precTemplate)
            precTemplate = &text1;

        if ( !precTemplate->length() )
        {
            *precTemplate = wxT("%.");
            *precTemplate << wxString::Format( wxT("%i"), precision );
            *precTemplate << wxT('f');
        }

        target.Printf ( precTemplate->c_str(), value );

    }
    else
    {
        target.Printf ( wxT("%f"), value );
    }

    if ( removeZeroes && precision != 0 )
    {
        // Remove excess zeroes (do not remove this code just yet)
        int cur_pos = target.length() - 1;
        wxChar a;
        a = target.GetChar ( cur_pos );
        while ( a == '0' && cur_pos > 0 )
        {
            cur_pos--;
            a = target.GetChar ( cur_pos );
        }

        wxChar cur_char = target.GetChar ( cur_pos );
        if ( cur_char != wxT('.') && cur_char != wxT(',') )
        {
            cur_pos += 1;
            cur_char = target.GetChar ( cur_pos );
        }

        target.Truncate ( cur_pos );
    }
}

wxString wxFloatPropertyClass::GetValueAsString ( int arg_flags ) const
{
    wxString text;
    wxPropertyGrid::DoubleToString(text,m_value,
                                   m_precision,
                                   !(arg_flags & wxPG_FULL_VALUE),
                                   (wxString*) NULL);
    return text;
}

bool wxFloatPropertyClass::SetValueFromString ( const wxString& text, int arg_flags )
{
    wxString s;
    double value;
    bool res = text.ToDouble(&value);
    if ( res )
    {
#if wxPG_USE_VALIDATORS
        if ( m_validator )
        {
            m_validator->AssertDataType(wxT("double"));
            wxPGVariant tvariant(value);
            if ( !m_validator->Validate(tvariant,s) )
            {
                ShowError(s);
                return FALSE;
            }
        }
#endif

        if ( m_value != value )
        {
            m_value = value;
            return TRUE;
        }
    }
    else if ( arg_flags & wxPG_REPORT_ERROR )
    {
        s.Printf ( _("\"%s\" is not a floating-point number."), text.c_str() );
        ShowError(s);
    }
    return FALSE;
}

void wxFloatPropertyClass::SetAttribute ( int id, wxVariant value )
{
    if ( id == wxPG_FLOAT_PRECISION )
    {
        m_precision = value.GetLong();
    }
}

// -----------------------------------------------------------------------
// wxBoolProperty
// -----------------------------------------------------------------------

#define wxPG_PROP_USE_CHECKBOX      wxPG_PROP_CLASS_SPECIFIC_1
// DCC = Double Click Cycles
#define wxPG_PROP_USE_DCC           wxPG_PROP_CLASS_SPECIFIC_2


wxPG_BEGIN_PROPERTY_CLASS_BODY2(wxBoolPropertyClass,wxPGProperty,bool,int,bool,class)
    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_CHOICE_METHODS()
    WX_PG_DECLARE_ATTRIBUTE_METHODS()

    // Allows changing strings in choice control.
    static void SetBoolChoices ( const wxChar* true_choice, const wxChar* false_choice );

wxPG_END_PROPERTY_CLASS_BODY()


// We cannot use standard WX_PG_IMPLEMENT_PROPERTY_CLASS macro, since
// there is a custom GetEditorClass.

WX_PG_IMPLEMENT_CONSTFUNC(wxBoolProperty,bool)
WX_PG_IMPLEMENT_CLASSINFO(wxBoolProperty)
wxPG_GETCLASSNAME_IMPLEMENTATION(wxBoolProperty)
const wxPGValueType* wxBoolPropertyClass::GetValueType() const { return wxPG_VALUETYPE(bool); }

const wxPGEditor* wxBoolPropertyClass::GetEditorClass() const
{
    // Select correct editor control.
#if wxPG_INCLUDE_CHECKBOX
    if ( !(m_flags & wxPG_PROP_USE_CHECKBOX) )
        return wxPG_EDITOR(Choice);
    return wxPG_EDITOR(CheckBox);
#else
    return wxPG_EDITOR(Choice);
#endif
}

void wxBoolPropertyClass::SetBoolChoices ( const wxChar* true_choice,
                                           const wxChar* false_choice )
{
    wxPGGlobalVars->m_boolChoices[0] = false_choice;
    wxPGGlobalVars->m_boolChoices[1] = true_choice;
}

wxBoolPropertyClass::wxBoolPropertyClass ( const wxString& label, const wxString& name, bool value ) :
    wxPGProperty(label,name)
{
    int useval = 0;
    if ( value ) useval = 1;
    DoSetValue(useval);
}

wxBoolPropertyClass::~wxBoolPropertyClass () { }

void wxBoolPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = 0;
    if ( wxPGVariantToLong(value) != 0 )
        m_value = 1;
}

wxPGVariant wxBoolPropertyClass::DoGetValue () const
{
    return wxPGVariant(m_value);
}

wxString wxBoolPropertyClass::GetValueAsString ( int arg_flags ) const
{
    if ( !(arg_flags & wxPG_FULL_VALUE) )
    {
        return wxPGGlobalVars->m_boolChoices[m_value];
    }
    wxString text;

    if (m_value) text = wxT("TRUE");
    else text = wxT("FALSE");

    return text;
}

int wxBoolPropertyClass::GetChoiceInfo ( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
    {
        choiceinfo->m_itemCount = 2;
        choiceinfo->m_arrWxString = wxPGGlobalVars->m_boolChoices;
    }
    return m_value;
}

bool wxBoolPropertyClass::SetValueFromString ( const wxString& text, int /*arg_flags*/ )
{
    int value = 0;
    if ( text.CmpNoCase(wxPGGlobalVars->m_boolChoices[1]) == 0 || text.CmpNoCase(wxT("TRUE")) == 0 )
        value = 1;

    if ( m_value != value )
    {
        DoSetValue ( value );
        return TRUE;
    }
    /*
    else if ( arg_flags & wxPG_REPORT_ERROR )
    {
        wxLogError ( wxT("Property %s: \"%s\" is not a boolean value (True and False are valid)."), m_label.c_str(), text.c_str() );
    }
    */
    return FALSE;
}

bool wxBoolPropertyClass::SetValueFromInt ( long value, int )
{
    if ( value != 0 ) value = 1;

    if ( m_value != value )
    {
        m_value = value;
        return TRUE;
    }
    return FALSE;
}

void wxBoolPropertyClass::SetAttribute ( int id, wxVariant value )
{
    int ival = value.GetLong();
#if wxPG_INCLUDE_CHECKBOX
    if ( id == wxPG_BOOL_USE_CHECKBOX )
    {
        if ( ival )
            m_flags |= wxPG_PROP_USE_CHECKBOX;
        else
            m_flags &= ~(wxPG_PROP_USE_CHECKBOX);
    }
    else
#endif
    if ( id == wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING )
    {
        if ( ival )
            m_flags |= wxPG_PROP_USE_DCC;
        else
            m_flags &= ~(wxPG_PROP_USE_DCC);
    }
}

// -----------------------------------------------------------------------
// Choice related methods from various classes
// -----------------------------------------------------------------------


WX_DECLARE_HASH_MAP(size_t,                // type of the keys
                    wxPGConstants*,        // type of the values
                    wxIntegerHash,         // hasher
                    wxIntegerEqual,        // key equality predicate
                    wxPGHashMapConstants); // name of the class

// -----------------------------------------------------------------------

// If last id was valid (i.e. non-zero), then clears it.
// Then sets this wxPGConstants to new id.
void wxPGConstants::SetId( size_t id )
{
    wxASSERT ( IsOk() );

    if ( !IsTemporary() )
    {
        wxPGHashMapConstants* socs =
            (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

        if ( m_id )
            socs->erase(m_id);

        (*socs)[id] = this;
    }
    m_id = id;
}

// -----------------------------------------------------------------------

#ifdef __WXDEBUG__
// Displays what dynamic arrays are allocated
void wxPropertyGrid::DumpAllocatedChoiceSets()
{

    wxPGHashMapConstants* socs = (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

    wxLogDebug(wxT("****************************************************************"));

    wxPGHashMapConstants::iterator soc_it;
    int soc_count = 0;

    //
    // NOTE: May fail to iterate through every and each item.
    //   (I have no idea why)
    for( soc_it = socs->begin(); soc_it != socs->end(); ++soc_it )
    {
        wxPGConstants* soc = soc_it->second;
        wxASSERT ( soc );

        wxArrayString& labels = soc->GetLabels();
        wxArrayInt& values = soc->GetValues();

        wxLogDebug(wxT("."));
        wxLogDebug(wxT("**** Dumping 0x%X (%i labels, %i values, %i references)****"),
            (unsigned int)soc,(int)labels.GetCount(),(int)values.GetCount(),
            (int)soc->GetRefCount());

        wxASSERT( !values.GetCount() || labels.GetCount() == values.GetCount() );

        wxASSERT_MSG( soc->GetRefCount(),
            wxT("soc with no references should have been deleted") );
        wxASSERT_MSG( soc->IsOk() && !soc->IsTemporary(),
            wxT("soc in hash map was invalid") );

        size_t i;
        for ( i=0; i<labels.GetCount(); i++ )
        {
            if ( values.GetCount() )
            {
                wxLogDebug(wxT("  %s = %i"),
                    labels[i].c_str(),values[i]);
                wxASSERT( values[i] != ((int)wxPG_INVALID_VALUE) );
            }
            else
                wxLogDebug(wxT("  %s"),
                    labels[i].c_str());
        }

        wxLogDebug(wxT("."));

        soc_count++;

    }

    wxLogDebug(wxT("Total %i sets"),soc_count);
    wxLogDebug(wxT("****************************************************************"));

}
#endif

// -----------------------------------------------------------------------

wxPGConstants* wxPropertyGrid::AddConstantsArray(const wxChar** labels,
                                                 const long* values,
                                                 int itemcount)
{

    wxPGHashMapConstants* socs =
        (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

    wxPGConstants* soc;

    size_t id = (size_t)labels;

    wxPGHashMapConstants::iterator it;
    it = socs->find(id);

    if ( it != socs->end() )
    {
        soc = (wxPGConstants*) it->second;
#ifdef __WXDEBUG__
        // Validate array pairing
        wxASSERT_MSG ( values == soc->m_origValueArray,
            wxT("one set of labels must always be paired with the same set of values") );
#endif
        soc->Ref();
        return soc;
    }

    // if necessary, determine itemcount
    if ( !itemcount && labels )
    {
        const wxChar** p = &labels[0];
        while ( *p ) { p++; itemcount++; }
    }
    //wxASSERT ( itemcount > 0 );

    soc = new wxPGConstants();
#ifdef __WXDEBUG__
    soc->m_origValueArray = values;
#endif

    //
    // Populate arrays
    size_t i;
    for ( i=0; i<(size_t)itemcount; i++ )
        soc->AddString(labels[i]);

    if ( values )
        for ( i=0; i<(size_t)itemcount; i++ )
            soc->AddInt(values[i]);

    // If no labels given, then this is unique array:
    //   set labels to point at created string array
    if ( !labels )
        labels = (const wxChar**) &soc->GetLabels();


#ifdef __WXDEBUG__
    // Make sure id is not already in use
    it = socs->find(id);

    wxASSERT_MSG( it == socs->end(),
        wxT("id for this set of choices was already in use") );
#endif

    // Cannot use SetId because we have zero refcount
    soc->SetupId(id);
    (*socs)[id] = soc;

    soc->Ref();
    return soc;
}

// -----------------------------------------------------------------------

wxPGConstants* wxPropertyGrid::AddConstantsArray(const wxArrayString& labels,
                                                 const wxArrayInt& values,
                                                 bool acceptLabelsAsId)
{
    wxPGHashMapConstants* socs =
        (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

    wxPGConstants* soc;

    size_t id = 0;

    if ( acceptLabelsAsId )
    {

        size_t id = (size_t) &labels;

        wxPGHashMapConstants::iterator it;
        it = socs->find(id);

        if ( it != socs->end() )
        {
            soc = (wxPGConstants*) it->second;
            soc->Ref();
            return soc;
        }

    }

    soc = new wxPGConstants();

    // If so preferred, create new id to avoid stack aliasing
    if ( !id )
        id = (size_t) soc;

#ifdef __WXDEBUG__
    // Make sure id is not already in use
    wxPGHashMapConstants::iterator it;
    it = socs->find(id);

    wxASSERT_MSG( it == socs->end(),
        wxT("id for this set of choices was already in use") );
#endif

#ifdef __WXDEBUG__
    soc->m_origValueArray = (const long*)&values;
#endif

    // Copy arrays
    soc->SetLabels(labels);
    if ( &values )
        soc->SetValues(values);

    // Cannot use SetId because we have zero refcount
    soc->SetupId(id);
    (*socs)[id] = soc;

    /*
    wxLogDebug(wxT("Added SOC %X - id: 0x%X (items = %i, first = %s)"),
        (unsigned int)soc,
        (unsigned int)id,
        (int)soc->GetCount(),
        soc->GetLabel(0).c_str());
    */

    soc->Ref();
    return soc;
}

// -----------------------------------------------------------------------

wxPGConstants* wxPropertyGrid::AddConstantsArray( wxPGConstants& constants )
{
    wxPGHashMapConstants* socs =
        (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

    wxASSERT( constants.IsOk() );

    size_t id = constants.GetId();
    if ( id )
    {
        // We can't use constants directly since it may be a temporary.
        wxPGHashMapConstants::iterator it;
        it = socs->find(id);

        if ( it != socs->end() )
        {
            wxPGConstants* soc = it->second;
            wxASSERT( !soc->IsTemporary() );
            soc->Ref();
            return soc;
        }
    }

    // If its a temporary array (zero refcount), then create a permanent
    // version (zero id meant it didn't exist).
    wxASSERT( constants.IsTemporary() );

    wxPGConstants* soc = AddConstantsArray(constants.GetLabels(),
                                           constants.GetValues(),
                                           false);

    constants.SetId(soc->GetId());

    return soc;
}

// -----------------------------------------------------------------------

// Creates "permanent" wxPGConstants. NOTE: Creates with
// zero refcount, so Ref has to be called.
wxPGConstants* wxPropertyGrid::CreateConstantsArray(size_t id)
{
    wxPGHashMapConstants* socs =
        (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

    wxPGConstants* soc = new wxPGConstants();
    if (!id)
        id = (size_t) soc;

    wxASSERT ( id );
    soc->SetupId(id);

#ifdef __WXDEBUG__
    soc->m_origValueArray = (const long*)&soc->GetValues();
#endif

    (*socs)[id] = soc;

    return soc;
}

// -----------------------------------------------------------------------

wxPGConstants* wxPropertyGrid::GetConstantsArray(size_t id)
{
    wxPGHashMapConstants* socs =
        (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

    wxPGHashMapConstants::iterator it;
    it = socs->find(id);
    if ( it != socs->end() )
        return (wxPGConstants*) it->second;
    return (wxPGConstants*) NULL;
}

// -----------------------------------------------------------------------

void wxPropertyContainerMethods::SetPropertyChoices(wxPGId id, wxPGConstants& choices)
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT( p );

    wxPGChoiceInfo ci;
    ci.m_constants = (wxPGConstants**) NULL;

    p->GetChoiceInfo(&ci);

    //wxASSERT_MSG( ci.m_constants, wxT("this property does not have choices") );

    if ( ci.m_constants )
    {
        wxPGConstants* cons = *ci.m_constants;
        wxPGUnRefChoices( cons );

        *ci.m_constants = wxPropertyGrid::AddConstantsArray(choices);
    }
}

// -----------------------------------------------------------------------

void wxPropertyContainerMethods::AddPropertyChoice(wxPGId id,
                                                   const wxString& label,
                                                   int value )
{
    wxASSERT (wxPGIdToPtr(id));

    wxPGConstants& cons = GetPropertyChoices(id);

    // If no real choices yet, then we need to setup them
    if ( !cons.IsOk() )
    {
        wxArrayString labels;
        wxArrayInt values;
        labels.Add(label);
        if ( value != wxPG_INVALID_VALUE )
            values.Add(value);

        // This will, of course, fail if property cannot have choices
        wxPGIdToPtr(id)->SetChoices(0,labels,values);
    }
    else
        cons.Add(label,value);
}

// -----------------------------------------------------------------------
// wxBaseEnumPropertyClass
// -----------------------------------------------------------------------

// Class body is in propdev.h.

wxBaseEnumPropertyClass::wxBaseEnumPropertyClass( const wxString& label, const wxString& name )
    : wxPGProperty(label,name)
{
}

/** If has values array, then returns number at index with value -
    otherwise just returns the value.
*/
int wxBaseEnumPropertyClass::GetIndexForValue( int value ) const
{
    return value;
}

void wxBaseEnumPropertyClass::DoSetValue ( wxPGVariant value )
{
    int intval = (int) value.GetLong();
    m_index = GetIndexForValue(intval);
}

wxPGVariant wxBaseEnumPropertyClass::DoGetValue () const
{
    if ( m_index < 0 )
        return wxPGVariant((long)-1);

    int val;
    GetEntry(m_index,&val);

    return val;
}

wxString wxBaseEnumPropertyClass::GetValueAsString ( int ) const
{
    if ( m_index >= 0 )
    {
        int unused_val;
        const wxString* pstr = GetEntry( m_index, &unused_val );

        if ( pstr )
            return *pstr;
    }
    return wxEmptyString;
}

bool wxBaseEnumPropertyClass::SetValueFromString ( const wxString& text, int WXUNUSED(arg_flags) )
{
    size_t i = 0;
    const wxString* entry_label;
    int entry_value;
    int use_index = -1;
    long use_value = 0;

    entry_label = GetEntry(i,&entry_value);
    while ( entry_label )
    {
        //wxLogDebug(wxT("entry_label: %s"),entry_label->c_str());
        if ( text.CmpNoCase(*entry_label) == 0 )
        {
            use_index = (int)i;
            use_value = (long)entry_value;
            break;
        }

        i++;
        entry_label = GetEntry(i,&entry_value);
    }

    if ( m_index != use_index )
    {
        // FIXME: Why can't this be virtual call?
        wxBaseEnumPropertyClass::DoSetValue ( use_value );

        return TRUE;
    }
    /*}
    else if ( arg_flags & wxPG_REPORT_ERROR )
    {
        wxString s;
        s.Printf ( wxT("\"%s\" was not among valid choices."), text.c_str() );
        ShowError(s);
    }*/
    return FALSE;
}

bool wxBaseEnumPropertyClass::SetValueFromInt ( long value, int arg_flags )
{
    if ( arg_flags & wxPG_FULL_VALUE )
    {
        DoSetValue(value);
        return TRUE;
    }
    else
    {
        if ( m_index != value )
        {
            m_index = value;
            return TRUE;
        }
    }
    return FALSE;
}

// -----------------------------------------------------------------------
// wxEnumProperty
// -----------------------------------------------------------------------

// Class body is in propdev.h.

wxPGProperty* wxEnumProperty ( const wxString& label, const wxString& name, const wxChar** labels,
    const long* values, unsigned int itemcount, int value )
{
    return new wxEnumPropertyClass (label,name,labels,values,itemcount,value);
}

wxPGProperty* wxEnumProperty ( const wxString& label, const wxString& name,
    wxPGConstants& constants, int value )
{
    return new wxEnumPropertyClass (label,name,constants,value);
}

WX_PG_IMPLEMENT_CLASSINFO(wxEnumProperty)

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxEnumProperty,long,Choice)

wxEnumPropertyClass::wxEnumPropertyClass ( const wxString& label, const wxString& name, const wxChar** labels,
    const long* values, unsigned int itemcount, int value ) : wxBaseEnumPropertyClass(label,name)
{
    m_constants = (wxPGConstants*) NULL;
    m_index = 0;

    if ( labels )
    {
        m_constants = wxPropertyGrid::AddConstantsArray(labels,values,itemcount);

        if ( GetItemCount() )
            wxEnumPropertyClass::DoSetValue( value );
    }
    else
        m_constants = &wxPGGlobalVars->m_emptyConstants;
}

wxEnumPropertyClass::wxEnumPropertyClass ( const wxString& label, const wxString& name,
    wxPGConstants& constants, int value )
    : wxBaseEnumPropertyClass(label,name)
{
    //m_itemCount = constants.GetCount();
    //constants.RealizeArrays(&m_arrLabels,&m_arrValues);

    m_constants = wxPropertyGrid::AddConstantsArray(constants);

    if ( GetItemCount() )
        wxEnumPropertyClass::DoSetValue( value );
}

int wxEnumPropertyClass::GetIndexForValue( int value ) const
{
    const wxArrayInt& arrValues = m_constants->GetValues();

    if ( arrValues.GetCount() )
    {
        int intval = arrValues.Index(value);

        // TODO: Use real default instead of 0.
        if ( intval < 0 )
            intval = 0;

        return intval;
    }
    return value;
}

wxEnumPropertyClass::~wxEnumPropertyClass ()
{
    wxPGUnRefChoices(m_constants);
}

const wxString* wxEnumPropertyClass::GetEntry( size_t index, int* pvalue ) const
{
    if ( index < m_constants->GetCount() )
    {
        const wxArrayInt& arrValues = m_constants->GetValues();

        int value = (int)index;
        if ( arrValues.GetCount() )
            value = arrValues[index];

        *pvalue = value;

        return &m_constants->GetLabel(index);
    }
    return (const wxString*) NULL;
}

int wxEnumPropertyClass::GetChoiceInfo ( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
    {
        choiceinfo->m_itemCount = m_constants->GetCount();
        if ( m_constants->GetCount() )
            choiceinfo->m_arrWxString = (wxString*)&m_constants->GetLabel(0);
        if ( !(m_flags & wxPG_PROP_STATIC_CHOICES) )
            choiceinfo->m_constants = &m_constants;
    }
    return m_index;
}

// -----------------------------------------------------------------------
// wxFlagsProperty
// -----------------------------------------------------------------------

// Class body is in propdev.h.

wxPGProperty* wxFlagsProperty( const wxString& label, const wxString& name, const wxChar** labels,
    const long* values, unsigned int itemcount, int value )
{
    return new wxFlagsPropertyClass (label,name,labels,values,itemcount,value);
}

wxPGProperty* wxFlagsProperty( const wxString& label, const wxString& name,
    wxPGConstants& constants, int value )
{
    return new wxFlagsPropertyClass (label,name,constants,value);
}

WX_PG_IMPLEMENT_CLASSINFO(wxFlagsProperty)

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxFlagsProperty,long,TextCtrl)

void wxFlagsPropertyClass::Init ()
{
    //wxFlagsPropertyClass::DoSetValue( value );

    long value = m_value;

    //
    // Generate children
    //
    unsigned int i;

    // Delete old children
    for ( i=0; i<m_children.GetCount(); i++ )
        delete ( (wxPGProperty*) m_children[i] );

    const wxArrayInt& values = GetValues();

    for ( i=0; i<GetItemCount(); i++ )
    {
        bool child_val;
        if ( values.GetCount() )
            child_val = ( value & values[i] )?TRUE:FALSE;
        else
            child_val = ( value & (1<<i) )?TRUE:FALSE;

        wxPGProperty* bool_prop;

    #if wxUSE_INTL
        if ( wxPGGlobalVars->m_autoGetTranslation )
        {
            bool_prop = wxBoolProperty( ::wxGetTranslation ( GetLabel(i) ), wxEmptyString, child_val );
        }
        else
    #endif
        {
            bool_prop = wxBoolProperty( GetLabel(i), wxEmptyString, child_val );
        }
        AddChild(bool_prop);
    }
}

wxFlagsPropertyClass::wxFlagsPropertyClass ( const wxString& label, const wxString& name,
    const wxChar** labels, const long* values, unsigned int itemcount,
    long value ) : wxPGPropertyWithChildren(label,name)
{

    //m_constants = (wxPGConstants*) NULL;
    m_value = 0;

    if ( labels )
    {
        m_constants = wxPropertyGrid::AddConstantsArray(labels,values,itemcount);

        wxASSERT ( GetItemCount() );

        DoSetValue( value );
        //Init ( value );
    }
    else
    {
        m_constants = &wxPGGlobalVars->m_emptyConstants;
    }
}

wxFlagsPropertyClass::wxFlagsPropertyClass ( const wxString& label, const wxString& name,
    wxPGConstants& constants, long value )
    : wxPGPropertyWithChildren(label,name)
{
    m_constants = wxPropertyGrid::AddConstantsArray(constants);

    wxASSERT ( GetItemCount() );

    //Init ( value );
    DoSetValue( value );
}

wxFlagsPropertyClass::~wxFlagsPropertyClass ()
{
    wxPGUnRefChoices(m_constants);
}

void wxFlagsPropertyClass::DoSetValue ( wxPGVariant value )
{
    wxASSERT_MSG( GetItemCount(),
        wxT("wxFlagsPropertyClass::DoSetValue must not be called with zero items"));

    long val = value.GetLong();

    long full_flags = 0;

    // normalize the value (i.e. remove extra flags)
    unsigned int i;
    const wxArrayInt& values = GetValues();
    if ( values.GetCount() )
    {
        for ( i = 0; i < GetItemCount(); i++ )
            full_flags |= values[i];
    }
    else
    {
        for ( i = 0; i < GetItemCount(); i++ )
            full_flags |= (1<<i);
    }
    val &= full_flags;

    m_value = val;

    // Need to (re)init now?
    /*if ( m_constants != m_oldConstants ||
         GetCount() != GetItemCount() )
        Init();
    else*/
    if ( !GetCount() )
        Init();

    RefreshChildren();
}

wxPGVariant wxFlagsPropertyClass::DoGetValue () const
{
    return wxPGVariant((long)m_value);
}

wxString wxFlagsPropertyClass::GetValueAsString ( int ) const
{
    wxString text;
    long flags = m_value;
    unsigned int i;
    const wxArrayInt& values = GetValues();

    if ( values.GetCount() )
    {
        for ( i = 0; i < GetItemCount(); i++ )
        {
            if ( flags & values[i] )
            {
                text += GetLabel(i);
                text += wxT(", ");
            }
        }
    }
    else
    {
        for ( i = 0; i < GetItemCount(); i++ )
            if ( flags & (1<<i) )
            {
                text += GetLabel(i);
                text += wxT(", ");
            }
    }

    // remove last comma
    if ( text.Len() > 1 )
        text.Truncate ( text.Len() - 2 );

    return text;
}

// Translate string into flag tokens
bool wxFlagsPropertyClass::SetValueFromString ( const wxString& text, int )
{
    // Need to (re)init now?
    //if ( GetCount() != GetItemCount() )
    //    Init(0);

    long new_flags = 0;

    // semicolons are no longer valid delimeters
    WX_PG_TOKENIZER1_BEGIN(text,wxT(','))

        if ( token.length() )
        {
            // Determine which one it is
            long bit = IdToBit( token );

            if ( bit != -1 )
            {
                // Changed?
                new_flags |= bit;
            }
            else
            {
            // Unknown identifier
                wxString s;
                s.Printf ( wxT("! %s: Unknown flag identifier \"%s\""), m_label.c_str(), token.c_str() );
                ShowError(s);
            }
        }

    WX_PG_TOKENIZER1_END()

    //wxLogDebug ( wxT("new_flags = 0x%X, old_flags = 0x%X"),new_flags,m_value);

    if ( new_flags != m_value )
    {
        // Set child modified states
        if ( GetItemCount() )
        {
            unsigned int i;
            const wxArrayInt& values = GetValues();
            if ( values.GetCount() )
                for ( i = 0; i < GetItemCount(); i++ )
                {
                    long flag = values[i];
                    if ( (new_flags & flag) != (m_value & flag) )
                        ((wxPGProperty*)m_children.Item( i ))->SetFlag ( wxPG_PROP_MODIFIED );
                }
            else
                for ( i = 0; i < GetItemCount(); i++ )
                {
                    long flag = (1<<i);
                    if ( (new_flags & flag) != (m_value & flag) )
                        ((wxPGProperty*)m_children.Item( i ))->SetFlag ( wxPG_PROP_MODIFIED );
                }
        }

        DoSetValue ( new_flags );

        return TRUE;
    }

    return FALSE;
}

// Converts string id to a relevant bit.
long wxFlagsPropertyClass::IdToBit ( const wxString& id ) const
{
    unsigned int i;
    const wxArrayInt& values = GetValues();
    for ( i = 0; i < GetItemCount(); i++ )
    {
        const wxChar* ptr = GetLabel(i);
        if ( id == ptr
             /*wxStrncmp(id,ptr,id_len) == 0 &&
             ptr[id_len] == 0*/
           )
        {
            //*pindex = i;
            if ( values.GetCount() )
                return values[i];
            return (1<<i);
        }
    }
    return -1;
}

void wxFlagsPropertyClass::RefreshChildren()
{
    if ( !GetCount() ) return;
    const wxArrayInt& values = GetValues();
    long flags = m_value;
    unsigned int i;
    if ( values.GetCount() )
        for ( i = 0; i < GetItemCount(); i++ )
            Item(i)->DoSetValue ( ((long)((flags & values[i])?TRUE:FALSE)) );
    else
        for ( i = 0; i < GetItemCount(); i++ )
            Item(i)->DoSetValue ( ((long)((flags & (1<<i))?TRUE:FALSE)) );
}

void wxFlagsPropertyClass::ChildChanged ( wxPGProperty* p )
{
    wxASSERT ( this == p->GetParent() );

    const wxArrayInt& values = GetValues();
    long val = p->DoGetValue().GetRawLong(); // bypass type checking
    unsigned int iip = p->GetIndexInParent();
    unsigned long vi = (1<<iip);
    if ( values.GetCount() ) vi = values[iip];
    if ( val )
        m_value |= vi;
    else
        m_value &= ~(vi);
}

int wxFlagsPropertyClass::GetChoiceInfo ( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
        choiceinfo->m_constants = &m_constants;
    return -1;
}

// -----------------------------------------------------------------------
// wxDirProperty
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_STRING_PROPERTY(wxDirProperty)

bool wxDirPropertyClass::OnButtonClick ( wxPropertyGrid* propgrid, wxString& value )
{
    wxSize dlg_sz(300,400);

    wxDirDialog dlg(propgrid,_("Choose a directory:"),
        value,0,
        propgrid->GetGoodEditorDialogPosition (this,dlg_sz),dlg_sz );

/*#if defined(__WXGTK__) && !defined(__WXGTK20__)
    dlg.SetFont(*wxNORMAL_FONT);
#endif*/

    if ( dlg.ShowModal() == wxID_OK )
    {
        value = dlg.GetPath();
        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------
// wxFileProperty
// -----------------------------------------------------------------------

// Class body is in propdev.h.

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxFileProperty,wxString,const wxString&,TextCtrlAndButton)

wxFilePropertyClass::wxFilePropertyClass ( const wxString& label, const wxString& name,
    const wxString& value ) : wxPGProperty(label,name)
{
    m_wildcard = _("All files (*.*)|*.*");
    m_flags |= wxPG_PROP_SHOW_FULL_FILENAME;
    m_indFilter = -1;
    DoSetValue(value);
}

wxFilePropertyClass::~wxFilePropertyClass () {}

void wxFilePropertyClass::DoSetValue ( wxPGVariant value )
{
    const wxString& str = wxPGVariantToString(value);
    m_filename = str;
    m_fnstr = str;
    wxPG_SetVariantValue(str);

    // Find index for extension.
    if ( m_indFilter < 0 && m_fnstr.length() )
    {
        wxChar a = wxT(' ');
        const wxChar* p = m_wildcard.c_str();
        wxString ext = m_filename.GetExt();
        int curind = 0;
        do
        {
            while ( a && a != '|' ) { a = *p; p++; }
            if ( !a ) break;

            a = *p;
            p++;
            if ( !a ) break;
            a = *p;
            p++;

            const wxChar* ext_begin = p;

            if ( *ext_begin == wxT('*') )
            {
                m_indFilter = curind;
                break;
            }

            while ( a && a != '|' ) { a = *p; p++; }

            a = wxT(' ');

            int count = p-ext_begin-1;
            if ( count > 0 )
            {
                wxASSERT ( count < 32 );
                wxString found_ext = m_wildcard.Mid(ext_begin-m_wildcard.c_str(),count);

                if ( ext.CmpNoCase(found_ext) == 0 )
                {
                    m_indFilter = curind;
                    break;
                }
            }

            curind++;

        } while ( a );
    }
}

wxPGVariant wxFilePropertyClass::DoGetValue () const
{
    return wxPGVariant(m_fnstr);
}

wxString wxFilePropertyClass::GetValueAsString ( int arg_flags ) const
{
    if ( (m_flags & wxPG_PROP_SHOW_FULL_FILENAME) || (arg_flags & wxPG_FULL_VALUE) )
        return m_filename.GetFullPath();
    else
        return m_filename.GetFullName();
}

bool wxFilePropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
    wxEvent& event )
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        // If text in control is changed, then update it to value.
        PrepareValueForDialogEditing(propgrid);

        wxString path;
        path = m_filename.GetPath();

        wxFileDialog dlg(propgrid,_("Choose a file"),m_filename.GetPath(),
            wxEmptyString, m_wildcard, 0, wxDefaultPosition );

        if ( m_indFilter >= 0 )
            dlg.SetFilterIndex( m_indFilter );

        if ( dlg.ShowModal() == wxID_OK )
        {
            m_indFilter = dlg.GetFilterIndex();
            wxString path = dlg.GetPath();
            SetValueFromString ( path, wxPG_FULL_VALUE );
            if ( primary )
                GetEditorClass()->SetControlStringValue( primary, GetValueAsString(0) );
            return TRUE;
        }
    }
    return FALSE;
}

bool wxFilePropertyClass::SetValueFromString ( const wxString& text, int arg_flags )
{
    if ( (m_flags & wxPG_PROP_SHOW_FULL_FILENAME) || (arg_flags & wxPG_FULL_VALUE) )
    {
        if ( m_filename != text )
        {
            return StdValidationProcedure( text );
        }
    }
    else
    {
        if ( m_filename.GetFullName() != text )
        {
            wxFileName fn = m_filename;
            fn.SetFullName(text);
            wxString val = fn.GetFullPath();
            return StdValidationProcedure( val );
        }
    }

    return FALSE;
}

void wxFilePropertyClass::SetAttribute ( int id, wxVariant value )
{
    if ( id == wxPG_FILE_SHOW_FULL_PATH )
    {
        if ( value.GetLong() )
            m_flags |= wxPG_PROP_SHOW_FULL_FILENAME;
        else
            m_flags &= ~(wxPG_PROP_SHOW_FULL_FILENAME);
    }
    else if ( id == wxPG_FILE_WILDCARD )
    {
        m_wildcard = value.GetString();
    }
}

// -----------------------------------------------------------------------
// wxLongStringProperty
// -----------------------------------------------------------------------

// Class body is in propdev.h.

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxLongStringProperty,wxString,const wxString&,TextCtrlAndButton)

wxLongStringPropertyClass::wxLongStringPropertyClass ( const wxString& label, const wxString& name,
    const wxString& value ) : wxPGProperty(label,name)
{
    DoSetValue(value);
}

wxLongStringPropertyClass::~wxLongStringPropertyClass () {}

void wxLongStringPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = wxPGVariantToString(value);
    wxPG_SetVariantValue(m_value);
}

wxPGVariant wxLongStringPropertyClass::DoGetValue () const
{
    return wxPGVariant(m_value);
}

wxString wxLongStringPropertyClass::GetValueAsString ( int ) const
{
    return m_value;
}

bool wxLongStringPropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
    wxEvent& event )
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        // Update the value
        PrepareValueForDialogEditing(propgrid);

        wxString val1 = GetValueAsString(0);
        wxString val_orig = val1;

        wxString value;
        wxPropertyGrid::ExpandEscapeSequences(value,val1);

        // Run editor dialog.
        if ( OnButtonClick(propgrid,value) )
        {
            wxPropertyGrid::CreateEscapeSequences(val1,value);
            if ( val1 != val_orig )
            {
                SetValueFromString ( val1, 0 );
                UpdateControl ( primary );
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool wxLongStringPropertyClass::OnButtonClick ( wxPropertyGrid* propgrid, wxString& value )
{
    // launch editor dialog
    wxDialog* dlg = new wxDialog (propgrid,-1,m_label,
        wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLIP_CHILDREN);

    dlg->SetFont(propgrid->GetFont()); // Correctly use propgrid's font

    // Multi-line text editor dialog.
    const int spacing = 8;
    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    wxTextCtrl* ed = new wxTextCtrl(dlg,11,value,
        wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE);
    rowsizer->Add ( ed, 1, wxEXPAND|wxALL, spacing );
    topsizer->Add ( rowsizer, 1, wxEXPAND, 0 );
    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    const int but_sz_flags =
        wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT;
    rowsizer->Add ( new wxButton(dlg,wxID_OK,_("Ok")),
        0, but_sz_flags, spacing );
    rowsizer->Add ( new wxButton(dlg,wxID_CANCEL,_("Cancel")),
        0, but_sz_flags, spacing );
    topsizer->Add ( rowsizer, 0, wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL, 0 );

    dlg->SetSizer ( topsizer );
    topsizer->SetSizeHints( dlg );

    dlg->SetSize (400,300);

    dlg->Move ( propgrid->GetGoodEditorDialogPosition (this,dlg->GetSize()) );

    int res = dlg->ShowModal();

    if ( res == wxID_OK )
    {
        value = ed->GetValue();
        dlg->Destroy();
        return TRUE;
    }
    dlg->Destroy();
    return FALSE;
}

bool wxLongStringPropertyClass::SetValueFromString ( const wxString& text, int )
{
    if ( m_value != text )
    {
        DoSetValue ( text );
        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------
// wxArrayEditorDialog
// -----------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxArrayEditorDialog, wxDialog)
    EVT_IDLE(wxArrayEditorDialog::OnIdle)
    EVT_LISTBOX(24, wxArrayEditorDialog::OnListBoxClick)
    EVT_LISTBOX(24, wxArrayEditorDialog::OnListBoxClick)
    EVT_TEXT_ENTER(21, wxArrayEditorDialog::OnAddClick)
    EVT_BUTTON(22, wxArrayEditorDialog::OnAddClick)
    EVT_BUTTON(23, wxArrayEditorDialog::OnDeleteClick)
    EVT_BUTTON(25, wxArrayEditorDialog::OnUpClick)
    EVT_BUTTON(26, wxArrayEditorDialog::OnDownClick)
    EVT_BUTTON(27, wxArrayEditorDialog::OnUpdateClick)
    //EVT_BUTTON(28, wxArrayEditorDialog::OnCustomEditClick)
END_EVENT_TABLE()

IMPLEMENT_ABSTRACT_CLASS(wxArrayEditorDialog, wxDialog)

#include <wx/statline.h>

// -----------------------------------------------------------------------

void wxArrayEditorDialog::OnIdle (wxIdleEvent&)
{
    //
    // Do control focus detection here.
    //

    if ( m_curFocus == 0 && FindFocus() == m_lbStrings )
    {
        // ListBox was just focused.
        m_butAdd->Enable(FALSE);
        m_butUpdate->Enable(FALSE);
        m_butRemove->Enable(TRUE);
        m_butUp->Enable(TRUE);
        m_butDown->Enable(TRUE);
        m_curFocus = 1;
    }
    else if ( (m_curFocus == 1 && FindFocus() == m_edValue) /*|| m_curFocus == 2*/ )
    {
        // TextCtrl was just focused.
        m_butAdd->Enable(TRUE);
        bool upd_enable = FALSE;
        if ( m_lbStrings->GetCount() && m_lbStrings->GetSelection() >= 0 )
            upd_enable = TRUE;
        m_butUpdate->Enable(upd_enable);
        m_butRemove->Enable(FALSE);
        m_butUp->Enable(FALSE);
        m_butDown->Enable(FALSE);
        m_curFocus = 0;
    }
    //wxLogDebug(wxT("wxArrayEditorDialog::OnFocusChange"));
}

// -----------------------------------------------------------------------

wxArrayEditorDialog::wxArrayEditorDialog()
    : wxDialog()
{
    Init();
}

// -----------------------------------------------------------------------

void wxArrayEditorDialog::Init()
{
    m_custBtText = (const wxChar*) NULL;
    //m_pCallingClass = (wxArrayStringPropertyClass*) NULL;
}

// -----------------------------------------------------------------------

wxArrayEditorDialog::wxArrayEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              long style,
                              const wxPoint& pos,
                              const wxSize& sz )
                              : wxDialog()
{
    Init();
    Create(parent,message,caption,style,pos,sz);
}

// -----------------------------------------------------------------------

bool wxArrayEditorDialog::Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                long style,
                const wxPoint& pos,
                const wxSize& sz )
{

    bool res = wxDialog::Create (parent,1,caption,pos,sz,style);

    SetFont(parent->GetFont()); // Correctly use propgrid's font

    const int spacing = 4;

    m_modified = FALSE;

    m_curFocus = 1;

    const int but_sz_flags =
        wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL|wxALL; //wxBOTTOM|wxLEFT|wxRIGHT;

    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );

    // Message
    if ( message.length() )
        topsizer->Add ( new wxStaticText(this,-1,message),
            0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxALL, spacing );

    // String editor
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    /*rowsizer->Add ( new wxStaticText(this,-1,_("String:")),
        0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxBOTTOM|wxTOP|wxLEFT, spacing );*/
    m_edValue = new wxTextCtrl(this,21,wxEmptyString,
        wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
    rowsizer->Add ( m_edValue,
        1, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxALL, spacing );

    // Add button
    m_butAdd = new wxButton(this,22,_("Add"));
    rowsizer->Add ( m_butAdd,
        0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, spacing );
    topsizer->Add ( rowsizer, 0, wxEXPAND, spacing );

    // Separator line
    topsizer->Add ( new wxStaticLine(this,-1),
        0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, spacing );

    rowsizer = new wxBoxSizer( wxHORIZONTAL );

    // list box
    //m_lbStrings = new wxListBox(this, 24, wxDefaultPosition, wxDefaultSize, array);
    m_lbStrings = new wxListBox(this, 24, wxDefaultPosition, wxDefaultSize);
    unsigned int i;
    for ( i=0; i<ArrayGetCount(); i++ )
        m_lbStrings->Append( ArrayGet(i) );
    rowsizer->Add ( m_lbStrings, 1, wxEXPAND|wxRIGHT, spacing );

    // Manipulator buttons
    wxBoxSizer* colsizer = new wxBoxSizer( wxVERTICAL );
    m_butCustom = (wxButton*) NULL;
    if ( m_custBtText )
    {
        m_butCustom = new wxButton(this,28,::wxGetTranslation(m_custBtText));
        colsizer->Add ( m_butCustom,
            0, wxALIGN_CENTER|wxTOP/*wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT*/,
            spacing );
    }
    m_butUpdate = new wxButton(this,27,_("Update"));
    colsizer->Add ( m_butUpdate,
        0, wxALIGN_CENTER|wxTOP, spacing );
    m_butRemove = new wxButton(this,23,_("Remove"));
    colsizer->Add ( m_butRemove,
        0, wxALIGN_CENTER|wxTOP, spacing );
    m_butUp = new wxButton(this,25,_("Up"));
    colsizer->Add ( m_butUp,
        0, wxALIGN_CENTER|wxTOP, spacing );
    m_butDown = new wxButton(this,26,_("Down"));
    colsizer->Add ( m_butDown,
        0, wxALIGN_CENTER|wxTOP, spacing );
    rowsizer->Add ( colsizer, 0, 0, spacing );

    topsizer->Add ( rowsizer, 1, wxLEFT|wxRIGHT|wxEXPAND, spacing );

    // Separator line
    topsizer->Add ( new wxStaticLine(this,-1),
        0, wxEXPAND|wxTOP|wxLEFT|wxRIGHT, spacing );

    // buttons
    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    /*
    const int but_sz_flags =
        wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT;
    */
    rowsizer->Add ( new wxButton(this,wxID_OK,_("Ok")),
        0, but_sz_flags, spacing );
    rowsizer->Add ( new wxButton(this,wxID_CANCEL,_("Cancel")),
        0, but_sz_flags, spacing );
    topsizer->Add ( rowsizer, 0, wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL, 0 );

    m_edValue->SetFocus();

    SetSizer ( topsizer );
    topsizer->SetSizeHints( this );

    if ( sz.x == wxDefaultSize.x &&
         sz.y == wxDefaultSize.y
       )
        SetSize ( wxSize(275,360) );
    else
        SetSize (sz);

    return res;
}

// -----------------------------------------------------------------------

void wxArrayEditorDialog::OnAddClick(wxCommandEvent& )
{
    wxString text = m_edValue->GetValue();
    if ( text.length() )
    {
        if ( ArrayInsert( text, -1 ) )
        {
            m_lbStrings->Append( text );
            m_modified = TRUE;
            m_edValue->Clear();
        }
    }
}

// -----------------------------------------------------------------------

void wxArrayEditorDialog::OnDeleteClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 )
    {
        ArrayRemoveAt( index );
        m_lbStrings->Delete ( index );
        m_modified = TRUE;
    }
}

// -----------------------------------------------------------------------

void wxArrayEditorDialog::OnUpClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index > 0 )
    {
        ArraySwap(index-1,index);
        /*wxString old_str = m_array[index-1];
        wxString new_str = m_array[index];
        m_array[index-1] = new_str;
        m_array[index] = old_str;*/
        m_lbStrings->SetString ( index-1, ArrayGet(index-1) );
        m_lbStrings->SetString ( index, ArrayGet(index) );
        m_lbStrings->SetSelection ( index-1 );
        m_modified = TRUE;
    }
}

// -----------------------------------------------------------------------

void wxArrayEditorDialog::OnDownClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 && index < (m_lbStrings->GetCount()-1) )
    {
        ArraySwap(index,index+1);
        /*wxString old_str = m_array[index+1];
        wxString new_str = m_array[index];
        m_array[index+1] = new_str;
        m_array[index] = old_str;*/
        m_lbStrings->SetString ( index+1, ArrayGet(index+1) );
        m_lbStrings->SetString ( index, ArrayGet(index) );
        m_lbStrings->SetSelection ( index+1 );
        m_modified = TRUE;
    }
}

// -----------------------------------------------------------------------

void wxArrayEditorDialog::OnUpdateClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 )
    {
        wxString str = m_edValue->GetValue();
        if ( ArraySet(index,str) )
        {
            m_lbStrings->SetString ( index, str );
            //m_array[index] = str;
            m_modified = TRUE;
        }
    }
}

// -----------------------------------------------------------------------

/*void wxArrayEditorDialog::OnCustomEditClick(wxCommandEvent& )
{
    wxASSERT ( m_pCallingClass );
    wxString str = m_edValue->GetValue();
    if ( m_pCallingClass->OnCustomStringEdit(m_parent,str) )
    {
        //m_edValue->SetValue ( str );
        if ( ArrayInsert(-1,str) )
        {
            m_lbStrings->Append ( str );
            m_modified = TRUE;
        }
    }
}*/

// -----------------------------------------------------------------------

void wxArrayEditorDialog::OnListBoxClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 )
    {
        m_edValue->SetValue ( m_lbStrings->GetString(index) );
    }
}

// -----------------------------------------------------------------------
// wxArrayStringEditorDialog
// -----------------------------------------------------------------------

class wxArrayStringEditorDialog : public wxArrayEditorDialog
{
public:
    wxArrayStringEditorDialog();

    void Init();

    wxArrayStringEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              wxArrayString& array,
                              long style = wxAEDIALOG_STYLE,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& sz = wxDefaultSize );

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                wxArrayString& array,
                long style = wxAEDIALOG_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& sz = wxDefaultSize );

    inline void SetCustomButton ( const wxChar* custBtText, wxArrayStringPropertyClass* pcc )
    {
        m_custBtText = custBtText;
        m_pCallingClass = pcc;
    }

    const wxArrayString& GetArray() const { return m_array; }

    void OnCustomEditClick(wxCommandEvent& event);

protected:
    wxArrayString   m_array;

    wxArrayStringPropertyClass*     m_pCallingClass;

    virtual wxString ArrayGet( size_t index );
    virtual size_t ArrayGetCount();
    virtual bool ArrayInsert( const wxString& str, int index );
    virtual bool ArraySet( size_t index, const wxString& str );
    virtual void ArrayRemoveAt( int index );
    virtual void ArraySwap( size_t first, size_t second );

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxArrayStringEditorDialog)
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxArrayStringEditorDialog, wxArrayEditorDialog)
    EVT_BUTTON(28, wxArrayStringEditorDialog::OnCustomEditClick)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxArrayStringEditorDialog, wxArrayEditorDialog)

// -----------------------------------------------------------------------

wxString wxArrayStringEditorDialog::ArrayGet( size_t index )
{
    return m_array[index];
}

size_t wxArrayStringEditorDialog::ArrayGetCount()
{
    return m_array.GetCount();
}

bool wxArrayStringEditorDialog::ArrayInsert( const wxString& str, int index )
{
    if (index<0)
        m_array.Add(str);
    else
        m_array.Insert(str,index);
    return TRUE;
}

bool wxArrayStringEditorDialog::ArraySet( size_t index, const wxString& str )
{
    m_array[index] = str;
    return TRUE;
}

void wxArrayStringEditorDialog::ArrayRemoveAt( int index )
{
    m_array.RemoveAt(index);
}

void wxArrayStringEditorDialog::ArraySwap( size_t first, size_t second )
{
    wxString old_str = m_array[first];
    wxString new_str = m_array[second];
    m_array[first] = new_str;
    m_array[second] = old_str;
}

wxArrayStringEditorDialog::wxArrayStringEditorDialog()
    : wxArrayEditorDialog()
{
    Init();
}

void wxArrayStringEditorDialog::Init()
{
    wxArrayEditorDialog::Init();
    m_pCallingClass = (wxArrayStringPropertyClass*) NULL;
}

wxArrayStringEditorDialog::wxArrayStringEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              wxArrayString& array,
                              long style,
                              const wxPoint& pos,
                              const wxSize& sz )
                              : wxArrayEditorDialog()
{
    Init();
    Create(parent,message,caption,array,style,pos,sz);
/*#if defined(__WXGTK__) && !defined(__WXGTK20__)
    SetFont(*wxNORMAL_FONT);
#endif*/
}

bool wxArrayStringEditorDialog::Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                wxArrayString& array,
                long style,
                const wxPoint& pos,
                const wxSize& sz )
{

    m_array = array;

    return wxArrayEditorDialog::Create (parent,message,caption,style,pos,sz);
}

void wxArrayStringEditorDialog::OnCustomEditClick(wxCommandEvent& )
{
    wxASSERT ( m_pCallingClass );
    wxString str = m_edValue->GetValue();
    if ( m_pCallingClass->OnCustomStringEdit(m_parent,str) )
    {
        //m_edValue->SetValue ( str );
        m_lbStrings->Append ( str );
        m_array.Add ( str );
        m_modified = TRUE;
    }
}

// -----------------------------------------------------------------------
// wxArrayStringProperty
// -----------------------------------------------------------------------

// Class body is in propdev.h

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxArrayStringProperty,wxArrayString,const wxArrayString&,TextCtrlAndButton)

wxArrayStringPropertyClass::wxArrayStringPropertyClass ( const wxString& label, const wxString& name,
    const wxArrayString& array ) : wxPGProperty(label,name)
{
    DoSetValue( array );
}

wxArrayStringPropertyClass::~wxArrayStringPropertyClass () { }

void wxArrayStringPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = wxPGVariantToArrayString(value);
    GenerateValueAsString();
    wxPG_SetVariantValue(m_value);
}

wxPGVariant wxArrayStringPropertyClass::DoGetValue () const
{
    return wxPGVariant(m_value);
}

wxString wxArrayStringPropertyClass::GetValueAsString ( int /*arg_flags*/ ) const
{
    //if ( !(arg_flags & wxPG_FULL_VALUE) )
        return m_display;

    /*wxString s_ret;
    wxPropertyGrid::ArrayStringToString(s_ret,m_value,wxT('"'),wxT('"'),0);
    return s_ret;*/
}

// Converts wxArrayString to a string separated by delimeters and spaces.
// preDelim is useful for "str1" "str2" style. Set flags to 1 to do slash
// conversion.
void wxPropertyGrid::ArrayStringToString ( wxString& dst, const wxArrayString& src,
                                           wxChar preDelim, wxChar postDelim,
                                           int flags )
{
    wxString pdr;

    unsigned int i;
    unsigned int itemcount = src.GetCount();

    wxChar preas[2];

    dst.Empty();

    if ( !preDelim )
        preas[0] = 0;
    else if ( (flags & 1) )
    {
        preas[0] = preDelim;
        preas[1] = 0;
        pdr = wxT("\\");
        pdr += preDelim;
    }

    if ( itemcount )
        dst.append ( preas );

    wxASSERT ( postDelim );

    for ( i = 0; i < itemcount; i++ )
    {
        wxString str( src.Item(i) );

        // Do some character conversion.
        // Convertes \ to \\ and <preDelim> to \<preDelim>
        // Useful when preDelim and postDelim are "\"".
        if ( flags & 1 )
        {
            str.Replace ( wxT("\\"), wxT("\\\\"), TRUE );
            if ( pdr.length() )
                str.Replace ( preas, pdr, TRUE );
        }

        dst.append ( str );

        if ( i < (itemcount-1) )
        {
            dst.append ( postDelim );
            dst.append ( wxT(" ") );
            dst.append ( preas );
        }
        else if ( preDelim )
            dst.append ( postDelim );
    }
}

void wxArrayStringPropertyClass::GenerateValueAsString ()
{
    wxPropertyGrid::ArrayStringToString(m_display,m_value,wxT('"'),wxT('"'),1);
}

// Default implementation doesn't do anything.
bool wxArrayStringPropertyClass::OnCustomStringEdit ( wxWindow*, wxString& )
{
    return FALSE;
}

bool wxArrayStringPropertyClass::OnButtonClick ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
    const wxChar* cbt )
{
    // Update the value
    PrepareValueForDialogEditing(propgrid);

    // Create editor dialog.
    wxArrayStringEditorDialog dlg;

    if ( cbt )
        dlg.SetCustomButton(cbt,this);

    dlg.Create(propgrid, wxEmptyString, m_label, m_value );

    dlg.Move ( propgrid->GetGoodEditorDialogPosition (this,dlg.GetSize()) );

    int res = dlg.ShowModal();
    if ( res == wxID_OK && dlg.IsModified() )
    {
        DoSetValue ( dlg.GetArray() );
        UpdateControl ( primary );
        return TRUE;
    }
    return FALSE;
}

bool wxArrayStringPropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
    wxEvent& event )
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
        return OnButtonClick(propgrid,primary,(const wxChar*) NULL);
    return FALSE;
}

bool wxArrayStringPropertyClass::SetValueFromString ( const wxString& text, int )
{
    m_value.Empty();

    WX_PG_TOKENIZER2_BEGIN(text,wxT('"'))

        // Need to replace backslashes with empty characters
        // (opposite what is done in GenerateValueString).
        token.Replace ( wxT("\\"), wxT(""), TRUE );

        m_value.Add ( token );

    WX_PG_TOKENIZER2_END()

    GenerateValueAsString();

    return TRUE;
}

// -----------------------------------------------------------------------
// Statics in one class for easy destruction.
// -----------------------------------------------------------------------

#include <wx/ptr_scpd.h>

// TODO: Consider converting this to wxModule.
wxDECLARE_SCOPED_PTR(wxPGGlobalVarsClass, wxPGGlobalVarsClassPtr)
wxDEFINE_SCOPED_PTR(wxPGGlobalVarsClass, wxPGGlobalVarsClassPtr)

static wxPGGlobalVarsClassPtr gs_spStaticVars;
wxPGGlobalVarsClass* wxPGGlobalVars = (wxPGGlobalVarsClass*) NULL;

wxPGGlobalVarsClass::wxPGGlobalVarsClass()
{
    wxPGGlobalVars = this;

    m_dictConstants = new wxPGHashMapConstants();

    m_emptyConstants.SetRefCount(0xFFFFFFF);

    m_boolChoices[0] = _("False");
    m_boolChoices[1] = _("True");

    m_autoGetTranslation = false;

    m_offline = 0;
}

wxPGGlobalVarsClass::~wxPGGlobalVarsClass()
{
    size_t i;

    //
    // Destroy value type class instances.
    wxPGHashMapS2P::iterator vt_it;

    for( vt_it = m_dictValueType.begin(); vt_it != m_dictValueType.end(); ++vt_it )
    {
        wxPGValueType* pcls = (wxPGValueType*) vt_it->second;
        wxASSERT ( pcls );
        delete pcls;
    }

    // Destroy editor class instances.
    for ( i=0; i< m_arrEditorClasses.GetCount(); i++ )
    {
        delete ((wxPGEditor*)m_arrEditorClasses.Item(i));
    }

    //
    // Destroy key=value sets
    wxPGHashMapConstants* socs = (wxPGHashMapConstants*) m_dictConstants;

    delete socs;
}

// -----------------------------------------------------------------------
// wxPGProperty
// -----------------------------------------------------------------------

void wxPGProperty::Init()
{
    m_y = -3;
    m_arrIndex = 0xFFFF;
    m_parent = (wxPGPropertyWithChildren*) NULL;

#if wxPG_USE_CLIENT_DATA
    m_clientData = (void*) NULL;
#endif

#if wxPG_USE_VALIDATORS
    m_validator = (wxPropertyValidator*) NULL;
#endif
    m_flags = 0;

    m_depth = 1;
    m_parentingType = 0;
    m_bgColIndex = 0;
}

wxPGProperty::wxPGProperty()
#if wxPG_EMBED_VARIANT
    : wxVariant()
#elif wxPG_INCLUDE_WXOBJECT
    : wxObject()
#endif
{
    Init();
}

wxPGProperty::wxPGProperty( const wxString& label, const wxString& name )
#if wxPG_EMBED_VARIANT
//    : wxVariant()
#elif wxPG_INCLUDE_WXOBJECT
    : wxObject()
#endif
{
    m_label = label;
    if ( ((void*)(&name)) != NULL )
        DoSetName ( name );
    else
        DoSetName ( label );

    Init();
}

wxPGProperty::~wxPGProperty()
{
#if wxPG_USE_VALIDATORS
    // Proper validator removal (delete if refcount reached zero)
    if ( m_validator && m_validator->UnRef() )
        delete m_validator;
#endif
}

bool wxPGProperty::IsSomeParent ( wxPGProperty* candidate ) const
{
    wxPGPropertyWithChildren* parent = m_parent;
    do
    {
        if ( parent == (wxPGPropertyWithChildren*)candidate )
            return TRUE;
        parent = parent->m_parent;
    } while ( parent );
    return FALSE;
}

wxPropertyGridState* wxPGProperty::GetParentState() const
{
    wxASSERT ( m_parent );
    return m_parent->GetParentState();
}

size_t wxPGProperty::GetChildCount() const
{
    int cc = GetParentingType();
    if ( cc == 0 ) return 0;
    return ((wxPGPropertyWithChildren*)this)->GetCount();
}

void wxPGProperty::ShowError ( const wxString& msg )
{
    if ( !msg.length() )
        return;

#if wxUSE_STATUSBAR
    if ( !wxPGGlobalVars->m_offline )
    {
        wxPropertyGrid* pg = GetParentState()->m_pPropGrid;
        wxASSERT (pg);
        wxWindow* topWnd = ::wxGetTopLevelParent(pg);
        if ( topWnd )
        {
            wxFrame* pFrame = wxDynamicCast(topWnd,wxFrame);
            if ( pFrame )
            {
                wxStatusBar* pStatusBar = pFrame->GetStatusBar();
                if ( pStatusBar )
                {
                    pStatusBar->SetStatusText(msg);
                    return;
                }
            }
        }
    }
#endif
    ::wxLogError(msg);
}

void wxPGProperty::UpdateControl ( wxPGCtrlClass* primary )
{
    if ( primary )
        GetEditorClass()->UpdateControl(this,primary);
}

void wxPGProperty::DoSetValue ( wxPGVariant )
{
    // Actually, this should never get called
    wxFAIL_MSG ( wxT("must be overridden") );
}

// wxPGRootPropertyClass, at least, should make use of this.
wxPGVariant wxPGProperty::DoGetValue () const
{
    return wxPGVariant((long)0);
}

wxString wxPGProperty::GetValueAsString ( int ) const
{
    wxFAIL_MSG ( wxT("must be overridden") );
    return m_name;
}

bool wxPGProperty::SetValueFromString ( const wxString&, int )
{
    wxFAIL_MSG ( wxT("must be overridden") );
    return FALSE;
}

bool wxPGProperty::SetValueFromInt ( long, int )
{
    wxFAIL_MSG ( wxT("must be overridden") );
    return FALSE;
}

wxSize wxPGProperty::GetImageSize() const
{
    return wxSize(0,0);
}

const wxPGEditor* wxPGProperty::GetEditorClass () const
{
    return wxPG_EDITOR(TextCtrl);
}

// Default extra property event handling - that is, none at all.
bool wxPGProperty::OnEvent ( wxPropertyGrid*, wxPGCtrlClass*, wxEvent& )
{
    return FALSE;
}

int wxPGProperty::GetChoiceInfo ( wxPGChoiceInfo* )
{
    //wxFAIL_MSG(wxT("must be overridden"));
    return 0;
}

void wxPGProperty::SetAttribute ( int, wxVariant )
{
}

void wxPGProperty::OnCustomPaint ( wxDC&,
    const wxRect&, wxPGPaintData& )
{
}

size_t wxPGProperty::SetChoices ( size_t id, const wxArrayString& labels, const wxArrayInt& values )
{
    wxPGChoiceInfo ci;
    ci.m_constants = (wxPGConstants**) NULL;

    GetChoiceInfo(&ci);
    if ( ci.m_constants )
    {
        wxPGConstants* cons = wxPropertyGrid::GetConstantsArray(id);

        if ( !cons )
        {
            // Create new with given id
            cons = wxPropertyGrid::CreateConstantsArray(id);
            cons->SetLabels(labels);
            if ( &values && values.GetCount() )
                cons->SetValues(values);
        }

        // Replace
        wxPGUnRefChoices((*ci.m_constants));
        cons->Ref();
        *ci.m_constants = cons;

        wxASSERT ( cons->GetId() );

        //DoSetValue(GetValueType()->GetDefaultValue());

        // Redraw
        //wxPropertyGrid* pg = GetGrid();
        //if ( pg->GetSelectedProperty() == this )

        return cons->GetId();
    }

    return 0;
}

// Privatizes set of choices
void wxPGProperty::SetChoicesPrivate()
{
    wxPGChoiceInfo ci;
    ci.m_constants = (wxPGConstants**) NULL;

    GetChoiceInfo(&ci);
    if ( ci.m_constants )
    {
        wxPGConstants* old = *ci.m_constants;

        if ( old->GetRefCount() > 1 )
        {
            old->UnRef();

            wxPGConstants* soc = wxPropertyGrid::AddConstantsArray(old->GetLabels(),
                                                                   old->GetValues(),
                                                                   false);
            *ci.m_constants = soc;
        }
    }
}

#if wxPG_USE_VALIDATORS
void wxPGProperty::SetValidator ( wxPropertyValidator& validator )
{
    // Validator must have same data type?
    // NOTE: So that int validators can also be used with arrays,
    //   do not use this here.
    //validator.AssertDataType(GetValueType()->GetTypeName());

    // Proper validator removal (delete if refcount reached zero)
    if ( m_validator && m_validator->UnRef() )
        delete m_validator;

    m_validator = validator.Ref();
}

wxPropertyValidator& wxPGProperty::GetValidator () const
{
    if ( m_validator )
    {
        return *m_validator;
    }
    wxFAIL_MSG ( wxT("Do not call GetPropertyValidator for properties that don't have one.") );
    return *((wxPropertyValidator*)NULL);
}

// Validates and sets value
bool wxPGProperty::StdValidationProcedure( wxPGVariant variant )
{
    if ( m_validator )
    {
        wxString s;
#ifdef __WXDEBUG__
        m_validator->AssertDataType(GetValueType()->GetTypeName());
#endif
        if ( !m_validator->Validate(variant,s) )
        {
            ShowError(s);
            return FALSE;
        }
    }
    DoSetValue( variant );
    return TRUE;
}

#endif

bool wxPGProperty::PrepareValueForDialogEditing( wxPropertyGrid* propgrid )
{
    wxPGCtrlClass* primary = propgrid->GetPrimaryEditor();
    if ( primary && propgrid->IsEditorsValueModified() )
    {
         GetEditorClass()->CopyValueFromControl( this, primary );
         return true;
    }
    else if ( m_flags & wxPG_PROP_UNSPECIFIED )
    {
        // Set default value in case it was unspecified
        DoSetValue(GetValueType()->GetDefaultValue());
    }
    return false;
}

// -----------------------------------------------------------------------
// wxPGPropertyWithChildren
// -----------------------------------------------------------------------

/*
WX_PG_IMPLEMENT_CLASSINFO(wxParentProperty)

// Since it has no value argument, we can't use macro implementer
wxPGProperty* wxParentProperty( const wxString& label, const wxString& name )
{
    return new wxParentPropertyClass(label,name);
}


const wxPGValueType* wxPGPropertyWithChildren::GetValueType() const
{
    return wxPG_VALUETYPE(wxString);
}

const wxChar* wxPGPropertyWithChildren::GetClassName() const
{
    return wxT("wxParentProperty");
}

const wxPGEditor* wxPGPropertyWithChildren::GetEditorClass() const
{
    return wxPG_EDITOR(TextCtrl);
}
*/


wxPGPropertyWithChildren::wxPGPropertyWithChildren()
    : wxPGProperty()
{
    m_expanded = 1;
    m_y = -2;
    m_bgColIndex = 0;
    m_parentingType = -1;
}

wxPGPropertyWithChildren::wxPGPropertyWithChildren( const wxString &label, const wxString& name )
    : wxPGProperty(label,name)
{
    m_expanded = 1;
    m_y = -2;
    m_bgColIndex = 0;
    m_parentingType = -1;
}

wxPGPropertyWithChildren::~wxPGPropertyWithChildren()
{
    Empty(); // this deletes items
}

void wxPGPropertyWithChildren::AddChild ( wxPGProperty* prop, int index, bool correct_mode )
{
    if ( index < 0 || (size_t)index >= m_children.GetCount() )
    {
        if ( correct_mode ) prop->m_arrIndex = m_children.GetCount();
        m_children.Add ( (void*)prop );
    }
    else
    {
        m_children.Insert ( (void*)prop, index );
        if ( correct_mode ) FixIndexesOfChildren( index );
    }

    prop->m_parent = this;
}

void wxPGPropertyWithChildren::FixIndexesOfChildren ( size_t starthere )
{
    size_t i;
    for ( i=starthere;i<GetCount();i++)
        Item(i)->m_arrIndex = i;
}

wxPGProperty* wxPGPropertyWithChildren::GetItemAtY ( unsigned int y, unsigned int lh )
{
    // Linear search.
    unsigned int i = 0;
    unsigned int i_max = GetCount();
    unsigned long py = 0xFFFFFFFF;
    wxPGProperty* p = (wxPGProperty*) NULL;

    while ( i < i_max )
    {
        p = Item(i);
        if ( p->m_y >= 0 )
        {
            //wxLogDebug(wxT("vis(%i) > %s"),(int)p->m_y,p->GetLabel().c_str());
            py = (unsigned long)p->m_y;
            if ( (py+lh) > y )
                break;
        }
        i++;
    }
    if ( py <= y && i < i_max )
    {
        // perfectly this item
        wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
        return p;
    }
    else
    {

        // If no visible children, we must retract our steps
        // (should not really happen, so right now we check that it
        // really doesn't).
        //wxASSERT ( py != 0xFFFFFFFF );
        if ( py == 0xFFFFFFFF )
        {
            wxLogDebug(wxT("wxPropertyGrid: \"%s\" (y=%i) did not have visible children (it should)."),m_label.c_str(),(int)m_y);
            return (wxPGProperty*) NULL;
        }

        // We are about to return a child of previous' visible item.

    #ifdef __WXDEBUG__
        if ( i < 1 )
        {
            wxLogDebug ( wxT("WARNING: \"%s\"->GetItemAtY: (i <= 0)"), m_label.c_str() );
            wxLogDebug ( wxT(" \\--> y = %i, py = %i"), (int)y, (int)py );
            if ( p )
                wxLogDebug ( wxT(" \\--> p = \"%s\""), p->GetLabel().c_str() );
            else
                wxLogDebug ( wxT(" \\--> p = None") );
            return (wxPGProperty*) NULL;
        }
    #endif

        // Get previous *visible* parent.
        wxPGPropertyWithChildren* pwc;
        do
        {
            wxASSERT ( i > 0 );
            i--;
            pwc = (wxPGPropertyWithChildren*)Item(i);
        } while ( pwc->m_y < 0 );

        if ( pwc->GetParentingType() != 0 )
        {
        #ifdef __WXDEBUG__
            if ( !pwc->m_expanded || pwc->m_y < 0 )
            {
                wxLogDebug (wxT("WARNING: wxPGPropertyWithChildren::GetItemAtY: Item %s should have been visible and expanded."),pwc->m_label.c_str());
                wxLogDebug (wxT("    (%s[%i]: %s)"),pwc->m_parent->m_label.c_str(),pwc->m_arrIndex,pwc->m_label.c_str());
                //wxLogDebug (wxT("    py=%i"),(int)py);
                return (wxPGProperty*) NULL;
            }
        #endif
            return pwc->GetItemAtY(y,lh);
        }
    }
    return (wxPGProperty*) NULL;
}

void wxPGPropertyWithChildren::Empty()
{
    size_t i;
    if ( m_expanded != wxPG_EXP_OF_COPYARRAY )
        for (i=0;i<GetCount();i++)
            delete ((wxPGProperty*)Item(i));

    m_children.Empty();
}

/*int wxPGPropertyWithChildren::GetParentingType() const
{
// this is normal behaviour for most properties with children - category must override.
    return -1;
}*/

void wxPGPropertyWithChildren::ChildChanged ( wxPGProperty* WXUNUSED(p) )
{
}

wxString wxPGPropertyWithChildren::GetValueAsString ( int arg_flags ) const
{
    wxASSERT_MSG ( GetCount() > 0,
        wxT("If user property does not have any children, it must override GetValueAsString."));

    wxString text;

    int i;
    int i_max = m_children.GetCount();
    int i_max_minus_1 = i_max-1;

    wxPGProperty* curChild = (wxPGProperty*) m_children.Item(0);

    //if ( curChild->GetValueType() == wxPG_VALUETYPE(double) )
    //    wxLogDebug(wxT("%.4f"),m_parentState->GetGrid()->GetPropertyValueAsDouble(wxPGId(curChild)));

    for ( i = 0; i < i_max; i++ )
    {
        wxString s = curChild->GetValueAsString (arg_flags);
        if ( curChild->GetParentingType() == 0 )
            text += s;
        else
            text += wxT("[") + s + wxT("]");

        if ( i < i_max_minus_1 )
        {
            curChild = (wxPGProperty*) m_children.Item(i+1);

            if ( curChild->GetParentingType() == 0 )
                text += wxT("; ");
            else
                text += wxT(" ");
        }
    }

    return text;
}

// Convert semicolon delimited tokens into child values.
// Goes through the children recursively.
bool wxPGPropertyWithChildren::SetValueFromString ( const wxString& text, int )
{
    /*
    wxASSERT_MSG ( GetCount() > 0,
        wxT("If user property does not have any children, it must override SetValueFromString."));

    unsigned int cur_child = 0;

    bool changed = FALSE;

    //if ( !GetCount() )
    //    return changed;

    // Comma is no longer a valid delimeter.
    WX_PG_TOKENIZER1_BEGIN(text,wxT(';'))

        // Set child's value
        wxPGProperty* child = Item(cur_child);

        if ( child->SetValueFromString ( token, wxPG_REPORT_ERROR ) )
        {
            // If modified, set mod flag and store value back to parent
            child->SetFlag ( wxPG_PROP_MODIFIED );
            ChildChanged ( child );
            changed = TRUE;
        }

        cur_child++;
        if ( cur_child >= GetCount() )
            break;

    WX_PG_TOKENIZER1_END()

    return changed;
    */

    wxASSERT_MSG ( GetCount() > 0,
        wxT("If user property does not have any children, it must override SetValueFromString."));

    unsigned int curChild = 0;

    bool changed = FALSE;

    //if ( !GetCount() )
    //    return changed;

    wxString token;
    size_t pos = 0;

    bool addOnlyIfNotEmpty = false;
    const wxChar delimeter = wxT(';');
    wxChar a;

    size_t lastPos = text.length();
    size_t tokenStart = 0xFFFFFF;

    do
    {
        a = text[pos];

        if ( tokenStart != 0xFFFFFF )
        {
            // Token is running
            if ( a == delimeter || a == 0 )
            {
                token = text.substr(tokenStart,pos-tokenStart);
                token.Trim(true);

                //wxLogDebug(wxT("token1: %s"),token.c_str());

                if ( !addOnlyIfNotEmpty || token.length() )
                {

                    wxPGProperty* child = Item(curChild);

                    if ( child->SetValueFromString ( token, wxPG_REPORT_ERROR ) )
                    {
                        // If modified, set mod flag and store value back to parent
                        child->SetFlag ( wxPG_PROP_MODIFIED );
                        ChildChanged ( child );
                        changed = true;
                    }

                    curChild++;
                    if ( curChild >= GetCount() )
                        break;
                }

                tokenStart = 0xFFFFFF;
            }
        }
        else
        {
            // Token is not running
            if ( a != wxT(' ') )
            {

                addOnlyIfNotEmpty = false;

                // Is this a group of tokens?
                if ( a == wxT('[') )
                {
                    int depth = 1;

                    pos++;
                    size_t startPos = pos;

                    // Group item - find end
                    do
                    {
                        a = text[pos];
                        pos++;

                        if ( a == wxT(']') )
                            depth--;
                        else if ( a == wxT('[') )
                            depth++;

                    } while ( depth > 0 && a );

                    token = text.substr(startPos,pos-startPos-1);
                    //wxLogDebug(wxT("token2: %s"),token.c_str());

                    if ( !token.length() )
                        break;

                    wxPGProperty* child = Item(curChild);

                    if ( child->SetValueFromString ( token, wxPG_REPORT_ERROR ) )
                    {
                        // If modified, set mod flag and store value back to parent
                        child->SetFlag ( wxPG_PROP_MODIFIED );
                        ChildChanged ( child );
                        changed = true;
                    }

                    curChild++;
                    if ( curChild >= GetCount() )
                        break;

                    addOnlyIfNotEmpty = true;

                    tokenStart = 0xFFFFFF;
                }
                else
                {
                    tokenStart = pos;

                    if ( a == delimeter )
                    {
                        pos--;
                    }
                }
            }

        }
        pos++;

    }
    while ( pos <= lastPos );

    return changed;
}


void wxPGPropertyWithChildren::RefreshChildren ()
{
}

// -----------------------------------------------------------------------
// wxParentProperty
// -----------------------------------------------------------------------

wxPGProperty* wxParentProperty( const wxString& label, const wxString& name )
{
    return new wxParentPropertyClass (label,name);
}

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxParentProperty,none,TextCtrl)
WX_PG_IMPLEMENT_CLASSINFO(wxParentProperty)

wxParentPropertyClass::wxParentPropertyClass ( const wxString& label, const wxString& name )
    : wxPGPropertyWithChildren(label,name)
{
    m_parentingType = -2;
}

wxParentPropertyClass::~wxParentPropertyClass () { }

void wxParentPropertyClass::DoSetValue ( wxPGVariant value )
{
    const wxString& str = wxPGVariantToString(value);
    m_string = str;
    SetValueFromString(str,wxPG_REPORT_ERROR);
}

wxPGVariant wxParentPropertyClass::DoGetValue () const
{
    return wxPGVariant();
}

void wxParentPropertyClass::ChildChanged ( wxPGProperty* WXUNUSED(p) )
{
    // TODO: Maybe need to fill this method
}

// -----------------------------------------------------------------------
// wxPGRootPropertyClass
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxPGRootProperty,none,TextCtrl)

wxPGRootPropertyClass::wxPGRootPropertyClass()
    : wxPGPropertyWithChildren()
{
    m_parentingType = 1;
}

wxPGRootPropertyClass::~wxPGRootPropertyClass()
{
}

/*int wxPGRootPropertyClass::GetParentingType() const
{
    return 1;
}*/

// -----------------------------------------------------------------------
// wxPropertyCategoryClass
// -----------------------------------------------------------------------

wxPGProperty* wxPropertyCategory ( const wxString& label, const wxString& name )
{
    return new wxPropertyCategoryClass(label,name);
}

WX_PG_IMPLEMENT_CLASSINFO(wxPropertyCategory)

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxPropertyCategory,none,TextCtrl)

wxPropertyCategoryClass::wxPropertyCategoryClass()
    : wxPGPropertyWithChildren()
{
    // don't set colour - prepareadditem method should do this
    m_parentingType = 1;
}

wxPropertyCategoryClass::wxPropertyCategoryClass( const wxString &label, const wxString& name )
    : wxPGPropertyWithChildren(label,name)
{
    // don't set colour - prepareadditem method should do this
    m_parentingType = 1;
}

wxPropertyCategoryClass::~wxPropertyCategoryClass()
{
}

wxString wxPropertyCategoryClass::GetValueAsString ( int ) const
{
    return wxEmptyString;
}

void wxPropertyCategoryClass::CalculateTextExtent ( wxWindow* wnd, wxFont& font )
{
    int x = 0, y = 0;
	wnd->GetTextExtent( m_label, &x, &y, 0, 0, &font );
    m_textExtent = x;
}

// -----------------------------------------------------------------------
// wxCustomProperty
// -----------------------------------------------------------------------

wxPGProperty* wxCustomProperty( const wxString& label, const wxString& name )
{
    return new wxCustomPropertyClass (label,name);
}

WX_PG_IMPLEMENT_CLASSINFO(wxCustomProperty)
wxPG_GETCLASSNAME_IMPLEMENTATION(wxCustomProperty)

const wxPGValueType* wxCustomPropertyClass::GetValueType() const
{
    return wxPG_VALUETYPE(wxString);
}

const wxPGEditor* wxCustomPropertyClass::GetEditorClass() const
{
    return m_editor;
}

wxCustomPropertyClass::wxCustomPropertyClass(const wxString& label,
                                             const wxString& name)
    : wxPGPropertyWithChildren(label,name)
{
    m_parentingType = -2;
    m_editor = wxPGEditor_TextCtrl;
    m_constants = &wxPGGlobalVars->m_emptyConstants;
    m_bitmap = (wxBitmap*) NULL;
    m_callback = (wxPropertyGridCallback) NULL;
    m_paintCallback = (wxPGPaintCallback) NULL;
}

wxCustomPropertyClass::~wxCustomPropertyClass()
{
    wxPGUnRefChoices(m_constants);

    delete m_bitmap;
}

void wxCustomPropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = value.GetString();
}

wxPGVariant wxCustomPropertyClass::DoGetValue () const
{
    return m_value;
}

bool wxCustomPropertyClass::SetValueFromString ( const wxString& text, int /*flags*/ )
{
    if ( text != m_value )
    {
        m_value = text;
        return true;
    }
    return false;
}

wxString wxCustomPropertyClass::GetValueAsString ( int /*arg_flags*/ ) const
{
    return m_value;
}

wxSize wxCustomPropertyClass::GetImageSize() const
{
    if ( m_paintCallback )
        return wxSize(-wxPG_CUSTOM_IMAGE_WIDTH,-wxPG_CUSTOM_IMAGE_WIDTH);
    else if ( m_bitmap && m_bitmap->Ok() )
        return wxSize(m_bitmap->GetWidth(),-1);

    return wxSize(0,0);
}

void wxCustomPropertyClass::OnCustomPaint(wxDC& dc,
                                          const wxRect& rect,
                                          wxPGPaintData& paintdata)
{
    if ( m_paintCallback )
    {
        m_paintCallback(this,dc,rect,paintdata);
    }
    else
    {
        wxASSERT( m_bitmap && m_bitmap->Ok() );

        wxASSERT_MSG( rect.x >= 0,
            wxT("unexpected measure call") );

        dc.DrawBitmap(*m_bitmap,rect.x,rect.y);
    }
}

// Need to do some extra event handling.
bool wxCustomPropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary, wxEvent& event )
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        if ( m_callback )
            return m_callback(propgrid,this,primary,0);
    }
    return false;
}

bool wxCustomPropertyClass::SetValueFromInt ( long value, int )
{
    size_t index = value;
    const wxArrayInt& values = m_constants->GetValues();
    if ( values.GetCount() )
        index = values.Index(value);

    const wxString& sAtIndex = m_constants->GetLabel(index);
    if ( sAtIndex != m_value )
    {
        m_value = sAtIndex;
        return true;
    }

    return false;
}

int wxCustomPropertyClass::GetChoiceInfo ( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
    {
        choiceinfo->m_itemCount = m_constants->GetCount();

        if ( m_constants->GetCount() )
            choiceinfo->m_arrWxString = (wxString*)&m_constants->GetLabel(0);

        choiceinfo->m_constants = &m_constants;
    }

    if ( m_constants->IsOk() )
        return m_constants->GetLabels().Index(m_value);

    return -1;
}

void wxCustomPropertyClass::SetAttribute ( int id, wxVariant value )
{
    if ( id == wxPG_CUSTOM_EDITOR )
    {
        m_editor = (wxPGEditor*) value.GetVoidPtr();
        // TODO: If selected, force re-selection
    }
    else if ( id == wxPG_CUSTOM_IMAGE )
    {
        wxBitmap* bmp = (wxBitmap*) value.GetWxObjectPtr();
        if ( bmp && bmp != &wxNullBitmap )
        {
            wxASSERT( bmp->IsKindOf(CLASSINFO(wxBitmap)) );

            if ( bmp->Ok() )
            {
                wxPropertyGrid* pg = GetParentState()->GetGrid();

                if ( !m_bitmap )
                    m_bitmap = new wxBitmap();

                wxSize maxSize = pg->GetImageSize();

                // Crop bitmap to ideal image height and maximum image width
                if ( bmp->GetWidth() > 64 || bmp->GetHeight() > maxSize.y )
                {
                    int bw = bmp->GetWidth();
                    if ( bw > 64 ) bw = 64;
                    int bh = bmp->GetHeight();
                    if ( bh > maxSize.y ) bh = maxSize.y;
                    *m_bitmap = bmp->GetSubBitmap(wxRect(0,0,bw,bh));
                }
                else
                {
                    *m_bitmap = *bmp;
                }

                m_flags |= wxPG_PROP_CUSTOMIMAGE;
            }
        }
        else
        {
            delete m_bitmap;
            m_bitmap = (wxBitmap*) NULL;
            if ( !m_paintCallback )
                m_flags &= ~(wxPG_PROP_CUSTOMIMAGE);
        }
        // TODO: If selected, force re-selection, redraw
    }
    else if ( id == wxPG_CUSTOM_CALLBACK )
    {
        m_callback = (wxPropertyGridCallback) value.GetVoidPtr();
    }
    else if ( id == wxPG_CUSTOM_PAINT_CALLBACK )
    {
        void* voidValue = value.GetVoidPtr();
        m_paintCallback = (wxPGPaintCallback) voidValue;
        if ( voidValue )
            m_flags |= wxPG_PROP_CUSTOMIMAGE;
        else if ( !m_bitmap )
            m_flags &= ~(wxPG_PROP_CUSTOMIMAGE);
    }
    else if ( id == wxPG_CUSTOM_PRIVATE_CHILDREN )
    {
        if ( value.GetLong() )
            m_parentingType = -1;
        else
            m_parentingType = -2;
    }
}

// -----------------------------------------------------------------------
// wxParentProperty
// -----------------------------------------------------------------------

// This is a simple property which holds sub-properties. Has default editing textctrl
// based editing capability. In essence, it is a category that has look and feel
// of a property, and which children can be edited via the textctrl.
/*class wxParentPropertyClass : public wxPGPropertyWithChildren
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

wxPGProperty* wxParentProperty( const wxString& label, const wxString& name )
{
    return new wxParentPropertyClass (label,name);
}

WX_PG_IMPLEMENT_CLASSINFO(wxParentProperty)

WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxParentProperty,none,TextCtrl)

wxParentPropertyClass::wxParentPropertyClass ( const wxString& label, const wxString& name )
    : wxPGPropertyWithChildren(label,name)
{
    m_parentingType = -2;
}

wxParentPropertyClass::~wxParentPropertyClass () { }

void wxParentPropertyClass::DoSetValue ( wxPGVariant value )
{
    const wxString& str = wxPGVariantToString(value);
    m_string = str;
    SetValueFromString(str,wxPG_REPORT_ERROR);
}

wxPGVariant wxParentPropertyClass::DoGetValue () const
{
    return wxPGVariant();
}

void wxParentPropertyClass::ChildChanged ( wxPGProperty* WXUNUSED(p) )
{
    // TODO: Maybe need to fill this method
}*/

// -----------------------------------------------------------------------
// wxPGEditor
// -----------------------------------------------------------------------

wxPGEditor::~wxPGEditor()
{
}

void wxPGEditor::DrawValue ( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const
{
    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        dc.DrawText ( property->GetDisplayedString(), rect.x+wxPG_XBEFORETEXT, rect.y );
    /*else
        wxLogDebug(wxT("%s's value was unspecified"),property->GetLabel().c_str());*/
}

void wxPGEditor::SetControlStringValue ( wxPGCtrlClass*, const wxString& ) const
{
}

void wxPGEditor::SetControlIntValue ( wxPGCtrlClass*, int ) const
{
}

int wxPGEditor::AppendItem ( wxPGCtrlClass*, const wxString& ) const
{
    return -1;
}

bool wxPGEditor::CanContainCustomImage () const
{
    return FALSE;
}

// -----------------------------------------------------------------------
// wxPGClipperWindow
// -----------------------------------------------------------------------

#if wxPG_ENABLE_CLIPPER_WINDOW

//
// Clipper window should no longer be necessary
//
class wxPGClipperWindow : public wxWindow
{
    DECLARE_CLASS(wxPGClipperWindow)
public:

    wxPGClipperWindow()
        : wxWindow()
    {
        wxPGClipperWindow::Init();
    }

    wxPGClipperWindow(wxWindow* parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize)
    {
        Init();
        Create(parent,id,pos,size);
    }

    void Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize);

    virtual ~wxPGClipperWindow();

    virtual bool ProcessEvent(wxEvent& event);

    inline wxWindow* GetControl () const { return m_ctrl; }

    // This is called before wxControl is constructed.
    void GetControlRect ( int xadj, int yadj, wxPoint& pt, wxSize& sz );

    // This is caleed after wxControl has been constructed.
    void SetControl ( wxWindow* ctrl );

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void SetFocus();

    virtual bool SetFont(const wxFont& font);

    inline int GetXClip () const { return m_xadj; }

    inline int GetYClip () const { return m_yadj; }

protected:
    wxWindow*       m_ctrl;

    int             m_xadj; // Horizontal border clip.

    int             m_yadj; // Vertical border clip.

private:
    void Init ()
    {
        m_ctrl = (wxWindow*) NULL;
    }
};

IMPLEMENT_CLASS(wxPGClipperWindow,wxWindow)

// This is called before wxControl is constructed.
void wxPGClipperWindow::GetControlRect ( int xadj, int yadj, wxPoint& pt, wxSize& sz )
{
    m_xadj = xadj;
    m_yadj = yadj;
    pt.x = -xadj;
    pt.y = -yadj;
    wxSize own_size = GetSize();
    sz.x = own_size.x+(xadj*2);
    sz.y = own_size.y+(yadj*2);
}

// This is caleed after wxControl has been constructed.
void wxPGClipperWindow::SetControl ( wxWindow* ctrl )
{
    m_ctrl = ctrl;

    // GTK requires this.
    ctrl->SetSizeHints(3,3);

    // Correct size of this window to match the child.
    wxSize sz = GetSize();
    wxSize chsz = ctrl->GetSize();

    int hei_adj = chsz.y - (sz.y+(m_yadj*2));
    if ( hei_adj )
        SetSize(sz.x,chsz.y-(m_yadj*2));

/*    Connect ( wxPG_SUBID1, wxEVT_COMMAND_TEXT_UPDATED,
            (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent );*/

}

void wxPGClipperWindow::Refresh( bool eraseBackground, const wxRect *rect )
{
    wxWindow::Refresh(false,rect);
    if ( m_ctrl )
        // FIXME: Rect to sub-ctrl refresh too
        m_ctrl->Refresh(eraseBackground);
}

// Pass focus to control
void wxPGClipperWindow::SetFocus()
{
    if ( m_ctrl )
        m_ctrl->SetFocus();
    else
        wxWindow::SetFocus();
}

bool wxPGClipperWindow::SetFont(const wxFont& font)
{
    bool res = wxWindow::SetFont(font);
    if ( m_ctrl )
        return m_ctrl->SetFont(font);
    return res;
}

void wxPGClipperWindow::Create(wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size )
{
    wxWindow::Create(parent,id,pos,size);
}

wxPGClipperWindow::~wxPGClipperWindow()
{
}

bool wxPGClipperWindow::ProcessEvent(wxEvent& event)
{
    if ( event.GetEventType() == wxEVT_SIZE )
    {
        if ( m_ctrl )
        {
            // Maintain correct size relationship.
            wxSize sz = GetSize();
            m_ctrl->SetSize(sz.x+(m_xadj*2),sz.y+(m_yadj*2));
            event.Skip();
            return FALSE;
        }
    }
    return wxWindow::ProcessEvent(event);
}

#endif // wxPG_ENABLE_CLIPPER_WINDOW

// -----------------------------------------------------------------------
// wxPGTextCtrlEditor
// -----------------------------------------------------------------------

#if wxPG_NAT_TEXTCTRL_BORDER_X ||wxPG_NAT_TEXTCTRL_BORDER_Y
    #define wxPG_NAT_TEXTCTRL_BORDER_ANY    1
    #define wxPGDeclareRealTextCtrl(WND) \
        wxASSERT( WND ); \
        wxCCustomTextCtrl* tc = (wxCCustomTextCtrl*)((wxPGClipperWindow*)WND)->GetControl()
#else
    #define wxPG_NAT_TEXTCTRL_BORDER_ANY    0
    #define wxPGDeclareRealTextCtrl(WND) \
        wxASSERT( WND ); \
        wxCCustomTextCtrl* tc = (wxCCustomTextCtrl*)WND
#endif

class wxPGTextCtrlEditor : public wxPGEditor
{
public:
    virtual ~wxPGTextCtrlEditor();

    WX_PG_IMPLEMENT_EDITOR_CLASS_STD_METHODS()

    virtual void SetControlStringValue ( wxPGCtrlClass* ctrl, const wxString& txt ) const;
};

WX_PG_IMPLEMENT_EDITOR_CLASS_CONSTFUNC(TextCtrl,wxPGTextCtrlEditor,wxPGEditor)

wxPGCtrlClass* wxPGTextCtrlEditor::CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxPGCtrlClass** ) const
{
    wxString text;

    // If has children and limited editing, then don't create.
    if ((property->GetFlags() & wxPG_PROP_NOEDITOR) &&
        property->GetParentingType() < 0 &&
        wxStrcmp(property->GetClassName(),wxT("wxCustomProperty")) != 0)
        return (wxPGCtrlClass*) NULL;

    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        text = property->GetValueAsString(0);

    return propgrid->GenerateEditorTextCtrl(pos,sz,text,(wxPGCtrlClass*)NULL);
}

void wxPGTextCtrlEditor::UpdateControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const
{
    wxPGDeclareRealTextCtrl(ctrl);
    tc->SetValue(property->GetDisplayedString());
}

bool wxPGTextCtrlEditor::OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property,
    wxPGCtrlClass* ctrl, wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER )
    {
        if ( propgrid->IsEditorsValueModified() )
        {
            if ( CopyValueFromControl( property, ctrl ) )
                return TRUE;

            propgrid->EditorsValueWasNotModified();
        }
    }
    else if ( event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED )
    {
    #if !wxPG_USE_CUSTOM_CONTROLS
        // If value is unspecified and character count is zero,
        // then do not set as modified.
        if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) ||
             !ctrl || ((wxCCustomTextCtrl*)ctrl)->GetLastPosition() > 0 )
    #endif
            propgrid->EditorsValueWasModified();
    }
    return FALSE;
}

bool wxPGTextCtrlEditor::CopyValueFromControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const
{
    wxPGDeclareRealTextCtrl(ctrl);

    bool res = property->SetValueFromString(tc->GetValue(),0);

    // Changing unspecified always causes event (returning
    // TRUE here should be enough to trigger it).
    if ( !res && property->IsFlagSet(wxPG_PROP_UNSPECIFIED) )
        res = TRUE;

    return res;
}

void wxPGTextCtrlEditor::SetValueToUnspecified ( wxPGCtrlClass* ctrl ) const
{
    wxPGDeclareRealTextCtrl(ctrl);

#if wxPG_USE_CUSTOM_CONTROLS
    tc->SetValue(wxEmptyString);
#else
    tc->Remove(0,tc->GetValue().length());
#endif
}

void wxPGTextCtrlEditor::SetControlStringValue ( wxPGCtrlClass* ctrl, const wxString& txt ) const
{
    wxPGDeclareRealTextCtrl(ctrl);

    tc->SetValue(txt);
}

wxPGTextCtrlEditor::~wxPGTextCtrlEditor() { }

// -----------------------------------------------------------------------
// wxPGChoiceEditor
// -----------------------------------------------------------------------

class wxPGChoiceEditor : public wxPGEditor
{
public:
    virtual ~wxPGChoiceEditor();

    WX_PG_IMPLEMENT_EDITOR_CLASS_STD_METHODS()

    virtual void SetControlIntValue ( wxPGCtrlClass* ctrl, int value ) const;
    virtual void SetControlStringValue ( wxPGCtrlClass* ctrl, const wxString& txt ) const;

    virtual int AppendItem ( wxPGCtrlClass* ctrl, const wxString& label ) const;
    virtual bool CanContainCustomImage () const;
};

WX_PG_IMPLEMENT_EDITOR_CLASS_CONSTFUNC(Choice,wxPGChoiceEditor,wxPGEditor)

#if wxPG_USE_CUSTOM_CONTROLS

static wxSize prop_image_paint_func ( wxDC& dc, const wxRect& rect, int index, void* userdata )
{
    wxPGProperty* p = (wxPGProperty*)userdata;
    wxPGPaintData paintdata;
    paintdata.m_parent = NULL;
    paintdata.m_choiceItem = index;
    paintdata.m_drawnWidth = rect.width;
    p->OnCustomPaint ( dc, rect, paintdata );
    return wxSize(paintdata.m_drawnWidth,rect.height);
}

#else

void wxPropertyGrid::OnComboItemPaint (wxPGComboBox* pCb,int item,wxDC& dc,
    wxRect& rect, int flags )
{
    //wxLogDebug( wxT("wxPropertyGrid::OnComboItemPaint") );

    wxASSERT ( IsKindOf(CLASSINFO(wxPropertyGrid)) );

    wxPGProperty* p = m_selected;

    //
    // Decide what custom image size to use
    //
    wxSize cis = GetImageSize(p);

    if ( rect.x < 0 && !(m_iFlags & wxPG_FL_SELECTED_IS_PAINT_FLEXIBLE) )
    {
        rect.width = cis.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 + 9;
        rect.height = cis.y + 2;
        return;
    }

    wxPGPaintData paintdata;
    paintdata.m_parent = NULL;
    paintdata.m_choiceItem = item;

    // this is by the current (1.0.0b) spec - if painting control, item is -1
    if ( flags & wxODCB_CB_PAINTING_CONTROL )
        paintdata.m_choiceItem = -1;

    if ( rect.x >= 0 )
    {
        wxPoint pt(rect.x + wxPG_CONTROL_MARGIN - wxPG_CHOICEXADJUST - 1,
                   rect.y + 1);

        if ( cis.x > 0 )
        {
            pt.x += wxCC_CUSTOM_IMAGE_MARGIN1;
            wxRect r(pt.x,pt.y,cis.x,cis.y);

            if ( flags & wxODCB_CB_PAINTING_CONTROL )
            {
                //r.width = wxPG_CUSTOM_IMAGE_WIDTH;
                r.width = cis.x;
                r.height = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
            }

            paintdata.m_drawnWidth = r.width;

            dc.SetPen(m_colPropFore);
            if ( item >= 0 )
                p->OnCustomPaint ( dc, r, paintdata );
            else
                dc.DrawRectangle( r );

            pt.x += paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN2 - 1;
        }
        else
            // TODO: This aligns text so that it seems to be horizontally
            //       on the same line as property values. Not really
            //       sure if its needed, but seems to not cause any harm.
            pt.x -= 1;

        //
        // Draw text
        //

        pt.y += (rect.height-m_fontHeight)/2 - 1;

        wxString text;
        if ( item >= 0 )
        {
            if ( !(flags & wxODCB_CB_PAINTING_CONTROL) )
                text = pCb->GetString(item);
            else
                text = p->GetValueAsString(0);
        }

        dc.DrawText ( text, pt.x + wxPG_XBEFORETEXT, pt.y );

    }
    else
    {
        p->OnCustomPaint ( dc, rect, paintdata );
        rect.height = paintdata.m_drawnHeight + 2;
        rect.width = cis.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 + 9;
    }
}

#endif

wxPGCtrlClass* wxPGChoiceEditor::CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxPGCtrlClass** ) const
{
    wxString        defstring;
    wxPGChoiceInfo  choiceinfo;

    // Get choices.
    choiceinfo.m_arrWxString = (wxString*) NULL;
    choiceinfo.m_arrWxChars = (const wxChar**) NULL;
    choiceinfo.m_itemCount = 0;

    int index = property->GetChoiceInfo ( &choiceinfo );

    if ( property->GetFlags() & wxPG_PROP_UNSPECIFIED )
        index = -1;
    else
        defstring = property->GetDisplayedString();

    // SLAlloc allows fast conversion using potentially pre-allocated wxStrings
    // (and appending is out of question due to performance problems on some platforms).

    // If itemcount is < 0, fill wxArrayString using GetEntry
    if ( choiceinfo.m_itemCount < 0 )
    {
        wxBaseEnumPropertyClass* ep = (wxBaseEnumPropertyClass*) property;
        size_t i = 0;
        const wxString* entry_label;
        int entry_value;

        wxArrayString& sl = propgrid->SLGet();

        entry_label = ep->GetEntry(i,&entry_value);
        while ( entry_label )
        {
            if ( sl.GetCount() > i )
                sl[i] = *entry_label;
            else
                sl.Add(*entry_label);
            i++;
            entry_label = ep->GetEntry(i,&entry_value);
        }
        choiceinfo.m_itemCount = ((int)i) - 1;
    }
    else if ( !choiceinfo.m_arrWxString )
    {
        wxASSERT ( choiceinfo.m_arrWxChars || !choiceinfo.m_itemCount );
        propgrid->SLAlloc ( choiceinfo.m_itemCount, choiceinfo.m_arrWxChars );
        if ( choiceinfo.m_itemCount )
            choiceinfo.m_arrWxString = &propgrid->SLGet().Item(0);
    }

    wxCCustomComboBox* cb;

    wxPoint po(pos);
    wxSize si(sz);
#if !wxPG_USE_CUSTOM_CONTROLS
    po.y += wxPG_CHOICEYADJUST;
    si.y -= (wxPG_CHOICEYADJUST*2);
#endif

#if wxPG_NAT_CHOICE_BORDER_ANY
    po.x += (wxPG_CHOICEXADJUST+wxPG_NAT_CHOICE_BORDER_X);
    si.x -= (wxPG_CHOICEXADJUST+wxPG_NAT_CHOICE_BORDER_X);
    wxPGClipperWindow* wnd = new wxPGClipperWindow(propgrid,wxPG_SUBID1,po,si);
    wxWindow* ctrl_parent = wnd;
    wnd->GetControlRect(wxPG_NAT_CHOICE_BORDER_X,wxPG_NAT_CHOICE_BORDER_Y,po,si);
#else
    po.x += wxPG_CHOICEXADJUST;
    si.x -= wxPG_CHOICEXADJUST;
# if !wxPG_USE_CUSTOM_CONTROLS
    wxWindow* ctrl_parent = propgrid;
# endif
#endif

#if wxPG_USE_CUSTOM_CONTROLS

    wxSize imagesz = property->GetImageSize();

    long flags = wxNO_BORDER|wxCH_CC_DOUBLE_CLICK_CYCLES|
        wxCH_CC_DROPDOWN_ANCHOR_RIGHT|wxCB_READONLY;

    if ( imagesz.x < 0 )
    {
        if ( imagesz.x == -1 )
            imagesz.x = wxPG_CUSTOM_IMAGE_WIDTH;
        else
            imagesz.x = -imagesz.x;
    }

    if ( imagesz.x > 0 && imagesz.y < 1 )
        flags |= wxCH_CC_IMAGE_EXTENDS;

    if ( imagesz.y < -1 )
    {
        imagesz.y = -imagesz.y;
    }

    cb = new wxCCustomComboBox ( propgrid->GetCCManager(), wxPG_SUBID1, wxEmptyString,
        pos, sz, choiceinfo.m_itemCount, (const wxChar**) choiceinfo.m_arrWxString,
        flags, imagesz );

    cb->SetImagePaintFunction ( prop_image_paint_func, property );

    propgrid->Connect( propgrid->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
        (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent );

#else

    int odcb_flags = wxCB_READONLY|wxNO_BORDER/*|wxWANTS_CHARS*/;

    if ( !(property->GetFlags() & wxPG_PROP_CUSTOMIMAGE) )
        odcb_flags |= wxODCB_STD_CONTROL_PAINT;

    if ( (property->GetFlags() & wxPG_PROP_USE_DCC) &&
         (property->GetClassName()==wxBoolProperty_ClassName) )
        odcb_flags |= wxODCB_DOUBLE_CLICK_CYCLES;

    cb = new wxPGComboBox();
#ifdef __WXMSW__
    cb->Hide();
#endif
    cb->Create(ctrl_parent,
               wxPG_SUBID1,
               wxString(wxEmptyString),
               po,
               si,
               choiceinfo.m_itemCount,choiceinfo.m_arrWxString,
               (wxComboPaintCallback) &wxPropertyGrid::OnComboItemPaint,
               odcb_flags);

    int ext_right = propgrid->GetClientSize().x - (po.x+si.x);

    cb->SetPopupExtents( 1, ext_right );

    if ( property->GetFlags() & wxPG_PROP_CUSTOMIMAGE )
    {
        wxSize imageSize = propgrid->GetImageSize(property);
        cb->SetCustomPaintArea( imageSize.x+6 );
    }

    propgrid->Connect( wxPG_SUBID1, wxEVT_COMMAND_COMBOBOX_SELECTED,
        (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent );

#endif

    if ( index >= 0 && index < choiceinfo.m_itemCount )
    {
        cb->SetSelection( index );
        if ( defstring.length() )
            cb->SetValue( defstring );
    }
    else
        cb->SetSelection( -1 );

#ifdef __WXMSW__
    cb->Show();
#endif

#if wxPG_NAT_CHOICE_BORDER_ANY
    wnd->SetControl(cb);
    return (wxPGCtrlClass*) wnd;
#else
    return (wxPGCtrlClass*) cb;
#endif
}

void wxPGChoiceEditor::UpdateControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const
{
    wxASSERT ( ctrl );
#if wxPG_NAT_CHOICE_BORDER_ANY
    wxCCustomComboBox* cb = (wxCCustomComboBox*)((wxPGClipperWindow*)ctrl)->GetControl();
#else
    wxCCustomComboBox* cb = (wxCCustomComboBox*)ctrl;
#endif
    wxASSERT ( cb->IsKindOf(CLASSINFO(wxCCustomComboBox)));
    int ind = property->GetChoiceInfo( (wxPGChoiceInfo*)NULL );
    cb->SetSelection(ind);
}

int wxPGChoiceEditor::AppendItem ( wxPGCtrlClass* ctrl, const wxString& label ) const
{
    wxASSERT ( ctrl );
#if wxPG_NAT_CHOICE_BORDER_ANY
    wxCCustomComboBox* cb = (wxCCustomComboBox*)((wxPGClipperWindow*)ctrl)->GetControl();
#else
    wxCCustomComboBox* cb = (wxCCustomComboBox*)ctrl;
#endif
    wxASSERT ( cb->IsKindOf(CLASSINFO(wxCCustomComboBox)));
    return cb->Append(label);
}

bool wxPGChoiceEditor::OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property,
    wxPGCtrlClass* ctrl, wxEvent& event ) const
{
#if wxPG_USE_CUSTOM_CONTROLS
    if ( event.GetEventType() == wxEVT_COMMAND_CHOICE_SELECTED )
#else
    if ( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
#endif
    {
        if ( CopyValueFromControl( property, ctrl ) )
        {
            /*
        #if wxPG_USE_CUSTOM_CONTROLS
            // Make sure we show the correct string.
            // (for example, in wxColourProperty show "(R,G,B)" instead of "Custom")
            // TODO: Maybe move this to CopyValueFromControl.
            wxString disp_str = property->GetDisplayedString();
            if ( disp_str != ((wxCCustomComboBox*)ctrl)->GetValue() )
                ((wxCCustomComboBox*)ctrl)->SetValue(disp_str);
        #endif
        */
            return TRUE;
        }

        propgrid->EditorsValueWasNotModified();

        wxPropertyGridState::ClearPropertyAndChildrenFlags(property,wxPG_PROP_UNSPECIFIED);
    }
    return FALSE;
}

bool wxPGChoiceEditor::CopyValueFromControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const
{
#if wxPG_NAT_CHOICE_BORDER_ANY
    wxCCustomComboBox* cb = (wxCCustomComboBox*)((wxPGClipperWindow*)ctrl)->GetControl();
#else
    wxCCustomComboBox* cb = (wxCCustomComboBox*)ctrl;
#endif

    int index = cb->GetSelection();

    if ( index != property->GetChoiceInfo( (wxPGChoiceInfo*) NULL ) ||
        // Changing unspecified always causes event (returning
        // TRUE here should be enough to trigger it).
         property->IsFlagSet(wxPG_PROP_UNSPECIFIED)
       )
    {
        property->SetValueFromInt(index,0);
        return TRUE;
    }
    return FALSE;
}

//#if wxPG_USE_CUSTOM_CONTROLS
void wxPGChoiceEditor::SetControlStringValue ( wxPGCtrlClass* ctrl, const wxString& txt ) const
{
#if wxPG_NAT_CHOICE_BORDER_ANY
    wxCCustomComboBox* cb = (wxCCustomComboBox*)((wxPGClipperWindow*)ctrl)->GetControl();
#else
    wxCCustomComboBox* cb = (wxCCustomComboBox*)ctrl;
#endif
    wxASSERT ( cb );
    cb->SetValue(txt);
}
//#endif

void wxPGChoiceEditor::SetControlIntValue ( wxPGCtrlClass* ctrl, int value ) const
{
#if wxPG_NAT_CHOICE_BORDER_ANY
    wxCCustomComboBox* cb = (wxCCustomComboBox*)((wxPGClipperWindow*)ctrl)->GetControl();
#else
    wxCCustomComboBox* cb = (wxCCustomComboBox*)ctrl;
#endif
    wxASSERT ( cb );
    cb->SetSelection(value);
}

void wxPGChoiceEditor::SetValueToUnspecified ( wxPGCtrlClass* ctrl ) const
{
#if wxPG_NAT_CHOICE_BORDER_ANY
    wxCCustomComboBox* cb = (wxCCustomComboBox*)((wxPGClipperWindow*)ctrl)->GetControl();
#else
    wxCCustomComboBox* cb = (wxCCustomComboBox*)ctrl;
#endif
    cb->SetSelection(-1);
}

bool wxPGChoiceEditor::CanContainCustomImage () const
{
#if wxPG_USE_CUSTOM_CONTROLS
    return FALSE;
#else
    return TRUE;
#endif
}

wxPGChoiceEditor::~wxPGChoiceEditor() { }

// -----------------------------------------------------------------------
// wxPGChoiceAndButtonEditor
// -----------------------------------------------------------------------

class wxPGChoiceAndButtonEditor : public wxPGChoiceEditor
{
public:
    virtual ~wxPGChoiceAndButtonEditor();
    virtual wxPGCtrlClass* CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxPGCtrlClass** psecondary ) const;
};

// This simpler implement_editor macro doesn't define class body.
WX_PG_IMPLEMENT_EDITOR_CLASS_CONSTFUNC(ChoiceAndButton,wxPGChoiceAndButtonEditor,wxPGChoiceEditor)

wxPGCtrlClass* wxPGChoiceAndButtonEditor::CreateControls (wxPropertyGrid* propgrid,
        wxPGProperty* property, const wxPoint& pos, const wxSize& sz,
        wxPGCtrlClass** psecondary ) const
{

    // Size of button.
    // Use one two units smaller to match size of the combo's dropbutton.
    // (normally a bigger button is used because it looks better)
    int bt_wid = sz.y;
#if !wxPG_USE_CUSTOM_CONTROLS
    bt_wid -= 2;
#endif
    wxSize bt_sz(bt_wid,bt_wid);

    // Position of button.
    wxPoint bt_pos(pos.x+sz.x-bt_sz.x,pos.y);
#if !wxPG_USE_CUSTOM_CONTROLS
    bt_pos.y += 1;
#endif

    wxPGCtrlClass* bt = propgrid->GenerateEditorButton ( bt_pos, bt_sz );

    // Size of choice.
    wxSize ch_sz(sz.x-bt->GetSize().x,sz.y);

    wxPGCtrlClass* ch = wxPG_EDITOR(Choice)->CreateControls(propgrid,property,
        pos,ch_sz,(wxPGCtrlClass**)NULL);

#ifdef __WXMSW__
    bt->Show();
#endif

    *psecondary = bt;
    return ch;
}

wxPGChoiceAndButtonEditor::~wxPGChoiceAndButtonEditor() { }

// -----------------------------------------------------------------------
// wxPGTextCtrlAndButtonEditor
// -----------------------------------------------------------------------

class wxPGTextCtrlAndButtonEditor : public wxPGTextCtrlEditor
{
public:
    virtual ~wxPGTextCtrlAndButtonEditor();
    virtual wxPGCtrlClass* CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxPGCtrlClass** psecondary ) const;
};

// This simpler implement_editor macro doesn't define class body.
WX_PG_IMPLEMENT_EDITOR_CLASS_CONSTFUNC(TextCtrlAndButton,wxPGTextCtrlAndButtonEditor,wxPGTextCtrlEditor)

wxPGCtrlClass* wxPGTextCtrlAndButtonEditor::CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz, wxPGCtrlClass** psecondary ) const
{
    return propgrid->GenerateEditorTextCtrlAndButton ( pos, sz, psecondary,
        property->GetFlags() & wxPG_PROP_NOEDITOR, property);
}

wxPGTextCtrlAndButtonEditor::~wxPGTextCtrlAndButtonEditor() { }

// -----------------------------------------------------------------------
// wxPGCheckBoxEditor
// -----------------------------------------------------------------------

#if wxPG_INCLUDE_CHECKBOX

class wxPGCheckBoxEditor : public wxPGEditor
{
public:
    virtual ~wxPGCheckBoxEditor();

    WX_PG_IMPLEMENT_EDITOR_CLASS_STD_METHODS()

    virtual void DrawValue ( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const;

    virtual void SetControlIntValue ( wxPGCtrlClass* ctrl, int value ) const;
};

WX_PG_IMPLEMENT_EDITOR_CLASS_CONSTFUNC(CheckBox,wxPGCheckBoxEditor,wxPGEditor)


// state argument: 0x01 = set if checked
//                 0x02 = set if rectangle should be bold
static void DrawSimpleCheckBox ( wxDC& dc, const wxRect& rect, int box_hei, int state, const wxColour& linecol )
{

    //box_hei-=2;
    // Box rectangle.
    wxRect r(rect.x+wxPG_XBEFORETEXT,rect.y+((rect.height-box_hei)/2),box_hei,box_hei);

    // Draw check mark first because it is likely to overdraw the
    // surrounding rectangle.
    if ( state & 1 )
    {
        wxRect r2(r.x+wxPG_CHECKMARK_XADJ,
                  r.y+wxPG_CHECKMARK_YADJ,
                  r.width+wxPG_CHECKMARK_WADJ,
                  r.height+wxPG_CHECKMARK_HADJ);
    #if wxPG_CHECKMARK_DEFLATE
        r2.Deflate(wxPG_CHECKMARK_DEFLATE);
    #endif
        dc.DrawCheckMark(r2);

        // This would draw a simple cross check mark.
        // dc.DrawLine(r.x,r.y,r.x+r.width-1,r.y+r.height-1);
        // dc.DrawLine(r.x,r.y+r.height-1,r.x+r.width-1,r.y);

    }

    if ( !(state & 2) )
    {
        // Pen for thin rectangle.
        dc.SetPen(linecol);
    }
    else
    {
        // Pen for bold rectangle.
        wxPen linepen(linecol,2,wxSOLID);
        linepen.SetJoin(wxJOIN_MITER); // This prevents round edges.
        dc.SetPen(linepen);
        r.x++;
        r.y++;
        r.width--;
        r.height--;
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(r);
    dc.SetPen(*wxTRANSPARENT_PEN);
}

//
// Real simple custom-drawn checkbox-without-label class.
//
class wxSimpleCheckBox : public wxPGCtrlClass
{
public:

    void SetValue ( int value );

#if !wxPG_USE_CUSTOM_CONTROLS

    wxSimpleCheckBox(wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize )
        : wxWindow(parent,id,pos,size,0/*wxWANTS_CHARS*/)
    {

        // Due to SetOwnFont stuff necessary for GTK+ 1.2, we need to have this
        SetFont ( parent->GetFont() );

        m_state = 0;
        m_boxHeight = ((wxPropertyGrid*)parent)->GetFontHeight();
    }

    virtual bool ProcessEvent(wxEvent& event);

#else

    wxSimpleCheckBox ( wxCustomControlManager* manager, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize )
        : wxPGCtrlClass(manager,id,pos,size)
    {
        m_cbh.Create(this,pos,size);
        manager->AddChildData(&m_cbh);
        m_state = 0;
        m_boxHeight = ((wxPropertyGrid*)manager->GetWindow())->GetFontHeight();
    }

    virtual void DoDraw ( wxDC& dc, const wxRect& rect );
    virtual bool OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata );
    virtual bool OnKeyEvent ( wxKeyEvent& event );

protected:
    wxCustomControlHandler  m_cbh;

public:
#endif // !wxPG_USE_CUSTOM_CONTROLS


    int m_state;
    int m_boxHeight;
};

// value = 2 means toggle
void wxSimpleCheckBox::SetValue ( int value )
{
    if ( value > 1 )
    {
        m_state++;
        if ( m_state > 1 ) m_state = 0;
    }
    else
    {
        m_state = value;
    }
    Refresh();

    wxCommandEvent evt(wxEVT_COMMAND_CHECKBOX_CLICKED,GetParent()->GetId());
    ((wxPropertyGrid*)GetParent())->OnCustomEditorEvent(evt);
}

#if !wxPG_USE_CUSTOM_CONTROLS

bool wxSimpleCheckBox::ProcessEvent(wxEvent& event)
{
    if ( ( (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK)
          && ((wxMouseEvent&)event).m_x > (wxPG_XBEFORETEXT-2)
          && ((wxMouseEvent&)event).m_x <= (wxPG_XBEFORETEXT-2+m_boxHeight) )
       )
    {
        SetValue(2);
        return TRUE;
    }
    else if ( event.GetEventType() == wxEVT_PAINT )
    {
        wxPaintDC dc(this);

        wxRect rect(0,0,GetSize().x,GetSize().y);
        wxColour bgcol = GetBackgroundColour();
        dc.SetBrush(bgcol);
        dc.SetPen(bgcol);
        rect.x -= 1;
        rect.width += 1;
        dc.DrawRectangle(rect);

        m_boxHeight = ((wxPropertyGrid*)GetParent())->GetFontHeight();

        wxColour txcol = GetForegroundColour();

        int state = m_state;
        if ( m_font.GetWeight() == wxBOLD )
            state |= 2;

        DrawSimpleCheckBox(dc,rect,m_boxHeight,
            state,txcol);

        // If focused, indicate it somehow.
        if ( wxWindow::FindFocus() == this )
        {
            rect.x += 1;
            rect.width -= 1;

            wxPen pen(txcol,1,wxDOT);

            // TODO: Remove this line after CreatePen/ExtCreatePen issue fixed in main lib
            pen.SetCap( wxCAP_BUTT );

            dc.SetPen(pen);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(rect);
        }

        return TRUE;
    }
    else if ( event.GetEventType() == wxEVT_SIZE ||
              event.GetEventType() == wxEVT_SET_FOCUS ||
              event.GetEventType() == wxEVT_KILL_FOCUS
            )
    {
        Refresh();
    }
    else if ( event.GetEventType() == wxEVT_KEY_DOWN )
    {
        wxKeyEvent& keyEv = (wxKeyEvent&) event;

        /*
#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )
        if ( keyEv.GetKeyCode() == WXK_TAB )
        {
            Navigate(wxNavigationKeyEvent::FromTab|
                     (!keyEv.ShiftDown()?wxNavigationKeyEvent::IsForward:
                                         wxNavigationKeyEvent::IsBackward));
        }
        else
#endif
        */
        if ( keyEv.GetKeyCode() == WXK_SPACE )
        {
            SetValue(2);
            return TRUE;
        }
    }
    return wxWindow::ProcessEvent(event);
}

#else // !wxPG_USE_CUSTOM_CONTROLS

void wxSimpleCheckBox::DoDraw ( wxDC& dc, const wxRect& rect )
{
    wxRect r(rect);
    wxColour bgcol = GetBackgroundColour();

    dc.SetBrush(bgcol);
    dc.SetPen(bgcol);

    dc.DrawRectangle(r);

    r.x -= 1;
    r.width += 1;

    wxColour txcol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

    int state = m_state;
    if ( m_pFont->GetWeight() == wxBOLD )
        state |= 2;

    m_boxHeight = ((wxPropertyGrid*)GetParent())->GetFontHeight();
    DrawSimpleCheckBox(dc,r,m_boxHeight,
        state, txcol );

    // If focused, indicate it somehow.
    if ( HasFocus() )
    {
        wxPen pen(txcol,1,wxDOT);

        // TODO: Remove this line after CreatePen/ExtCreatePen issue fixed in main lib
        pen.SetCap( wxCAP_BUTT );

        dc.SetPen(pen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(rect);
    }
}

bool wxSimpleCheckBox::OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* )
{
    if ( (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK)
         && ((wxMouseEvent&)event).m_x > (wxPG_XBEFORETEXT-2)
         && ((wxMouseEvent&)event).m_x <= (wxPG_XBEFORETEXT-2+m_boxHeight)
       )
    {
        SetValue(2);
    }
    return FALSE;
}

bool wxSimpleCheckBox::OnKeyEvent ( wxKeyEvent& event )
{
    if ( event.GetEventType() == wxEVT_KEY_DOWN )
    {
        if ( event.GetKeyCode() == WXK_SPACE )
            SetValue(2);
    }
    return FALSE;
}

#endif // wxPG_USE_CUSTOM_CONTROLS

wxPGCtrlClass* wxPGCheckBoxEditor::CreateControls ( wxPropertyGrid* propgrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& size, wxPGCtrlClass** ) const
{
    wxPoint pt = pos;
    pt.x -= wxPG_XBEFOREWIDGET;
    wxSize sz = size;
    sz.x += wxPG_XBEFOREWIDGET;

#if !wxPG_USE_CUSTOM_CONTROLS
    wxSimpleCheckBox* cb = new wxSimpleCheckBox(propgrid,wxPG_SUBID1,pt,sz);
#else
    wxSimpleCheckBox* cb = new wxSimpleCheckBox(propgrid->GetCCManager(),wxPG_SUBID1,pt,sz);
#endif

#if !wxPG_USE_CUSTOM_CONTROLS
    cb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    cb->Connect ( wxPG_SUBID1, wxEVT_LEFT_DOWN,
            (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent, NULL, propgrid );

    cb->Connect ( wxPG_SUBID1, wxEVT_LEFT_DCLICK,
            (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent, NULL, propgrid );
#endif

    if ( property->GetChoiceInfo((wxPGChoiceInfo*)NULL) &&
         !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        cb->m_state = 1;

    // If mouse cursor was on the item, toggle the value now.
    if ( propgrid->GetInternalFlags() & wxPG_FL_ACTIVATION_BY_CLICK )
    {
        wxPoint pt = propgrid->ScreenToClient(::wxGetMousePosition());
        if ( pt.x <= (cb->GetPosition().x+wxPG_XBEFORETEXT-2+cb->m_boxHeight) )
        {
            cb->m_state++;

            if ( cb->m_state > 1 )
                cb->m_state = 0;

            property->ClearFlag(wxPG_PROP_UNSPECIFIED);
            property->SetValueFromInt(cb->m_state,0);
            propgrid->PropertyWasModified(property);
        }
    }

    return cb;
}

void wxPGCheckBoxEditor::DrawValue ( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const
{
    int state = 0;
    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
    {
        state = property->GetChoiceInfo((wxPGChoiceInfo*)NULL);
        if ( dc.GetFont().GetWeight() == wxBOLD ) state |= 2;
    }
    DrawSimpleCheckBox(dc,rect,dc.GetCharHeight(),state,dc.GetTextForeground());
}

void wxPGCheckBoxEditor::UpdateControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const
{
    wxASSERT ( ctrl );
    ((wxSimpleCheckBox*)ctrl)->m_state = property->GetChoiceInfo((wxPGChoiceInfo*)NULL);
    ctrl->Refresh();
}

bool wxPGCheckBoxEditor::OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property,
    wxPGCtrlClass* ctrl, wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_CHECKBOX_CLICKED )
    {
        if ( CopyValueFromControl( property, ctrl ) )
            return TRUE;

        propgrid->EditorsValueWasNotModified();

        wxPropertyGridState::ClearPropertyAndChildrenFlags(property,wxPG_PROP_UNSPECIFIED);
    }
    return FALSE;
}

bool wxPGCheckBoxEditor::CopyValueFromControl ( wxPGProperty* property, wxPGCtrlClass* ctrl ) const
{
    wxSimpleCheckBox* cb = (wxSimpleCheckBox*)ctrl;

    int index = cb->m_state;

    if ( index != property->GetChoiceInfo( (wxPGChoiceInfo*) NULL ) ||
         // Changing unspecified always causes event (returning
         // TRUE here should be enough to trigger it).
         property->IsFlagSet(wxPG_PROP_UNSPECIFIED)
       )
    {
        property->SetValueFromInt(index,0);
        return TRUE;
    }
    return FALSE;
}

void wxPGCheckBoxEditor::SetControlIntValue ( wxPGCtrlClass* ctrl, int value ) const
{
    if ( value != 0 ) value = 1;
    ((wxSimpleCheckBox*)ctrl)->m_state = value;
    ctrl->Refresh();
}

void wxPGCheckBoxEditor::SetValueToUnspecified ( wxPGCtrlClass* ctrl ) const
{
    ((wxSimpleCheckBox*)ctrl)->m_state = 0;
    ctrl->Refresh();
}

wxPGCheckBoxEditor::~wxPGCheckBoxEditor() { }

#endif // wxPG_INCLUDE_CHECKBOX

// -----------------------------------------------------------------------
// wxPGBrush
// -----------------------------------------------------------------------

//
// This class is a wxBrush derivative used in the background colour
// brush cache. It adds wxPG-type colour-in-long to the class.
//

class wxPGBrush : public wxBrush
{
public:
    wxPGBrush( const wxColour& colour );
    wxPGBrush();
    virtual ~wxPGBrush() { }
    void SetColour2 ( const wxColour& colour );
    inline long GetColourAsLong () const { return m_colAsLong; }
protected:
    long    m_colAsLong;
};

void wxPGBrush::SetColour2 ( const wxColour& colour )
{
    wxBrush::SetColour(colour);
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}

wxPGBrush::wxPGBrush() : wxBrush()
{
    m_colAsLong = 0;
}

wxPGBrush::wxPGBrush( const wxColour& colour ) : wxBrush(colour)
{
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}

// -----------------------------------------------------------------------
// wxPropertyGrid
// -----------------------------------------------------------------------

IMPLEMENT_CLASS(wxPropertyGrid, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxPropertyGrid, wxScrolledWindow)
#if wxPG_USE_CUSTOM_CONTROLS
  EVT_MOTION(wxPropertyGrid::OnMouseEvent)
  EVT_LEFT_DOWN(wxPropertyGrid::OnMouseEvent)
  EVT_LEFT_UP(wxPropertyGrid::OnMouseEvent)
  EVT_RIGHT_UP(wxPropertyGrid::OnMouseEvent)
  EVT_LEFT_DCLICK(wxPropertyGrid::OnMouseEvent)
  EVT_MOUSEWHEEL(wxPropertyGrid::OnMouseEvent)
#else
  EVT_MOTION(wxPropertyGrid::OnMouseMove)
  EVT_LEFT_DOWN(wxPropertyGrid::OnMouseClick)
  EVT_LEFT_UP(wxPropertyGrid::OnMouseUp)
  EVT_RIGHT_UP(wxPropertyGrid::OnMouseRightClick)
  EVT_LEFT_DCLICK(wxPropertyGrid::OnMouseClick)
  //EVT_MOUSEWHEEL(wxPropertyGrid::OnMouseClick)
#endif
  EVT_PAINT(wxPropertyGrid::OnPaint)
  EVT_SIZE(wxPropertyGrid::OnResize)
  EVT_KEY_DOWN(wxPropertyGrid::OnKey)
  EVT_CHAR(wxPropertyGrid::OnKey)
  EVT_ENTER_WINDOW(wxPropertyGrid::OnMouseEntry)
  EVT_LEAVE_WINDOW(wxPropertyGrid::OnMouseEntry)
#if !wxPG_USE_CUSTOM_CONTROLS
  EVT_MOUSE_CAPTURE_CHANGED(wxPropertyGrid::OnCaptureChange)
#endif
  EVT_SCROLLWIN(wxPropertyGrid::OnScrollEvent)
#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )
  EVT_NAVIGATION_KEY(wxPropertyGrid::OnNavigationKey)
#endif
  EVT_SET_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_KILL_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_SYS_COLOUR_CHANGED(wxPropertyGrid::OnSysColourChanged)
END_EVENT_TABLE()

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid()
    : wxScrolledWindow()
{
    Init1();
}

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid (wxWindow *parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxChar* name)
    :
    wxScrolledWindow()
{
    Init1();
    Create(parent,id,pos,size,style,name);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Create (wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxChar* name)
{

    if ( !(style&wxBORDER_MASK) )
        style |= wxSIMPLE_BORDER;

    style |= wxVSCROLL;

#ifdef __WXMSW__
    // This prevents crash under Win2K, but still
    // enables keyboard navigation
    if ( style & wxTAB_TRAVERSAL )
    {
        style &= ~(wxTAB_TRAVERSAL);
        style |= wxWANTS_CHARS;
    }
#else
    if ( style & wxTAB_TRAVERSAL )
        style |= wxWANTS_CHARS;
#endif

    wxScrolledWindow::Create(parent,id,pos,size,style,name);
    Init2();

    return TRUE;
}

// -----------------------------------------------------------------------

static void wxPGRegisterStandardPropertyClasses();

//
// Initialize values to defaults
//
void wxPropertyGrid::Init1()
{

    // Init common variables.
    if ( !wxPGGlobalVars )
        gs_spStaticVars.reset( new wxPGGlobalVarsClass );

    // Register type classes, if necessary.
    if ( wxPGGlobalVars->m_dictValueType.empty() )
        RegisterDefaultValues();

    // Register editor classes, if necessary.
    if ( wxPGGlobalVars->m_arrEditorClasses.empty() )
        RegisterDefaultEditors();

    // Register property classes, if necessary
    if ( wxPGGlobalVars->m_dictPropertyClassInfo.empty() )
        wxPGRegisterStandardPropertyClasses();

    m_iFlags = 0;

    m_pState = (wxPropertyGridState*) NULL;

    m_wndPrimary = m_wndSecondary = (wxPGCtrlClass*) NULL;

    m_selected = (wxPGProperty*) NULL;

    m_propHover = (wxPGProperty*) NULL;

    m_processingEvent = 0;

    m_dragStatus = 0;

    m_mouseSide = 16;

    m_editorFocused = 0;

    m_coloursCustomized = 0;

    m_frozen = 0;

#if wxPG_DOUBLE_BUFFER
    m_doubleBuffer = (wxBitmap*) NULL;
#endif

    m_clearThisMany = 0;

#ifndef wxPG_ICON_WIDTH
	m_expandbmp = NULL;
	m_collbmp = NULL;
	m_iconwidth = 11;
	m_iconheight = 11;
#else
    m_iconwidth = wxPG_ICON_WIDTH;
#endif

    m_prevVY = -1;

    m_calcVisHeight = 0;

    m_gutterwidth = wxPG_GUTTER_MIN;
    m_subgroup_extramargin = 10;

    m_lineHeight = 0;

    m_width = m_height = 0;

    m_bottomy = 0;

    m_splitterx = wxPG_DEFAULT_SPLITTERX;

#if !wxPG_HEAVY_GFX
    m_splitterpen.SetColour( *wxBLACK );
    m_splitterpen.SetWidth( 4 );

    m_splitterprevdrawnx = -1;
#endif

    //m_prevSelected = (wxPGCtrlClass*) NULL;

    /*
#ifdef __WXDEBUG__
    if ( sizeof(wxString) > sizeof(wxChar*) )
        wxLogDebug(wxT("WARNING: Sizeof wxString = %i"),(int)sizeof(wxString));
#endif
    */

}

// -----------------------------------------------------------------------

//
// Initialize after parent etc. set
//
void wxPropertyGrid::Init2()
{

    wxASSERT ( !(m_iFlags & wxPG_FL_INITIALIZED ) );

    // Now create state, if one didn't exist already
    // (wxPropertyGridManager might have created it for us).
    if ( !m_pState )
    {
        m_pState = new wxPropertyGridState();
        m_pState->m_pPropGrid = this;
        m_iFlags |= wxPG_FL_CREATEDSTATE;
    }

    if ( !(m_windowStyle & wxPG_SPLITTER_AUTO_CENTER) )
        m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    if ( m_windowStyle & wxPG_HIDE_CATEGORIES )
    {
        m_pState->InitNonCatMode ();

        FROM_STATE(m_properties) = FROM_STATE(m_abcArray);
    }

    GetClientSize(&m_width,&m_height);

#if !wxPG_HEAVY_GFX
    m_splitterpen.SetColour( *wxBLACK );
    m_splitterpen.SetWidth( 4 );
#endif

#ifndef wxPG_ICON_WIDTH
    // create two bitmap nodes for drawing
	m_expandbmp = new wxBitmap(expand_xpm);
	m_collbmp = new wxBitmap(collapse_xpm);

	// calculate average font height for bitmap centering

	m_iconwidth = m_expandbmp->GetWidth();
	m_iconheight = m_expandbmp->GetHeight();
#endif

    m_curcursor = wxCURSOR_ARROW;
    m_cursor_sizewe = new wxCursor ( wxCURSOR_SIZEWE );

    if ( !m_font.Ok() )
    {
        wxFont useFont = wxScrolledWindow::GetFont();

        useFont.SetWeight(wxFONTWEIGHT_LIGHT);
        //
        // This hack won't work - indentation stays the same.
        // (see method GenerateEditorTextCtrl for other solution)
    /*#if (wxMINOR_VERSION >= 7 || ( wxMINOR_VERSION == 6 && wxRELEASE_NUMBER >= 1 )) && defined(__WXMSW__)
        useFont.SetFaceName(wxT("MS Shell Dlg"));
    #endif*/

    #if wxMINOR_VERSION >= 6 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )
        wxScrolledWindow::SetOwnFont ( useFont );
    #else
        wxScrolledWindow::SetFont ( useFont );
    #endif
    }

    // Add base brush item.
    m_arrBgBrushes.Add((void*)new wxPGBrush());

    RegainColours();

	// set standard DC font

	// adjust bitmap icon y position so they are centered
    m_vspacing = wxPG_DEFAULT_VSPACING;
	CalculateFontAndBitmapStuff( wxPG_DEFAULT_VSPACING );

	// set virtual size to this window size
    wxSize wndsize = GetSize();
	SetVirtualSize(wndsize.GetWidth(), wndsize.GetWidth());

#if wxPG_USE_CUSTOM_CONTROLS
    // Initialize custom controls manager.
    m_ccManager.Create ( this );
#endif

#if wxMINOR_VERSION >= 6 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )
    // Set background style that indicates that we
    // do much custom drawing.
    SetBackgroundStyle ( wxBG_STYLE_CUSTOM );
#endif

    m_iFlags |= wxPG_FL_INITIALIZED;

}

// -----------------------------------------------------------------------

wxPropertyGrid::~wxPropertyGrid()
{

    size_t i;

    // This should do prevent things from going too badly wrong
    m_iFlags &= ~(wxPG_FL_INITIALIZED);

    END_MOUSE_CAPTURE

#ifdef __WXDEBUG__
    if ( IsEditorsValueModified() )
        ::wxMessageBox(wxT("Most recent change in property editor was lost!!!\n\n(please report this, along with circumstances to jmsalli79@hotmail.com)"),
                       wxT("wxPropertyGrid Debug Warning") );
#endif

    //ClearSelection();

    //if ( m_prevSelected )
    //    delete m_prevSelected;

#if wxPG_DOUBLE_BUFFER
    if ( m_doubleBuffer )
        delete m_doubleBuffer;
#endif

    m_selected = (wxPGProperty*) NULL;

#if wxPG_USE_CUSTOM_CONTROLS
    delete m_wndPrimary;
    delete m_wndSecondary;
#endif

    if ( m_iFlags & wxPG_FL_CREATEDSTATE )
        delete m_pState;

    delete m_cursor_sizewe;

#ifndef wxPG_ICON_WIDTH
	delete m_expandbmp;
	delete m_collbmp;
#endif

    // Delete cached brushes.
    for ( i=0; i<m_arrBgBrushes.GetCount(); i++ )
    {
        delete (wxPGBrush*)m_arrBgBrushes.Item(i);
    }

}

// -----------------------------------------------------------------------
// wxPropertyGrid overridden wxWindow methods
// -----------------------------------------------------------------------

void wxPropertyGrid::SetWindowStyleFlag( long style )
{
    long old_style = m_windowStyle;

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxASSERT( m_pState );

        if ( !(style & wxPG_HIDE_CATEGORIES) && (old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Enable categories
            EnableCategories ( TRUE );
        }
        else if ( (style & wxPG_HIDE_CATEGORIES) && !(old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Disable categories
            EnableCategories ( FALSE );
        }
        if ( !(old_style & wxPG_AUTO_SORT) && (style & wxPG_AUTO_SORT) )
        {
            //
            // Autosort enabled
            //
            if ( !m_frozen )
                PrepareAfterItemsAdded();
            else
                FROM_STATE(m_itemsAdded) = 1;
        }
    #if wxPG_SUPPORT_TOOLTIPS
        if ( !(old_style & wxPG_TOOLTIPS) && (style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips enabled
            //
            /*wxToolTip* tooltip = new wxToolTip ( wxEmptyString );
            SetToolTip ( tooltip );
            tooltip->SetDelay ( wxPG_TOOLTIP_DELAY );*/
        }
        else if ( (old_style & wxPG_TOOLTIPS) && !(style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips disabled
            //
            SetToolTip ( (wxToolTip*) NULL );
        }
    #endif
    }

    wxScrolledWindow::SetWindowStyleFlag ( style );

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxASSERT( m_pState );

        if ( (old_style & wxPG_HIDE_MARGIN) != (style & wxPG_HIDE_MARGIN) )
        {
            CalculateFontAndBitmapStuff( m_vspacing );
            //DoExpandAll(1);
            RedrawAllVisible();
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Freeze()
{
    m_frozen = 1;
    wxScrolledWindow::Freeze();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Thaw()
{
    m_frozen = 0;
    wxScrolledWindow::Thaw();
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    Refresh();
#endif
    // Force property re-selection
    if ( m_selected )
        SelectProperty(m_selected,false,true);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetExtraStyle ( long exStyle )
{
    wxScrolledWindow::SetExtraStyle ( exStyle /*& 0xFFF*/ );
    if ( exStyle & wxPG_EX_INIT_NOCAT )
        m_pState->InitNonCatMode ();
    else if ( exStyle & wxPG_EX_CLASSIC_SPACING )
    {
        CalculateFontAndBitmapStuff ( wxPG_DEFAULT_VSPACING );
        RedrawAllVisible();
    }
}

// -----------------------------------------------------------------------

// returns the best acceptable minimal size
wxSize wxPropertyGrid::DoGetBestSize() const
{

    /*
    //return wxSize( m_marginwidth + wxPG_DRAG_MARGIN, m_lineHeight+1 );
    unsigned int hei = 300;
    // TODO: Fix this thing.
    //if ( m_bottomy > hei ) hei = m_bottomy;
    //if ( m_bottomy < (unsigned int)m_lineHeight ) hei = m_lineHeight;
    return wxSize( wxPG_OPTIMAL_WIDTH, (int)hei );
    */
    //wxLogDebug(wxT("%i"),m_lineHeight+10);
    int hei = 15;
    if ( m_lineHeight > hei )
        hei = m_lineHeight;
    wxSize sz = wxSize( 60, hei+40 );
    //CacheBestSize(sz);
    return sz;
    //return wxSize( wxPG_OPTIMAL_WIDTH, 300 );
}

// -----------------------------------------------------------------------
// wxPropertyGrid Font and Colour Methods
// -----------------------------------------------------------------------

void wxPropertyGrid::CalculateFontAndBitmapStuff( int vspacing )
{
	int x = 0, y = 0;

    m_captionFont = GetFont();

	GetTextExtent(wxT("jG"), &x, &y, 0, 0, &m_captionFont);
    m_subgroup_extramargin = x + (x/2);
	m_fontHeight = y;

#if wxPG_USE_RENDERER_NATIVE
    m_iconwidth = wxPG_ICON_WIDTH;
#elif wxPG_ICON_WIDTH
    // scale icon
    m_iconwidth = (m_fontHeight * wxPG_ICON_WIDTH) / 13;
    if ( m_iconwidth < 5 ) m_iconwidth = 5;
    else if ( !(m_iconwidth & 0x01) ) m_iconwidth++; // must be odd

#endif

    m_gutterwidth = m_iconwidth / wxPG_GUTTER_DIV;
    if ( m_gutterwidth < wxPG_GUTTER_MIN )
        m_gutterwidth = wxPG_GUTTER_MIN;

    int vdiv = 6;
    if ( vspacing <= 1 ) vdiv = 12;
    else if ( vspacing >= 3 ) vdiv = 3;

    m_spacingy = m_fontHeight / vdiv;
    if ( m_spacingy < wxPG_YSPACING_MIN )
        m_spacingy = wxPG_YSPACING_MIN;

    m_marginwidth = 0;
    if ( !(m_windowStyle & wxPG_HIDE_MARGIN) )
        m_marginwidth = m_gutterwidth*2 + m_iconwidth;

    m_captionFont.SetWeight(wxBOLD);
	GetTextExtent(wxT("jG"), &x, &y, 0, 0, &m_captionFont);

    m_lineHeight = m_fontHeight+(2*m_spacingy)+1;

    // button spacing
    if ( GetExtraStyle() & wxPG_EX_CLASSIC_SPACING )
    {
        m_buttonSpacingY = m_spacingy;
    }
    else
    {
        m_buttonSpacingY = (m_lineHeight - m_iconheight) / 2;
        if ( m_buttonSpacingY < 0 ) m_buttonSpacingY = 0;
    }

#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )
    InvalidateBestSize();
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnSysColourChanged ( wxSysColourChangedEvent &WXUNUSED(event) )
{
    RegainColours();
    Refresh();
}

// -----------------------------------------------------------------------

#if !wxPG_NEW_COLOUR_SCHEME

void wxPropertyGrid::RegainColours ()
{
    wxColour def_bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_SCROLLBAR );
    // This is for 1.1 default scheme
    //wxColour def_bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW );

    if ( !(m_coloursCustomized & 0x01) )
        m_colMargin = def_bgcol;

    if ( !(m_coloursCustomized & 0x02) )
        m_colCapBack = def_bgcol;
        // This is for 1.1 default scheme
        //m_colCapBack = wxSystemSettings::GetColour ( wxSYS_COLOUR_SCROLLBAR );

    if ( !(m_coloursCustomized & 0x04) )
        m_colCapFore.Set ( 128, 128, 128 );

    if ( !(m_coloursCustomized & 0x08) )
    {
        wxColour bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW );
        m_colPropBack = bgcol;
        // Set the brush as well.
        ((wxPGBrush*)m_arrBgBrushes.Item(0))->SetColour2(bgcol);
    }

    if ( !(m_coloursCustomized & 0x10) )
        m_colPropFore = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOWTEXT );

    if ( !(m_coloursCustomized & 0x20) )
        m_colSelBack = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHT );

    if ( !(m_coloursCustomized & 0x40) )
        m_colSelFore = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHTTEXT );

    if ( !(m_coloursCustomized & 0x80) )
        m_colLine = wxSystemSettings::GetColour ( wxSYS_COLOUR_SCROLLBAR );
}

// -----------------------------------------------------------------------

#else // !wxPG_NEW_COLOUR_SCHEME

static wxColour wxPGAdjustColour(const wxColour& src, int ra,
                                 int ga = 1000, int ba = 1000,
                                 bool forceDifferent = false)
{
    if ( ga >= 1000 )
        ga = ra;
    if ( ba >= 1000 )
        ba = ra;
#ifdef __WXDEBUG__
    // Recursion guard (allow 2 max)
    static int isinside = 0;
    isinside++;
    wxASSERT_MSG( isinside < 3,
        wxT("wxPGAdjustColour should not be recursively called more than once"));
#endif
    wxColour dst;

    int r = src.Red();
    int g = src.Green();
    int b = src.Blue();
    int r2 = r + ra;
    if ( r2>255 ) r2 = 255;
    else if ( r2<0) r2 = 0;
    int g2 = g + ga;
    if ( g2>255 ) g2 = 255;
    else if ( g2<0) g2 = 0;
    int b2 = b + ba;
    if ( b2>255 ) b2 = 255;
    else if ( b2<0) b2 = 0;

    // Make sure they are somewhat different
    if ( forceDifferent && (abs((r+g+b)-(r2+g2+b2)) < abs(ra/2)) )
        dst = wxPGAdjustColour(src,-(ra*2));
    else
        dst = wxColour(r2,g2,b2);

#ifdef __WXDEBUG__
    // Recursion guard (allow 2 max)
    isinside--;
#endif
    return dst;
}


static int wxPGGetColAvg ( const wxColour& col )
{
    return (col.Red() + col.Green() + col.Blue()) / 3;
}


void wxPropertyGrid::RegainColours ()
{
    //wxColour def_bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_SCROLLBAR );
    wxColour def_bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW );

    if ( !(m_coloursCustomized & 0x02) )
    {
        wxColour col = wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE );

        // Make sure colour is dark enough
    #ifdef __WXGTK__
        int colDec = wxPGGetColAvg(col) - 230;
    #else
        int colDec = wxPGGetColAvg(col) - 200;
    #endif
        if ( colDec > 0 )
            //m_colCapBack = wxPGAdjustColour(col,-(colDec+8),-(colDec+9),-(colDec));
            m_colCapBack = wxPGAdjustColour(col,-colDec);
        else
            m_colCapBack = col;

    }

    if ( !(m_coloursCustomized & 0x01) )
        m_colMargin = m_colCapBack;

    if ( !(m_coloursCustomized & 0x04) )
    {
    #ifdef __WXGTK__
        int colDec = -90;
    #else
        int colDec = -72;
    #endif
        m_colCapFore = wxPGAdjustColour(m_colCapBack,colDec,5000,5000,true);
    }

    if ( !(m_coloursCustomized & 0x08) )
    {
        wxColour bgcol = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW );
        m_colPropBack = bgcol;
        // Set the brush as well.
        ((wxPGBrush*)m_arrBgBrushes.Item(0))->SetColour2(bgcol);
    }

    if ( !(m_coloursCustomized & 0x10) )
        m_colPropFore = wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOWTEXT );

    if ( !(m_coloursCustomized & 0x20) )
        m_colSelBack = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHT );

    if ( !(m_coloursCustomized & 0x40) )
        m_colSelFore = wxSystemSettings::GetColour ( wxSYS_COLOUR_HIGHLIGHTTEXT );

    if ( !(m_coloursCustomized & 0x80) )
        m_colLine = m_colCapBack;
}

#endif // wxPG_NEW_COLOUR_SCHEME

// -----------------------------------------------------------------------

void wxPropertyGrid::ResetColours()
{
    m_coloursCustomized = 0;

    RegainColours();

    Refresh();
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::SetFont ( const wxFont& font )
{

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::SetFont") );
#endif

    // Must disable active editor.
    if ( m_selected ) ClearSelection();

#if !defined(__WXMAC__)
    bool res = wxScrolledWindow::SetFont ( font );
    if ( res )
    {

        CalculateFontAndBitmapStuff( m_vspacing );

        if ( m_pState )
        {
            // Recalculate caption text extents.
            // TODO: This should also be done to other pages of manager
            //   (so add wxPropertyGridManager::SetFont), but since font
            //   is usually set before categories are added, this is
            //   quite low priority.
            size_t i;
            for ( i=0;i<FROM_STATE(m_regularArray).GetCount();i++)
            {
                wxPGProperty* p = FROM_STATE(m_regularArray).Item(i);

                if ( p->GetParentingType() > 0 )
                    ((wxPropertyCategoryClass*)p)->CalculateTextExtent(this,m_captionFont);
            }

            CalculateYs(NULL,-1);
        }

    #if wxPG_USE_CUSTOM_CONTROLS
        m_ccManager.SetFont ( font );
    #endif

        Refresh();
    }
#else
    // ** wxMAC Only **
    // TODO: Remove after SetFont crash fixed.
    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxLogDebug(wxT("WARNING: propgrid.cpp: wxPropertyGrid::SetFont has been disabled on wxMac since there has been crash reported in it. If you are willing to debug the cause, replace line '#if !defined(__WXMAC__)' with line '#if 1' in wxPropertyGrid::SetFont."));
    }
    bool res = false;
#endif
    return res;
}

// -----------------------------------------------------------------------

/*bool wxPropertyGrid::SetBackgroundColour ( const wxColour& col )
{
    bool res = wxScrolledWindow::SetBackgroundColour ( col );
    if ( res )
    {
        m_coloursCustomized |= 0x01;
        // Emulate SetOwnBackgroundColour.
    #if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 3 )
        // <= 2.5.2
        m_hasBgCol = FALSE;
    #else
        // >= 2.5.3
        m_inheritBgCol = FALSE;
    #endif
        Refresh();
        return TRUE;
    }
    return FALSE;
}*/

// -----------------------------------------------------------------------

void wxPropertyGrid::SetLineColour ( const wxColour& col )
{
    m_colLine = col;
    m_coloursCustomized |= 0x80;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetMarginColour ( const wxColour& col )
{
    m_colMargin = col;
    m_coloursCustomized |= 0x01;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellBackgroundColour ( const wxColour& col )
{
    wxColour old_colpropback = m_colPropBack;

    m_colPropBack = col;
    m_coloursCustomized |= 0x08;

    // Set the brush as well.
    ((wxPGBrush*)m_arrBgBrushes.Item(0))->SetColour2(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellTextColour ( const wxColour& col )
{
    m_colPropFore = col;
    m_coloursCustomized |= 0x10;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionBackground ( const wxColour& col )
{
    m_colSelBack = col;
    m_coloursCustomized |= 0x20;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionForeground ( const wxColour& col )
{
    m_colSelFore = col;
    m_coloursCustomized |= 0x40;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionBackgroundColour ( const wxColour& col )
{
    m_colCapBack = col;
    m_coloursCustomized |= 0x02;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionForegroundColour ( const wxColour& col )
{
    m_colCapFore = col;
    m_coloursCustomized |= 0x04;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetColourIndex ( wxPGProperty* p, int index )
{
    unsigned char ind = index;

    p->m_bgColIndex = ind;

    if ( p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
        size_t i;
        for ( i=0; i<pwc->GetCount(); i++ )
            SetColourIndex(pwc->Item(i),index);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyColour ( wxPGId id, const wxColour& colour )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    size_t i;
    int col_ind = -1;

    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    long colaslong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());

    // As it is most likely that the previous colour is used, start comparison
    // from the end.
    for ( i=(m_arrBgBrushes.GetCount()-1); i>0; i-- )
    {
        if ( ((wxPGBrush*)m_arrBgBrushes.Item(i))->GetColourAsLong() == colaslong )
        {
            col_ind = i;
            break;
        }
    }

    if ( col_ind < 0 )
    {
        col_ind = m_arrBgBrushes.GetCount();
        wxCHECK_RET ( col_ind < 256, wxT("wxPropertyGrid: Warning - Only 255 different property background colours allowed.") );
        m_arrBgBrushes.Add( (void*)new wxPGBrush(colour) );
    }

    // Set indexes
    SetColourIndex(p,col_ind);

    // If this was on a visible grid, then draw it.
    if ( p->GetParentState() == m_pState )
        DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

wxColour wxPropertyGrid::GetPropertyColour ( wxPGId id ) const
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    return ((wxPGBrush*)m_arrBgBrushes.Item(p->m_bgColIndex))->GetColour();
}

// -----------------------------------------------------------------------
// wxPropertyGrid property adding and removal
// -----------------------------------------------------------------------

wxPGId wxPropertyGrid::Append ( wxPGProperty* property )
{
    return FROM_STATE(Append(property));
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGrid::Insert ( wxPGProperty* priorthis, wxPGProperty* property )
{
    wxASSERT ( priorthis );
    return FROM_STATE(Insert (priorthis->GetParent(), priorthis->GetArrIndex(), property ));
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Delete ( wxPGProperty* item )
{
    if ( m_selected )
    {
        ClearSelection();
    }

    m_pState->Delete ( item );

    if ( !m_frozen )
    {
        // This should be enough to resolve even the worst
        // graphics glitch imagined.
        Update();
        Refresh();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::PrepareAfterItemsAdded()
{

    if ( !FROM_STATE(m_itemsAdded) ) return;

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("PrepareAfterItemsAdded( in thread 0x%lX )"),
        (unsigned long)wxThread::GetCurrentId());
#endif

    FROM_STATE(m_itemsAdded) = 0;

    if ( m_windowStyle & wxPG_AUTO_SORT )
    {
        Sort ();
    }
    else
    {
        if ( m_bottomy < 1 )
        {
            // Whole page update
            CalculateYs( NULL, -1 );
        }
        else
        {
            RecalculateVirtualSize();
            // Update visibles array (maybe not necessary here, but just in case)
            CalculateVisibles ( -1, TRUE );
        }
    }

}

// -----------------------------------------------------------------------
// wxPropertyGrid property value setting and getting
// -----------------------------------------------------------------------

void wxPGGetFailed ( const wxPGProperty* p, const wxChar* typestr )
{
    wxPGTypeOperationFailed(p,typestr,wxT("Get"));
}

// -----------------------------------------------------------------------

void wxPGTypeOperationFailed ( const wxPGProperty* p, const wxChar* typestr,
    const wxChar* op )
{
    wxASSERT ( p != NULL );
    wxLogError( _("Type operation \"%s\" failed: Property labeled \"%s\" is of type \"%s\", NOT \"%s\"."),
        op,p->GetLabel().c_str(),p->GetValueType()->GetCustomTypeName(),typestr );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValue ( wxPGId id, const wxPGValueType* typeclass, wxPGVariant value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( m_pState->SetPropertyValue(p,typeclass,value) )
        DrawItemAndChildren ( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValue ( wxPGId id, const wxChar* typestring, wxPGVariant value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( m_pState->SetPropertyValue(p,typestring,value) )
        DrawItemAndChildren ( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValue ( wxPGId id, const wxString& value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( m_pState->SetPropertyValue(p,value) )
        DrawItemAndChildren ( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValue ( wxPGId id, wxObject* value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( m_pState->SetPropertyValue(p,value) )
        DrawItemAndChildren ( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValue ( wxPGId id, wxVariant& value )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( m_pState->SetPropertyValue(p,value) )
        DrawItemAndChildren ( p );
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::ClearPropertyValue ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( m_pState->ClearPropertyValue(p) )
    {
        DrawItemAndChildren ( p );
        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyValueUnspecified ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    m_pState->SetPropertyValueUnspecified(p);
    DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------
// wxPropertyGrid miscellaneous GetPropertyXXX methods
// -----------------------------------------------------------------------

wxPGId wxPropertyGrid::DoGetPropertyByName ( wxPGNameStr name ) const
{
    return m_pState->BaseGetPropertyByName(name);
}

// -----------------------------------------------------------------------

wxPropertyCategoryClass* wxPropertyGrid::GetPropertyCategory ( wxPGProperty* p )
{
    wxPGProperty* parent = (wxPGPropertyWithChildren*)p;
    wxPGProperty* grandparent = (wxPGProperty*)parent->GetParent();
    do
    {
        parent = grandparent;
        grandparent = (wxPGProperty*)parent->GetParent();
        if ( parent->GetParentingType() > 0 && grandparent )
            return (wxPropertyCategoryClass*)parent;
    } while ( grandparent );
    return (wxPropertyCategoryClass*) NULL;
}

// -----------------------------------------------------------------------
// wxPropertyGrid property operations
// -----------------------------------------------------------------------

bool wxPropertyGrid::EnableProperty( wxPGId id, bool enable )
{
    wxPGProperty* p = wxPGIdToPtr( id );
    if ( p )
    {
        if ( enable )
        {
            if ( !(p->m_flags & wxPG_PROP_DISABLED) )
                return FALSE;

            // If active, Set active Editor.
            if ( p == m_selected )
                SelectProperty( p, FALSE, TRUE );
        }
        else
        {
            if ( p->m_flags & wxPG_PROP_DISABLED )
                return FALSE;

            // If active, Disable as active Editor.
            if ( p == m_selected )
                SelectProperty( p, FALSE, TRUE );
        }

        m_pState->EnableProperty(p,enable);

        wxPG_CLIENT_DC_INIT_R(TRUE)

        DrawItemAndChildren ( dc, p );

        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::LimitPropertyEditing ( wxPGId id, bool limit )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    m_pState->LimitPropertyEditing(p,limit);
    if ( p == m_selected )
        SelectProperty ( p, FALSE, TRUE );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyLabel( wxPGProperty* p,
    const wxString& newproplabel )
{
    p->m_label = newproplabel;
    if ( m_windowStyle & wxPG_AUTO_SORT )
    {
        Sort(p->GetParent());
        Refresh();
    }
    else
        DrawItem ( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoSetPropertyName ( wxPGProperty* p,
    const wxString& newname )
{

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::SetPropertyName ( %s -> %s )"),
        p->GetName().c_str(), newname.c_str() );
#endif

    if ( p->GetName().Len() ) FROM_STATE(m_dictName).erase ( wxPGNameConv(p->GetName()) );
    if ( newname.Len() ) FROM_STATE(m_dictName)[newname] = (void*) p;

    p->DoSetName(newname);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::EnsureVisible ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    if ( !p ) return FALSE;

    Update();

    bool changed = FALSE;

    //unsigned char old_frozen = m_frozen;
    //m_frozen = 1;

    // Is it inside collapsed section?
    if ( p->m_y < 0 )
    {
        // expand parents
        wxPGProperty* parent = p->GetParent();
        wxPGProperty* grandparent = parent->GetParent();

        if ( grandparent && grandparent != FROM_STATE(m_properties) )
            Expand ( grandparent );

        Expand ( parent );
        changed = TRUE;
    }

    // Need to scroll?
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;

    if ( p->m_y < vy )
    {
        Scroll (vx, p->m_y/wxPG_PIXELS_PER_UNIT );
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = TRUE;
    }
    else if ( (p->m_y+m_lineHeight) > (vy+m_height) )
    {
        Scroll (vx, (p->m_y-m_height+(m_lineHeight*2))/wxPG_PIXELS_PER_UNIT );
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = TRUE;
    }

    if ( changed )
        DrawItems ( p, p );

    //m_frozen = old_frozen;

    return changed;
}

// -----------------------------------------------------------------------
// wxPropertyGrid helper methods called by properties
// -----------------------------------------------------------------------

// Control font changer helper.
void wxPropertyGrid::SetCurControlBoldFont()
{
    wxASSERT( m_wndPrimary );

#ifdef __WXMSW__
    // See below in GenerateEditorTextCtrl for reason
    // why this code is needed.
    if ( wxStrcmp(m_wndPrimary->GetClassInfo()->GetClassName(),wxT("wxTextCtrl")) == 0 )
    {
        wxString faceName = m_captionFont.GetFaceName();
        int textCtrlXAdjust = 0;

        if ( (faceName == wxT("MS Shell Dlg 2") ||
              faceName == wxT("Tahoma")) &&
              m_font.GetWeight() != wxFONTWEIGHT_BOLD )
        {
            textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;
        }

        wxRect r = m_wndPrimary->GetRect();
        r.x += textCtrlXAdjust;
        r.width -= textCtrlXAdjust;
        m_ctrlXAdjust += textCtrlXAdjust;
        m_wndPrimary->SetSize(r);
    }
#endif

#if wxPG_USE_CUSTOM_CONTROLS
    wxClientDC dc(this);
#endif
    m_wndPrimary->SetFont( m_captionFont wxPG_SETFONT_ADDENDUM );
}

// -----------------------------------------------------------------------

wxPGCtrlClass* wxPropertyGrid::GenerateEditorTextCtrl( const wxPoint& pos,
    const wxSize& sz, const wxString& value, wxPGCtrlClass* secondary,
    const wxSize&)
{

#if wxPG_USE_CUSTOM_CONTROLS

    wxPoint p(pos);
    wxSize s(sz);

    // Take button into acccount
    if ( secondary )
        s.x -= secondary->GetSize().x;

    wxCCustomTextCtrl* ed = new wxCCustomTextCtrl ( &m_ccManager, wxPG_SUBID1, value,
        p, s, wxTE_PROCESS_ENTER );

#else // wxPG_USE_CUSTOM_CONTROLS

    int tcFlags = wxTE_PROCESS_ENTER|wxWANTS_CHARS;

    // STUPID HACK: wxTextCtrl has different indentation with different
    //   fonts, so this is to solve most common case (ie. using MS Shell Dlg 2
    //   or Tahoma - which are usually the same).
#ifdef __WXMSW__
    wxString faceName = m_font.GetFaceName();
    int textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;
    if ( (faceName == wxT("MS Shell Dlg 2") ||
          faceName == wxT("Tahoma")) &&
         m_font.GetWeight() != wxFONTWEIGHT_BOLD )
        textCtrlXAdjust = 0;
#else
    const int textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;
#endif

    wxPoint p(pos.x+textCtrlXAdjust,pos.y);
    wxSize s(sz.x-textCtrlXAdjust,sz.y);

    // Take button into acccount
    if ( secondary )
    {
        s.x -= (secondary->GetSize().x + wxPG_TEXTCTRL_AND_BUTTON_SPACING);
        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
    }

#if wxPG_NAT_TEXTCTRL_BORDER_ANY

    // Create clipper window
    wxPGClipperWindow* wnd = new wxPGClipperWindow();
#if defined(__WXMSW__)
    wnd->Hide();
#endif
    wnd->Create(this,wxPG_SUBID1,p,s);

    // This generates rect of the control inside the clipper window
    wnd->GetControlRect(wxPG_NAT_TEXTCTRL_BORDER_X,wxPG_NAT_TEXTCTRL_BORDER_Y,p,s);

    wxWindow* ctrlParent = wnd;

#else

    wxWindow* ctrlParent = this;
    tcFlags |= wxNO_BORDER;

#endif

    wxCCustomTextCtrl* tc = new wxCCustomTextCtrl();
#if defined(__WXMSW__) && !wxPG_NAT_TEXTCTRL_BORDER_ANY
    tc->Hide();
#endif
    tc->Create(ctrlParent,wxPG_SUBID1,value,p,/*wxDefaultSize*/s,tcFlags);

#if wxPG_NAT_TEXTCTRL_BORDER_ANY
    wxWindow* ed = wnd;
    wnd->SetControl(tc);
#else
    wxWindow* ed = tc;
#endif

    Connect ( wxPG_SUBID1, wxEVT_COMMAND_TEXT_UPDATED,
            (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent );
    Connect ( wxPG_SUBID1, wxEVT_COMMAND_TEXT_ENTER,
            (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent );

    // Center the control vertically
    wxRect finalPos = ed->GetRect();
    int y_adj = (m_lineHeight - finalPos.height)/2 + wxPG_TEXTCTRLYADJUST;

    // Prevent over-sized control
    int sz_dec = (y_adj + finalPos.height) - m_lineHeight;
    if ( sz_dec < 0 ) sz_dec = 0;

    finalPos.y += y_adj;
    finalPos.height -= (y_adj+sz_dec);
    /*wxLogDebug(wxT("m_spacingY = %i, sz_dec = %i, finalPos(%i,%i)"),
        m_spacingy,sz_dec,y_adj,finalPos.height);*/

    ed->SetSize(finalPos);

#ifdef __WXMSW__
    ed->Show();
    if ( secondary )
        secondary->Show();
#endif

#endif // !wxPG_USE_CUSTOM_CONTROLS

    return (wxPGCtrlClass*) ed;
}

// -----------------------------------------------------------------------

wxPGCtrlClass* wxPropertyGrid::GenerateEditorButton( const wxPoint& pos, const wxSize& sz )
{
#if wxPG_USE_CUSTOM_CONTROLS

    wxPoint p(pos.x+sz.x-sz.y+wxPG_BUTTON_SIZEDEC,pos.y+wxPG_BUTTON_SIZEDEC);
    wxSize s(sz.y-(wxPG_BUTTON_SIZEDEC*2),sz.y-(wxPG_BUTTON_SIZEDEC*2));

    wxCCustomButton* but = new wxCCustomButton(&m_ccManager,wxPG_SUBID2,wxT("..."),p,s);
    but->SetFont( m_captionFont, NULL );

#else // wxPG_USE_CUSTOM_CONTROLS
    wxSize s(sz.y-(wxPG_BUTTON_SIZEDEC*2)+(wxPG_NAT_BUTTON_BORDER_Y*2),
        sz.y-(wxPG_BUTTON_SIZEDEC*2)+(wxPG_NAT_BUTTON_BORDER_Y*2));
    wxPoint p(pos.x+sz.x-s.y,
        pos.y+wxPG_BUTTON_SIZEDEC-wxPG_NAT_BUTTON_BORDER_Y);

    //wxLogDebug(wxT("Button at y=%i"),(int)p.y);

    wxButton* but = new wxButton();
#ifdef __WXMSW__
    but->Hide();
#endif
    but->Create(this,wxPG_SUBID2,wxT("..."),p,s,0);

    but->SetFont( m_captionFont );

    //but->SetBackgroundColour ( m_colPropBack );
    Connect(wxPG_SUBID2, wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction)&wxPropertyGrid::OnCustomEditorEvent );
#endif // !wxPG_USE_CUSTOM_CONTROLS
    return but;
}

// -----------------------------------------------------------------------

wxPGCtrlClass* wxPropertyGrid::GenerateEditorTextCtrlAndButton( const wxPoint& pos,
    const wxSize& sz, wxPGCtrlClass** psecondary, int limited_editing,
    wxPGProperty* property )
{
    wxCCustomButton* but = (wxCCustomButton*)GenerateEditorButton(pos,sz);
    *psecondary = (wxPGCtrlClass*)but;

    if ( limited_editing )
    {
    #ifdef __WXMSW__
        // There is button Show in GenerateEditorTextCtrl as well
        but->Show();
    #endif
        return (wxPGCtrlClass*) NULL;
    }

    wxString text;

    if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        text = property->GetDisplayedString();

    return GenerateEditorTextCtrl(pos,sz,text,but);
}

// -----------------------------------------------------------------------

wxPoint wxPropertyGrid::GetGoodEditorDialogPosition ( wxPGProperty* p,
    const wxSize& sz )
{
    int x = m_splitterx;
    int y = p->m_y;

    wxASSERT ( y >= 0 );
    wxASSERT ( y < (int)m_bottomy );

    ImprovedClientToScreen ( &x, &y );

    int sw = wxSystemSettings::GetMetric ( ::wxSYS_SCREEN_X );
    int sh = wxSystemSettings::GetMetric ( ::wxSYS_SCREEN_Y );

    int new_x;
    int new_y;

    if ( x > (sw/2) )
        // left
        new_x = x + (m_width-m_splitterx) - sz.x;
    else
        // right
        new_x = x;

    if ( y > (sh/2) )
        // above
        new_y = y - sz.y;
    else
        // below
        new_y = y + m_lineHeight;

    return wxPoint(new_x,new_y);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SLAlloc ( unsigned int itemcount, const wxChar** items )
{
    wxArrayString& sl = m_sl;
    unsigned int i;
    unsigned int sl_oldcount = sl.GetCount();
    if ( sl_oldcount > itemcount ) sl_oldcount = itemcount;
#if wxUSE_INTL
    if ( !wxPGGlobalVars->m_autoGetTranslation )
    {
#endif
        for ( i=0; i<sl_oldcount; i++ )
            sl.Item(i) = items[i];
        for ( i=sl_oldcount; i<itemcount; i++ )
            sl.Add ( items[i] );
#if wxUSE_INTL
    }
    else
    {
        for ( i=0; i<sl_oldcount; i++ )
            sl.Item(i) = ::wxGetTranslation ( items[i] );
        for ( i=sl_oldcount; i<itemcount; i++ )
            sl.Add ( ::wxGetTranslation ( items[i] ) );
    }
#endif
}

wxString& wxPropertyGrid::ExpandEscapeSequences ( wxString& dst_str, wxString& src_str )
{
    if ( src_str.length() < 1 )
        return src_str;

    const wxChar* src = src_str.c_str();
    wxChar* dst;
    wxStringBuffer strbuf(dst_str,src_str.length());
    dst = strbuf;
    //wxChar*  dst_start = strbuf;
    bool prev_is_slash = FALSE;

    wxChar a = *src;
    while ( a )
    {
        src++;

        if ( a != wxT('\\') )
        {
            if ( !prev_is_slash )
            {
                *dst = a;
            }
            else
            {
                if ( a == wxT('n') )
                {
            #ifdef __WXMSW__
                    *dst = 13;
                    dst++;
                    *dst = 10;
            #else
                    *dst = 10;
            #endif
                }
                else if ( a == wxT('t') )
                    *dst = 9;
                else
                    *dst = a;
            }
            dst++;
            prev_is_slash = FALSE;
        }
        else
        {
            if ( prev_is_slash )
            {
                *dst = wxT('\\');
                dst++;
                prev_is_slash = FALSE;
            }
            else
            {
                prev_is_slash = TRUE;
            }
        }

        a = *src;
    }
    *dst = 0;
    return dst_str;

    /*
    const wxChar* src = src_str.c_str();
    wxChar* dst_start;
    wxChar* dst;
    dst = dst_start = dst_str.GetWriteBuf(src_str.length());
    bool prev_is_slash = FALSE;

    wxChar a = *src;
    while ( a )
    {
        src++;

        if ( a != wxT('\\') )
        {
            if ( !prev_is_slash )
            {
                *dst = a;
            }
            else
            {
                if ( a == wxT('n') )
                {
            #ifdef __WXMSW__
                    *dst = 13;
                    dst++;
                    *dst = 10;
            #else
                    *dst = 10;
            #endif
                }
                else if ( a == wxT('t') )
                    *dst = 9;
                else
                    *dst = a;
            }
            dst++;
            prev_is_slash = FALSE;
        }
        else
        {
            if ( prev_is_slash )
            {
                *dst = wxT('\\');
                dst++;
                prev_is_slash = FALSE;
            }
            else
            {
                prev_is_slash = TRUE;
            }
        }

        a = *src;
    }
    *dst = 0;
    dst_str.UngetWriteBuf(dst-dst_start);
    return dst_str;
    */
}

// -----------------------------------------------------------------------

wxString& wxPropertyGrid::CreateEscapeSequences ( wxString& dst_str, wxString& src_str )
{
    if ( src_str.length() < 1 )
        return src_str;

    const wxChar* src = src_str.c_str();
    wxChar* dst;
    wxStringBuffer strbuf(dst_str,src_str.length()*2);
    dst = strbuf;
    //wxChar* dst_start = strbuf;

    wxChar a = *src;
    while ( a )
    {
        src++;

        if ( a >= wxT(' ') || (char)a < 0 )
        {
            // This surely is not something that requires an escape sequence.
            *dst = a;
            dst++;
        }
        else
        {
            *dst = wxT('\\');
            dst++;

            // This might need...
            if ( a == 13 && *src == 10 )
            {
                // DOS style line end.
                *dst = wxT('n');
                src++;
            }
            else if ( a == 10 )
                // UNIX style line end.
                *dst = wxT('n');
            else if ( a == 9 )
                // Tab.
                *dst = wxT('t');
            else
            {
                wxLogDebug(wxT("WARNING: Could not create escape sequence for character #%i"),(int)a);
                dst-=2; // Compensate.
            }
            dst++;
        }

        a = *src;
    }
    *dst = 0;
    return dst_str;

    /*
    const wxChar* src = src_str.c_str();
    wxChar* dst_start;
    wxChar* dst;
    dst = dst_start = dst_str.GetWriteBuf( (src_str.length()*2) );

    wxChar a = *src;
    while ( a )
    {
        src++;

        if ( a >= wxT(' ') )
        {
            // This surely is not something that requires an escape sequence.
            *dst = a;
            dst++;
        }
        else
        {
            *dst = wxT('\\');
            dst++;

            // This might need...
            if ( a == 13 && *src == 10 )
            {
                // DOS style line end.
                *dst = wxT('n');
                src++;
            }
            else if ( a == 10 )
                // UNIX style line end.
                *dst = wxT('n');
            else if ( a == 9 )
                // Tab.
                *dst = wxT('t');
            else
            {
                wxLogDebug(wxT("WARNING: Could not create escape sequence for character #%i"),(int)a);
                dst-=2; // Compensate.
            }
            dst++;
        }

        a = *src;
    }
    *dst = 0;
    dst_str.UngetWriteBuf(dst-dst_start);
    return dst_str;
    */
}

// -----------------------------------------------------------------------
// Item iteration macros
// -----------------------------------------------------------------------

#define II_INVALID_I    0x00FFFFFF

#define ITEM_ITERATION_VARIABLES \
    wxPGPropertyWithChildren* parent; \
    unsigned int i; \
    unsigned int i_max;

#define ITEM_ITERATION_DCAE_VARIABLES \
    wxPGPropertyWithChildren* parent; \
    unsigned int i; \
    unsigned int i_max;

#define ITEM_ITERATION_INIT_FROM_THE_TOP \
    parent = FROM_STATE(m_properties); \
    i = 0;

#define ITEM_ITERATION_INIT(startparent,startindex) \
    parent = startparent; \
    i = (unsigned int)startindex; \
    if ( parent == (wxPGPropertyWithChildren*) NULL ) \
    { \
        parent = FROM_STATE(m_properties); \
        i = 0; \
    }

#define ITEM_ITERATION_LOOP_BEGIN \
    unsigned char parent_expanded; \
    do \
    { \
        parent_expanded = (unsigned char)parent->m_expanded; \
        if ( parent->m_parent && !parent->m_parent->m_expanded ) \
            parent_expanded = 0; \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            int parenting = p->GetParentingType();

#define ITEM_ITERATION_LOOP_END \
            if ( parenting ) \
            { \
                i = 0; \
                parent = (wxPGPropertyWithChildren*)p; \
                if ( parent_expanded ) \
                    parent_expanded = (unsigned char)parent->m_expanded; \
                else \
                    parent_expanded = 0; \
                i_max = parent->GetCount(); \
            } \
            else \
                i++; \
        } \
        i = parent->m_arrIndex + 1; \
        parent = parent->m_parent; \
    } \
    while ( parent != NULL );

// DCAE = Don't care about parent_expanded (this is the least space hungry method).
#define ITEM_ITERATION_DCAE_LOOP_BEGIN \
    do \
    { \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            int parenting = p->GetParentingType();

#define ITEM_ITERATION_DCAE_LOOP_END \
            if ( parenting ) \
            { \
                i = 0; \
                parent = (wxPGPropertyWithChildren*)p; \
                i_max = parent->GetCount(); \
            } \
            else \
                i++; \
        } \
        i = parent->m_arrIndex + 1; \
        parent = parent->m_parent; \
    } \
    while ( parent != NULL );

// DCAE_ISP = Don't care about parent_expanded, Ignore sub-properties.
// Note that this treats fixed sub-properties same as sub-properties
// of wxParentProperty. Mode conversion requires this behaviour.
#define ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN \
    do \
    { \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            int parenting = p->GetParentingType();

#define ITEM_ITERATION_DCAE_ISP_LOOP_END \
            if ( parenting > 0 ) \
            { \
                i = 0; \
                parent = (wxPGPropertyWithChildren*)p; \
                i_max = parent->GetCount(); \
            } \
            else \
                i++; \
        } \
        i = parent->m_arrIndex + 1; \
        parent = parent->m_parent; \
    } \
    while ( parent != (wxPGPropertyWithChildren*) NULL );

//        if ( parent->m_parent && !parent->m_parent->m_expanded )
//            parent_expanded = 0;

//
//        wxLogDebug(wxT("parent \"%s\" childcount: %i, expanded: %i"),
//            parent->GetLabel().c_str(),parent->GetCount(),(int)parent->m_expanded);
//

// VO = Visible only (including those outside the scrolled section).
#define ITEM_ITERATION_VO_LOOP_BEGIN \
    if ( (parent == FROM_STATE(m_properties) || parent->m_y >= 0) && parent->m_expanded ) \
    { \
    do \
    { \
        i_max = parent->GetCount(); \
        while ( i < i_max ) \
        {  \
            wxPGProperty* p = parent->Item(i); \
            if ( p->m_y >= 0 ) \
            { \
                int parenting = p->GetParentingType();

#define ITEM_ITERATION_VO_LOOP_END \
                if ( parenting && ((wxPGPropertyWithChildren*)p)->m_expanded ) \
                { \
                    parent = (wxPGPropertyWithChildren*)p; \
                    i = 0; \
                    break; \
                } \
            } \
            i++; \
        } \
        if ( i >= i_max ) \
        { \
            i = parent->m_arrIndex + 1; \
            parent = parent->m_parent; \
        } \
    } \
    while ( parent != (wxPGPropertyWithChildren*) NULL ); \
    }

// -----------------------------------------------------------------------
// wxPropertyGrid visibility related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::CalculateYs ( wxPGPropertyWithChildren* startparent,
    int startindex )
{
    // Selection must be temporarily cleared during y-recalc
    wxPGProperty* prevSelected = m_selected;
    if ( prevSelected )
        ClearSelection();

    ITEM_ITERATION_VARIABLES

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("CalculateYs(startsfrom: %s[%i] ) "),
        startparent?startparent->m_label.c_str():wxT("NULL"),
            startindex);
#endif

    ITEM_ITERATION_INIT(startparent,startindex)

    wxASSERT ( !m_frozen );

    int cury = 0;
    int lh = m_lineHeight;

    if ( startparent != NULL )
        cury = parent->Item(i)->m_y;

    wxASSERT_MSG ( cury >= 0, wxT("CalculateYs first item was not visible!!!") );

    long hide_state = m_iFlags & wxPG_FL_HIDE_STATE;
    bool inside_hidden_part = FALSE;
    //parent_expanded = (unsigned char)parent->m_expanded;
    wxPGPropertyWithChildren* nearest_expanded = (wxPGPropertyWithChildren*) NULL;

    // Find first visible and expanded parent.
    while ( !parent->IsExpanded() ||
            ( (parent->m_flags & wxPG_PROP_HIDEABLE) && hide_state )
          )
    {
        parent = parent->GetParent();
        i = 0;
    }

    wxASSERT ( parent );

    //parent = nearest_expanded;

    do
    {
        i_max = parent->GetCount();

        if ( !inside_hidden_part )
        {
            while ( i < i_max )
            {
                wxPGProperty* p = parent->Item(i);

                int parenting = p->GetParentingType();
                if ( !(p->m_flags & wxPG_PROP_HIDEABLE) || (!hide_state) )
                {
                    //wxLogDebug(wxT("Visible: \"%s\""),p->GetLabel().c_str());
                    // item is visible (all parents are expanded, non-hideable or not in hide state)
                    p->m_y = (int)cury;
                    cury += lh;

                #ifdef __WXDEBUG__
                    //if ( i==0 && parent->m_parent )
                        //wxLogDebug("  expanded=%i, arrIndex=%i",(int)parent->m_expanded,(int)parent->m_arrIndex);
                    //wxLogDebug("%s[%i]: %s (y=%i)",parent->m_label.c_str(),i,p->m_label.c_str(),(int)p->m_y);
                #endif

                }
                else
                {
                    //wxLogDebug(wxT("Hidden: \"%s\""),p->GetLabel().c_str());
                    p->m_y = -1;
                }

                if ( parenting )
                {
                    wxPGPropertyWithChildren* p2 = (wxPGPropertyWithChildren*)p;

                    if ( !p2->m_expanded ||
                         ( (p2->m_flags & wxPG_PROP_HIDEABLE) && hide_state )
                       )
                    {
                       inside_hidden_part = TRUE;
                       nearest_expanded = parent;
                    }

                    parent = p2;
                    i = 0;

                    break;
                }

                i++;
            }
        }
        else
        {
            while ( i < i_max )
            {
                wxPGProperty* p = parent->Item(i);
                //wxLogDebug(wxT("Hidden: \"%s\""),p->GetLabel().c_str());
                int parenting = p->GetParentingType();
                p->m_y = -1;
                if ( parenting )
                {
                    parent = (wxPGPropertyWithChildren*)p;
                    i = 0;
                    break;
                }
                i++;
            }
        }
        if ( i >= i_max )
        {
            i = parent->m_arrIndex + 1;
            parent = parent->m_parent;
            if ( inside_hidden_part && parent == nearest_expanded )
            {
                inside_hidden_part = FALSE;
            }
        }
    }
    while ( parent != (wxPGPropertyWithChildren*) NULL );

    m_bottomy = cury;

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("  \\-> m_bottomy = %i"),(int)m_bottomy);
#endif

#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )
    // Forces a new DoGetBestSize() call.
    wxScrolledWindow::InvalidateBestSize();
#endif

    // Visibles need to be recalculated *always* after y recalculation
    // (but make sure it stays here, above RecalculateVirtualSize).
    CalculateVisibles( -1, TRUE );

    RecalculateVirtualSize();

    // Reselect
    if ( prevSelected )
        SelectProperty( prevSelected, FALSE, FALSE, TRUE );

}

// -----------------------------------------------------------------------

// Call when scroll position changes. Do not pre-fill m_prevVY.
void wxPropertyGrid::CalculateVisibles ( int vy, bool full_recalc )
{
    //int prevVY = m_prevVY;

    if ( vy < 0 )
    {
        int vx;
        GetViewStart(&vx,&vy);
        vy *= wxPG_PIXELS_PER_UNIT;
        if ( full_recalc )
            m_prevVY = -1;
    }

    // Control not yet properly built.
    if ( vy >= (int)m_bottomy )
        return;

#if !wxPG_USE_CUSTOM_CONTROLS
    // Hide popup
    // FIXME: Delete after transient popup support added
    if ( m_wndPrimary && m_wndPrimary->IsKindOf(CLASSINFO(wxPGComboBox)) )
    {
        ((wxPGComboBox*)m_wndPrimary)->HidePopup( false );
    }
#endif

    int vy2 = vy + m_height;

    if ( vy2 > (int)m_bottomy )
        vy2 = m_bottomy;

    unsigned int arr_index = 0;
    unsigned int vis_height = vy2-vy;
    unsigned int new_item_count = vis_height/m_lineHeight;
    if ( vis_height % m_lineHeight )
        new_item_count++;

    wxPGArrayProperty& arr = m_arrVisible;

    arr.SetCount ( new_item_count );

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::CalculateVisibles ( vy=%i, vy2=%i, m_height=%i, newitemcount=%i, lineheight=%i )"),
        (int)vy, (int)vy2, (int)m_height, (int)new_item_count, (int)m_lineHeight );
#endif

    //wxASSERT ( vy != m_prevVY );
    wxASSERT ( vy >= 0 );

    if ( !new_item_count )
    {
        arr.Empty();
        return;
    }

    ITEM_ITERATION_VARIABLES

    wxPGProperty* base = NULL;

    // Will simpler operation be enough?
    if ( m_prevVY >= 0 )
    {
        if ( m_calcVisHeight == m_height )
        {
            if ( m_iFlags & wxPG_FL_SCROLLED )
            {
                int diff = vy - m_prevVY;
                if ( diff == m_lineHeight )
                {
                    // Scrolled one down
                    base = DoGetItemAtY_Full( vy2 - 1 );
                    wxASSERT ( base );
                    arr_index = new_item_count - 1;
                    for ( i=0; i<arr_index; i++ )
                        arr.Item(i) = arr.Item(i+1);
                    arr.Item(arr_index) = base;
                    base = (wxPGProperty*) NULL;
                }
                else if ( diff == -m_lineHeight )
                {
                    // Scrolled one up
                    base = DoGetItemAtY_Full( vy );
                    wxASSERT ( base );
                    vy2 = vy + m_lineHeight; // update visibility
                    for ( i=(new_item_count-1); i>arr_index; i-- )
                        arr.Item(i) = arr.Item(i-1);
                    arr.Item(arr_index) = base;
                    base = (wxPGProperty*) NULL;
                }
                else
                    base = DoGetItemAtY_Full( vy );
            }
            else
                base = DoGetItemAtY_Full( vy );
        }
        else
        if ( m_prevVY == vy && !(m_iFlags & wxPG_FL_SCROLLED) )
        {
            if ( m_height > m_calcVisHeight )
            {
            // Increased height - add missing items
                arr_index = (m_calcVisHeight-1)/m_lineHeight;
                if ( arr_index >= new_item_count )
                {
                    // Now, were probably below last item here
                    //if ( (vy+m_calcVisHeight) >= (int)m_bottomy )
                        base = NULL;
                    /*else
                        arr_index = arr.GetCount()-1;*/
                }
                else
                {
                    base = (wxPGProperty*) arr.Item( arr_index );
                }
            }
            else
            {
            // Decreased height - do nothing
                //base = NULL;
            }
        }
        else
            base = DoGetItemAtY_Full( vy );
    }
    else
    {
        base = DoGetItemAtY_Full( vy );
    }

    if ( base )
    {

        ITEM_ITERATION_INIT(base->m_parent,base->m_arrIndex)

    #if __INTENSE_DEBUGGING__
        wxLogDebug ( wxT("  Starting at index %i"), (int)arr_index );
    #endif

        ITEM_ITERATION_VO_LOOP_BEGIN

            //wxASSERT ( p->m_y >= 0 );

            // update visibility limit reached?
            if ( p->m_y >= vy2 ) { parent = NULL; break; }

            //wxLogDebug ( wxT("Visible(%i): %s"), (int)p->m_y, p->m_label.c_str() );

        #ifdef __WXDEBUG__
            if ( arr_index >= arr.GetCount() )
            {
                wxLogDebug (wxT("  wxPropertyGrid::CalculateVisibles Loop overflow (index=%i,vy+vis_height=%i,p->m_y=%i)"),
                    (int)arr_index,(int)(vy+vis_height),(int)p->m_y);
            }
        #endif

            arr.Item(arr_index) = (void*)p;
            arr_index++;

        ITEM_ITERATION_VO_LOOP_END
    }

    m_iFlags &= ~(wxPG_FL_SCROLLED);

    m_prevVY = vy;

    m_calcVisHeight = m_height;

    //wxLogDebug ( wxT("2") );

}

// -----------------------------------------------------------------------

// This version uses the visible item cache.
wxPGProperty* wxPropertyGrid::DoGetItemAtY ( int y )
{

    //wxASSERT ( m_prevVY >= 0 );

    // Outside(check 1)?
    if ( y >= (int)m_bottomy || y < 0 )
    {
        /*
    #if __PAINT_DEBUGGING__
        wxLogDebug (wxT("WARNING: DoGetItemAtY(a): y = %i"),y);
    #endif
        */
        return (wxPGProperty*) NULL;
    }

    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;

    // Need to recalculate visibility cache
    // Note: need to check for y < m_prevVY is a hack.
    if ( m_prevVY != vy ||y < m_prevVY ) //m_iFlags & wxPG_FL_SCROLLED ||
        CalculateVisibles( vy, TRUE );

    // Outside(check 2)?
    if ( y >= (vy+m_height) )
    {
        /*
    #if __PAINT_DEBUGGING__
        wxLogDebug (wxT("WARNING: DoGetItemAtY(b): y = %i"),y);
    #endif
        */
        return NULL;
    }

    //wxLogDebug ( wxT("wxPropertyGrid::DoGetItemAtY ( %i )"), y );

    unsigned int index = (unsigned int)((y - vy) / m_lineHeight);

    //wxLogDebug ( wxT("  \\--> index = %i, y = %i, m_prevVY = %i"), index, y, m_prevVY );

#ifdef __WXDEBUG__
    // Consistency checks
    if ( !m_arrVisible.GetCount() )
        return NULL;
    if ( index >= m_arrVisible.GetCount() )
    {
        wxLogDebug (wxT("  index = %i"),(int)index);
        wxLogDebug (wxT("  (height/lineheight+1) = %i"),(int)((m_height/m_lineHeight)+1));
        wxLogDebug (wxT("  m_arrVisible.GetCount() = %i"),(int)m_arrVisible.GetCount());

        wxCHECK_MSG( index < m_arrVisible.GetCount(), (wxPGProperty*) NULL,
            wxT("Not enough entries in m_arrVisible (y was < m_bottomy).") );
    }
#endif

    if ( index >= m_arrVisible.GetCount() )
    {
        index = m_arrVisible.GetCount()-1;
    }

    return (wxPGProperty*)m_arrVisible.Item(index);
}

// -----------------------------------------------------------------------
// wxPropertyGrid graphics related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::OnPaint( wxPaintEvent& WXUNUSED(event) )
{

    wxPG_PAINT_DC_INIT()

    // Prevent paint calls from occurring after destruction has started
    if ( !(m_iFlags & wxPG_FL_INITIALIZED) )
        return;

#if __PAINT_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::OnPaint()") );
#endif

    // Find out where the window is scrolled to
    int vx,vy;                     // Top left corner of client
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();

    r.y += vy;

    //wxLogDebug ( wxT(" area = %i - %i"), r.y, r.y + r.height );

    // Repaint this rectangle
    //if ( r.height > 0 )
        DrawItems ( dc, r.y, r.y + r.height,
    #if wxPG_ALLOW_CLIPPING
            NULL //&r
    #else
            NULL
    #endif
        );

    // We assume that the size set when grid is shown
    // is what is desired.
    m_iFlags |= wxPG_FL_GOOD_SIZE_SET;

}

// -----------------------------------------------------------------------

//
// This is the one called by OnPaint event handler and others.
// topy and bottomy are already unscrolled
// Clears any area in coordinates that doesn't have items.
//
void wxPropertyGrid::DrawItems ( wxDC& dc, unsigned int topy, unsigned int bottomy,
    const wxRect* clip_rect )
{

    if ( m_frozen || m_height < 1 || bottomy < topy ) return;

#if __PAINT_DEBUGGING__
    wxLogDebug (wxT("wxPropertyGrid::DrawItems ( %i -> %i, clip_rect = 0x%X )"),topy,bottomy,
        (unsigned int)clip_rect);
#endif

    // items added check
    if ( FROM_STATE(m_itemsAdded) ) PrepareAfterItemsAdded();

    unsigned int vx, vy;                     // Top left corner of client
    GetViewStart((int*)&vx,(int*)&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    unsigned int client_bottom = (unsigned int)m_height + vy;

    // Clip topy and bottomy
    if ( bottomy > client_bottom )
        bottomy = client_bottom;
    if ( topy < vy )
        topy = vy;

#if __PAINT_DEBUGGING__
    wxLogDebug (wxT("  \\--> ( final area %i -> %i )"),topy,bottomy);
#endif

    //
    // Determine first and last item to draw
    // (don't draw if already over the top)
    //

    if ( topy < client_bottom && topy < m_bottomy && FROM_STATE(m_properties)->GetCount() > 0 )
    {

        wxPGProperty* first_item = DoGetItemAtY(topy);

        if ( first_item == (wxPGProperty*) NULL )
        {
    #ifdef __WXDEBUG__
            wxString msg;
            msg.Printf(wxT("WARNING: wxPropertyGrid::DrawItems(): first_item == NULL!"));
            wxMessageBox(msg);
            wxLogDebug(msg);
            wxLogDebug(wxT("  More info: y: %i -> %i   visible_window: %i -> %i"),
                (int)topy,(int)bottomy,(int)vy,(int)client_bottom);
            // This is here for debugging purposes.
            DoGetItemAtY(topy);
    #endif
            return;
        }

        wxPGProperty* last_item = (wxPGProperty*) NULL;

        // last_item may be NULL on call to DoDrawItems
        // in this case last_item will truly become the last item

        if ( bottomy > topy && bottomy < m_bottomy )
        {
            last_item = DoGetItemAtY(bottomy-1);
        #if __PAINT_DEBUGGING__
            wxLogDebug ( wxT("  \\--> WARNING: last_item acquisition failed (should not)!"));
        #endif
        }

        DoDrawItems ( dc, first_item, last_item, clip_rect );
    }

    // Clear area beyond m_bottomy?
    if ( bottomy > m_bottomy )
    {
        //wxLogDebug(wxT("Clear area beyond m_bottomy"));
        dc.BeginDrawing();
        wxColour& bgc = wxPG_SLACK_BACKROUND;
        //wxColour& bgc = wxColour(255,0,255);
        dc.SetPen ( wxPen(bgc) );
        dc.SetBrush ( wxBrush(bgc) );
        unsigned int clear_top = m_bottomy;
        if ( topy > clear_top ) clear_top = topy;
        dc.DrawRectangle ( 0, clear_top, m_width, m_height-(clear_top-vy) );
        dc.EndDrawing();
    }
}

// -----------------------------------------------------------------------

#define ITEM_ITERATION_UVC_VARIABLES \
    unsigned int ind; \
    wxPGProperty* p;

// UVC = Use Visibility Cache
// VISTART = index to first item from visibility cache to use.
// BOTTOMY = Logical y coordinate of last item to draw.
#define ITEM_ITERATION_UVC_LOOP_BEGIN(VISTART,BOTTOMY) \
    ind = VISTART; \
    do \
    { \
        p = (wxPGProperty*)m_arrVisible.Item(ind); \
        ind++; \
        int parenting = p->GetParentingType();

#define ITEM_ITERATION_UVC_LOOP_END(BOTTOMY) \
    } while ( p->m_y < BOTTOMY ); \

//
// Uses three pass approach, so it is optimized for drawing
// multiple items at once.
//
// IMPORTANT NOTES:
// - Clipping rectangle must be of physical coordinates.
//
//
void wxPropertyGrid::DoDrawItems (
#if wxPG_DOUBLE_BUFFER
        wxDC& dc_main,
#else
        wxDC& dc,
#endif
        wxPGProperty* first_item, wxPGProperty* last_item,
        const wxRect* clip_rect )
{

    if ( m_frozen || m_height < 1 )
        return;

    ITEM_ITERATION_UVC_VARIABLES

    wxPGPaintData paintdata;
    wxRect r;
    int lh = m_lineHeight;

    wxASSERT ( !FROM_STATE(m_itemsAdded) );
    wxASSERT ( first_item != NULL );
    wxASSERT ( FROM_STATE(m_properties->GetCount()) );

    // Make sure visibility cache is up-to-date
    int vy;
    int vx;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;
    if ( vy != m_prevVY )
        CalculateVisibles(vy,TRUE);

    if ( vy != m_prevVY )
        return;

    // Determine last item, if not given (but requires clip_rect).
    if ( last_item == NULL )
    {
        if ( clip_rect != NULL )
        {

            unsigned int bottomy = /*vy + */clip_rect->y + clip_rect->height;

            if ( bottomy <= (unsigned int)first_item->m_y )
                last_item = first_item;
        }

        if ( last_item == NULL )
        {
            last_item = DoGetItemAtY(vy+m_height-1);
            if ( last_item == NULL )
                last_item = GetLastItem(TRUE);
            //wxLogDebug (wxT("  GetLastItem() = %s"),last_item?last_item->m_label.c_str():wxT("NULL"));
        }
    }

    int first_item_top_y = first_item->m_y;
    int last_item_bottom_y = last_item->m_y+lh-1;
    //unsigned int start_index = first_item->GetIndexInParent();

    int y_rel_mod = 0;

    // Entire range outside scrolled, visible area?
    if ( first_item_top_y >= (vy+m_height) || last_item_bottom_y <= vy )
        return;

#if __PAINT_DEBUGGING__
    wxLogDebug(wxT("  -> DoDrawItems ( \"%s\" -> \"%s\", height=%i (ch=%i), clip_rect = 0x%lX )"),
        first_item->GetLabel().c_str(),
        last_item->GetLabel().c_str(),
        (int)(last_item_bottom_y - first_item_top_y),
        (int)m_height,
        (unsigned long)clip_rect );
#endif

    wxASSERT ( first_item_top_y < last_item_bottom_y );

    // Get first and last indexes to visibility cache
    unsigned int vi_start = (first_item_top_y - vy) / lh;
    int vi_end_y = last_item->m_y;

    if ( vi_start >= m_arrVisible.GetCount() )
    {
        wxLogDebug (wxT("WARNING: vi_start >= m_arrVisible.GetCount() ( %i >= %i )"),
            (int)vi_start, (int)m_arrVisible.GetCount() );
        return;
    }

#ifdef __WXDEBUG__
    unsigned int vi_end = (last_item->m_y - vy) / lh;
    if ( vi_end >= m_arrVisible.GetCount() )
    {
        wxLogDebug (wxT("WARNING: vi_end >= m_arrVisible.GetCount() ( %i >= %i )"),
            (int)vi_end, (int)m_arrVisible.GetCount() );
        return;
    }
#endif

    int x = m_marginwidth;
    int y; // = first_item_top_y;
/*#if wxPG_DOUBLE_BUFFER
    wxCoord width = m_width;
#endif*/

    long window_style = m_windowStyle;

    //
    // With wxPG_DOUBLE_BUFFER, do double buffering
    // - buffer's y = 0, so align cliprect and coordinates to that
    //
#if wxPG_DOUBLE_BUFFER

    //wxASSERT ( m_doubleBuffer != NULL );
    if ( !m_doubleBuffer )
        return;

    // Need to hide the custom control caret
# if wxPG_USE_CUSTOM_CONTROLS
    m_ccManager.HideCaretBalanced();
# endif

    // Must fit to double-buffer
# ifdef __WXDEBUG__
    if ( (last_item_bottom_y - first_item_top_y) > m_doubleBuffer->GetHeight() )
    {
        wxString msg;
        msg.Printf ( wxT("wxPropertyGrid: DOUBLE-BUFFER TOO SMALL ( drawn %i vs db height %i vs client_height %i)!"),
            (int)(last_item_bottom_y - first_item_top_y),
            (int)(m_doubleBuffer->GetHeight()),
            (int)m_height );
        wxLogError ( msg );
        wxLogDebug ( msg );
    }
# endif

    const wxRect* blit_clip_rect = clip_rect;
    wxRect cr2;

    //
    // clip_rect conversion
    if ( clip_rect )
    {
        cr2 = *clip_rect;
        cr2.y -= (first_item_top_y);
        clip_rect = &cr2;
    }
    int render_height = last_item->m_y - first_item_top_y + m_lineHeight;
    y_rel_mod = first_item_top_y;
    last_item_bottom_y -= first_item_top_y;
    first_item_top_y = 0;
    //y = 0;

    wxMemoryDC dc;
    dc.SelectObject ( *m_doubleBuffer );

#endif

    wxFont& normalfont = m_font;

    //
    // Prepare some pens and brushes that are often changed to.
    //

    wxBrush marginBrush(m_colMargin);
    wxPen marginPen(m_colMargin);
    wxBrush capbgbrush(m_colCapBack,wxSOLID);
    wxPen linepen(m_colLine,1,wxSOLID);

    // pen that has same colour as text
    wxPen outlinepen(m_colPropFore,1,wxSOLID);

    if ( clip_rect )
        dc.SetClippingRegion ( *clip_rect );

    dc.BeginDrawing();

    //
    // Clear margin with background colour
    //
    dc.SetBrush ( marginBrush );
    if ( !(window_style & wxPG_HIDE_MARGIN) )
    {
        dc.SetPen ( *wxTRANSPARENT_PEN );
        dc.DrawRectangle (-1,first_item_top_y-1,m_marginwidth+2,last_item_bottom_y-first_item_top_y+3);
    }

    /*
    // This colorizer helps to debug painting.
    bool small_draw = FALSE;
    if ( render_height < (m_height-(lh*3)) )
    {
        if ( first_item == last_item )
        {
            bgbrush = wxBrush(wxColour(255,128,128));
            linepen = wxPen(wxColour(128,0,255));
            //boxbrush = wxBrush(wxColour(192,192,192));
        }
        else
        {
            bgbrush = wxBrush(wxColour(128,255,128));
            linepen = wxPen(wxColour(0,0,255));
            //boxbrush = wxBrush(wxColour(230,230,230));
        }
        small_draw = TRUE;
    }
    */

    //dc.SetPen ( *wxTRANSPARENT_PEN );
    //dc.SetFont(normalfont);

    wxPGProperty* selected = m_selected;

/*#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    bool selected_painted = FALSE;
#endif*/

    // NOTE: Clipping and pen/brush switching are main reasons for multi-pass approach.

    //
    // zero'th pass: Wireframes.
    // (this could be embedded in another loop)

    dc.SetBrush ( marginBrush );

    unsigned long cur_first_ind = vi_start;
    unsigned long next_cur_first_ind = 0xFFFFFFFF;
    wxPGPropertyWithChildren* cur_category = (wxPGPropertyWithChildren*) NULL;
    int vcache_last_item_y = vy + m_height;
    if ( vcache_last_item_y > (int)m_bottomy ) vcache_last_item_y = m_bottomy;
    vcache_last_item_y -= lh;

    ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

        wxPGPropertyWithChildren* parent = p->GetParent();

        // Does this wireframe end?
        // Conditions:
        // A) This is last item within its parent, and parent is category.
        // B) Next is category.
        // C) This is collapsed category.
        // D) This is the last item drawn.

        //wxLogDebug(wxT("  %s (%i vs %i)"),p->m_label.c_str(),p->m_y,vi_end_y);

        if ( p->m_y >= vi_end_y )
        {
            // This is the last item.
            //wxLogDebug(wxT("--> last item"));
            next_cur_first_ind = ind;
        }
        else if ( p->m_arrIndex >= (parent->GetCount()-1) &&
                  parent->GetParentingType() > 0 &&
                  parenting <= 0 )
                  /*( parenting == 0 ||
                    !((wxPGPropertyWithChildren*)p)->GetCount() ||
                    !((wxPGPropertyWithChildren*)p)->IsExpanded() ) )*/
        {
            // This is last item within its parent, and parent is category, but this isn't
            // an non-empty category.
            //wxLogDebug(wxT("--> category ends"));
            cur_category = (wxPropertyCategoryClass*)parent;
            next_cur_first_ind = ind;
        }
        else if ( ((wxPGProperty*)m_arrVisible.Item(ind))->GetParentingType() > 0 )
        {
            // Next item is a category.
            //wxLogDebug(wxT("--> next item is  category"));
            next_cur_first_ind = ind;
        }
        else if ( parenting > 0 &&
                  (!((wxPGPropertyWithChildren*)p)->IsExpanded() ||
                   !((wxPGPropertyWithChildren*)p)->GetCount()) )
        {
            // This is collapsed category.
            //wxLogDebug(wxT("--> collapsed category"));
            cur_category = (wxPropertyCategoryClass*)p;
            next_cur_first_ind = ind;
        }

        // When new category begins or old ends, draw wireframe for items in-between
        if ( next_cur_first_ind < 0xFFFFFF )
        {

            wxPGProperty* cur_first = (wxPGProperty*)m_arrVisible.Item(cur_first_ind);
            wxPGPropertyWithChildren* cur_last_item = (wxPGPropertyWithChildren*)p;

            if ( !cur_category )
            {
                if ( cur_first->GetParentingType() > 0 )
                {
                    cur_category = (wxPropertyCategoryClass*)cur_first;
                }
                else if ( !(m_windowStyle & wxPG_HIDE_CATEGORIES) )
                {
                    cur_category = GetPropertyCategory(cur_first);
                    /*if ( !cur_category )
                        cur_category = (wxPropertyCategoryClass*)FROM_STATE(m_properties);*/
                }
            }

            int draw_top = cur_first->m_y - y_rel_mod;
            int draw_bottom = cur_last_item->m_y + lh - y_rel_mod;
            int frame_top = draw_top;
            int frame_bottom = draw_bottom;
            int margin_top = draw_top;
            int margin_bottom = draw_bottom;

            int ly = frame_top + lh - 1;

            if ( cur_first->GetParentingType() > 0 )
            {
                wxPropertyCategoryClass* pc = ((wxPropertyCategoryClass*)cur_first);
                frame_top += lh;
                if ( !pc->IsExpanded() )
                {
                    // Category collapsed.
                    frame_top = frame_bottom + 1;
                }
            }

            int grey_x = x;
            if ( cur_category /*!(window_style & wxPG_HIDE_CATEGORIES)*/ )
                grey_x += ((unsigned int)((cur_category->GetDepth()-1)*m_subgroup_extramargin));

            //wxLogDebug ( wxT("wireframe: %s -> %s (grey_x:%i)"), cur_first->GetLabel().c_str(),
            //    cur_last_item->GetLabel().c_str(),((int)grey_x-x));

            dc.SetPen ( *wxTRANSPARENT_PEN );

            // Clear extra margin area.
            dc.DrawRectangle ( x-1, margin_top, grey_x - x + 1, margin_bottom-margin_top );

            dc.SetPen ( linepen );

            if ( frame_bottom > frame_top )
            {

                //if ( cat_top < first_item_top_y )
                //  cat_top = first_item_top_y;


                // Margin Edge
                dc.DrawLine ( grey_x, frame_top, grey_x, frame_bottom );

                // Splitter
                dc.DrawLine ( m_splitterx, frame_top, m_splitterx, frame_bottom );

                // Horizontal Lines
                while ( ly < (frame_bottom-1) )
                {
                    dc.DrawLine ( grey_x, ly, m_width, ly );
                    ly += lh;
                }

            }

            int use_depth = grey_x; // Default is to simply tidy up this wireframe.

            // Properly draw top line of next wireframe, if adjacent.

            // Get next item.
            wxPGProperty* next_item;
            //if ( ind < m_arrVisible.GetCount() )
            if ( cur_last_item->m_y < vcache_last_item_y )
            {
                next_item = (wxPGProperty*)m_arrVisible.Item(ind);
            }
            else
            {
                // Was not in visibility cache, so use clumsier method.
                next_item = GetNeighbourItem(cur_last_item,TRUE,1);
                if (!next_item)
                    next_item = cur_last_item; // This will serve our purpose.
            }
            //wxLogDebug(wxT("next_item: %s"),next_item->GetLabel().c_str());

            // Just take the depth and is-it-category out of it.
            int next_parenting = next_item->GetParentingType();
            int last_parenting = cur_last_item->GetParentingType();

            // A) If both are categories, draw line with background colour.
            // B) If only next is category, use its category's depth.
            // C) If only last is category, use grey_x as depth.
            // D) If neither is a category, use smaller.
            if ( next_parenting > 0 )
            {
                // Next is category.

                if ( last_parenting > 0 )
                {
                    // Last is a category too - draw complete line with background colour.
                    dc.SetPen ( marginPen );
                    use_depth = x;
                }
            }
            else
            {
                // Next is not a category.
                wxPropertyCategoryClass* next_cat = GetPropertyCategory(next_item);
                int depth_next = x;
                if ( next_cat && /*cur_category*/ !(window_style & wxPG_HIDE_CATEGORIES) )
                {
                    //wxLogDebug(wxT("next_item_cat: %s"),next_cat->GetLabel().c_str());
                    depth_next += ((unsigned int)((next_cat->GetDepth()-1)*m_subgroup_extramargin));
                }

                if ( last_parenting <= 0 )
                {
                    // Last is not a category - use lesser depth
                    if ( depth_next < grey_x )
                        use_depth = depth_next;
                    //wxLogDebug(wxT("- neither is category"));
                }
                else
                {
                    // Last is a category
                    use_depth = depth_next;
                    //wxLogDebug(wxT("last only is category"));
                }
            }

            //wxLogDebug(wxT("last_line_use_depth: %i"),(int)use_depth);
            dc.DrawLine ( use_depth, ly, m_width, ly );

            cur_first_ind = next_cur_first_ind;
            next_cur_first_ind = 0xFFFFFFFF;
            //cur_first = (wxPGPropertyWithChildren*)p;
            cur_category = (wxPGPropertyWithChildren*) NULL;
        }

        //cur_last_item = p;

    ITEM_ITERATION_UVC_LOOP_END(vi_end_y)

    //
    // First pass: Category background and text, Images, Label+value background.
    //

    //wxLogDebug (wxT("  \\--> first pass..."));
    //ITEM_ITERATION_INIT(first_item->m_parent,start_index)
    y = first_item_top_y;

    dc.SetTextForeground( m_colCapFore );
    dc.SetFont( m_captionFont );
    dc.SetPen ( *wxTRANSPARENT_PEN );

    ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

        y += m_spacingy;

        int text_x = x + ((unsigned int)((p->GetDepth()-1)*m_subgroup_extramargin));

        if ( parenting > 0 )
        {

            dc.SetBrush ( capbgbrush ); // Category label background colour.

            // Category - draw background, text and possibly selection rectangle.
            wxPropertyCategoryClass* pc = (wxPropertyCategoryClass*)p;

            // Note how next separator line is overdrawn if next item is category .
            int use_lh = lh;
            if ( ind < (m_arrVisible.GetCount()) &&
                 ( ((wxPGProperty*)m_arrVisible[ind])->GetParentingType() <= 0 )
               )
                use_lh -= 1;

            //dc.DrawRectangle ( x, y-m_spacingy, m_width-x, use_lh );
            dc.DrawRectangle ( text_x, y-m_spacingy, m_width-text_x, use_lh );
            dc.DrawText ( pc->GetLabel(), text_x+wxPG_XBEFORETEXT, y );

            // active caption gets nice dotted rectangle
            if ( p == selected )
            {
                wxPen pen( m_colPropFore, 1, wxDOT );

                // TODO: pen.SetCap is disabled to demonstrate the wxDOT/ExtCreatePen bug
                // TODO: Remove this line after CreatePen/ExtCreatePen issue fixed in main lib
                pen.SetCap( wxCAP_BUTT );

                dc.SetPen ( pen );
                dc.SetBrush ( *wxTRANSPARENT_BRUSH );
                dc.DrawRectangle ( text_x+wxPG_XBEFORETEXT-wxPG_CAPRECTXMARGIN,
                    y-wxPG_CAPRECTYMARGIN, pc->GetTextExtent()+(wxPG_CAPRECTXMARGIN*2),
                        m_fontHeight+(wxPG_CAPRECTYMARGIN*2) );
                dc.SetPen ( *wxTRANSPARENT_PEN );
            }

        }
        else
        {

            // Basic background colour.
            dc.SetBrush ( *(wxPGBrush*)m_arrBgBrushes[p->m_bgColIndex] );

            //wxLogDebug(wxT("%s: %i"),p->m_label.c_str(),(int)p->m_depthBgCol);

            int grey_depth = 0;
            if ( !(window_style & wxPG_HIDE_CATEGORIES) )
                grey_depth = (((int)p->m_depthBgCol)-1) * m_subgroup_extramargin;

            // In two parts to retain splitter

            if ( p == m_selected )
            {
            // Selected get different label background.
                dc.SetBrush ( m_colSelBack );

                dc.DrawRectangle ( x+grey_depth+1, y-m_spacingy, m_splitterx-grey_depth-x-1, lh-1 );

            }
            else
            {
                dc.DrawRectangle ( x+grey_depth+1, y-m_spacingy, m_splitterx-grey_depth-x-1, lh-1 );
            }

            dc.DrawRectangle ( m_splitterx+1, y-m_spacingy, m_width-m_splitterx, lh-1 );

        }

        y += m_fontHeight+m_spacingy+1;

    ITEM_ITERATION_UVC_LOOP_END(vi_end_y)

    dc.SetTextForeground( m_colPropFore );
    dc.SetFont( normalfont );

    // Restore boxbrush.
    //boxbrush.SetColour ( m_colPropBack );

    //
    // Second pass: Expander Buttons, Labels.
    //
    // Second pass happens entirely on the left side, so sometimes
    // we can just skip it.
    if ( clip_rect == NULL || clip_rect->x < m_splitterx )
    {

        //wxLogDebug (wxT("  \\--> second pass..."));

        y = first_item_top_y;

        r = wxRect(0,y,m_splitterx,last_item_bottom_y);
        dc.SetClippingRegion ( r );

        dc.SetFont(normalfont);

        ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

            //
            // Expand/collapse button image.
            if (parenting != 0                     &&
                !(window_style & wxPG_HIDE_MARGIN) &&
                ((wxPGPropertyWithChildren*)p)->GetChildCount() )
            {
                wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

                int image_x = m_gutterwidth + ((unsigned int)((p->m_depth-1)*m_subgroup_extramargin));

                y += m_buttonSpacingY;

            #if (wxPG_USE_RENDERER_NATIVE)
                // Prepare rectangle to be used
                r.x = image_x; r.y = y;
                r.width = m_iconwidth; r.height = m_iconheight;
            #elif wxPG_ICON_WIDTH
            // Drawing expand/collapse button manually
                dc.SetPen(wxPen(m_colPropFore));
                if ( parenting > 0 ) dc.SetBrush(*wxTRANSPARENT_BRUSH);
                else dc.SetBrush(m_colPropBack);
                dc.DrawRectangle( image_x, y, m_iconwidth, m_iconwidth );
                int _y = y+(m_iconwidth/2);
                //dc.SetPen(wxPen(m_colPropFore,1,wxDOT));
                dc.DrawLine(image_x+2,_y,image_x+m_iconwidth-2,_y);
            #else
                wxBitmap* bmp;
            #endif

                if ( pwc->m_expanded )
                {
                #if (wxPG_USE_RENDERER_NATIVE)
                    wxRendererNative::Get().DrawTreeItemButton (
                            this,
                            dc,
                            r,
                            wxCONTROL_EXPANDED
                        );
                #elif wxPG_ICON_WIDTH
                    //
                #else
                    bmp = m_collbmp;
                #endif

                }
                else
                {
                #if (wxPG_USE_RENDERER_NATIVE)
                    wxRendererNative::Get().DrawTreeItemButton (
                            this,
                            dc,
                            r,
                            0
                        );
                #elif wxPG_ICON_WIDTH
                    int _x = image_x+(m_iconwidth/2);
                    dc.DrawLine(_x,y+2,_x,y+m_iconwidth-2);
                #else
                    bmp = m_expandbmp;
                #endif
                }

            #if (wxPG_USE_RENDERER_NATIVE)
                //
            #elif wxPG_ICON_WIDTH
                //
            #else
                dc.DrawBitmap ( *bmp, m_gutterwidth, y, true );
            #endif

                y -= m_buttonSpacingY;
            }

            y += m_spacingy;

            if ( parenting <= 0 )
            {
                // Non-categories.

                int text_x = x;
                // Use basic depth if in non-categoric mode and parent is base array.
                if ( !(window_style & wxPG_HIDE_CATEGORIES) || p->GetParent() != FROM_STATE(m_properties) )
                {
                    text_x += ((unsigned int)((p->m_depth-1)*m_subgroup_extramargin));
                }
                /*
                else
                {
                    wxLogDebug  ( wxT("%s"), p->GetLabel().c_str() );
                    text_x = x;
                }
                */

                if ( p != selected )
                {
                // background
                    dc.DrawText ( p->m_label, text_x+wxPG_XBEFORETEXT, y );
                }
                else
                {
                // Selected gets different colours.
                    //if ( m_iFlags & wxPG_FL_FOCUSED )
                    {
                        //bgbrush.SetColour (  );
                        //dc.SetBrush ( m_colSelBack );
                        //dc.SetPen ( *wxTRANSPARENT_PEN );
                        dc.SetTextForeground( m_colSelFore );
                    }

                    //int grey_depth = 0;
                    //if ( !(window_style & wxPG_HIDE_CATEGORIES) )
                    //    grey_depth = (((int)p->m_depthBgCol)-1) * m_subgroup_extramargin;

                    //dc.DrawRectangle ( x+grey_depth+1, y-m_spacingy, m_splitterx-x-grey_depth, lh-1 );
                    dc.DrawText ( p->m_label, text_x+wxPG_XBEFORETEXT, y );

                    dc.SetTextForeground( m_colPropFore );

                /*#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
                    selected_painted = TRUE;
                #endif*/

                }
            }
            else
            {
                /*// switch background colour
                bgbrush.SetColour ( ((wxPropertyCategoryClass*)p)->m_colCellBg );
                dc.SetBrush ( bgbrush );*/
            }

            y += m_fontHeight+m_spacingy+1;

            //if ( y > last_item_bottom_y ) { parent = NULL; break; }

        ITEM_ITERATION_UVC_LOOP_END(vi_end_y)
    }


    //
    // Third pass: Values (including editor gfx if custom controls).
    //
    dc.DestroyClippingRegion();

    if ( clip_rect )
    {
        // third pass happens entirely on the right side, so sometimes
        // we can just skip it
        if ( (clip_rect->x + clip_rect->width) < m_splitterx )
            x = -1;
        dc.SetClippingRegion ( *clip_rect );
    }

    // This used with value drawer method.
    wxRect value_rect(0,0,
        m_width-(m_splitterx+wxPG_CONTROL_MARGIN),
        m_fontHeight);

    wxSize imageSize;

    if ( x != -1 )
    {

        r.x = m_splitterx+1+wxPG_CONTROL_MARGIN;
        r.width = m_width-m_splitterx-wxPG_CONTROL_MARGIN;
        //r.x = m_splitterx+wxPG_DIST_SPLITTER_TO_IMAGE;
        //r.width = m_width-m_splitterx-wxPG_DIST_SPLITTER_TO_IMAGE-1;
        r.height = lh-1;

    #if wxCC_CORRECT_CONTROL_POSITION
        const int vy2 = vy;
    #elif wxPG_USE_CUSTOM_CONTROLS
        const int vy2 = 0;
    #endif

            //wxLogDebug (wxT("  \\--> third pass..."));

        // Altough this line may seem unnecessary, it isn't
        dc.SetFont(normalfont);

        dc.SetPen( *wxTRANSPARENT_PEN );

        // Prepare paintdata.
        paintdata.m_parent = this;
        paintdata.m_choiceItem = -1; // Not drawing list item at this time.

        y = first_item_top_y;

        ITEM_ITERATION_UVC_LOOP_BEGIN(vi_start,vi_end_y)

            if ( parenting <= 0 )
            {
                r.y = y;
                y += m_spacingy;

             // background
                dc.SetBrush ( *(wxPGBrush*)m_arrBgBrushes[p->m_bgColIndex] );

                // draw value string only if editor widget not open
                // (exception: no primary editor widget or it is hidden)
                if ( p != selected || !m_wndPrimary
                    // "if not primary shown" is required because
                    // primary is not usually shown during splitter
                    // movement.
                   /*#if !wxPG_USE_CUSTOM_CONTROLS
                        || !m_wndPrimary->IsShown()
                   #endif*/
                        || m_dragStatus > 0
                   )
                {

                    value_rect.x = m_splitterx+wxPG_CONTROL_MARGIN;
                    value_rect.y = y;

                    if ( p->m_flags & wxPG_PROP_CUSTOMIMAGE )
                    {
                        imageSize = GetImageSize(p);
                        value_rect.x += imageSize.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
                    }

                    // Greyed text?
                    if ( p->m_flags & wxPG_PROP_DISABLED )
                        dc.SetTextForeground ( m_colCapFore );

                    // Set bold font?
                    if ( p->m_flags & wxPG_PROP_MODIFIED && (window_style & wxPG_BOLD_MODIFIED) )
                        dc.SetFont ( m_captionFont );

                    const wxPGEditor* editor = p->GetEditorClass();

                    if ( p != selected )
                    {
                        dc.DrawRectangle ( r );
                        //wxLogDebug(wxT("out_combo: %i"),(int)(paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN2));
                        editor->DrawValue(dc,p,value_rect);
                    }
                    else
                    {
                        if ( m_wndPrimary )
                        #if wxPG_USE_CUSTOM_CONTROLS
                            dc.SetBrush ( m_ccManager.GetWindowColour() );
                        #else
                            //dc.SetBrush ( m_wndPrimary->GetBackgroundColour() );
                            dc.SetBrush ( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
                        #endif
                        else
                            dc.SetBrush ( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

                        dc.DrawRectangle( r );
                        editor->DrawValue(dc,p,value_rect);

                    #if wxPG_USE_CUSTOM_CONTROLS
                        if ( m_wndSecondary )
                        {
                        # if wxPG_DOUBLE_BUFFER
                            wxPoint alignpt(0,vy2-first_item->m_y);
                        # else
                            wxPoint alignpt(0,vy2);
                        # endif
                            m_wndSecondary->Draw ( dc, &alignpt );

                            dc.SetFont( normalfont );
                            dc.SetPen( *wxTRANSPARENT_PEN );
                            dc.SetTextForeground( m_colPropFore ); // It may change text colour too.
                        }
                    #endif
                    }

                    // Return original text colour?
                    if ( p->m_flags & wxPG_PROP_DISABLED )
                        dc.SetTextForeground( m_colPropFore );

                    // Return original font?
                    if ( p->m_flags & wxPG_PROP_MODIFIED && (window_style & wxPG_BOLD_MODIFIED) )
                        dc.SetFont(normalfont);

                    if ( p->m_flags & wxPG_PROP_CUSTOMIMAGE )
                    {
                        wxRect imagerect(r.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                            r.y+wxPG_CUSTOM_IMAGE_SPACINGY,wxPG_CUSTOM_IMAGE_WIDTH,
                            r.height-(wxPG_CUSTOM_IMAGE_SPACINGY*2));

                        dc.SetPen ( outlinepen );
                        if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
                        {
                            p->OnCustomPaint( dc, imagerect, paintdata );
                        }
                        else
                        {
                            dc.SetBrush(*wxWHITE_BRUSH);
                            dc.DrawRectangle(imagerect);
                        }
                        dc.SetPen( *wxTRANSPARENT_PEN );
                    }

                }
                else
                {

                    if (
                #if !wxPG_USE_CUSTOM_CONTROLS
                         !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) ||
                #endif
                         m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE
                       )
                    {
                        //wxLogDebug(wxT("Primary doesn't fill entire"));
                        //dc.SetBrush ( m_wndPrimary->GetBackgroundColour() );
                        dc.SetBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
                        dc.DrawRectangle( r );
                    }
                    if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE )
                    {
                        wxRect imagerect(r.x + wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                            r.y+wxPG_CUSTOM_IMAGE_SPACINGY,wxPG_CUSTOM_IMAGE_WIDTH,
                            r.height-(wxPG_CUSTOM_IMAGE_SPACINGY*2));

                        dc.SetPen ( outlinepen );
                        if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
                        {
                            p->OnCustomPaint( dc, imagerect, paintdata );
                        }
                        else
                        {
                            dc.SetBrush(*wxWHITE_BRUSH);
                            dc.DrawRectangle(imagerect);
                        }
                    }
                #if wxPG_USE_CUSTOM_CONTROLS

                # if wxPG_DOUBLE_BUFFER
                    wxPoint alignpt(0,vy2-first_item->m_y);
                # else
                    wxPoint alignpt(0,vy2);
                # endif
                    m_wndPrimary->Draw ( dc, &alignpt );
                    if ( m_wndSecondary ) m_wndSecondary->Draw ( dc, &alignpt );

                    // reset colours etc.
                    dc.SetFont( normalfont );
                    dc.SetTextForeground( m_colPropFore );
                    dc.SetPen( *wxTRANSPARENT_PEN );
                #else
                    dc.SetPen( *wxTRANSPARENT_PEN );
                #endif
                }

                y += m_fontHeight+m_spacingy + 1;
            }
            else
            {
                // caption item
                //dc.DrawRectangle ( m_splitterx, y, width-m_splitterx, lh );
                y += lh;
            }

            //if ( y > last_item_bottom_y ) { parent = NULL; break; }

        ITEM_ITERATION_UVC_LOOP_END(vi_end_y)

    }

    dc.EndDrawing();
    dc.DestroyClippingRegion(); // Is this really necessary?

#if wxPG_DOUBLE_BUFFER
    dc_main.BeginDrawing();
    if ( blit_clip_rect )
        dc_main.SetClippingRegion( *blit_clip_rect );
    //wxLogDebug (wxT("  \\--> (0,%i)"),(int)final_y);
    dc_main.Blit ( 0, first_item->m_y, m_width, render_height,
        &dc, 0, 0, wxCOPY );
    dc_main.EndDrawing();
    dc_main.DestroyClippingRegion(); // Is this really necessary?

    // Need to show the custom control caret
#if wxPG_USE_CUSTOM_CONTROLS
    m_ccManager.ShowCaretBalanced();
#endif

#endif

/* // This doesn't work, custom controls or double buffer or not
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    if ( selected_painted )
    {
        if ( m_wndPrimary ) m_wndPrimary->Refresh();
        if ( m_wndSecondary ) m_wndSecondary->Refresh();
    }
#endif*/

#if __PAINT_DEBUGGING__
    wxLogDebug (wxT("  \\--> ends..."));
#endif

}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItems ( wxPGProperty* p1, wxPGProperty* p2 )
{
    if ( m_frozen ) return;
    if ( FROM_STATE(m_itemsAdded) ) PrepareAfterItemsAdded();
    if ( m_width < 10 || m_height < 10 ) return;

    int vx,vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;

    wxPG_CLIENT_DC_INIT()

    if ( m_clearThisMany )
    {
        // Need to clear some area at the end
        dc.BeginDrawing();
        wxColour& bgc = wxPG_SLACK_BACKROUND;
        dc.SetPen ( wxPen(bgc) );
        dc.SetBrush ( wxBrush(bgc) );
        dc.DrawRectangle ( 0, m_bottomy, m_width, m_clearThisMany*m_lineHeight );
        dc.EndDrawing();
        m_clearThisMany = 0;
    }

    if ( !FROM_STATE(m_properties)->GetCount() )
        return;

    if ( p1 == NULL ) return;

    int visual_bottom = vy + m_height;

    if ( p1->m_y >= visual_bottom )
        return;

    if ( p2 == NULL || p2->m_y >= visual_bottom )
    {
        p2 = DoGetItemAtY ( visual_bottom );
        //if ( !p2 ) p2 = GetLastItem ( TRUE, TRUE );
    }

    if ( p2 )
    {
        wxASSERT ( ( p2->m_y - p1->m_y - m_lineHeight ) < m_height );
    }

    //wxASSERT ( p2 != NULL );

    DoDrawItems(dc,p1,p2,NULL);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItem ( wxDC& dc, wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    // do not draw a single item if multiple pending
    if ( FROM_STATE(m_itemsAdded) )
        return;

    if ( p->m_y < 0 )
        return;

#if __PAINT_DEBUGGING__
    wxLogDebug(wxT("wxPropertyGrid::DrawItem( %s )"), p->GetLabel().c_str() );
#endif

    DoDrawItems ( dc, p, p, NULL );
}

// -----------------------------------------------------------------------

inline void wxPropertyGrid::DrawItem ( wxPGProperty* p )
{
    wxPG_CLIENT_DC_INIT()
    DrawItem(dc,p);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItemAndChildren ( wxPGProperty* p )
{
    wxPG_CLIENT_DC_INIT()
    DrawItemAndChildren(dc,p);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItemAndChildren ( wxDC& dc, wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    // do not draw a single item if multiple pending
    if ( FROM_STATE(m_itemsAdded) || p->m_y < 0 || m_frozen )
        return;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

#if __PAINT_DEBUGGING__
    wxLogDebug(wxT("wxPropertyGrid::DrawItemAndChildren( %s )"), p->GetLabel().c_str() );
#endif

    // Update child control.
    if ( m_selected && m_selected->GetParent() == pwc )
        m_selected->UpdateControl(m_wndPrimary);

    wxPGProperty* last_drawn = pwc;

    if ( pwc->GetParentingType() != 0 &&
         pwc->GetCount() &&
         pwc->m_expanded
       )
    {
        last_drawn = GetNearestPaintVisible ( pwc->Last() );
    }

    DoDrawItems ( dc, pwc, last_drawn, NULL );

}

// -----------------------------------------------------------------------

void wxPropertyGrid::Refresh( bool WXUNUSED(eraseBackground),
                              const wxRect *rect )
{
    wxWindow::Refresh(false,rect);
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    // I think this really helps only GTK+1.2
    if ( m_wndPrimary ) m_wndPrimary->Refresh();
    if ( m_wndSecondary ) m_wndSecondary->Refresh();
#endif
}

// -----------------------------------------------------------------------

/*
void wxPropertyGrid::RedrawAllVisible ()
{
    // TODO: Is this safe?
    //Update();

    if ( m_frozen || !IsShown() )
        return;

    wxPG_CLIENT_DC_INIT()

#if __PAINT_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::RedrawAllVisible()") );
#endif

    int vx,vy;                     // Top left corner of client
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    int y1 = vy;
    int y2 = y1 + m_height;

    // Repaint this rectangle
    DrawItems ( dc, y1, y2, (wxRect*) NULL );

}
*/

// -----------------------------------------------------------------------

#if wxPG_HEAVY_GFX
void wxPropertyGrid::DrawSplitterDragColumn ( wxDC&, int ) { }
#else
void wxPropertyGrid::DrawSplitterDragColumn ( wxDC& dc, int x )
{
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;

    dc.BeginDrawing();

    dc.SetLogicalFunction(wxINVERT);
    dc.DestroyClippingRegion();

    dc.SetPen( m_splitterpen );
    dc.DrawLine(x,vy,x,vy+m_height);

    dc.EndDrawing();

}
#endif

// -----------------------------------------------------------------------
// wxPropertyGrid global operations
// -----------------------------------------------------------------------

void wxPropertyGrid::Clear()
{
    //wxASSERT_MSG ( wxStrcmp(GetParent()->GetClassInfo()->GetClassName(),wxT("wxPropertyGridManager")) != 0,
    //    wxT("Don't call this in wxPropertyGridManager."));

    if ( m_selected ) ClearSelection();

    FROM_STATE(Clear());

    m_propHover = NULL;
    m_bottomy = 0;

    m_prevVY = 0;
    m_arrVisible.Empty();

    //m_catArray.Clear();

    RecalculateVirtualSize();

    // Need to clear some area at the end
    if ( !m_frozen )
    {
        wxColour& bgc = wxPG_SLACK_BACKROUND;
        wxPG_CLIENT_DC_INIT()
        dc.SetPen ( bgc );
        dc.SetBrush ( bgc );
        dc.DrawRectangle ( 0, 0, m_width, m_height );
        m_clearThisMany = 0;
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::EnableCategories ( bool enable )
{
    ClearSelection();

    if ( enable )
    {
        //
        // Enable categories
        //
        m_windowStyle &= ~(wxPG_HIDE_CATEGORIES);
    }
    else
    {
        //
        // Disable categories
        //
        m_windowStyle |= wxPG_HIDE_CATEGORIES;
    }

    if ( !m_pState->EnableCategories(enable) )
        return FALSE;

    if ( !m_frozen )
    {
        if ( m_windowStyle & wxPG_AUTO_SORT )
        {
            FROM_STATE(m_itemsAdded) = 1; // force
            PrepareAfterItemsAdded();
        }
        else
        {
            CalculateYs(NULL,-1);
            //CalculateVisibles( -1 );
        }
    }
    else
        FROM_STATE(m_itemsAdded) = 1;

    Refresh();

    return TRUE;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SwitchState ( wxPropertyGridState* pNewState )
{

    wxASSERT ( pNewState );

    wxPGProperty* oldSelection = m_selected;

    // Deselect.
    if ( m_selected )
        ClearSelection();

    m_pState->m_selected = oldSelection;

    bool orig_mode = m_pState->IsInNonCatMode();
    bool new_state_mode = pNewState->IsInNonCatMode();

    m_pState = pNewState;

    m_bottomy = 0; // This is necessary or y's won't get updated.

    // If necessary, convert state to correct mode.
    if ( orig_mode != new_state_mode )
    {
        // This should refresh as well.
        EnableCategories ( orig_mode?FALSE:TRUE );
    }
    else if ( !m_frozen )
    {
        // Refresh, if not frozen.
        if ( FROM_STATE(m_itemsAdded) )
            PrepareAfterItemsAdded();
        else
            CalculateYs(NULL,-1);

        // Reselect
        if ( FROM_STATE(m_selected) )
            SelectProperty ( FROM_STATE(m_selected), FALSE );

        //RedrawAllVisible();
        Refresh();
    }
    else
        m_pState->m_itemsAdded = 1;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Sort ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    m_pState->Sort( p );

    // Because order changed, Y's need to be changed as well
    if ( p->GetParentState() == m_pState )
        CalculateYs ( p->m_parent, p->m_arrIndex );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Sort ()
{
    ClearSelection();

    m_pState->Sort();

    CalculateYs( NULL, -1 );
}

// -----------------------------------------------------------------------

// Call to SetSplitterPosition will always disable splitter auto-centering
// if parent window is shown.
void wxPropertyGrid::SetSplitterPosition ( int newxpos, bool refresh )
{
    if ( ( /*newxpos >= 0 && */newxpos < wxPG_DRAG_MARGIN ) ||
         ( m_width > wxPG_DRAG_MARGIN && newxpos > (m_width-wxPG_DRAG_MARGIN) ) )
        return;

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::SetSplitterPosition ( %i )"), newxpos );
#endif

#if wxPG_HEAVY_GFX
    m_splitterx = newxpos;

    if ( refresh )
    {
        if ( m_selected)
            CorrectEditorWidgetSizeX( m_splitterx, m_width );

        Refresh();
        //RedrawAllVisible(); // no flicker
    }
#else
    if ( !m_dragStatus )
    {
        // Only do this if this was not a call from HandleMouseUp
        m_startingSplitterX = m_splitterx;
        m_splitterx = newxpos;
    }

    // Clear old
    if ( m_splitterprevdrawnx != -1 )
    {
        wxPG_CLIENT_DC_INIT()

        DrawSplitterDragColumn( dc, m_splitterprevdrawnx );
        m_splitterprevdrawnx = -1;
    }

    // Redraw only if drag really moved
    if ( m_splitterx != m_startingSplitterX && refresh )
    {
        if ( m_selected)
            CorrectEditorWidgetSizeX( m_splitterx, m_width );

        Update(); // This fixes a graphics-mess in wxMSW

        Refresh();
        //RedrawAllVisible(); // no flicker
    }
#endif

}

// -----------------------------------------------------------------------

void wxPropertyGrid::CenterSplitter ( bool enable_auto_centering )
{
    SetSplitterPosition ( m_width/2, TRUE );
    if ( enable_auto_centering && ( m_windowStyle & wxPG_SPLITTER_AUTO_CENTER ) )
        m_iFlags &= ~(wxPG_FL_DONT_CENTER_SPLITTER);
}

// -----------------------------------------------------------------------
// wxPropertyGrid item iteration (GetNextProperty etc.) methods
// -----------------------------------------------------------------------

// Returns nearest paint visible property (such that will be painted unless
// window is scrolled or resized). If given property is paint visible, then
// it itself will be returned
wxPGProperty* wxPropertyGrid::GetNearestPaintVisible ( wxPGProperty* p )
{
    int vx,vy1;// Top left corner of client
    GetViewStart(&vx,&vy1);
    vy1 *= wxPG_PIXELS_PER_UNIT;

    int vy2 = vy1 + m_height;

    if ( (p->m_y + m_lineHeight) < vy1 )
    {
    // Too high
        return DoGetItemAtY( vy1 );
    }
    else if ( p->m_y > vy2 )
    {
    // Too low
        return DoGetItemAtY( vy2 );
    }

    // Itself paint visible
    return p;

}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGrid::GetNeighbourItem ( wxPGProperty* item,
    bool need_visible, int dir ) const
{
    wxPGPropertyWithChildren* parent = item->m_parent;
    unsigned int indinparent = item->GetIndexInParent();

    if ( dir > 0 )
    {
        if ( item->GetChildCount() == 0 ||
             (!((wxPGPropertyWithChildren*)item)->m_expanded && need_visible) )
        {
            // current item did not have any expanded children
            if ( indinparent < (parent->GetCount()-1) )
            {
                // take next in parent's array
                item = parent->Item(indinparent+1);
            }
            else
            {
                // no more in parent's array; move up until found;
                wxPGPropertyWithChildren* p2 = parent;
                parent = parent->m_parent;
                item = (wxPGProperty*) NULL;
                while ( parent )
                {
                    if ( p2->m_arrIndex < (parent->GetCount()-1) )
                    {
                        item = parent->Item(p2->m_arrIndex+1);
                        break;
                    }
                    p2 = parent;
                    parent = parent->m_parent;
                }
            }
        }
        else
        {
            // take first of current item's children
            wxPGPropertyWithChildren* p2 = (wxPGPropertyWithChildren*)item;
            item = p2->Item(0);
            //indinparent = 0;
        }
    }
    else
    {

        // items in array left?
        if ( indinparent > 0 )
        {
            // take prev in parent's array
            item = parent->Item(indinparent-1);
            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)item;

            // Recurse to it's last child
            while ( item->GetParentingType() != 0 && pwc->GetCount() &&
                    ( pwc->m_expanded || !need_visible )
                  )
            {
                item = pwc->Last();
                pwc = (wxPGPropertyWithChildren*)item;
            }

        }
        else
        {
            // If we were at first, go to parent
            item = parent;
        }
    }

    if ( item == FROM_STATE(m_properties) )
        return (wxPGProperty*) NULL;

    // If item was hidden and need_visible, get next.
    if ( (m_iFlags & wxPG_FL_HIDE_STATE) && need_visible && item )
    {
        if ( item->m_flags & wxPG_PROP_HIDEABLE )
        {
            // Speed-up: If parent is hidden as well, then skip to last child or to itself
            if ( parent->m_flags & wxPG_PROP_HIDEABLE )
            {
                item = parent; // if dir up
                if ( dir > 0 )
                    item = parent->Last(); // if dir down
            }

            return GetNeighbourItem ( item, need_visible, dir );
        }
    }

    return item;
}

// -----------------------------------------------------------------------

// This is used in DoDrawItems.
wxPGProperty* wxPropertyGrid::GetLastItem ( bool need_visible, bool allow_subprops )
{
    if ( FROM_STATE(m_properties)->GetCount() < 1 )
        return (wxPGProperty*) NULL;

    wxPGProperty* p = FROM_STATE(m_properties)->Last();

    int parenting = p->GetParentingType();

    while ( parenting != 0 && ( allow_subprops || parenting > 0 ) )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        parenting = 0;
        if ( pwc->GetCount() )
        {

            if ( (!need_visible || pwc->m_expanded) )
            {
                p = pwc->Last();
                parenting = p->GetParentingType();
            }
            else
                parenting = 0;
        }
    }

    // If item was hidden and need_visible, get previous.
    if ( (m_iFlags & wxPG_FL_HIDE_STATE) && need_visible &&
         p && ( p->m_flags & wxPG_PROP_HIDEABLE )
       )
        return GetNeighbourItem ( p, need_visible, -1 );

    return p;
}

// -----------------------------------------------------------------------
// Methods related to change in value, value modification and sending events
// -----------------------------------------------------------------------

// commits any changes in editor of selected property
// return true if DoPropertyChanged was called
bool wxPropertyGrid::CommitChangesFromEditor()
{
    //wxLogDebug(wxT("wxPropertyGrid::CommitChangesFromEditor"));

    if ( m_wndPrimary && IsEditorsValueModified() &&
         (m_iFlags & wxPG_FL_INITIALIZED) )
    {
        wxASSERT( m_selected );

        // Save value (only if truly modified).
        if ( !m_selected->GetEditorClass()->CopyValueFromControl( m_selected, m_wndPrimary ) )
            EditorsValueWasNotModified();

        wxClientDC dc(this);
        DoPropertyChanged ( dc, m_selected );

        return true;
    }

    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoPropertyChanged ( wxDC& dc, wxPGProperty* p )
{

    if ( m_processingEvent )
        return;

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("wxPropertyGrid::DoPropertyChanged( %s )"),p->GetLabel().c_str());
#endif

    m_pState->m_anyModified = 1;

    wxPropertyGridEvent evt( wxEVT_PG_CHANGED, GetId() );
    evt.SetPropertyGrid (this);
    evt.SetEventObject (this);

    m_processingEvent = 1;

    // No longer unspecified.
    m_pState->ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);

    if ( m_iFlags & wxPG_FL_VALUE_MODIFIED )
    {
        m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);

        // Set as Modified (not if dragging just began)
        if ( !(p->m_flags & wxPG_PROP_MODIFIED) )
        {
            p->m_flags |= wxPG_PROP_MODIFIED;
            if ( p == m_selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
            {
                if ( m_wndPrimary )
                    SetCurControlBoldFont();
            }
        }

        wxPGProperty* curChild = p;
        wxPGPropertyWithChildren* curParent = p->m_parent;

        // Also update parent(s), if any
        // (but not if its wxCustomProperty)

        while ( curParent &&
                curParent->GetParentingType() < 0 &&
                wxStrcmp(curParent->GetClassName(),wxT("wxCustomProperty")) != 0 )
        {
            // Set as Modified
            if ( !(curParent->m_flags & wxPG_PROP_MODIFIED) )
            {
                curParent->m_flags |= wxPG_PROP_MODIFIED;
                if ( curParent == m_selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
                {
                    if ( m_wndPrimary )
                        SetCurControlBoldFont();
                }
            }

            curParent->ChildChanged ( curChild );

            DrawItem ( dc, curParent );

            curChild = curParent;
            curParent = curParent->GetParent();
        }

        // Draw the actual property
        if ( ( p != m_selected ) || !m_wndPrimary ||
             ( p->GetParentingType() < 0 ) ||
             ( p->m_flags & wxPG_PROP_CUSTOMIMAGE )
           )
        {
            DrawItemAndChildren ( dc, p );
        }

        if ( curChild != p )
            m_pState->ClearPropertyAndChildrenFlags(curChild,wxPG_PROP_UNSPECIFIED);

        // Call wx event handler for property (or its topmost parent, but only
        // when dealign with legitemate sub-properties - see above).
        evt.SetProperty( curChild );

        // Maybe need to update control
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
        if ( m_wndPrimary ) m_wndPrimary->Refresh();
        if ( m_wndSecondary ) m_wndSecondary->Refresh();
#endif

        GetEventHandler()->AddPendingEvent(evt);

    }

    m_processingEvent = 0;

}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnCustomEditorEvent ( wxEvent &event )
{
    wxASSERT ( m_selected );

    bool res1, res2;

    // First call editor class' event handler.
    res1 = m_selected->GetEditorClass()->OnEvent ( this, m_selected, m_wndPrimary, event );

    // Then the property's custom handler (must be always called).
    res2 = m_selected->OnEvent ( this, m_wndPrimary, event );

    if ( res1 || res2 )
    {
        // Setting this is not required if res was TRUE, so we do it now.
        m_iFlags |= wxPG_FL_VALUE_MODIFIED;

        wxPG_CLIENT_DC_INIT()

        DoPropertyChanged(dc,m_selected);

        /*
        if ( !m_wndPrimary || m_selected->GetParentingType() < 0 ||
             (m_selected->m_flags & wxPG_PROP_CUSTOMIMAGE)
           )
            DrawItemAndChildren ( dc, m_selected );
        */

    }
}

// -----------------------------------------------------------------------

// When a property's value was modified internally (using SetValueFromString
// or SetValueFromInt, for example), then this should be called afterwards.
void wxPropertyGrid::PropertyWasModified ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    wxPG_CLIENT_DC_INIT()
    EditorsValueWasModified();
    DoPropertyChanged(dc,p);
}

// -----------------------------------------------------------------------
// wxPropertyGrid editor control helper methods
// -----------------------------------------------------------------------

// inline because it is used exactly once in the code
inline wxRect wxPropertyGrid::GetEditorWidgetRect ( wxPGProperty* p )
{
    //wxASSERT ( p->m_y >= 0 ); // item is not visible

    int itemy = p->m_y;
#if wxCC_CORRECT_CONTROL_POSITION || !wxPG_USE_CUSTOM_CONTROLS
    int vx,vy;// Top left corner of client
    GetViewStart(&vx,&vy);
    vy *= wxPG_PIXELS_PER_UNIT;
#else
    //const int vx = 0;
    const int vy = 0;
#endif
    int cust_img_space = 0;

    //m_iFlags &= ~(wxPG_FL_CUR_USES_CUSTOM_IMAGE);

    // TODO: If custom image detection changes from current, change this.
    if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE /*p->m_flags & wxPG_PROP_CUSTOMIMAGE*/ )
    {
        //m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;
        int imwid = p->GetImageSize().x;
        if ( imwid < 1 ) imwid = wxPG_CUSTOM_IMAGE_WIDTH;
        cust_img_space = imwid + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
    }

    return wxRect
      (
        m_splitterx+cust_img_space+wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1,
        itemy-vy,
        m_width-m_splitterx-wxPG_XBEFOREWIDGET-wxPG_CONTROL_MARGIN-cust_img_space-1,
        m_lineHeight-1
      );
}

// -----------------------------------------------------------------------

// return size of custom paint image
wxSize wxPropertyGrid::GetImageSize ( wxPGId id ) const
{
    if ( id.IsOk() )
    {
        wxSize cis = wxPGIdToPtr(id)->GetImageSize();

        if ( cis.x < 0 )
        {
            /*
            if ( cis.x == -1 )
                cis.x = wxPG_CUSTOM_IMAGE_WIDTH;
            else
                cis.x = -cis.x;
            */
            if ( cis.x <= -1 )
                cis.x = wxPG_CUSTOM_IMAGE_WIDTH;
        }
        if ( cis.y <= 0 )
        {
            if ( cis.y >= -1 )
                cis.y = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
            else
                cis.y = -cis.y;
        }
        return cis;
    }
    return wxSize(wxPG_CUSTOM_IMAGE_WIDTH,wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight));
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CorrectEditorWidgetSizeX ( int new_splitterx, int new_width )
{
    wxASSERT ( m_selected );

    int sec_wid = 0;

    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;
    int propY = m_selected->m_y - vy;

    if ( m_wndSecondary )
    {
        // if width change occurred, move secondary wnd by that amount
        wxRect r = m_wndSecondary->GetRect();
        int adjust = r.y % wxPG_PIXELS_PER_UNIT;
        if ( adjust > (wxPG_PIXELS_PER_UNIT/2) )
            adjust = adjust - wxPG_PIXELS_PER_UNIT;
        int y = propY + adjust;
        sec_wid = r.width;

        m_wndSecondary->Move ( new_width-r.width,y );

    #if !wxPG_USE_CUSTOM_CONTROLS
        // if primary is textctrl, then we have to add some extra space
        if ( m_wndPrimary && m_wndPrimary->IsKindOf(CLASSINFO(wxTextCtrl)) )
            sec_wid += wxPG_TEXTCTRL_AND_BUTTON_SPACING;
    #endif
    }

    if ( m_wndPrimary )
    {
        wxRect r = m_wndPrimary->GetRect();
        int adjust = r.y % wxPG_PIXELS_PER_UNIT;
        if ( adjust > (wxPG_PIXELS_PER_UNIT/2) )
            adjust = adjust - wxPG_PIXELS_PER_UNIT;
        int y = propY + adjust;

    #if !wxPG_USE_CUSTOM_CONTROLS
        const int ctrlXAdjust = m_ctrlXAdjust;
    #else

        int cust_img_space = 0;
        if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE )
        {
            int imwid = m_selected->GetImageSize().x;
            if ( imwid < 1 ) imwid = wxPG_CUSTOM_IMAGE_WIDTH;
            cust_img_space = imwid + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
        }

        const int ctrlXAdjust = 1 + cust_img_space;
    #endif

        m_wndPrimary->SetSize(
            //new_splitterx+cust_img_space+wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1,
            new_splitterx+ctrlXAdjust,
            y,
            new_width-(new_splitterx+ctrlXAdjust)-sec_wid,
            r.height
        );
    }

#if !wxPG_USE_CUSTOM_CONTROLS
    if ( m_wndSecondary )
        m_wndSecondary->Refresh();
#endif

}

// -----------------------------------------------------------------------

/*void wxPropertyGrid::CorrectEditorWidgetSizeY( int cy )
{
    if ( m_selected )
    {
        wxPoint cp(0,cy);

        if ( m_wndPrimary )
            m_wndPrimary->Move ( m_wndPrimary->GetPosition() + cp );

        if ( m_wndSecondary )
            m_wndSecondary->Move ( m_wndSecondary->GetPosition() + cp );
    }
}*/

// -----------------------------------------------------------------------

// takes scrolling into account
void wxPropertyGrid::ImprovedClientToScreen ( int* px, int* py )
{
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;
    vx*=wxPG_PIXELS_PER_UNIT;
    *px -= vx;
    *py -= vy;
    ClientToScreen ( px, py );
}

// -----------------------------------------------------------------------

// custom set cursor
void wxPropertyGrid::CustSetCursor ( int type )
{
    if ( type == m_curcursor ) return;

    //wxLogDebug ( wxT("wxPropertyGrid::CustSetCursor(%i)"),type);

    wxCursor* cursor = &wxPG_DEFAULT_CURSOR;
    //wxCursor* cursor = wxSTANDARD_CURSOR;
    //wxCursor* cursor = &wxNullCursor;

    if ( type == wxCURSOR_SIZEWE )
        cursor = m_cursor_sizewe;

    SetCursor ( *cursor );

    //if ( m_wndPrimary ) m_wndPrimary->SetCursor(wxNullCursor);

    m_curcursor = type;
}

// -----------------------------------------------------------------------
// wxPropertyGrid property selection
// -----------------------------------------------------------------------

#define CONNECT_CHILD(EVT,FUNCTYPE,FUNC) \
    wnd->Connect(id, EVT, \
        (wxObjectEventFunction) (wxEventFunction)  \
        FUNCTYPE (&wxPropertyGrid::FUNC), \
        NULL, this );

// Setups event handling for child control
#if !wxPG_USE_CUSTOM_CONTROLS
void wxPropertyGrid::SetupEventHandling ( wxWindow* arg_wnd, int id )
{
    wxWindow* wnd = arg_wnd;

#if wxPG_ENABLE_CLIPPER_WINDOW
    // Pass real control instead of clipper window
    if ( wnd->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
    {
        wnd = ((wxPGClipperWindow*)arg_wnd)->GetControl();
    }
#endif

    if ( wnd == m_wndPrimary )
    {
        CONNECT_CHILD(wxEVT_MOTION,(wxMouseEventFunction),OnMouseMoveChild)
        CONNECT_CHILD(wxEVT_LEFT_UP,(wxMouseEventFunction),OnMouseUpChild)
        CONNECT_CHILD(wxEVT_LEFT_DOWN,(wxMouseEventFunction),OnMouseClickChild)
        CONNECT_CHILD(wxEVT_RIGHT_UP,(wxMouseEventFunction),OnMouseRightClickChild)
        CONNECT_CHILD(wxEVT_ENTER_WINDOW,(wxMouseEventFunction),OnMouseEntry)
        CONNECT_CHILD(wxEVT_LEAVE_WINDOW,(wxMouseEventFunction),OnMouseEntry)
        CONNECT_CHILD(wxEVT_KEY_DOWN,(wxCharEventFunction),OnKeyChild)
    }
    CONNECT_CHILD(wxEVT_SET_FOCUS,(wxFocusEventFunction),OnFocusEvent)
    CONNECT_CHILD(wxEVT_KILL_FOCUS,(wxFocusEventFunction),OnFocusEvent)
}
#else
void wxPropertyGrid::SetupEventHandling ( wxPGCtrlClass*, int ) { }
#endif // !wxPG_USE_CUSTOM_CONTROLS

// Call with NULL to just deactivate any active editor
void wxPropertyGrid::SelectProperty (wxPGProperty* p,
                                     bool focus,
                                     bool forceswitch,
                                     bool nonvisible)
{

    /*if ( m_prevSelected )
    {
        delete m_prevSelected;
        m_prevSelected = (wxPGCtrlClass*) NULL;
    }*/

#if __INTENSE_DEBUGGING__
    if (p)
        wxLogDebug(wxT("SelectProperty( %s (%s[%i]) )"),p->m_label.c_str(),
            p->m_parent->m_label.c_str(),p->GetIndexInParent());
    else
        wxLogDebug(wxT("SelectProperty( NULL, -1 )"));;
#endif

    //
    // If we are frozen, then just set the values.
    if ( m_frozen )
    {
        m_editorFocused = 0;
        m_selected = p;
        FROM_STATE(m_selected) = p;

        // If frozen, always free controls. But don't worry, as Thaw will
        // recall SelectProperty to recreate them.
        if ( m_wndSecondary )
        {
        #if wxPG_USE_CUSTOM_CONTROLS
            delete m_wndSecondary;
        #else
            m_wndSecondary->Destroy();
        #endif
            m_wndSecondary = NULL;
        }

        if ( m_wndPrimary )
        {
        #if wxPG_USE_CUSTOM_CONTROLS
            delete m_wndPrimary;
        #else
            m_wndPrimary->Destroy();
            //m_wndPrimary->Hide();
            //m_prevSelected = m_wndPrimary;
        #endif
            m_wndPrimary = NULL;
        }

        p = (wxPGProperty*) NULL;

    }
    else
    {

        // Is it the same?
        if ( m_selected == p && !forceswitch )
        {
            // Only set focus if not deselecting
            if ( p )
            {
                if ( focus )
                {
                    if ( m_wndPrimary )
                    {
                        m_wndPrimary->SetFocus ();
                        m_editorFocused = 1;
                    }
                }
                else
                {
                #if wxPG_USE_CUSTOM_CONTROLS
                    if ( m_wndPrimary )
                        m_wndPrimary->RemoveFocus ();
                #else
                    wxScrolledWindow::SetFocus();
                #endif
                    m_editorFocused = 0;
                }
            }

            return;
        }

        wxPG_CLIENT_DC_INIT()

        //
        // First, deactivate previous
        if ( m_selected )
        {

    #if __INTENSE_DEBUGGING__
            wxLogDebug (wxT("  (closing previous (%s))"), m_selected->m_label.c_str() );
    #endif

            wxPGProperty* prev = m_selected;

            // Must double-check if this is an selected in case of forceswitch
            if ( p != prev )
                CommitChangesFromEditor();

            if ( m_wndSecondary )
            {
            #if wxPG_USE_CUSTOM_CONTROLS
                delete m_wndSecondary;
            #else
                m_wndSecondary->Destroy();
            #endif
                m_wndSecondary = NULL;
            }

            if ( m_wndPrimary )
            {
            #if wxPG_USE_CUSTOM_CONTROLS
                delete m_wndPrimary;
            #else
                m_wndPrimary->Destroy();
                //m_wndPrimary->Hide();
                //m_prevSelected = m_wndPrimary;
            #endif
                m_wndPrimary = NULL;
            }

            m_iFlags &= ~(wxPG_FL_SELECTED_IS_PAINT_FLEXIBLE);
            m_selected = (wxPGProperty*) NULL;
            FROM_STATE(m_selected) = (wxPGProperty*) NULL;

            // Make sure the previous selection is refreshed
            if ( prev->m_y < (int)m_bottomy )
                DoDrawItems ( dc, prev, prev, NULL );

            m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);
        }

        //
        // Then, activate the one given.
        if ( p )
        {

            m_iFlags &= ~(wxPG_FL_NAVIGATING_OUT);

            m_editorFocused = 0;
            m_selected = p;
            FROM_STATE(m_selected) = p;
            m_iFlags |= wxPG_FL_PRIMARY_FILLS_ENTIRE;

            m_wndPrimary = (wxPGCtrlClass*) NULL;

            //
            // Only create editor for non-disabled non-caption
            if ( p->GetParentingType() <= 0 && !(p->m_flags & wxPG_PROP_DISABLED) )
            {
            // do this for non-caption items

                // Do we need to paint the custom image, if any?
                m_iFlags &= ~(wxPG_FL_CUR_USES_CUSTOM_IMAGE);
                if ( (p->m_flags & wxPG_PROP_CUSTOMIMAGE) &&
                     !p->GetEditorClass()->CanContainCustomImage()
                   )
                    m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;

                wxRect grect = GetEditorWidgetRect(p);
                wxPoint good_pos = grect.GetPosition();
            #if wxPG_CREATE_CONTROLS_HIDDEN && !wxPG_USE_CUSTOM_CONTROLS
                int coord_adjust = m_height - good_pos.y;
                good_pos.y += coord_adjust;
            #endif

                const wxPGEditor* editor = p->GetEditorClass();
                wxASSERT_MSG(editor,
                    wxT("NULL editor class not allowed"));

                m_wndPrimary = editor->CreateControls(this,
                                                      p,
                                                      good_pos,
                                                      grect.GetSize(),
                                                      &m_wndSecondary);

                // NOTE: It is allowed for m_wndPrimary to be NULL - in this case
                //       value is drawn as normal, and m_wndSecondary is assumed
                //       to be a right-aligned button that triggers a separate editor
                //       window.

                if ( m_wndPrimary )
                {

                    // If it has modified status, use bold font
                    // (must be done before capturing m_ctrlXAdjust)
                    if ( (p->m_flags & wxPG_PROP_MODIFIED) && (m_windowStyle & wxPG_BOLD_MODIFIED) )
                        SetCurControlBoldFont();

                #if !wxPG_USE_CUSTOM_CONTROLS

                    // Store x relative to splitter (we'll need it).
                    m_ctrlXAdjust = m_wndPrimary->GetPosition().x - m_splitterx;

                    // Correct cursor.
                    //m_wndPrimary->SetCursor(wxNullCursor);
                #endif

                    //ClearValueBackground ( dc, p );

                    // Check if background clear is not necessary
                    // FIXME: Reverse conditional (as would be logical)
                    wxPoint pos = m_wndPrimary->GetPosition();
                    if ( pos.x <= (m_splitterx+1) && pos.y <= p->m_y )
                    {
                    }
                    else
                    {
                        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
                    }

                #if !wxPG_USE_CUSTOM_CONTROLS
                    m_wndPrimary->SetSizeHints(3,3);
                #endif

                #if wxPG_CREATE_CONTROLS_HIDDEN && !wxPG_USE_CUSTOM_CONTROLS
                    m_wndPrimary->Show(FALSE);
                    m_wndPrimary->Freeze();

                    good_pos = m_wndPrimary->GetPosition();
                    good_pos.y -= coord_adjust;
                    m_wndPrimary->Move ( good_pos );
                #endif

                #if !wxPG_USE_CUSTOM_CONTROLS
                    SetupEventHandling(m_wndPrimary,wxPG_SUBID1);
                #endif

                /*#if !wxPG_USE_CUSTOM_CONTROLS
                    // Center the control vertically.
                    int y_adj = (m_lineHeight - m_wndPrimary->GetSize().y)/2;
                    //wxLogDebug(wxT("lh:%i  y:%i"),m_lineHeight,m_wndPrimary->GetSize().y);
                    if ( y_adj )
                    {
                        wxPoint pt(m_wndPrimary->GetPosition());
                        if ( pt.y == p->m_y )
                        {
                            pt.y += y_adj;
                            m_wndPrimary->Move(pt);
                        }
                    }
                #endif*/
                    // Focus and select all (wxTextCtrl and wxComboBox only)
                    if ( focus )
                    {
                        wxPGCtrlClass* ctrl = m_wndPrimary;
                        ctrl->SetFocus();

                    #if wxPG_NAT_TEXTCTRL_BORDER_ANY
                        // Take into account textctrl in clipper window
                        if ( ctrl->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
                            ctrl = ((wxPGClipperWindow*)ctrl)->GetControl();
                    #endif

                        if ( ctrl->IsKindOf(CLASSINFO(wxCCustomTextCtrl)) )
                            wxStaticCast(ctrl,wxCCustomTextCtrl)->SetSelection(-1,-1);
                    }
                }

                if ( m_wndSecondary )
                {

                #if !wxPG_USE_CUSTOM_CONTROLS
                    m_wndSecondary->SetSizeHints(3,3);
                #endif

                #if wxPG_CREATE_CONTROLS_HIDDEN && !wxPG_USE_CUSTOM_CONTROLS
                    wxRect sec_rect = m_wndSecondary->GetRect();
                    sec_rect.y -= coord_adjust;

                    // Fine tuning required to fix "oversized"
                    // button disappearance bug.
                    if ( sec_rect.y < 0 )
                    {
                        sec_rect.height += sec_rect.y;
                        sec_rect.y = 0;
                    }
                    m_wndSecondary->SetSize ( sec_rect );
                #endif

                #if !wxPG_USE_CUSTOM_CONTROLS
                    SetupEventHandling(m_wndSecondary,wxPG_SUBID2);
                #endif

                    // If no primary editor, focus to button to allow
                    // it to interprete ENTER etc.
                    if ( focus && !m_wndPrimary )
                        m_wndSecondary->SetFocus();

                }

    //#if wxMINOR_VERSION < 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER < 3 )
                if ( focus )
                    m_editorFocused = 1;
    //#endif

            }
            else
            {
                // wxGTK atleast seems to need this (wxMSW not)
                SetFocus();
            }

            wxSize imsz = p->GetImageSize();
            if ( imsz.y < -1 )
                m_iFlags |= wxPG_FL_SELECTED_IS_PAINT_FLEXIBLE;

            m_iFlags &= ~(wxPG_FL_VALUE_MODIFIED);

            //Update();

            // If it's inside collapsed section, expand parent, scroll, etc.
            // Also, if it was partially visible, scroll it into view.
            int vx, vy;
            GetViewStart(&vx,&vy);
            vy*=wxPG_PIXELS_PER_UNIT;
            int vy2 = vy + m_height;

            //wxLogDebug(wxT("y=%i, vy=%i, bottom=%i"),p->m_y,vy,(vy+m_height));

            if ( (p->m_y < vy ||
                  (p->m_y <= vy2 &&
                   (p->m_y+m_lineHeight) > vy2)) &&
                 !nonvisible )
                EnsureVisible ( wxPGIdGen(p) );

            if ( m_wndPrimary )
            {
            #if !wxPG_USE_CUSTOM_CONTROLS
                // Clear its background
                // FIXME: This should not be necessary, since DoDrawItems is called,
                // but it is. Probably due to bug in DoDrawItems.
                if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) )
                {
                    dc.SetPen(*wxTRANSPARENT_PEN);
                    dc.SetBrush( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) );
                    dc.DrawRectangle(m_splitterx+1,p->m_y,
                                     m_width-m_splitterx,m_lineHeight-1);
                }
            #endif

            #if wxPG_CREATE_CONTROLS_HIDDEN && !wxPG_USE_CUSTOM_CONTROLS
                m_wndPrimary->Thaw();
                m_wndPrimary->Show(TRUE);
            #endif
            }

            DoDrawItems ( dc, p, p, (const wxRect*) NULL );

        }
    }

#if wxUSE_STATUSBAR

    //
    // Show help text in status bar
    //   (if found and grid not embedded in manager with help box).
    //

    wxStatusBar* statusbar = (wxStatusBar*) NULL;
    if ( !(m_iFlags & wxPG_FL_NOSTATUSBARHELP) )
    {
        wxFrame* frame = wxDynamicCast(::wxGetTopLevelParent(this),wxFrame);
        if ( frame )
            statusbar = frame->GetStatusBar();
    }

    if ( statusbar )
    {
        const wxString* pHelpString = (const wxString*) NULL;

        if ( p )
        {
            pHelpString = &p->m_helpString;
            if ( pHelpString->length() )
            {
                // Set help box text.
                statusbar->SetStatusText ( *pHelpString );
                m_iFlags |= wxPG_FL_STRING_IN_STATUSBAR;
            }
        }

        if ( (!pHelpString || !pHelpString->length()) &&
             (m_iFlags & wxPG_FL_STRING_IN_STATUSBAR) )
        {
            // Clear help box - but only if it was written
            // by us at previous time.
            statusbar->SetStatusText ( wxEmptyString );
            m_iFlags &= ~(wxPG_FL_STRING_IN_STATUSBAR);
        }
    }
#endif

    // call wx event handler (here so that deselecting gets processed as well)
    wxPropertyGridEvent evt( wxEVT_PG_SELECTED, GetId() );
    evt.SetPropertyGrid (this);
    evt.SetEventObject (this);
    evt.SetProperty (m_selected);
    GetEventHandler()->AddPendingEvent(evt);

}

// -----------------------------------------------------------------------

// This method is not inline because it called dozens of times
// (i.e. two-arg function calls create smaller code size).
void wxPropertyGrid::ClearSelection ()
{
    SelectProperty((wxPGProperty*)NULL,FALSE);
}

// -----------------------------------------------------------------------
// wxPropertyGrid expand/collapse state and priority (compact mode) related
// -----------------------------------------------------------------------

bool wxPropertyGrid::Collapse ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return FALSE;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return FALSE;

    if ( !pwc->m_expanded ) return FALSE;

    //wxLogDebug("COLLAPSE(%i)",index);

    // If active editor was inside collapsed section, then disable it
    if ( m_selected && m_selected->IsSomeParent (p) )
    {
        ClearSelection();
    }

    // Store dont-center-splitter flag 'cause we need to temporarily set it
    wxUint32 old_flag = m_iFlags & wxPG_FL_DONT_CENTER_SPLITTER;
    m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 0;

    // Redraw etc. only if collapsed was visible.
    if (pwc->m_y >= 0 &&
        !m_frozen &&
        ( pwc->GetParentingType() != 1 || !(m_windowStyle & wxPG_HIDE_CATEGORIES) ) )
    {
        /*int y_adjust = 0;

        if ( m_selected && m_selected->m_y > pwc->m_y )
        {
            wxPGProperty* next_vis = GetNeighbourItem(pwc,TRUE,1);
            wxASSERT ( next_vis );

            y_adjust = next_vis->m_y - pwc->m_y - m_lineHeight;
        }*/

        CalculateYs( pwc->m_parent, pwc->m_arrIndex );

        // Fix control position.
        /*if ( y_adjust )
            CorrectEditorWidgetSizeY ( -y_adjust );*/

        // When item is collapsed so that scrollbar would move,
        // graphics mess is about (unless we redraw everything).
        Refresh();
    }

    // Clear dont-center-splitter flag if it wasn't set
    m_iFlags = m_iFlags & ~(wxPG_FL_DONT_CENTER_SPLITTER) | old_flag;

    return TRUE;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Expand ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return FALSE;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return FALSE;

    if ( pwc->m_expanded ) return FALSE;

    //wxLogDebug("EXPAND(%i)",index);

    // Store dont-center-splitter flag 'cause we need to temporarily set it
    wxUint32 old_flag = m_iFlags & wxPG_FL_DONT_CENTER_SPLITTER;
    m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 1;

    // Redraw etc. only if expanded was visible.
    if ( pwc->m_y >= 0 && !m_frozen &&
         ( pwc->GetParentingType() != 1 || !(m_windowStyle & wxPG_HIDE_CATEGORIES) )
       )
    {
        CalculateYs( pwc->m_parent, pwc->m_arrIndex );

        /*int y_adjust = pwc->GetCount()*m_lineHeight;

        // Fix widget position as well
        if ( m_selected && m_selected->m_y > pwc->m_y )
            CorrectEditorWidgetSizeY ( y_adjust );*/

        // Redraw
    #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
        Refresh();
    #else
        wxPG_CLIENT_DC_INIT_R(TRUE)
        DrawItems( dc, pwc->m_y, m_bottomy );
    #endif
    }

    // Clear dont-center-splitter flag if it wasn't set
    m_iFlags = m_iFlags & ~(wxPG_FL_DONT_CENTER_SPLITTER) | old_flag;

    return TRUE;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Compact ( bool compact )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::Compact()") );
#endif
    if ( compact )
    {
        if ( !(m_iFlags & wxPG_FL_HIDE_STATE) )
        {
            // Deselect selected if it was hideable
            if ( m_selected && ( m_selected->m_flags & wxPG_PROP_HIDEABLE ) )
            {
                ClearSelection();
            }

            m_iFlags |= wxPG_FL_HIDE_STATE;

            if ( !m_frozen )
            {
                CalculateYs( NULL, -1 );
                RedrawAllVisible();
            }
        }
    }
    else
    {
        if ( m_iFlags & wxPG_FL_HIDE_STATE )
        {

            m_iFlags &= ~(wxPG_FL_HIDE_STATE);

            if ( !m_frozen )
            {
                CalculateYs( NULL, -1 );
                RedrawAllVisible();
            }
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyPriority ( wxPGId id, int priority )
{
    if ( wxPGIdIsOk(id) )
    {
        wxPGProperty* p = wxPGIdToPtr(id);

        if ( (m_iFlags & wxPG_FL_HIDE_STATE) && m_selected &&
             ( m_selected == p || m_selected->IsSomeParent(p) )
           )
            ClearSelection();

        m_pState->SetPropertyPriority(p,priority);

        if ( m_iFlags & wxPG_FL_HIDE_STATE )
        {
            CalculateYs(NULL,-1);
            RedrawAllVisible();
        }
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid size related methods
// -----------------------------------------------------------------------

// This is called by CalculateYs (so those calling it won't need to call this)
void wxPropertyGrid::RecalculateVirtualSize()
{

    int x = m_width;
    int y = m_bottomy;

    //wxLogDebug(wxT("RecalculateVirtualSize(%i,%i)"),x,y);

	SetVirtualSize(x, y);

	PGAdjustScrollbars(y);

    int width, height;
    GetClientSize(&width,&height);

    if ( m_selected && width != m_width )
    {
        CorrectEditorWidgetSizeX( m_splitterx, width );
    }

    m_width = width;
    m_height = height;

    //GetVirtualSize(&x, &y);
    //wxLogDebug(wxT("virtual_height = %i"),(int)y);

}

// -----------------------------------------------------------------------

// Adjust scrollbars.
void wxPropertyGrid::PGAdjustScrollbars ( int y )
{
    //wxLogDebug(wxT("AdjustScrollbars"));

	y += wxPG_PIXELS_PER_UNIT+2; // One more scrollbar unit + 2 pixels.
    int y_amount = y/wxPG_PIXELS_PER_UNIT;

    int y_pos = GetScrollPos( wxVERTICAL );
    SetScrollbars( 0, wxPG_PIXELS_PER_UNIT, 0,
		           y_amount, 0, y_pos, TRUE );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnResize ( wxSizeEvent& event )
{

    if ( !(m_iFlags & wxPG_FL_INITIALIZED) )
        return;

    if ( FROM_STATE(m_itemsAdded) && !m_frozen )
        PrepareAfterItemsAdded();

    int width, height;
    GetClientSize(&width,&height);

#if __INTENSE_DEBUGGING__
    wxLogDebug (wxT("wxPropertyGrid::OnResize ( %i, %i )"),width,height);
#endif

    int old_width = m_width;
    int old_height = m_height;
    int old_splitterx = m_splitterx;

    m_width = width;
    m_height = height;
    bool sb_vis_toggled = FALSE;

    // Determine if scrollbar appeared or disappeared.
    if ( abs(event.GetSize().x-width) >= wxPG_MIN_SCROLLBAR_WIDTH )
    {
        // There is a scrollbar.
        if ( !(m_iFlags & wxPG_FL_SCROLLBAR_DETECTED) )
        {
            //wxLogDebug(wxT("Scrollbar Appeared"));
            sb_vis_toggled = TRUE;
            m_iFlags |= wxPG_FL_SCROLLBAR_DETECTED;
        }
    }
    else if ( m_iFlags & wxPG_FL_SCROLLBAR_DETECTED )
    {
        //wxLogDebug(wxT("Scrollbar Disappeared"));
        sb_vis_toggled = TRUE;
        m_iFlags &= ~(wxPG_FL_SCROLLBAR_DETECTED);
    }

#if wxPG_DOUBLE_BUFFER
    int dblh = (m_lineHeight*2);
    if ( !m_doubleBuffer )
    {
        // Create double buffer bitmap to draw on, if none
        int w = (width>250)?width:250;
        int h = height + dblh;
        h = (h>400)?h:400;
        m_doubleBuffer = new wxBitmap ( w, h );
    #if wxPG_USE_CUSTOM_CONTROLS
        m_ccManager.SetDoubleBuffer ( m_doubleBuffer );
    #endif
    }
    else
    {
        int w = m_doubleBuffer->GetWidth();
        int h = m_doubleBuffer->GetHeight();

        // Double buffer must be large enough
        if ( w < width || h < (height+dblh) )
        {
            if ( w < width ) w = width;
            if ( h < (height+dblh) ) h = height + dblh;
            delete m_doubleBuffer;
            m_doubleBuffer = new wxBitmap ( w, h );
        #if wxPG_USE_CUSTOM_CONTROLS
            m_ccManager.SetDoubleBuffer ( m_doubleBuffer );
        #endif
        }
    }
    //wxLogMessage(wxT("%i,%i"),(int)m_doubleBuffer->GetWidth(),(int)m_doubleBuffer->GetHeight());
#endif

    //
    // Center splitter when...
    // * always when propgrid not shown yet or its full size is not realized yet
    // * auto-centering is enabled and scrollbar was not toggled
    //

    // Need to center splitter?
    if ( width!=old_width )
    {

        //wxLogDebug(wxT("About to SetSplitterPosition(%i)"),width);
        if ( !(m_iFlags & wxPG_FL_GOOD_SIZE_SET) &&
             width > 90 )
        {
            //wxLogDebug(wxT("SetSplitterPositionGS(%i)"),width);
            SetSplitterPosition( width / 2, FALSE );
        }
        else if ( !(m_iFlags & wxPG_FL_DONT_CENTER_SPLITTER) &&
                  (!sb_vis_toggled||!(m_iFlags & wxPG_FL_GOOD_SIZE_SET)) )
        {
            //wxLogDebug(wxT("SetSplitterPosition(%i)"),width);
            SetSplitterPosition( width / 2, FALSE );
            /*m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;
            if ( m_windowStyle & wxPG_SPLITTER_AUTO_CENTER )
                m_iFlags &= ~(wxPG_FL_DONT_CENTER_SPLITTER);*/
        }
        // Splitter must remain visible! Move it if necessary.
        else if ( (m_splitterx + wxPG_DRAG_MARGIN) > width )
        {
            //unsigned int old_flags = m_iFlags;
            SetSplitterPosition( width - wxPG_DRAG_MARGIN - 1, FALSE );
            // retain wxPG_FL_DONT_CENTER_SPLITTER
            //m_iFlags = (m_iFlags & ~(wxPG_FL_DONT_CENTER_SPLITTER)) | (old_flags&wxPG_FL_DONT_CENTER_SPLITTER);
        }

        // if now seen as visible, then consider that
        // "good size" has been set
        /*if ( ::wxGetTopLevelParent(this)->IsShown() )
        {
            m_iFlags |= wxPG_FL_GOOD_SIZE_SET;
        }*/

    }

    // Need to correct widget position?
    if ( m_selected && width != old_width /*&& !sb_vis_toggled*/ )
    {
        // Take splitter position change into account
        CorrectEditorWidgetSizeX( m_splitterx, width );
    }

    /*
#if defined(__WXGTK__)
    // This is needed atleast on wxGTK:
    // When we move from size that requires scrollbar to size that doesn't,
    // we need to call PGAdjustScrollbars() etc. wxMSW doesn't seem to require
    // this code.
    int vszy = (int)(m_bottomy+m_lineHeight+2);
    if ( ( ( height < vszy && !(m_iFlags & wxPG_FL_SCROLLBAR_DETECTED) ) ||
           ( height >= vszy && (m_iFlags & wxPG_FL_SCROLLBAR_DETECTED) ) )
         && height > 15
       )
        PGAdjustScrollbars(m_bottomy);
#endif
    */

    if ( !m_frozen )
    {

        // Need to recalculate visibles array?
        //if ( height != old_height )
        if ( height > m_calcVisHeight )
            CalculateVisibles ( -1, FALSE );

        //wxScrolledWindow::Refresh(FALSE,NULL);
        if ( sb_vis_toggled )
        {
            Refresh();
        }
        else if ( m_splitterx != old_splitterx )
        {
            if ( abs(height-old_height) < 100 )
            {
                Update(); // Necessary, atleast on wxMSW
                RedrawAllVisible();
            }
            else
            {
                Refresh();
            }
        }
        else
        {
        #if wxPG_USE_CUSTOM_CONTROLS
            // Need to update custom controls.

            // First off, if selected item has custom image, we need to completely redraw it or
            // risk a graphics glitch.
            if ( m_selected &&
                 (
                   m_selected->IsFlagSet(wxPG_PROP_CUSTOMIMAGE) ||
                   ( m_wndSecondary && !m_wndPrimary )
                 )
               )
            {
                DrawItem ( m_selected );
            }
            else
            {
                if ( m_wndPrimary )
                    m_wndPrimary->Draw();
                if ( m_wndSecondary )
                    m_wndSecondary->Draw();
            }
        #endif
        }
    }

    RecalculateVirtualSize();

}

// -----------------------------------------------------------------------
// wxPropertyGrid mouse event handling
// -----------------------------------------------------------------------

// Return FALSE if should be skipped
bool wxPropertyGrid::HandleMouseClick( int x, unsigned int y, wxMouseEvent &event )
{
    bool res = TRUE;

#if __MOUSE_DEBUGGING__
    wxLogDebug ( wxT("  \\--> HandleMouseClick") );
#endif

    // Need to set focus?
    if ( !(m_iFlags & wxPG_FL_FOCUSED) )
    {
        //m_parent->SetFocus (this);
        SetFocus();
        //OnSetFocus ( *((wxFocusEvent*)NULL) );
    }

    if ( y < m_bottomy )
    {

        wxPGProperty* p = DoGetItemAtY(y);

        if ( p )
        {
            int parenting = p->GetParentingType();
            int margin_ends = m_marginwidth + ( ( (int)p->GetDepth()-1 ) * m_subgroup_extramargin );
            if ( x >= margin_ends )
            {
                // Outside margin.

                if ( parenting > 0 )
                {
                    // This is category.
                    wxPropertyCategoryClass* pwc = (wxPropertyCategoryClass*)p;

                    int text_x = m_marginwidth + ((unsigned int)((pwc->m_depth-1)*m_subgroup_extramargin));

                    // Expand, collapse, activate etc. if click on text or left of splitter.
                    if ( x >= text_x
                         &&
                         ( x < (text_x+pwc->GetTextExtent()+(wxPG_CAPRECTXMARGIN*2))
                           ||
                           x < m_splitterx
                         )
                        )
                    {
                        SelectProperty( p, FALSE );

                        // On double-click expand/collapse.
                        if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                        {
                            if ( pwc->m_expanded ) Collapse ( p );
                            else Expand ( p );
                        }
                    }
                }
                else if ( x > (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
                     x < (m_splitterx - wxPG_SPLITTERX_DETECTMARGIN1)
                   )
                {
                // Click on value.
                    bool on_value = FALSE;
                    if ( x > m_splitterx )
                    {
                        m_iFlags |= wxPG_FL_ACTIVATION_BY_CLICK;
                        on_value = TRUE;
                    }
                    SelectProperty( p, on_value );
                    m_iFlags &= ~(wxPG_FL_ACTIVATION_BY_CLICK);

                    if ( p->GetParentingType() <  0 )
                    // On double-click expand/collapse
                        if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                        {
                            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
                            if ( pwc->m_expanded ) Collapse ( p );
                            else Expand ( p );
                        }
                    res = FALSE;
                }
                else
                {
                // click on splitter
                    if ( !(m_windowStyle & wxPG_STATIC_SPLITTER) )
                    {

                        if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
                        {
                            // Double-clicking the splitter causes auto-centering
                            CenterSplitter ( TRUE );
                            // TODO: Would this be more natural?
                            //   .NET grid doesn't do it but maybe we should.
                            //CustSetCursor ( wxCURSOR_ARROW );
                        }
                        else if ( m_dragStatus == 0 )
                        {
                        //
                        // Begin draggin the splitter
                        //
                        #if __MOUSE_DEBUGGING__
                            wxLogDebug ( wxT("       dragging begins at splitter + %i"),
                                (int)(x - m_splitterx) );
                        #endif

                            BEGIN_MOUSE_CAPTURE

                            m_dragStatus = 1;

                            m_dragOffset = x - m_splitterx;

                            wxPG_CLIENT_DC_INIT()

                            #if !wxPG_USE_CUSTOM_CONTROLS
                            if ( m_wndPrimary )
                            {
                                // Changes must be committed here or the
                                // value won't be drawn correctly
                                CommitChangesFromEditor();

                                m_wndPrimary->Show ( FALSE );
                            }
                            #endif

                        #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT && !wxPG_USE_CUSTOM_CONTROLS
                            // Fixes button disappearance bug
                            if ( m_wndSecondary )
                                m_wndSecondary->Show ( FALSE );
                        #endif

                            m_startingSplitterX = m_splitterx;
                        #if wxPG_HEAVY_GFX
                        #else
                            Update(); // clear graphics mess
                            DrawSplitterDragColumn( dc, m_splitterx );
                            m_splitterprevdrawnx = m_splitterx;
                        #endif

                        }
                    }
                }
            }
            else
            {
            // Click on margin.
                //wxLogDebug ("ClickOnMargin(%i,%i,%i)",x,y,index);
                if ( parenting != 0 )
                {
                    int nx = x + m_marginwidth - margin_ends; // Normalize x.

                    if ( (nx >= m_gutterwidth && nx < (m_gutterwidth+m_iconwidth)) )
                    {
                        int y2 = y - p->m_y;
                        if ( (y2 >= m_buttonSpacingY && y2 < (m_buttonSpacingY+m_iconheight)) )
                        {
                            //wxLogDebug ("ClickOnIcon()");
                            if ( ((wxPGPropertyWithChildren*)p)->m_expanded )
                            {
                                Collapse ( p );
                            }
                            else
                            {
                                Expand ( p );
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleMouseRightClick( int, unsigned int y, wxMouseEvent& )
{
    if ( y < m_bottomy )
    {
        // Select property here as well
        wxPGProperty* p = m_propHover;
        if ( p != m_selected )
            SelectProperty( p );

        // Send right click event.
        wxPropertyGridEvent evt( wxEVT_PG_RIGHT_CLICK, GetId() );
        evt.SetPropertyGrid (this);
        evt.SetEventObject (this);
        evt.SetProperty (p);
        GetEventHandler()->AddPendingEvent(evt);
        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------

// Return FALSE if should be skipped
bool wxPropertyGrid::HandleMouseMove ( int x, unsigned int y, wxMouseEvent &event )
{
    // Safety check (needed because mouse capturing may
    // otherwise freeze the control)
    if ( m_dragStatus > 0 && !event.Dragging() )
    {
        wxLogDebug(wxT("MOUSE CAPTURE SAFETY RELEASE TRIGGERED"));
        HandleMouseUp(x,y,event);
    }

    if ( m_dragStatus > 0 )
    {

        if ( x > (m_marginwidth + wxPG_DRAG_MARGIN) &&
             x < (m_width - wxPG_DRAG_MARGIN)
           )
        {

            //wxLogDebug (wxT("...dragging continues..."));

        #if wxPG_HEAVY_GFX

            int new_splitterx = x - m_dragOffset;

            // Splitter redraw required?
            if ( new_splitterx != m_splitterx )
            {

                //wxLogDebug (wxT("splitterx=%i (old=%i)"),(int)x,(int)m_splitterx);

                if ( m_selected )
                    //CorrectEditorWidgetSizeX( x-m_splitterx, 0 );
                    CorrectEditorWidgetSizeX( new_splitterx, m_width );

                // Move everything
                m_splitterx = new_splitterx;

                Update();
                RedrawAllVisible();

            }

        #else

            if ( x != m_splitterx )
            {
                wxPG_CLIENT_DC_INIT_R(FALSE)

                if ( m_splitterprevdrawnx != -1 )
                    DrawSplitterDragColumn( dc, m_splitterprevdrawnx );

                m_splitterx = x;

                DrawSplitterDragColumn( dc, x );

                m_splitterprevdrawnx = x;
            }

        #endif

            m_dragStatus = 2;

        }

        return FALSE;
    }
    else
    {

        int ih = m_lineHeight;
        int sy = y;

    #if wxPG_SUPPORT_TOOLTIPS
        wxPGProperty* prev_hover = m_propHover;
        unsigned char prev_side = m_mouseSide;
    #endif

        // On which item it hovers
        if ( ( !m_propHover && y < m_bottomy)
             ||
             ( m_propHover && ( sy < m_propHover->m_y || sy >= (m_propHover->m_y+ih) ) )
           )
        {
            // Mouse moves on another property

            m_propHover = DoGetItemAtY(y);

            // Send hover event
            wxPropertyGridEvent evt( wxEVT_PG_HIGHLIGHTED, GetId() );
            evt.SetPropertyGrid (this);
            evt.SetEventObject (this);
            evt.SetProperty (m_propHover);
            GetEventHandler()->AddPendingEvent(evt);

        }

    #if wxPG_SUPPORT_TOOLTIPS
        // Store which side we are on
        m_mouseSide = 0;
        if ( x >= m_splitterx )
            m_mouseSide = 2;
        else if ( x >= m_marginwidth )
            m_mouseSide = 1;

        //
        // If tooltips are enabled, show label or value as a tip
        // in case it doesn't otherwise show in full length.
        //
        if ( m_windowStyle & wxPG_TOOLTIPS )
        {
            wxToolTip* tooltip = GetToolTip();

            if ( m_propHover != prev_hover || prev_side != m_mouseSide )
            {
                if ( m_propHover && m_propHover->GetParentingType() <= 0 )
                {
                    wxString tipstring;
                    int space = 0;

                    if ( m_mouseSide == 1 )
                    {
                        tipstring = m_propHover->m_label;
                        space = m_splitterx-m_marginwidth-3;
                    }
                    else if ( m_mouseSide == 2 )
                    {
                        tipstring = m_propHover->GetDisplayedString ();
                        space = m_width - m_splitterx;
                        if ( m_propHover->m_flags & wxPG_PROP_CUSTOMIMAGE )
                            space -= wxPG_CUSTOM_IMAGE_WIDTH + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
                    }

                    if ( space )
                    {
                        int tw, th;
	                    GetTextExtent( tipstring, &tw, &th, 0, 0, &m_font );
                        if ( tw > space )
                        {
                            //wxLogDebug ( wxT("%s"), tipstring.c_str() );
                            /*
                            if ( !tooltip )
                            {
                                tooltip = new wxToolTip( tipstring );
                                SetToolTip ( tooltip );
                            }
                            else
                            {
                                tooltip->SetTip ( tipstring );
                            }
                            */
                            SetToolTip ( tipstring );
                        }
                    }
                    else
                    {
                        if ( tooltip )
                        {
                        #if wxPG_ALLOW_EMPTY_TOOLTIPS
                            SetToolTip ( wxEmptyString );
                        #else
                            SetToolTip ( NULL );
                        #endif
                        }
                    }
                }
                else
                {
                    if ( tooltip )
                    {
                    #if wxPG_ALLOW_EMPTY_TOOLTIPS
                        SetToolTip ( wxEmptyString );
                    #else
                        SetToolTip ( NULL );
                    #endif
                    }
                }
            }
        }
    #endif

        if ( x > (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
             x < (m_splitterx - wxPG_SPLITTERX_DETECTMARGIN1) ||
             y >= m_bottomy ||
             (m_windowStyle & wxPG_STATIC_SPLITTER)
           )
        {
            // hover on something else
            if ( m_curcursor != wxCURSOR_ARROW )
                CustSetCursor ( wxCURSOR_ARROW );
        }
        else
        {
            // Do not allow splitter cursor on caption items.
            // (also not if we were dragging and its started
            // outside the splitter region)

            if ( m_propHover &&
                 m_propHover->GetParentingType() <= 0 &&
                 !event.Dragging() )
            {

                // hover on splitter
                if ( m_curcursor != wxCURSOR_SIZEWE )
                    CustSetCursor ( wxCURSOR_SIZEWE );

                return FALSE;
            }
            else
            {
                // hover on something else
                if ( m_curcursor != wxCURSOR_ARROW )
                    CustSetCursor ( wxCURSOR_ARROW );
            }
        }
    }
    return TRUE;
}

// -----------------------------------------------------------------------

// Also handles Leaving event
bool wxPropertyGrid::HandleMouseUp ( int x, unsigned int y, wxMouseEvent &WXUNUSED(event) )
{
    bool res = FALSE;

#if __MOUSE_DEBUGGING__
    wxLogDebug ( wxT("  \\--> HandleMouseUp") );
#endif

    // No event type check - basicly calling this method should
    // just stop dragging.
    //if( event.LeftUp() || event.Leaving() )
	//{
        // Left up after dragged?
        if ( m_dragStatus >= 1 )
        {
        //
        // End Splitter Dragging
        //
        #if __MOUSE_DEBUGGING__
            wxLogDebug ( wxT("       dragging ends") );
        #endif

            // DO NOT ENABLE FOLLOWING LINE!
            // (it is only here as a reminder to not to do it)
            //m_splitterx = x;

        #if wxPG_HEAVY_GFX
            //Refresh();
        #else
            SetSplitterPosition( -1 ); // -1 tells not to make change

            // Hack to clear-up editor graphics mess (on wxMSW, atleast)
            if ( m_selected )
                DrawItem ( m_selected );

        #endif
            // Disable splitter auto-centering
            m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

            // This is necessary to return cursor
            END_MOUSE_CAPTURE

            // Set back the default cursor, if necessary
            if ( x > (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
                 x < (m_splitterx - wxPG_SPLITTERX_DETECTMARGIN1) ||
                 y >= m_bottomy )
            {
                CustSetCursor ( wxCURSOR_ARROW );
            }

            m_dragStatus = 0;

            #if !wxPG_USE_CUSTOM_CONTROLS && wxPG_HEAVY_GFX
            // Control background needs to be cleared
            if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) && m_selected )
                DrawItem ( m_selected );
            #endif

            if ( m_wndPrimary )
            {
            #if !wxPG_USE_CUSTOM_CONTROLS
                m_wndPrimary->Show ( TRUE );
            #endif
            }

        #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT && !wxPG_USE_CUSTOM_CONTROLS
            // Fixes button disappearance bug
            if ( m_wndSecondary )
                m_wndSecondary->Show ( TRUE );
        #endif

        #if !wxPG_USE_CUSTOM_CONTROLS
            // This clears the focus.
            m_editorFocused = 0;
        #endif

        }
    //}
    return res;
}

// -----------------------------------------------------------------------

#if __MOUSE_DEBUGGING__
# define OMPMR  1
# define OMPMO  0
#else
# define OMPMR  0
# define OMPMO  0
#endif

#if wxPG_USE_CUSTOM_CONTROLS

//wxLogDebug ( wxT("On") wxT(#func) wxT("( %i, %i )"),(int)ux,(int)uy );

void wxPropertyGrid::OnMouseEvent ( wxMouseEvent &event )
{
    int ux, uy;
    CalcUnscrolledPosition( event.m_x, event.m_y, &ux, &uy );

    int type = event.GetEventType();

    // Send mousewheel events to custom control popup
    if ( type == wxEVT_MOUSEWHEEL )
    {
        if ( m_ccManager.GetPopup() )
        {
            m_ccManager.GetPopup()->AddPendingEvent(event);
            return;
        }
        /*else if ( m_ccManager.HasKbFocus() )
        {
            return;
        }*/
        event.Skip();
        return;
    }

    // Check whether inside control area or not
    bool inside_ctrl_area = FALSE;
    if ( m_selected != NULL &&
         uy >= m_selected->m_y &&
         uy < (m_selected->m_y+m_lineHeight)
       )
    {
        int x_limit = m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2 + 1;
        if ( m_wndPrimary )
        {
            if ( m_wndPrimary->GetX() > x_limit ) x_limit = m_wndPrimary->GetX();
        }
        else if ( m_wndSecondary )
            x_limit = m_wndSecondary->GetX();
        else
            x_limit = m_width;

        //wxLogDebug ( wxT("x_limit = %i"), x_limit );

        if ( ux >= x_limit )
            inside_ctrl_area = TRUE;
    }

    if ( type == wxEVT_RIGHT_UP )
    {
        HandleMouseRightClick ( ux, uy, event );
    }
    else
    // Do parent mouse event handling *if*
    //   No active editor (so no controls to be worried about) OR
    //   Splitter dragging OR
    //   (Cursor outside the area AND not control-dragging) OR
    //   (Cursor inside the area BUT no actual control-mouse-focus)
    if ( m_selected == NULL || m_dragStatus ||
         ( !inside_ctrl_area && !m_ccManager.IsDragging() )
       )
    {
        // Force leave from custom control
        if ( m_ccManager.HasMouseFocus() && !m_ccManager.IsDragging() )
            m_ccManager.ForceMouseLeave ( event );

        if ( type == wxEVT_MOTION )
        {
            HandleMouseMove ( ux, uy, event );
        }
        else if ( type == wxEVT_LEFT_UP )
        {
            HandleMouseUp ( ux, uy, event );
        }
        /*else if ( type == wxEVT_RIGHT_UP )
        {
            HandleMouseRightClick ( ux, uy, event );
        }*/
        else
        {
            HandleMouseClick ( ux, uy, event );
        }

        return;
    }
    else
    {
        if ( !m_ccManager.ProcessMouseEvent ( event ) )
        {
            // this happens if event did not occur for a custom control

            // verify correct cursor
            if ( m_curcursor != wxCURSOR_ARROW ) CustSetCursor ( wxCURSOR_ARROW );

            //return;
        }
    }

    m_curcursor = wxCURSOR_BLANK; // this makes sure cursor is changed back on return

    if ( m_ccManager.HasEvent() )
        OnCustomEditorEvent ( m_ccManager.GetEvent() );

}

#else // wxPG_USE_CUSTOM_CONTROLS

bool wxPropertyGrid::OnMouseCommon ( wxMouseEvent& event, int* px, int* py )
{
    int ux, uy;
    CalcUnscrolledPosition( event.m_x, event.m_y, &ux, &uy );

#if !wxPG_USE_CUSTOM_CONTROLS
    // Hide popup on clicks
    // FIXME: Not necessary after transient window implemented
    if ( event.GetEventType() != wxEVT_MOTION )
        if ( m_wndPrimary && m_wndPrimary->IsKindOf(CLASSINFO(wxPGComboBox)) )
        {
            ((wxPGComboBox*)m_wndPrimary)->HidePopup( false );
        }
#endif

    //if (printmsg) wxLogDebug ( wxT("On") wxT(#func) wxT("( %i, %i )"),(int)ux,(int)uy );
    wxRect r;
    wxPGCtrlClass* wnd = m_wndPrimary;
    if ( wnd )
        r = wnd->GetRect();
    if ( wnd == (wxPGCtrlClass*) NULL || m_dragStatus ||
         (
           ux <= (m_splitterx + wxPG_SPLITTERX_DETECTMARGIN2) ||
           event.m_y < r.y ||
           event.m_y >= (r.y+r.height)
         )
       )
    {
        *px = ux;
        *py = uy;
        return TRUE;
    }
    else
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustSetCursor ( wxCURSOR_ARROW );
    }
    return FALSE;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseClick( wxMouseEvent &event )
{
    //HANDLE_ON_MOUSE_EV(MouseClick,OMPMR)

    /*if ( event.GetEventType() != wxEVT_LEFT_DOWN )
    {
        wxLogDebug(wxT("%i"),(int)event.GetEventType());
        event.Skip();
    }*/

    int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseClick(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseRightClick( wxMouseEvent &event )
{
    /*HANDLE_ON_MOUSE_EV(MouseRightClick,OMPMR)
    event.Skip();*/
    /*int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseRightClick(x,y,event);
    }
    event.Skip();*/
    int x, y;
    CalcUnscrolledPosition( event.m_x, event.m_y, &x, &y );
    HandleMouseRightClick(x,y,event);
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseMove( wxMouseEvent &event )
{
    /*HANDLE_ON_MOUSE_EV(MouseMove,OMPMO)
    event.Skip();*/
    int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseMove(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseUp( wxMouseEvent &event )
{
    /*HANDLE_ON_MOUSE_EV(MouseUp,OMPMR)
    event.Skip();*/
    int x, y;
    if ( OnMouseCommon ( event, &x, &y ) )
    {
        HandleMouseUp(x,y,event);
    }
    event.Skip();
}

#endif // !wxPG_USE_CUSTOM_CONTROLS

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseEntry ( wxMouseEvent &event )
{
    if ( event.Entering() )
    {
        if ( !(m_iFlags & wxPG_FL_MOUSE_INSIDE) )
        {
        #if __MOUSE_DEBUGGING__
            wxLogDebug (wxT("Mouse Enters Window"));
        #endif
            //SetCursor ( *wxSTANDARD_CURSOR );
            // TODO: Fix this (detect parent and only do
            //   cursor trick if it is a manager).
            wxASSERT ( GetParent() );
            GetParent()->SetCursor(wxNullCursor);

            m_iFlags |= wxPG_FL_MOUSE_INSIDE;
            //if ( m_wndPrimary ) m_wndPrimary->Show ( TRUE );
        }
        else
            GetParent()->SetCursor(wxNullCursor);
    }
    else if ( event.Leaving() )
    {
        //wxLogDebug (wxT("Mouse Leaves Window(%i,%i)"),x,y);

        // Without this, wxSpinCtrl editor will sometimes have wrong cursor
        SetCursor( wxNullCursor );

        // This may get called from child control as well, so event's
        // mouse position cannot be relied on.
        wxPoint pt = ScreenToClient(::wxGetMousePosition());

        if ( ( pt.x <= 0 || pt.y <= 0 || pt.x >= m_width || pt.y >= m_height ) )
        {
            CommitChangesFromEditor();

            if ( (m_iFlags & wxPG_FL_MOUSE_INSIDE) )
            {
            #if __MOUSE_DEBUGGING__
                wxLogDebug (wxT("Mouse Leaves Window"));
            #endif
                m_iFlags &= ~(wxPG_FL_MOUSE_INSIDE);
                //if ( m_wndPrimary ) m_wndPrimary->Show ( FALSE );
            }

            if ( m_dragStatus )
                wxPropertyGrid::HandleMouseUp ( -1, 10000, event );
        }
        else
        {
        #if wxPG_NO_CHILD_EVT_MOTION
            // cursor must be reset because EVT_MOTION handler is not there to do it
            if ( m_curcursor != wxCURSOR_ARROW ) CustSetCursor ( wxCURSOR_ARROW );
        #endif
        }
    }

    event.Skip();
}

// -----------------------------------------------------------------------

#if !wxPG_USE_CUSTOM_CONTROLS

//    if (printmsg) wxLogDebug ( wxT("On") wxT(#func) wxT("Child ( %i, %i )"),(int)event.m_x,(int)event.m_y );

// Common code used by various OnMouseXXXChild methods.
bool wxPropertyGrid::OnMouseChildCommon ( wxMouseEvent &event, int* px, int *py )
{
    wxWindow* top_ctrl_wnd = (wxWindow*)event.GetEventObject();
    wxASSERT ( top_ctrl_wnd );
    int x, y;
    event.GetPosition(&x,&y);

#if wxPG_ENABLE_CLIPPER_WINDOW
    // Take clipper window into account
    if (top_ctrl_wnd->GetPosition().x < 1 &&
        !top_ctrl_wnd->IsKindOf(CLASSINFO(wxPGClipperWindow)))
    {
        top_ctrl_wnd = top_ctrl_wnd->GetParent();
        //wxLogDebug(top_ctrl_wnd->GetClassInfo()->GetClassName());
        wxASSERT( top_ctrl_wnd->IsKindOf(CLASSINFO(wxPGClipperWindow)) );
        x -= ((wxPGClipperWindow*)top_ctrl_wnd)->GetXClip();
        y -= ((wxPGClipperWindow*)top_ctrl_wnd)->GetYClip();
    }
#endif

    wxRect r = top_ctrl_wnd->GetRect();
    if ( !m_dragStatus &&
         x > (m_splitterx-r.x+wxPG_SPLITTERX_DETECTMARGIN2) &&
         y >= 0 && y < r.height \
       )
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustSetCursor ( wxCURSOR_ARROW );
        event.Skip();
    }
    else
    {
        CalcUnscrolledPosition( event.m_x + r.x, event.m_y + r.y, \
            px, py );
        return TRUE;
    }
    return FALSE;
}

void wxPropertyGrid::OnMouseClickChild( wxMouseEvent &event )
{
    //HANDLE_ON_MOUSE_EV_CHILD(MouseClick,OMPMR)
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseClick(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseRightClickChild( wxMouseEvent &event )
{
    //HANDLE_ON_MOUSE_EV_CHILD(MouseRightClick,OMPMR)
    /*
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseRightClick(x,y,event);
        if ( !res ) event.Skip();
    }*/
    //wxLogDebug(wxT("wxPropertyGrid::OnMouseRightClickChild"));
    int x,y;
    wxASSERT ( m_wndPrimary );
    // These coords may not be exact (about +-2),
    // but that should not matter (right click is about item, not position).
    wxPoint pt = m_wndPrimary->GetPosition();
    CalcUnscrolledPosition( event.m_x + pt.x, event.m_y + pt.y, &x, &y );
    wxASSERT ( m_selected );
    m_propHover = m_selected;
    bool res = HandleMouseRightClick(x,y,event);
    if ( !res ) event.Skip();
}

void wxPropertyGrid::OnMouseMoveChild( wxMouseEvent &event )
{
    //HANDLE_ON_MOUSE_EV_CHILD(MouseMove,OMPMO)
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseMove(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseUpChild( wxMouseEvent &event )
{
    //HANDLE_ON_MOUSE_EV_CHILD(MouseUp,OMPMR)
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseUp(x,y,event);
        if ( !res ) event.Skip();
    }
}

#endif // !wxPG_USE_CUSTOM_CONTROLS

// -----------------------------------------------------------------------
// wxPropertyGrid keyboard event handling
// -----------------------------------------------------------------------

// If wxPG_OLD_STYLE_KEYNAV == 1, then TABs are handled manually
#if ( wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 ) ) && !wxPG_USE_CUSTOM_CONTROLS
    #define wxPG_OLD_STYLE_KEYNAV   0

void wxPropertyGrid::SendNavigationKeyEvent( int dir )
{
    wxNavigationKeyEvent evt;
    evt.SetFlags(
#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 4 )
        wxNavigationKeyEvent::FromTab|
#endif
                 (dir?wxNavigationKeyEvent::IsForward:
                      wxNavigationKeyEvent::IsBackward));
    evt.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(evt);
}

#else
    #define wxPG_OLD_STYLE_KEYNAV   1
#endif

void wxPropertyGrid::HandleKeyEvent(wxKeyEvent &event)
{

    //
    // Handles key event when editor control is not focused.
    //

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::HandleKeyEvent(%i)"),(int)event.GetKeyCode() );
#endif

    if ( m_frozen )
        return;

    // Travelsal between items, collapsing/expanding, etc.
    int keycode = event.GetKeyCode();

#if !wxPG_OLD_STYLE_KEYNAV
    if ( keycode == WXK_TAB )
    {
        SendNavigationKeyEvent( event.ShiftDown()?0:1 );
        return;
    }
#endif

    // Must update changes if special key was used
    // (because it may trigger closing of this app)
    if ( event.AltDown() ||
         event.ControlDown() ||
         event.MetaDown() )
    {
        CommitChangesFromEditor();
    }

    if ( m_selected )
    {
        wxPGProperty* p = m_selected;
        bool focus_selected = FALSE;
        int select_dir = -2;

        if ( p->GetParentingType() != 0 &&
             !(p->m_flags & wxPG_PROP_DISABLED)
           )
        {
            if ( keycode == WXK_LEFT )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || Collapse ( p ) )
                    keycode = 0;
            }
            else if ( keycode == WXK_RIGHT )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || Expand ( p ) )
                    keycode = 0;
            }
        #if wxPG_OLD_STYLE_KEYNAV
            else if ( keycode == WXK_TAB && !m_wndPrimary )
            {
                wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
                focus_selected = TRUE;
                if ( !event.ShiftDown() )
                {
                    if ( !pwc->m_expanded )
                        Expand ( p );
                    select_dir = 1;
                }
                else
                    select_dir = 0;
                keycode = 0;
            }
        #endif
        }

        if ( keycode )
        {
            if ( keycode == WXK_UP || keycode == WXK_LEFT )
            {
                select_dir = 0;
            }
            else if ( keycode == WXK_DOWN || keycode == WXK_RIGHT )
            {
                select_dir = 1;
            }
        #if wxPG_OLD_STYLE_KEYNAV
            else if ( keycode == WXK_TAB )
            {
                // Tab, no editor: Go to next, focus editor if any.
                // Tab, editor: Focus to editor.
                // Shift-Tab, no editor: Go to prev, focus editor if any.
                // Shift-Tab, editor: Go to prev, focus editor if any.

                select_dir = 1;
                if ( event.ShiftDown() )
                    select_dir = 0;

                if ( m_windowStyle & wxTAB_TRAVERSAL )
                {
                    if ( select_dir )
                    {
                        focus_selected = TRUE;

                        // Forwards

                        // If editor not yet focused, then do that now.
                        if ( m_wndPrimary && !(p->m_flags & wxPG_PROP_DISABLED)
                             &&
                        #if wxPG_USE_CUSTOM_CONTROLS
                             !( m_ccManager.HasKbFocus() )
                        #else
                             (wxWindow::FindFocus()!=m_wndPrimary)
                        #endif
                             )
                        {
                            //wxLogDebug(wxT("\"%s\" -> SetFocusFromKbd()"),p->m_label.c_str());
                            m_wndPrimary->SetFocusFromKbd();

                            /*
                        #if !wxPG_USE_CUSTOM_CONTROLS
                        # ifdef __WXDEBUG__
                            if ( wxWindow::FindFocus() != m_wndPrimary )
                                wxLogDebug(wxT("wxPropertyGrid: Warning! Failed to set focus to primary editor control."));
                        # endif
                        #endif
                            */

                            m_editorFocused = 1;

                            select_dir = -2;
                        }
                    }
                }
                else
                {
                    // just navigate out of control, if no internal
                    // tab-traversal was used
                    NavigateOut ( select_dir );
                }
            }
            else
            {
                event.Skip();
            }
        #endif

        }

        if ( select_dir >= -1 )
        {
            //wxLogDebug(wxT("\"%s\" -> GetNeighbourItem(%i)"),p->m_label.c_str(),(int)select_dir);
            p = GetNeighbourItem ( p, TRUE, select_dir );
            if ( p )
                SelectProperty(p,focus_selected);
            // keycode may not be valid here, so must use event.GetKeyCode()
        #if wxPG_OLD_STYLE_KEYNAV
            else if ( event.GetKeyCode() == WXK_TAB )
                // if at the first or last, navigate out of control
                NavigateOut ( select_dir );
        #endif
        }

    }
    else
    {
        if ( keycode != WXK_ESCAPE )
        {
            // If nothing was selected, select the first item now
            // (or navigate out on tab).
#if wxPG_OLD_STYLE_KEYNAV
            if ( keycode != WXK_TAB )
            {
#endif
                wxPGProperty* p = GetFirst();
                if ( p ) SelectProperty(p);
#if wxPG_OLD_STYLE_KEYNAV
            }
            else
            {
                NavigateOut(event.ShiftDown()?0:1);
            }
#endif
        }
    }
}

// -----------------------------------------------------------------------

// Potentially handles a keyboard event for editor controls.
// Returns false if event should *not* be skipped (on true it can
// be optionally skipped).
// Basicly, false means that SelectProperty was called (or was about
// to be called, if canDestroy was false).
bool wxPropertyGrid::HandleChildKey ( wxKeyEvent& event, bool canDestroy )
{
    int keycode = event.GetKeyCode();
    bool res = true;

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::HandleChildKey(%i)"),(int)event.GetKeyCode() );
#endif

    // Handle TAB and ESCAPE in control
    // (but should *not' handle anything else)
    if ( m_wndPrimary &&
#if wxPG_USE_CUSTOM_CONTROLS
        ( m_ccManager.HasKbFocus() ||
#else
        ( wxWindow::FindFocus() == m_wndPrimary ||
#endif
        m_editorFocused || !canDestroy) /* &&
         (
            ( m_selected->GetParentingType() >= 0 )
         )*/
       )
    {
        //wxLogDebug(wxT("Handle TAB and ESCAPE in control"));
        // Next? Prev?
#if wxPG_OLD_STYLE_KEYNAV
        if ( m_windowStyle & wxTAB_TRAVERSAL )
#endif
        {
#if wxPG_OLD_STYLE_KEYNAV
            if ( keycode == WXK_TAB )
            {
                wxPGProperty* next;
                int getdir = 0;
                if ( !event.ShiftDown() )
                    getdir = 1;

                next = GetNeighbourItem ( m_selected, true, getdir );

                if ( next )
                {
                    // This allows preventing NavigateOut to occur
                    res = false;

                    // For custom controls, canDetory is only use to
                    // indicate that this is really a child call.
                #if !wxPG_USE_CUSTOM_CONTROLS
                    if ( canDestroy )
                #endif
                        SelectProperty ( next, true );
                }
                else
                    NavigateOut ( getdir, true );
            }
            else
#endif
            // Unfocus?
            if ( keycode == WXK_ESCAPE )
            {
                wxPGProperty* p = m_selected;

                res = false;

            #if !wxPG_USE_CUSTOM_CONTROLS
                if ( canDestroy )
            #endif
                {
                    ClearSelection();
                    SelectProperty ( p, false );
                    SetFocus();
                }
            }
        //#if wxPG_USE_CUSTOM_CONTROLS
            else
            {
                event.Skip();
            }
        //#endif
        }
#if wxPG_OLD_STYLE_KEYNAV
        else
        {
            event.Skip();
        }
#endif
    }
    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnKey ( wxKeyEvent &event )
{
#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGrid::OnKey(%i)"),(int)event.GetKeyCode() );
#endif

    //
    // Events to editor controls should get relayed here.
    //

#if wxPG_USE_CUSTOM_CONTROLS

    if ( m_ccManager.HasKbFocus() )
    {
        bool processed = m_ccManager.ProcessKeyboardEvent ( event );

        if ( processed )
        {
            if ( m_ccManager.HasEvent() )
                OnCustomEditorEvent ( m_ccManager.GetEvent() );
            // Its probably not a good idea to process the event
            // twice (used to do that, but call probably never
            // did anything).
            // HandleChildKey ( event );
        }
        else
        {
            HandleChildKey ( event, false );
        }

        return;
    }
    HandleKeyEvent ( event );

#else

    wxWindow* focused = wxWindow::FindFocus();

    /*if ( focused )
        wxLogDebug(focused->GetClassInfo()->GetClassName());
    else
        wxLogDebug(wxT("<no focus>"));*/

    if ( m_wndPrimary &&
         (focused==m_wndPrimary
          || m_editorFocused
    #if wxPG_ENABLE_CLIPPER_WINDOW
          || ((m_wndPrimary->IsKindOf(CLASSINFO(wxPGClipperWindow))) &&
              ((wxPGClipperWindow*)m_wndPrimary)->GetControl() == focused)
    #endif
          ) )
    {
        // Child key must be processed here, since it can
        // destroy the control which is referred by its own
        // event handling.
        HandleChildKey ( event, true );
    }
    else
        HandleKeyEvent ( event );

    /*if ( m_editorFocused && m_wndPrimary )
    {
        m_wndPrimary->AddPendingEvent( event );
    }*/

#endif

}

// -----------------------------------------------------------------------

#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )

void wxPropertyGrid::OnNavigationKey( wxNavigationKeyEvent& event )
{
    //wxLogDebug(wxT("wxPropertyGrid::OnNavigationKey(flags=%X)"),(int)event.m_flags);

#if !wxPG_USE_CUSTOM_CONTROLS

    // Ignore events that occur very close to focus set
    if ( m_iFlags & wxPG_FL_IGNORE_NEXT_NAVKEY )
    {
        m_iFlags &= ~(wxPG_FL_IGNORE_NEXT_NAVKEY);
        event.Skip();
        return;
    }

    wxPGProperty* next = (wxPGProperty*) NULL;

    int dir = event.GetDirection()?1:0;

    if ( m_selected )
    {
        if ( dir == 1 && (m_wndPrimary || m_wndSecondary) )
        {
            wxWindow* focused = wxWindow::FindFocus();
            //if ( focused ) wxLogDebug(wxT("  focused: %s"),focused->GetName().c_str());
            //else wxLogDebug(wxT("  focused: <NULL>"));

            wxWindow* wndToCheck = m_wndPrimary;

            // If no primary editor control, focus to button (if any)
            if ( !wndToCheck )
                wndToCheck = m_wndSecondary;

        #if wxPG_ENABLE_CLIPPER_WINDOW
            if ( wndToCheck->IsKindOf(CLASSINFO(wxPGClipperWindow)) )
                wndToCheck = ((wxPGClipperWindow*)wndToCheck)->GetControl();
        #endif

            if ( focused != wndToCheck )
            {
                wndToCheck->SetFocus();

                // Select all text in wxTextCtrl
                if ( wndToCheck->IsKindOf(CLASSINFO(wxTextCtrl)) )
                    ((wxTextCtrl*)wndToCheck)->SetSelection(-1,-1);

                m_editorFocused = 1;
                next = m_selected;
            }
        }

        if ( !next )
        {
            next = GetNeighbourItem(m_selected,true,dir);

            if ( next )
            {
                //wxLogDebug(wxT("  next = %s"),next->GetName().c_str());
                // This allows preventing NavigateOut to occur
                SelectProperty ( next, true );
            }
        }
    }

    if ( !next )
        event.Skip();

#else

    event.Skip();

#endif

}

#endif

// -----------------------------------------------------------------------

#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 3 )

// tab/shift-tab to previous or next control. dir=1 means forward.
void wxPropertyGrid::NavigateOut ( int dir, bool isChildFocused )
{
    wxWindow* navParent = this;
    if ( m_iFlags & wxPG_FL_IN_MANAGER )
        navParent = GetParent();

    m_iFlags |= wxPG_FL_NAVIGATING_OUT;

    //wxLogDebug(wxT("wxPropertyGrid::NavigateOut(%s,%i)"),navParent->GetName().c_str(),dir);

#if wxMINOR_VERSION > 5 || ( wxMINOR_VERSION == 5 && wxRELEASE_NUMBER >= 4 )
    int navFlags = wxNavigationKeyEvent::FromTab;
#else
    int navFlags = 0;
#endif

    if ( isChildFocused )
#if !wxPG_USE_CUSTOM_CONTROLS
        navFlags |= wxNavigationKeyEvent::WinChange;
#else
        navFlags |= 0;
#endif

    if ( dir > 0 )
        navFlags |= wxNavigationKeyEvent::IsForward;
    else
        navFlags |= wxNavigationKeyEvent::IsBackward;

    navParent->Navigate( navFlags );
}
#else
void wxPropertyGrid::NavigateOut ( int, bool ) { }
#endif

// -----------------------------------------------------------------------

#if !wxPG_USE_CUSTOM_CONTROLS

void wxPropertyGrid::OnKeyChild( wxKeyEvent &event )
{
    //wxLogDebug ( wxT("wxPropertyGrid::OnKeyChild") );

    // Must update changes if special key was used
    // (because it may trigger closing of this app)
    if ( event.AltDown() ||
         event.ControlDown() ||
         event.MetaDown() )
    {
        CommitChangesFromEditor();
    }

    // Since event handling may destroy the control which
    // triggered this event, we need to send it separately
    // to the wxPropertyGrid itself.
    if ( HandleChildKey(event,false) == false )
        AddPendingEvent( event );
    else
        event.Skip();

}

#endif

// -----------------------------------------------------------------------
// wxPropertyGrid miscellaneous event handling
// -----------------------------------------------------------------------

void wxPropertyGrid::OnFocusEvent ( wxFocusEvent& event )
{
    unsigned int oldFlags = m_iFlags;

    if ( event.GetEventType() == wxEVT_SET_FOCUS )
    {
        m_iFlags |= wxPG_FL_FOCUSED;
    }
    else
    {
        wxWindow* nextFocus = event.GetWindow();

        m_iFlags &= ~(wxPG_FL_FOCUSED);

        wxWindow* parent = nextFocus;

        // This must be one parent nextFocus' parents.
        while ( parent )
        {
            if ( parent == this )
            {
                m_iFlags |= wxPG_FL_FOCUSED;
                break;
            }
            parent = parent->GetParent();
        }

    }

    if ( (m_iFlags & wxPG_FL_FOCUSED) !=
         (oldFlags & wxPG_FL_FOCUSED) )
    {

        // On each focus kill, mark the next nav key event
        // to be ignored (can't do on set focus since the
        // event would occur before it).
        if ( !(m_iFlags & wxPG_FL_FOCUSED) )
        {
        #if wxPG_USE_CUSTOM_CONTROLS
            m_ccManager.OnParentFocusChange ( FALSE );
        #endif

            //wxLogDebug(wxT("wxPropertyGrid::OnFocusEvent(wxEVT_KILL_FOCUS)"));

            m_iFlags |= wxPG_FL_IGNORE_NEXT_NAVKEY;

            // Need to store changed value
            CommitChangesFromEditor();

        }
        else
        {
        #if wxPG_USE_CUSTOM_CONTROLS
            m_ccManager.OnParentFocusChange ( TRUE );
        #endif

            //wxLogDebug(wxT("wxPropertyGrid::OnFocusEvent(wxEVT_SET_FOCUS)"));
            m_iFlags &= ~(wxPG_FL_IGNORE_NEXT_NAVKEY);
        }

        // Redraw selected
        //if ( m_selected )
        //    DrawItem( m_selected );
    }

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnScrollEvent ( wxScrollWinEvent &event )
{
    //wxLogDebug ( wxT("OnScrollEvent") );

    m_iFlags |= wxPG_FL_SCROLLED;

#if wxPG_USE_CUSTOM_CONTROLS
    m_ccManager.OnParentScrollWinEvent( event );
#endif

    event.Skip();
}

// -----------------------------------------------------------------------

#if !wxPG_USE_CUSTOM_CONTROLS
void wxPropertyGrid::OnCaptureChange ( wxMouseCaptureChangedEvent& WXUNUSED(event) )
{
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
    {
    #if __MOUSE_DEBUGGING__
        wxLogDebug ( wxT("wxPropertyGrid: mouse capture lost") );
    #endif
        m_iFlags &= ~(wxPG_FL_MOUSE_CAPTURED);
    }
}
#endif // !wxPG_USE_CUSTOM_CONTROLS

// -----------------------------------------------------------------------
// Property text-based storage
// -----------------------------------------------------------------------

#define wxPG_PROPERTY_FLAGS_COUNT   8

// property-flag-to-text array
static const wxChar* gs_property_flag_to_string[wxPG_PROPERTY_FLAGS_COUNT] =
{
    wxT("Modified"),
    wxT("Disabled"),
    wxT("LowPriority"),
    (const wxChar*) NULL, // wxPG_PROP_CUSTOMIMAGE is auto-generated flag
    wxT("LimitedEditing"),
    wxT("Unspecified"),
    (const wxChar*) NULL, // Special flags cannot be stored as-is
    (const wxChar*) NULL  //
};

wxString wxPropertyContainerMethods::GetPropertyAttributes( wxPGId id, unsigned int flagmask )
{
    wxPGProperty* p = id;
    wxASSERT(p);

    wxString s;
    unsigned int i;
    unsigned int flags = ((unsigned int)p->GetFlags()) &
                         flagmask &
                         ~(wxPG_PROP_CUSTOMIMAGE |
                           wxPG_PROP_CLASS_SPECIFIC_1 |
                           wxPG_PROP_CLASS_SPECIFIC_2);

    if ( !flags )
        return wxEmptyString;

    for ( i=0; i<wxPG_PROPERTY_FLAGS_COUNT; i++ )
    {
        if ( flags & (1<<i) )
        {
            s.append( gs_property_flag_to_string[i] );
            flags &= ~(1<<i);
            if ( !flags )
                break;
            s.append(wxT(", "));
        }
    }

    return s;
}

// -----------------------------------------------------------------------

void wxPropertyContainerMethods::SetPropertyAttributes( wxPGId id, const wxString& attributes )
{
    wxPGProperty* p = id;
    wxASSERT(p);
    size_t i;

    WX_PG_TOKENIZER1_BEGIN(attributes,wxT(','))

        for (i=0;i<wxPG_PROPERTY_FLAGS_COUNT;i++)
        {
            const wxChar* flagText = gs_property_flag_to_string[i];
            if ( flagText && token == flagText )
            {
                p->SetFlag( 1<<i );
                break;
            }
        }

    WX_PG_TOKENIZER1_END()

}

// -----------------------------------------------------------------------

wxString wxPropertyContainerMethods::GetPropertyShortClassName( wxPGId id )
{
    wxPGProperty* p = id;
    if ( p->GetParentingType() != 1 )
    {
        const wxChar* src = p->GetClassName();
        wxASSERT( src[0] == wxT('w') && src[1] == wxT('x') );
        wxString s(&src[2]);
        wxASSERT ( (((int)s.length())-8) > 0 );
        s.Truncate(s.length()-8);
        //s.LowerCase();
        return s;
    }
    return wxT("Category");
}

// -----------------------------------------------------------------------
// Value type related methods (should all be pretty much static).

wxPGValueType::~wxPGValueType()
{
}

const wxChar* wxPGValueType::GetCustomTypeName() const
{
    return GetTypeName();
}

// Implement default types.
WX_PG_IMPLEMENT_VALUE_TYPE(wxString,wxStringProperty,wxPGTypeName_wxString,GetString,wxEmptyString)
WX_PG_IMPLEMENT_VALUE_TYPE(long,wxIntProperty,wxPGTypeName_long,GetLong,(long)0)
WX_PG_IMPLEMENT_VALUE_TYPE(double,wxFloatProperty,wxPGTypeName_double,GetDouble,0.0)
WX_PG_IMPLEMENT_VALUE_TYPE(wxArrayString,wxArrayStringProperty,wxPGTypeName_wxArrayString,GetArrayString,wxArrayString())

// Bool is a special case... thanks to the C++'s bool vs int vs long inconsistency issues.
const wxPGValueType *wxPGValueType_bool = (wxPGValueType *) NULL;
class wxPGValueTypeboolClass : public wxPGValueType
{
public:
    virtual const wxChar* GetTypeName() const { return wxPGTypeName_long; }
    virtual const wxChar* GetCustomTypeName() const { return wxPGTypeName_bool; }
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant(0); }
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const
    { return wxVariant ( value.GetBool(), name ); }
    virtual wxPGProperty* GenerateProperty ( const wxString& label, const wxString& name ) const
    {
        return wxPG_NEWPROPERTY(Bool,label,name,false);
    }
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const
    {
        wxASSERT_MSG( wxStrcmp(wxPGTypeName_bool,value.GetType().c_str()) == 0,
            wxT("SetValueFromVariant: wxVariant type mismatch.") );
        property->DoSetValue(value.GetBool()?1:0);
    }
};

// Implement nonetype.
const wxPGValueType *wxPGValueType_none = (wxPGValueType*) NULL;
class wxPGValueTypenoneClass : public wxPGValueType
{
public:
    virtual const wxChar* GetTypeName() const { return wxT("null"); }
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant((long)0); }
    virtual wxVariant GenerateVariant ( wxPGVariant, const wxString& name ) const
    { return wxVariant ( (long)0, name ); }
    virtual wxPGProperty* GenerateProperty ( const wxString&, const wxString& ) const
    { return (wxPGProperty*) NULL; }
    virtual void SetValueFromVariant ( wxPGProperty*, wxVariant& ) const
    { }
};

// Implement void* type.
const wxPGValueType *wxPGValueType_void = (wxPGValueType*) NULL;
class wxPGValueTypevoidClass : public wxPGValueType
{
public:
    virtual const wxChar* GetTypeName() const { return wxPGTypeName_void; }
    virtual wxPGVariant GetDefaultValue () const { return wxPGVariant((void*)NULL); }
    virtual wxVariant GenerateVariant ( wxPGVariant value, const wxString& name ) const
    { return wxVariant ( value.GetRawPtr(), name ); }
    virtual wxPGProperty* GenerateProperty ( const wxString&, const wxString& ) const
    { return (wxPGProperty*) NULL; }
    virtual void SetValueFromVariant ( wxPGProperty* property, wxVariant& value ) const
    {
        wxASSERT_MSG( wxStrcmp(GetTypeName(),value.GetType().c_str()) == 0,
            wxT("SetValueFromVariant: wxVariant type mismatch.") );
        property->DoSetValue(value.GetVoidPtr());
    }
};

// Registers all default value types
void wxPropertyGrid::RegisterDefaultValues()
{
    wxPGRegisterDefaultValueType ( none );
    wxPGRegisterDefaultValueType ( wxString );
    wxPGRegisterDefaultValueType ( long );
    wxPGRegisterDefaultValueType ( bool );
    wxPGRegisterDefaultValueType ( double );
    wxPGRegisterDefaultValueType ( void );
    wxPGRegisterDefaultValueType ( wxArrayString );
}

// no_def_check = TRUE prevents infinite recursion.
wxPGValueType* wxPropertyGrid::RegisterValueType ( wxPGValueType* valueclass, bool no_def_check )
{
    wxASSERT ( valueclass );

    if ( !no_def_check && wxPGGlobalVars->m_dictValueType.empty() )
        RegisterDefaultValues();

    wxString temp_str;
    const wxChar* name = valueclass->GetType();

    wxPGValueType* p_at_slot = (wxPGValueType*) wxPGGlobalVars->m_dictValueType[name];

    if ( !p_at_slot )
    {
        //wxLogDebug (wxT("Registered value type \"%s\""),name);
        wxPGGlobalVars->m_dictValueType[name] = (void*) valueclass;
        return valueclass;
    }

    // Delete given object instance, but only if it wasn't the same as in the hashmap.
    if ( p_at_slot != valueclass )
        delete valueclass;

    return p_at_slot;
}


/*
 * wxPGVariantDataWxObj
 */

//IMPLEMENT_DYNAMIC_CLASS(wxPGVariantDataWxObj, wxVariantData)

#if wxUSE_STD_IOSTREAM
bool wxPGVariantDataWxObj::Write(wxSTD ostream&) const
{
    // Not implemented
    return TRUE;
}
#endif

bool wxPGVariantDataWxObj::Write(wxString&) const
{
    // Not implemented
    return TRUE;
}

#if wxUSE_STD_IOSTREAM
bool wxPGVariantDataWxObj::Read(wxSTD istream& WXUNUSED(str))
{
    // Not implemented
    return FALSE;
}
#endif

bool wxPGVariantDataWxObj::Read(wxString& WXUNUSED(str))
{
    // Not implemented
    return FALSE;
}

// -----------------------------------------------------------------------
// Editor class specific.

// no_def_check = TRUE prevents infinite recursion.
wxPGEditor* wxPropertyGrid::RegisterEditorClass ( wxPGEditor* editorclass, bool no_def_check )
{
    wxASSERT ( editorclass );

    if ( !no_def_check && wxPGGlobalVars->m_arrEditorClasses.empty() )
        RegisterDefaultEditors();

    wxPGGlobalVars->m_arrEditorClasses.Add((void*)editorclass);

    return editorclass;
}

// Registers all default editor classes
void wxPropertyGrid::RegisterDefaultEditors()
{
    wxPGRegisterDefaultEditorClass ( TextCtrl );
    wxPGRegisterDefaultEditorClass ( Choice );
    wxPGRegisterDefaultEditorClass ( TextCtrlAndButton );
#if wxPG_INCLUDE_CHECKBOX
    wxPGRegisterDefaultEditorClass ( CheckBox );
#endif
    wxPGRegisterDefaultEditorClass ( ChoiceAndButton );
}

#if wxPG_USE_VALIDATORS

// -----------------------------------------------------------------------
// wxPropertyValidator
// -----------------------------------------------------------------------

wxPropertyValidator::wxPropertyValidator()
{
    m_refCount = 0;
    m_refObject = (wxPropertyValidator*) NULL;
}

wxPropertyValidator::~wxPropertyValidator()
{
}

wxPropertyValidator* wxPropertyValidator::Ref()
{
    if ( m_refCount )
    {
        // If object given to SetPropertyValidator was one got
        // from GetPropertyValidator, we go here.
        m_refCount++;
        //wxLogDebug(wxT("%X: Refcount risen (now %i)"),(unsigned int)this,m_refCount);
        return this;
    }

    wxPropertyValidator* refObj = m_refObject;

    if ( !refObj )
    {
        refObj = Clone();
        m_refObject = refObj;
    }
    refObj->m_refCount++;
    //wxLogDebug(wxT("%X: Refcount risen (now %i)"),(unsigned int)refObj,refObj->m_refCount);

    return refObj;
}

// Return TRUE if need to delete
bool wxPropertyValidator::UnRef()
{
    m_refCount--;
    //wxLogDebug(wxT("%X: Refcount dropped (now %i)"),(unsigned int)this,m_refCount);
    return ( m_refCount < 1 );
}

#ifdef __WXDEBUG__
void wxPropertyValidator::AssertDataType ( const wxChar* ) const
{
}
#endif

// -----------------------------------------------------------------------
// wxStringPropertyValidator
// -----------------------------------------------------------------------

wxStringPropertyValidator::wxStringPropertyValidator( const wxString& excludeList )
    : wxPropertyValidator()
{
    //wxLogDebug(wxT("%X: wxStringPropertyValidator::wxStringPropertyValidator()"),(unsigned int)this);
    m_excludeList = excludeList;
}

wxStringPropertyValidator::~wxStringPropertyValidator()
{
    //wxLogDebug(wxT("%X: wxStringPropertyValidator::~wxStringPropertyValidator()"),(unsigned int)this);
}

wxPropertyValidator* wxStringPropertyValidator::Clone() const
{
    return new wxStringPropertyValidator(m_excludeList);
}

bool wxStringPropertyValidator::Validate ( wxPGVariant& value, wxString& showmsg ) const
{
    const wxString& str = value.GetString();
    size_t i;

    // Check exclude list
    for ( i=0; i<m_excludeList.length(); i++ )
    {
        if ( str.Index(m_excludeList[i]) != wxNOT_FOUND )
        {
            showmsg.Printf(_("String cannot contain any of these: %s"),m_excludeList.c_str());
            return FALSE;
        }
    }

    return TRUE;
}

#ifdef __WXDEBUG__
void wxStringPropertyValidator::AssertDataType ( const wxChar* typestr ) const
{
    wxASSERT ( wxStrcmp(typestr,wxT("string")) == 0  );
}
#endif

// -----------------------------------------------------------------------
// wxIntPropertyValidator
// -----------------------------------------------------------------------

wxIntPropertyValidator::wxIntPropertyValidator( long min, long max )
    : wxPropertyValidator()
{
    wxASSERT_MSG ( min < max, wxT("Invalid value range given for property validator") );
    m_min = min;
    m_max = max;
}

wxIntPropertyValidator::~wxIntPropertyValidator()
{
}

wxPropertyValidator* wxIntPropertyValidator::Clone() const
{
    return new wxIntPropertyValidator(m_min,m_max);
}

bool wxIntPropertyValidator::Validate ( wxPGVariant& value, wxString& showmsg ) const
{
    long val = value.GetLong();
    long ret_val = val;

    if ( val < m_min )
        ret_val = m_min;
    else if ( val > m_max )
        ret_val = m_max;

    if ( ret_val != val )
    {
        value.m_v.m_long = ret_val;
        showmsg.Printf(_("Value must be between %i and %i"),(int)m_min,(int)m_max);
        return FALSE;
    }

    return TRUE;
}

#ifdef __WXDEBUG__
void wxIntPropertyValidator::AssertDataType ( const wxChar* typestr ) const
{
    wxASSERT ( wxStrcmp(typestr,wxT("long")) == 0  );
}
#endif

// -----------------------------------------------------------------------
// wxFloatPropertyValidator
// -----------------------------------------------------------------------

wxFloatPropertyValidator::wxFloatPropertyValidator( double min, double max )
    : wxPropertyValidator()
{
    wxASSERT_MSG ( min < max, wxT("Invalid value range given for property validator") );
    m_min = min;
    m_max = max;
}

wxFloatPropertyValidator::~wxFloatPropertyValidator()
{
}

wxPropertyValidator* wxFloatPropertyValidator::Clone() const
{
    return new wxFloatPropertyValidator(m_min,m_max);
}

bool wxFloatPropertyValidator::Validate ( wxPGVariant& value, wxString& showmsg ) const
{
    double* pval = value.GetDoublePtr();
    double val = *pval;

    if ( val < m_min || val > m_max )
    {
        if ( val < m_min )
            *pval = m_min;
        else if ( val > m_max )
            *pval = m_max;

        showmsg.Printf(_("Value must be between %.3f and %.3f"),m_min,m_max);
        return FALSE;
    }

    return TRUE;
}

#ifdef __WXDEBUG__
void wxFloatPropertyValidator::AssertDataType ( const wxChar* typestr ) const
{
    wxASSERT ( wxStrcmp(typestr,wxT("double")) == 0  );
}
#endif

#endif

// -----------------------------------------------------------------------
// wxPGStringTokenizer
//   Needed to handle C-style string lists (e.g. "str1" "str2")
// -----------------------------------------------------------------------

wxPGStringTokenizer::wxPGStringTokenizer( const wxString& str, wxChar delimeter )
{
    m_str = &str;
    m_delimeter = delimeter;
}

wxPGStringTokenizer::~wxPGStringTokenizer()
{
}

bool wxPGStringTokenizer::HasMoreTokens()
{
    wxASSERT_MSG ( m_curPos, wxT("Do not call wxPGStringTokenizer methods after HasMoreTokens has returned FALSE."));

    const wxChar* ptr = m_curPos;
    const wxChar* ptr_end = &m_str->c_str()[m_str->length()];

    size_t store_index = 0xFFFFFFFF;

#if !wxUSE_STL
    wxChar* store_ptr_base = (wxChar*) NULL;
#endif

    wxChar delim = m_delimeter;
    wxChar a = *ptr;
    wxChar prev_a = 0;

    while ( a )
    {
        if ( store_index == 0xFFFFFFFF )
        {
            if ( a == delim )
            {
                size_t req_len = ptr_end-ptr+1;
            #if wxUSE_STL
                if ( m_readyToken.length() < req_len )
                    m_readyToken.resize( req_len, wxT(' ') );
            #else
                store_ptr_base = m_readyToken.GetWriteBuf ( req_len );
            #endif
                store_index = 0;
                prev_a = 0;
            }
        }
        else
        {
            if ( prev_a != wxT('\\') )
            {
                if ( a != delim )
                {
                    if ( a != wxT('\\') )
                    {
                    #if wxUSE_STL
                        m_readyToken[store_index] = a;
                    #else
                        store_ptr_base[store_index] = a;
                    #endif
                        store_index++;
                    }
                }
                else
                {
                #if wxUSE_STL
                    m_readyToken[store_index] = 0;
                    m_readyToken.resize(store_index,wxT(' '));
                #else
                    store_ptr_base[store_index] = 0;
                    m_readyToken.UngetWriteBuf ( store_index );
                #endif
                    //wxLogDebug(wxT("Ready_token: \"%s\""),m_readyToken.c_str());
                    m_curPos = ptr+1;
                    return TRUE;
                }
                prev_a = a;
            }
            else
            {
            #if wxUSE_STL
                m_readyToken[store_index] = a;
            #else
                store_ptr_base[store_index] = a;
            #endif
                store_index++;
                prev_a = 0;
            }
        }
        ptr++;
        a = *ptr;
    }
#if !wxUSE_STL
    if ( store_index != 0xFFFFFFFF )
        m_readyToken.UngetWriteBuf ( store_index );
#endif
    m_curPos = (const wxChar*) NULL;
    return FALSE;
}

wxString wxPGStringTokenizer::GetNextToken()
{
    wxASSERT_MSG ( m_curPos, wxT("Do not call wxPGStringTokenizer methods after HasMoreTokens has returned FALSE."));
    return m_readyToken;
}

// -----------------------------------------------------------------------
// wxPGConstants
// -----------------------------------------------------------------------

void wxPGConstants::Add ( const wxChar* label, int value )
{
    wxASSERT_MSG ( IsOk(),
        wxT("do not add items to to invalid wxPGConstants") );

    if ( value != -1 && m_arrLabels.GetCount() == m_arrValues.GetCount() )
        m_arrValues.Add ( value );

    m_arrLabels.Add ( label );
}

// -----------------------------------------------------------------------

void wxPGConstants::Add ( const wxChar** labels, const long* values, unsigned int itemcount )
{
    wxASSERT_MSG ( IsOk(),
        wxT("do not add items to to invalid wxPGConstants") );

    if ( !itemcount )
    {
        const wxChar** p = &labels[0];
        while ( *p ) { p++; itemcount++; }
    }

    unsigned int i;
    for ( i = 0; i < itemcount; i++ )
    {
        m_arrLabels.Add ( labels[i] );
    }
    if ( values )
    {
        for ( i = 0; i < itemcount; i++ )
        {
            m_arrValues.Add ( values[i] );
        }
    }
}

// -----------------------------------------------------------------------

void wxPGConstants::Add ( const wxArrayString& arr, const long* values )
{
    wxASSERT_MSG ( IsOk(),
        wxT("do not add items to to invalid wxPGConstants") );

    unsigned int i;
    unsigned int itemcount = arr.GetCount();

    for ( i = 0; i < itemcount; i++ )
    {
        m_arrLabels.Add ( arr[i] );
    }
    if ( values )
    {
        for ( i = 0; i < itemcount; i++ )
            m_arrValues.Add ( values[i] );
    }
}

// -----------------------------------------------------------------------

void wxPGConstants::Add ( const wxArrayString& arr, const wxArrayInt& arrint )
{
    wxASSERT_MSG ( IsOk(),
        wxT("do not add items to to invalid wxPGConstants") );

    unsigned int i;
    unsigned int itemcount = arr.GetCount();

    for ( i = 0; i < itemcount; i++ )
    {
        m_arrLabels.Add ( arr[i] );
        m_arrValues.Add ( arrint[i] );
    }
}

// -----------------------------------------------------------------------

void wxPGConstants::Free()
{
    m_arrLabels.Empty();
    m_arrValues.Empty();
}

// -----------------------------------------------------------------------

wxPGConstants::wxPGConstants()
{
    Init();
}

// -----------------------------------------------------------------------

wxPGConstants::wxPGConstants( const wxChar** labels, const long* values, unsigned int itemcount )
{
    Init();
    Set(labels,values,itemcount);
}

// -----------------------------------------------------------------------

wxPGConstants::wxPGConstants( const wxArrayString& labels, const wxArrayInt& values )
{
    Init();
    Set(labels,values);
}

// -----------------------------------------------------------------------

wxPGConstants::~wxPGConstants()
{
    Free ();

    // Must clear id from table
    if ( m_id )
    {
        wxPGHashMapConstants* socs =
            (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

        socs->erase(m_id);
    }

/*    if ( m_ptrLabels )
        delete [] m_ptrLabels;
    if ( m_ptrValues )
        delete [] m_ptrValues;*/
}

// -----------------------------------------------------------------------
// wxPropertyGridEvent
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyGridEvent, wxCommandEvent)

DEFINE_EVENT_TYPE( wxEVT_PG_SELECTED )
DEFINE_EVENT_TYPE( wxEVT_PG_CHANGED )
DEFINE_EVENT_TYPE( wxEVT_PG_HIGHLIGHTED )
DEFINE_EVENT_TYPE( wxEVT_PG_RIGHT_CLICK )
DEFINE_EVENT_TYPE( wxEVT_PG_PAGE_CHANGED )

wxPropertyGridEvent::wxPropertyGridEvent(wxEventType commandType, int id)
    : wxCommandEvent(commandType,id)
{
    m_property = NULL;
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::wxPropertyGridEvent(const wxPropertyGridEvent& event)
    : wxCommandEvent(event)
{
    m_eventType = event.GetEventType();
    m_eventObject = event.m_eventObject;
    m_pg = event.m_pg;
    m_property = event.m_property;
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::~wxPropertyGridEvent()
{
}

// -----------------------------------------------------------------------

wxEvent* wxPropertyGridEvent::Clone() const
{
    return new wxPropertyGridEvent ( *this );
}

// -----------------------------------------------------------------------
// wxPropertyContainerMethods
// - common methods for wxPropertyGrid and wxPropertyGridManager -
// -----------------------------------------------------------------------

void wxPropertyContainerMethods::SetPropertyAttribute (wxPGId id, int attrid,
                                                       wxVariant value, long arg_flags )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT(p);
    p->SetAttribute(attrid,value);
    if ( ( arg_flags & wxRECURSE ) && p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            SetPropertyAttribute(pwc->Item(i),attrid,value,arg_flags);
    }
}

// -----------------------------------------------------------------------

void wxPropertyContainerMethods::SetBoolChoices (const wxChar* true_choice,
                                                 const wxChar* false_choice)
{
    wxBoolPropertyClass::SetBoolChoices(true_choice,false_choice);
}

// -----------------------------------------------------------------------

wxPGConstants& wxPropertyContainerMethods::GetPropertyChoices( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT( p );

    wxPGChoiceInfo ci;
    ci.m_constants = (wxPGConstants**) NULL;

    p->GetChoiceInfo(&ci);

    //wxASSERT_MSG( ci.m_constants, wxT("this property does not have choices") );

    wxPGConstants* cons = &wxPGGlobalVars->m_emptyConstants;
    if ( ci.m_constants )
        cons = *ci.m_constants;
    return *cons;
}

// -----------------------------------------------------------------------

// Since GetPropertyByName is used *a lot*, this makes sense
// since non-virtual method can be called with less code.
wxPGId wxPropertyContainerMethods::GetPropertyByName( wxPGNameStr name ) const
{
    return DoGetPropertyByName(name);
}

// -----------------------------------------------------------------------
// GetPropertyValueAsXXX methods

#define IMPLEMENT_GET_VALUE(T,TRET,BIGNAME,DEFRETVAL) \
TRET wxPropertyContainerMethods::GetPropertyValueAs##BIGNAME ( wxPGId id ) \
{ \
    wxPGProperty* p = wxPGIdToPtr(id); \
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") ); \
    if ( !p ) return (TRET)DEFRETVAL; \
    if ( p->GetValueType()->GetTypeName() != wxPGTypeName_##T ) \
    { \
        wxPGGetFailed(p,wxPGTypeName_##T); \
        return (TRET)DEFRETVAL; \
    } \
    return (TRET)wxPGVariantTo##BIGNAME(p->DoGetValue()); \
}

// String is different than others.
wxString wxPropertyContainerMethods::GetPropertyValueAsString ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return wxEmptyString;
    wxString str;
    return p->GetValueAsString(wxPG_FULL_VALUE);
}

IMPLEMENT_GET_VALUE(long,long,Long,0)
IMPLEMENT_GET_VALUE(long,bool,Bool,FALSE)
IMPLEMENT_GET_VALUE(double,double,Double,0.0)
IMPLEMENT_GET_VALUE(void,void*,VoidPtr,NULL)
IMPLEMENT_GET_VALUE(wxArrayString,const wxArrayString&,ArrayString,*((wxArrayString*)NULL))

// wxObject is different than others.
const wxObject* wxPropertyContainerMethods::GetPropertyValueAsWxObjectPtr ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return (const wxObject*) NULL;
    const wxChar* typestr = p->GetValueType()->GetTypeName();
    if ( typestr[0] != wxT('w') || typestr[1] != wxT('x') )
    {
        wxPGGetFailed(p,wxT("wxObject"));
        return (const wxObject*) NULL;
    }
    return (const wxObject*)(p->DoGetValue().GetRawPtr());
}

// -----------------------------------------------------------------------

bool wxPropertyContainerMethods::IsPropertyExpanded ( wxPGId id )
{
    wxPGPropertyWithChildren* p = (wxPGPropertyWithChildren*)wxPGIdToPtr( id );
    if ( p->GetParentingType() == 0 )
        return FALSE;
    return p->IsExpanded();
}

// -----------------------------------------------------------------------

// returns value type class for type name
wxPGValueType* wxPropertyContainerMethods::GetValueType(const wxString &type)
{
    wxPGHashMapS2P::iterator it;

    it = wxPGGlobalVars->m_dictValueType.find(type);

    if ( it != wxPGGlobalVars->m_dictValueType.end() )
        return (wxPGValueType*) it->second;

    return (wxPGValueType*) NULL;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyContainerMethods::CreatePropertyByType(const wxString &valuetype,
                                                               const wxString &label,
                                                               const wxString &name)
{
    wxPGHashMapS2P::iterator it;

    it = wxPGGlobalVars->m_dictValueType.find(valuetype);

    if ( it != wxPGGlobalVars->m_dictValueType.end() )
    {
        wxPGValueType* vt = (wxPGValueType*) it->second;
        wxPGProperty* p = vt->GenerateProperty(label,name);
    #ifdef __WXDEBUG__
        if ( !p )
        {
            wxLogDebug (wxT("WARNING: CreatePropertyByValueType generated NULL property for ValueType \"%s\""),valuetype.c_str());
            return (wxPGProperty*) NULL;
        }
    #endif
        return p;
    }

    wxLogDebug(wxT("WARNING: No value type registered with name \"%s\""),valuetype.c_str());
    return (wxPGProperty*) NULL;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyContainerMethods::CreatePropertyByClass(const wxString &classname,
                                                                const wxString &label,
                                                                const wxString &name)
{
    wxPGHashMapS2P* cis =
        (wxPGHashMapS2P*) &wxPGGlobalVars->m_dictPropertyClassInfo;

    const wxString* pClassname = &classname;
    wxString s;

    // Translate to long name, if necessary
    if ( pClassname->GetChar(0) != wxT('w') || pClassname->GetChar(1) == wxT('x') )
    {
        if ( classname != wxT("Category") )
            s.Printf(wxT("wx%sProperty"),pClassname->c_str());
        else
            s = wxT("wxPropertyCategory");
        pClassname = &s;
    }

    wxPGHashMapS2P::iterator it;
    it = cis->find(*pClassname);

    if ( it != cis->end() )
    {
        wxPGPropertyClassInfo* pci = (wxPGPropertyClassInfo*) it->second;
        wxPGProperty* p = pci->m_constructor(label,name);
        return p;
    }
    wxLogError(wxT("No such property class: %s"),pClassname->c_str());
    return (wxPGProperty*) NULL;
}

/*
// create property from text
wxPGProperty* wxPropertyContainerMethods::CreateProperty(const wxString &label,
                                                         const wxString &name,
                                                         wxPGValueType* valuetype,
                                                         const wxString &value)
{
    wxPGProperty* p = valuetype->GenerateProperty(label,name);
#ifdef __WXDEBUG__
    if ( !p )
    {
        wxLogDebug (wxT("WARNING: wxPropertyGridMethods::CreateProperty generated NULL property for ValueType \"%s\""),valuetype->GetTypeName());
        return (wxPGProperty*) NULL;
    }
#endif
    if ( value.length() )
        p->SetValueFromString(value,wxPG_FULL_VALUE);

    return p;
}
*/

// -----------------------------------------------------------------------

// lazy way to prevent RegisterPropertyClass infinite recursion
static int gs_registering_standard_props = 0;

bool wxPropertyContainerMethods::RegisterPropertyClass (const wxChar* name,
                                                        wxPGPropertyClassInfo* classinfo)
{

    // Standard classes must be registered first!
    if ( !gs_registering_standard_props &&
         wxPGGlobalVars->m_dictPropertyClassInfo.empty()
       )
        wxPGRegisterStandardPropertyClasses();

    wxPGHashMapS2P::iterator it;

    it = wxPGGlobalVars->m_dictPropertyClassInfo.find(name);

    // only register if not registered already
    if ( it == wxPGGlobalVars->m_dictPropertyClassInfo.end() )
    {
        wxPGGlobalVars->m_dictPropertyClassInfo[name] = classinfo;
        return true;
    }

    wxLogDebug(wxT("WARNING: Property with name \"%s\" was already registered."),name);

    return false;
}

static void wxPGRegisterStandardPropertyClasses()
{

    if ( gs_registering_standard_props )
        return;

    gs_registering_standard_props = 1; // no need to reset this

    wxPGRegisterPropertyClass(wxStringProperty);
    wxPGRegisterPropertyClass(wxIntProperty);
    wxPGRegisterPropertyClass(wxFloatProperty);
    wxPGRegisterPropertyClass(wxBoolProperty);
    wxPGRegisterPropertyClass(wxEnumProperty);
    wxPGRegisterPropertyClass(wxFlagsProperty);
    wxPGRegisterPropertyClass(wxLongStringProperty);

    wxPGRegisterPropertyClass(wxPropertyCategory);
    wxPGRegisterPropertyClass(wxParentProperty);
    wxPGRegisterPropertyClass(wxCustomProperty);

    // TODO: Are these really "standard" ?
    wxPGRegisterPropertyClass(wxArrayStringProperty);
    wxPGRegisterPropertyClass(wxFileProperty);
    wxPGRegisterPropertyClass(wxDirProperty);

}

// -----------------------------------------------------------------------
// wxPropertyGridState
// -----------------------------------------------------------------------

// reset helper macro
#undef FROM_STATE
#define FROM_STATE(A) A

// -----------------------------------------------------------------------
// wxPropertyGridState item iteration methods
// -----------------------------------------------------------------------

// Skips categories and sub-properties (unless in wxParentProperty).
wxPGId wxPropertyGridState::GetFirstProperty() const
{
    if ( !m_properties->GetCount() ) return wxPGIdGen((wxPGProperty*)NULL);
    wxPGProperty* p = m_properties->Item(0);
    int parenting = p->GetParentingType();
    if ( parenting > 0 || parenting == -2 )
        return GetNextProperty ( wxPGIdGen(p) );
    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

// Skips categories and sub-properties (unless in wxParentProperty).
wxPGId wxPropertyGridState::GetNextProperty ( wxPGId id ) const
{
    wxPGPropertyWithChildren* p = (wxPGPropertyWithChildren*)wxPGIdToPtr(id);

    // Go with first child?
    int parenting = p->GetParentingType();
    if ( parenting == 0 || parenting == -1 || !p->GetCount() )
    {
        // No...

        wxPGPropertyWithChildren* parent = p->m_parent;

        // As long as last item, go up and get parent' sibling
        while ( p->m_arrIndex >= (parent->GetCount()-1) )
        {
            p = parent;
            if ( p == m_properties ) return wxPGIdGen((wxPGProperty*)NULL);
            parent = parent->m_parent;
        }

        p = (wxPGPropertyWithChildren*)parent->Item(p->m_arrIndex+1);

        // Go with the next sibling of parent's parent?
    }
    else
    {
        // Yes...
        p = (wxPGPropertyWithChildren*)p->Item(0);
    }

    // If it's category or parentproperty, then go recursive
    parenting = p->GetParentingType();
    if ( parenting > 0 || parenting == -2 )
        return GetNextProperty ( wxPGIdGen (p) );

    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetNextSibling ( wxPGId id )
{
    wxASSERT ( wxPGIdIsOk(id) );
    wxPGProperty* p = wxPGIdToPtr(id);
    wxPGPropertyWithChildren* parent = p->m_parent;
    size_t next_ind = p->m_arrIndex + 1;
    if ( next_ind >= parent->GetCount() ) return wxPGIdGen((wxPGProperty*)NULL);
    return wxPGIdGen(parent->Item(next_ind));
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetPrevSibling ( wxPGId id )
{
    wxPGProperty* p = wxPGIdToPtr(id);
    size_t ind = p->m_arrIndex;
    if ( ind < 1 ) return wxPGIdGen((wxPGProperty*)NULL);
    return wxPGIdGen(p->m_parent->Item(ind-1));
}

// -----------------------------------------------------------------------

// Skips categories and sub-properties (unless in wxParentProperty).
wxPGId wxPropertyGridState::GetPrevProperty ( wxPGId id ) const
{
    wxPGPropertyWithChildren* p = (wxPGPropertyWithChildren*)wxPGIdToPtr(id);
    wxPGPropertyWithChildren* parent = p->m_parent;

    // Is there a previous sibling?
    if ( p->m_arrIndex > 0 )
    {
        // There is!
        p = (wxPGPropertyWithChildren*)parent->Item ( p->m_arrIndex-1 );
        int parenting = p->GetParentingType();

        // Do we return it's last child?
        while ( (parenting > 0 || parenting == -2) && p->GetCount() )
        {
            p = (wxPGPropertyWithChildren*)p->Last();
            parenting = p->GetParentingType();
        }
    }
    else if ( parent != m_properties )
    // Return parent if it isnt' the root
        p = parent;
    else
        return wxPGIdGen((wxPGProperty*)NULL);

    // Skip category and parentproperty.
    int parenting = p->GetParentingType();
    if ( parenting > 0 || parenting == -2 )
        return GetPrevProperty ( wxPGIdGen(p) );

    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetFirstCategory () const
{
    //if ( IsInNonCatMode() )
    //    return wxPGIdGen((wxPGProperty*)NULL);

    wxPGProperty* found = (wxPGProperty*)NULL;

    size_t i;
    for ( i=0; i<m_regularArray.GetCount(); i++ )
    {
        wxPGProperty* p = m_regularArray.Item(i);
        if ( p->GetParentingType() > 0 )
        {
            found = p;
            break;
        }
    }
    return wxPGIdGen(found);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetNextCategory ( wxPGId id ) const
{
    wxPGPropertyWithChildren* current = (wxPGPropertyWithChildren*)wxPGIdToPtr(id);

    wxCHECK_MSG( !IsInNonCatMode() || current->GetParentingType() == 1, wxPGIdGen((wxPGProperty*)NULL),
        wxT("GetNextCategory should not be called with non-category argument in non-categoric mode.") );

    wxPGPropertyWithChildren* parent = current->m_parent;
    wxPGProperty* found = (wxPGProperty*) NULL;
    size_t i;

    // Find sub-category, if any.
    if ( current->GetParentingType() > 0 )
    {
        // Find first sub-category in current's array.
        for ( i = 0; i<current->GetCount(); i++ )
        {
            wxPGProperty* p = current->Item(i);
            if ( p->GetParentingType() > 0 )
            {
                found = p;
                break;
            }
        }
        if ( found )
            return wxPGIdGen(found);
    }

    // Find next category in parent's array.
    // (and go up in hierarchy until one found or
    // top is reached).
    do
    {
        for ( i = current->m_arrIndex+1; i<parent->GetCount(); i++ )
        {
            wxPGProperty* p = parent->Item(i);
            if ( p->GetParentingType() > 0 )
            {
                found = p;
                break;
            }
        }
        current = parent;
        parent = parent->m_parent;
    } while ( !found && parent );

    return wxPGIdGen(found);
}

// -----------------------------------------------------------------------
// wxPropertyGridState GetPropertyXXX methods
// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::GetPropertyByLabel ( const wxString& label, wxPGPropertyWithChildren* parent ) const
{

    size_t i;

    if ( !parent ) parent = (wxPGPropertyWithChildren*) &m_regularArray;

    for ( i=0; i<parent->GetCount(); i++ )
    {
        wxPGProperty* p = parent->Item(i);
        if ( p->m_label == label )
            return wxPGIdGen(p);
        // Check children recursively.
        if ( p->GetParentingType() != 0 )
        {
            p = wxPGIdToPtr(GetPropertyByLabel(label,(wxPGPropertyWithChildren*)p));
            if ( p )
                return wxPGIdGen(p);
        }
    }

    return wxPGIdGen((wxPGProperty*) NULL);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::BaseGetPropertyByName ( wxPGNameStr name ) const
{
    wxPGHashMapS2P::const_iterator it;
    it = m_dictName.find(name);
    if ( it != m_dictName.end() )
        return wxPGIdGen( (wxPGProperty*) it->second );
    return wxPGIdGen( (wxPGProperty*) NULL );
}

// -----------------------------------------------------------------------
// wxPropertyGridState global operations
// -----------------------------------------------------------------------

bool wxPropertyGridState::EnableCategories ( bool enable )
{
    ITEM_ITERATION_VARIABLES

    if ( enable )
    {
        //
        // Enable categories
        //

        if ( !IsInNonCatMode() )
            return FALSE;

        m_properties = &m_regularArray;

        // fix parents and indexes
        ITEM_ITERATION_INIT_FROM_THE_TOP

        ITEM_ITERATION_LOOP_BEGIN

            p->m_arrIndex = i;

            p->m_parent = parent;

        ITEM_ITERATION_LOOP_END

    }
    else
    {
        //
        // Disable categories
        //

        if ( IsInNonCatMode() )
            return FALSE;

        // Create array, if necessary.
        if ( !m_abcArray )
            InitNonCatMode ();

        m_properties = m_abcArray;

        // fix parents and indexes
        ITEM_ITERATION_INIT_FROM_THE_TOP

        ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN

            p->m_arrIndex = i;

            p->m_parent = parent;

        ITEM_ITERATION_DCAE_ISP_LOOP_END

    }

    return TRUE;
}

// -----------------------------------------------------------------------

static int wxPG_SortFunc(void **p1, void **p2)
{
    wxPGProperty *pp1 = *((wxPGProperty**)p1);
    wxPGProperty *pp2 = *((wxPGProperty**)p2);
    return pp1->GetLabel().Cmp ( pp2->GetLabel().c_str() );
}

void wxPropertyGridState::Sort ( wxPGProperty* p )
{
    if ( !p )
        p = (wxPGProperty*)m_properties;

    wxASSERT( p->GetParentingType() != 0 );

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

    // Can only sort items with children
    if ( pwc->m_children.GetCount() < 1 )
        return;

    pwc->m_children.Sort ( wxPG_SortFunc );

    // Fix indexes
    pwc->FixIndexesOfChildren();

}

// -----------------------------------------------------------------------

void wxPropertyGridState::Sort ()
{
    Sort ( m_properties );

    // Sort categories as well
    if ( !IsInNonCatMode() )
    {
        size_t i;
        for ( i=0;i<m_properties->GetCount();i++)
        {
            wxPGProperty* p = m_properties->Item(i);
            if ( p->GetParentingType() > 0 )
                Sort ( p );
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridState::ExpandAll ( unsigned char do_expand )
{
    ITEM_ITERATION_DCAE_VARIABLES

    if (m_pPropGrid->GetState() == this &&
        m_selected &&
        m_selected->GetParent() != m_properties)
    {
        m_pPropGrid->ClearSelection();
    }

    if ( !do_expand )
        m_selected = (wxPGProperty*) NULL;

    ITEM_ITERATION_INIT_FROM_THE_TOP

    ITEM_ITERATION_DCAE_LOOP_BEGIN

        if ( parenting != 0 )
            ((wxPGPropertyWithChildren*)p)->m_expanded = do_expand;

    ITEM_ITERATION_DCAE_LOOP_END

    if ( m_pPropGrid->GetState() == this )
    {
        m_pPropGrid->CalculateYs((wxPGPropertyWithChildren*)NULL,-1);

        m_pPropGrid->RedrawAllVisible();
    }
}

// -----------------------------------------------------------------------
// wxPropertyGridState property value setting and getting
// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropVal( wxPGProperty* p, wxPGVariant value )
{
    p->DoSetValue(value);
    if ( m_selected==p && this==m_pPropGrid->GetState() )
        p->UpdateControl(m_pPropGrid->m_wndPrimary);
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::ClearPropertyValue ( wxPGProperty* p )
{
    if ( p )
    {
        const wxPGValueType* valueclass = p->GetValueType();

        //wxLogDebug ( wxT("pValueClass: %X"),(unsigned int)valueclass);
        //wxLogDebug ( wxT("pValueClass(long): %X"),(unsigned int)wxPG_VALUETYPE(long));

        if ( valueclass != wxPG_VALUETYPE(none) )
        {
            //wxLogDebug ( valueclass->GetTypeName() );
            // wnd_primary has to be given so the editor control can be updated as well.
            SetPropVal(p,valueclass->GetDefaultValue());

            return TRUE;
        }
    }
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue ( wxPGProperty* p, const wxPGValueType* typeclass, wxPGVariant value )
{
    if ( p )
    {
        //wxLogDebug(wxT("property's type: %s"),p->GetValueType()->GetTypeName());
        //wxLogDebug(wxT("function's type: %s"),typeclass->GetTypeName());
        if ( p->GetValueType()->GetTypeName() == typeclass->GetTypeName() )
        {
            ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);

            SetPropVal(p,value);

            return TRUE;
        }
        wxPGTypeOperationFailed ( p, typeclass->GetTypeName(), wxT("Set") );
    }
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue ( wxPGProperty* p, const wxChar* typestring, wxPGVariant value )
{
    if ( p )
    {
        if ( wxStrcmp(p->GetValueType()->GetCustomTypeName(),typestring) == 0 )
        {
            // wnd_primary has to be given so the control can be updated as well.
            ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
            SetPropVal(p,value);
            return TRUE;
        }
        wxPGTypeOperationFailed ( p, typestring, wxT("Set") );
    }
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue ( wxPGProperty* p, const wxString& value )
{
    if ( p )
    {
        ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
        p->SetValueFromString ( value,
            wxPG_REPORT_ERROR|wxPG_FULL_VALUE );

        if ( m_selected==p && this==m_pPropGrid->GetState() )
            p->UpdateControl(m_pPropGrid->m_wndPrimary);

        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue ( wxPGProperty* p, wxVariant& value )
{
    if ( p )
    {
        ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
        p->GetValueType()->SetValueFromVariant(p,value);
        if ( m_selected==p && this==m_pPropGrid->GetState() )
            p->UpdateControl(m_pPropGrid->m_wndPrimary);

        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::SetPropertyValue ( wxPGProperty* p, wxObject* value )
{
    if ( p )
    {
        if ( wxStrcmp (p->GetValueType()->GetTypeName(),
                       value->GetClassInfo()->GetClassName()
                      ) == 0
           )
        {
            ClearPropertyAndChildrenFlags(p,wxPG_PROP_UNSPECIFIED);
            // wnd_primary has to be given so the control can be updated as well.
            SetPropVal(p,wxPGVariantFromWxObject(value));
            return TRUE;
        }
        wxPGTypeOperationFailed ( p, wxT("wxObject"), wxT("Set") );
    }
    return FALSE;
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyValueUnspecified ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );

    if ( !(p->m_flags & wxPG_PROP_UNSPECIFIED) )
    {
        // Flag should be set first - editor class methods may need it.
        p->m_flags |= wxPG_PROP_UNSPECIFIED;

        wxASSERT ( m_pPropGrid );

        if ( m_pPropGrid->GetState() == this )
        {
            if ( m_pPropGrid->m_selected == p && m_pPropGrid->m_wndPrimary )
            {
                p->GetEditorClass()->SetValueToUnspecified(m_pPropGrid->m_wndPrimary);
            }
        }

        if ( p->GetParentingType() != 0 )
        {
            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

            size_t i;
            for ( i = 0; i < pwc->GetCount(); i++ )
                SetPropertyValueUnspecified ( pwc->Item(i) );
        }
    }

}

// -----------------------------------------------------------------------
// wxPropertyGridState property operations
// -----------------------------------------------------------------------

void wxPropertyGridState::LimitPropertyEditing ( wxPGProperty* p, bool limit )
{
    if ( p )
    {
        if ( limit )
            p->m_flags |= wxPG_PROP_NOEDITOR;
        else
            p->m_flags &= ~(wxPG_PROP_NOEDITOR);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridState::ClearModifiedStatus ( wxPGProperty* p )
{
    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

    if ( p->m_flags & wxPG_PROP_MODIFIED )
    {
        p->m_flags &= ~(wxPG_PROP_MODIFIED);

        if ( m_pPropGrid->GetState() == this )
        {
            // Clear active editor bold
            if ( p == m_selected && m_pPropGrid->m_wndPrimary )
            #if wxPG_USE_CUSTOM_CONTROLS
                m_pPropGrid->m_wndPrimary->ResetFont ();
            #else
                m_pPropGrid->m_wndPrimary->SetFont ( m_pPropGrid->GetFont() );
            #endif

            m_pPropGrid->DrawItem ( p );
        }
    }

    if ( pwc->GetParentingType() != 0 )
    {
        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            ClearModifiedStatus ( pwc->Item(i) );
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::Collapse ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return FALSE;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return FALSE;

    if ( !pwc->m_expanded ) return FALSE;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 0;

    return TRUE;
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::Expand ( wxPGProperty* p )
{
    wxASSERT_MSG ( p, wxT("Cannot call this method with invalid property id") );
    if ( !p ) return FALSE;

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
    if ( pwc->GetParentingType() == 0 ) return FALSE;

    if ( pwc->m_expanded ) return FALSE;

    // m_expanded must be set just before call to CalculateYs
    pwc->m_expanded = 1;

    return TRUE;
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyLabel( wxPGProperty* p, const wxString& newlabel )
{
    wxASSERT(p);
    p->SetLabel(newlabel);
    if ( m_pPropGrid->GetWindowStyleFlag() & wxPG_AUTO_SORT )
        Sort(p->GetParent());
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyPriority ( wxPGProperty* p, int priority )
{
    int parenting = p->GetParentingType();

    if ( priority == wxPG_HIGH ) p->ClearFlag( wxPG_PROP_HIDEABLE );
    else p->SetFlag( wxPG_PROP_HIDEABLE );

    if ( parenting != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;
        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            SetPropertyPriority(pwc->Item(i),priority);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyAndChildrenFlags ( wxPGProperty* p, long flags )
{
    p->m_flags |= flags;

    if ( p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            ClearPropertyAndChildrenFlags ( pwc->Item(i), flags );
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridState::ClearPropertyAndChildrenFlags ( wxPGProperty* p, long flags )
{
    p->m_flags &= ~(flags);

    if ( p->GetParentingType() != 0 )
    {
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            ClearPropertyAndChildrenFlags ( pwc->Item(i), flags );
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridState::EnableProperty ( wxPGProperty* p, bool enable )
{
    if ( p )
    {
        if ( enable )
        {
            if ( !(p->m_flags & wxPG_PROP_DISABLED) )
                return FALSE;

            // Enabling

            p->m_flags &= ~(wxPG_PROP_DISABLED);
        }
        else
        {
            if ( p->m_flags & wxPG_PROP_DISABLED )
                return FALSE;

            // Disabling

            p->m_flags |= wxPG_PROP_DISABLED;

        }

        if ( p->GetParentingType() == 0 )
            return TRUE;

        // Apply same to sub-properties as well
        wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)p;

        size_t i;
        for ( i = 0; i < pwc->GetCount(); i++ )
            EnableProperty ( pwc->Item(i), enable );

        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------
// wxPropertyGridState wxVariant related routines
// -----------------------------------------------------------------------

// Returns list of wxVariant objects (non-categories and non-sub-properties only).
// Never includes sub-properties (unless they are parented by wxParentProperty).
wxVariant wxPropertyGridState::GetPropertyValues ( const wxString& listname, wxPGId baseparent, long flags ) const
{
    ITEM_ITERATION_DCAE_VARIABLES

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)wxPGIdToPtr(baseparent);

    // Root is the default base-parent.
    if ( !pwc )
        pwc = m_properties;

    wxList temp_list;
    wxVariant v( temp_list, listname );

    if ( flags & wxKEEP_STRUCTURE )
    {
        wxASSERT ( (pwc->GetParentingType() < -1) || (pwc->GetParentingType() > 0) );

        size_t i;
        for ( i=0; i<pwc->GetCount(); i++ )
        {
            wxPGProperty* p = pwc->Item(i);
            int parenting = p->GetParentingType();
            if ( parenting == 0 || parenting == -1 )
            {
                v.Append ( p->GetValueAsVariant() );
            }
            else
                v.Append ( GetPropertyValues(p->m_name,wxPGIdGen(p),wxKEEP_STRUCTURE) );
        }
    }
    else
    {
        ITEM_ITERATION_INIT((wxPGPropertyWithChildren*)wxPGIdToPtr(baseparent),0)
        ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN

            // Use a trick to ignore wxParentProperty itself, but not its sub-properties.
            if ( parenting == -2 )
            {
                parenting = 1;
            }
            else if ( parenting <= 0 )
            {
                //wxLogDebug ( wxT("label: %s"), p->GetLabel().c_str() );
                v.Append ( p->GetValueAsVariant() );
            }

        ITEM_ITERATION_DCAE_ISP_LOOP_END
    }

    return v;
}

// -----------------------------------------------------------------------

/*
wxPGProperty* wxPropertyGridState::NewProperty (const wxString &label,
                                                const wxString &name,
                                                wxVariant& value )
{
    //wxLogDebug (wxT("NewProperty: %s"),label.c_str());

    wxString variant_type = value.GetType();

    wxPGHashMapS2P::iterator it;

    it = wxPGGlobalVars->m_dictValueType.find(variant_type);

    if ( it != wxPGGlobalVars->m_dictValueType.end() )
    {
        wxPGValueType* vt = (wxPGValueType*) it->second;
        wxPGProperty* p = vt->GenerateProperty(label,name);
    #ifdef __WXDEBUG__
        if ( !p )
        {
            wxLogDebug (wxT("WARNING: wxPropertyGrid::NewProperty generated NULL property for ValueType \"%s\""),variant_type.c_str());
            return (wxPGProperty*) NULL;
        }
    #endif
        vt->SetValueFromVariant(p,value);
        return p;
    }

    wxLogDebug(wxT("WARNING: No value type registered with name \"%s\""),variant_type.c_str());
    return (wxPGProperty*) NULL;
}
*/

// -----------------------------------------------------------------------

void wxPropertyGridState::SetPropertyValues ( const wxList& list, wxPGId default_category )
{

    unsigned char orig_frozen = 1;

    if ( m_pPropGrid->GetState() == this )
    {
        orig_frozen = m_pPropGrid->m_frozen;
        if ( !orig_frozen ) m_pPropGrid->Freeze();
    }

    wxPropertyCategoryClass* use_category = (wxPropertyCategoryClass*)wxPGIdToPtr(default_category);

    if ( !use_category )
        use_category = (wxPropertyCategoryClass*)m_properties;

    // Let's iterate over the list of variants.
    wxList::const_iterator node;

    //for ( wxList::Node *node = list.GetFirst(); node; node = node->GetNext() )
    for ( node = list.begin(); node != list.end(); node++ )
    {
        //wxVariant *current = (wxVariant*)node->GetData();
        wxVariant *current = (wxVariant*)*node;

        // Make sure it is wxVariant.
        wxASSERT ( current );
        wxASSERT ( wxStrcmp(current->GetClassInfo()->GetClassName(),wxT("wxVariant")) == 0 );

        if ( current->GetName().length() > 0 )
        {
            wxPGId found_prop = BaseGetPropertyByName(current->GetName());
            if ( wxPGIdIsOk(found_prop) )
            {
                wxPGProperty* p = wxPGIdToPtr(found_prop);

                const wxPGValueType* vtype = p->GetValueType();

                // If it was a list, we still have to go through it.
                if ( current->GetType() == wxT("list") )
                {
                    SetPropertyValues( current->GetList(),
                        wxPGIdGen(
                            p->GetParentingType()>0?p:((wxPGProperty*)NULL)
                        ) );
                }
                else //if ( current->GetType() == vtype->GetTypeName() )
                {
            #ifdef __WXDEBUG__
                    if ( current->GetType() != vtype->GetTypeName() &&
                         current->GetType() != vtype->GetCustomTypeName() )
                    {
                        wxLogDebug(wxT("wxPropertyGridState::SetPropertyValues Warning: Setting value of property \"%s\" from variant"),
                            p->m_name.c_str());
                        wxLogDebug(wxT("    but variant's type name (%s) doesn't match either base type name (%s) nor custom type name (%s)."),
                            current->GetType().c_str(),vtype->GetTypeName(),
                            vtype->GetCustomTypeName());
                    }
            #endif

                    vtype->SetValueFromVariant(p,*current);

                    //wxLogDebug(wxT("m_selected=%s"),m_selected->GetLabel().c_str());
                }
            }
            else
            {
                // Is it list?
                if ( current->GetType() != wxT("list") )
                {
                    // Not.
                    AppendIn(use_category,current->GetName(),wxPG_LABEL,(wxVariant&)*current);
                }
                else
                {
                    // Yes, it is; create a sub category and append contents there.
                    wxPGId new_cat = Insert(use_category,-1,new wxPropertyCategoryClass(current->GetName(),wxPG_LABEL));
                    SetPropertyValues( current->GetList(), new_cat );
                }
            }
        }
    }

    if ( !orig_frozen )
    {
        m_pPropGrid->Thaw();

        if ( this == m_pPropGrid->GetState() )
        {
            m_selected->UpdateControl(m_pPropGrid->m_wndPrimary);
        }
    }

}

// -----------------------------------------------------------------------
// wxPropertyGridState property adding and removal
// -----------------------------------------------------------------------

int wxPropertyGridState::PrepareToAddItem (wxPGProperty* property,
                                           wxPGPropertyWithChildren* scheduled_parent)
{
    wxPropertyGrid* propgrid = m_pPropGrid;
    wxASSERT ( propgrid );

    int parenting = property->GetParentingType();

    // This will allow better behaviour.
    if ( scheduled_parent == m_properties )
        scheduled_parent = (wxPGPropertyWithChildren*) NULL;

    if ( parenting > 0 )
    {
        // Parent of a category must be either root or another category
        // (otherwise Bad Stuff might happen).
        wxASSERT_MSG ( scheduled_parent == (wxPGPropertyWithChildren*) NULL ||
                       scheduled_parent == m_properties ||
                       scheduled_parent->GetParentingType() > 0,
                 wxT("Parent of a category must be either root or another category."));

        /*
        wxASSERT_MSG ( m_properties == &m_regularArray,
                wxT("Do not add categories in non-categoric mode!"));
        */

        // If we already have category with same name, delete given property
        // and use it instead as most recent caption item.
        wxPGId found_id = BaseGetPropertyByName ( property->GetName() );
        if ( wxPGIdIsOk(found_id) )
        {
            wxPropertyCategoryClass* pwc = (wxPropertyCategoryClass*)wxPGIdToPtr(found_id);
            if ( pwc->GetParentingType() > 0 ) // Must be a category.
            {
                delete property;
                m_currentCategory = pwc;
                return 2; // Tells the caller what we did.
            }
        }
    }

#ifdef __WXDEBUG__
    // Warn for identical names in debug mode.
    if ( property->GetName().length() && BaseGetPropertyByName(property->GetName()).IsOk() )
        wxLogError(wxT("wxPropertyGrid: Warning - item with name \"%s\" already exists."),
            property->GetName().c_str());
#endif

    // Make sure nothing is selected.
    if ( propgrid && propgrid->m_selected )
        propgrid->ClearSelection();

    property->m_y = -1;

    if ( scheduled_parent )
    {
        // Use parent's background colour.
        property->m_bgColIndex = scheduled_parent->m_bgColIndex;
    }

    // If in hideable adding mode, or if assigned parent is hideable, then
    // make this one hideable.
    if (
         ( scheduled_parent && (scheduled_parent->m_flags & wxPG_PROP_HIDEABLE) ) ||
         ( propgrid && (propgrid->m_iFlags & wxPG_FL_ADDING_HIDEABLES) )
       )
        property->SetFlag ( wxPG_PROP_HIDEABLE );

    // Set custom image flag.
    if ( property->GetImageSize().y < 0 )
    {
        property->m_flags |= wxPG_PROP_CUSTOMIMAGE;
    }

    if ( propgrid->GetWindowStyleFlag() & wxPG_LIMITED_EDITING )
        property->m_flags |= wxPG_PROP_NOEDITOR;

    if ( parenting < 1 )
    {
        // This is not a category.

        wxASSERT_MSG ( property->GetEditorClass(), wxT("Editor class not initialized!") );

        // Depth.
        unsigned char depth = 1;
        if ( scheduled_parent )
        {
            depth = scheduled_parent->m_depth;
            if ( scheduled_parent->GetParentingType() < 0 )
                depth++;
        }
        property->m_depth = depth;
        unsigned char grey_depth = depth;

        //= (wxPropertyCategoryClass*) NULL;
        if ( scheduled_parent )
        {
            wxPropertyCategoryClass* pc;

            if ( scheduled_parent->GetParentingType() > 0 )
                pc = (wxPropertyCategoryClass*)scheduled_parent;
            else
                // This conditional compile is necessary because
                // otherwise things get too complex for some compilers.
                pc = wxPropertyGrid::GetPropertyCategory(scheduled_parent);

            if ( pc )
                grey_depth = pc->GetDepth();
            else
                grey_depth = scheduled_parent->m_depthBgCol;
        }

        //wxLogDebug(wxT("__%s__: %i"),property->m_label.c_str(),(int)grey_depth);

        property->m_depthBgCol = grey_depth;

        // Add children to propertywithchildren.
        if ( parenting < 0 )
        {
            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)property;

            pwc->m_parentState = this;

            pwc->m_expanded = 0; // Properties with children are not expanded by default.
            if ( propgrid && propgrid->GetWindowStyleFlag() & wxPG_HIDE_MARGIN )
                pwc->m_expanded = 1; // ...unless it cannot not be expanded.

            //
            // If children were added prior to append, then this is considered
            // a "fixed" parent (otherwise the -2 is set, see below, to mark
            // its customizable).
            if ( pwc->GetCount() )
            {

                depth++;

                unsigned char inheritFlags =
                    property->m_flags & (wxPG_PROP_HIDEABLE|wxPG_PROP_NOEDITOR);

                unsigned char bgcolind = property->m_bgColIndex;

                //
                // Set some values to the children
                //
                size_t i = 0;
                wxPGPropertyWithChildren* nparent = pwc;

                while ( i < nparent->GetCount() )
                {
                    wxPGProperty* np = nparent->Item(i);

                    np->m_y = -1; // Collapsed.
                    np->m_flags |= inheritFlags; // Hideable also if parent.
                    np->m_depth = depth;
                    np->m_depthBgCol = grey_depth;
                    np->m_bgColIndex = bgcolind;

                    // Set custom image flag.
                    if ( np->GetImageSize().y < 0 )
                        np->m_flags |= wxPG_PROP_CUSTOMIMAGE;

                    // Also handle children of children
                    if ( np->GetParentingType() != 0 &&
                         ((wxPGPropertyWithChildren*)np)->GetCount() > 0 )
                    {
                        nparent = (wxPGPropertyWithChildren*) np;
                        i = 0;

                        // Init
                        nparent->m_expanded = 0;
                        nparent->m_parentState = this;
                        depth++;
                    }
                    else
                    {
                        // Next sibling
                        i++;
                    }

                    // After reaching last sibling, go back to processing
                    // siblings of the parent
                    while ( i >= nparent->GetCount() )
                    {
                        // Exit the loop when top parent hit
                        if ( nparent == pwc )
                            break;

                        depth--;

                        i = nparent->GetArrIndex() + 1;
                        nparent = nparent->GetParent();
                    }
                }

            }
            else
            {
                pwc->m_parentingType = -2;
            }

        }

    }
    else
    {
        // This is a category.

        // depth
        unsigned char depth = 1;
        if ( scheduled_parent )
        {
            depth = scheduled_parent->m_depth + 1;
        }
        property->m_depth = depth;
        property->m_depthBgCol = depth;

        m_currentCategory = (wxPropertyCategoryClass*)property;

        wxPropertyCategoryClass* pc = (wxPropertyCategoryClass*)property;
        pc->m_parentState = this;

        // Calculate text extent for caption item.
        pc->CalculateTextExtent(propgrid,*propgrid->GetCaptionFont());

    }

    return parenting;
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::Append ( wxPGProperty* property )
{
    wxPropertyCategoryClass* cur_cat = m_currentCategory;
    if ( property->GetParentingType() > 0 )
        cur_cat = (wxPropertyCategoryClass*) NULL;

    return Insert ( cur_cat, -1, property );
    /*
    wxPropertyGrid* propgrid = m_pPropGrid;

    wxPropertyCategoryClass* cur_cat = m_currentCategory;
    if ( property->GetParentingType() > 0 )
        cur_cat = (wxPropertyCategoryClass*) NULL;

    // Major slowdown: if last caption is not the bottommost,
    //   we have to insert instead of append.
    if ( cur_cat == m_currentCategory && !m_lastCaptionBottomnest )
    {
        return Insert ( propgrid, cur_cat, -1, property );
    }

#if __INTENSE_DEBUGGING__
    wxLogDebug ( wxT("wxPropertyGridState::Append( \"%s\" )"),property->m_label.c_str());
#endif

    int parenting = PrepareToAddItem ( propgrid, property, cur_cat );

    // this type of invalid parenting value indicates we should exit now, returning
    // id of most recent category.
    if ( parenting > 1 )
        return wxPGIdGen(m_currentCategory);

    // note that item must be added into current mode later

    // Generate Y. Conditions:
    // - Y already added or first item.
    // - Not category or categoric mode.
    // - Scheduled category, if any, must be expanded.
    // - Not hideable or not in compact mode.
    if ( propgrid )
    {
        if ( (propgrid->m_bottomy || !m_itemsAdded) &&
             ( m_properties == &m_regularArray || parenting < 0 ) &&
             ( !cur_cat || cur_cat->IsExpanded() ) &&
             ( !(propgrid->m_iFlags & wxPG_FL_HIDE_STATE) || !property->IsFlagSet(wxPG_PROP_HIDEABLE) )
           )
        {
            property->m_y = propgrid->m_bottomy;
            propgrid->m_bottomy += propgrid->m_lineHeight;
        }
    }

    if ( parenting < 1 )
    {
        // This is not a category.

        // Maybe add to alternative array
        if ( m_properties == &m_regularArray )
            if ( m_abcArray )
                m_abcArray->AddChild ( property );

        // if caption item available, set it as parent
        if ( cur_cat )
        {

            cur_cat->AddChild ( property );

            if ( !cur_cat->IsExpanded() )
                property->m_y = -1;

#if wxPG_USE_CATEGORY_ARRAY
            property->SetCatIndex ( ((wxPropertyCategoryClass*)cur_cat)->m_ctIndex );
#endif
        }
        else
        {
            m_regularArray.AddChild ( property );
        }

        // Must add later to current array.
        if ( m_properties != &m_regularArray )
        {
            wxASSERT ( m_abcArray != NULL );
            m_abcArray->AddChild ( property );
        }

        // Y's for children (if this has subproperties and starts expanded)?
        if ( parenting < 0 && propgrid )
        {
            wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)property;
            if ( pwc->IsExpanded() )
            {
                unsigned int i;

                for ( i=0; i<pwc->GetCount(); i++ )
                {
                    pwc->Item(i)->m_y = propgrid->m_bottomy;
                    propgrid->m_bottomy += propgrid->m_lineHeight;
                }
            }
        }
    }
    else
    {
        // This is a category.

        m_regularArray.AddChild ( property );

        // Last caption equals bottom one
        m_lastCaptionBottomnest = 1;
    }

    //if ( m_dictLabel ) (*m_dictLabel)[property->m_label] = property;
    // Add name
    if ( property->GetName().length() ) m_dictName[property->GetName()] = property;

    m_itemsAdded = 1;

    return property->GetId();
    */
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::Insert ( wxPGPropertyWithChildren* parent, int index, wxPGProperty* property )
{
    wxASSERT ( property );

    if ( !parent )
        parent = m_properties;

    wxPropertyGrid* propgrid = m_pPropGrid;

    wxASSERT_MSG ( parent->GetParentingType() != 0,
        wxT("wxPropertyGrid: Invalid parent") );

    wxASSERT_MSG ( (parent->GetParentingType() != -1),
        wxT("wxPropertyGrid: Do not attempt to insert items under fixed parents.") );

    int parenting = PrepareToAddItem ( property, (wxPropertyCategoryClass*)parent );

    //wxLogDebug ( wxT("wxPropertyGridState::Insert ( parent: \"%s\")"),parent->GetLabel().c_str());

    // This type of invalid parenting value indicates we should exit now, returning
    // id of most recent category.
    if ( parenting > 1 )
        return wxPGIdGen(m_currentCategory);

    // Note that item must be added into current mode later.

    // If parent is wxParentProperty, just stick it in...
    // If parent is root (m_properties), then...
    //   In categoric mode: Add as last item in m_abcArray (if not category).
    //                      Add to given index in m_regularArray.
    //   In non-cat mode:   Add as last item in m_regularArray.
    //                      Add to given index in m_abcArray.
    // If parent is category, then...
    //   1) Add to given category in given index.
    //   2) Add as last item in m_abcArray.

    int parents_parenting = parent->GetParentingType();
    if ( parents_parenting < 0 )
    {
        // Parent is wxParentingProperty: Just stick it in...
        parent->AddChild ( property, index );
    }
    else
    {
        // Parent is Category or Root.

        /*
        int noncat_index = -2;

        if ( m_abcArray )
        {
            wxPGProperty* p = (wxPGProperty*) NULL;
            if ( index >= 0 && (unsigned int)index < parent->GetCount() )
                p = parent->Item(index);
            noncat_index = p?m_abcArray->Index(p):-1;
        }
        */

        if ( m_properties == &m_regularArray )
        {
            // Categorized mode

            // Only add non-categories to m_abcArray.
            if ( m_abcArray && parenting <= 0 )
                m_abcArray->AddChild ( property, -1, FALSE );

            // Add to current mode.
            parent->AddChild ( property, index );

        }
        else
        {
            // Non-categorized mode.

            if ( parent != m_properties )
                // Parent is category.
                parent->AddChild ( property, index, FALSE );
            else
                // Parent is root.
                m_regularArray.AddChild ( property, -1, FALSE );

            // Add to current mode (no categories).
            if ( parenting <= 0 )
                m_abcArray->AddChild ( property, index );
        }
    }

    // category stuff
    if ( parenting > 0 )
    {
    // This is a category caption item.

        // Last caption is not the bottom one (this info required by append)
        m_lastCaptionBottomnest = 0;
    }

    if ( property->m_name.length() ) m_dictName[property->m_name] = (void*) property;

    m_itemsAdded = 1;

    if ( propgrid )
        propgrid->m_bottomy = 0; // this signals y recalculation

    return wxPGIdGen(property);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::AppendIn (wxPGPropertyWithChildren* pwc,
                                      const wxString& label,
                                      const wxString& propname,
                                      wxVariant& value )
{
    wxPGProperty* p = wxPropertyContainerMethods::
        CreatePropertyByType(value.GetType(),label,propname);

    if ( p )
    {
        p->GetValueType()->SetValueFromVariant(p,value);
        return Insert(pwc,-1,p);
    }
    return wxPGIdGen((wxPGProperty*)NULL);
}

// -----------------------------------------------------------------------

void wxPropertyGridState::Delete ( wxPGProperty* item )
{

    wxASSERT_MSG ( item != &m_regularArray && item != m_abcArray,
        wxT("wxPropertyGrid: Do not attempt to remove the root item.") );

    size_t i;
    int parenting = item->GetParentingType();
    unsigned int indinparent = item->GetIndexInParent();

    wxPGPropertyWithChildren* pwc = (wxPGPropertyWithChildren*)item;

    wxASSERT_MSG ( item->GetParent()->GetParentingType() != -1,
        wxT("wxPropertyGrid: Do not attempt to remove sub-properties.") );

    if ( parenting > 0 )
    {
        // deleting a category

        // erase category entries from the hash table
        for ( i=0; i<pwc->GetCount(); i++ )
        {
            wxPGProperty* sp = pwc->Item( i );
            if ( sp->GetName().Len() ) m_dictName.erase( wxPGNameConv(sp->GetName()) );
        }

        if ( pwc == m_currentCategory )
            m_currentCategory = (wxPropertyCategoryClass*) NULL;

        if ( m_abcArray )
        {
        // Remove children from non-categorized array.
            for ( i=0; i<pwc->GetCount(); i++ )
            {
                wxPGProperty * p = pwc->Item( i );
                wxASSERT ( p != NULL );
                m_abcArray->m_children.Remove( (void*)p );
            }

            if ( IsInNonCatMode() )
                m_abcArray->FixIndexesOfChildren();
        }

    }

    if ( !IsInNonCatMode() )
    {
        // categorized mode - non-categorized array
        if ( parenting <= 0 )
        {
            if ( m_abcArray )
            {
                m_abcArray->m_children.Remove( item );
                m_abcArray->FixIndexesOfChildren();
            }
        }

        // categorized mode - categorized array
        item->m_parent->m_children.RemoveAt(indinparent);
        item->m_parent->FixIndexesOfChildren(/*indinparent*/);
    }
    else
    {
        // non-categorized mode - categorized array

        // We need to find location of item.
        wxPGPropertyWithChildren* cat_parent = &m_regularArray;
        int cat_index = m_regularArray.GetCount();
        size_t i;
        for ( i = 0; i < m_regularArray.GetCount(); i++ )
        {
            wxPGProperty* p = m_regularArray.Item(i);
            if ( p == item ) { cat_index = i; break; }
            if ( p->GetParentingType() > 0 )
            {
                int subind = ((wxPGPropertyWithChildren*)p)->Index(item);
                if ( subind != wxNOT_FOUND )
                {
                    cat_parent = ((wxPGPropertyWithChildren*)p);
                    cat_index = subind;
                    break;
                }
            }
        }
        cat_parent->m_children.RemoveAt(cat_index);

        // non-categorized mode - non-categorized array
        if ( parenting <= 0 )
        {
            wxASSERT ( item->m_parent == m_abcArray );
            item->m_parent->m_children.RemoveAt(indinparent);
            item->m_parent->FixIndexesOfChildren(indinparent);
        }
    }

    if ( item->GetName().Len() ) m_dictName.erase( wxPGNameConv(item->GetName()) );

    // We can actually delete it now
    delete item;

    m_itemsAdded = 1; // Not a logical assignment (but required nonetheless).

    if ( this == m_pPropGrid->GetState() )
    {
        m_pPropGrid->m_clearThisMany = 1;
        m_pPropGrid->m_bottomy = 0; // this signals y recalculation

        if ( !m_pPropGrid->IsFrozen() )
            m_pPropGrid->Refresh();
    }
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridState::ReplaceProperty( wxPGProperty* replaced, wxPGProperty* property )
{
    wxASSERT( replaced );
    wxASSERT( property );

    // Replaced must have either no or fixed children
    wxASSERT_MSG( replaced->GetParentingType() == 0 || replaced->GetParentingType() == -1,
        wxT("Property being replaced must have no children, or children must be fixed"));

    wxASSERT_MSG( !IsInNonCatMode(),
        wxT("ReplaceProperty can only be performed in categoric mode"));

    // Get address to the slot
    wxPGPropertyWithChildren* parent = replaced->GetParent();

    int ind = replaced->GetIndexInParent();

    Delete(replaced);

    Insert(parent,ind,property);

    /*
    // Replace name in hash table
    // (must be done around PrepareToAddItem)
    if ( replaced->GetName().Len() ) m_dictName.erase( wxPGNameConv(replaced->GetName()) );

    PrepareToAddItem ( property, (wxPropertyCategoryClass*)parent );

    if ( property->m_name.length() ) m_dictName[property->m_name] = (void*) property;

    // Relay some info if both are parents
    if ( replaced->GetParentingType() && property->GetParentingType() )
    {
        ((wxPGPropertyWithChildren*)property)->m_expanded = ((wxPGPropertyWithChildren*)replaced)->m_expanded;
    }

    parent->m_children[ind] = property;
    property->m_parent = parent;
    property->m_arrIndex = ind;

    // If parent is categoric root, then add to other mode as well
    if ( parent->GetParentingType() > 0 )
    {
        if ( !IsInNonCatMode() )
        {
            if ( m_abcArray )
            {
                m_abcArray->m_children.Remove(replaced);
                m_abcArray->m_children.Add(replaced);
            }
        }
    }

    wxASSERT(property->GetParent());

    delete replaced;

    // Force recalculations
    m_itemsAdded = 1;

    if ( this == m_pPropGrid->GetState() )
    {
        m_pPropGrid->m_bottomy = 0; // this signals y recalculation
        m_pPropGrid->Refresh();
    }
    */

    return wxPGIdGen(property);
}

// -----------------------------------------------------------------------
// wxPropertyGridState init etc.
// -----------------------------------------------------------------------

void wxPropertyGridState::InitNonCatMode ()
{
    ITEM_ITERATION_DCAE_VARIABLES

    if ( !m_abcArray )
    {
        m_abcArray = new wxPGRootPropertyClass();
        m_abcArray->SetParentState(this);
        m_abcArray->m_expanded = wxPG_EXP_OF_COPYARRAY;
    }

    // Must be called when FROM_STATE(m_properties) still points to regularArray.
    //wxASSERT ( FROM_STATE(m_properties) == &FROM_STATE(m_regularArray) );
    wxPGPropertyWithChildren* old_properties = m_properties;

    // Must use temp value in FROM_STATE(m_properties) for item iteration loop
    // to run as expected.
    m_properties = &m_regularArray;

    // Copy items.
    ITEM_ITERATION_INIT_FROM_THE_TOP

    ITEM_ITERATION_DCAE_ISP_LOOP_BEGIN

    if ( parenting < 1 &&
        ( parent == m_properties || parent->GetParentingType() > 0 ) )
    {

        //wxLogDebug("IN: %s",p->m_label.c_str());

        m_abcArray->AddChild ( p );
        p->m_parent = &FROM_STATE(m_regularArray);
    }
    //else wxLogDebug("OUT: %s",p->m_label.c_str());

    ITEM_ITERATION_DCAE_ISP_LOOP_END

    m_properties = old_properties;

}

// -----------------------------------------------------------------------

void wxPropertyGridState::Clear()
{
    m_regularArray.Empty();
    if ( m_abcArray )
        m_abcArray->Empty();

    m_dictName.clear();

    m_currentCategory = (wxPropertyCategoryClass*) NULL;
    m_lastCaptionBottomnest = 1;
    m_itemsAdded = 0;

    m_selected = (wxPGProperty*) NULL;
}

// -----------------------------------------------------------------------

wxPropertyGridState::wxPropertyGridState()
{
    m_pPropGrid = (wxPropertyGrid*) NULL;
    m_regularArray.SetParentState(this);
    m_properties = &m_regularArray;
    m_abcArray = (wxPGRootPropertyClass*) NULL;
    m_currentCategory = (wxPropertyCategoryClass*) NULL;
    m_selected = (wxPGProperty*) NULL;
    m_lastCaptionBottomnest = 1;
    m_itemsAdded = 0;
    m_anyModified = 0;
}

// -----------------------------------------------------------------------

wxPropertyGridState::~wxPropertyGridState()
{
    if ( m_abcArray )
        delete m_abcArray;
}

// -----------------------------------------------------------------------
// wxPropertyGridPopulator
// -----------------------------------------------------------------------

void wxPropertyGridPopulator::Init( wxPropertyGrid* pg, wxPGId popRoot )
{
    m_propGrid = pg;
    m_popRoot = popRoot;
    wxPGGlobalVars->m_offline++;
}

// -----------------------------------------------------------------------

wxPropertyGridPopulator::~wxPropertyGridPopulator()
{

    // Free unused sets of choices

    wxPGHashMapConstants* socs =
        (wxPGHashMapConstants*) wxPGGlobalVars->m_dictConstants;

    wxPGHashMapI2I::iterator it;

    for( it = m_dictIdChoices.begin(); it != m_dictIdChoices.end(); ++it )
    {
        size_t id = it->second;
        wxPGConstants* soc = wxPropertyGrid::GetConstantsArray( id );
        wxASSERT ( soc );

        if ( soc->GetRefCount() < 1 )
        {
#ifdef __WXDEBUG__
            wxLogDebug(wxT("WARNING: Set of Choices (id_in_source=0x%X,%i items,first=%s) was not referenced"),
                (unsigned int)it->first,(int)soc->GetCount(),soc->GetLabel(0).c_str() );
            wxLogDebug(wxT("  (it may be that you have tried to set choices for a property that can't have any)"));
#endif
            delete soc;
            socs->erase(id);
        }
    }

    wxPGGlobalVars->m_offline--;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::HasChoices( size_t id ) const
{
    wxPGHashMapI2I::const_iterator it = m_dictIdChoices.find(id);
    return ( it != m_dictIdChoices.end() );
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::BeginChildren()
{
    if (m_lastProperty.IsOk() &&
        wxPGIdToPtr(m_lastProperty)->CanHaveExtraChildren())
    {
        m_curParent = m_lastProperty;
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::AddChoices(size_t choicesId,
                                         const wxArrayString& choiceLabels,
                                         const wxArrayInt& choiceValues)
{
#ifdef __WXDEBUG__
    // Make sure the id is not used yet
    wxPGHashMapI2I::iterator it = m_dictIdChoices.find(choicesId);
    wxASSERT_MSG( it == m_dictIdChoices.end(),
        wxT("added set of choices to same id twice (use HasChoices if necessary)") );
#endif

    wxPGConstants* cons = wxPropertyGrid::CreateConstantsArray(0);
    wxASSERT(cons);

    cons->SetLabels(choiceLabels);
    if ( &choiceValues && choiceValues.GetCount() )
        cons->SetValues(choiceValues);

    m_dictIdChoices[choicesId] = cons->GetId();
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridPopulator::DoAppend(wxPGProperty* p,
                                         const wxString& value,
                                         const wxString& attributes,
                                         size_t choicesId,
                                         const wxArrayString& choiceLabels,
                                         const wxArrayInt& choiceValues)
{
    wxASSERT ( m_propGrid );

    // Make sure m_curParent is ok
    if ( !m_curParent.IsOk() )
    {
        if ( !m_popRoot.IsOk() )
            m_popRoot = m_propGrid->GetRoot();
        m_curParent = m_popRoot;
    }

    if ( p )
    {

        // Set choices
        if ( choicesId )
        {
            wxPGHashMapI2I::iterator it = m_dictIdChoices.find(choicesId);
            size_t translated_id = 0;

            if ( it != m_dictIdChoices.end() )
                translated_id = it->second;

            size_t ret_id = p->SetChoices(translated_id,choiceLabels,choiceValues);

            if ( !translated_id )
                m_dictIdChoices[choicesId] = ret_id;

            //wxLogDebug(wxT("choices_id: %u, translated_id: %u, ret_id: %u"),
            //    choices_id,translated_id,ret_id);

        }

        // Value setter must be before append
        if ( value.length() )
        {
            p->SetValueFromString(value,wxPG_FULL_VALUE);
        }

        // Set attributes
        if ( attributes.length() )
            wxPropertyGrid::SetPropertyAttributes(p,attributes);

        // Append to grid
        m_propGrid->AppendIn(m_curParent,p);

        m_lastProperty = p;
    }
    return wxPGIdGen(p);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridPopulator::AppendByClass(const wxString& classname,
                                              const wxString& label,
                                              const wxString& name,
                                              const wxString& value,
                                              const wxString& attributes,
                                              size_t choicesId,
                                              const wxArrayString& choiceLabels,
                                              const wxArrayInt& choiceValues)
{
    wxPGProperty* p = m_propGrid->CreatePropertyByClass(classname,label,name);
    return DoAppend(p,value,attributes,choicesId,choiceLabels,choiceValues);
}

// -----------------------------------------------------------------------

wxPGId wxPropertyGridPopulator::AppendByType(const wxString& valuetype,
                                             const wxString& label,
                                             const wxString& name,
                                             const wxString& value,
                                             const wxString& attributes,
                                             size_t choicesId,
                                             const wxArrayString& choiceLabels,
                                             const wxArrayInt& choiceValues)
{
    wxPGProperty* p = m_propGrid->CreatePropertyByType(valuetype,label,name);
    return DoAppend(p,value,attributes,choicesId,choiceLabels,choiceValues);
}

// -----------------------------------------------------------------------
