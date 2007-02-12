/////////////////////////////////////////////////////////////////////////////
// Name:        advprops.h
// Purpose:     wxPropertyGrid Advanced Properties (font, colour, etc.)
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_ADVPROPS_H_
#define _WX_PROPGRID_ADVPROPS_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "advprops.cpp"
#endif

// -----------------------------------------------------------------------

bool WXDLLIMPEXP_PG operator == (const wxFont&, const wxFont&);

WX_PG_DECLARE_VALUE_TYPE_WITH_DECL(wxFont,WXDLLIMPEXP_PG)

WX_PG_DECLARE_VALUE_TYPE_WITH_DECL(wxColour,WXDLLIMPEXP_PG)

bool WXDLLIMPEXP_PG operator == (const wxArrayInt& array1, const wxArrayInt& array2);

WX_PG_DECLARE_VALUE_TYPE_VOIDP_WITH_DECL(wxArrayInt,WXDLLIMPEXP_PG)

// -----------------------------------------------------------------------

// Web colour is currently unsupported
#define wxPG_COLOUR_WEB_BASE        0x10000
//#define wxPG_TO_WEB_COLOUR(A)   ((wxUint32)(A+wxPG_COLOUR_WEB_BASE))

#define wxPG_COLOUR_CUSTOM      0xFFFFFF

/** \class wxColourPropertyValue
    \ingroup classes
    \brief Because text, background and other colours tend to differ between
    platforms, wxSystemColourProperty must be able to select between system
    colour and, when necessary, to pick a custom one. wxSystemColourProperty
    value makes this possible.
*/
class WXDLLIMPEXP_PG wxColourPropertyValue : public wxObject
{
public:
    /** An integer value relating to the colour, and which exact
        meaning depends on the property with which it is used.
        For wxSystemColourProperty:

        Any of wxSYS_COLOUR_XXX, or any web-colour ( use wxPG_TO_WEB_COLOUR
        macro - (currently unsupported) ), or wxPG_COLOUR_CUSTOM.

        For custom colour properties without values array specified:

        index or wxPG_COLOUR_CUSTOM

        For custom colour properties <b>with</b> values array specified:

        m_arrValues[index] or wxPG_COLOUR_CUSTOM

    */
    wxUint32    m_type;

    /** Resulting colour. Should be correct regardless of type. */
    wxColour    m_colour;

    wxColourPropertyValue () { }

    inline wxColourPropertyValue ( const wxColour& colour )
    {
        m_type = wxPG_COLOUR_CUSTOM;
        m_colour = colour;
    }

    inline wxColourPropertyValue ( wxUint32 type )
    {
        m_type = type;
    }

    inline wxColourPropertyValue ( wxUint32 type, const wxColour& colour )
    {
        m_type = type;
        m_colour = colour;
    }

private:
    DECLARE_DYNAMIC_CLASS(wxColourPropertyValue)
};

bool WXDLLIMPEXP_PG operator == (const wxColourPropertyValue&, const wxColourPropertyValue&);

WX_PG_DECLARE_VALUE_TYPE_WITH_DECL(wxColourPropertyValue,WXDLLIMPEXP_PG)

// -----------------------------------------------------------------------
// Declare part of custom colour property macro pairs.

#define WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_WITH_DECL(NAME,DECL) \
extern DECL wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxColourPropertyValue& = *((wxColourPropertyValue*)NULL) );

#define WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY(NAME) \
extern wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxColourPropertyValue& = *((wxColourPropertyValue*)NULL) );

#define WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR_WITH_DECL(NAME,DECL) \
extern DECL wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxColour& = *wxBLACK );

#define WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(NAME) \
extern wxPGProperty* wxPG_CONSTFUNC(NAME)( const wxString&, const wxString& = wxPG_LABEL, const wxColour& = *wxBLACK );

// Declare advanced property types.
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxFontProperty,const wxFont&,*wxNORMAL_FONT,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxSystemColourProperty,const wxColourPropertyValue&,*((wxColourPropertyValue*)NULL),WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxCursorProperty,int,wxCURSOR_NONE,WXDLLIMPEXP_PG)
#if wxUSE_IMAGE
#include <wx/image.h>
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxImageFileProperty,const wxString&,wxEmptyString,WXDLLIMPEXP_PG)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxAdvImageFileProperty,const wxString&,wxEmptyString,WXDLLIMPEXP_PG)
#endif

WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR_WITH_DECL(wxColourProperty,WXDLLIMPEXP_PG)

// MultiChoice is trickier.
extern WXDLLIMPEXP_PG wxPGProperty* wxPG_CONSTFUNC(wxMultiChoiceProperty)( const wxString&, const wxString& /*= wxPG_LABEL*/,
        wxArrayString& strings /*= wxArrayString()*/,
        const wxArrayInt& value = *((wxArrayInt*)NULL) );

extern WXDLLIMPEXP_PG wxPGProperty* wxPG_CONSTFUNC(wxMultiChoiceProperty)( const wxString&, const wxString& = wxPG_LABEL,
        const wxArrayInt& = wxArrayInt() );

// -----------------------------------------------------------------------

//
// Define property classes *only* if propdev.h was included
//
#ifdef _WX_PROPGRID_PROPDEV_H_

// -----------------------------------------------------------------------

class WXDLLIMPEXP_PG wxFontPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxFontPropertyClass ( const wxString& label, const wxString& name, const wxFont& );
    virtual ~wxFontPropertyClass ();

    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
    virtual wxString GetValueAsString ( int arg_flags = 0 ) const;

    WX_PG_DECLARE_EVENT_METHODS()
    WX_PG_DECLARE_PARENTAL_METHODS()

