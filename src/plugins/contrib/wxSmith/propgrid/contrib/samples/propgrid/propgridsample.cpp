/////////////////////////////////////////////////////////////////////////////
// Name:        propgridsample.cpp
// Purpose:     wxPropertyGrid sample
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

//
//
// NOTES
//
// * Examples of custom property classes are in sampleprops.cpp.
//
// * Additional ones can be found below.
//
// * Currently there is no example of a custom property editor. However,
//   SpinCtrl editor sample is well-commented. It can be found in
//   contrib/src/propgrid/advprops.cpp.
//
// * To find code that populates the grid with properties, search for
//   string "::Populate".
//
// * To find code that handles property grid changes, search for string
//   "::OnPropertyGridChange".
//
// * At the end of file there is example code for using the owner-drawn combo
//   box independently outside the wxPropertyGrid.
//
//

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


// Main propertygrid header.
#include <wx/propgrid/propgrid.h>

// Needed for implementing custom properties.
#include <wx/propgrid/propdev.h>

// Extra property classes.
#include <wx/propgrid/advprops.h>

// This defines wxPropertyGridManager.
#include <wx/propgrid/manager.h>

#include "propgridsample.h"
#include "sampleprops.h"

#if wxUSE_DATEPICKCTRL
    #include <wx/datectrl.h>
#endif

// -----------------------------------------------------------------------
// wxTestCustomFlagsProperty
// -----------------------------------------------------------------------

//
// Constant definitions required by wxFlagsProperty examples.
//

static const wxChar* _fs_framestyle_labels[] = {
    wxT("wxCAPTION"),
    wxT("wxMINIMIZE"),
    wxT("wxMAXIMIZE"),
    wxT("wxCLOSE_BOX"),
    wxT("wxSTAY_ON_TOP"),
    wxT("wxSYSTEM_MENU"),
    wxT("wxRESIZE_BORDER"),
    wxT("wxFRAME_TOOL_WINDOW"),
    wxT("wxFRAME_NO_TASKBAR"),
    wxT("wxFRAME_FLOAT_ON_PARENT"),
    wxT("wxFRAME_SHAPED"),
    (const wxChar*) NULL
};

static const long _fs_framestyle_values[] = {
    wxCAPTION,
    wxMINIMIZE,
    wxMAXIMIZE,
    wxCLOSE_BOX,
    wxSTAY_ON_TOP,
    wxSYSTEM_MENU,
    wxRESIZE_BORDER,
    wxFRAME_TOOL_WINDOW,
    wxFRAME_NO_TASKBAR,
    wxFRAME_FLOAT_ON_PARENT,
    wxFRAME_SHAPED
};


WX_PG_IMPLEMENT_CUSTOM_FLAGS_PROPERTY(wxTestCustomFlagsProperty,
                                      _fs_framestyle_labels,
                                      _fs_framestyle_values,
                                      wxDEFAULT_FRAME_STYLE)

WX_PG_IMPLEMENT_CUSTOM_ENUM_PROPERTY(wxTestCustomEnumProperty,
                                      _fs_framestyle_labels,
                                      _fs_framestyle_values,
                                      wxCAPTION)


// Colour labels. Last (before NULL, if any) must be Custom.
static const wxChar* mycolprop_labels[] = {
    wxT("Black"),
    wxT("Blue"),
    wxT("Brown"),
    wxT("Custom"),
    (const wxChar*) NULL
};

// Relevant colour values as unsigned longs.
static unsigned long mycolprop_colours[] = {
    wxPG_COLOUR(0,0,0),
    wxPG_COLOUR(0,0,255),
    wxPG_COLOUR(166,124,81),
    wxPG_COLOUR(0,0,0)
};

// Implement property class. Third argument is optional values array,
// but in this example we are only interested in creating a shortcut
// for user to access the colour values. Last arg is itemcount, but
// it will be deprecated in the future.
WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty,
                                                     mycolprop_labels,
                                                     (long*)NULL,
                                                     mycolprop_colours)


WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY(wxMyColour2Property,
                                       mycolprop_labels,
                                       (long*)NULL,
                                       mycolprop_colours)



// Just testing the macros
WX_PG_DECLARE_STRING_PROPERTY(wxTestStringProperty)
WX_PG_IMPLEMENT_STRING_PROPERTY(wxTestStringProperty,wxPG_NO_ESCAPE)
bool wxTestStringPropertyClass::OnButtonClick( wxPropertyGrid*,
                                               wxString& )
{
    ::wxMessageBox(wxT("Button Clicked"));
    return true;
}

WX_PG_DECLARE_STRING_PROPERTY(wxTextStringPropertyWithValidator)
WX_PG_IMPLEMENT_STRING_PROPERTY_WITH_VALIDATOR(wxTextStringPropertyWithValidator,
                                               wxPG_NO_ESCAPE)

bool wxTextStringPropertyWithValidatorClass::OnButtonClick( wxPropertyGrid* WXUNUSED(propgrid),
                                                            wxString& WXUNUSED(value) )
{
    ::wxMessageBox(wxT("Button Clicked"));
    return true;
}

wxValidator* wxTextStringPropertyWithValidatorClass::DoGetValidator() const
{
    WX_PG_DOGETVALIDATOR_ENTRY()
    wxTextValidator* validator = new
        wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString oValid;
    oValid.Add(wxT("0"));
    oValid.Add(wxT("1"));
    oValid.Add(wxT("2"));
    oValid.Add(wxT("3"));
    oValid.Add(wxT("4"));
    oValid.Add(wxT("5"));
    oValid.Add(wxT("6"));
    oValid.Add(wxT("7"));
    oValid.Add(wxT("8"));
    oValid.Add(wxT("9"));
    oValid.Add(wxT("$"));
    validator->SetIncludes(oValid);
    WX_PG_DOGETVALIDATOR_EXIT(validator)
}

// -----------------------------------------------------------------------

// wxValidator for testing

class wxInvalidWordValidator : public wxValidator
{
public:

    wxInvalidWordValidator( const wxString& invalidWord )
        : wxValidator(), m_invalidWord(invalidWord)
    {
    }

    virtual wxObject* Clone() const
    {
        return new wxInvalidWordValidator(m_invalidWord);
    }

    virtual bool Validate(wxWindow* WXUNUSED(parent))
    {
        wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
        wxCHECK_MSG(tc, true, wxT("validator window must be wxTextCtrl"));

        wxString val = tc->GetValue();

        if ( val.find(m_invalidWord) == wxString::npos )
            return true;

        ::wxMessageBox(wxString::Format(wxT("%s is not allowed word"),m_invalidWord.c_str()),
                       wxT("Validation Failure"));

        return false;
    }

private:
    wxString    m_invalidWord;
};

// -----------------------------------------------------------------------
// Test STL-container as a value type.
// -----------------------------------------------------------------------

WX_DECLARE_STRING_HASH_MAP(wxArrayString, ImageItemMap);

//declare new value type
WX_PG_DECLARE_VALUE_TYPE_VOIDP(ImageItemMap)

// declare property class using the value type
WX_PG_DECLARE_PROPERTY(ImageItemMapProperty, const ImageItemMap&, ImageItemMap())

class ImageItemMapPropertyClass : public wxLongStringPropertyClass
{
	WX_PG_DECLARE_PROPERTY_CLASS()
public:
	ImageItemMapPropertyClass(const wxString& label = wxEmptyString,
                              const wxString& name = wxPG_LABEL,
                              const ImageItemMap& value = ImageItemMap());
    virtual ~ImageItemMapPropertyClass() { }

	virtual void DoSetValue(wxPGVariant value);
	virtual wxPGVariant ImageItemMapPropertyClass::DoGetValue() const;
	virtual bool SetValueFromString(const wxString& text, int );
	virtual wxString GetValueAsString(int) const;
	virtual bool OnEvent(wxPropertyGrid* propGrid, wxWindow* primary, wxEvent& event);

protected:
	ImageItemMap	m_value_ImageItemMap;
};


// implement (dummy) equality operator
bool operator == (const ImageItemMap&, const ImageItemMap&)
{
    return true;
}

// implement property class using the value type
WX_PG_IMPLEMENT_PROPERTY_CLASS(ImageItemMapProperty,  // Name of the property
                               wxLongStringProperty,  // Name of the parent property
                               ImageItemMap,  // Name of the used data type
                               const ImageItemMap&,  // Data type as function argument
                               TextCtrlAndButton)  // Used editor control

// implement value type
WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(ImageItemMap, ImageItemMapProperty, ImageItemMap())


ImageItemMapPropertyClass::ImageItemMapPropertyClass(const wxString& label,
                                                     const wxString& name,
                                                     const ImageItemMap& value)
	: wxLongStringPropertyClass(label, name, wxEmptyString)
{
	m_value_ImageItemMap = value;
}

void ImageItemMapPropertyClass::DoSetValue(wxPGVariant value)
{
    m_value_ImageItemMap = *((ImageItemMap*)value.GetRawPtr());
}

wxPGVariant ImageItemMapPropertyClass::DoGetValue() const
{
    return wxPGVariant((void*)&m_value);
}

bool ImageItemMapPropertyClass::SetValueFromString(const wxString& WXUNUSED(text), int)
{
	// You can try to get away without implementing this
	wxFAIL_MSG(wxT("TODO"));
    return true;
}

wxString ImageItemMapPropertyClass::GetValueAsString(int) const
{
	// You need to implement this, otherwise value cannot be represented as text
	wxString sValue;
	wxFAIL_MSG(wxT("TODO"));
    return sValue;
}

bool ImageItemMapPropertyClass::OnEvent(wxPropertyGrid* propGrid, wxWindow* WXUNUSED(primary),
                                        wxEvent& event)
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        // Update the value
        PrepareValueForDialogEditing(propGrid);

        // Run editor dialog.
        wxFAIL_MSG(wxT("TODO"));
    }
    return false;
}

// -----------------------------------------------------------------------
// AdvImageFile Property
// -----------------------------------------------------------------------

class wxMyImageInfo;

WX_DECLARE_OBJARRAY(wxMyImageInfo, wxArrayMyImageInfo);

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


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayMyImageInfo);

wxArrayString       g_myImageNames;
wxArrayMyImageInfo  g_myImageArray;


// Preferred thumbnail height.
#define PREF_THUMBNAIL_HEIGHT       64


class wxAdvImageFilePropertyClass : public wxFilePropertyClass
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxAdvImageFilePropertyClass( const wxString& label,
                                 const wxString& name,
                                 const wxString& value );
    virtual ~wxAdvImageFilePropertyClass ();

    void DoSetValue( wxPGVariant value ); // Override to allow image loading.

    WX_PG_DECLARE_CHOICE_METHODS()
    WX_PG_DECLARE_EVENT_METHODS()
    WX_PG_DECLARE_CUSTOM_PAINT_METHODS()

    void LoadThumbnails( size_t n );

protected:
    wxImage*    m_pImage; // Temporary thumbnail data.

    int m_index; // Index required for choice behaviour.
};


WX_PG_IMPLEMENT_PROPERTY_CLASS(wxAdvImageFileProperty,wxFileProperty,
                               wxString,const wxString&,ChoiceAndButton)


wxAdvImageFilePropertyClass::wxAdvImageFilePropertyClass( const wxString& label,
                                                          const wxString& name,
                                                          const wxString& value)
    : wxFilePropertyClass(label,name,value)
{
    m_wildcard = wxPGGetDefaultImageWildcard();

    m_index = -1;

    m_pImage = (wxImage*) NULL;

    // Only show names.
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

void wxAdvImageFilePropertyClass::DoSetValue( wxPGVariant value )
{
    wxFilePropertyClass::DoSetValue(value);

    // Delete old image
    if ( m_pImage )
    {
        delete m_pImage;
        m_pImage = (wxImage*) NULL;
    }

    wxString imagename = GetValueAsString(0);

    if ( imagename.length() )
    {

        int index = g_myImageNames.Index(imagename);

        // If not in table, add now.
        if ( index == wxNOT_FOUND )
        {
            g_myImageNames.Add( imagename );
            g_myImageArray.Add( new wxMyImageInfo( m_filename.GetFullPath() ) );

            index = g_myImageArray.GetCount() - 1;
        }

        // If no thumbnail ready, then need to load image.
        if ( !g_myImageArray[index].m_pThumbnail2 )
        {
            // Load if file exists.
            if ( m_filename.FileExists() )
                m_pImage = new wxImage( m_filename.GetFullPath() );
        }

        m_index = index;

    }
    else
        m_index = -1;

}

int wxAdvImageFilePropertyClass::GetChoiceInfo( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
    {
        choiceinfo->m_itemCount = g_myImageNames.GetCount();
        if ( g_myImageNames.GetCount() )
            choiceinfo->m_arrWxString = &g_myImageNames.Item(0);
    }
    return m_index;
}

bool wxAdvImageFilePropertyClass::SetValueFromInt( long value, int WXUNUSED(flags) )
{
    wxASSERT( value >= 0 );
    m_index = value;
    return SetValueFromString( g_myImageNames.Item(value), wxPG_FULL_VALUE );
}

bool wxAdvImageFilePropertyClass::OnEvent( wxPropertyGrid* propgrid, wxWindow* primary,
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
                    GetEditorClass()->InsertItem(primary,name,-1);

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

void wxAdvImageFilePropertyClass::LoadThumbnails( size_t index )
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

            m_pImage->Rescale( im_wid, im_hei );

            mii.m_pThumbnail2 = new wxBitmap( *m_pImage );

            wxSize cis = GetParentState()->GetGrid()->GetImageSize();
            m_pImage->Rescale ( cis.x, cis.y );

            mii.m_pThumbnail1 = new wxBitmap( *m_pImage );

        }

        if ( m_pImage )
        {
            delete m_pImage;
            m_pImage = (wxImage*) NULL;
        }
    }
}

void wxAdvImageFilePropertyClass::OnCustomPaint( wxDC& dc,
                                                 const wxRect& rect,
                                                 wxPGPaintData& pd )
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


// -----------------------------------------------------------------------
// wxVectorProperty
// -----------------------------------------------------------------------

// See propgridsample.h for wxVector3f


WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxVector3f,wxVectorProperty,wxVector3f())


class wxVectorPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxVectorPropertyClass( const wxString& label,
                           const wxString& name,
                           const wxVector3f& value );
    virtual ~wxVectorPropertyClass ();

    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
    WX_PG_DECLARE_PARENTAL_METHODS()

protected:
    wxVector3f          m_value;
};


WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVectorProperty,wxBaseParentProperty,
                               wxVector3f,const wxVector3f&,TextCtrl)


wxVectorPropertyClass::wxVectorPropertyClass( const wxString& label,
                                              const wxString& name,
                                              const wxVector3f& value)
    : wxPGPropertyWithChildren(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxVector3f)
    DoSetValue((void*)&value);
    AddChild( wxFloatProperty(wxT("X"),wxPG_LABEL,value.x) );
    AddChild( wxFloatProperty(wxT("Y"),wxPG_LABEL,value.y) );
    AddChild( wxFloatProperty(wxT("Z"),wxPG_LABEL,value.z) );
}

wxVectorPropertyClass::~wxVectorPropertyClass() { }

void wxVectorPropertyClass::DoSetValue( wxPGVariant value )
{
    wxVector3f* pObj = (wxVector3f*)wxPGVariantToVoidPtr(value);
    m_value = *pObj;
    RefreshChildren();
}

wxPGVariant wxVectorPropertyClass::DoGetValue() const
{
    return wxPGVariant((void*)&m_value);
}

void wxVectorPropertyClass::RefreshChildren()
{
    if ( !GetCount() ) return;
    Item(0)->DoSetValue( m_value.x );
    Item(1)->DoSetValue( m_value.y );
    Item(2)->DoSetValue( m_value.z );
}

void wxVectorPropertyClass::ChildChanged( wxPGProperty* p )
{
    switch ( p->GetIndexInParent() )
    {
        case 0: m_value.x = p->DoGetValue().GetDouble(); break;
        case 1: m_value.y = p->DoGetValue().GetDouble(); break;
        case 2: m_value.z = p->DoGetValue().GetDouble(); break;
    }
}


// -----------------------------------------------------------------------
// wxTriangleProperty
// -----------------------------------------------------------------------

// See propgridsample.h for wxTriangle


WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxTriangle,wxTriangleProperty,wxTriangle())


class wxTrianglePropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxTrianglePropertyClass( const wxString& label,
                             const wxString& name,
                             const wxTriangle& value );
    virtual ~wxTrianglePropertyClass();

    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
    WX_PG_DECLARE_PARENTAL_METHODS()

protected:
    wxTriangle            m_value;
};


WX_PG_IMPLEMENT_PROPERTY_CLASS(wxTriangleProperty,wxBaseParentProperty,
                               wxTriangle,const wxTriangle&,TextCtrl)


wxTrianglePropertyClass::wxTrianglePropertyClass( const wxString& label,
                                                  const wxString& name,
                                                  const wxTriangle& value)
    : wxPGPropertyWithChildren(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxTriangle)
    DoSetValue((void*)&value);
    AddChild( wxVectorProperty(wxT("A"),wxPG_LABEL,value.a) );
    AddChild( wxVectorProperty(wxT("B"),wxPG_LABEL,value.b) );
    AddChild( wxVectorProperty(wxT("C"),wxPG_LABEL,value.c) );
}

wxTrianglePropertyClass::~wxTrianglePropertyClass() { }

void wxTrianglePropertyClass::DoSetValue( wxPGVariant value )
{
    wxTriangle* pObj = (wxTriangle*)wxPGVariantToVoidPtr(value);
    m_value = *pObj;
    RefreshChildren();
}

wxPGVariant wxTrianglePropertyClass::DoGetValue() const
{
    return wxPGVariant((void*)&m_value);
}

void wxTrianglePropertyClass::RefreshChildren()
{
    if ( !GetCount() ) return;
    Item(0)->DoSetValue( (void*) &m_value.a );
    Item(1)->DoSetValue( (void*) &m_value.b );
    Item(2)->DoSetValue( (void*) &m_value.c );
}

void wxTrianglePropertyClass::ChildChanged( wxPGProperty* p )
{
    switch ( p->GetIndexInParent() )
    {
        case 0: m_value.a = *((wxVector3f*) p->DoGetValue().GetVoidPtr()); break;
        case 1: m_value.b = *((wxVector3f*) p->DoGetValue().GetVoidPtr()); break;
        case 2: m_value.c = *((wxVector3f*) p->DoGetValue().GetVoidPtr()); break;
    }
}


// -----------------------------------------------------------------------

#include <wx/propgrid/odcombo.h>  // for wxODCCount (which should not be needed in normal apps
                                  // that don't require compliancy between vast number of
                                  // wxWidgets versions).

// -----------------------------------------------------------------------

enum
{
    PGID = 1,
    TCID,
    ID_ABOUT,
    ID_QUIT,
    ID_APPENDPROP,
    ID_APPENDCAT,
    ID_INSERTPROP,
    ID_INSERTCAT,
    ID_ENABLE,
    ID_HIDE,
    ID_DELETE,
    ID_DELETER,
    ID_DELETEALL,
    ID_UNSPECIFY,
    ID_ITERATE1,
    ID_ITERATE2,
    ID_ITERATE3,
    ID_ITERATE4,
    ID_CLEARMODIF,
    ID_FREEZE,
    ID_DUMPLIST,
    ID_COLOURSCHEME1,
    ID_COLOURSCHEME2,
    ID_COLOURSCHEME3,
    ID_COLOURSCHEME4,
    ID_CATCOLOURS,
    ID_SETCOLOUR,
    ID_COMPACT,
    ID_SETASHIDEABLE,
    ID_STATICLAYOUT,
    ID_CLEAR,
    ID_POPULATE1,
    ID_POPULATE2,
    ID_COLLAPSE,
    ID_COLLAPSEALL,
    ID_GETVALUES,
    ID_SETVALUES,
    ID_SETVALUES2,
    ID_RUNTEST,
    ID_SAMPLEDIALOG,
    ID_SAVETOFILE,
    ID_SAVETOFILE2,
    ID_LOADFROMFILE,
    ID_CHANGEFLAGSITEMS,
    ID_TESTINSERTCHOICE,
    ID_TESTDELETECHOICE,
    ID_INSERTPAGE,
    ID_REMOVEPAGE,
    ID_SETSPINCTRLEDITOR,
    ID_SETPROPERTYVALUE,
    ID_TESTREPLACE
};

