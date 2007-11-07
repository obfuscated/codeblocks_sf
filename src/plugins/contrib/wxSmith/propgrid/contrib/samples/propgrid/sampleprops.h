/////////////////////////////////////////////////////////////////////////////
// Name:        sampleprops.h
// Purpose:     wxPropertyGrid Sample Properties Header
// Author:      Jaakko Salli
// Modified by:
// Created:     Mar-05-2006
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_PROPGRID_SAMPLEPROPS_H
#define WX_PROPGRID_SAMPLEPROPS_H

#ifndef __WXPYTHON__
    // This decl is what you would use in a normal app
    #define wxPG_EXTRAS_DECL
#else
    #define wxPG_EXTRAS_DECL     WXDLLIMPEXP_PG
#endif

//
// Additional properties that can be useful as more than just a demonstration.
//

#ifndef SWIG

#if wxMINOR_VERSION < 7 || ( wxMINOR_VERSION == 7 && wxRELEASE_NUMBER < 1 )
// NOTE: Since wxWidgets at this point doesn't have wxArrayDouble, we have
//   to create it ourself, using wxObjArray model.
WX_DECLARE_OBJARRAY_WITH_DECL(double, wxArrayDouble, class wxPG_EXTRAS_DECL);
#endif

WX_PG_DECLARE_VALUE_TYPE_VOIDP_WITH_DECL(wxArrayDouble,wxPG_EXTRAS_DECL)

WX_PG_DECLARE_PROPERTY_WITH_DECL(wxArrayDoubleProperty,const wxArrayDouble&,wxArrayDouble(),wxPG_EXTRAS_DECL)

WX_PG_DECLARE_VALUE_TYPE_WITH_DECL(wxFontData,wxPG_EXTRAS_DECL)
WX_PG_DECLARE_VALUE_TYPE_VOIDP_WITH_DECL(wxPoint,wxPG_EXTRAS_DECL)
WX_PG_DECLARE_VALUE_TYPE_VOIDP_WITH_DECL(wxSize,wxPG_EXTRAS_DECL)
WX_PG_DECLARE_VALUE_TYPE_VOIDP_WITH_DECL(wxLongLong,wxPG_EXTRAS_DECL)



// wxPaintedEnumPropertyClass - wxEnumProperty with totally custom painted list items.
class wxPaintedEnumPropertyClass : public wxEnumPropertyClass
{
public:
    wxPaintedEnumPropertyClass(const wxString& label, const wxString& name,
                          const wxArrayString& labels, const wxArrayInt& values = wxArrayInt(),
                          int value = 0);

    virtual void OnCustomPaint(wxDC& dc, const wxRect& rect, wxPGPaintData& paintdata);
    virtual wxSize GetImageSize() const;
};


#endif


WX_PG_DECLARE_PROPERTY_WITH_DECL(wxFontDataProperty,const wxFontData&,wxFontData(),wxPG_EXTRAS_DECL)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxPointProperty,const wxPoint&,wxPoint(0,0),wxPG_EXTRAS_DECL)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxSizeProperty,const wxSize&,wxSize(0,0),wxPG_EXTRAS_DECL)
WX_PG_DECLARE_PROPERTY_WITH_DECL(wxLongLongProperty,const wxLongLong&,wxLongLong(0),wxPG_EXTRAS_DECL)
WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(wxDirsProperty,wxPG_EXTRAS_DECL)


#endif // WX_PROPGRID_SAMPLEPROPS_H
