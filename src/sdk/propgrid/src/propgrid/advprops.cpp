/////////////////////////////////////////////////////////////////////////////
// Name:        advprops.cpp
// Purpose:     wxPropertyGrid Advanced Properties (font, colour, etc.)
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "advprops.h"
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
    #include "wx/button.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/choice.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/scrolwin.h"
    #include "wx/dirdlg.h"
    #include "wx/combobox.h"
    #include "wx/layout.h"
    #include "wx/sizer.h"
    #include "wx/textdlg.h"
    #include "wx/filedlg.h"
    #include "wx/intl.h"
#endif

#define __wxPG_SOURCE_FILE__

#include <wx/propgrid/propgrid.h>

#if wxPG_INCLUDE_ADVPROPS

#include <wx/propgrid/propdev.h>

#include <wx/propgrid/advprops.h>

// -----------------------------------------------------------------------

#if defined(__WXMSW__)
# define wxPG_CAN_DRAW_CURSOR           1
#elif defined(__WXGTK__)
# define wxPG_CAN_DRAW_CURSOR           0
#elif defined(__WXMAC__)
# define wxPG_CAN_DRAW_CURSOR           0
#else
# define wxPG_CAN_DRAW_CURSOR           0
#endif

// -----------------------------------------------------------------------
// Value type related
// -----------------------------------------------------------------------

bool operator == (const wxFont&, const wxFont&)
{
    return FALSE;
}

WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ(wxFont,wxFontProperty,(const wxFont*)NULL)

// Implement dynamic class for type value.
IMPLEMENT_DYNAMIC_CLASS(wxColourPropertyValue,wxObject)

bool operator == (const wxColourPropertyValue& a, const wxColourPropertyValue& b)
{
    return ( ( a.m_colour == b.m_colour ) && (a.m_type == b.m_type) );
}

WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ(wxColourPropertyValue,wxSystemColourProperty,
                                 (const wxColourPropertyValue*)NULL)

WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ(wxColour,wxColourProperty,
                                 (const wxColour*)wxBLACK)

bool operator == (const wxArrayInt& array1, const wxArrayInt& array2)
{
    if ( array1.GetCount() != array2.GetCount() )
        return FALSE;
    size_t i;
    for ( i=0; i<array1.GetCount(); i++ )
    {
        if ( array1[i] != array2[i] )
            return FALSE;
    }
    return TRUE;
}

WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxArrayInt,wxMultiChoiceProperty,
                                 wxArrayInt())

// -----------------------------------------------------------------------
// wxFontProperty
// -----------------------------------------------------------------------

#include <wx/fontdlg.h>

static const wxChar* gs_fp_es_family_labels[] = {
    wxT("Default"), wxT("Decorative"),
    wxT("Roman"), wxT("Script"),
    wxT("Swiss"), wxT("Modern")
};

static long gs_fp_es_family_values[] = {
    wxDEFAULT, wxDECORATIVE,
    wxROMAN, wxSCRIPT,
    wxSWISS, wxMODERN
};

static const wxChar* gs_fp_es_style_labels[] = {
    wxT("Normal"),
    wxT("Slant"),
    wxT("Italic"),
};

static long gs_fp_es_style_values[] = {
    wxNORMAL,
    wxSLANT,
    wxITALIC
};

static const wxChar* gs_fp_es_weight_labels[] = {
    wxT("Normal"),
    wxT("Light"),
    wxT("Bold")
};

static long gs_fp_es_weight_values[] = {
    wxNORMAL,
    wxLIGHT,
    wxBOLD
};

// Class body is in advprops.h

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxFontProperty,wxFont,const wxFont&,TextCtrlAndButton)

wxFontPropertyClass::wxFontPropertyClass ( const wxString& label, const wxString& name,
    const wxFont& value ) : wxPGPropertyWithChildren(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxFont)
    DoSetValue(value);

    AddChild( wxIntProperty( _("Point Size"),wxEmptyString,m_value_wxFont.GetPointSize() ) );

    AddChild( wxEnumProperty(_("Family"),wxEmptyString,
            gs_fp_es_family_labels,gs_fp_es_family_values,6,m_value_wxFont.GetFamily()) );

    AddChild( wxStringProperty(_("Face Name"),wxEmptyString, m_value_wxFont.GetFaceName()) );

    AddChild( wxEnumProperty(_("Style"),wxEmptyString,
            gs_fp_es_style_labels,gs_fp_es_style_values,3,m_value_wxFont.GetStyle()) );

    AddChild( wxEnumProperty(_("Weight"),wxEmptyString,
            gs_fp_es_weight_labels,gs_fp_es_weight_values,3,m_value_wxFont.GetWeight()) );

    AddChild( wxBoolProperty(_("Underlined"),wxEmptyString,
            m_value_wxFont.GetUnderlined()) );

}

wxFontPropertyClass::~wxFontPropertyClass () { }

void wxFontPropertyClass::DoSetValue ( wxPGVariant value )
{
    const wxFont* font = wxPGVariantToWxObjectPtr(value,wxFont);

    if ( font && font->Ok() )
        m_value_wxFont = *font;
    else
        m_value_wxFont = wxFont(10,wxSWISS,wxNORMAL,wxNORMAL);

    RefreshChildren();
}

wxPGVariant wxFontPropertyClass::DoGetValue () const
{
    return wxPGVariant((const wxObject*)&m_value_wxFont);
}