BEGIN_EVENT_TABLE(FormMain, wxFrame)
    EVT_IDLE(FormMain::OnIdle)
    EVT_MOVE(FormMain::OnMove)
    EVT_SIZE(FormMain::OnResize)
    EVT_PAINT(FormMain::OnPaint)

    // This occurs when a property is selected
    EVT_PG_SELECTED( PGID, FormMain::OnPropertyGridSelect )
    // This occurs when a property value changes
    EVT_PG_CHANGED( PGID, FormMain::OnPropertyGridChange )
    // This occurs when a mouse moves over another property
    EVT_PG_HIGHLIGHTED( PGID, FormMain::OnPropertyGridHighlight )
    // This occurs when mouse is right-clicked.
    EVT_PG_RIGHT_CLICK( PGID, FormMain::OnPropertyGridItemRightClick )
    // This occurs when mouse is double-clicked.
    EVT_PG_DOUBLE_CLICK( PGID, FormMain::OnPropertyGridItemDoubleClick )
    // This occurs when propgridmanager's page changes.
    EVT_PG_PAGE_CHANGED( PGID, FormMain::OnPropertyGridPageChange )
    // This occurs when property's editor button (if any) is clicked.
    EVT_BUTTON( PGID, FormMain::OnPropertyGridButtonClick )

    EVT_PG_ITEM_COLLAPSED( PGID, FormMain::OnPropertyGridItemCollapse )
    EVT_PG_ITEM_EXPANDED( PGID, FormMain::OnPropertyGridItemExpand )

    EVT_TEXT( PGID, FormMain::OnPropertyGridTextUpdate )

    EVT_KEY_DOWN( FormMain::OnPropertyGridKeyEvent )
    EVT_KEY_UP( FormMain::OnPropertyGridKeyEvent )

    EVT_MENU( ID_APPENDPROP, FormMain::OnAppendPropClick )
    EVT_MENU( ID_APPENDCAT, FormMain::OnAppendCatClick )
    EVT_MENU( ID_INSERTPROP, FormMain::OnInsertPropClick )
    EVT_MENU( ID_INSERTCAT, FormMain::OnInsertCatClick )
    EVT_MENU( ID_DELETE, FormMain::OnDelPropClick )
    EVT_MENU( ID_DELETER, FormMain::OnDelPropRClick )
    EVT_MENU( ID_UNSPECIFY, FormMain::OnMisc )
    EVT_MENU( ID_DELETEALL, FormMain::OnClearClick )
    EVT_MENU( ID_ENABLE, FormMain::OnEnableDisable )
    EVT_MENU( ID_HIDE, FormMain::OnHideShow )
    EVT_MENU( ID_SETASHIDEABLE, FormMain::OnSetAsHideable )
    EVT_MENU( ID_ITERATE1, FormMain::OnIterate1Click )
    EVT_MENU( ID_ITERATE2, FormMain::OnIterate2Click )
    EVT_MENU( ID_ITERATE3, FormMain::OnIterate3Click )
    EVT_MENU( ID_ITERATE4, FormMain::OnIterate4Click )
    EVT_MENU( ID_SETCOLOUR, FormMain::OnMisc )
    EVT_MENU( ID_CLEARMODIF, FormMain::OnClearModifyStatusClick )
    EVT_MENU( ID_FREEZE, FormMain::OnFreezeClick )
    EVT_MENU( ID_DUMPLIST, FormMain::OnDumpList )

    EVT_MENU( ID_COLOURSCHEME1, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME2, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME3, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME4, FormMain::OnColourScheme )

    EVT_MENU( ID_ABOUT, FormMain::OnAbout )
    EVT_MENU( ID_QUIT, FormMain::OnCloseClick )

    EVT_MENU( ID_CATCOLOURS, FormMain::OnCatColours )
    EVT_MENU( ID_COMPACT, FormMain::OnCompact )

    EVT_MENU( ID_STATICLAYOUT, FormMain::OnMisc )
    EVT_MENU( ID_CLEAR, FormMain::OnMisc )
    EVT_MENU( ID_COLLAPSE, FormMain::OnMisc )
    EVT_MENU( ID_COLLAPSEALL, FormMain::OnMisc )

    EVT_MENU( ID_POPULATE1, FormMain::OnPopulateClick )
    EVT_MENU( ID_POPULATE2, FormMain::OnPopulateClick )

    EVT_MENU( ID_GETVALUES, FormMain::OnMisc )
    EVT_MENU( ID_SETVALUES, FormMain::OnMisc )
    EVT_MENU( ID_SETVALUES2, FormMain::OnMisc )

    EVT_MENU( ID_SAVETOFILE, FormMain::OnSaveToFileClick )
    EVT_MENU( ID_SAVETOFILE2, FormMain::OnSaveToFileClick )
    EVT_MENU( ID_LOADFROMFILE, FormMain::OnLoadFromFileClick )

    EVT_MENU( ID_CHANGEFLAGSITEMS, FormMain::OnChangeFlagsPropItemsClick )

    EVT_MENU( ID_RUNTEST, FormMain::OnMisc )

    EVT_MENU( ID_SAMPLEDIALOG, FormMain::CustomComboSampleDialog )

    EVT_MENU( ID_TESTINSERTCHOICE, FormMain::OnInsertChoice )
    EVT_MENU( ID_TESTDELETECHOICE, FormMain::OnDeleteChoice )

    EVT_MENU( ID_INSERTPAGE, FormMain::OnInsertPage )
    EVT_MENU( ID_REMOVEPAGE, FormMain::OnRemovePage )

    EVT_MENU( ID_SETSPINCTRLEDITOR, FormMain::OnSetSpinCtrlEditorClick )
    EVT_MENU( ID_TESTREPLACE, FormMain::OnTestReplaceClick )
    EVT_MENU( ID_SETPROPERTYVALUE, FormMain::OnSetPropertyValue )

    EVT_CONTEXT_MENU( FormMain::OnContextMenu )

    // propgrid may be destroyed at close event, so we won't be testing CanClose right now.
    //EVT_CLOSE( FormMain::OnCloseEvent )
END_EVENT_TABLE()

// -----------------------------------------------------------------------

