/////////////////////////////////////////////////////////////////////////////
// Name:        sampleprops.cpp
// Purpose:     wxPropertyGrid Sample Properties
// Author:      Jaakko Salli
// Modified by:
// Created:     Mar-05-2006
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// -----------------------------------------------------------------------


#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>

// You will *NOT* need to include this in your application.
#include <wx/propgrid/extras.h>

// This silly condition is required incase these extra properties
// were already part of the compiled library.
#if !defined(WX_PROPGRID_SAMPLEPROPS_H) || defined(__WXPYTHON__)

#ifndef WX_PROPGRID_SAMPLEPROPS_H
    #include "sampleprops.h"
#endif


// -----------------------------------------------------------------------
// wxFontDataProperty
// -----------------------------------------------------------------------

// Dummy comparison required by value type implementation.
bool operator == (const wxFontData&, const wxFontData&)
{
    return FALSE;
}

WX_PG_IMPLEMENT_VALUE_TYPE_WXOBJ_OWNDEFAULT(wxFontData,wxFontDataProperty,wxFontData())

// Custom version of wxFontProperty that also holds colour in the value.
// Original version by Vladimir Vainer.

#include <wx/fontdlg.h>

class wxFontDataPropertyClass : public wxFontPropertyClass
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxFontDataPropertyClass( const wxString& label, const wxString& name, const wxFontData& value );
    virtual ~wxFontDataPropertyClass ();

    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
    WX_PG_DECLARE_PARENTAL_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()

protected:
    wxFontData  m_value_wxFontData;
};

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxFontDataProperty,wxFontProperty,
                               wxFontData,const wxFontData&,TextCtrlAndButton)

wxFontDataPropertyClass::wxFontDataPropertyClass( const wxString& label, const wxString& name,
   const wxFontData& value ) : wxFontPropertyClass(label,name,value.GetInitialFont())
{
    // Initialize custom default value.
    if ( !wxPG_VALUETYPE_PTR(wxFontData) )
    {
        // Make sure custom value type is initialized.
        wxPG_INIT_REQUIRED_TYPE(wxFontData)
        // Set a more sophisticated default value.
        wxFontData& def_value = (wxFontData&)*wxPG_VALUETYPE_PTR(wxFontData)->GetDefaultValue().GetWxObjectPtr();
        def_value.SetChosenFont(*wxNORMAL_FONT);
        def_value.SetColour(*wxBLACK);
    }

    // Set initial value - should be done in a simpler way like this
    // (instead of calling DoSetValue) in derived (wxObject) properties.
    m_value_wxFontData = value;

    // Fix value.
    m_value_wxFontData.SetChosenFont(value.GetInitialFont());
    if ( !m_value_wxFontData.GetColour().Ok() )
        m_value_wxFontData.SetColour(*wxBLACK);

    // Add extra children.
    AddChild( wxColourProperty(_("Colour"),wxPG_LABEL,
              m_value_wxFontData.GetColour() ) );

}

wxFontDataPropertyClass::~wxFontDataPropertyClass () { }

void wxFontDataPropertyClass::DoSetValue( wxPGVariant value )
{
    wxFontData* pObj = wxPGVariantToWxObjectPtr(value,wxFontData);
    m_value_wxFontData = *pObj;
    wxFont font = pObj->GetChosenFont();
    wxFontPropertyClass::DoSetValue(wxPGVariantCreator(&font));
}

wxPGVariant wxFontDataPropertyClass::DoGetValue() const
{
    return wxPGVariantCreator(m_value_wxFontData);
}

// Must re-create font dialog displayer.
bool wxFontDataPropertyClass::OnEvent( wxPropertyGrid* propgrid, wxWindow* primary,
   wxEvent& event )
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        // Update value from last minute changes
        PrepareValueForDialogEditing(propgrid);

        m_value_wxFontData.SetInitialFont(m_value_wxFontData.GetChosenFont());

        wxFontDialog dlg(propgrid, m_value_wxFontData);

        if ( dlg.ShowModal() == wxID_OK )
        {
            wxFontData& fd = dlg.GetFontData();
            wxFontDataPropertyClass::DoSetValue ( &fd );
            UpdateControl (primary);
            return TRUE;
        }
    }
    return FALSE;
}

void wxFontDataPropertyClass::RefreshChildren()
{
    wxFontPropertyClass::RefreshChildren();
    if ( GetCount() < 6 ) // Number is count of inherit prop's children + 1.
        return;
    Item(6)->DoSetValue( wxPGVariantCreator(m_value_wxFontData.GetColour()) );
}