wxString wxFontPropertyClass::GetValueAsString ( int arg_flags ) const
{
    return wxPGPropertyWithChildren::GetValueAsString(arg_flags);
}

bool wxFontPropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
    wxEvent& event )
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        // Update value from last minute changes
        PrepareValueForDialogEditing(propgrid);

        wxFontData data;
        data.SetInitialFont(m_value_wxFont);
        data.SetColour(*wxBLACK);

        wxFontDialog dlg(propgrid, &data);
        if ( dlg.ShowModal() == wxID_OK )
        {
            propgrid->EditorsValueWasModified();

            wxFontData retData = dlg.GetFontData();

            DoSetValue(retData.GetChosenFont());
            UpdateControl(primary);

            return TRUE;
        }
    }
    return FALSE;
}

void wxFontPropertyClass::RefreshChildren()
{
    if ( !GetCount() ) return;
    Item(0)->DoSetValue ( (long)m_value_wxFont.GetPointSize() );
    Item(1)->DoSetValue ( (long)m_value_wxFont.GetFamily() );
    Item(2)->DoSetValue ( m_value_wxFont.GetFaceName() );
    Item(3)->DoSetValue ( (long)m_value_wxFont.GetStyle() );
    Item(4)->DoSetValue ( (long)m_value_wxFont.GetWeight() );
    Item(5)->DoSetValue ( m_value_wxFont.GetUnderlined() );
}

void wxFontPropertyClass::ChildChanged ( wxPGProperty* p )
{
    wxASSERT ( this == p->GetParent() );

    int ind = p->GetIndexInParent();

    if ( ind == 0 )
    {
        m_value_wxFont.SetPointSize ( p->DoGetValue().GetRawLong() );
    }
    else if ( ind == 1 )
    {
        int fam = p->DoGetValue().GetRawLong();
        if ( fam < wxDEFAULT ||
             fam > wxTELETYPE )
             fam = wxDEFAULT;
        m_value_wxFont.SetFamily ( fam );
    }
    else if ( ind == 2 )
    {
        m_value_wxFont.SetFaceName ( p->GetValueAsString(wxPG_FULL_VALUE) );
    }
    else if ( ind == 3 )
    {
        int st = p->DoGetValue().GetRawLong();
        if ( st != wxFONTSTYLE_NORMAL &&
             st != wxFONTSTYLE_SLANT &&
             st != wxFONTSTYLE_ITALIC )
             st = wxFONTWEIGHT_NORMAL;
        m_value_wxFont.SetStyle ( st );
    }
    else if ( ind == 4 )
    {
        int wt = p->DoGetValue().GetRawLong();
        if ( wt != wxFONTWEIGHT_NORMAL &&
             wt != wxFONTWEIGHT_LIGHT &&
             wt != wxFONTWEIGHT_BOLD )
             wt = wxFONTWEIGHT_NORMAL;
        m_value_wxFont.SetWeight ( wt );
    }
    else if ( ind == 5 )
    {
        m_value_wxFont.SetUnderlined ( p->DoGetValue().GetBool()?true:false );
    }
}

// -----------------------------------------------------------------------
// wxSystemColourProperty
// -----------------------------------------------------------------------

#include <wx/colordlg.h>

#define wx_cp_es_syscolours_len 25
static const wxChar* gs_cp_es_syscolour_labels[wx_cp_es_syscolours_len] = {
    wxT("AppWorkspace"),
    wxT("ActiveBorder"),
    wxT("ActiveCaption"),
    wxT("ButtonFace"),
    wxT("ButtonHighlight"),
    wxT("ButtonShadow"),
    wxT("ButtonText"),
    wxT("CaptionText"),
    wxT("ControlDark"),
    wxT("ControlLight"),
    wxT("Desktop"),
    wxT("GrayText"),
    wxT("Highlight"),
    wxT("HighlightText"),
    wxT("InactiveBorder"),
    wxT("InactiveCaption"),
    wxT("InactiveCaptionText"),
    wxT("Menu"),
    wxT("Scrollbar"),
    wxT("Tooltip"),
    wxT("TooltipText"),
    wxT("Window"),
    wxT("WindowFrame"),
    wxT("WindowText"),
    wxT("Custom")
};

static long gs_cp_es_syscolour_values[wx_cp_es_syscolours_len] = {
    wxSYS_COLOUR_APPWORKSPACE,
    wxSYS_COLOUR_ACTIVEBORDER,
    wxSYS_COLOUR_ACTIVECAPTION,
    wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_BTNTEXT ,
    wxSYS_COLOUR_CAPTIONTEXT,
    wxSYS_COLOUR_3DDKSHADOW,
    wxSYS_COLOUR_3DLIGHT,
    wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_GRAYTEXT,
    wxSYS_COLOUR_HIGHLIGHT,
    wxSYS_COLOUR_HIGHLIGHTTEXT,
    wxSYS_COLOUR_INACTIVEBORDER,
    wxSYS_COLOUR_INACTIVECAPTION,
    wxSYS_COLOUR_INACTIVECAPTIONTEXT,
    wxSYS_COLOUR_MENU,
    wxSYS_COLOUR_SCROLLBAR,
    wxSYS_COLOUR_INFOBK,
    wxSYS_COLOUR_INFOTEXT,
    wxSYS_COLOUR_WINDOW,
    wxSYS_COLOUR_WINDOWFRAME,
    wxSYS_COLOUR_WINDOWTEXT,
    wxPG_COLOUR_CUSTOM
};