void FormMain::OnMove ( wxMoveEvent& event )
{
    if ( !m_pPropGridMan )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update position properties
    int x, y;
    GetPosition(&x,&y);

    wxPGId id;

#if TESTING_WXPROPERTYGRIDADV

    // Must check if they exist (as they may be deleted).

    // Using m_pPropGridMan, we can scan all pages automatically.
    id = m_pPropGridMan->GetPropertyByName ( wxT("X") );
    if ( id.IsOk() )
        m_pPropGridMan->SetPropertyValue( id, x );

    id = m_pPropGridMan->GetPropertyByName ( wxT("Y") );
    if ( id.IsOk() )
        m_pPropGridMan->SetPropertyValue( id, y );

    id = m_pPropGridMan->GetPropertyByName ( wxT("Position") );
    if ( id.IsOk() )
        m_pPropGridMan->SetPropertyValue( id, wxPoint(x,y) );

#else

    // Since grid may on another page, we must check that the names exist.
    id = m_propGrid->GetPropertyByName ( wxT("X") );
    if ( id.IsOk() )
        m_propGrid->SetPropertyValue( id, x );

    id = m_propGrid->GetPropertyByName ( wxT("Y") );
    if ( id.IsOk() )
        m_propGrid->SetPropertyValue( id, y );

    id = m_propGrid->GetPropertyByName ( wxT("Position") );
    if ( id.IsOk() )
        m_propGrid->SetPropertyValue( id, wxPoint(x,y) );

#endif

    // Should always call event.Skip() in frame's MoveEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnResize( wxSizeEvent& event )
{
    if ( !m_pPropGridMan )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update size properties
    int w, h;
    GetSize(&w,&h);

    wxPGId id;

#if TESTING_WXPROPERTYGRIDADV

    // Must check if they exist (as they may be deleted).

    // Using m_pPropGridMan, we can scan all pages automatically.
    id = m_pPropGridMan->GetPropertyByName ( wxT("Width") );
    if ( id.IsOk() )
        m_pPropGridMan->SetPropertyValue( id, w );

    id = m_pPropGridMan->GetPropertyByName ( wxT("Height") );
    if ( id.IsOk() )
        m_pPropGridMan->SetPropertyValue( id, h );

    id = m_pPropGridMan->GetPropertyByName ( wxT("Size") );
    if ( id.IsOk() )
        m_pPropGridMan->SetPropertyValue( id, wxSize(w,h) );

#else

    // Since grid may on another page, we must check that the names exist.
    id = m_propGrid->GetPropertyByName ( wxT("Width") );
    if ( id.IsOk() )
        m_propGrid->SetPropertyValue( id, w );
    id = m_propGrid->GetPropertyByName ( wxT("Height") );
    if ( id.IsOk() )
        m_propGrid->SetPropertyValue( id, h );

    id = m_propGrid->GetPropertyByName ( wxT("Size") );
    if ( id.IsOk() )
        m_propGrid->SetPropertyValue( id, wxSize(w,h) );

#endif

    // Should always call event.Skip() in frame's SizeEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnPaint( wxPaintEvent& )
{
    wxPaintDC(this);
}

// -----------------------------------------------------------------------

void TestPaintCallback(wxPGProperty* WXUNUSED(property), wxDC& dc,
                       const wxRect& rect, wxPGPaintData& paintdata);

//
// Note how we use three types of value getting in this method:
//   A) event.GetPropertyValueAsXXX
//   B) event.GetPropertValue, and then variant's GetXXX
//   C) grid's GetPropertyValueAsXXX(id)
//
void FormMain::OnPropertyGridChange( wxPropertyGridEvent& event )
{
    wxPGId id = event.GetProperty();

    const wxString& name = event.GetPropertyName();
    wxVariant value = event.GetPropertyValue();

    //wxLogDebug( wxT("Changed value of '%s' to '%s'"),
    //            name.c_str(),
    //            event.GetPropertyValueAsString().c_str() );

    wxPGId cpId = m_pPropGridMan->GetPropertyByName(wxT("CustomProperty"));

    //
    // Must first check ifs a child of CustomProperty
    // (since it also has sub-item named Label, which is also a top-level item).
    //
    if ( event.GetMainParent() == cpId )
    {
        wxString subName = name;

        // Parse CustomProperty manipulation

        //wxLogDebug(event.GetPropertyValueAsString());

        if ( subName == wxT("Label") )
        {
            // Changing label
            m_pPropGridMan->SetPropertyLabel( cpId, event.GetPropertyValueAsString() );
        }
        else if ( subName == wxT("Image") )
        {
            // Changing OnCustomPaint image
            wxImage image;
            wxString path = event.GetPropertyValueAsString();

            if ( ::wxFileExists(path) )
                image.LoadFile(path);

            if ( image.Ok() )
            {
                wxBitmap bmp(image);
                m_pPropGridMan->SetPropertyImage( cpId, bmp );
            }
            else
            {
                m_pPropGridMan->SetPropertyImage( cpId, wxNullBitmap );
            }
            Refresh();
        }
        else if ( subName == wxT("Editor") )
        {
            // Changing editor class
            int index = event.GetPropertyValueAsInt();

            wxPGEditor* editor = wxPGEditor_TextCtrl;
            switch( index )
            {
                case 0:
                    editor = wxPGEditor_TextCtrl;
                    break;
                case 1:
                    editor = wxPGEditor_Choice;
                    break;
                case 2:
                    editor = wxPGEditor_ComboBox;
                    break;
                case 3:
                    editor = wxPGEditor_TextCtrlAndButton;
                    break;
                case 4:
                    editor = wxPGEditor_ChoiceAndButton;
                    break;
            }

            m_pPropGridMan->SetPropertyEditor ( cpId, editor );

            wxPGId propChoices = m_pPropGridMan->GetPropertyByName(wxT("CustomProperty.Choices"));
            wxASSERT(propChoices.IsOk());

            // Toggle disabled-state of choices property
            if ( index == 1 || index == 2 || index == 4 )
            {
                m_pPropGridMan->EnableProperty(propChoices,true);

                wxPGChoices choices(m_pPropGridMan->
                    GetPropertyValueAsArrayString(propChoices));

                // Also set the current choices now
                m_pPropGridMan->SetPropertyChoices( cpId, choices );
            }
            else
            {
                m_pPropGridMan->EnableProperty(propChoices,false);
            }

        }
        else if ( subName == wxT("Choices") )
        {
            wxPGChoices choices(m_pPropGridMan->
                GetPropertyValueAsArrayString(wxT("CustomProperty.Choices")));

            // Changing list of choices for
            m_pPropGridMan->SetPropertyChoices( cpId, choices );
        }
        else if ( subName == wxT("Paint Mode") )
        {
            // Test either callback or bitmap
            int ind = event.GetPropertyValueAsInt();
            void* callback = NULL;
            if ( ind )
                callback = (void*) TestPaintCallback;

            m_pPropGridMan->SetPropertyAttribute(cpId,
                                                 wxPG_CUSTOM_PAINT_CALLBACK,
                                                 callback);
        }
    }
    else
    // Some settings are disabled outside Windows platform
    if ( name == wxT("X") )
        SetSize ( m_pPropGridMan->GetPropertyValueAsInt(id), -1, -1, -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Y") )
        SetSize ( -1, value.GetLong(), -1, -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Width") )
        SetSize ( -1, -1, event.GetPropertyValueAsInt(), -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Height") )
        SetSize ( -1, -1, -1, value.GetLong(), wxSIZE_USE_EXISTING );
    //else if ( name == wxT("Position") )
    //    SetPosition ( event.GetPropertyValueAsPoint() );
    //else if ( name == wxT("Size") )
    //    SetSize ( event.GetPropertyValueAsSize() );
    else if ( name == wxT("Label") )
    {
        SetTitle ( m_pPropGridMan->GetPropertyValueAsString(id) );
    }
    else if ( name == wxT("Password") )
    {
        static int pwdMode = 0;

        //m_pPropGridMan->SetPropertyAttribute(id, wxPG_STRING_PASSWORD, (long)pwdMode);

        pwdMode++;
        pwdMode &= 1;
    }
    else
    if ( name == wxT("Font") )
    {
        wxFont& font = *wxDynamicCast(event.GetPropertyValueAsWxObjectPtr(),wxFont);

        m_pPropGridMan->SetFont ( font );
    }
    else
    if ( name == wxT("Margin Colour") )
    {
        wxColourPropertyValue& cpv = *wxGetVariantCast(value,wxColourPropertyValue);
        //wxColourPropertyValue& cpv = *((wxColourPropertyValue*)value.GetWxObjectPtr());
        m_pPropGridMan->GetGrid()->SetMarginColour ( cpv.m_colour );

        // Test wxGetVariantCast for normal variant (causes memory leak?)
        //wxVariant black_col(wxBLACK);
        //SetBackgroundColour ( *wxGetVariantCast(black_col,wxColour) );
        //m_pPropGridMan->GetGrid()->SetBackgroundColour ( *wxGetVariantCast(black_col,wxColour) );
    }
    else if ( name == wxT("Cell Colour") )
    {
        wxColourPropertyValue& cpv = *wxGetVariantCast(value,wxColourPropertyValue);
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour ( cpv.m_colour );
    }
    else if ( name == wxT("Line Colour") )
    {
        wxColourPropertyValue& cpv = *wxGetVariantCast(value,wxColourPropertyValue);
        m_pPropGridMan->GetGrid()->SetLineColour ( cpv.m_colour );
    }
    else if ( name == wxT("Cell Text Colour") )
    {
        wxColourPropertyValue& cpv = *wxGetVariantCast(value,wxColourPropertyValue);
        m_pPropGridMan->GetGrid()->SetCellTextColour ( cpv.m_colour );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridSelect ( wxPropertyGridEvent& event )
{
    wxPGId id = event.GetProperty();
    if ( id.IsOk() )
    {
        m_itemEnable->Enable ( TRUE );
        if ( event.IsPropertyEnabled() )
            m_itemEnable->SetText ( wxT("Disable") );
        else
            m_itemEnable->SetText ( wxT("Enable") );
    }
    else
    {
        m_itemEnable->Enable ( FALSE );
    }

#if wxUSE_STATUSBAR && TESTING_WXPROPERTYGRIDADV
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop.IsOk() )
    {
        wxString text(wxT("Selected: "));
        text += m_pPropGridMan->GetPropertyLabel( prop );
        sb->SetStatusText ( text );
    }
#endif

}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
#if wxUSE_STATUSBAR && TESTING_WXPROPERTYGRIDADV
    wxStatusBar* sb = GetStatusBar();
    wxString text(wxT("Page Changed: "));
    text += m_pPropGridMan->GetPageName(m_pPropGridMan->GetSelectedPage());
    sb->SetStatusText( text );
#endif

#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage(m_pPropGridMan->GetSelectedPage());
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridHighlight( wxPropertyGridEvent& /*event*/ )
{
/*#if wxUSE_STATUSBAR
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop.IsOk() )
    {
        wxString text(wxT("Highlighted: "));
        text += m_pPropGridMan->GetPropertyLabel( prop );
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif*/
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemRightClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop.IsOk() )
    {
        wxString text(wxT("Right-clicked: "));
        text += event.GetPropertyLabel();
        text += wxT(", name=");
        text += m_pPropGridMan->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemDoubleClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop.IsOk() )
    {
        wxString text(wxT("Double-clicked: "));
        text += event.GetPropertyLabel();
        text += wxT(", name=");
        text += m_pPropGridMan->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridButtonClick ( wxCommandEvent& )
{
#if wxUSE_STATUSBAR
    wxPGId prop = m_pPropGridMan->GetSelectedProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop.IsOk() )
    {
        wxString text(wxT("Button clicked: "));
        text += m_pPropGridMan->GetPropertyLabel(prop);
        text += wxT(", name=");
        text += m_pPropGridMan->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxT("SHOULD NOT HAPPEN!!!") );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemCollapse( wxPropertyGridEvent& )
{
    wxLogDebug(wxT("Item was Collapsed"));
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemExpand( wxPropertyGridEvent& )
{
    wxLogDebug(wxT("Item was Expanded"));
}

// -----------------------------------------------------------------------

// EVT_TEXT handling
void FormMain::OnPropertyGridTextUpdate( wxCommandEvent& event )
{
    //wxLogDebug(wxT("FormMain::OnPropertyGridTextUpdate(text=\"%s\")"),event.GetString().c_str());
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridKeyEvent ( wxKeyEvent& /*event*/ )
{
    // Occurs on wxGTK mostly, but not wxMSW.

    //wxLogDebug(wxT("FormMain::OnPropertyGridKeyEvent()"));
/*#if wxUSE_STATUSBAR
    wxPGId prop = m_pPropGridMan->GetSelectedProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop.IsOk() )
    {
        sb->SetStatusText ( wxString::Format(wxT("Key event: %i"),(int)event.GetKeyCode()) );
    }
    else
    {
        sb->SetStatusText ( wxT("SHOULD NOT HAPPEN!!!") );
    }
#endif*/
}

// -----------------------------------------------------------------------

void FormMain::OnLabelTextChange ( wxCommandEvent& WXUNUSED(event) )
{
// Uncomment following to allow property label modify in real-time
//    wxPGProperty& p = m_pPropGridMan->GetGrid()->GetSelection();
//    if ( !p.IsOk() ) return;
//    m_pPropGridMan->SetPropertyLabel ( p, m_tcPropLabel->DoGetValue() );
}

// -----------------------------------------------------------------------

//
// Normally, wxPropertyGrid does not check whether item with identical
// label already exists. However, since in this sample we use labels for
// identifying properties, we have to be sure not to generate identical
// labels.
//
#if TESTING_WXPROPERTYGRIDADV
void GenerateUniquePropertyLabel ( wxPropertyGridManager* pg, wxString& baselabel )
#else
void GenerateUniquePropertyLabel ( wxPropertyGrid* pg, wxString& baselabel )
#endif
{
    int count = -1;
    wxString newlabel;

    if ( pg->GetPropertyByLabel ( baselabel ).IsOk() )
    {
        for (;;)
        {
            count++;
            newlabel.Printf(wxT("%s%i"),baselabel.c_str(),count);
            if ( !pg->GetPropertyByLabel ( newlabel ).IsOk() ) break;
        }
    }

    if ( count >= 0 )
    {
        baselabel = newlabel;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertPropClick ( wxCommandEvent& WXUNUSED(event) )
{
    wxString prop_label;

    // Make sure we are on the current page.
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage ( m_pPropGridMan->GetSelectedPage() );
#endif

    if ( !m_pPropGridMan->GetChildrenCount() )
    {
        wxMessageBox(wxT("No items to relate - first add some with Append."));
        return;
    }

    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !id.IsOk() )
    {
        wxMessageBox(wxT("First select a property - new one will be inserted right before that."));
        return;
    }
    if ( prop_label.Len() < 1 ) prop_label = wxT("Property");

    GenerateUniquePropertyLabel ( m_pPropGridMan, prop_label );

    m_pPropGridMan->Insert ( m_pPropGridMan->GetPropertyParent(id),
                             m_pPropGridMan->GetPropertyIndex(id),
                             wxStringProperty(prop_label) );

}

// -----------------------------------------------------------------------

void FormMain::OnAppendPropClick ( wxCommandEvent& WXUNUSED(event) )
{
    wxString prop_label;

    // Make sure we are on the current page.
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage ( m_pPropGridMan->GetSelectedPage() );
#endif

    //prop_label = m_tcPropLabel->GetValue();
    if ( prop_label.Len() < 1 ) prop_label = wxT("Property");

    GenerateUniquePropertyLabel ( m_pPropGridMan, prop_label );

    m_pPropGridMan->Append ( wxStringProperty(prop_label) );

    m_pPropGridMan->Refresh();
}

// -----------------------------------------------------------------------

void FormMain::OnClearClick ( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridMan->GetGrid()->Clear();
}

// -----------------------------------------------------------------------

void FormMain::OnAppendCatClick ( wxCommandEvent& WXUNUSED(event) )
{
    wxString prop_label;

    // Make sure we are on the current page.
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage ( m_pPropGridMan->GetSelectedPage() );
#endif

    if ( prop_label.Len() < 1 ) prop_label = wxT("Category");

    GenerateUniquePropertyLabel ( m_pPropGridMan, prop_label );

    m_pPropGridMan->Append ( wxPropertyCategory (prop_label) );

    m_pPropGridMan->Refresh();

}

// -----------------------------------------------------------------------

void FormMain::OnInsertCatClick ( wxCommandEvent& WXUNUSED(event) )
{
    wxString prop_label;

    // Make sure we are on the current page.
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage ( m_pPropGridMan->GetSelectedPage() );
#endif

    if ( !m_pPropGridMan->GetChildrenCount() )
    {
        wxMessageBox(wxT("No items to relate - first add some with Append."));
        return;
    }

    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !id.IsOk() )
    {
        wxMessageBox(wxT("First select a property - new one will be inserted right before that."));
        return;
    }

    if ( prop_label.Len() < 1 ) prop_label = wxT("Category");

    GenerateUniquePropertyLabel ( m_pPropGridMan, prop_label );

    m_pPropGridMan->Insert ( m_pPropGridMan->GetPropertyParent(id),
                             m_pPropGridMan->GetPropertyIndex(id),
                             wxPropertyCategory (prop_label) );

}

// -----------------------------------------------------------------------

void FormMain::OnDelPropClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !id.IsOk() )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }
    /*
    else if ( m_propGrid->IsSubProperty(id) )
    {
        wxMessageBox(wxT("Do not try to delete sub-properties."));
        return;
    }
    */

    m_pPropGridMan->Delete( id );
}

// -----------------------------------------------------------------------

void FormMain::OnDelPropRClick( wxCommandEvent& WXUNUSED(event) )
{
    // Delete random property
    wxPGPropertyWithChildren* p = (wxPGPropertyWithChildren*) wxPGIdToPtr(m_pPropGridMan->GetGrid()->GetRoot());

    for (;;)
    {
        if ( p->GetParentingType() <= 0 )
        {
            m_pPropGridMan->Delete( wxPGIdGen(p) );
            break;
        }

        if ( !p->GetChildCount() )
            break;

        int n = rand() % ((int)p->GetChildCount());
        //wxMessageBox(wxString::Format(wxT("%i"),n));

        //p = (wxPGPropertyWithChildren*) p->Item(n);
        p = (wxPGPropertyWithChildren*) p->Item(n);
    }
}

// -----------------------------------------------------------------------

void FormMain::OnContextMenu( wxContextMenuEvent& event )
{
    wxLogDebug(wxT("FormMain::OnContextMenu(%i,%i)"),
        event.GetPosition().x,event.GetPosition().y);

    //event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnCloseClick( wxCommandEvent& WXUNUSED(event) )
{
/*#ifdef __WXDEBUG__
    m_pPropGridMan->GetGrid()->DumpAllocatedChoiceSets();
    wxLogDebug(wxT("\\-> Don't worry, this is perfectly normal in this sample."));
#endif*/

    Close(false);
}

// -----------------------------------------------------------------------

int IterateMessage( wxPropertyGrid* pg, wxPGId id )
{
    wxString s;

    s.Printf( wxT("\"%s\" class = %s, valuetype = %s"), pg->GetPropertyLabel(id).c_str(),
        pg->GetPropertyClassName(id), pg->GetPropertyValueType(id)->GetTypeName() );

    return wxMessageBox( s, wxT("Iterating... (press CANCEL to end)"), wxOK|wxCANCEL );
}

// -----------------------------------------------------------------------

void FormMain::OnIterate1Click( wxCommandEvent& WXUNUSED(event) )
{
    wxString s;

#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage( m_pPropGridMan->GetSelectedPage() );
#endif

    // Iterate over all properties.
    wxPGId id = m_pPropGridMan->GetFirstProperty();

    while ( id.IsOk() )
    {
        int res = IterateMessage( m_pPropGridMan->GetGrid(), id );
        if ( res == wxCANCEL ) break;

        id = m_pPropGridMan->GetNextProperty( id );

    }

}

// -----------------------------------------------------------------------

void FormMain::OnIterate2Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();
    wxString s;

    // Iterate over all visible items.
    wxPGId id = pg->GetFirstVisible();

    while ( id.IsOk() )
    {
        int res = IterateMessage( pg, id );
        if ( res == wxCANCEL ) break;

        // Get next
        id = pg->GetNextVisible( id );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate3Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_propGrid;
    wxString s;

#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage( m_pPropGridMan->GetSelectedPage() );
#endif

    // iterate over items in reverse order
    wxPGId id = m_pPropGridMan->GetGrid()->GetLastProperty();

    while ( id.IsOk() )
    {
        int res = IterateMessage ( pg, id );
        if ( res == wxCANCEL ) break;

        // Get previous
        id = m_pPropGridMan->GetPrevProperty( id );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate4Click( wxCommandEvent& WXUNUSED(event) )
{
    //wxPropertyGrid* pg = m_propGrid;
    wxString s;

#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage( m_pPropGridMan->GetSelectedPage() );
#endif

    // iterate over all visible items
    wxPGId id = m_pPropGridMan->GetFirstCategory();

    while ( id.IsOk() )
    {
        int res = IterateMessage( m_pPropGridMan->GetGrid(), id );
        if ( res == wxCANCEL ) break;

        // Get next.
        id = m_pPropGridMan->GetNextCategory( id );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnChangeFlagsPropItemsClick( wxCommandEvent& WXUNUSED(event) )
{

    wxPGId id = m_pPropGridMan->GetPropertyByName(wxT("Window Styles"));

    wxPGChoices newChoices;

    newChoices.Add(wxT("Fast"),0x1);
    newChoices.Add(wxT("Powerful"),0x2);
    newChoices.Add(wxT("Safe"),0x4);
    newChoices.Add(wxT("Sleek"),0x8);

    m_pPropGridMan->SetPropertyChoices(id,newChoices);
    //m_pPropGridMan->ReplaceProperty(wxT("Window Styles"),
    //    wxFlagsProperty(wxT("Window Styles"),wxPG_LABEL,newChoices));
}

// -----------------------------------------------------------------------

void FormMain::OnEnableDisable( wxCommandEvent& )
{
    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !id.IsOk() )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }
    /*
    else if ( m_propGrid->IsSubProperty(id) )
    {
        wxMessageBox(wxT("Do not try to enable/disable sub-properties."));
        return;
    }
    */

    if ( m_pPropGridMan->IsPropertyEnabled( id ) )
    {
        m_pPropGridMan->DisableProperty ( id );
        m_itemEnable->SetText( wxT("Enable") );
    }
    else
    {
        m_pPropGridMan->EnableProperty ( id );
        m_itemEnable->SetText( wxT("Disable") );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnHideShow( wxCommandEvent& WXUNUSED(event) )
{
    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !id.IsOk() )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }
    /*
    else if ( m_propGrid->IsSubProperty(id) )
    {
        wxMessageBox(wxT("Do not try to enable/disable sub-properties."));
        return;
    }
    */

    if ( m_pPropGridMan->IsPropertyShown( id ) )
    {
        m_pPropGridMan->HideProperty( id, true );
        m_itemEnable->SetText( wxT("Show") );
    }
    else
    {
        m_pPropGridMan->HideProperty( id, false );
        m_itemEnable->SetText( wxT("Hide") );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnSetAsHideable( wxCommandEvent& WXUNUSED(event) )
{
    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !id.IsOk() )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }

    m_pPropGridMan->TogglePropertyPriority( id );
}

// -----------------------------------------------------------------------

void FormMain::OnInsertPage( wxCommandEvent& WXUNUSED(event) )
{
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->AddPage(wxT("New Page"));
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnRemovePage( wxCommandEvent& WXUNUSED(event) )
{
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->RemovePage(m_pPropGridMan->GetSelectedPage());
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnSetSpinCtrlEditorClick( wxCommandEvent& WXUNUSED(event) )
{
#if wxUSE_SPINBTN
    wxPGId pgId = m_pPropGridMan->GetSelectedProperty();
    if ( pgId.IsOk() )
        m_pPropGridMan->SetPropertyEditor( pgId, wxPG_EDITOR(SpinCtrl) );
    else
        wxMessageBox(wxT("First select a property"));
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnTestReplaceClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPGId pgId = m_pPropGridMan->GetSelectedProperty();
    if ( pgId.IsOk() )
    {
        wxPGChoices choices;
        choices.Add(wxT("Flag 0"),0x0001);
        choices.Add(wxT("Flag 1"),0x0002);
        choices.Add(wxT("Flag 2"),0x0004);
        choices.Add(wxT("Flag 3"),0x0008);
        wxPGId newId = m_pPropGridMan->ReplaceProperty( pgId,
            wxFlagsProperty(wxT("ReplaceFlagsProperty"), wxPG_LABEL, choices, 0x0003) );
        m_pPropGridMan->SetPropertyAttribute( newId,
                                              wxPG_BOOL_USE_CHECKBOX,
                                             (long)1,
                                             wxPG_RECURSE );
    }
    else
        wxMessageBox(wxT("First select a property"));
}

// -----------------------------------------------------------------------

void FormMain::OnClearModifyStatusClick( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridMan->ClearModifiedStatus();
}

// -----------------------------------------------------------------------

// Freeze check-box checked?
void FormMain::OnFreezeClick( wxCommandEvent& event )
{
    if ( !m_pPropGridMan ) return;

    if ( event.IsChecked() )
    {
        if ( !m_pPropGridMan->IsFrozen() )
        {
            m_pPropGridMan->Freeze();
        }
    }
    else
    {
        if ( m_pPropGridMan->IsFrozen() )
        {
            m_pPropGridMan->Thaw();
            m_pPropGridMan->Refresh();
        }
    }
}

// -----------------------------------------------------------------------

void FormMain::OnDumpList( wxCommandEvent& WXUNUSED(event) )
{
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->SetTargetPage(m_pPropGridMan->GetSelectedPage());
#endif

    wxVariant values = m_pPropGridMan->GetPropertyValues();
    wxString text = wxT("This only tests that wxVariant related routines do not crash.");
    wxString t;

    wxDialog* dlg = new wxDialog(this,-1,wxT("wxVariant Test"),
        wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    wxODCCount i;  // wxODCCount used since in wxWidgets 2.7.0 some counts migrated to from int to size_t
    for ( i = 0; i < (wxODCCount)values.GetCount(); i++ )
    {
        wxVariant& v = values[i];

        wxString strValue = v.GetString();

        t.Printf(wxT("%i: name=\"%s\"  type=\"%s\"  value=\"%s\"\n"),(int)i,
            v.GetName().c_str(),v.GetType().c_str(),strValue.c_str());

        text += t;
    }

    // multi-line text editor dialog
    const int spacing = 8;
    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    wxTextCtrl* ed = new wxTextCtrl(dlg,11,text,
        wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);
    rowsizer->Add( ed, 1, wxEXPAND|wxALL, spacing );
    topsizer->Add( rowsizer, 1, wxEXPAND, 0 );
    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    const int butSzFlags =
        wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT;
    rowsizer->Add( new wxButton(dlg,wxID_OK,wxT("Ok")),
        0, butSzFlags, spacing );
    topsizer->Add( rowsizer, 0, wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL, 0 );

    dlg->SetSizer( topsizer );
    topsizer->SetSizeHints( dlg );

    dlg->SetSize(400,300);
    dlg->Centre();
    dlg->ShowModal();
}

// -----------------------------------------------------------------------

void FormMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("wxPropertyGrid %i.%i.%i Sample")
#if wxUSE_UNICODE
  #if defined(wxUSE_UNICODE_UTF8) && wxUSE_UNICODE_UTF8
                wxT(" <utf-8>")
  #else
                wxT(" <unicode>")
  #endif
#else
                wxT(" <ansi>")
#endif
#ifdef __WXDEBUG__
                wxT(" <debug>")
#else
                wxT(" <release>")
#endif
                wxT("\n\n")
                wxT("Programmed by %s ( %s ).\n\n")
                wxT("Powered by %s.\n\n"),
            wxPROPGRID_MAJOR, wxPROPGRID_MINOR, wxPROPGRID_RELEASE,
            wxT("Jaakko Salli"), wxT("jmsalli79@hotmail.com, jaakko.salli@pp.inet.fi"), wxVERSION_STRING
            );

    wxMessageBox(msg, _T("About"), wxOK | wxICON_INFORMATION, this);
}

// -----------------------------------------------------------------------

#ifdef wxPGS_USE_TABCTRL
void FormMain::OnTabChange( wxTabEvent& WXUNUSED(event) )
{
    int sel = m_tabctrl->GetSelection();
#else
void FormMain::OnRadioBoxChange( wxCommandEvent& WXUNUSED(event) )
{
    int sel = m_radiobox->GetSelection();
#endif

    if ( !m_propGrid )
        return;

    if ( sel == 0 )
        m_propGrid->EnableCategories( TRUE );
    else
        m_propGrid->EnableCategories( FALSE );

    if ( !m_propGrid->IsFrozen() )
        m_propGrid->Refresh();
}

// -----------------------------------------------------------------------

void FormMain::OnColourScheme( wxCommandEvent& event )
{
    //m_pPropGridMan->SetPropertyValue(wxT("ColourProperty"),*wxBLACK);

    int id = event.GetId();
    if ( id == ID_COLOURSCHEME1 )
    {
        m_pPropGridMan->GetGrid()->ResetColours();
    }
    else if ( id == ID_COLOURSCHEME2 )
    {
        // white
        wxColour my_grey_1(212,208,200);
        wxColour my_grey_3(113,111,100);
        m_pPropGridMan->Freeze();
        m_pPropGridMan->GetGrid()->SetMarginColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCaptionBackgroundColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCellTextColour( my_grey_3 );
        m_pPropGridMan->GetGrid()->SetLineColour( my_grey_1 ); //wxColour(160,160,160)
        m_pPropGridMan->Thaw();
    }
    else if ( id == ID_COLOURSCHEME3 )
    {
        // .NET
        wxColour my_grey_1(212,208,200);
        wxColour my_grey_2(236,233,216);
        m_pPropGridMan->Freeze();
        m_pPropGridMan->GetGrid()->SetMarginColour( my_grey_1 );
        m_pPropGridMan->GetGrid()->SetCaptionBackgroundColour( my_grey_1 );
        m_pPropGridMan->GetGrid()->SetLineColour( my_grey_1 );
        m_pPropGridMan->Thaw();
    }
    else if ( id == ID_COLOURSCHEME4 )
    {
        // cream

        wxColour my_grey_1(212,208,200);
        wxColour my_grey_2(241,239,226);
        wxColour my_grey_3(113,111,100);
        m_pPropGridMan->Freeze();
        m_pPropGridMan->GetGrid()->SetMarginColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCaptionBackgroundColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour( my_grey_2 );
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour( my_grey_2 );
        m_pPropGridMan->GetGrid()->SetCellTextColour( my_grey_3 );
        m_pPropGridMan->GetGrid()->SetLineColour( my_grey_1 );
        m_pPropGridMan->Thaw();
    }
}

// -----------------------------------------------------------------------

void FormMain::OnCatColours( wxCommandEvent& event )
{
    m_pPropGridMan->Freeze();
    if ( event.IsChecked() )
    {
        // Set custom colours.
        m_pPropGridMan->SetCaptionTextColour( wxT("Appearance"), wxColour(255,0,0) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("Appearance"), wxColour(255,255,183) );
        m_pPropGridMan->SetPropertyTextColour( wxT("Appearance"), wxColour(255,0,183) );
        m_pPropGridMan->SetCaptionTextColour( wxT("PositionCategory"), wxColour(0,255,0) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("PositionCategory"), wxColour(255,226,190) );
        m_pPropGridMan->SetPropertyTextColour( wxT("PositionCategory"), wxColour(255,0,190) );
        m_pPropGridMan->SetCaptionTextColour( wxT("Environment"), wxColour(0,0,255) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("Environment"), wxColour(208,240,175) );
        m_pPropGridMan->SetPropertyTextColour( wxT("Environment"), wxColour(255,255,255) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("More Examples"), wxColour(172,237,255) );
        m_pPropGridMan->SetPropertyTextColour( wxT("More Examples"), wxColour(172,0,255) );
    }
    else
    {
        // Revert to original.
        m_pPropGridMan->SetPropertyColourToDefault( wxT("Appearance") );
        m_pPropGridMan->SetPropertyColourToDefault( wxT("PositionCategory") );
        m_pPropGridMan->SetPropertyColourToDefault( wxT("Environment") );
        m_pPropGridMan->SetPropertyColourToDefault( wxT("More Examples") );
    }
    m_pPropGridMan->Thaw();
    m_pPropGridMan->Refresh();
}

// -----------------------------------------------------------------------

void FormMain::OnCompact( wxCommandEvent& event )
{
    if ( event.IsChecked() )
    {
        // Compact
        m_pPropGridMan->Compact( TRUE );
    }
    else
    {
        // Expand
        m_pPropGridMan->Compact( FALSE );
    }
}

// -----------------------------------------------------------------------

#include <wx/textfile.h>

static void WritePropertiesToFile( wxPropertyGrid* pg, wxPGId id, wxTextFile& f, int depth, bool useClassName )
{
    wxString s;
    wxString s2;
    wxString s_value;
    wxString s_attribs;

    while ( id.IsOk() )
    {

        // Write property info ( as classname,label,value)
        // Note how we omit the name as it is convenient
        // to assume that it matches the label.
        s_value = pg->GetPropertyValueAsString(id);

        // Since we surround tokens by '"'s, we need to
        // replace '"' with '\"' and '\'s with '\\'
        s_value.Replace(wxT("\\"),wxT("\\\\"));
        s_value.Replace(wxT("\""),wxT("\\\""));

        wxString classname;

        // If we categorize using class name, then just get
        // property's short class name.
        if ( useClassName )
            classname = pg->GetPropertyShortClassName(id);
        else
        // If we categorize using value types, use string
        // "category" for categories and real value type
        // name for others.
            if ( pg->GetFirstChild(id).IsOk() )
                classname = wxT("category");
            else
                classname = pg->GetPropertyValueType(id)->GetType();


        s.Printf(wxT("%*s\"%s\" \"%s\" \"%s\""),
            depth*2,
            wxT(""),
            classname.c_str(),
            pg->GetPropertyLabel(id).c_str(),
            s_value.c_str());

        // Write attributes, if any
        s_attribs = pg->GetPropertyAttributes(id);
        if ( s_attribs.length() )
        {
            s.Append(wxT(" \""));
            s.Append(s_attribs);
            s.Append(wxT("\""));
        }

        // Append choices, if any
        wxPGChoices& choices = pg->GetPropertyChoices(id);
        if ( choices.IsOk() )
        {
            // If no attribs already, add empty token as a substitute
            // (must be because we add tokens after it)
            if ( !s_attribs.length() )
                s.Append(wxT(" \"\""));

            // First add id of the choices list inorder to optimize
            s2.Printf(wxT(" \"%X\""),(unsigned int)choices.GetId());
            s.Append(s2);

            size_t i;
            wxArrayString& labels = choices.GetLabels();
            wxArrayInt& values = choices.GetValues();
            if ( values.GetCount() )
                for ( i=0; i<labels.GetCount(); i++ )
                {
                    s2.Printf(wxT(" \"%s||%i\""),labels[i].c_str(),values[i]);
                    s.Append(s2);
                }
            else
                for ( i=0; i<labels.GetCount(); i++ )
                {
                    s2.Printf(wxT(" \"%s\""),labels[i].c_str());
                    s.Append(s2);
                }
        }

        f.AddLine(s);

        // Write children, if any
        wxPGId first_child = pg->GetFirstChild(id);
        if ( first_child.IsOk() )
        {
            WritePropertiesToFile( pg, first_child, f, depth+1, useClassName );

            // Add parent's terminator
            s.Printf(wxT("%*s\"%s\" \"Ends\""),
                depth*2,
                wxT(""),
                classname.c_str());
            f.AddLine(s);
        }

        id = pg->GetNextSibling(id);
    }
}

// -----------------------------------------------------------------------

void FormMain::OnSaveToFileClick ( wxCommandEvent& event )
{
    int id = event.GetId();

    wxFileDialog dlg(this,
                     wxT("Choose File to Save"),
                     wxEmptyString,
                     wxEmptyString,
                     wxT("Text files (*.txt)|*.txt|All files (*.*)|*.*"),
#if wxCHECK_VERSION(2,8,0)
                     wxFD_SAVE|wxFD_OVERWRITE_PROMPT
#else
                     wxSAVE|wxOVERWRITE_PROMPT
#endif
                    );

    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    if ( dlg.ShowModal() == wxID_OK )
    {
        wxTextFile f(dlg.GetPath());

        // Determine whether we want to categorize properties by
        // their class names or value types.
        if ( id==ID_SAVETOFILE )
            f.AddLine(wxT("\"wxPropertyGrid State ByClass\""));
        else
            f.AddLine(wxT("\"wxPropertyGrid State ByValueType\""));

        // Iterate through properties
        wxPGId first_child = pg->GetFirstChild(pg->GetRoot());
        if ( first_child.IsOk() )
            WritePropertiesToFile(pg,first_child,f,0,id==ID_SAVETOFILE);

        f.Write();
    }
}

// -----------------------------------------------------------------------

void FormMain::OnLoadFromFileClick ( wxCommandEvent& )
{
    wxFileDialog dlg(this,
                     wxT("Choose File to Load"),
                     wxEmptyString,
                     wxEmptyString,
                     wxT("Text files (*.txt)|*.txt|All files (*.*)|*.*"),
#if wxCHECK_VERSION(2,8,0)
                     wxFD_OPEN
#else
                     wxOPEN
#endif
                     );

    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    if ( dlg.ShowModal() == wxID_OK )
    {
        pg->Freeze();
        pg->Clear();

        wxTextFile f(dlg.GetPath());

        f.Open();

        wxPropertyGridPopulator populator(pg);

        unsigned int linenum = 0;

        wxString s_class;
        wxString s_name;
        wxString s_value;
        wxString s_attr;
        wxPGChoicesId choices_id;

        bool useClassName = true;

        wxArrayString choice_labels;
        wxArrayInt choice_values;

        // Identify property categorization (i.e. class name or value type)
        if ( linenum < f.GetLineCount() )
        {
            wxString& s = f.GetLine(linenum);
            linenum++;
            if ( s.SubString(1,14) != wxT("wxPropertyGrid") )
            {
                wxLogError(wxT("File is not of valid type."));
                linenum = f.GetLineCount();
            }
            else
            if ( s.Find(wxT("ByValueType")) != wxNOT_FOUND )
                useClassName = false;
        }

        while ( linenum < f.GetLineCount() )
        {
            wxString& s = f.GetLine(linenum);
            linenum++;

            //wxLogDebug(wxT("%s"),s.c_str());

            int step = 0;
            choices_id = 0;
            int choices_mode = 0; // 1 = no values, 2 = with values
            size_t end = s.length();

            choice_labels.Empty();
            choice_values.Empty();
            s_attr.Empty();

            if ( s.length() && s.GetChar(0) != wxT('#') )
            {

                // Parse tokens
                // NOTE: This code *requires* inclusion of propdev.h
                //   (due to tokenizer)
                WX_PG_TOKENIZER2_BEGIN(s,wxT('"'))
                    if ( step == 0 )
                        s_class = token;
                    else if ( step == 1 )
                        s_name = token;
                    else if ( step == 2 )
                        s_value = token;
                    else if ( step == 3 )
                        s_attr = token;
                    else if ( step == 4 )
                    {
                        unsigned long a;
                        if ( token.ToULong(&a,16) )
                        {
                            choices_id = (wxPGChoicesId)a;
                            if ( populator.HasChoices(choices_id) )
                                break;
                        }
                        else
                            wxLogError(wxT("Line %i: Fourth token, if any, must be hexadecimal identifier (it was \"%s\")"),linenum,token.c_str());
                    }
                    else
                    {
                        // Choices
                        if ( !choices_mode )
                        {
                            choices_mode = 1;
                            if ( s.find(wxT("||")) < end )
                                choices_mode = 2;
                        }

                        if ( choices_mode == 1 )
                        {
                            choice_labels.Add(token);
                        }
                        else
                        {
                            size_t delim_pos = token.rfind(wxT("||"));
                            if ( delim_pos < end )
                            {
                                choice_labels.Add(token.substr(0,delim_pos));
                                long int_val = 0;
                                if ( token.length() > (size_t)(delim_pos+2) )
                                {
                                    wxString int_str = token.substr(delim_pos+2);
                                    int_str.ToLong(&int_val,10);
                                }

                                choice_values.Add(int_val);

                            }
                            else
                            {
                                choice_labels.Add(token);
                                choice_values.Add(0);
                            }
                        }

                        //wxLogDebug(wxT("%s=%i"),choice_labels[choice_labels.GetCount()-1].c_str(),
                        //    choice_values.GetCount()?choice_values[choice_values.GetCount()-1]:0);

                    }

                    step++;
                WX_PG_TOKENIZER2_END()

                if ( step )
                {

                    wxLogDebug(wxT("%s: %s %s"),s_class.c_str(),s_name.c_str(),s_value.c_str());

                    // First check for group terminator
                    if ( step == 2 &&
                         s_name == wxT("Ends") )
                    {
                        populator.EndChildren();
                    }
                    else
                    // There is no value type for category, so we need to
                    // check it separately.
                    if ( !useClassName && s_class == wxT("category") )
                    {
                        populator.AppendByClass(wxT("Category"),
                                                s_name,
                                                wxPG_LABEL);
                        populator.BeginChildren();
                    }
                    else if ( step >= 3 )
                    {
                        // Prepare choices, if any
                        if ( choices_id && !populator.HasChoices(choices_id) )
                        {
                            populator.AddChoices(choices_id,choice_labels,choice_values);
                        }

                        // New property
                        if ( useClassName )
                            populator.AppendByClass(s_class,
                                                    s_name,
                                                    wxPG_LABEL,
                                                    s_value,
                                                    s_attr,
                                                    choices_id);
                        else
                            populator.AppendByType(s_class,
                                                   s_name,
                                                   wxPG_LABEL,
                                                   s_value,
                                                   s_attr,
                                                   choices_id);

                        // Automatically start adding children
                        // This returns false if that really was not possible
                        // (i.e. we appended something else than category)
                        populator.BeginChildren();

                    }
                    else
                    {
                        wxLogError(wxT("Line %i: Only %i tokens (minimum of 3 required for this type)"),linenum,step);
                    }
                }
            }
        }

        pg->Thaw();
    }
}

// -----------------------------------------------------------------------

void FormMain::OnSetPropertyValue( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();
    wxPGId selected = pg->GetSelection();

    if ( selected.IsOk() )
    {
        wxString value = ::wxGetTextFromUser( wxT("Enter new value:") );
        pg->SetPropertyValue( selected, value );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertChoice( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    wxPGId selected = pg->GetSelection();
    wxPGChoices& choices = pg->GetPropertyChoices(selected);

    // Insert new choice to the center of list

    if ( choices.IsOk() )
    {
        int pos = choices.GetCount() / 2;
        pg->InsertPropertyChoice(selected,wxT("New Choice"),pos);
    }
    else
    {
        ::wxMessageBox(wxT("First select a property with some choices."));
    }
}

// -----------------------------------------------------------------------

void FormMain::OnDeleteChoice( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    wxPGId selected = pg->GetSelection();
    wxPGChoices& choices = pg->GetPropertyChoices(selected);

    // Deletes choice from the center of list

    if ( choices.IsOk() )
    {
        int pos = choices.GetCount() / 2;
        pg->DeletePropertyChoice(selected,pos);
    }
    else
    {
        ::wxMessageBox(wxT("First select a property with some choices."));
    }
}

// -----------------------------------------------------------------------

#include <wx/colordlg.h>

void FormMain::OnMisc ( wxCommandEvent& event )
{
    int id = event.GetId();
    if ( id == ID_STATICLAYOUT )
    {
        long wsf = m_pPropGridMan->GetWindowStyleFlag();
        if ( event.IsChecked() ) m_pPropGridMan->SetWindowStyleFlag( wsf|wxPG_STATIC_LAYOUT );
        else m_pPropGridMan->SetWindowStyleFlag( wsf&~(wxPG_STATIC_LAYOUT) );
    }
    else if ( id == ID_CLEAR )
    {
        m_pPropGridMan->ClearPropertyValue(m_pPropGridMan->GetGrid()->GetSelection());
    }
    else if ( id == ID_COLLAPSEALL )
    {
        m_pPropGridMan->CollapseAll();
    }
    else if ( id == ID_GETVALUES )
    {
#if TESTING_WXPROPERTYGRIDADV
        m_pPropGridMan->SetTargetPage( m_pPropGridMan->GetSelectedPage() );
#endif
        //m_storedValues = m_pPropGridMan->GetPropertyValues(wxT("wxPropertyGridValues"));
        m_storedValues = m_pPropGridMan->GetPropertyValues(wxT("Test"),m_pPropGridMan->GetRoot(),wxPG_KEEP_STRUCTURE);
    }
    else if ( id == ID_SETVALUES )
    {
#if TESTING_WXPROPERTYGRIDADV
        m_pPropGridMan->SetTargetPage( m_pPropGridMan->GetSelectedPage() );
#endif
        if ( m_storedValues.GetType() == wxT("list") )
        {
            m_pPropGridMan->SetPropertyValues(m_storedValues);
        }
        else
            wxMessageBox(wxT("First use Get Property Values."));
    }
    else if ( id == ID_SETVALUES2 )
    {
#if TESTING_WXPROPERTYGRIDADV
        m_pPropGridMan->SetTargetPage( m_pPropGridMan->GetSelectedPage() );
#endif
        wxVariant list;
        list.NullList();
        list.Append( wxVariant((long)1234,wxT("VariantLong")) );
        list.Append( wxVariant((bool)TRUE,wxT("VariantBool")) );
        list.Append( wxVariant(wxT("Test Text"),wxT("VariantString")) );
        m_pPropGridMan->SetPropertyValues(list);
    }
    else if ( id == ID_COLLAPSE )
    {
        // Collapses selected.
        wxPGId id = m_pPropGridMan->GetSelectedProperty();
        if ( id.IsOk() )
        {
            m_pPropGridMan->Collapse(id);
        }
    }
    else if ( id == ID_RUNTEST )
    {
        // Runs a regression test.
        RunTests();
    }
    else if ( id == ID_UNSPECIFY )
    {
        wxPGId prop = m_pPropGridMan->GetSelectedProperty();
        if ( prop.IsOk() )
        {
            m_pPropGridMan->SetPropertyUnspecified(prop);
        }
    }
    else if ( id == ID_SETCOLOUR )
    {
        wxPGId prop = m_pPropGridMan->GetSelectedProperty();
        if ( prop.IsOk() )
        {
            wxColourData data;
            data.SetChooseFull(true);
            int i;
            for ( i = 0; i < 16; i++)
            {
                wxColour colour(i*16, i*16, i*16);
                data.SetCustomColour(i, colour);
            }

            wxColourDialog dialog(this, &data);
            if ( dialog.ShowModal() == wxID_OK )
            {
                wxColourData retData = dialog.GetColourData();
                m_pPropGridMan->SetPropertyColour(prop,retData.GetColour());
            }
        }
    }
}

// -----------------------------------------------------------------------

#define RT_START_TEST(TESTNAME) \
    { \
        ed->AppendText(wxT(#TESTNAME)); \
        ed->AppendText(wxT("\n\n")); \
        wxLogDebug(wxT(#TESTNAME)); \
    }

#define RT_FAILURE() \
    { \
        wxLogDebug(wxT("Test failure in %s, line %i."),wxT(__FILE__),__LINE__-1); \
        failures++; \
    }


void FormMain::RunTests ()
{
#if TESTING_WXPROPERTYGRIDADV
    wxString t;

    wxPropertyGridManager* pgman = m_pPropGridMan;
    wxPropertyGrid* pg;

    wxPGId id;

    size_t i;

    pgman->ClearSelection();

    int failures = 0;

    wxDialog* dlg = new wxDialog (this,-1,wxT("wxVariant Test"),
        wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    // multi-line text editor dialog
    const int spacing = 8;
    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    wxTextCtrl* ed = new wxTextCtrl(dlg,11,wxEmptyString,
        wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);
    rowsizer->Add ( ed, 1, wxEXPAND|wxALL, spacing );
    topsizer->Add ( rowsizer, 1, wxEXPAND, 0 );
    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    const int butSzFlags =
        wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT;
    rowsizer->Add ( new wxButton(dlg,wxID_OK,wxT("Ok")),
        0, butSzFlags, spacing );
    topsizer->Add ( rowsizer, 0, wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL, 0 );

    dlg->SetSizer ( topsizer );
    topsizer->SetSizeHints( dlg );

    dlg->SetSize (400,300);
    dlg->Centre();
    dlg->Show();

    {
        RT_START_TEST(GetPropertyValues)

        for ( i=0; i<3; i++ )
        {
            wxString text;

            pgman->SetTargetPage(i);

            wxVariant values = pgman->GetPropertyValues();

            wxODCCount i;  // wxODCCount used since in wxWidgets 2.7.0 some counts migrated to from int to size_t
            for ( i = 0; i < (wxODCCount)values.GetCount(); i++ )
            {
                wxVariant& v = values[i];

                t.Printf(wxT("%i: name=\"%s\"  type=\"%s\"\n"),(int)i,
                    v.GetName().c_str(),v.GetType().c_str());

                text += t;
            }
            ed->AppendText(text);
        }
    }

    {
        RT_START_TEST(SetPropertyValue_and_GetPropertyValue)

        //pg = (wxPropertyGrid*) NULL;

        wxArrayString test_arrstr_1;
        test_arrstr_1.Add(wxT("Apple"));
        test_arrstr_1.Add(wxT("Orange"));
        test_arrstr_1.Add(wxT("Lemon"));

        wxArrayString test_arrstr_2;
        test_arrstr_2.Add(wxT("Potato"));
        test_arrstr_2.Add(wxT("Cabbage"));
        test_arrstr_2.Add(wxT("Cucumber"));

        wxArrayInt test_arrint_1;
        test_arrint_1.Add(1);
        test_arrint_1.Add(2);
        test_arrint_1.Add(3);

        wxArrayInt test_arrint_2;
        test_arrint_2.Add(0);
        test_arrint_2.Add(1);
        test_arrint_2.Add(4);

#if wxUSE_DATETIME
        wxDateTime dt1 = wxDateTime::Now();
        dt1.SetYear(dt1.GetYear()-1);

        wxDateTime dt2 = wxDateTime::Now();
        dt2.SetYear(dt2.GetYear()-10);
#endif

#define FLAG_TEST_SET1 (wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxRESIZE_BORDER)
#define FLAG_TEST_SET2 (wxSTAY_ON_TOP|wxCAPTION|wxICONIZE|wxSYSTEM_MENU)

        pgman->SetPropertyValue(wxT("StringProperty"),wxT("Text1"));
        pgman->SetPropertyValue(wxT("IntProperty"),1024);
        pgman->SetPropertyValue(wxT("FloatProperty"),1024.0000000001);
        pgman->SetPropertyValue(wxT("BoolProperty"),FALSE);
        pgman->SetPropertyValue(wxT("EnumProperty"),120);
        pgman->SetPropertyValue(wxT("Custom FlagsProperty"),FLAG_TEST_SET1);
        pgman->SetPropertyValue(wxT("ArrayStringProperty"),test_arrstr_1);
        wxColour empty_col;
        pgman->SetPropertyValue(wxT("ColourProperty"),empty_col);
        pgman->SetPropertyValue(wxT("ColourProperty"),(wxObject*)wxBLACK);
        pgman->SetPropertyValue(wxT("Size"),wxSize(150,150));
        pgman->SetPropertyValue(wxT("Position"),wxPoint(150,150));
        pgman->SetPropertyValue(wxT("MultiChoiceProperty"),test_arrint_1);
#if wxUSE_DATETIME
        pgman->SetPropertyValue(wxT("DateProperty"),dt1);
#endif

        pgman->SelectPage(2);
        pg = pgman->GetGrid();

        if ( pg->GetPropertyValueAsString(wxT("StringProperty")) != wxT("Text1") )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsInt(wxT("IntProperty")) != 1024 )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsDouble(wxT("FloatProperty")) != 1024.0000000001 )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsBool(wxT("BoolProperty")) != FALSE )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsLong(wxT("EnumProperty")) != 120 )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsArrayString(wxT("ArrayStringProperty")) != test_arrstr_1 )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsLong(wxT("Custom FlagsProperty")) != FLAG_TEST_SET1 )
            RT_FAILURE();
        if ( *((wxColour*)pg->GetPropertyValueAsWxObjectPtr(wxT("ColourProperty"))) != *wxBLACK )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsSize(wxT("Size")) != wxSize(150,150) )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsPoint(wxT("Position")) != wxPoint(150,150) )
            RT_FAILURE();
        if ( !(pg->GetPropertyValueAsArrayInt(wxT("MultiChoiceProperty")) == test_arrint_1) )
            RT_FAILURE();
#if wxUSE_DATETIME
        if ( !(pg->GetPropertyValueAsDateTime(wxT("DateProperty")) == dt1) )
            RT_FAILURE();
#endif

        pg->SetPropertyValue(wxT("StringProperty"),wxT("Text2"));
        pg->SetPropertyValue(wxT("IntProperty"),512);
        pg->SetPropertyValue(wxT("FloatProperty"),512.0);
        pg->SetPropertyValue(wxT("BoolProperty"),TRUE);
        pg->SetPropertyValue(wxT("EnumProperty"),80);
        pg->SetPropertyValue(wxT("ArrayStringProperty"),test_arrstr_2);
        pg->SetPropertyValue(wxT("Custom FlagsProperty"),FLAG_TEST_SET2);
        pg->SetPropertyValue(wxT("ColourProperty"),(wxObject*)wxWHITE);
        pg->SetPropertyValue(wxT("Size"),wxSize(300,300));
        pg->SetPropertyValue(wxT("Position"),wxPoint(300,300));
        pg->SetPropertyValue(wxT("MultiChoiceProperty"),test_arrint_2);
#if wxUSE_DATETIME
        pg->SetPropertyValue(wxT("DateProperty"),dt2);
#endif

        //pg = (wxPropertyGrid*) NULL;

        pgman->SelectPage(0);

        if ( pgman->GetPropertyValueAsString(wxT("StringProperty")) != wxT("Text2") )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsInt(wxT("IntProperty")) != 512 )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsDouble(wxT("FloatProperty")) != 512.0 )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsBool(wxT("BoolProperty")) != TRUE )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsLong(wxT("EnumProperty")) != 80 )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsArrayString(wxT("ArrayStringProperty")) != test_arrstr_2 )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsLong(wxT("Custom FlagsProperty")) != FLAG_TEST_SET2 )
            RT_FAILURE();
        if ( *((wxColour*)pgman->GetPropertyValueAsWxObjectPtr(wxT("ColourProperty"))) != *wxWHITE )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsSize(wxT("Size")) != wxSize(300,300) )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsPoint(wxT("Position")) != wxPoint(300,300) )
            RT_FAILURE();
        if ( !(pgman->GetPropertyValueAsArrayInt(wxT("MultiChoiceProperty")) == test_arrint_2) )
            RT_FAILURE();
#if wxUSE_DATETIME
        if ( !(pgman->GetPropertyValueAsDateTime(wxT("DateProperty")) == dt2) )
            RT_FAILURE();
#endif

    }

    {
        RT_START_TEST(GetPropertyValues)

        pgman->SetTargetPage(0);
        wxVariant pg1_values = pgman->GetPropertyValues(wxT("Page1"),NULL,wxPG_KEEP_STRUCTURE);
        pgman->SetTargetPage(1);
        wxVariant pg2_values = pgman->GetPropertyValues(wxT("Page2"),NULL,wxPG_KEEP_STRUCTURE);
        pgman->SetTargetPage(2);
        wxVariant pg3_values = pgman->GetPropertyValues(wxT("Page3"),NULL,wxPG_KEEP_STRUCTURE);

        RT_START_TEST(SetPropertyValues)

        pgman->SetTargetPage(0);
        pgman->SetPropertyValues(pg3_values);
        pgman->SetTargetPage(1);
        pgman->SetPropertyValues(pg1_values);
        pgman->SetTargetPage(2);
        pgman->SetPropertyValues(pg2_values);
    }

    if ( !(pgman->GetWindowStyleFlag()&wxPG_HIDE_CATEGORIES) )
    {
        RT_START_TEST(Collapse_and_GetFirstCategory_and_GetNextCategory)

        for ( i=0; i<3; i++ )
        {
            pgman->SetTargetPage(i);

            id = pgman->GetFirstCategory();

            while ( id.IsOk() )
            {
                if ( !pgman->IsPropertyCategory(id) )
                    RT_FAILURE();

                pgman->Collapse( id );

                t.Printf(wxT("Collapsing: %s\n"),pgman->GetPropertyLabel(id).c_str());
                ed->AppendText(t);

                id = pgman->GetNextCategory(id);
            }
        }
    }

    //if ( !(pgman->GetWindowStyleFlag()&wxPG_HIDE_CATEGORIES) )
    {
        RT_START_TEST(Expand_and_GetFirstCategory_and_GetNextCategory)

        for ( i=0; i<3; i++ )
        {
            pgman->SetTargetPage(i);

            id = pgman->GetFirstCategory();

            while ( id.IsOk() )
            {
                if ( !pgman->IsPropertyCategory(id) )
                    RT_FAILURE();

                pgman->Expand( id );

                t.Printf(wxT("Expand: %s\n"),pgman->GetPropertyLabel(id).c_str());
                ed->AppendText(t);

                id = pgman->GetNextCategory(id);
            }
        }
    }

    //if ( !(pgman->GetWindowStyleFlag()&wxPG_HIDE_CATEGORIES) )
    {
        RT_START_TEST(RandomCollapse)

        // Select the most error prone page as visible.
        pgman->SelectPage(1);

        for ( i=0; i<3; i++ )
        {
            wxArrayPtrVoid arr;

            pgman->SetTargetPage(i);

            id = pgman->GetFirstCategory();

            while ( id.IsOk() )
            {
                //ed->AppendText(pgman->GetPropertyLabel(id));
                arr.Add((void*)id.GetPropertyPtr());
                id = pgman->GetNextCategory(id);
            }

            if ( arr.GetCount() )
            {
                size_t n;

                pgman->Collapse( wxPGId((wxPGProperty*)arr.Item(0)) );

                for ( n=arr.GetCount()-1; n>0; n-- )
                {
                    pgman->Collapse( wxPGId((wxPGProperty*)arr.Item(n)) );
                }
            }

        }
    }

    {
        RT_START_TEST(EnsureVisible)
        pgman->EnsureVisible(wxT("Cell Colour"));
    }

    {
        RT_START_TEST(SetPropertyColour)
        wxCommandEvent evt;
        evt.SetInt(1); // IsChecked() will return TRUE.
        evt.SetId(ID_COLOURSCHEME4);
        OnCatColours(evt);
        OnColourScheme(evt);
    }

    {
        // This must be last!!!
        RT_START_TEST(ClearPropertyValue)

        for ( i=0; i<3; i++ )
        {

            pgman->SetTargetPage(i);

            // Iterate over all properties.
            wxPGId id = pgman->GetFirstProperty();

            while ( id.IsOk() )
            {
                pgman->ClearPropertyValue( id );

                id = pgman->GetNextProperty ( id );
            }
        }
    }

    if ( failures )
        wxLogDebug(wxT("There were %i failures!!!"),failures);
    else
        wxLogDebug(wxT("All tests succesfull"));

    pgman->SelectPage(0);

    // Test may screw up the toolbar, so we need to refresh it.
    pgman->GetToolBar()->Refresh();

#endif // TESTING_WXPROPERTYGRIDADV
}

// -----------------------------------------------------------------------

void FormMain::OnPopulateClick( wxCommandEvent& event )
{
    int id = event.GetId();
    m_propGrid->Clear();
    m_propGrid->Freeze();
    if ( id == ID_POPULATE1 )
    {
        PopulateWithStandardItems();
    }
    else if ( id == ID_POPULATE2 )
    {
        PopulateWithLibraryConfig();
    }
    m_propGrid->Thaw();
}

// -----------------------------------------------------------------------

void FormMain::OnCloseEvent( wxCloseEvent& event )
{
    /*if ( !m_propGrid->CanClose() )
    {
        ::wxMessageBox(wxT("Cannot exit: value being edited in the property grid is invalid."));
        event.Veto();
        return;
    }*/
    event.Skip();
}

// -----------------------------------------------------------------------

static const wxChar* _fs_windowstyle_labels[] = {
    wxT("wxSIMPLE_BORDER"),
    wxT("wxDOUBLE_BORDER"),
    wxT("wxSUNKEN_BORDER"),
    wxT("wxRAISED_BORDER"),
    wxT("wxNO_BORDER"),
    wxT("wxTRANSPARENT_WINDOW"),
    wxT("wxTAB_TRAVERSAL"),
    wxT("wxWANTS_CHARS"),
    wxT("wxNO_FULL_REPAINT_ON_RESIZE"),
    wxT("wxVSCROLL"),
    wxT("wxALWAYS_SHOW_SB"),
    wxT("wxCLIP_CHILDREN"),
    wxT("wxFULL_REPAINT_ON_RESIZE"),
    (const wxChar*) NULL // terminator is always needed
};

static const long _fs_windowstyle_values[] = {
    wxSIMPLE_BORDER,
    wxDOUBLE_BORDER,
    wxSUNKEN_BORDER,
    wxRAISED_BORDER,
    wxNO_BORDER,
    wxTRANSPARENT_WINDOW,
    wxTAB_TRAVERSAL,
    wxWANTS_CHARS,
    wxNO_FULL_REPAINT_ON_RESIZE,
    wxVSCROLL,
    wxALWAYS_SHOW_SB,
    wxCLIP_CHILDREN,
    wxFULL_REPAINT_ON_RESIZE
};

// -----------------------------------------------------------------------

// for wxCustomProperty testing
bool TestCallback(wxPropertyGrid*,wxPGProperty*,wxWindow*,int)
{
    ::wxMessageBox(wxT("Button was clicked."),wxT("Callback works"));
    return false;
}

// for wxCustomProperty testing
void TestPaintCallback(wxPGProperty* WXUNUSED(property),wxDC& dc,
                       const wxRect& rect,wxPGPaintData& paintdata)
{
    if ( rect.x < 0 )
    {
        // Measuring
        paintdata.m_drawnHeight = 20+paintdata.m_choiceItem;
    }
    else
    {
        // Drawing

        wxRect r = rect;
        r.width += paintdata.m_choiceItem;
        if ( paintdata.m_choiceItem >= 0 )
            r.height = 20+paintdata.m_choiceItem;

        dc.SetClippingRegion(r);
        wxColour oldTxCol = dc.GetTextForeground();

        dc.SetPen(*wxBLACK);
        dc.SetBrush(*wxWHITE);
        dc.DrawRectangle(r);
        dc.SetTextForeground(*wxBLACK);
        wxString s;
        s.Printf(wxT("%i"),paintdata.m_choiceItem);
        dc.DrawText(s,r.x+6,r.y+2);

        paintdata.m_drawnWidth = r.width;

        dc.SetTextForeground(oldTxCol);
        dc.DestroyClippingRegion();
    }
}

void FormMain::PopulateWithStandardItems ()
{
#if TESTING_WXPROPERTYGRIDADV
    wxPropertyGridManager* pgman = m_pPropGridMan;
    wxPropertyGridManager* pg = pgman;

    pgman->SetTargetPage (wxT("Standard Items"));
#else
    wxPropertyGrid* pg = m_propGrid;
#endif
    wxPGId pid;

    // Append is ideal way to add items to wxPropertyGrid.
    pg->Append( wxPropertyCategory(wxT("Appearance"),wxPG_LABEL) );

    pg->Append( wxStringProperty(wxT("Label"),wxPG_LABEL,GetTitle()) );
    pg->Append( wxFontProperty(wxT("Font"),wxPG_LABEL) );
    pg->SetPropertyHelpString ( wxT("Font"), wxT("Editing this will change font used in the property grid.") );

    pg->Append( wxSystemColourProperty(wxT("Margin Colour"),wxPG_LABEL,
        pg->GetGrid()->GetMarginColour()) );

    pg->Append( wxSystemColourProperty(wxT("Cell Colour"),wxPG_LABEL,
        pg->GetGrid()->GetCellBackgroundColour()) );
    pg->Append( wxSystemColourProperty(wxT("Cell Text Colour"),wxPG_LABEL,
        pg->GetGrid()->GetCellTextColour()) );
    pg->Append( wxSystemColourProperty(wxT("Line Colour"),wxPG_LABEL,
        pg->GetGrid()->GetLineColour()) );
    pg->Append( wxFlagsProperty(wxT("Window Styles"),wxPG_LABEL,
        m_combinedFlags, GetWindowStyle()) );

    //pg->SetPropertyAttribute(wxT("Window Styles"),wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING,(long)1,wxPG_RECURSE);

    pg->Append( wxCursorProperty(wxT("Cursor"),wxPG_LABEL) );

    pg->Append( wxPropertyCategory(wxT("Position"),wxT("PositionCategory")) );
    pg->SetPropertyHelpString ( wxT("PositionCategory"), wxT("Change in items in this category will cause respective changes in frame.") );
    pg->Append( wxIntProperty(wxT("Height"),wxPG_LABEL,480) );
    pg->Append( wxIntProperty(wxT("Width"),wxPG_LABEL,640) );
    pg->Append( wxIntProperty(wxT("X"),wxPG_LABEL,10) );
    pg->Append( wxIntProperty(wxT("Y"),wxPG_LABEL,10) );

    pg->SetDefaultPriority ( wxPG_LOW );

    const wxChar* disabledHelpString = wxT("This property is simply disabled. Inorder to have label disabled as well, ")
                                       wxT("you need to set wxPG_EX_GREY_LABEL_WHEN_DISABLED using SetExtraStyle.");

    pg->Append( wxPropertyCategory(wxT("Environment"),wxPG_LABEL) );
    pg->Append( wxStringProperty(wxT("Operating System"),wxPG_LABEL,::wxGetOsDescription()) );

    pg->Append( wxStringProperty(wxT("User Id"),wxPG_LABEL,::wxGetUserId()) );
    pg->Append( wxDirProperty(wxT("User Home"),wxPG_LABEL,::wxGetUserHome()) );
    pg->Append( wxStringProperty(wxT("User Name"),wxPG_LABEL,::wxGetUserName()) );

    // Disable some of them
    pg->DisableProperty( wxT("Operating System") );
    pg->DisableProperty( wxT("User Id") );
    pg->DisableProperty( wxT("User Name") );

    pg->SetPropertyHelpString( wxT("Operating System"), disabledHelpString );
    pg->SetPropertyHelpString( wxT("User Id"), disabledHelpString );
    pg->SetPropertyHelpString( wxT("User Name"), disabledHelpString );

    pg->Append( wxPropertyCategory(wxT("More Examples"),wxPG_LABEL) );

    pg->Append( wxFontDataProperty( wxT("FontDataProperty"), wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("FontDataProperty"),
        wxT("This demonstrates wxFontDataProperty class defined in this sample app. ")
        wxT("It is exactly like wxFontProperty from the library, but also has colour sub-property.")
        );

    pg->Append( wxDirsProperty(wxT("DirsProperty"),wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("DirsProperty"),
        wxT("This demonstrates wxDirsProperty class defined in this sample app. ")
        wxT("It is built with WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR macro, ")
        wxT("with custom action (dir dialog popup) defined.")
        );

    pg->Append( wxAdvImageFileProperty(wxT("AdvImageFileProperty"),wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("AdvImageFileProperty"),
        wxT("This demonstrates wxAdvImageFileProperty class defined in this sample app. ")
        wxT("Button can be used to add new images to the popup list.")
        );

    wxArrayDouble arrdbl;
    arrdbl.Add(-1.0);
    arrdbl.Add(-0.5);
    arrdbl.Add(0.0);
    arrdbl.Add(0.5);
    arrdbl.Add(1.0);

    pg->Append( wxArrayDoubleProperty(wxT("ArrayDoubleProperty"),wxPG_LABEL,arrdbl) );
    //pg->SetPropertyAttribute(wxT("ArrayDoubleProperty"),wxPG_FLOAT_PRECISION,(long)2);
    pg->SetPropertyHelpString( wxT("ArrayDoubleProperty"),
        wxT("This demonstrates wxArrayDoubleProperty class defined in this sample app. ")
        wxT("It is an example of a custom list editor property.")
        );

    pg->Append( wxLongStringProperty(wxT("Information"),wxPG_LABEL,
        wxT("Editing properties will have immediate effect on this window, ")
        wxT("and vice versa (atleast in most cases, that is). Low priority after Examples ")
        wxT("means it will be hidden in Compact mode.")
        ) );

    pg->SetPropertyReadOnly( wxT("Information"), true );

    pg->ResetDefaultPriority();

    //
    // Custom user property demonstration
    //

    pg->AppendCategory( wxT("Custom User Property") );

    pid = pg->Append( wxCustomProperty(wxT("CustomProperty"),wxPG_LABEL) );
    pg->SetPropertyHelpString( pid,
        wxT("This is example of wxCustomProperty, easily customizable property class. ")
        wxT("Editing child properties will modify this property in real-time.") );

#ifdef wxPG_COMPATIBILITY_1_0_0
    // In 1.1.0+ button click event handler should handle this.
    pg->SetPropertyAttribute( pid, wxPG_CUSTOM_CALLBACK, (void*) TestCallback );
#endif

    // Add items for editing the custom property in real-time
    // See FormMain::OnPropertyGridChange for appropriate event handling

    pg->AppendIn( pid, wxStringProperty(wxT("Label"),
                                        wxPG_LABEL,
                                        wxT("CustomProperty")) );

    pg->AppendIn( pid, wxImageFileProperty(wxT("Image"),
                                           wxPG_LABEL) );

    static const wxChar* editors[] = {wxT("TextCtrl"),
                                      wxT("Choice"),
                                      wxT("ComboBox"),
                                      wxT("TextCtrlAndButton"),
                                      wxT("ChoiceAndButton"),
                                      (const wxChar*) NULL};

    pg->AppendIn( pid, wxEnumProperty(wxT("Editor"),
                                      wxPG_LABEL,
                                      editors) );

    wxArrayString defaultChoices;
    defaultChoices.Add(wxT("First Choice"));

    pg->AppendIn( pid, wxArrayStringProperty(wxT("Choices"),
                                             wxPG_LABEL,
                                             defaultChoices) );
    pg->DisableProperty(wxT("CustomProperty.Choices")); // Initially cannot be modified
    pg->SetPropertyHelpString(wxT("CustomProperty.Choices"),
        wxT("Disabled for non-Choice editors"));

    static const wxChar* paintMode[] = {wxT("Use Image"),
                                        wxT("Use Callback"),
                                        (const wxChar*) NULL};

    pg->AppendIn( pid, wxEnumProperty(wxT("Paint Mode"),
                                      wxPG_LABEL,
                                      paintMode) );

    //pg->SetPropertyAttribute( pid, wxPG_CUSTOM_PRIVATE_CHILDREN, (long) 1 );
}

// -----------------------------------------------------------------------

void FormMain::PopulateWithExamples ()
{
#if TESTING_WXPROPERTYGRIDADV
    wxPropertyGridManager* pgman = m_pPropGridMan;
    wxPropertyGridManager* pg = pgman;

    pgman->SetTargetPage (wxT("Examples"));
#else
    wxPropertyGrid* pg = m_propGrid;
#endif
    wxPGId pid;

    pg->SetDefaultPriority ( wxPG_LOW );

    //pg->Append( wxPropertyCategory(wxT("Examples (low priority)"),wxT("Examples")) );
    //pg->SetPropertyHelpString ( wxT("Examples"), wxT("This category has example of (almost) every built-in property class. Low priority means they will be hidden in compact mode.") );

    // Add int property - args similar as above. Note that wxPG_LABEL instructs
    // constructor to use label as name as well.
    pg->Append( wxIntProperty ( wxT("IntProperty"), wxPG_LABEL, 12345678 ) );

#if wxUSE_SPINBTN
    pg->Append( wxIntProperty ( wxT("SpinCtrl"), wxPG_LABEL, 0 ) );

    pg->SetPropertyEditor( wxT("SpinCtrl"), wxPG_EDITOR(SpinCtrl) );

    pg->SetPropertyHelpString( wxT("SpinCtrl"),
        wxT("This is regular wxIntProperty, which editor has been ")
        wxT("changed to wxPG_EDITOR(SpinCtrl). Note however that ")
        wxT("static wxPropertyGrid::RegisterAdditionalEditors() ")
        wxT("needs to be called prior to using it."));

#endif

    // Add bool property
    pg->Append( wxBoolProperty ( wxT("BoolProperty"), wxPG_LABEL, false ) );

    // Add bool property with check box
    pg->Append( wxBoolProperty ( wxT("BoolProperty with CheckBox"), wxPG_LABEL, false ) );
    pg->SetPropertyAttribute( wxT("BoolProperty with CheckBox"),
                              wxPG_BOOL_USE_CHECKBOX,
                              (long)1 );

    pg->SetPropertyHelpString( wxT("BoolProperty with CheckBox"),
        wxT("Property attribute wxPG_BOOL_USE_CHECKBOX has been set to (long)1.") );

    pid = pg->Append( wxFloatProperty( wxT("FloatProperty"),
                                       wxPG_LABEL,
                                       1234500.23 ) );

    // A string property that can be edited in a separate editor dialog.
    pg->Append( wxLongStringProperty( wxT("LongStringProperty"), wxT("LongStringProp"),
        wxT("This is much longer string than the first one. Edit it by clicking the button.") ) );

    // A property that edits a wxArrayString.
    wxArrayString example_array;
    example_array.Add( wxT("String 1"));
    example_array.Add( wxT("String 2"));
    example_array.Add( wxT("String 3"));
    pg->Append( wxArrayStringProperty ( wxT("ArrayStringProperty"), wxPG_LABEL,
         example_array) );

    // For testing validator in the string editor dialog
    wxInvalidWordValidator iwValidator(wxT("test"));
    pg->SetPropertyValidator( wxT("ArrayStringProperty"), iwValidator );
    pg->SetPropertyHelpString( wxT("ArrayStringProperty"),
        wxT("Has validator that prevents using text 'test'. This should work in the ")
        wxT("dialog as well.") );

    // Test adding same category multiple times ( should not actually create a new one )
    //pg->Append( wxPropertyCategory(wxT("Examples (low priority)"),wxT("Examples")) );

    // A file selector property. Note that argument between name
    // and initial value is wildcard (format same as in wxFileDialog).
    wxPGProperty* prop = wxFileProperty( wxT("FileProperty"), wxT("TextFile") );
    pg->Append( prop );

    prop->SetAttrib(wxPG_FILE_WILDCARD,wxT("Text Files (*.txt)|*.txt"));
    prop->SetAttrib(wxPG_FILE_DIALOG_TITLE,wxT("Custom File Dialog Title"));
    prop->SetAttrib(wxPG_FILE_SHOW_FULL_PATH,(long)0);

#ifdef __WXMSW__
    prop->SetAttrib(wxPG_FILE_SHOW_RELATIVE_PATH,wxT("C:\\Windows"));
    pg->SetPropertyValue(prop,wxT("C:\\Windows\\System32\\msvcrt71.dll"));
#endif

#if wxUSE_IMAGE
    // An image file property. Arguments are just like for FileProperty, but
    // wildcard is missing (it is autogenerated from supported image formats).
    // If you really need to override it, create property separately, and call
    // its SetWildcard method.
    pg->Append( wxImageFileProperty( wxT("ImageFile"), wxPG_LABEL ) );
#endif

    pid = pg->Append( wxColourProperty(wxT("ColourProperty"),wxPG_LABEL,*wxRED) );
    pg->SetPropertyAttribute(pid,wxPG_COLOUR_ALLOW_CUSTOM,(long)0);
    pg->SetPropertyEditor( wxT("ColourProperty"), wxPG_EDITOR(Choice) );
    pg->SetPropertyHelpString( wxT("ColourProperty"),
        wxT("wxPropertyGrid::SetPropertyEditor method has been used to change ")
        wxT("editor of this property to wxPG_EDITOR(Choice)"));

    //
    // This demonstrates using alternative editor for colour property
    // to trigger colour dialog directly from button.
    pg->Append( wxColourProperty(wxT("ColourProperty2"),wxPG_LABEL,*wxGREEN) );

    pg->Append( wxMyColourProperty(wxT("CustomColourProperty1")) );

    pg->SetPropertyHelpString(wxT("CustomColourProperty1"),
        wxT("This is a wxMyColourProperty from the sample app. ")
        wxT("It is built with WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR macro ")
        wxT("and has wxColour as its data type"));

    pg->Append( wxMyColour2Property(wxT("CustomColourProperty2")) );

    pg->SetPropertyHelpString(wxT("CustomColourProperty2"),
        wxT("This is a wxMyColour2Property from the sample app. ")
        wxT("It is built with WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY macro ")
        wxT("and has wxColourPropertyValue as its data type"));

    //
    // wxEnumProperty does not store strings or even list of strings
    // ( so that's why they are static in function ).
    static const wxChar* enum_prop_labels[] = { wxT("One Item"),
        wxT("Another Item"), wxT("One More"), wxT("This Is Last"), NULL };

    // this value array would be optional if values matched string indexes
    static long enum_prop_values[] = { 40, 80, 120, 160 };

    // note that the initial value (the last argument) is the actual value,
    // not index or anything like that. Thus, our value selects "Another Item".
    //
    // 0 before value is number of items. If it is 0, like in our example,
    // number of items is calculated, and this requires that the string pointer
    // array is terminated with NULL.
    pg->Append( wxEnumProperty(wxT("EnumProperty"),wxPG_LABEL,
        enum_prop_labels, enum_prop_values, 80 ) );

    wxPGChoices soc;

    // use basic table from our previous example
    // can also set/add wxArrayStrings and wxArrayInts directly.
    soc.Set ( enum_prop_labels, enum_prop_values );

    // add extra items
    soc.Add( wxT("Look, it continues"), 200 );
    soc.Add( wxT("Even More"), 240 );
    soc.Add( wxT("And More"), 280 );
    soc.Add( wxT("True End of the List"), 320 );

    pg->Append(wxEnumProperty(wxT("EnumProperty 2"),
                              wxPG_LABEL,
                              soc,
                              240) );
    pg->AddPropertyChoice(wxT("EnumProperty 2"),wxT("Testing Extra"),360);

    // Add a second time to test that the caching works
    pg->Append( wxEnumProperty(wxT("EnumProperty 3"),wxPG_LABEL,
        soc, 360 ) );
    pg->SetPropertyHelpString(wxT("EnumProperty 3"),
        wxT("Should have same choices as EnumProperty 2"));

    pg->Append( wxEnumProperty(wxT("EnumProperty 4"),wxPG_LABEL,
        soc, 240 ) );
    pg->SetPropertyHelpString(wxT("EnumProperty 4"),
        wxT("Should have same choices as EnumProperty 2"));

    pg->Append( wxEnumProperty(wxT("EnumProperty 5"),wxPG_LABEL,
        soc, 240 ) );
    pg->SetPropertyChoicesExclusive(wxT("EnumProperty 5"));
    pg->AddPropertyChoice(wxT("EnumProperty 5"),wxT("5th only"),360);
    pg->SetPropertyHelpString(wxT("EnumProperty 5"),
        wxT("Should have one extra item when compared to EnumProperty 4"));

    // Password property example.
    pg->Append( wxStringProperty(wxT("Password"),wxPG_LABEL, wxT("password")) );
    pg->SetPropertyAttribute( wxT("Password"), wxPG_STRING_PASSWORD, (long)1 );
    pg->SetPropertyHelpString( wxT("Password"),
        wxT("Has attribute wxPG_STRING_PASSWORD set to (long)1") );

    // String editor with dir selector button. Uses wxEmptyString as name, which
    // is allowed (naturally, in this case property cannot be accessed by name).
    pg->Append( wxDirProperty( wxT("DirProperty"), wxPG_LABEL, ::wxGetUserHome()) );
    pg->SetPropertyAttribute( wxT("DirProperty"),
                              wxPG_DIR_DIALOG_MESSAGE,
                              wxT("This is a custom dir dialog message") );

    // Add string property - first arg is label, second name, and third initial value
    pg->Append( wxStringProperty ( wxT("StringProperty"), wxPG_LABEL ) );
    pg->SetPropertyMaxLength( wxT("StringProperty"), 6 );
    pg->SetPropertyHelpString( wxT("StringProperty"),
        wxT("Max length of this text has been limited to 6, using wxPropertyGrid::SetPropertyMaxLength.") );

    // Set value after limiting so that it will be applied
    pg->SetPropertyValue( wxT("StringProperty"), wxT("some text") );


    // this value array would be optional if values matched string indexes
    //long flags_prop_values[] = { wxICONIZE, wxCAPTION, wxMINIMIZE_BOX, wxMAXIMIZE_BOX };

    //pg->Append( wxFlagsProperty(wxT("Example of FlagsProperty"),wxT("FlagsProp"),
    //    flags_prop_labels, flags_prop_values, 0, GetWindowStyle() ) );


    // Multi choice dialog.
    wxPGChoices tchoices;
    tchoices.Add(wxT("Cabbage"),10);
    tchoices.Add(wxT("Carrot"),15);
    tchoices.Add(wxT("Onion"),20);
    tchoices.Add(wxT("Potato"),25);
    tchoices.Add(wxT("Strawberry"),30);
    const wxArrayString& mcStrings = tchoices.GetLabels();
    
    wxArrayInt tchoicesValues;
    tchoicesValues.Add(1);
    tchoicesValues.Add(15);
    tchoicesValues.Add(20);

    pg->Append( wxEnumProperty(wxT("EnumProperty X"),wxPG_LABEL, tchoices ) );

    pg->Append( wxMultiChoiceProperty( wxT("MultiChoiceProperty"), wxPG_LABEL,
                                       tchoices, tchoicesValues ) );


    // Enum property with totally custom painted list items
    wxPGProperty* mep = new wxPaintedEnumPropertyClass(wxT("EnumProperty (custom painted)"),
                                                       wxPG_LABEL,
                                                       mcStrings);
    pg->Append( mep );

    pg->SetPropertyHelpString( wxPGId(mep),
        wxT("Subclassed wxEnumPropertyClass. Methods GetImageSize() and OnCustomPaint() have been implemented.") );

    pg->Append( wxTestCustomFlagsProperty(wxT("Custom FlagsProperty"), wxPG_LABEL ) );
    pg->SetPropertyEditor( wxT("Custom FlagsProperty"), wxPG_EDITOR(TextCtrlAndButton) );

    pg->Append( wxTestCustomEnumProperty(wxT("Custom EnumProperty"), wxPG_LABEL ) );

    pg->Append( wxSizeProperty( wxT("SizeProperty"), wxT("Size"), GetSize() ) );
    pg->Append( wxPointProperty( wxT("PointProperty"), wxT("Position"), GetPosition() ) );


    // UInt samples
    pg->Append( wxUIntProperty( wxT("UIntProperty"), wxPG_LABEL, 0xFEEEFEEE ));
    pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_PREFIX, wxPG_PREFIX_0x );
    pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_BASE, wxPG_BASE_HEX );

    //
    // wxEditEnumProperty
    wxPGChoices eech;
    eech.Add(wxT("Choice 1"));
    eech.Add(wxT("Choice 2"));
    eech.Add(wxT("Choice 3"));
    pg->Append( wxEditEnumProperty(wxT("EditEnumProperty"), wxPG_LABEL, eech) ); // , wxT("Choice 2")

    //wxString v_;
    //wxTextValidator validator1(wxFILTER_NUMERIC,&v_);
    //pg->SetPropertyValidator( wxT("EditEnumProperty"), validator1 );

#if wxUSE_DATETIME
    //
    // wxDateTimeProperty
    pg->Append( wxDateProperty(wxT("DateProperty"), wxPG_LABEL, wxDateTime::Now() ) );

#if wxUSE_DATEPICKCTRL
    pg->SetPropertyAttribute( wxT("DateProperty"), wxPG_DATE_PICKER_STYLE,
                              (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY) );

    pg->SetPropertyHelpString( wxT("DateProperty"),
        wxT("Attribute wxPG_DATE_PICKER_STYLE has been set to (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY).")
        wxT("Also note that wxPG_ALLOW_WXADV needs to be defined inorder to use wxDatePickerCtrl.") );
#endif

#endif

/*
    wxPGId categoryID = pg->Append( wxPropertyCategory(wxT("Cat1"), wxPG_LABEL));
    wxPGId parentID = pg->AppendIn(categoryID, wxParentProperty(wxT("Pivot"), wxPG_LABEL));
    pg->AppendIn(parentID, wxFloatProperty(wxT("x"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("y"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("z"), wxPG_LABEL, 1));
    parentID = pg->AppendIn(categoryID, wxParentProperty(wxT("Position_"), wxPG_LABEL));
    pg->AppendIn(parentID, wxFloatProperty(wxT("x"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("y"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("z"), wxPG_LABEL, 1)); 
*/

    //
    // Add Triangle properties as both wxTriangleProperty and
    // a generic wxParentProperty.
    //
    wxPGId topId = pg->Append( wxParentProperty(wxT("3D Object"), wxPG_LABEL) );

    pid = pg->AppendIn( topId, wxParentProperty(wxT("Triangle 1"), wxEmptyString) );
    pg->AppendIn( pid, wxVectorProperty( wxT("A"), wxEmptyString ) );
    pg->AppendIn( pid, wxVectorProperty( wxT("B"), wxEmptyString ) );
    pg->AppendIn( pid, wxVectorProperty( wxT("C"), wxEmptyString ) );

    pg->AppendIn( topId, wxTriangleProperty( wxT("Triangle 2"), wxEmptyString ) );

    pg->SetPropertyHelpString( wxT("3D Object"),
        wxT("3D Object is wxParentProperty. Two of its children are wxParentProperties with ")
        wxT("three wxVectorProperty children, and other two are custom wxTriangleProperties.") );

    pid = pg->AppendIn( topId, wxParentProperty(wxT("Triangle 3"), wxEmptyString) );
    pg->AppendIn( pid, wxVectorProperty( wxT("A"), wxEmptyString ) );
    pg->AppendIn( pid, wxVectorProperty( wxT("B"), wxEmptyString ) );
    pg->AppendIn( pid, wxVectorProperty( wxT("C"), wxEmptyString ) );

    pg->AppendIn( topId, wxTriangleProperty( wxT("Triangle 4"), wxEmptyString ) );

    pg->Append( wxTextStringPropertyWithValidator(wxT("TestProp1"), wxPG_LABEL) );
    pg->Append( wxLongLongProperty(wxT("LongLongProperty"), wxPG_LABEL, wxLL(1000000000000)) );

    pg->ResetDefaultPriority ();
}

// -----------------------------------------------------------------------

void FormMain::PopulateWithLibraryConfig ()
{
#if TESTING_WXPROPERTYGRIDADV
    wxPropertyGridManager* pgman = m_pPropGridMan;
    wxPropertyGridManager* pg = pgman;

    pgman->SetTargetPage (wxT("wxWidgets Library Config"));
#else
    wxPropertyGrid* pg = m_propGrid;
#endif
    wxPGId pid;

#define ADD_WX_LIB_CONF_GROUP(A) pg->AppendIn ( pid, wxPropertyCategory(A) );
#define ADD_WX_LIB_CONF(A) pg->Append(wxT(#A),wxPG_LABEL,(bool)((A>0)?TRUE:FALSE));
#define ADD_WX_LIB_CONF_NODEF(A) pg->Append(wxT(#A),wxPG_LABEL,(bool)FALSE); \
                            pg->DisableProperty(wxT(#A));

    pid = pg->Append( wxPropertyCategory( wxT("wxWidgets Library Configuration") ) );

    ADD_WX_LIB_CONF_GROUP(wxT("Global Settings"))
    ADD_WX_LIB_CONF ( wxUSE_GUI )

    ADD_WX_LIB_CONF_GROUP(wxT("Compatibility Settings"))
#if defined(WXWIN_COMPATIBILITY_2_2)
    ADD_WX_LIB_CONF ( WXWIN_COMPATIBILITY_2_2 )
#endif
#if defined(WXWIN_COMPATIBILITY_2_4)
    ADD_WX_LIB_CONF ( WXWIN_COMPATIBILITY_2_4 )
#endif
#if defined(WXWIN_COMPATIBILITY_2_6)
    ADD_WX_LIB_CONF ( WXWIN_COMPATIBILITY_2_6 )
#endif
#ifdef wxFONT_SIZE_COMPATIBILITY
    ADD_WX_LIB_CONF ( wxFONT_SIZE_COMPATIBILITY )
#else
    ADD_WX_LIB_CONF_NODEF ( wxFONT_SIZE_COMPATIBILITY )
#endif
#ifdef wxDIALOG_UNIT_COMPATIBILITY
    ADD_WX_LIB_CONF ( wxDIALOG_UNIT_COMPATIBILITY )
#else
    ADD_WX_LIB_CONF_NODEF ( wxDIALOG_UNIT_COMPATIBILITY )
#endif

    ADD_WX_LIB_CONF_GROUP(wxT("Debugging Settings"))
    ADD_WX_LIB_CONF ( wxUSE_DEBUG_CONTEXT )
    ADD_WX_LIB_CONF ( wxUSE_MEMORY_TRACING )
    ADD_WX_LIB_CONF ( wxUSE_GLOBAL_MEMORY_OPERATORS )
    ADD_WX_LIB_CONF ( wxUSE_DEBUG_NEW_ALWAYS )
    ADD_WX_LIB_CONF ( wxUSE_ON_FATAL_EXCEPTION )

    ADD_WX_LIB_CONF_GROUP(wxT("Unicode Support"))
    ADD_WX_LIB_CONF ( wxUSE_UNICODE )
    ADD_WX_LIB_CONF ( wxUSE_UNICODE_MSLU )
    ADD_WX_LIB_CONF ( wxUSE_WCHAR_T )

    ADD_WX_LIB_CONF_GROUP(wxT("Global Features"))
    ADD_WX_LIB_CONF ( wxUSE_EXCEPTIONS )
    ADD_WX_LIB_CONF ( wxUSE_EXTENDED_RTTI )
    ADD_WX_LIB_CONF ( wxUSE_STL )
    ADD_WX_LIB_CONF ( wxUSE_LOG )
    ADD_WX_LIB_CONF ( wxUSE_LOGWINDOW )
    ADD_WX_LIB_CONF ( wxUSE_LOGGUI )
    ADD_WX_LIB_CONF ( wxUSE_LOG_DIALOG )
    ADD_WX_LIB_CONF ( wxUSE_CMDLINE_PARSER )
    ADD_WX_LIB_CONF ( wxUSE_THREADS )
    ADD_WX_LIB_CONF ( wxUSE_STREAMS )
    ADD_WX_LIB_CONF ( wxUSE_STD_IOSTREAM )

    ADD_WX_LIB_CONF_GROUP(wxT("Non-GUI Features"))
    ADD_WX_LIB_CONF ( wxUSE_LONGLONG )
    ADD_WX_LIB_CONF ( wxUSE_FILE )
    ADD_WX_LIB_CONF ( wxUSE_FFILE )
    ADD_WX_LIB_CONF ( wxUSE_FSVOLUME )
    ADD_WX_LIB_CONF ( wxUSE_TEXTBUFFER )
    ADD_WX_LIB_CONF ( wxUSE_TEXTFILE )
    ADD_WX_LIB_CONF ( wxUSE_INTL )
    ADD_WX_LIB_CONF ( wxUSE_DATETIME )
    ADD_WX_LIB_CONF ( wxUSE_TIMER )
    ADD_WX_LIB_CONF ( wxUSE_STOPWATCH )
    ADD_WX_LIB_CONF ( wxUSE_CONFIG )
#ifdef wxUSE_CONFIG_NATIVE
    ADD_WX_LIB_CONF ( wxUSE_CONFIG_NATIVE )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_CONFIG_NATIVE )
#endif
    ADD_WX_LIB_CONF ( wxUSE_DIALUP_MANAGER )
    ADD_WX_LIB_CONF ( wxUSE_DYNLIB_CLASS )
    ADD_WX_LIB_CONF ( wxUSE_DYNAMIC_LOADER )
    ADD_WX_LIB_CONF ( wxUSE_SOCKETS )
    ADD_WX_LIB_CONF ( wxUSE_FILESYSTEM )
    ADD_WX_LIB_CONF ( wxUSE_FS_ZIP )
    ADD_WX_LIB_CONF ( wxUSE_FS_INET )
    ADD_WX_LIB_CONF ( wxUSE_ZIPSTREAM )
    ADD_WX_LIB_CONF ( wxUSE_ZLIB )
    ADD_WX_LIB_CONF ( wxUSE_APPLE_IEEE )
    ADD_WX_LIB_CONF ( wxUSE_JOYSTICK )
    ADD_WX_LIB_CONF ( wxUSE_FONTMAP )
    ADD_WX_LIB_CONF ( wxUSE_MIMETYPE )
    ADD_WX_LIB_CONF ( wxUSE_PROTOCOL )
    ADD_WX_LIB_CONF ( wxUSE_PROTOCOL_FILE )
    ADD_WX_LIB_CONF ( wxUSE_PROTOCOL_FTP )
    ADD_WX_LIB_CONF ( wxUSE_PROTOCOL_HTTP )
    ADD_WX_LIB_CONF ( wxUSE_URL )
#ifdef wxUSE_URL_NATIVE
    ADD_WX_LIB_CONF ( wxUSE_URL_NATIVE )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_URL_NATIVE )
#endif
    ADD_WX_LIB_CONF ( wxUSE_REGEX )
    ADD_WX_LIB_CONF ( wxUSE_SYSTEM_OPTIONS )
    ADD_WX_LIB_CONF ( wxUSE_SOUND )
#ifdef wxUSE_XRC
    ADD_WX_LIB_CONF ( wxUSE_XRC )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_XRC )
#endif
    ADD_WX_LIB_CONF ( wxUSE_XML )

    // Set them to use check box.
    pg->SetPropertyAttribute(pid,wxPG_BOOL_USE_CHECKBOX,(long)1,wxPG_RECURSE);
}


//
// Handle events of the third page here.
class wxMyPropertyGridPage : public wxPropertyGridPage
{
public:

    // Return false here to indicate unhandled events should be
    // propagated to manager's parent, as normal.
    virtual bool IsHandlingAllEvents() const { return false; }

protected:

    virtual wxPGId DoInsert( wxPGPropertyWithChildren* parent,
                             int index,
                             wxPGProperty* property )
    {
        wxPGId id = wxPropertyGridPage::DoInsert(parent,index,property);

        //wxLogDebug(wxT("wxMyPropertyGridPage::DoInsert(%s)"),property->GetName().c_str());

        return id;
    }

    void OnPropertySelect( wxPropertyGridEvent& event );
    void OnPropertyChange( wxPropertyGridEvent& event );
    void OnPageChange( wxPropertyGridEvent& event );

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxMyPropertyGridPage, wxPropertyGridPage)
    EVT_PG_SELECTED( wxID_ANY, wxMyPropertyGridPage::OnPropertySelect )
    EVT_PG_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPropertyChange )
    EVT_PG_PAGE_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPageChange )
END_EVENT_TABLE()


void wxMyPropertyGridPage::OnPropertySelect( wxPropertyGridEvent& WXUNUSED(event) )
{
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPropertySelect()"));
}

void wxMyPropertyGridPage::OnPropertyChange( wxPropertyGridEvent& WXUNUSED(event) )
{
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPropertyChange()"));
}

void wxMyPropertyGridPage::OnPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPageChange()"));
}

class wxPGKeyHandler : public wxEvtHandler
{
public:

    void OnKeyEvent( wxKeyEvent& event )
    {
        wxMessageBox(wxString::Format(wxT("%i"),event.GetKeyCode()));
        event.Skip();
    }
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxPGKeyHandler,wxEvtHandler)
    EVT_KEY_DOWN( wxPGKeyHandler::OnKeyEvent )
END_EVENT_TABLE()



#include <time.h>

FormMain::FormMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
           wxFrame((wxFrame *)NULL, -1, title, pos, size,
               (wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCAPTION|
                wxTAB_TRAVERSAL|wxCLOSE_BOX|wxNO_FULL_REPAINT_ON_RESIZE) )
{
    m_propGrid = NULL;

    srand( (unsigned)time( NULL ) );

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = ID_ABOUT;
#endif

    wxWindow* panel = new wxPanel(this,-1,wxPoint(0,0),wxSize(400,400));

//#if 0
    // Column
    wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );

#if !TESTING_WXPROPERTYGRIDADV && !SIMPLE_DEMO
    //
    // Tabs or RadioBox
    //
#ifdef wxPGS_USE_TABCTRL
    m_tabctrl = new wxTabCtrl( panel, TCID );
    m_tabctrl->InsertItem(0,wxT("Categorized Mode"));
    m_tabctrl->InsertItem(1,wxT("Alphabetic Mode"));

    Connect(m_tabctrl->GetId(), wxEVT_COMMAND_TAB_SEL_CHANGED,
            (wxObjectEventFunction) (wxEventFunction) (wxTabEventFunction)
            &FormMain::OnTabChange );

    topSizer->Add( m_tabctrl, 0, wxEXPAND|wxFIXED_MINSIZE );
#else
    const wxString rb_items[] = {wxString(wxT("Categorized")),
        wxString(wxT("Alphabetic"))};
    m_radiobox = new wxRadioBox ( panel, TCID, wxT(" Mode "),
        wxDefaultPosition, wxDefaultSize,
        2, rb_items, 2, wxRA_SPECIFY_COLS );

    Connect(m_radiobox->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &FormMain::OnRadioBoxChange );

    topSizer->Add( m_radiobox, 0, wxEXPAND );
#endif

#endif

    //
    // PropertyGrid
    //

    //
    // This shows how to combine two constant descriptors
    m_combinedFlags.Add( _fs_windowstyle_labels, _fs_windowstyle_values );
    m_combinedFlags.Add( _fs_framestyle_labels, _fs_framestyle_values );