void wxFontDataPropertyClass::ChildChanged ( wxPGProperty* p )
{
    switch ( p->GetIndexInParent() )
    {
        case 6: m_value_wxFontData.SetColour( *((wxColour*)p->DoGetValue().GetWxObjectPtr()) ); break;
        default:
            wxFontPropertyClass::ChildChanged(p);
            // Transfer subset to superset.
            m_value_wxFontData.SetChosenFont(m_value_wxFont);
    }
}

// -----------------------------------------------------------------------
// wxLongLongProperty
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxLongLong,wxLongLongProperty,wxLongLong(0))

class wxLongLongPropertyClass : public wxBasePropertyClass
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxLongLongPropertyClass( const wxString& label, const wxString& name, const wxLongLong& value );
    virtual ~wxLongLongPropertyClass ();

    WX_PG_DECLARE_BASIC_TYPE_METHODS()

protected:
    wxLongLong                  m_value;
};

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxLongLongProperty,wxBaseProperty,
                               wxLongLong,const wxLongLong&,TextCtrl)

wxLongLongPropertyClass::wxLongLongPropertyClass ( const wxString& label, const wxString& name,
    const wxLongLong& value) : wxBasePropertyClass(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxLongLong)
    m_value = value;
}

wxLongLongPropertyClass::~wxLongLongPropertyClass() { }

void wxLongLongPropertyClass::DoSetValue( wxPGVariant value )
{
    m_value = wxPGVariantToWxLongLong(value);
}

wxPGVariant wxLongLongPropertyClass::DoGetValue() const
{
    return wxPGVariantCreator(m_value);
}

wxString wxLongLongPropertyClass::GetValueAsString( int WXUNUSED(argFlags) ) const
{
    return m_value.ToString();
}

bool wxLongLongPropertyClass::SetValueFromString( const wxString& text, int )
{
    double temp;

    if ( !text.ToDouble(&temp) )
        return false;

    wxLongLong value;
    value.Assign(temp);

    if ( m_value != value )
    {
        m_value = value;
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------
// wxSizeProperty
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxSize,wxSizeProperty,wxSize(0,0))

class wxSizePropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxSizePropertyClass( const wxString& label, const wxString& name, const wxSize& value );
    virtual ~wxSizePropertyClass ();

    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
    WX_PG_DECLARE_PARENTAL_METHODS()

protected:

    // I stands for internal
    void SetValueI( const wxSize& value )
    {
        m_value = value;
        RefreshChildren();
    }

    wxSize                  m_value;
};

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxSizeProperty,wxBaseParentProperty,
                               wxSize,const wxSize&,TextCtrl)

wxSizePropertyClass::wxSizePropertyClass ( const wxString& label, const wxString& name,
    const wxSize& value) : wxPGPropertyWithChildren(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxSize)
    SetValueI(value);
    AddChild( wxIntProperty(wxT("Width"),wxPG_LABEL,value.x) );
    AddChild( wxIntProperty(wxT("Height"),wxPG_LABEL,value.y) );
}

wxSizePropertyClass::~wxSizePropertyClass() { }

void wxSizePropertyClass::DoSetValue( wxPGVariant value )
{
    SetValueI( wxPGVariantToWxSize(value) );
}

wxPGVariant wxSizePropertyClass::DoGetValue() const
{
    return wxPGVariantCreator(m_value);
}

void wxSizePropertyClass::RefreshChildren()
{
    if ( !GetCount() ) return;
    Item(0)->DoSetValue( (long)m_value.x );
    Item(1)->DoSetValue( (long)m_value.y );
}

void wxSizePropertyClass::ChildChanged ( wxPGProperty* p )
{
    switch ( p->GetIndexInParent() )
    {
        case 0: m_value.x = p->DoGetValue().GetLong(); break;
        case 1: m_value.y = p->DoGetValue().GetLong(); break;
    }
}

// -----------------------------------------------------------------------
// wxPointProperty
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxPoint,wxPointProperty,wxPoint(0,0))

class wxPointPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxPointPropertyClass( const wxString& label, const wxString& name, const wxPoint& value );
    virtual ~wxPointPropertyClass ();

    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
    WX_PG_DECLARE_PARENTAL_METHODS()

protected:

    // I stands for internal
    void SetValueI( const wxPoint& value )
    {
        m_value = value;
        RefreshChildren();
    }

    wxPoint                  m_value;
};

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPointProperty,wxBaseParentProperty,
                               wxPoint,const wxPoint&,TextCtrl)

