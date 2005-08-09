/////////////////////////////////////////////////////////////////////////////
// Name:        propertygridsample.h
// Purpose:     wxPropertyGrid sample
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _PROPGRIDSAMPLE_H_
#define _PROPGRIDSAMPLE_H_

// -----------------------------------------------------------------------
// Declare custom user properties.
// -----------------------------------------------------------------------

WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty)

WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY(wxMyColourProperty2)

// -----------------------------------------------------------------------

WX_PG_DECLARE_VALUE_TYPE(wxFontData)

WX_PG_DECLARE_PROPERTY(wxFontDataProperty,const wxFontData&,wxFontData())

// -----------------------------------------------------------------------

WX_PG_DECLARE_CUSTOM_FLAGS_PROPERTY(wxTestCustomFlagsProperty)

WX_PG_DECLARE_CUSTOM_ENUM_PROPERTY(wxTestCustomEnumProperty)

// -----------------------------------------------------------------------

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxPoint)

WX_PG_DECLARE_PROPERTY(wxPointProperty,const wxPoint&,wxPoint(0,0))

// -----------------------------------------------------------------------

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxSize)

WX_PG_DECLARE_PROPERTY(wxSizeProperty,const wxSize&,wxSize(0,0))

// -----------------------------------------------------------------------

class wxVector3f
{
public:
    wxVector3f()
    {
        x = y = z = 0.0;
    }
    wxVector3f( double x, double y, double z )
    {
        x = x; y = y; z = z;
    }

    double x, y, z;
};

bool operator == (const wxVector3f& a, const wxVector3f& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxVector3f)

WX_PG_DECLARE_PROPERTY(wxVectorProperty,const wxVector3f&,wxVector3f())

// -----------------------------------------------------------------------

class wxTriangle
{
public:
    wxVector3f a, b, c;
};

bool operator == (const wxTriangle& a, const wxTriangle& b)
{
    return (a.a == b.a && a.b == b.b && a.c == b.c);
}

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxTriangle)

WX_PG_DECLARE_PROPERTY(wxTriangleProperty,const wxTriangle&,wxTriangle())

// -----------------------------------------------------------------------

WX_PG_DECLARE_PROPERTY(wxAdvImageFileProperty,const wxString&,wxEmptyString)

// -----------------------------------------------------------------------

WX_PG_DECLARE_ARRAYSTRING_PROPERTY(wxDirsProperty)

// -----------------------------------------------------------------------

// NOTE: Since wxWidgets at this point doesn't have wxArrayDouble, we have
//   to create it ourself, using wxObjArray model.
WX_DECLARE_OBJARRAY(double, wxArrayDouble);

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxArrayDouble)

WX_PG_DECLARE_PROPERTY(wxArrayDoubleProperty,const wxArrayDouble&,*((wxArrayDouble*)NULL))

// -----------------------------------------------------------------------

// Change this to 0 to see it without the wxPropertyGridManager stuff.
#define TESTING_WXPROPERTYGRIDADV       1

// -----------------------------------------------------------------------

// Note that outside Msw we use radiobox instead of tabctrl
#if defined(__WXMSW__)
# define wxPGS_USE_TABCTRL
#endif

#ifdef wxPGS_USE_TABCTRL
# include <wx/tabctrl.h>
#else
# include <wx/radiobox.h>
#endif

//#undef wxPGS_USE_TABCTRL

// -----------------------------------------------------------------------

class FormMain: public wxFrame
{
  private:

  public:
    FormMain(const wxString& title, const wxPoint& pos, const wxSize& size );
	~FormMain();

#ifdef wxPGS_USE_TABCTRL
    wxTabCtrl*      m_tabctrl;
#else
    wxRadioBox*     m_radiobox;
#endif

#if TESTING_WXPROPERTYGRIDADV
    wxPropertyGridManager*  m_pPropGridMan;
#else
# define m_pPropGridMan m_proped
#endif
    wxPropertyGrid*     m_proped;

    wxTextCtrl*     m_tcPropLabel;

    wxPGConstants   m_combinedFlags;

    wxMenuItem*     m_itemCatColours;
    wxMenuItem*     m_itemCompact;
    wxMenuItem*     m_itemFreeze;
    wxMenuItem*     m_itemEnable;

    wxVariant       m_storedValues;

    void PopulateWithStandardItems ();
    void PopulateWithExamples ();
    void PopulateWithLibraryConfig ();

    void OnButtonClick ( wxCommandEvent& event );
    void OnLabelTextChange ( wxCommandEvent& event );

    void OnColourScheme ( wxCommandEvent& event );

    void OnInsertPropClick ( wxCommandEvent& event );
    void OnAppendPropClick ( wxCommandEvent& event );
    void OnClearClick ( wxCommandEvent& event );
    void OnAppendCatClick ( wxCommandEvent& event );
    void OnInsertCatClick ( wxCommandEvent& event );
    void OnDelPropClick ( wxCommandEvent& event );

    void OnEnableDisable ( wxCommandEvent& event );
    void OnSetAsHideable ( wxCommandEvent& event );
    void OnClearModifyStatusClick ( wxCommandEvent& event );
    void OnFreezeClick ( wxCommandEvent& event );
    void OnDumpList ( wxCommandEvent& event );
    void OnCatColours ( wxCommandEvent& event );
    void OnCompact ( wxCommandEvent& event );
    void OnMisc ( wxCommandEvent& event );
    void OnPopulateClick ( wxCommandEvent& event );

    void OnChangeFlagsPropItemsClick ( wxCommandEvent& event );

    void OnSaveToFileClick ( wxCommandEvent& event );
    void OnLoadFromFileClick ( wxCommandEvent& event );

#ifdef wxPGS_USE_TABCTRL
    void OnTabChange ( wxTabEvent& event );
#else
    void OnRadioBoxChange ( wxCommandEvent& event );
#endif

    void OnIterate1Click ( wxCommandEvent& event );
    void OnIterate2Click ( wxCommandEvent& event );
    void OnIterate3Click ( wxCommandEvent& event );
    void OnIterate4Click ( wxCommandEvent& event );

    void OnPropertyGridChange ( wxPropertyGridEvent& event );
    void OnPropertyGridSelect ( wxPropertyGridEvent& event );
    void OnPropertyGridHighlight ( wxPropertyGridEvent& event );
    void OnPropertyGridItemRightClick ( wxPropertyGridEvent& event );
    void OnPropertyGridPageChange ( wxPropertyGridEvent& event );

    void OnAbout ( wxCommandEvent& event );

    void OnMove ( wxMoveEvent& event );
    void OnResize ( wxSizeEvent& event );
    void OnPaint ( wxPaintEvent& event );

    void OnIdle( wxIdleEvent& event );

    void RunTests ();

    void CustomComboSampleDialog( wxCommandEvent& event );

    DECLARE_EVENT_TABLE()
};

// -----------------------------------------------------------------------

class cxApplication : public wxApp
{
  private:
  public:
    FormMain    *Form1;
    virtual bool OnInit();
};

DECLARE_APP(cxApplication)

// -----------------------------------------------------------------------

#endif // _PROPGRIDSAMPLE_H_