#if TESTING_WXPROPERTYGRIDADV

    wxPropertyGridManager* pgman = m_pPropGridMan =
        new wxPropertyGridManager(panel,
                                  // Don't change this into wxID_ANY in the sample, or the
                                  // event handling will obviously be broken.
                                  PGID, /*wxID_ANY*/
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxPG_BOLD_MODIFIED |
                                  wxPG_SPLITTER_AUTO_CENTER |
                                  wxPG_AUTO_SORT |
                                  //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
                                  //wxPG_TOOLTIPS |
                                  //wxPG_HIDE_CATEGORIES |
                                  //wxPG_LIMITED_EDITING |
                                  wxTAB_TRAVERSAL |
                                  wxPG_TOOLBAR |
                                  wxPG_DESCRIPTION
                                  //wxPG_COMPACTOR
                                 );

    pgman->SetExtraStyle(wxPG_EX_MODE_BUTTONS
                         //| wxPG_EX_AUTO_UNSPECIFIED_VALUES
                         //| wxPG_EX_GREY_LABEL_WHEN_DISABLED
                         //| wxPG_EX_NATIVE_DOUBLE_BUFFERING
                         //| wxPG_EX_HELP_AS_TOOLTIPS
                         );

    //pgman->GetGrid()->SetCellDisabledTextColour( wxColour(255,0,0) );
    //pgman->GetGrid()->CenterSplitter();

#else

    wxPropertyGrid* pg = m_propGrid =
        new wxPropertyGrid(panel,
                           // Don't change this into wxID_ANY in the sample, or the
                           // event handling will obviously be broken.
                           PGID, /*wxID_ANY*/
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxPG_BOLD_MODIFIED |
                           //wxPG_SPLITTER_AUTO_CENTER |
                           wxPG_AUTO_SORT |
                           //wxPG_HIDE_MARGIN | wxPG_STATIC_SPLITTER |
                           //wxPG_TOOLTIPS |
                           //wxPG_NOCATEGORIES |
                           wxTAB_TRAVERSAL |
                           wxSUNKEN_BORDER
                          );

    pg->SetExtraStyle(wxPG_EX_MODE_BUTTONS|wxPG_EX_HELP_AS_TOOLTIPS); // |wxPG_EX_HELP_AS_TOOLTIPS

#endif

    // Register *all* present property classes
    // (required for text-save/load support)
    m_pPropGridMan->RegisterAdvancedPropertyClasses();
    wxPGRegisterPropertyClass(wxFontDataProperty);
    wxPGRegisterPropertyClass(wxPointProperty);
    wxPGRegisterPropertyClass(wxSizeProperty);
    wxPGRegisterPropertyClass(wxAdvImageFileProperty);
    wxPGRegisterPropertyClass(wxDirsProperty);
    wxPGRegisterPropertyClass(wxArrayDoubleProperty);
    wxPGRegisterPropertyClass(wxVectorProperty);
    wxPGRegisterPropertyClass(wxTriangleProperty);
    wxPGRegisterPropertyClass(wxTestCustomEnumProperty);
    wxPGRegisterPropertyClass(wxTestCustomFlagsProperty);
    wxPGRegisterPropertyClass(wxMyColourProperty);
    wxPGRegisterPropertyClass(wxMyColour2Property);

    m_pPropGridMan->GetGrid()->SetVerticalSpacing( 2 );

    // Register all editors (SpinCtrl etc.)
    m_pPropGridMan->RegisterAdditionalEditors();

#if wxUSE_IMAGE
    // This is here to really test the wxImageFileProperty.
    wxInitAllImageHandlers();
#endif

#if TESTING_WXPROPERTYGRIDADV
    pgman->AddPage(wxT("Standard Items"));

    PopulateWithStandardItems();

    pgman->AddPage(wxT("wxWidgets Library Config"));

    PopulateWithLibraryConfig();

    // Use wxMyPropertyGridPage (see above) to test the
    // custom wxPropertyGridPage feature.
    pgman->AddPage(wxT("Examples"),wxNullBitmap,new wxMyPropertyGridPage());

    PopulateWithExamples();

    pgman->SetTargetPage(0);

#else
    PopulateWithStandardItems();
#endif

    // Change some attributes in all properties
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING,(long)1);
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX,(long)1);

    //m_pPropGridMan->SetSplitterLeft(true);
    //m_pPropGridMan->SetSplitterPosition(137);

    /*
    // This would setup event handling without event table entries
    Connect(m_pPropGridMan->GetId(), wxEVT_PG_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxPropertyGridEventFunction)
            &FormMain::OnPropertyGridSelect );
    Connect(m_pPropGridMan->GetId(), wxEVT_PG_CHANGED,
            (wxObjectEventFunction) (wxEventFunction) (wxPropertyGridEventFunction)
            &FormMain::OnPropertyGridChange );
    */

    topSizer->Add( m_pPropGridMan, 1, wxEXPAND );

#if !SIMPLE_DEMO
    // Additional control (for keyboard navigation testing)
    topSizer->Add( new wxButton(panel,1234,wxT("Button for Tab Traversal Testing")), 0, wxEXPAND|wxFIXED_MINSIZE );
#endif

//#endif // 0
#if 0
    m_pPropGridMan = NULL;
    m_propGrid = new wxPropertyGrid(panel,
					wxID_ANY,
					wxDefaultPosition,
					wxDefaultSize,
					wxPG_BOLD_MODIFIED |
					//wxPG_SPLITTER_AUTO_CENTER |
					wxPG_AUTO_SORT |
					//wxPG_HIDE_MARGIN | wxPG_STATIC_SPLITTER |
					//wxPG_TOOLTIPS |
					//wxPG_NOCATEGORIES |
					wxTAB_TRAVERSAL |
					wxSUNKEN_BORDER
					);

	m_propGrid->Append( wxPropertyCategory(wxT("Appearance"),wxPG_LABEL) );

	m_propGrid->Append( wxStringProperty(wxT("Label"),wxPG_LABEL,wxT("Test")) );

	wxBoxSizer* topSizer = new wxBoxSizer (wxVERTICAL);
	topSizer->Add( m_propGrid, 1, wxALL|wxEXPAND );
	panel->SetSizer(topSizer);
	Layout();
#endif

    //
    // Create menubar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    wxMenu *menuTry = new wxMenu;
    wxMenu *menuTools1 = new wxMenu;
    wxMenu *menuTools2 = new wxMenu;
    wxMenu *menuHelp = new wxMenu;

    menuHelp->Append(ID_ABOUT, wxT("&About..."), wxT("Show about dialog") );

    menuTools1->Append(ID_APPENDPROP, wxT("Append New Property") );
    menuTools1->Append(ID_APPENDCAT, wxT("Append New Category\tCtrl-S") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_INSERTPROP, wxT("Insert New Property\tCtrl-Q") );
    menuTools1->Append(ID_INSERTCAT, wxT("Insert New Category\tCtrl-W") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_DELETE, wxT("Delete Selected") );
    menuTools1->Append(ID_DELETER, wxT("Delete Random") );
    menuTools1->Append(ID_DELETEALL, wxT("Delete All") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_SETCOLOUR, wxT("Set Bg Colour") );
    menuTools1->Append(ID_UNSPECIFY, wxT("Set to Unspecified") );
    menuTools1->Append(ID_SETASHIDEABLE, wxT("Toggle Priority"),
        wxT("Low-priority items are hidden in Compact mode (high priority is the default).") );
    menuTools1->Append(ID_CLEAR, wxT("Set Value to Default") );
    menuTools1->AppendSeparator();
    m_itemEnable = menuTools1->Append(ID_ENABLE, wxT("Enable"),
        wxT("Toggles item's enabled state.") );
    m_itemEnable->Enable( FALSE );
    menuTools1->Append(ID_HIDE, wxT("Hide"), wxT("Shows or hides a property") );

    menuTools2->Append(ID_ITERATE1, wxT("Iterate Over Properties") );
    menuTools2->Append(ID_ITERATE2, wxT("Iterate Over Visible Items") );
    menuTools2->Append(ID_ITERATE3, wxT("Reverse Iterate Over Properties") );
    menuTools2->Append(ID_ITERATE4, wxT("Iterate Over Categories") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SETPROPERTYVALUE, wxT("Set Property Value") );
    menuTools2->Append(ID_CLEARMODIF, wxT("Clear Modified Status"), wxT("Clears wxPG_MODIFIED flag from all properties.") );
    menuTools2->AppendSeparator();
    m_itemFreeze = menuTools2->AppendCheckItem(ID_FREEZE, wxT("Freeze"),
        wxT("Disables painting, auto-sorting, etc.") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_DUMPLIST, wxT("Display Values as wxVariant List"), wxT("Tests GetAllValues method and wxVariant conversion.") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_GETVALUES, wxT("Get Property Values"), wxT("Stores all property values.") );
    menuTools2->Append(ID_SETVALUES, wxT("Set Property Values"), wxT("Reverts property values to those last stored.") );
    menuTools2->Append(ID_SETVALUES2, wxT("Set Property Values 2"), wxT("Adds property values that should not initially be as items (so new items are created).") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_COLLAPSE, wxT("Collapse Selected") );
    menuTools2->Append(ID_COLLAPSEALL, wxT("Collapse All") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_INSERTPAGE, wxT("Add Page") );
    menuTools2->Append(ID_REMOVEPAGE, wxT("Remove Page") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_CHANGEFLAGSITEMS, wxT("Change Children of FlagsProp") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_TESTINSERTCHOICE, wxT("Test InsertPropertyChoice") );
    menuTools2->Append(ID_TESTDELETECHOICE, wxT("Test DeletePropertyChoice") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SETSPINCTRLEDITOR, wxT("Use SpinCtrl Editor") );
    menuTools2->Append(ID_TESTREPLACE, wxT("Test ReplaceProperty") );