wxPointPropertyClass::wxPointPropertyClass( const wxString& label, const wxString& name,
    const wxPoint& value) : wxPGPropertyWithChildren(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxPoint)
    SetValueI(value);
    AddChild( wxIntProperty(wxT("X"),wxPG_LABEL,value.x) );
    AddChild( wxIntProperty(wxT("Y"),wxPG_LABEL,value.y) );
}

wxPointPropertyClass::~wxPointPropertyClass() { }

void wxPointPropertyClass::DoSetValue( wxPGVariant value )
{
    SetValueI( wxPGVariantToWxPoint(value) );
}

wxPGVariant wxPointPropertyClass::DoGetValue() const
{
    return wxPGVariantCreator(m_value);
}

void wxPointPropertyClass::RefreshChildren()
{
    if ( !GetCount() ) return;
    Item(0)->DoSetValue( (long)m_value.x );
    Item(1)->DoSetValue( (long)m_value.y );
}

void wxPointPropertyClass::ChildChanged( wxPGProperty* p )
{
    switch ( p->GetIndexInParent() )
    {
        case 0: m_value.x = p->DoGetValue().GetLong(); break;
        case 1: m_value.y = p->DoGetValue().GetLong(); break;
    }
}


// -----------------------------------------------------------------------
// Dirs Property
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(wxDirsProperty,wxT(','),wxT("Browse"))

#if wxUSE_VALIDATORS

wxValidator* wxDirsPropertyClass::DoGetValidator() const
{
    return wxFilePropertyClass::GetClassValidator();
}

#endif


bool wxDirsPropertyClass::OnCustomStringEdit ( wxWindow* parent, wxString& value )
{
    wxDirDialog dlg(parent,
                    _("Select a directory to be added to the list:"),
                    value,
                    0);

    if ( dlg.ShowModal() == wxID_OK )
    {
        value = dlg.GetPath();
        return TRUE;
    }
    return FALSE;
}

// -----------------------------------------------------------------------
// wxArrayDoubleEditorDialog
// -----------------------------------------------------------------------


//
// You can *almost* convert wxArrayDoubleEditorDialog to wxArrayXXXEditorDialog
// by replacing each ArrayDouble with ArrayXXX.
//

class wxArrayDoubleEditorDialog : public wxArrayEditorDialog
{
public:
    wxArrayDoubleEditorDialog();

    void Init();

    wxArrayDoubleEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              wxArrayDouble& array,
                              long style = wxAEDIALOG_STYLE,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& sz = wxDefaultSize );

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                wxArrayDouble& array,
                long style = wxAEDIALOG_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& sz = wxDefaultSize );

    const wxArrayDouble& GetArray() const { return m_array; }

    // Extra method for this type of array
    inline void SetPrecision ( int precision )
    {
        m_precision = precision;
        m_dtoaTemplate.Empty();
    }

protected:
    // Mandatory array of type
    wxArrayDouble   m_array;

    // Use this to avoid extra wxString creation+Printf
    // on double-to-wxString conversion.
    wxString        m_dtoaTemplate;

    int             m_precision;

    // Mandatory overridden methods
    virtual wxString ArrayGet( size_t index );
    virtual size_t ArrayGetCount();
    virtual bool ArrayInsert( const wxString& str, int index );
    virtual bool ArraySet( size_t index, const wxString& str );
    virtual void ArrayRemoveAt( int index );
    virtual void ArraySwap( size_t first, size_t second );

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxArrayDoubleEditorDialog)
};

IMPLEMENT_DYNAMIC_CLASS(wxArrayDoubleEditorDialog, wxArrayEditorDialog)

//
// Array dialog array access and manipulation
//

wxString wxArrayDoubleEditorDialog::ArrayGet( size_t index )
{
    wxString str;
    wxPropertyGrid::DoubleToString(str,m_array[index],m_precision,true,&m_dtoaTemplate);
    return str;
}

size_t wxArrayDoubleEditorDialog::ArrayGetCount()
{
    return m_array.GetCount();
}

bool wxArrayDoubleEditorDialog::ArrayInsert( const wxString& str, int index )
{
    double d;
    if ( !str.ToDouble(&d) )
        return FALSE;

    if (index<0)
        m_array.Add(d);
    else
        m_array.Insert(d,index);
    return TRUE;
}

bool wxArrayDoubleEditorDialog::ArraySet( size_t index, const wxString& str )
{
    double d;
    if ( !str.ToDouble(&d) )
        return FALSE;
    m_array[index] = d;
    return TRUE;
}