// Class body is in advprops.h

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxSystemColourProperty,wxColourPropertyValue,const wxColourPropertyValue&,Choice)

void wxSystemColourPropertyClass::Init ( int type, const wxColour& colour )
{

    m_value.m_type = type;
    if ( colour.Ok() )
        m_value.m_colour = colour;
    else
        m_value.m_colour = *wxWHITE;

    m_flags |= wxPG_PROP_STATIC_CHOICES; // Colour selection cannot be changed.
}

wxSystemColourPropertyClass::wxSystemColourPropertyClass( const wxString& label, const wxString& name,
    const wxColourPropertyValue& value )
    : wxEnumPropertyClass( label,
                      name,
                      gs_cp_es_syscolour_labels,
                      gs_cp_es_syscolour_values,
                      wx_cp_es_syscolours_len )
{
    wxPG_INIT_REQUIRED_TYPE(wxColourPropertyValue)

    if ( &value )
        Init(value.m_type,value.m_colour);
    else
        Init(0,*wxBLACK);

    DoSetValue ( m_value );
}

wxSystemColourPropertyClass::wxSystemColourPropertyClass( const wxString& label, const wxString& name,
    const wxChar** labels, const long* values, unsigned int itemcount,
    const wxColourPropertyValue& value )
    : wxEnumPropertyClass( label,name,labels,values,itemcount )
{
    if ( &value )
        Init(value.m_type,value.m_colour);
    else
        Init(wxPG_COLOUR_CUSTOM,*wxBLACK);
}

wxSystemColourPropertyClass::wxSystemColourPropertyClass( const wxString& label, const wxString& name,
    const wxChar** labels, const long* values, unsigned int itemcount,
    const wxColour& value )
    : wxEnumPropertyClass( label,name,labels,values,itemcount )
{
    Init(wxPG_COLOUR_CUSTOM,value);
}

wxSystemColourPropertyClass::~wxSystemColourPropertyClass () { }

int wxSystemColourPropertyClass::ColToInd ( const wxColour& colour )
{
    size_t i;
    size_t i_max = m_constants->GetCount() - 1;

    long pixval = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
    const wxArrayInt& arrValues = m_constants->GetValues();

    for ( i=0; i<i_max; i++ )
    {
        int ind = i;
        if ( arrValues.GetCount() )
            ind = arrValues[i];

        if ( GetColour(ind) == pixval )
        {
            /*wxLogDebug(wxT("%s(%s): Index %i for ( getcolour(%i,%i,%i), colour(%i,%i,%i))"),
                GetClassName(),GetLabel().c_str(),
                (int)i,(int)GetColour(ind).Red(),(int)GetColour(ind).Green(),(int)GetColour(ind).Blue(),
                (int)colour.Red(),(int)colour.Green(),(int)colour.Blue());*/
            return ind;
        }
    }
    return wxNOT_FOUND;
}

static inline wxColour wxColourFromPGLong ( long col )
{
    return wxColour((col&0xFF),((col>>8)&0xFF),((col>>16)&0xFF));
}

void wxSystemColourPropertyClass::DoSetValue ( wxPGVariant value )
{
    wxColourPropertyValue* pval = wxPGVariantToWxObjectPtr(value,wxColourPropertyValue);

    if ( pval == (wxColourPropertyValue*) NULL )
    {
        m_value.m_type = wxPG_COLOUR_CUSTOM;
        m_value.m_colour = *wxWHITE;
    }
    else if ( pval != &m_value )
        m_value = *pval;

    // If available, use matching list item instead.
    if ( m_value.m_type == wxPG_COLOUR_CUSTOM )
    {
        if ( (m_flags & wxPG_PROP_TRANSLATE_CUSTOM) )
        {
            int found_ind = ColToInd(m_value.m_colour);
            if ( found_ind != wxNOT_FOUND )
                m_value.m_type = found_ind;
        }
    }

    wxASSERT ( m_value.m_colour.Ok() );

    if ( m_value.m_type < wxPG_COLOUR_WEB_BASE )
    {
        m_value.m_colour = GetColour ( m_value.m_type );
        wxEnumPropertyClass::DoSetValue ( (long)m_value.m_type );
    }
    else
    {
        m_index = m_constants->GetCount()-1;
    }
}

long wxSystemColourPropertyClass::GetColour ( int index )
{
    wxColour colour = wxSystemSettings::GetColour ( (wxSystemColour)index );
    return wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}

wxPGVariant wxSystemColourPropertyClass::DoGetValue () const
{
    return wxPGVariant(&m_value);
}

wxString wxSystemColourPropertyClass::GetValueAsString ( int ) const
{
    if ( m_value.m_type == wxPG_COLOUR_CUSTOM )
    {
        wxString temp;
        temp.Printf ( wxT("(%i,%i,%i)"),
            (int)m_value.m_colour.Red(),(int)m_value.m_colour.Green(),(int)m_value.m_colour.Blue());
        return temp;
    }
    return m_constants->GetLabel(m_index);
}

wxSize wxSystemColourPropertyClass::GetImageSize() const
{
    return wxSize(-1,-1);
}