#if !TESTING_WXPROPERTYGRIDADV
    menuTry->AppendRadioItem( ID_POPULATE1, wxT("Default Items") );
    menuTry->AppendRadioItem( ID_POPULATE2, wxT("wxWidgets Library Config") );
    menuTry->AppendSeparator();
#endif
    menuTry->AppendRadioItem( ID_COLOURSCHEME1, wxT("Standard Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME2, wxT("White Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME3, wxT(".NET Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME4, wxT("Cream Colour Scheme") );
    menuTry->AppendSeparator();
    m_itemCatColours = menuTry->AppendCheckItem(ID_CATCOLOURS, wxT("Category Specific Colours"),
        wxT("Switches between category-specific cell colours and default scheme (actually done using SetPropertyTextColour and SetPropertyBackgroundColour).") );
    menuTry->AppendSeparator();
    m_itemCompact = menuTry->AppendCheckItem(ID_COMPACT, wxT("Compact"),
        wxT("When checked, all hideable (sort-of low priority) properties are hidden.") );
    menuTry->AppendSeparator();
    menuTry->AppendCheckItem(ID_STATICLAYOUT, wxT("Static Layout"),
        wxT("Switches between user-modifiedable and static layouts.") );
    menuTry->AppendSeparator();
    menuTry->Append(ID_RUNTEST, wxT("Run Tests") );
    menuTry->AppendSeparator();
    menuTry->Append(ID_SAMPLEDIALOG, wxT("Custom Combo Sample") );

    menuFile->Append(ID_SAVETOFILE, wxT("&Save Page (use class names)..."),
        wxT("Saves current property page to a text file, using property class names.") );
    menuFile->Append(ID_SAVETOFILE2, wxT("&Save Page (use value names)..."),
        wxT("Saves current property page to a text file, using property value type names.") );
    menuFile->Append(ID_LOADFROMFILE, wxT("&Load Page..."),
        wxT("Loads current property page from a text file") );
    menuFile->AppendSeparator();
    menuFile->Append(ID_QUIT, wxT("E&xit\tAlt-X"), wxT("Quit this program") );

    // Now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, wxT("&File") );
    menuBar->Append(menuTry, wxT("&Try These!") );
    menuBar->Append(menuTools1, wxT("&Basic") );
    menuBar->Append(menuTools2, wxT("&Advanced") );
    menuBar->Append(menuHelp, wxT("&Help") );

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar
    CreateStatusBar(1);
    SetStatusText(wxEmptyString);
#endif // wxUSE_STATUSBAR


    //
    // Finalize
    //

    panel->SetSizer( topSizer );
    topSizer->SetSizeHints( panel );

    wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
    panelSizer->Add( panel, 1, wxEXPAND|wxFIXED_MINSIZE );
    SetSizer( panelSizer );
    panelSizer->SetSizeHints( this );

    SetSize(
        (wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/10)*4,
        (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/10)*8
        );
    Centre();
}

// -----------------------------------------------------------------------

#include <wx/propgrid/odcombo.h>

//
// wxListView Custom popup interface
//

#include <wx/listctrl.h>

class ListViewComboPopup : public wxListView, public wxPGComboPopup
{
public:

    ListViewComboPopup(wxPGComboControl* combo)
        : wxListView(), wxPGComboPopup(combo)
    {
        m_value = -1;
        m_itemHere = -1; // hot item in list
    }

    virtual bool Create( wxWindow* parent )
    {
        return wxListView::Create(parent,1,
                                  wxPoint(0,0),wxDefaultSize,
                                  wxLC_LIST|wxLC_SINGLE_SEL|
                                  wxLC_SORT_ASCENDING|wxSIMPLE_BORDER);
    }

    virtual wxWindow *GetControl() { return this; }

    //virtual int GetIntValue() const { return m_value; }

    virtual void SetStringValue( const wxString& s )
    {
        int n = wxListView::FindItem(-1,s);
        if ( n >= 0 && n < GetItemCount() )
            wxListView::Select(n);
    }

    virtual wxString GetStringValue() const
    {
        if ( m_value >= 0 )
            return wxListView::GetItemText(m_value);
        return wxEmptyString;
    }

    //
    // Popup event handlers
    //

    // Mouse hot-tracking
    void OnMouseMove(wxMouseEvent& event)
    {
        // Move selection to cursor if it is inside the popup

        int resFlags;
        int itemHere = HitTest(event.GetPosition(),resFlags);
        if ( itemHere >= 0 )
        {
            wxListView::Select(itemHere,true);
            m_itemHere = itemHere;
        }
        event.Skip();
    }

    // On mouse left, set the value and close the popup
    void OnMouseClick(wxMouseEvent& WXUNUSED(event))
    {
        m_value = m_itemHere;
        // TODO: Send event
        Dismiss();
    }

    //
    // Utilies for item manipulation
    //

    void AddSelection( const wxString& selstr )
    {
        wxListView::InsertItem(GetItemCount(),selstr);
    }

protected:

    int             m_value; // current item index
    int             m_itemHere; // hot item in popup

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ListViewComboPopup, wxListView)
    EVT_MOTION(ListViewComboPopup::OnMouseMove)
    // NOTE: Left down event is used instead of left up right now
    //       since MSW wxListCtrl doesn't seem to emit left ups
    //       consistently.
    EVT_LEFT_DOWN(ListViewComboPopup::OnMouseClick)
END_EVENT_TABLE()



//
// wxTreeCtrl Custom popup interface
//

#include <wx/treectrl.h>

class TreeCtrlComboPopup : public wxTreeCtrl, public wxPGComboPopup
{
public:

    TreeCtrlComboPopup(wxPGComboControl* combo)
        : wxTreeCtrl(), wxPGComboPopup(combo)
    {
    }

    virtual bool Create( wxWindow* parent )
    {
        return wxTreeCtrl::Create(parent,1,
                                  wxPoint(0,0),wxDefaultSize,
                                  wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|
                                  wxTR_SINGLE|wxTR_LINES_AT_ROOT|
                                  wxSIMPLE_BORDER);
    }

    virtual void OnShow()
    {
        // make sure selected item is visible
        if ( m_value.IsOk() )
            EnsureVisible(m_value);
    }

    virtual wxSize GetAdjustedSize( int minWidth,
                                    int WXUNUSED(prefHeight),
                                    int maxHeight )
    {
        return wxSize(wxMax(300,minWidth),wxMin(250,maxHeight));
    }

    virtual wxWindow *GetControl() { return this; }

    // Needed by SetStringValue
    wxTreeItemId FindItemByText( wxTreeItemId parent, const wxString& text )
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(parent,cookie);
        while ( child.IsOk() )
        {
            if ( GetItemText(child) == text )
            {
                return child;
            }
            if ( ItemHasChildren(child) )
            {
                wxTreeItemId found = FindItemByText(child,text);
                if ( found.IsOk() )
                    return found;
            }
            child = GetNextChild(parent,cookie);
        }
        return wxTreeItemId();
    }

    virtual void SetStringValue( const wxString& s )
    {
        wxTreeItemId root = GetRootItem();
        if ( !root.IsOk() )
            return;

        wxTreeItemId found = FindItemByText(root,s);
        if ( found.IsOk() )
        {
            m_value = m_itemHere = found;
            wxTreeCtrl::SelectItem(found);
        }
    }

    virtual wxString GetStringValue() const
    {
        if ( m_value.IsOk() )
            return wxTreeCtrl::GetItemText(m_value);
        return wxEmptyString;
    }

    //
    // Popup event handlers
    //

    // Mouse hot-tracking
    void OnMouseMove(wxMouseEvent& event)
    {
        int resFlags;
        wxTreeItemId itemHere = HitTest(event.GetPosition(),resFlags);
        if ( itemHere.IsOk() && (resFlags & wxTREE_HITTEST_ONITEMLABEL) )
        {
            wxTreeCtrl::SelectItem(itemHere,true);
            m_itemHere = itemHere;
        }
        event.Skip();
    }

    // On mouse left, set the value and close the popup
    void OnMouseClick(wxMouseEvent& event)
    {
        int resFlags;
        wxTreeItemId itemHere = HitTest(event.GetPosition(),resFlags);
        if ( itemHere.IsOk() && (resFlags & wxTREE_HITTEST_ONITEMLABEL) )
        {
            m_itemHere = itemHere;
            m_value = itemHere;
            Dismiss();
            // TODO: Send event
        }
        event.Skip();
    }

protected:

    wxTreeItemId        m_value; // current item index
    wxTreeItemId        m_itemHere; // hot item in popup

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TreeCtrlComboPopup, wxTreeCtrl)
    EVT_MOTION(TreeCtrlComboPopup::OnMouseMove)
    // NOTE: Left down event is used instead of left up right now
    //       since MSW wxTreeCtrl doesn't seem to emit left ups
    //       consistently.
    EVT_LEFT_DOWN(TreeCtrlComboPopup::OnMouseClick)
END_EVENT_TABLE()



class wxArrStrEditorPanel : public wxPanel
{
public:
    wxArrStrEditorPanel();
    ~wxArrStrEditorPanel();

    void Init();

    wxArrStrEditorPanel(wxWindow *parent, wxWindowID id, const wxArrayString& strings,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& sz = wxDefaultSize,
                        int style = 0
                       );

    bool Create(wxWindow *parent, wxWindowID id, const wxArrayString& strings,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& sz = wxDefaultSize,
                int style = 0
               );

    /*inline void SetCustomButton ( const wxChar* custBtText, wxArrayStringPropertyClass* pcc )
    {
        m_custBtText = custBtText;
        m_pCallingClass = pcc;
    }*/

    // Returns TRUE if string array was actually modified
    inline bool IsModified() const { return m_modified; }

    inline const wxArrayString& GetStrings() const { return m_array; }

    void SetStrings( const wxArrayString& strings );

    // implementation from now on
    void OnUpdateClick(wxCommandEvent& event);
    void OnAddClick(wxCommandEvent& event);
    void OnDeleteClick(wxCommandEvent& event);
    void OnListBoxClick(wxCommandEvent& event);
    //void OnUpClick(wxCommandEvent& event);
    //void OnDownClick(wxCommandEvent& event);
    //void OnCustomEditClick(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnCloseClick(wxCommandEvent& event);

protected:
    wxTextCtrl*     m_edValue;
    wxListBox*      m_lbStrings;

    wxButton*       m_butAdd;       // Button pointers
    //wxButton*       m_butCustom;    // required for disabling/enabling changing.
    wxButton*       m_butUpdate;
    wxButton*       m_butRemove;
    //wxButton*       m_butUp;
    //wxButton*       m_butDown;

    wxButton*       m_butClose;

    wxArrayString   m_array;

    const wxChar*                   m_custBtText;
    //wxArrayStringPropertyClass*     m_pCallingClass;

    bool            m_modified;

    unsigned char   m_curFocus;

private:
    DECLARE_DYNAMIC_CLASS(wxArrStrEditorPanel)
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxArrStrEditorPanel, wxPanel)
    EVT_IDLE(wxArrStrEditorPanel::OnIdle)
    EVT_LISTBOX(24, wxArrStrEditorPanel::OnListBoxClick)
    EVT_LISTBOX(24, wxArrStrEditorPanel::OnListBoxClick)
    EVT_TEXT_ENTER(21, wxArrStrEditorPanel::OnAddClick)
    EVT_BUTTON(22, wxArrStrEditorPanel::OnAddClick)
    EVT_BUTTON(23, wxArrStrEditorPanel::OnDeleteClick)
    //EVT_BUTTON(25, wxArrStrEditorPanel::OnUpClick)
    //EVT_BUTTON(26, wxArrStrEditorPanel::OnDownClick)
    EVT_BUTTON(27, wxArrStrEditorPanel::OnUpdateClick)
    //EVT_BUTTON(28, wxArrStrEditorPanel::OnCustomEditClick)
    EVT_BUTTON(30, wxArrStrEditorPanel::OnCloseClick)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxArrStrEditorPanel, wxPanel)

#include <wx/statline.h>

void wxArrStrEditorPanel::OnIdle (wxIdleEvent&)
{
    //
    // Do control focus detection here.
    //
/*
    if ( m_curFocus == 0 && FindFocus() == m_lbStrings )
    {
        // ListBox was just focused.
        m_butAdd->Enable(FALSE);
        m_butUpdate->Enable(FALSE);
        m_butRemove->Enable(TRUE);
        //m_butUp->Enable(TRUE);
        //m_butDown->Enable(TRUE);
        m_curFocus = 1;
    }
    else if ( (m_curFocus == 1 && FindFocus() == m_edValue) )
    {
        // TextCtrl was just focused.
        m_butAdd->Enable(TRUE);
        bool upd_enable = FALSE;
        if ( m_lbStrings->GetCount() && m_lbStrings->GetSelection() >= 0 )
            upd_enable = TRUE;
        m_butUpdate->Enable(upd_enable);
        m_butRemove->Enable(FALSE);
        //m_butUp->Enable(FALSE);
        //m_butDown->Enable(FALSE);
        m_curFocus = 0;
    }
*/
    //wxLogDebug(wxT("wxArrStrEditorPanel::OnFocusChange"));
}

wxArrStrEditorPanel::wxArrStrEditorPanel()
    : wxPanel()
{
    Init();
}

wxArrStrEditorPanel::~wxArrStrEditorPanel()
{
}

void wxArrStrEditorPanel::Init()
{
    m_custBtText = (const wxChar*) NULL;
    //m_pCallingClass = (wxArrayStringPropertyClass*) NULL;
}

wxArrStrEditorPanel::wxArrStrEditorPanel(wxWindow *parent,
                                 wxWindowID id,
                                 const wxArrayString& strings,
                                 const wxPoint& pos,
                                 const wxSize& sz,
                                 int style )
                              : wxPanel()
{
    Init();
    Create(parent,id,strings,pos,sz,style);
}