void wxArrayDoubleEditorDialog::ArrayRemoveAt( int index )
{
    m_array.RemoveAt(index);
}

void wxArrayDoubleEditorDialog::ArraySwap( size_t first, size_t second )
{
    double a = m_array[first];
    double b = m_array[second];
    m_array[first] = b;
    m_array[second] = a;
}

//
// Array dialog construction etc.
//

wxArrayDoubleEditorDialog::wxArrayDoubleEditorDialog()
    : wxArrayEditorDialog()
{
    Init();
}

void wxArrayDoubleEditorDialog::Init()
{
    wxArrayEditorDialog::Init();
    SetPrecision(-1);
}

wxArrayDoubleEditorDialog::wxArrayDoubleEditorDialog(wxWindow *parent,
                              const wxString& message,
                              const wxString& caption,
                              wxArrayDouble& array,
                              long style,
                              const wxPoint& pos,
                              const wxSize& sz )
                              : wxArrayEditorDialog()
{
    Init();
    Create(parent,message,caption,array,style,pos,sz);
}

bool wxArrayDoubleEditorDialog::Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                wxArrayDouble& array,
                long style,
                const wxPoint& pos,
                const wxSize& sz )
{

    m_array = array;

    return wxArrayEditorDialog::Create (parent,message,caption,style,pos,sz);
}

// -----------------------------------------------------------------------
// wxArrayDoubleProperty
// -----------------------------------------------------------------------

#include <math.h> // for fabs

#if wxMINOR_VERSION < 7 || ( wxMINOR_VERSION == 7 && wxRELEASE_NUMBER < 1 )
// NOTE: Since wxWidgets at this point doesn't have wxArrayDouble, we have
//   to create it ourself, using wxObjArray model.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayDouble);
#endif

// Comparison required by value type implementation.
bool operator == (const wxArrayDouble& a, const wxArrayDouble& b)
{
    if ( a.GetCount() != b.GetCount() )
        return FALSE;

    size_t i;

    for ( i=0; i<a.GetCount(); i++ )
    {
        // Can't do direct equality comparison with floating point numbers.
        if ( fabs(a[i] - b[i]) > 0.0000000001 )
        {
            //wxLogDebug(wxT("%f != %f"),a[i],b[i]);
            return FALSE;
        }
    }
    return TRUE;
}


WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxArrayDouble,
                                 wxArrayDoubleProperty,
                                 wxArrayDouble())


class wxArrayDoublePropertyClass : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxArrayDoublePropertyClass (const wxString& label,
                                const wxString& name,
                                const wxArrayDouble& value);

    virtual ~wxArrayDoublePropertyClass ();

    WX_PG_DECLARE_BASIC_TYPE_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()
    WX_PG_DECLARE_ATTRIBUTE_METHODS()

    // Generates cache for displayed text
    virtual void GenerateValueAsString ( wxString& target, int prec, bool removeZeroes ) const;

protected:
    wxArrayDouble   m_value;
    wxString        m_display; // Stores cache for displayed text
    int             m_precision; // Used when formatting displayed string.
    wxChar          m_delimiter; // Delimiter between array entries.
};


WX_PG_IMPLEMENT_PROPERTY_CLASS(wxArrayDoubleProperty,
                               wxBaseProperty,
                               wxArrayDouble,
                               const wxArrayDouble&,
                               TextCtrlAndButton)


wxArrayDoublePropertyClass::wxArrayDoublePropertyClass (const wxString& label,
                                                        const wxString& name,
                                                        const wxArrayDouble& array )
    : wxPGProperty(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxArrayDouble)

    m_precision = -1;

    //
    // Need to figure out delimiter needed for this locale
    // (ie. can't use comma when comma acts as decimal point in float).
    wxChar use_delimiter = wxT(',');

    if (wxString::Format(wxT("%.2f"),12.34).Find(use_delimiter) >= 0)
        use_delimiter = wxT(';');

    m_delimiter = use_delimiter;

    if ( &array )
        DoSetValue( (void*)&array );
}

wxArrayDoublePropertyClass::~wxArrayDoublePropertyClass () { }

void wxArrayDoublePropertyClass::DoSetValue ( wxPGVariant value )
{
    m_value = *((wxArrayDouble*)value.GetVoidPtr());
    GenerateValueAsString( m_display, m_precision, true );
}

wxPGVariant wxArrayDoublePropertyClass::DoGetValue () const
{
    return wxPGVariant((void*)&m_value);
}