// Need to do some extra event handling.
bool wxSystemColourPropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary, wxEvent& event )
{
#if wxPG_USE_CUSTOM_CONTROLS
    if ( event.GetEventType() == wxEVT_COMMAND_CHOICE_SELECTED )
#else
    if ( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
#endif
    {
        int index = m_index; // m_index has already been updated.
        int type = wxEnumPropertyClass::DoGetValue().GetRawLong();

        const wxArrayInt& arrValues = m_constants->GetValues();

        if ( ( arrValues.GetCount() && type == wxPG_COLOUR_CUSTOM ) ||
             ( !arrValues.GetCount() && index == (int)(m_constants->GetCount()-1) )
           )
        {
            m_value.m_type = wxPG_COLOUR_CUSTOM;

            wxColourData data;
            data.SetChooseFull(true);
            int i;
            for ( i = 0; i < 16; i++)
            {
                wxColour colour(i*16, i*16, i*16);
                data.SetCustomColour(i, colour);
            }

            wxColourDialog dialog(propgrid, &data);
            if ( dialog.ShowModal() == wxID_OK )
            {
                wxColourData retData = dialog.GetColourData();
                m_value.m_colour = retData.GetColour();
                wxSystemColourPropertyClass::DoSetValue(m_value);
            }

            // Update text in combo box (so it is "(R,G,B)" not "Custom").
            if ( primary )
                GetEditorClass()->SetControlStringValue(primary,GetValueAsString(0));

            return TRUE;
        }
        else
        {
            m_value.m_type = type;
            m_value.m_colour = GetColour( type );
        }
    }
    return FALSE;
}

void wxSystemColourPropertyClass::OnCustomPaint ( wxDC& dc,
    const wxRect& rect, wxPGPaintData& paintdata )
{
    if ( paintdata.m_choiceItem >= 0 && paintdata.m_choiceItem < (int)(GetItemCount()-1) )
    {
        int colind = paintdata.m_choiceItem;
        const wxArrayInt& values = m_constants->GetValues();
        if ( values.GetCount() )
             colind = values[paintdata.m_choiceItem];
        dc.SetBrush ( wxColour ( GetColour ( colind ) ) );
    }
    else if ( !(m_flags & wxPG_PROP_UNSPECIFIED) )
        dc.SetBrush ( m_value.m_colour );
    else
        dc.SetBrush ( *wxWHITE );

    dc.DrawRectangle ( rect );

}

bool wxSystemColourPropertyClass::SetValueFromString ( const wxString& text, int flags )
{
    wxColourPropertyValue val;
    if ( text[0] == wxT('(') )
    {
        // Custom colour.
        val.m_type = wxPG_COLOUR_CUSTOM;

        int r, g, b;
        wxSscanf(text.c_str(),wxT("(%i,%i,%i)"),&r,&g,&b);
        val.m_colour.Set(r,g,b);

        wxSystemColourPropertyClass::DoSetValue ( &val );

        return TRUE;
    }
    else
    {
        // Predefined colour.
        bool res = wxEnumPropertyClass::SetValueFromString(text,flags);
        if ( res )
        {
            val.m_type = m_index;
            const wxArrayInt& values = GetValues();
            if ( values.GetCount() )
                val.m_type = values[m_index];

            // Get proper colour for type.
            val.m_colour = wxColourFromPGLong(GetColour(val.m_type));

            wxSystemColourPropertyClass::DoSetValue ( &val );
            return TRUE;
        }
    }
    return FALSE;
}

// -----------------------------------------------------------------------
// wxColourProperty
// -----------------------------------------------------------------------

#define wx_cp_es_normcolours_len 19
static const wxChar* gs_cp_es_normcolour_labels[wx_cp_es_normcolours_len] = {
    wxT("Black"),
    wxT("Maroon"),
    wxT("Navy"),
    wxT("Purple"),
    wxT("Teal"),
    wxT("Gray"),
    wxT("Green"),
    wxT("Olive"),
    wxT("Brown"),
    wxT("Blue"),
    wxT("Fuchsia"),
    wxT("Red"),
    wxT("Orange"),
    wxT("Silver"),
    wxT("Lime"),
    wxT("Aqua"),
    wxT("Yellow"),
    wxT("White"),
    wxT("Custom")
};

static unsigned long gs_cp_es_normcolour_colours[wx_cp_es_normcolours_len] = {
    wxPG_COLOUR(0,0,0),
    wxPG_COLOUR(128,0,0),
    wxPG_COLOUR(0,0,128),
    wxPG_COLOUR(128,0,128),
    wxPG_COLOUR(0,128,128),
    wxPG_COLOUR(128,128,128),
    wxPG_COLOUR(0,128,0),
    wxPG_COLOUR(128,128,0),
    wxPG_COLOUR(166,124,81),
    wxPG_COLOUR(0,0,255),
    wxPG_COLOUR(255,0,255),
    wxPG_COLOUR(255,0,0),
    wxPG_COLOUR(247,148,28),
    wxPG_COLOUR(192,192,192),
    wxPG_COLOUR(0,255,0),
    wxPG_COLOUR(0,255,255),
    wxPG_COLOUR(255,255,0),
    wxPG_COLOUR(255,255,255),
    wxPG_COLOUR(0,0,0)
};

WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxColourProperty,
                                               gs_cp_es_normcolour_labels,
                                               (const long*)NULL,
                                               gs_cp_es_normcolour_colours,
                                               wx_cp_es_normcolours_len)