protected:
    wxFont m_value_wxFont;
};

// -----------------------------------------------------------------------

/** If set, then match from list is searched for a custom colour. */
#define wxPG_PROP_TRANSLATE_CUSTOM      wxPG_PROP_CLASS_SPECIFIC_1

class WXDLLIMPEXP_PG wxSystemColourPropertyClass : public wxEnumPropertyClass
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxSystemColourPropertyClass ( const wxString& label, const wxString& name,
        const wxColourPropertyValue& value );
    ~wxSystemColourPropertyClass ();

    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()
    WX_PG_DECLARE_CUSTOM_PAINT_METHODS()

    // Default is to use wxSystemSettings::GetColour(index). Override to use
    // custom colour tables etc.
    virtual long GetColour ( int index );

protected:

    // Special constructors to be used by derived classes.
    wxSystemColourPropertyClass( const wxString& label, const wxString& name,
        const wxChar** labels, const long* values, unsigned int itemcount,
        const wxColourPropertyValue& value );
    wxSystemColourPropertyClass( const wxString& label, const wxString& name,
        const wxChar** labels, const long* values, unsigned int itemcount,
        const wxColour& value );

    void Init ( int type, const wxColour& colour );

    // Translates colour to a int value, return wxNOT_FOUND if no match.
    int ColToInd ( const wxColour& colour );

    wxColourPropertyValue   m_value;
};

// -----------------------------------------------------------------------

class wxCursorPropertyClass : public wxEnumPropertyClass
{
    WX_PG_DECLARE_DERIVED_PROPERTY_CLASS()
public:

    wxCursorPropertyClass ( const wxString& label, const wxString& name, int value );
    virtual ~wxCursorPropertyClass ();

    WX_PG_DECLARE_CUSTOM_PAINT_METHODS()
};

// -----------------------------------------------------------------------

#if wxUSE_IMAGE

WXDLLIMPEXP_PG const wxString& wxPGGetDefaultImageWildcard ();

class wxImageFilePropertyClass : public wxFilePropertyClass
{
    WX_PG_DECLARE_DERIVED_PROPERTY_CLASS()
public:

    wxImageFilePropertyClass( const wxString& label, const wxString& name, const wxString& value );
    virtual ~wxImageFilePropertyClass ();

    virtual void DoSetValue ( wxPGVariant value );
    WX_PG_DECLARE_CUSTOM_PAINT_METHODS()

protected:
    wxBitmap*   m_pBitmap; // final thumbnail area
    wxImage*    m_pImage; // intermediate thumbnail area

};

#endif

#if wxUSE_CHOICEDLG

class wxMultiChoicePropertyClass : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxMultiChoicePropertyClass(const wxString& label,
                               const wxString& name,
                               wxArrayString& strings,
                               const wxArrayInt& value);
    wxMultiChoicePropertyClass( const wxString& label, const wxString& name = wxPG_LABEL,
        const wxArrayInt& value = wxArrayInt() );
    virtual ~wxMultiChoicePropertyClass ();

    virtual void DoSetValue ( wxPGVariant value );
    virtual wxPGVariant DoGetValue () const;
    virtual wxString GetValueAsString ( int flags = 0 ) const;
    virtual bool SetValueFromString ( const wxString& text, int flags );
    WX_PG_DECLARE_EVENT_METHODS()

    virtual int GetChoiceInfo ( wxPGChoiceInfo* choiceinfo );

protected:

    void GenerateValueAsString ();

    wxPGConstants*  m_constants; // Holds strings (any values given are ignored).

    wxArrayInt      m_value_wxArrayInt; // Actual value.

    wxString        m_display; // Cache displayed text since generating it is relatively complicated.
};

#endif // wxUSE_CHOICEDLG

// -----------------------------------------------------------------------

#if wxUSE_IMAGE


class wxMyImageInfo
{
public:
    wxString    m_path;
    wxBitmap*   m_pThumbnail1; // smaller thumbnail
    wxBitmap*   m_pThumbnail2; // larger thumbnail

    wxMyImageInfo ( const wxString& str )
    {
        m_path = str;
        m_pThumbnail1 = (wxBitmap*) NULL;
        m_pThumbnail2 = (wxBitmap*) NULL;
    }
    ~wxMyImageInfo()
    {
        if ( m_pThumbnail1 )
            delete m_pThumbnail1;
        if ( m_pThumbnail2 )
            delete m_pThumbnail2;
    }

};





// Preferred thumbnail height.
#define PREF_THUMBNAIL_HEIGHT       64

class wxAdvImageFilePropertyClass : public wxFilePropertyClass
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxAdvImageFilePropertyClass( const wxString& label, const wxString& name,
        const wxString& value );
    virtual ~wxAdvImageFilePropertyClass ();

    void DoSetValue ( wxPGVariant value ); // Override to allow image loading.

    WX_PG_DECLARE_CHOICE_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()
    WX_PG_DECLARE_CUSTOM_PAINT_METHODS()

    void LoadThumbnails ( size_t n );

protected:
    wxImage*    m_pImage; // Temporary thumbnail data.

    int m_index; // Index required for choice behaviour.
};

#endif // WXUSE_IMAGE
#endif // _WX_PROPGRID_PROPDEV_H_

#endif // _WX_PROPGRID_ADVPROPS_H_