bool wxArrStrEditorPanel::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxArrayString& strings,
                                 const wxPoint& pos,
                                 const wxSize& sz,
                                 int style )
{

    bool res = wxPanel::Create (parent,id,pos,sz,style);

    const int spacing = 4;

    m_array = strings;

    m_modified = FALSE;

    m_curFocus = 1;

    /*const int butSzFlags =
        wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL|wxALL; //wxBOTTOM|wxLEFT|wxRIGHT;*/

    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );

    // Message
    /*if ( message.length() )
        topsizer->Add ( new wxStaticText(this,-1,message),
            0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxALL, spacing );*/

    // String editor
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    /*rowsizer->Add ( new wxStaticText(this,-1,_("String:")),
        0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxBOTTOM|wxTOP|wxLEFT, spacing );*/
    m_edValue = new wxTextCtrl(this,21,wxEmptyString,
        wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
    rowsizer->Add ( m_edValue,
        1, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxALL, spacing );

    // Add button
    m_butAdd = new wxButton(this,22,_("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    rowsizer->Add ( m_butAdd,
        0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, spacing );
    topsizer->Add ( rowsizer, 0, wxEXPAND, spacing );

    // Separator line
    topsizer->Add ( new wxStaticLine(this,-1),
        0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, spacing );

    rowsizer = new wxBoxSizer( wxHORIZONTAL );

    // list box
    m_lbStrings = new wxListBox(this, 24, wxDefaultPosition, wxDefaultSize, strings);
    rowsizer->Add ( m_lbStrings, 1, wxEXPAND|wxRIGHT|wxBOTTOM, spacing );

    // Manipulator buttons
    wxBoxSizer* colsizer = new wxBoxSizer( wxVERTICAL );
    /*m_butCustom = (wxButton*) NULL;
    if ( m_custBtText )
    {
        m_butCustom = new wxButton(this,28,::wxGetTranslation(m_custBtText));
        colsizer->Add ( m_butCustom,
            0, wxALIGN_CENTER|wxTOP,
            spacing );
    }*/
    m_butUpdate = new wxButton(this,27,_("Update"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    colsizer->Add ( m_butUpdate,
        0, wxALIGN_CENTER|wxTOP, spacing );
    m_butRemove = new wxButton(this,23,_("Remove"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    colsizer->Add ( m_butRemove,
        0, wxALIGN_CENTER|wxTOP, spacing );
    /*m_butUp = new wxButton(this,25,_("Up"));
    colsizer->Add ( m_butUp,
        0, wxALIGN_CENTER|wxTOP, spacing );
    m_butDown = new wxButton(this,26,_("Down"));
    colsizer->Add ( m_butDown,
        0, wxALIGN_CENTER|wxTOP, spacing );*/
    m_butClose = new wxButton(this,30,_("Close"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    colsizer->Add ( m_butClose,
        0, wxALIGN_CENTER|wxALIGN_BOTTOM|wxTOP|wxEXPAND, spacing );
    rowsizer->Add ( colsizer, 0, 0, spacing );

    topsizer->Add ( rowsizer, 1, wxLEFT|wxRIGHT|wxEXPAND, spacing );

    // Separator line
    /*topsizer->Add ( new wxStaticLine(this,-1),
        0, wxEXPAND|wxTOP|wxLEFT|wxRIGHT, spacing );

    // buttons
    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    rowsizer->Add ( new wxButton(this,30,_("Close")),
        0, butSzFlags, spacing );
    rowsizer->Add ( new wxButton(this,wxID_CANCEL,_("Cancel")),
        0, butSzFlags, spacing );
    topsizer->Add ( rowsizer, 0, wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL, 0 );*/

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

void wxArrStrEditorPanel::SetStrings( const wxArrayString& strings )
{
    m_array = strings;
    m_lbStrings->Clear();
    size_t i;
    for ( i=0; i<strings.GetCount(); i++ )
        m_lbStrings->Append(strings[i]);
}

void wxArrStrEditorPanel::OnAddClick(wxCommandEvent& )
{
    wxString text = m_edValue->GetValue();
    if ( text.length() )
    {
        m_lbStrings->Append ( text );
        m_array.Add( text );
        m_modified = TRUE;
        m_edValue->Clear();
    }
}

void wxArrStrEditorPanel::OnDeleteClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 )
    {
        m_lbStrings->Delete ( index );
        m_array.RemoveAt ( index );
        m_modified = TRUE;
    }
}

/*void wxArrStrEditorPanel::OnUpClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index > 0 )
    {
        wxString old_str = m_array[index-1];
        wxString new_str = m_array[index];
        m_array[index-1] = new_str;
        m_array[index] = old_str;
        m_lbStrings->SetString ( index-1, new_str );
        m_lbStrings->SetString ( index, old_str );
        m_lbStrings->SetSelection ( index-1 );
        m_modified = TRUE;
    }
}

void wxArrStrEditorPanel::OnDownClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 && index < (m_lbStrings->GetCount()-1) )
    {
        wxString old_str = m_array[index+1];
        wxString new_str = m_array[index];
        m_array[index+1] = new_str;
        m_array[index] = old_str;
        m_lbStrings->SetString ( index+1, new_str );
        m_lbStrings->SetString ( index, old_str );
        m_lbStrings->SetSelection ( index+1 );
        m_modified = TRUE;
    }
}*/

void wxArrStrEditorPanel::OnUpdateClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 )
    {
        wxString str = m_edValue->GetValue();
        m_lbStrings->SetString ( index, str );
        m_array[index] = str;
        m_modified = TRUE;
    }
}

/*void wxArrStrEditorPanel::OnCustomEditClick(wxCommandEvent& )
{
    wxASSERT ( m_pCallingClass );
    wxString str = m_edValue->GetValue();
    if ( m_pCallingClass->OnCustomStringEdit(m_parent,str) )
    {
        //m_edValue->SetValue ( str );
        m_lbStrings->Append ( str );
        m_array.Add( str );
        m_modified = TRUE;
    }
}*/

void wxArrStrEditorPanel::OnListBoxClick(wxCommandEvent& )
{
    int index = m_lbStrings->GetSelection();
    if ( index >= 0 )
    {
        m_edValue->SetValue ( m_lbStrings->GetString(index) );
    }
}

void wxArrStrEditorPanel::OnCloseClick(wxCommandEvent& )
{
    wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED,GetId());
    GetEventHandler()->AddPendingEvent(evt);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

/*
#include <wx/tokenzr.h>

class ArrStrEditorPopupInterface : public wxComboPopupInterface
{
public:

    ArrStrEditorPopupInterface()
        : wxComboPopupInterface()
    {
        m_popup = (wxArrStrEditorPanel*) NULL;
    }

    virtual bool Init ( wxPGOwnerDrawnComboBox* combo )
    {
        wxComboPopupInterface::Init(combo);
        return false; // generate on show
    }

    virtual wxWindow* GeneratePopup( wxWindow* parent, int minWidth,
                                     int, int prefHeight )
    {
        if ( parent )
        {
            m_popup =
                new wxArrStrEditorPanel(parent,1,m_strings,
                    wxDefaultPosition,wxDefaultSize,wxSIMPLE_BORDER);
        }
        else
            m_popup->SetStrings(m_strings);

        m_popup->SetSize ( minWidth, prefHeight );

        return m_popup;
    }

    virtual void SetValueFromString ( const wxString& s )
    {
        m_strings.Empty();

        wxStringTokenizer tkz(s, wxT(","));
        while ( tkz.HasMoreTokens() )
        {
            wxString token = tkz.GetNextToken();

            // trim spaces
            token.Trim(TRUE);
            token.Trim(FALSE);

            m_strings.Add( token );

        }

        if ( m_popup )
            m_popup->SetStrings( m_strings );

    }

    virtual wxString GetValueAsString() const
    {
        wxString s;

        size_t i;

        for ( i=0; i<m_strings.GetCount(); i++ )
        {
            s += m_strings[i];
            if ( i < (m_strings.GetCount()-1) )
                s += wxT(", ");
        }

        return s;
    }

    void AddString ( const wxString& s )
    {
        m_strings.Add(s);
    }

    // event handlers
    void OnPanelClose ( wxCommandEvent& )
    {
        m_strings = m_popup->GetStrings();
        m_combo->HidePopup();
    }

protected:

    wxArrStrEditorPanel*    m_popup;

    wxArrayString           m_strings;

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ArrStrEditorPopupInterface, wxComboPopupInterface)
    EVT_BUTTON(1,ArrStrEditorPopupInterface::OnPanelClose)
END_EVENT_TABLE()

*/

class SampleFrame : public wxDialog
{
public:
    SampleFrame( wxWindow* parent );
    ~SampleFrame();

    void OnComboItemPaint(wxPGComboControl* pCc,int item,wxDC& dc,
        wxRect& rect,int flags );

    void OnComboBoxUpdate( wxCommandEvent& event );

    void OnCloseClick( wxCommandEvent& event );
    void OnNavigationKeyEvent( wxNavigationKeyEvent& event );

    void OnClose( wxCloseEvent& event );

protected:
/*
    wxPGOwnerDrawnComboBox* m_odc0;
    wxPGOwnerDrawnComboBox* m_odc;
    wxPGComboControl* m_odc2;
    wxPGOwnerDrawnComboBox* m_odc3;
*/

    wxTextCtrl *m_logWin;
    wxLog *m_logOld;
private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(SampleFrame, wxDialog)
    EVT_TEXT(wxID_ANY,SampleFrame::OnComboBoxUpdate)
    EVT_COMBOBOX(wxID_ANY,SampleFrame::OnComboBoxUpdate)
    EVT_BUTTON(40,SampleFrame::OnCloseClick)
    EVT_CLOSE(SampleFrame::OnClose)
END_EVENT_TABLE()

void SampleFrame::OnNavigationKeyEvent( wxNavigationKeyEvent& event )
{
    wxLogDebug(wxT("SampleFrame::OnNavigationKeyEvent"));
    event.Skip();
}

void SampleFrame::OnCloseClick( wxCommandEvent& )
{
    Close();
}

void SampleFrame::OnClose( wxCloseEvent& )
{
    Destroy();
}

void SampleFrame::OnComboBoxUpdate( wxCommandEvent& event )
{
    // Don't show messages for the log output window (it'll crash)
    if ( event.GetId() == 105 )
        return;

    if ( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
        wxLogDebug(wxT("EVT_COMBOBOX(%i)"),event.GetSelection());
    else if ( event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED )
        wxLogDebug(wxT("EVT_TEXT(\"%s\")"),event.GetString().c_str());
}

class wxPenStyleComboBox : public wxPGOwnerDrawnComboBox
{
public:
    virtual bool OnDrawListItem( wxDC& dc, const wxRect& rect, int item, int flags )
    {
        wxRect r(rect);
        r.Deflate(3);
        r.height -= 2;

        int pen_style = wxSOLID;
        if ( item == 1 )
            pen_style = wxTRANSPARENT;
        else if ( item == 2 )
            pen_style = wxDOT;
        else if ( item == 3 )
            pen_style = wxLONG_DASH;
        else if ( item == 4 )
            pen_style = wxSHORT_DASH;
        else if ( item == 5 )
            pen_style = wxDOT_DASH;
        else if ( item == 6 )
            pen_style = wxBDIAGONAL_HATCH;
        else if ( item == 7 )
            pen_style = wxCROSSDIAG_HATCH;
        else if ( item == 8 )
            pen_style = wxFDIAGONAL_HATCH;
        else if ( item == 9 )
            pen_style = wxCROSS_HATCH;
        else if ( item == 10 )
            pen_style = wxHORIZONTAL_HATCH;
        else if ( item == 11 )
            pen_style = wxVERTICAL_HATCH;

        wxPen pen( dc.GetTextForeground(), 3, pen_style );

        // Get text colour as pen colour
        dc.SetPen ( pen );

        if ( !(flags & wxPGCC_PAINTING_CONTROL) )
        {
            dc.SetBrush( *wxTRANSPARENT_BRUSH );
            dc.DrawRectangle( r );

            dc.DrawText(GetString( item ),
                        r.x + 3,
                        (r.y + 0) + ( (r.height) - dc.GetCharHeight() )/2
                       );
        }
        else
        {
            dc.DrawLine( r.x+5, r.y+r.height/2, r.x+r.width - 5, r.y+r.height/2 );
        }

        return true;
    }
    virtual wxCoord OnMeasureListItem( int WXUNUSED(item) )
    {
        return 24;
    }
};

void SampleFrame::OnComboItemPaint(wxPGComboControl* pCc,
                                   int item,
                                   wxDC& dc,
                                   wxRect& rect,
                                   int flags )
{
    wxPGOwnerDrawnComboBox* pCb = (wxPGOwnerDrawnComboBox*) pCc;

    // Measure call?
    if ( rect.x < 0 )
    {
        rect.height = 24;
        return;
    }

    if ( item < 0 )
        return;

    wxRect r(rect);
    r.Deflate(2);

    int pen_style = wxSOLID;
    if ( item == 1 )
        pen_style = wxTRANSPARENT;
    else if ( item == 2 )
        pen_style = wxDOT;
    else if ( item == 3 )
        pen_style = wxLONG_DASH;
    else if ( item == 4 )
        pen_style = wxSHORT_DASH;
    else if ( item == 5 )
        pen_style = wxDOT_DASH;
    else if ( item == 6 )
        pen_style = wxBDIAGONAL_HATCH;
    else if ( item == 7 )
        pen_style = wxCROSSDIAG_HATCH;
    else if ( item == 8 )
        pen_style = wxFDIAGONAL_HATCH;
    else if ( item == 9 )
        pen_style = wxCROSS_HATCH;
    else if ( item == 10 )
        pen_style = wxHORIZONTAL_HATCH;
    else if ( item == 11 )
        pen_style = wxVERTICAL_HATCH;

    wxPen pen( dc.GetTextForeground(), 1, pen_style );
    //pen.SetCap(wxCAP_BUTT);
    //pen.SetCap(wxCAP_PROJECTING);
    //pen.SetCap(wxCAP_ROUND);
    //pen.SetJoin(wxJOIN_ROUND);

    // Get text colour as pen colour
    dc.SetPen ( pen );

    if ( !(flags & wxPGCC_PAINTING_CONTROL) )
    {
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        dc.DrawRectangle( r );

        dc.DrawText (pCb->GetString( item ),
                 r.x + 2,
                 (r.y + 0) + ( (r.height) - dc.GetCharHeight() )/2
                 );
    }
    else
    {
        dc.DrawLine ( r.x+5, r.y+r.height/2, r.x+r.width - 5, r.y+r.height/2 );
    }
}


//
// wxComboControl with entirely custom button action
//
class wxFileSelectorCombo : public wxPGComboControl
{
public:
    wxFileSelectorCombo() : wxPGComboControl() { Init(); }

    wxFileSelectorCombo(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxPGComboControl()
    {
        Init();
        Create(parent,id,value,pos,size,style,validator,name);

        //
        // Prepare custom button bitmap
        wxMemoryDC dc;
        wxBitmap bmp(32,20);
        dc.SelectObject(bmp);
        wxColour magic(255,0,255);
        wxBrush magicBrush(magic);
        dc.SetBrush( magicBrush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(0,0,bmp.GetWidth(),bmp.GetHeight());

        wxString str = wxT("...");
        int w,h;
        dc.GetTextExtent(str, &w, &h, 0, 0);
        dc.DrawText(str, (bmp.GetWidth()-w)/2, (bmp.GetHeight()-h)/2);

        dc.SelectObject( wxNullBitmap );
        wxMask *mask = new wxMask( bmp, magic );
        bmp.SetMask( mask );

        SetButtonBitmaps(bmp,true);
    }

    virtual void OnButtonClick()
    {
        // Show standard wxFileDialog on button click

        wxFileDialog dlg(this,
                         wxT("Choose File"),
                         wxEmptyString,
                         GetValue(),
                         wxT("All files (*.*)|*.*"),
#if wxCHECK_VERSION(2,8,0)
                         wxFD_OPEN
#else
                         wxOPEN
#endif
                         );

        if ( dlg.ShowModal() == wxID_OK )
        {
            SetValue(dlg.GetPath());
        }
    }

protected:
    void Init()
    {
        // Initialize member variables here
    }
};


SampleFrame::SampleFrame( wxWindow* parent ) :
           wxDialog(parent, -1, wxT("Custom Combo Sample"), wxDefaultPosition, wxDefaultSize,
               (wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCAPTION|wxCLOSE_BOX|wxTAB_TRAVERSAL|wxNO_FULL_REPAINT_ON_RESIZE) )
{
    wxBoxSizer* topSizer;
    wxBoxSizer* topRowSizer;
    wxBoxSizer* colSizer;
    wxBoxSizer* rowSizer;
    wxStaticBoxSizer* groupSizer;

    SetBackgroundColour( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE ) );

    // Prepare log window right away since it shows EVT_TEXTs
    m_logWin = new wxTextCtrl( this, 105, wxEmptyString, wxDefaultPosition,
                               wxSize(-1,125), wxTE_MULTILINE|wxFULL_REPAINT_ON_RESIZE );
    m_logWin->SetEditable(false);
    wxLogTextCtrl* logger = new wxLogTextCtrl( m_logWin );
    m_logOld = logger->SetActiveTarget( logger );
    logger->SetTimestamp( (const wxChar*)NULL );

    topSizer = new wxBoxSizer( wxVERTICAL );

    topRowSizer = new wxBoxSizer( wxHORIZONTAL );

    colSizer = new wxBoxSizer( wxVERTICAL );

#if wxUSE_IMAGE
    // This is here to really test the wxImageFileProperty.
    wxInitAllImageHandlers();
#endif

    wxComboBox* cb;
    wxPGComboControl* cc;
    wxPGOwnerDrawnComboBox* odc;

    // Create pen items array
    wxArrayString arrItems;
    arrItems.Add( wxT("Solid") );
    arrItems.Add( wxT("Transparent") );
    arrItems.Add( wxT("Dot") );
    arrItems.Add( wxT("Long Dash") );
    arrItems.Add( wxT("Short Dash") );
    arrItems.Add( wxT("Dot Dash") );
    arrItems.Add( wxT("Backward Diagonal Hatch") );
    arrItems.Add( wxT("Cross-diagonal Hatch") );
    arrItems.Add( wxT("Forward Diagonal Hatch") );
    arrItems.Add( wxT("Cross Hatch") );
    arrItems.Add( wxT("Horizontal Hatch") );
    arrItems.Add( wxT("Vertical Hatch") );

    int border = 4;

    //
    // Native ComboBox vs ODComboBox comparison
    //
    rowSizer = new wxBoxSizer(wxHORIZONTAL);

    groupSizer = new wxStaticBoxSizer(new wxStaticBox(this,wxID_ANY,wxT(" wxOwnerDrawnComboBox ")),
                                      wxVERTICAL);

    groupSizer->Add( new wxStaticText(this,-1,wxT("Writable, sorted:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT|wxEXPAND, border );

    odc = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->Append(wxT("H - Appended Item")); // test sorting in append

    odc->SetValue(wxT("Dot Dash"));

    groupSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Readonly ODComboBox
    //
    groupSizer->Add( new wxStaticText(this,-1,wxT("Read-only:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    odc = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->SetValue(wxT("Dot Dash"));

    groupSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Disabled ODComboBox
    //
    groupSizer->Add( new wxStaticText(this,-1,wxT("Disabled:")), 0,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    odc = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->SetValue(wxT("Dot Dash"));
    odc->Enable(false);

    groupSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    rowSizer->Add( groupSizer, 1, wxEXPAND|wxALL, border );


    groupSizer = new wxStaticBoxSizer(new wxStaticBox(this,wxID_ANY,wxT(" wxComboBox ")),
                                      wxVERTICAL);

    //
    // wxComboBox
    //
    groupSizer->Add( new wxStaticText(this,-1,wxT("Writable, sorted:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT|wxEXPAND, border );

    cb = new wxComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT // wxNO_BORDER|wxCB_READONLY
                                     );

    cb->Append(wxT("H - Appended Item")); // test sorting in append

    cb->SetValue(wxT("Dot Dash"));

    groupSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Readonly wxComboBox
    //
    groupSizer->Add( new wxStaticText(this,-1,wxT("Read-only:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    cb = new wxComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    cb->SetValue(wxT("Dot Dash"));

    groupSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Disabled wxComboBox
    //
    groupSizer->Add( new wxStaticText(this,-1,wxT("Disabled:")), 0,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    cb = new wxComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    cb->SetValue(wxT("Dot Dash"));
    cb->Enable(false);

    groupSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    rowSizer->Add( groupSizer, 1, wxEXPAND|wxALL, border );


    colSizer->Add( rowSizer, 1, wxEXPAND|wxALL, border );

/*
    //
    // Native ComboBox vs ODComboBox comparison
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    groupSizer = new wxStaticBoxSizer( wxVERTICAL );

    rowSizer->Add( new wxStaticText(this,-1,wxT("OwnerDrawnComboBox:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    rowSizer->Add( new wxStaticText(this,-1,wxT("Native ComboBox:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    odc = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->Append(wxT("H - Appended Item")); // test sorting in append

    odc->SetValue(wxT("Dot Dash"));

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );

    cb = new wxComboBox(this,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        arrItems,
                        wxCB_SORT // wxNO_BORDER
                       );

    cb->Append(wxT("H - Appended Item")); // test sorting in append

    cb->SetValue(wxT("Dot Dash"));

    rowSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    //
    // Native ComboBox vs ODComboBox comparison (readonly versions)
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(this,-1,wxT("Read-only")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    //rowSizer->Add( new wxStaticText(this,-1,wxT("Native ComboBox:")), 1,
    //               wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    odc = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->SetValue(wxT("Dot Dash"));

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );

    cb = new wxComboBox(this,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        arrItems,
                        wxCB_SORT|wxCB_READONLY // wxNO_BORDER
                       );

    cb->SetValue(wxT("Dot Dash"));

    rowSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );


    //
    // Native ComboBox vs ODComboBox comparison (disabled versions)
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(this,-1,wxT("Disabled:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    //rowSizer->Add( new wxStaticText(this,-1,wxT("Native ComboBox:")), 1,
    //               wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    odc = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->SetValue(wxT("Dot Dash"));
    odc->Enable(false);

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );

    cb = new wxComboBox(this,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        arrItems,
                        wxCB_SORT|wxCB_READONLY // wxNO_BORDER
                       );

    cb->SetValue(wxT("Dot Dash"));
    cb->Enable(false);

    rowSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );
*/
    //
    // Pen Selector Combo with gfx
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(this,-1,wxT("OwnerDrawnComboBox with Custom Paint Items and Button Placing:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    // When defining derivative class for callbacks, we need
    // to use two-stage creation (or redefine the common wx
    // constructor).
    odc = new wxPenStyleComboBox();
    odc->Create(this,wxID_ANY,wxEmptyString,
                wxDefaultPosition, wxDefaultSize,
                arrItems,
                wxCB_READONLY //wxNO_BORDER | wxCB_READONLY
               );

    //m_odc->SetCustomPaintWidth( 60 );
    odc->SetSelection(0);
    odc->SetButtonPosition(-2, // width adjustment
                           -6, // height adjustment
                           wxLEFT, // side
                           2 // horizontal spacing
                          );

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    rowSizer->AddStretchSpacer(1);
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    //
    // List View Combo
    //

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(this,-1,wxT("List View wxComboControl:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    rowSizer->Add( new wxStaticText(this,-1,wxT("Tree Ctrl wxComboControl:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    cc = new wxPGComboControl(this,2,wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize);

    cc->SetPopupMinWidth(300);

    ListViewComboPopup* iface = new ListViewComboPopup(cc);
    cc->SetPopup(iface);

    iface->AddSelection( wxT("Cabbage") );
    iface->AddSelection( wxT("Potato") );
    iface->AddSelection( wxT("Onion") );
    iface->AddSelection( wxT("Carrot") );
    iface->AddSelection( wxT("Cauliflower") );
    iface->AddSelection( wxT("Bean") );
    iface->AddSelection( wxT("Raddish") );
    iface->AddSelection( wxT("Banana") );
    iface->AddSelection( wxT("Apple") );
    iface->AddSelection( wxT("Orange") );
    iface->AddSelection( wxT("Kiwi") );
    iface->AddSelection( wxT("Strawberry") );
    iface->AddSelection( wxT("Cucumber") );
    iface->AddSelection( wxT("Blackberry") );
    iface->AddSelection( wxT("Melon") );
    iface->AddSelection( wxT("Cherry") );
    iface->AddSelection( wxT("Pea") );
    iface->AddSelection( wxT("Pear") );

    rowSizer->Add( cc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    //rowSizer->AddStretchSpacer(1);
    //colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    //
    // Tree Ctrl Combo
    //

    cc = new wxPGComboControl(this,wxID_ANY,wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize);

    // Set popup interface right away, otherwise some of the calls
    // below may fail
    TreeCtrlComboPopup* tcPopup = new TreeCtrlComboPopup(cc);
    cc->SetPopup(tcPopup);

    // Add items using wxTreeCtrl methods directly
    wxTreeItemId rootId = tcPopup->AddRoot(wxT("<hidden_root>"));

    wxTreeItemId groupId;

    groupId = tcPopup->AppendItem(rootId,wxT("Controls"));
    tcPopup->AppendItem(groupId,wxT("wxButton"));
    tcPopup->AppendItem(groupId,wxT("wxCheckBox"));
    tcPopup->AppendItem(groupId,wxT("wxListCtrl"));
    tcPopup->AppendItem(groupId,wxT("wxStaticBox"));
    tcPopup->AppendItem(groupId,wxT("wxStaticText"));
    tcPopup->AppendItem(groupId,wxT("wxTextCtrl"));
    tcPopup->AppendItem(groupId,wxT("wxTreeCtrl"));
    groupId = tcPopup->AppendItem(rootId,wxT("Dialogs"));
    tcPopup->AppendItem(groupId,wxT("wxDirDialog"));
    tcPopup->AppendItem(groupId,wxT("wxFileDialog"));
    tcPopup->AppendItem(groupId,wxT("wxWizard"));

    cc->SetValue(wxT("wxStaticBox"));

    // Move button to left - it makes more sense for a tree ctrl
    cc->SetButtonPosition(0, // width adjustment
                          0, // height adjustment
                          wxLEFT, // side
                          0 // horizontal spacing
                         );

    rowSizer->Add( cc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    //rowSizer->AddStretchSpacer(1);
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );


#if wxUSE_IMAGE
    //
    // Custom Dropbutton Bitmap
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(this,-1,wxT("OwnerDrawnComboBox with some lame custom dropbutton graphics:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    odc = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     (long)0 // wxCB_SORT // wxNO_BORDER | wxCB_READONLY
                                     );

    wxPGOwnerDrawnComboBox* odc2;
    odc2 = new wxPGOwnerDrawnComboBox(this,wxID_ANY,wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize,
                                      arrItems,
                                      (long)0 // wxCB_SORT // wxNO_BORDER | wxCB_READONLY
                                      );

    // Load images from disk
    wxImage imgNormal(wxT("dropbutn.png"));
    wxImage imgPressed(wxT("dropbutp.png"));
    wxImage imgHover(wxT("dropbuth.png"));

    if ( imgNormal.Ok() && imgPressed.Ok() && imgHover.Ok() )
    {
        wxBitmap bmpNormal(imgNormal);
        wxBitmap bmpPressed(imgPressed);
        wxBitmap bmpHover(imgHover);
        odc->SetButtonBitmaps(bmpNormal,false,bmpPressed,bmpHover);
        odc2->SetButtonBitmaps(bmpNormal,true,bmpPressed,bmpHover);
    }
    else
        wxLogError(wxT("Dropbutton images not found"));

    //odc2->SetButtonPosition(0, // width adjustment
    //                        0, // height adjustment
    //                        wxLEFT, // side
    //                        0 // horizontal spacing
    //                       );

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    rowSizer->Add( odc2, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );
#endif

    //
    // wxComboControl with totally custom button action
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(this,-1,wxT("wxComboControl with custom button action:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

#ifdef __WXMSW__
    wxString fileName(wxT("C:\\Windows\\system.ini"));
#else
    wxString fileName(wxT(""));
#endif

    wxFileSelectorCombo* fsc;
    fsc = new wxFileSelectorCombo(this,wxID_ANY,fileName,
                                  wxDefaultPosition, wxDefaultSize,
                                  (long)0
                                 );

    rowSizer->Add( fsc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    topRowSizer->Add( colSizer, 1, wxALL, 2 );

    topRowSizer->Add( m_logWin, 1, wxEXPAND|wxALL, 5 );
/*
    wxPropertyGrid* pg = new wxPropertyGrid(this, PGID,
        wxDefaultPosition, wxDefaultSize,
        wxPG_BOLD_MODIFIED |
        wxPG_SPLITTER_AUTO_CENTER |
        wxPG_AUTO_SORT |
        //wxPG_HIDE_MARGIN | wxPG_STATIC_SPLITTER |
        //wxPG_TOOLTIPS | // Right now, a bit messy.
        //wxPG_NOCATEGORIES |
        wxTAB_TRAVERSAL |
        wxSUNKEN_BORDER
       );

    pg->Append( wxStringProperty(wxT("Some String"),wxPG_LABEL,wxT("This is for testing wxPropertyGrid in a modal dialog")) );
    pg->Append( wxSystemColourProperty(wxT("Some Colour")) );
    pg->Append( wxColourProperty(wxT("Some Colour 2")) );

    colSizer->Add( pg, 0, wxEXPAND|wxALL, 5 );
*/

    topSizer->Add( topRowSizer, 1, wxEXPAND );

    // Close button
    /*
    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    rowSizer->Add( new wxButton(this,40,wxT("&Close")), 0, wxALIGN_RIGHT );
    topSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );
    */

    SetSizer( topSizer );
    topSizer->SetSizeHints( this );

    Centre();
}

SampleFrame::~SampleFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}

void FormMain::CustomComboSampleDialog ( wxCommandEvent& )
{
    SampleFrame* frame = new SampleFrame( this );

    frame->ShowModal();
}

// -----------------------------------------------------------------------

FormMain::~FormMain()
{
}

// -----------------------------------------------------------------------

IMPLEMENT_APP(cxApplication)

bool cxApplication::OnInit()
{
    //wxLocale Locale;
    //Locale.Init(wxLANGUAGE_FINNISH);

	FormMain* frame = Form1 = new FormMain( wxT("wxPropertyGrid Sample"), wxPoint(0,0), wxSize(300,500) );
	//SetTopWindow(frame);
	frame->Show(true);

	return true;
}

// -----------------------------------------------------------------------

void FormMain::OnIdle( wxIdleEvent& event )
{
    /*
    // This code is useful for debugging focus problems
    static wxWindow* last_focus = (wxWindow*) NULL;

    wxWindow* cur_focus = ::wxWindow::FindFocus();

    if ( cur_focus != last_focus )
    {
        const wxChar* class_name = wxT("<none>");
        if ( cur_focus )
            class_name = cur_focus->GetClassInfo()->GetClassName();
        last_focus = cur_focus;
        wxLogDebug( wxT("FOCUSED: %s %X"),
            class_name,
            (unsigned int)cur_focus);
    }
    */

    event.Skip();
}

// -----------------------------------------------------------------------