// -----------------------------------------------------------------------
// wxCursorProperty
// -----------------------------------------------------------------------

#define wxPG_CURSOR_IMAGE_WIDTH     32

#define wx_cp_es_syscursors_len 28
static const wxChar* gs_cp_es_syscursors_labels[wx_cp_es_syscursors_len] = {
    wxT("Default"),
    wxT("Arrow"),
    wxT("Right Arrow"),
    wxT("Blank"),
    wxT("Bullseye"),
    wxT("Character"),
    wxT("Cross"),
    wxT("Hand"),
    wxT("I-Beam"),
    wxT("Left Button"),
    wxT("Magnifier"),
    wxT("Middle Button"),
    wxT("No Entry"),
    wxT("Paint Brush"),
    wxT("Pencil"),
    wxT("Point Left"),
    wxT("Point Right"),
    wxT("Question Arrow"),
    wxT("Right Button"),
    wxT("Sizing NE-SW"),
    wxT("Sizing N-S"),
    wxT("Sizing NW-SE"),
    wxT("Sizing W-E"),
    wxT("Sizing"),
    wxT("Spraycan"),
    wxT("Wait"),
    wxT("Watch"),
    wxT("Wait Arrow"),
    //_("Cursor from File"),
    //_("Cursor from Resource")
};

static long gs_cp_es_syscursors_values[wx_cp_es_syscursors_len] = {
    wxCURSOR_NONE,
    wxCURSOR_ARROW,
    wxCURSOR_RIGHT_ARROW,
    wxCURSOR_BLANK,
    wxCURSOR_BULLSEYE,
    wxCURSOR_CHAR,
    wxCURSOR_CROSS,
    wxCURSOR_HAND,
    wxCURSOR_IBEAM,
    wxCURSOR_LEFT_BUTTON,
    wxCURSOR_MAGNIFIER,
    wxCURSOR_MIDDLE_BUTTON,
    wxCURSOR_NO_ENTRY,
    wxCURSOR_PAINT_BRUSH,
    wxCURSOR_PENCIL,
    wxCURSOR_POINT_LEFT,
    wxCURSOR_POINT_RIGHT,
    wxCURSOR_QUESTION_ARROW,
    wxCURSOR_RIGHT_BUTTON,
    wxCURSOR_SIZENESW,
    wxCURSOR_SIZENS,
    wxCURSOR_SIZENWSE,
    wxCURSOR_SIZEWE,
    wxCURSOR_SIZING,
    wxCURSOR_SPRAYCAN,
    wxCURSOR_WAIT,
    wxCURSOR_WATCH,
    wxCURSOR_ARROWWAIT,
    //wxPG_CURSOR_FROM_FILE,
    //wxPG_CURSOR_FROM_RESOURCE
};

WX_PG_IMPLEMENT_DERIVED_PROPERTY_CLASS(wxCursorProperty,int)

wxCursorPropertyClass::wxCursorPropertyClass ( const wxString& label, const wxString& name,
    int value )
    : wxEnumPropertyClass( label,
                           name,
                           gs_cp_es_syscursors_labels,
                           gs_cp_es_syscursors_values,
                           wx_cp_es_syscursors_len,
                           value )
{
    m_flags |= wxPG_PROP_STATIC_CHOICES; // Cursor selection cannot be changed.
    //wxEnumPropertyClass::DoSetValue ( (void*)&value, NULL );
}

wxCursorPropertyClass::~wxCursorPropertyClass ()
{
}

wxSize wxCursorPropertyClass::GetImageSize() const
{
#if wxPG_CAN_DRAW_CURSOR
    return wxSize(wxPG_CURSOR_IMAGE_WIDTH,wxPG_CURSOR_IMAGE_WIDTH);
#else
    return wxSize(0,0);
#endif
}

#if wxPG_CAN_DRAW_CURSOR
void wxCursorPropertyClass::OnCustomPaint ( wxDC& dc,
    const wxRect& rect, wxPGPaintData& paintdata )
{

    // Background brush
    dc.SetBrush ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE ) );

    if ( paintdata.m_choiceItem >= 0 )
    {
        dc.DrawRectangle ( rect );

        int cursorindex = gs_cp_es_syscursors_values[paintdata.m_choiceItem];

        /*
        if ( cursorindex == wxPG_CURSOR_FROM_FILE )
        {
            wxFAIL_MSG(wxT("not implemented"));
        }
        else if ( cursorindex == wxPG_CURSOR_FROM_RESOURCE )
        {
            wxFAIL_MSG(wxT("not implemented"));
        }
        else
        */
        {
            if ( cursorindex == wxCURSOR_NONE )
                cursorindex = wxCURSOR_ARROW;

            wxCursor cursor( cursorindex );

        #ifdef __WXMSW__
            ::DrawIconEx( (HDC)dc.GetHDC(),
                          rect.x,
                          rect.y,
                          (HICON)cursor.GetHandle(),
                          0,
                          0,
                          0,
                          NULL,
                          DI_COMPAT | DI_DEFAULTSIZE | DI_NORMAL
                        );
        #endif
        }
    }
}
#else
void wxCursorPropertyClass::OnCustomPaint ( wxDC&, const wxRect&, wxPGPaintData& ) { }
#endif

// -----------------------------------------------------------------------
// wxImageFileProperty
// -----------------------------------------------------------------------