wxString wxArrayDoublePropertyClass::GetValueAsString ( int arg_flags ) const
{
    if ( !(arg_flags & wxPG_FULL_VALUE ))
        return m_display;

    wxString s;
    GenerateValueAsString(s,-1,false);
    return s;
}

void wxArrayDoublePropertyClass::GenerateValueAsString ( wxString& target, int prec, bool removeZeroes ) const
{
    wxString s;
    wxString template_str;
    wxChar between[3] = wxT(", ");
    size_t i;

    between[0] = m_delimiter;

    target.Empty();

    for ( i=0; i<m_value.GetCount(); i++ )
    {

        wxPropertyGrid::DoubleToString(s,m_value[i],prec,removeZeroes,&template_str);

        target += s;

        if ( i<(m_value.GetCount()-1) )
            target += between;
    }
}

bool wxArrayDoublePropertyClass::OnEvent (wxPropertyGrid* propgrid,
                                          wxWindow* primary,
                                          wxEvent& event)
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        // Update the value in case of last minute changes
        PrepareValueForDialogEditing(propgrid);

        // Create editor dialog.
        wxArrayDoubleEditorDialog dlg;
        dlg.SetPrecision(m_precision);
        dlg.Create( propgrid, wxEmptyString, m_label, m_value );
        dlg.Move( propgrid->GetGoodEditorDialogPosition (this,dlg.GetSize()) );

        // Execute editor dialog
        int res = dlg.ShowModal();
        if ( res == wxID_OK && dlg.IsModified() )
        {
            DoSetValue ( (void*)&dlg.GetArray() );
            UpdateControl ( primary );
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

bool wxArrayDoublePropertyClass::SetValueFromString ( const wxString& text, int )
{
    double tval;
    wxString tstr;
    // Add values to a temporary array so that in case
    // of error we can opt not to use them.
    wxArrayDouble new_array;

    bool ok = TRUE;

    wxChar delimiter = m_delimiter;

    WX_PG_TOKENIZER1_BEGIN(text,delimiter)

        if ( token.length() )
        {

            // If token was invalid, exit the loop now
            if ( !token.ToDouble(&tval) )
            {
                tstr.Printf ( _("\"%s\" is not a floating-point number."), token.c_str() );
                ok = FALSE;
                break;
            }
            // TODO: Put validator code here

            new_array.Add(tval);

        }

    WX_PG_TOKENIZER1_END()

    // When invalid token found, show error message and don't change anything
    if ( !ok )
    {
        ShowError( tstr );
        return FALSE;
    }

    if ( !(m_value == new_array) )
    {
        m_value = new_array;

        GenerateValueAsString( m_display, m_precision, true );

        return TRUE;
    }

    return FALSE;
}

void wxArrayDoublePropertyClass::SetAttribute ( int id, wxVariant& value )
{
    if ( id == wxPG_FLOAT_PRECISION )
    {
        m_precision = value.GetLong();
        GenerateValueAsString( m_display, m_precision, true );
    }
}


// -----------------------------------------------------------------------
// wxPaintedEnumPropertyClass
//   wxEnumProperty with totally custom painted list items.
// -----------------------------------------------------------------------

wxPaintedEnumPropertyClass::wxPaintedEnumPropertyClass(const wxString& label, const wxString& name,
                                             const wxArrayString& labels, const wxArrayInt& values,
                                             int value)
    : wxEnumPropertyClass(label,name,labels,values,value)
{
}

wxSize wxPaintedEnumPropertyClass::GetImageSize() const
{
    // Custom paint whole item, with default height of -1
    return wxPG_FULL_CUSTOM_PAINT_SIZE(-1);
}

void wxPaintedEnumPropertyClass::OnCustomPaint(wxDC& dc, const wxRect& rect, wxPGPaintData& paintdata)
{
    int index = paintdata.m_choiceItem;

    if ( index < 0 )
    {
        // paintdata.m_choiceItem is -1 if control itself is being painted
        // set paintdata.m_drawnWidth to 0 ensure the default drawing will occur
        paintdata.m_drawnWidth = 0;
        return;
    }

    wxString text = m_choices.GetLabel(index);

    // Draw text
    if ( index & 1 )
        dc.SetTextForeground(*wxGREEN);
    else
        dc.SetTextForeground(*wxRED);

    dc.DrawText( text, rect.x + 1, rect.y + 1 );
}


#endif // !defined(WX_PROPGRID_SAMPLEPROPS_H) || defined(__WXPYTHON__)