#if wxUSE_IMAGE

const wxString& wxPGGetDefaultImageWildcard ()
{
    // Form the wildcard, if not done yet
    if ( !wxPGGlobalVars->m_pDefaultImageWildcard.length() )
    {

        wxString str;

        // TODO: This section may require locking (using global).

        wxList& handlers = wxImage::GetHandlers();

        wxList::iterator node;

        // Let's iterate over the image handler list.
        //for ( wxList::Node *node = handlers.GetFirst(); node; node = node->GetNext() )
        for ( node = handlers.begin(); node != handlers.end(); node++ )
        {
            wxImageHandler *handler = (wxImageHandler*)*node;

            wxString ext_lo = handler->GetExtension();
            wxString ext_up = ext_lo.Upper();

            str.append ( ext_up );
            str.append ( wxT(" files (*.") );
            str.append ( ext_up );
            str.append ( wxT(")|*.") );
            str.append ( ext_lo );
            str.append ( wxT("|") );
        }

        str.append ( wxT("All files (*.*)|*.*") );

        wxPGGlobalVars->m_pDefaultImageWildcard = str;
    }

    return wxPGGlobalVars->m_pDefaultImageWildcard;
}

WX_PG_IMPLEMENT_DERIVED_PROPERTY_CLASS(wxImageFileProperty,const wxString&)

wxImageFilePropertyClass::wxImageFilePropertyClass ( const wxString& label, const wxString& name,
    const wxString& value )
    : wxFilePropertyClass(label,name,value)
{

    m_wildcard = wxPGGetDefaultImageWildcard();

    m_pImage = (wxImage*) NULL;
    m_pBitmap = (wxBitmap*) NULL;
}

wxImageFilePropertyClass::~wxImageFilePropertyClass ()
{
    if ( m_pBitmap )
        delete m_pBitmap;
    if ( m_pImage )
        delete m_pImage;
}

void wxImageFilePropertyClass::DoSetValue ( wxPGVariant value )
{
    wxFilePropertyClass::DoSetValue(value);

    // Delete old image
    if ( m_pImage )
    {
        delete m_pImage;
        m_pImage = NULL;
    }
    if ( m_pBitmap )
    {
        delete m_pBitmap;
        m_pBitmap = NULL;
    }

    // Create the image thumbnail
    if ( m_filename.FileExists() )
    {
        m_pImage = new wxImage ( m_filename.GetFullPath() );
    }

}

wxSize wxImageFilePropertyClass::GetImageSize() const
{
    return wxSize(-1,-1);
}

void wxImageFilePropertyClass::OnCustomPaint ( wxDC& dc,
    const wxRect& rect, wxPGPaintData& )
{
    if ( m_pBitmap || (m_pImage && m_pImage->Ok() ) )
    {
        // Draw the thumbnail

        // Create the bitmap here because required size is not known in DoSetValue().
        if ( !m_pBitmap )
        {
            m_pImage->Rescale ( rect.width, rect.height );
            m_pBitmap = new wxBitmap ( *m_pImage );
            delete m_pImage;
            m_pImage = NULL;
        }

        dc.DrawBitmap ( *m_pBitmap, rect.x, rect.y, FALSE );
    }
    else
    {
        // No file - just draw a white box
        dc.SetBrush ( *wxWHITE_BRUSH );
        dc.DrawRectangle ( rect );
    }
}

#endif // wxUSE_IMAGE

// -----------------------------------------------------------------------
// wxMultiChoiceProperty
// -----------------------------------------------------------------------

#if wxUSE_CHOICEDLG

#include <wx/choicdlg.h>

wxPGProperty* wxPG_CONSTFUNC(wxMultiChoiceProperty)( const wxString& label, const wxString& name,
        wxArrayString& strings, const wxArrayInt& value )
{
    return new wxPG_PROPCLASS(wxMultiChoiceProperty)(label,name,strings,value);
}

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxMultiChoiceProperty,wxArrayInt,
                               const wxArrayInt&,TextCtrlAndButton)

wxMultiChoicePropertyClass::wxMultiChoicePropertyClass ( const wxString& label,
                                                         const wxString& name,
                                                         wxArrayString& strings,
                                                         const wxArrayInt& value )
                                                         : wxPGProperty(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxArrayInt)

    m_constants = wxPropertyGrid::AddConstantsArray(strings,
                                                    // This is good (value is
                                                    // not supposed to go here)
                                                    *((const wxArrayInt*)NULL),
                                                    true);

    DoSetValue( (void*)&value );
}

wxMultiChoicePropertyClass::wxMultiChoicePropertyClass ( const wxString& label,
                                                         const wxString& name,
                                                         const wxArrayInt& WXUNUSED(value) )
                                                         : wxPGProperty(label,name)
{
    m_constants = &wxPGGlobalVars->m_emptyConstants;
}

wxMultiChoicePropertyClass::~wxMultiChoicePropertyClass ()
{
    wxPGUnRefChoices(m_constants);
}

void wxMultiChoicePropertyClass::DoSetValue ( wxPGVariant value )
{
    wxArrayInt* pObj = (wxArrayInt*)wxPGVariantToVoidPtr(value);
    if ( pObj )
    {
        m_value_wxArrayInt = *pObj;
        GenerateValueAsString();
        wxPG_SetVariantValueVoidPtr();
    }
}

wxPGVariant wxMultiChoicePropertyClass::DoGetValue () const
{
    return wxPGVariant((void*)&m_value_wxArrayInt);
}

wxString wxMultiChoicePropertyClass::GetValueAsString ( int ) const
{
    return m_display;
}

void wxMultiChoicePropertyClass::GenerateValueAsString ()
{

    // Allow zero-length strings list
    if ( !m_constants->GetCount() )
    {
        m_display = wxEmptyString;
        return;
    }

    wxString& tempstr = m_display;
    unsigned int i;
    unsigned int itemcount = m_value_wxArrayInt.GetCount();

    tempstr.Empty();

    if ( itemcount )
        tempstr.append ( wxT("\"") );

    for ( i = 0; i < itemcount; i++ )
    {
        int ind = m_value_wxArrayInt.Item(i);
        wxASSERT ( ind >= 0 && ind < (int)m_constants->GetCount() );
        tempstr.append ( m_constants->GetLabel(ind) );
        tempstr.append ( wxT("\"") );
        if ( i < (itemcount-1) )
            tempstr.append ( wxT(" \"") );
    }
}

bool wxMultiChoicePropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
    wxEvent& event )
{
    //if ( wxPGTextCtrlPropertyClass::OnEvent ( propgrid, primary, event ) )
    //    return TRUE;

    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        // Update the value
        PrepareValueForDialogEditing(propgrid);

        // launch editor dialog
        wxMultiChoiceDialog dlg(propgrid,
                                _("Make a selection:"),
                                m_label,
                                m_constants->GetCount(),
                                &m_constants->GetLabels()[0],
                                wxCHOICEDLG_STYLE);

        dlg.Move ( propgrid->GetGoodEditorDialogPosition (this,dlg.GetSize()) );
        dlg.SetSelections(m_value_wxArrayInt);

        if ( dlg.ShowModal() == wxID_OK /*&& dlg.IsModified()*/ )
        {
            wxArrayInt arrint = dlg.GetSelections();
            DoSetValue ( (void*)&arrint );
            UpdateControl ( primary );

            return TRUE;
        }
    }
    return FALSE;
}

int wxMultiChoicePropertyClass::GetChoiceInfo ( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
    {
        choiceinfo->m_itemCount = m_constants->GetCount();
        choiceinfo->m_constants = &m_constants;
    }
    return -1;
}

bool wxMultiChoicePropertyClass::SetValueFromString ( const wxString& text, int )
{
    m_value_wxArrayInt.Empty();
    wxArrayString& strings = m_constants->GetLabels();

    WX_PG_TOKENIZER2_BEGIN(text,wxT('"'))
        int ind = strings.Index( token );
        if ( ind != wxNOT_FOUND )
            m_value_wxArrayInt.Add ( ind );
    WX_PG_TOKENIZER2_END()

    GenerateValueAsString();

    return TRUE;
}

#endif // wxUSE_CHOICEDLG

// -----------------------------------------------------------------------
// wxPropertyContainerMethods
// -----------------------------------------------------------------------

void wxPropertyContainerMethods::InitAllTypeHandlers ()
{
    wxPG_INIT_REQUIRED_TYPE(wxColour)
    wxPG_INIT_REQUIRED_TYPE(wxFont)
    wxPG_INIT_REQUIRED_TYPE(wxArrayInt)
    wxPG_INIT_REQUIRED_TYPE(wxColourPropertyValue)
}

// -----------------------------------------------------------------------

void wxPropertyContainerMethods::RegisterAdvancedPropertyClasses()
{
    wxPGRegisterPropertyClass(wxMultiChoiceProperty);
    wxPGRegisterPropertyClass(wxImageFileProperty);
    wxPGRegisterPropertyClass(wxColourProperty);
    wxPGRegisterPropertyClass(wxFontProperty);
    wxPGRegisterPropertyClass(wxSystemColourProperty);
    wxPGRegisterPropertyClass(wxCursorProperty);
}

// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// AdvImageFile Property
// -----------------------------------------------------------------------

#if wxUSE_IMAGE
class wxMyImageInfo;

WX_DECLARE_OBJARRAY(wxMyImageInfo, wxArrayMyImageInfo);

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayMyImageInfo);

wxArrayString       g_myImageNames;
wxArrayMyImageInfo  g_myImageArray;

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxAdvImageFileProperty,wxString,
                               const wxString&,ChoiceAndButton)


wxAdvImageFilePropertyClass::wxAdvImageFilePropertyClass ( const wxString& label,
    const wxString& name, const wxString& value)
    : wxFilePropertyClass(label,name,value)
{
    m_wildcard = wxPGGetDefaultImageWildcard();

    m_index = -1;

    m_pImage = (wxImage*) NULL;

    m_flags &= ~(wxPG_PROP_SHOW_FULL_FILENAME);

}

wxAdvImageFilePropertyClass::~wxAdvImageFilePropertyClass ()
{
    // Delete old image
    if ( m_pImage )
    {
        delete m_pImage;
        m_pImage = (wxImage*) NULL;
    }
}

void wxAdvImageFilePropertyClass::DoSetValue ( wxPGVariant value )
{
    wxFilePropertyClass::DoSetValue(value);

    // Delete old image
    if ( m_pImage )
    {
        delete m_pImage;
        m_pImage = (wxImage*) NULL;
    }

   // Customized by cyberkoa to get the value as full path
     wxString imagename = GetValueAsString(1);

    if ( imagename.length() )
    {

        int index = g_myImageNames.Index(imagename);
        //int index = g_myImageNames.Index(m_filename.GetFullName());

        // If not in table, add now.
        if ( index == wxNOT_FOUND )
        {
            g_myImageNames.Add( imagename );
            g_myImageArray.Add( new wxMyImageInfo ( m_filename.GetFullPath() ) );

            index = g_myImageArray.GetCount() - 1;
        }

        // If no thumbnail ready, then need to load image.
        if ( !g_myImageArray[index].m_pThumbnail2 )
        {
            // Load if file exists.
            if ( m_filename.FileExists() )
                m_pImage = new wxImage ( m_filename.GetFullPath() );
        }

        m_index = index;

    }
    else
        m_index = -1;

}

int wxAdvImageFilePropertyClass::GetChoiceInfo ( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
    {
        choiceinfo->m_itemCount = g_myImageNames.GetCount();
        if ( g_myImageNames.GetCount() )
            choiceinfo->m_arrWxString = &g_myImageNames.Item(0);
    }
    return m_index;
}

bool wxAdvImageFilePropertyClass::SetValueFromInt ( long value, int /*flags*/ )
{
    wxASSERT ( value >= 0 );
    m_index = value;
    return SetValueFromString( g_myImageNames.Item(value), wxPG_FULL_VALUE );
}

bool wxAdvImageFilePropertyClass::OnEvent ( wxPropertyGrid* propgrid, wxPGCtrlClass* primary,
   wxEvent& event )
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        size_t prev_itemcount = g_myImageArray.GetCount();

        bool res = wxFilePropertyClass::OnEvent(propgrid,primary,event);

        if ( res )
        {
            wxString name = GetValueAsString(0);

            if ( g_myImageArray.GetCount() != prev_itemcount )
            {
                wxASSERT ( g_myImageArray.GetCount() == (prev_itemcount+1) );

                // Add to the control's array.
                // (should be added to own array earlier)

                if ( primary )
                    GetEditorClass()->AppendItem(primary,name);

            }

            if ( primary )
               GetEditorClass()->UpdateControl(this,primary);

            return TRUE;
        }

    }
    return FALSE;
}

wxSize wxAdvImageFilePropertyClass::GetImageSize() const
{
    return wxPG_FLEXIBLE_SIZE(PREF_THUMBNAIL_HEIGHT,PREF_THUMBNAIL_HEIGHT);
}

void wxAdvImageFilePropertyClass::LoadThumbnails ( size_t index )
{
    wxMyImageInfo& mii = g_myImageArray[index];

    if ( !mii.m_pThumbnail2 )
    {

        if ( !m_pImage || !m_pImage->Ok() ||
             m_filename != mii.m_path
           )
        {
            if ( m_pImage )
                delete m_pImage;
            m_pImage = new wxImage( mii.m_path );
        }

        if ( m_pImage && m_pImage->Ok() )
        {
            int im_wid = m_pImage->GetWidth();
            int im_hei = m_pImage->GetHeight();
            if ( im_hei > PREF_THUMBNAIL_HEIGHT )
            {
                // TNW = (TNH*IW)/IH
                im_wid = (PREF_THUMBNAIL_HEIGHT*m_pImage->GetWidth())/m_pImage->GetHeight();
                im_hei = PREF_THUMBNAIL_HEIGHT;
            }

            m_pImage->Rescale ( im_wid, im_hei );

            mii.m_pThumbnail2 = new wxBitmap ( *m_pImage );

            wxSize cis = GetParentState()->GetGrid()->GetImageSize();
            m_pImage->Rescale ( cis.x, cis.y );

            mii.m_pThumbnail1 = new wxBitmap ( *m_pImage );

        }

        if ( m_pImage )
        {
            delete m_pImage;
            m_pImage = (wxImage*) NULL;
        }
    }
}

void wxAdvImageFilePropertyClass::OnCustomPaint ( wxDC& dc,
    const wxRect& rect, wxPGPaintData& pd )
{
    int index = m_index;
    if ( pd.m_choiceItem >= 0 )
        index = pd.m_choiceItem;

    //wxLogDebug(wxT("%i"),index);

    if ( index >= 0 )
    {
        LoadThumbnails(index);

        // Is this a measure item call?
        if ( rect.x < 0 )
        {
            // Variable height
            //pd.m_drawnHeight = PREF_THUMBNAIL_HEIGHT;
            wxBitmap* pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail2;
            if ( pBitmap )
                pd.m_drawnHeight = pBitmap->GetHeight();
            else
                pd.m_drawnHeight = 16;
            return;
        }

        // Draw the thumbnail

        wxBitmap* pBitmap;

        if ( pd.m_choiceItem >= 0 )
            pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail2;
        else
            pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail1;

        if ( pBitmap )
        {
            dc.DrawBitmap ( *pBitmap, rect.x, rect.y, FALSE );

            // Tell the caller how wide we drew.
            pd.m_drawnWidth = pBitmap->GetWidth();

            return;
        }
    }

    // No valid file - just draw a white box.
    dc.SetBrush ( *wxWHITE_BRUSH );
    dc.DrawRectangle ( rect );
}
#endif // wxUSE_IMAGE


#endif // wxPG_INCLUDE_ADVPROPS
