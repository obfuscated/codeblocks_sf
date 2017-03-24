/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxscolourproperty.h"

#include <logmanager.h>
#include <wx/settings.h>
#include <wx/colordlg.h>
#include <wx/dc.h>
#if wxCHECK_VERSION(3, 0, 0)
#include <wx/wxcrtvararg.h>
#endif
#include "../wxsflags.h"

#if wxPG_USING_WXOWNERDRAWNCOMBOBOX || wxCHECK_VERSION(3, 0, 0)
    #include <wx/odcombo.h>
#else
    #include <wx/propgrid/odcombo.h>
#endif


using namespace wxsFlags;


// Creating custom colour property.
// This is based on wxSystemColourProperty from propgrid

namespace
{
#if wxCHECK_VERSION(3, 1, 0)
    static const char* wxsColourLabels[] = {
#else
    static const wxChar* wxsColourLabels[] = {
#endif // wxCHECK_VERSION
        _("Default"),
        _("Custom"),
        _("Scrollbar"),
        _("Desktop"),
        _("Active window caption"),
        _("Inactive window caption"),
        _("Menu background"),
        _("Window background"),
        _("Window frame"),
        _("Menu text"),
        _("Text in window"),
        _("Text in window caption"),
        _("Active window border"),
        _("Inactive window border"),
        _("Background for MDI apps"),
        _("Selected item"),
        _("Text of selected item"),
        _("Face of button"),
        _("Edge of button"),
        _("Grayed (disabled) text"),
        _("Text on buttons"),
        _("Text of inactive caption"),
        _("Highlight colour for buttons"),
        _("Dark shadow for 3D items"),
        _("Light for 3D items"),
        _("Tooltip text"),
        _("Tooltip background"),
        _("Listbox"),
        _("Hot light"),
        _("Gradient of active caption"),
        _("Gradnent of inactive caption"),
        _("Selected menu item"),
        _("Menu bar"),
        0
    };

    const int wxsColourCount = (sizeof(wxsColourLabels) / sizeof(wxsColourLabels[0])) - 1;

    static const long wxsColourValues[] = {
        wxsCOLOUR_DEFAULT,
        wxPG_COLOUR_CUSTOM,
        wxSYS_COLOUR_SCROLLBAR,
        wxSYS_COLOUR_DESKTOP,
        wxSYS_COLOUR_ACTIVECAPTION,
        wxSYS_COLOUR_INACTIVECAPTION,
        wxSYS_COLOUR_MENU,
        wxSYS_COLOUR_WINDOW,
        wxSYS_COLOUR_WINDOWFRAME,
        wxSYS_COLOUR_MENUTEXT,
        wxSYS_COLOUR_WINDOWTEXT,
        wxSYS_COLOUR_CAPTIONTEXT,
        wxSYS_COLOUR_ACTIVEBORDER,
        wxSYS_COLOUR_INACTIVEBORDER,
        wxSYS_COLOUR_APPWORKSPACE,
        wxSYS_COLOUR_HIGHLIGHT,
        wxSYS_COLOUR_HIGHLIGHTTEXT,
        wxSYS_COLOUR_BTNFACE,
        wxSYS_COLOUR_BTNSHADOW,
        wxSYS_COLOUR_GRAYTEXT,
        wxSYS_COLOUR_BTNTEXT,
        wxSYS_COLOUR_INACTIVECAPTIONTEXT,
        wxSYS_COLOUR_BTNHIGHLIGHT,
        wxSYS_COLOUR_3DDKSHADOW,
        wxSYS_COLOUR_3DLIGHT,
        wxSYS_COLOUR_INFOTEXT,
        wxSYS_COLOUR_INFOBK,
        wxSYS_COLOUR_LISTBOX,
        wxSYS_COLOUR_HOTLIGHT,
        wxSYS_COLOUR_GRADIENTACTIVECAPTION,
        wxSYS_COLOUR_GRADIENTINACTIVECAPTION,
        wxSYS_COLOUR_MENUHILIGHT,
        wxSYS_COLOUR_MENUBAR,
        0
    };

    static wxPGChoices gs_wxsColourProperty_choicesCache;

    // This is based on the code of wxSystemColourProperty in advprops.h|cpp of wxPropertyGrid
    class wxsMyColourPropertyClass : public wxEnumProperty
    {
        WX_PG_DECLARE_PROPERTY_CLASS(wxsMyColourPropertyClass)
    public:

        wxsMyColourPropertyClass( const wxString& label = wxEmptyString,
                                const wxString& name = wxPG_LABEL,
                                const wxColourPropertyValue& value = wxColourPropertyValue(wxsCOLOUR_DEFAULT,*wxWHITE) );
        ~wxsMyColourPropertyClass();

        virtual void OnSetValue();
        virtual bool IntToValue( wxVariant& variant, int number, int argFlags = 0 ) const;

        /** Override in derived class to customize how colours are printed as strings.
        */
        virtual wxString ColourToString( const wxColour& col, int index
#if wxCHECK_VERSION(3, 0, 0)
                                        , int argFlags = 0
#endif
                                         ) const;

        /** Returns index of entry that triggers colour picker dialog
            (default is last).
        */
        virtual int GetCustomColourIndex() const;

#if wxCHECK_VERSION(3, 0, 0)
        virtual wxString ValueToString( wxVariant& value, int argFlags = 0 ) const;
        virtual bool StringToValue( wxVariant& variant,
                                    const wxString& text,
                                    int argFlags = 0 ) const;
        virtual bool OnEvent( wxPropertyGrid* propgrid,
                              wxWindow* primary, wxEvent& event );
        virtual bool DoSetAttribute( const wxString& name, wxVariant& value );
        virtual wxSize OnMeasureImage( int item ) const;
        virtual void OnCustomPaint( wxDC& dc,
                                    const wxRect& rect, wxPGPaintData& paintdata );
#else
        WX_PG_DECLARE_BASIC_TYPE_METHODS()
        WX_PG_DECLARE_EVENT_METHODS()
        WX_PG_DECLARE_ATTRIBUTE_METHODS()

        WX_PG_DECLARE_CUSTOM_PAINT_METHODS()
#endif
        // Helper function to show the colour dialog
        bool QueryColourFromUser( wxVariant& variant ) const;

        /** Default is to use wxSystemSettings::GetColour(index). Override to use
            custom colour tables etc.
        */
        virtual wxColour GetColour( int index ) const;

        wxColourPropertyValue GetVal( const wxVariant* pVariant = NULL ) const;

    protected:

        void Init( int type, const wxColour& colour );

        // Utility functions for internal use
        virtual wxVariant DoTranslateVal( wxColourPropertyValue& v ) const;
        wxVariant TranslateVal( wxColourPropertyValue& v ) const
        {
            return DoTranslateVal( v );
        }
        wxVariant TranslateVal( int type, const wxColour& colour ) const
        {
            wxColourPropertyValue v(type, colour);
            return DoTranslateVal( v );
        }

        // Translates colour to a int value, return wxNOT_FOUND if no match.
        int ColToInd( const wxColour& colour ) const;
    };


    WX_PG_IMPLEMENT_PROPERTY_CLASS(wxsMyColourPropertyClass,wxEnumProperty,
                                   wxColourPropertyValue,const wxColourPropertyValue&,Choice)


    void wxsMyColourPropertyClass::Init( int type, const wxColour& colour )
    {
        wxColourPropertyValue cpv;

        if ( colour.Ok() )
            cpv.Init( type, colour );
        else
            cpv.Init( type, *wxWHITE );

        m_flags |= wxPG_PROP_STATIC_CHOICES; // Colour selection cannot be changed.

#if wxCHECK_VERSION(3, 0, 0)
        m_value << cpv;
#else
        m_value = wxColourPropertyValueToVariant(cpv);
#endif

        OnSetValue();
    }

    wxsMyColourPropertyClass::wxsMyColourPropertyClass( const wxString& label, const wxString& name,
        const wxColourPropertyValue& value )
        : wxEnumProperty( label,
                          name,
                          wxsColourLabels,
                          wxsColourValues,
                          &gs_wxsColourProperty_choicesCache )
    {
        Init( value.m_type, value.m_colour );
    }

    wxsMyColourPropertyClass::~wxsMyColourPropertyClass() { }


    wxColourPropertyValue wxsMyColourPropertyClass::GetVal( const wxVariant* pVariant ) const
    {
        if ( !pVariant )
            pVariant = &m_value;

        if ( pVariant->IsNull() )
            return wxColourPropertyValue(wxPG_COLOUR_UNSPECIFIED, wxColour());

#if wxCHECK_VERSION(3, 0, 0)
    if ( pVariant->GetType() == wxS("wxColourPropertyValue") )
    {
        wxColourPropertyValue v;
        v << *pVariant;
        return v;
    }
#else
        wxColourPropertyValue* v1 = &wxColourPropertyValueFromVariant(*pVariant);
        if ( v1 )
            return *v1;

        wxColour* pCol = wxGetVariantCast(*pVariant, wxColour);
#endif
        wxColour col;

#if wxCHECK_VERSION(3, 0, 0)
    bool variantProcessed = true;

    if ( pVariant->GetType() == wxS("wxColour*") )
    {
        wxColour* pCol = wxStaticCast(pVariant->GetWxObjectPtr(), wxColour);
        col = *pCol;
    }
    else if ( pVariant->GetType() == wxS("wxColour") )
    {
        col << *pVariant;
    }
    else if ( pVariant->GetType() == wxArrayInt_VariantType )
    {
        // This code is mostly needed for wxPython bindings, which
        // may offer tuple of integers as colour value.
        wxArrayInt arr;
        arr << *pVariant;

        if ( arr.size() >= 3 )
        {
            int r, g, b;
            int a = 255;

            r = arr[0];
            g = arr[1];
            b = arr[2];
            if ( arr.size() >= 4 )
                a = arr[3];

            col = wxColour(r, g, b, a);
        }
        else
        {
            variantProcessed = false;
        }
    }
    else
    {
        variantProcessed = false;
    }

    if ( !variantProcessed )
        return wxColourPropertyValue(wxPG_COLOUR_UNSPECIFIED, wxColour());
#else
        if ( pCol )
            col = *pCol;
        else
            col << *pVariant;
#endif
        wxColourPropertyValue v2( wxPG_COLOUR_CUSTOM, col );

        int colInd = ColToInd(col);
        if ( colInd != wxNOT_FOUND )
            v2.m_type = colInd;

        return v2;
    }

    wxVariant wxsMyColourPropertyClass::DoTranslateVal( wxColourPropertyValue& v ) const
    {
#if wxCHECK_VERSION(3, 0, 0)
    wxVariant variant;
    variant << v;
    return variant;
#else
        return wxColourPropertyValueToVariant(v);
#endif
    }

    int wxsMyColourPropertyClass::ColToInd( const wxColour& colour ) const
    {
        size_t i;
        size_t i_max = m_choices.GetCount();

        for ( i=0; i<i_max; i++ )
        {
            int ind = m_choices[i].GetValue();

            if ( colour == GetColour(ind) )
            {
                return ind;
            }
        }
        return wxNOT_FOUND;
    }


    static inline wxColour wxColourFromPGLong( long col )
    {
        return wxColour((col&0xFF),((col>>8)&0xFF),((col>>16)&0xFF));
    }


    void wxsMyColourPropertyClass::OnSetValue()
    {
        // Convert from generic wxobject ptr to wxPGVariantDataColour
#if wxCHECK_VERSION(3, 0, 0)
    if ( m_value.GetType() == wxS("wxColour*") )
    {
        wxColour* pCol = wxStaticCast(m_value.GetWxObjectPtr(), wxColour);
        m_value << *pCol;
    }
#else
        if ( wxPGIsVariantType(m_value, wxobject) )
        {
            wxASSERT( m_value.IsValueKindOf(CLASSINFO(wxColour)) );
            wxColour* pCol = (wxColour*) m_value.GetWxObjectPtr();
            m_value << *pCol;
        }
#endif

        wxColourPropertyValue val = GetVal(&m_value);

        if ( val.m_type == wxPG_COLOUR_UNSPECIFIED )
        {
            m_value.MakeNull();
            return;
        }
        else
        {

            if ( val.m_type < wxPG_COLOUR_WEB_BASE )
                val.m_colour = GetColour( val.m_type );

            m_value = TranslateVal(val);
        }

#if wxCHECK_VERSION(3, 0, 0)
    int ind = wxNOT_FOUND;

    if ( m_value.GetType() == wxS("wxColourPropertyValue") )
    {
        wxColourPropertyValue cpv;
        cpv << m_value;
        wxColour col = cpv.m_colour;

        if ( !col.IsOk() )
        {
            SetValueToUnspecified();
            SetIndex(wxNOT_FOUND);
            return;
        }

        if ( cpv.m_type < wxPG_COLOUR_WEB_BASE )
        {
            ind = GetIndexForValue(cpv.m_type);
        }
        else
        {
            cpv.m_type = wxPG_COLOUR_CUSTOM;
            ind = GetCustomColourIndex();
        }
    }
    else
    {
        wxColour col;
        col << m_value;

        if ( !col.IsOk() )
        {
            SetValueToUnspecified();
            SetIndex(wxNOT_FOUND);
            return;
        }

        ind = ColToInd(col);

        if ( ind == wxNOT_FOUND  )
            ind = GetCustomColourIndex();
    }
#else
        wxColourPropertyValue* pCpv = &wxColourPropertyValueFromVariant(m_value);
        wxColour col;
        if ( pCpv )
            col = pCpv->m_colour;
        else
            col << m_value;

        if ( !col.Ok() )
        {
            SetValueToUnspecified();
            SetIndex(wxNOT_FOUND);
            return;
        }

        int ind;

        if ( pCpv )
        {
            if ( pCpv->m_type < wxPG_COLOUR_WEB_BASE )
            {
                ind = GetIndexForValue(pCpv->m_type);
            }
            else if (pCpv->m_type == wxPG_COLOUR_CUSTOM)
            {
                ind = GetCustomColourIndex();
            }
            else
            {
                pCpv->m_type = wxsCOLOUR_DEFAULT;
                ind = 0;
            }
        }
        else
        {
            ind = ColToInd(col);

            if ( ind == wxNOT_FOUND )
                ind = GetCustomColourIndex();
        }
#endif
        SetIndex(ind);
    }


    wxColour wxsMyColourPropertyClass::GetColour( int index ) const
    {
        return wxSystemSettings::GetColour( (wxSystemColour)index );
    }

    wxString wxsMyColourPropertyClass::ColourToString( const wxColour& col,
#if wxCHECK_VERSION(3, 0, 0)
                                                       int index,
                                                       int argFlags ) const
#else
                                                       int index ) const
#endif
    {
        if ( index == wxNOT_FOUND )
        {
#if wxCHECK_VERSION(3, 0, 0)
        if ( (argFlags & wxPG_FULL_VALUE) ||
             GetAttributeAsLong(wxPG_COLOUR_HAS_ALPHA, 0) )
            return wxString::Format(wxS("(%i,%i,%i,%i)"),
                                    (int)col.Red(),
                                    (int)col.Green(),
                                    (int)col.Blue(),
                                    (int)col.Alpha());
        else
#endif
            return wxString::Format(wxT("(%i,%i,%i)"),
                                    (int)col.Red(),
                                    (int)col.Green(),
                                    (int)col.Blue());
        }
        else
            return m_choices.GetLabel(index);
    }

#if wxCHECK_VERSION(3, 0, 0)
    wxString wxsMyColourPropertyClass::ValueToString( wxVariant& value,
                                                    int argFlags ) const
    {
        wxColourPropertyValue val = GetVal(&value);

        int index;

        if ( argFlags & wxPG_VALUE_IS_CURRENT )
        {
            // GetIndex() only works reliably if wxPG_VALUE_IS_CURRENT flag is set,
            // but we should use it whenever possible.
            index = GetIndex();

            // If custom colour was selected, use invalid index, so that
            // ColourToString() will return properly formatted colour text.
            if ( index == GetCustomColourIndex() )
                index = wxNOT_FOUND;
        }
        else
        {
            index = m_choices.Index(val.m_type);
        }

        return ColourToString(val.m_colour, index, argFlags);
    }
#else
    wxString wxsMyColourPropertyClass::GetValueAsString(cb_unused int argFlags) const
    {
        wxColourPropertyValue val = GetVal();

        int ind = GetIndex();

        // Always show custom colour for textctrl-editor
        if (val.m_type == wxPG_COLOUR_CUSTOM)
        {
            ind = wxNOT_FOUND;
        }

        if (val.m_type == wxsCOLOUR_DEFAULT)
        {
            return wxsColourLabels[ind];
        }

        return ColourToString(val.m_colour, ind);
    }
#endif

    wxSize wxsMyColourPropertyClass::OnMeasureImage( int ) const
    {
        return wxPG_DEFAULT_IMAGE_SIZE;
    }


    int wxsMyColourPropertyClass::GetCustomColourIndex() const
    {
        for(size_t i = 0; m_choices.GetCount();++i)
        {
            if(m_choices[i].GetValue() == wxPG_COLOUR_CUSTOM)
                return i;
        }
        return -1;
    }


    bool wxsMyColourPropertyClass::QueryColourFromUser( wxVariant& variant ) const
    {
#if wxCHECK_VERSION(3, 0, 0)
        wxASSERT( m_value.GetType() != wxPG_VARIANT_TYPE_STRING );
#else
        wxASSERT( m_value.GetType() != wxT("string") );
#endif
        bool res = false;

        wxPropertyGrid* propgrid = GetGrid();
        wxASSERT( propgrid );

        // Must only occur when user triggers event
#if wxCHECK_VERSION(3, 0, 0)
        if ( !(propgrid->GetInternalFlags() & wxPG_FL_IN_HANDLECUSTOMEDITOREVENT) )
#else
        if ( !(propgrid->GetInternalFlags() & wxPG_FL_IN_ONCUSTOMEDITOREVENT) )
#endif
            return res;

        wxColourPropertyValue val = GetVal();

        val.m_type = wxPG_COLOUR_CUSTOM;

        wxColourData data;
        data.SetChooseFull(true);
        data.SetColour(val.m_colour);
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
            val.m_colour = retData.GetColour();

            variant = DoTranslateVal(val);

            SetValueInEvent(variant);

            res = true;
        }

        return res;
    }


    bool wxsMyColourPropertyClass::IntToValue( wxVariant& variant, int number, int WXUNUSED(argFlags) ) const
    {
        int index = number;
#if wxCHECK_VERSION(3, 0, 0)
        int type = m_choices.GetValue(index);
#else
        int type = GetValueForIndex(index);
#endif
        if ( type == wxPG_COLOUR_CUSTOM )
        {
            QueryColourFromUser(variant);
        }
        else
        {
            variant = TranslateVal( type, GetColour(type) );
        }

        return true;
    }

    // Need to do some extra event handling.
    bool wxsMyColourPropertyClass::OnEvent( wxPropertyGrid* propgrid,
                                          wxWindow* WXUNUSED(primary),
                                          wxEvent& event )
    {
        bool askColour = false;

        if ( propgrid->IsMainButtonEvent(event) )
        {
            askColour = true;
        }
        else if ( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
        {
            // Must override index detection since at this point GetIndex()
            // will return old value.
#if wxCHECK_VERSION(3, 0, 0)
            wxOwnerDrawnComboBox* cb =
                static_cast<wxOwnerDrawnComboBox*>(propgrid->GetEditorControl());
#else
            wxPGOwnerDrawnComboBox* cb =
                (wxPGOwnerDrawnComboBox*)propgrid->GetEditorControl();
#endif
            if ( cb )
            {
                int index = cb->GetSelection();

                if ( index == GetCustomColourIndex() )
                    askColour = true;
            }
        }

        if ( askColour && !propgrid->WasValueChangedInEvent() )
        {
            // We need to handle button click in case editor has been
            // switched to one that has wxButton as well.
            wxVariant variant;
            if ( QueryColourFromUser(variant) )
                return true;
        }
        return false;
    }

    void wxsMyColourPropertyClass::OnCustomPaint( wxDC& dc, const wxRect& rect,
                                                wxPGPaintData& paintdata )
    {
        int value;

        if ( paintdata.m_choiceItem >= 0 && paintdata.m_choiceItem < (int)(GetItemCount()) )
        {
            int index = paintdata.m_choiceItem;
            value = wxsColourValues[index];
        }
        else if ( !(m_flags & wxPG_PROP_UNSPECIFIED) )
        {
            value = GetVal().m_type;
        }
        else
        {
            dc.SetBrush ( *wxWHITE );
            dc.DrawRectangle ( rect );
            return;
        }

        if ( value == wxsCOLOUR_DEFAULT )
        {
            dc.SetBrush(*wxWHITE);
            dc.DrawRectangle(rect);
#if wxCHECK_VERSION(3, 0, 0)
            dc.SetBrush(wxBrush(*wxBLACK,wxHATCHSTYLE_BDIAGONAL));
#else
            dc.SetBrush(wxBrush(*wxBLACK,wxBDIAGONAL_HATCH));
#endif
            dc.DrawRectangle(rect);
            return;
        }

        if ( value == wxPG_COLOUR_CUSTOM )
        {
            dc.SetBrush( GetVal().m_colour );
        }
        else
        {
            dc.SetBrush(wxSystemSettings::GetColour((wxSystemColour)value));
        }
        dc.DrawRectangle(rect);


    }


    bool wxsMyColourPropertyClass::StringToValue( wxVariant& value, const wxString& text, int argFlags ) const
    {
        //
        // Accept colour format "[Name] [(R,G,B)]"
        // Name takes precedence.
        //
        wxString colourName;
        wxString colourRGB;

        int ppos = text.Find(wxT("("));

        if ( ppos == wxNOT_FOUND )
        {
            colourName = text;
        }
        else
        {
            colourName = text.substr(0, ppos);
            colourRGB = text.substr(ppos, text.length()-ppos);
        }

        // Strip spaces from extremities
        colourName.Trim(true);
        colourName.Trim(false);
        colourRGB.Trim(true);

        // Validate colourRGB string - (1,1,1) is shortest allowed
        if ( colourRGB.length() < 7 )
            colourRGB.clear();

        if ( colourRGB.length() == 0 && m_choices.GetCount() &&
             colourName == m_choices.GetLabel(GetCustomColourIndex()) )
        {
            if ( !(argFlags & wxPG_EDITABLE_VALUE ))
            {
                // This really should not occurr...
                // wxASSERT(false);
                ResetNextIndex();
                return false;
            }

            QueryColourFromUser(value);
        }
        else
        {
            wxColourPropertyValue val;

            bool done = false;

            if ( colourName.length() )
            {
                // Try predefined colour first
                bool res = wxEnumProperty::StringToValue(value, colourName, argFlags);
                if ( res && GetIndex() >= 0 )
                {
                    val.m_type = GetIndex();
                    if ( val.m_type < m_choices.GetCount() )
                        val.m_type = m_choices[val.m_type].GetValue();

                    // Get proper colour for type.
                    val.m_colour = GetColour(val.m_type);

                    done = true;
                }
            }
            if ( colourRGB.length() && !done )
            {
                // Then check custom colour.
                val.m_type = wxPG_COLOUR_CUSTOM;

                int r = -1, g = -1, b = -1;
                wxSscanf(colourRGB.c_str(),wxT("(%i,%i,%i)"),&r,&g,&b);

                if ( r >= 0 && r <= 255 &&
                     g >= 0 && g <= 255 &&
                     b >= 0 && b <= 255 )
                {
                    val.m_colour.Set(r,g,b);

                    done = true;
                }
            }

            if ( !done )
            {
                ResetNextIndex();
                return false;
            }

            value = DoTranslateVal(val);
        }

        return true;
    }

    bool wxsMyColourPropertyClass::DoSetAttribute(cb_unused const wxString& name, cb_unused wxVariant& value)
    {
        return false;

    }
}

wxColour wxsColourData::GetColour()
{
    if ( m_type == wxsCOLOUR_DEFAULT )
    {
        return wxColour();
    }

    if ( m_type == wxPG_COLOUR_CUSTOM )
    {
        return m_colour;
    }

    return wxSystemSettings::GetColour((wxSystemColour)m_type);
}

wxString wxsColourData::BuildCode(wxsCoderContext* Context)
{
    if ( m_type == wxsCOLOUR_DEFAULT )
    {
        return wxEmptyString;
    }

    switch ( Context->m_Language )
    {
        case wxsCPP:
        {
            if ( m_type == wxPG_COLOUR_CUSTOM )
            {
                return wxString::Format(_T("wxColour(%u,%u,%u)"),
                    (unsigned int)m_colour.Red(),
                    (unsigned int)m_colour.Green(),
                    (unsigned int)m_colour.Blue());
            }

            wxString SysColName;

            #define SYSCLR(N) if ( m_type == N ) SysColName = _T(#N); else
            SYSCLR(wxSYS_COLOUR_SCROLLBAR)
            SYSCLR(wxSYS_COLOUR_BACKGROUND)
            SYSCLR(wxSYS_COLOUR_DESKTOP)
            SYSCLR(wxSYS_COLOUR_ACTIVECAPTION)
            SYSCLR(wxSYS_COLOUR_INACTIVECAPTION)
            SYSCLR(wxSYS_COLOUR_MENU)
            SYSCLR(wxSYS_COLOUR_WINDOW)
            SYSCLR(wxSYS_COLOUR_WINDOWFRAME)
            SYSCLR(wxSYS_COLOUR_MENUTEXT)
            SYSCLR(wxSYS_COLOUR_WINDOWTEXT)
            SYSCLR(wxSYS_COLOUR_CAPTIONTEXT)
            SYSCLR(wxSYS_COLOUR_ACTIVEBORDER)
            SYSCLR(wxSYS_COLOUR_INACTIVEBORDER)
            SYSCLR(wxSYS_COLOUR_APPWORKSPACE)
            SYSCLR(wxSYS_COLOUR_HIGHLIGHT)
            SYSCLR(wxSYS_COLOUR_HIGHLIGHTTEXT)
            SYSCLR(wxSYS_COLOUR_BTNFACE)
            SYSCLR(wxSYS_COLOUR_3DFACE)
            SYSCLR(wxSYS_COLOUR_BTNSHADOW)
            SYSCLR(wxSYS_COLOUR_3DSHADOW)
            SYSCLR(wxSYS_COLOUR_GRAYTEXT)
            SYSCLR(wxSYS_COLOUR_BTNTEXT)
            SYSCLR(wxSYS_COLOUR_INACTIVECAPTIONTEXT)
            SYSCLR(wxSYS_COLOUR_BTNHIGHLIGHT)
            SYSCLR(wxSYS_COLOUR_BTNHILIGHT)
            SYSCLR(wxSYS_COLOUR_3DHIGHLIGHT)
            SYSCLR(wxSYS_COLOUR_3DHILIGHT)
            SYSCLR(wxSYS_COLOUR_3DDKSHADOW)
            SYSCLR(wxSYS_COLOUR_3DLIGHT)
            SYSCLR(wxSYS_COLOUR_INFOTEXT)
            SYSCLR(wxSYS_COLOUR_INFOBK)
            SYSCLR(wxSYS_COLOUR_LISTBOX)
            SYSCLR(wxSYS_COLOUR_HOTLIGHT)
            SYSCLR(wxSYS_COLOUR_GRADIENTACTIVECAPTION)
            SYSCLR(wxSYS_COLOUR_GRADIENTINACTIVECAPTION)
            SYSCLR(wxSYS_COLOUR_MENUHILIGHT)
            SYSCLR(wxSYS_COLOUR_MENUBAR)
            {}
            #undef SYSCLR

            if ( SysColName.empty() )
            {
                return wxEmptyString;
            }

            Context->AddHeader(_T("<wx/settings.h>"),_T(""),hfLocal);
            return _T("wxSystemSettings::GetColour(") + SysColName + _T(")");
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsColourData::BuildCode"),Context->m_Language);
        }
    }

    return wxEmptyString;
}

// Helper macros for fetching variables
#define VALUE   wxsVARIABLE(Object,ValueOffset,wxsColourData)


wxsColourProperty::wxsColourProperty(
    const wxString& PGName,
    const wxString& DataName,
    long _ValueOffset,
    int Priority):
        wxsProperty(PGName,DataName,Priority),
        ValueOffset(_ValueOffset)
        {}


void wxsColourProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    PGRegister(Object,Grid,Grid->AppendIn(Parent,new wxsMyColourPropertyClass(GetPGName(),wxPG_LABEL,VALUE)));
}

bool wxsColourProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,cb_unused long Index)
{
#if wxCHECK_VERSION(3, 0, 0)
    VALUE.m_type = wxsColourValues[Id->GetChoiceSelection()];

    if ( VALUE.m_type == wxsCOLOUR_DEFAULT )
    {
        VALUE.m_colour = wxColour(0,0,0);
    }
    else if ( VALUE.m_type == wxPG_COLOUR_CUSTOM )
    {
        VALUE.m_colour = wxColour(_T("rgb")+Id->GetValueAsString());
    }
    else
    {
        VALUE.m_colour = wxSystemSettings::GetColour((wxSystemColour)VALUE.m_type);
    }

#else
    wxVariant value = Grid->GetPropertyValue(Id);
    wxColourPropertyValue* Val = wxGetVariantCast(value,wxColourPropertyValue);

    if ( !Val ) return false;

    VALUE = *Val;

#endif // wxCHECK_VERSION
    return true;
}

bool wxsColourProperty::PGWrite(cb_unused wxsPropertyContainer* Object,cb_unused wxPropertyGridManager* Grid,cb_unused wxPGId Id,cb_unused long Index)
{
    return true;
}

bool wxsColourProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE.m_type = wxsCOLOUR_DEFAULT;
        VALUE.m_colour = wxColour(0,0,0);
        return false;
    }

    wxString Str;
    if ( !XmlGetString(Element,Str) || Str.empty() )
    {
        VALUE.m_type = wxsCOLOUR_DEFAULT;
        VALUE.m_colour = wxColour(0,0,0);
        return false;
    }

    unsigned long tmp = 0;
    if ( Str.Length() == 7 && Str[0] == _T('#') && wxSscanf(Str.c_str(),_T("#%lX"),&tmp) == 1 )
    {
        VALUE.m_type = wxPG_COLOUR_CUSTOM;
        VALUE.m_colour = wxColour((unsigned char) ((tmp & 0xFF0000) >> 16) ,
                                  (unsigned char) ((tmp & 0x00FF00) >> 8),
                                  (unsigned char) ((tmp & 0x0000FF)));
    }
    else
    {
        #define SYSCLR(N) if ( Str == _T(#N) ) { VALUE.m_type = N; VALUE.m_colour = wxSystemSettings::GetColour(N); } else
        SYSCLR(wxSYS_COLOUR_SCROLLBAR)
        SYSCLR(wxSYS_COLOUR_BACKGROUND)
        SYSCLR(wxSYS_COLOUR_DESKTOP)
        SYSCLR(wxSYS_COLOUR_ACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_INACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_MENU)
        SYSCLR(wxSYS_COLOUR_WINDOW)
        SYSCLR(wxSYS_COLOUR_WINDOWFRAME)
        SYSCLR(wxSYS_COLOUR_MENUTEXT)
        SYSCLR(wxSYS_COLOUR_WINDOWTEXT)
        SYSCLR(wxSYS_COLOUR_CAPTIONTEXT)
        SYSCLR(wxSYS_COLOUR_ACTIVEBORDER)
        SYSCLR(wxSYS_COLOUR_INACTIVEBORDER)
        SYSCLR(wxSYS_COLOUR_APPWORKSPACE)
        SYSCLR(wxSYS_COLOUR_HIGHLIGHT)
        SYSCLR(wxSYS_COLOUR_HIGHLIGHTTEXT)
        SYSCLR(wxSYS_COLOUR_BTNFACE)
        SYSCLR(wxSYS_COLOUR_3DFACE)
        SYSCLR(wxSYS_COLOUR_BTNSHADOW)
        SYSCLR(wxSYS_COLOUR_3DSHADOW)
        SYSCLR(wxSYS_COLOUR_GRAYTEXT)
        SYSCLR(wxSYS_COLOUR_BTNTEXT)
        SYSCLR(wxSYS_COLOUR_INACTIVECAPTIONTEXT)
        SYSCLR(wxSYS_COLOUR_BTNHIGHLIGHT)
        SYSCLR(wxSYS_COLOUR_BTNHILIGHT)
        SYSCLR(wxSYS_COLOUR_3DHIGHLIGHT)
        SYSCLR(wxSYS_COLOUR_3DHILIGHT)
        SYSCLR(wxSYS_COLOUR_3DDKSHADOW)
        SYSCLR(wxSYS_COLOUR_3DLIGHT)
        SYSCLR(wxSYS_COLOUR_INFOTEXT)
        SYSCLR(wxSYS_COLOUR_INFOBK)
        SYSCLR(wxSYS_COLOUR_LISTBOX)
        SYSCLR(wxSYS_COLOUR_HOTLIGHT)
        SYSCLR(wxSYS_COLOUR_GRADIENTACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_GRADIENTINACTIVECAPTION)
        SYSCLR(wxSYS_COLOUR_MENUHILIGHT)
        SYSCLR(wxSYS_COLOUR_MENUBAR)
        {
            VALUE.m_type = wxsCOLOUR_DEFAULT;
            VALUE.m_colour = wxColour(0,0,0);
        }
        #undef SYSCLR
    }

    return true;
}

bool wxsColourProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( VALUE.m_type == wxsCOLOUR_DEFAULT )
    {
        return false;
    }

    if ( VALUE.m_type == wxPG_COLOUR_CUSTOM )
    {
        XmlSetString(Element,
            wxString::Format(_T("#%02X%02X%02X"),
                (unsigned int)VALUE.m_colour.Red(),
                (unsigned int)VALUE.m_colour.Green(),
                (unsigned int)VALUE.m_colour.Blue()));
        return true;
    }

    #define SYSCLR(N) if ( VALUE.m_type == N ) XmlSetString(Element,_T(#N)); else
    SYSCLR(wxSYS_COLOUR_SCROLLBAR)
    SYSCLR(wxSYS_COLOUR_BACKGROUND)
    SYSCLR(wxSYS_COLOUR_DESKTOP)
    SYSCLR(wxSYS_COLOUR_ACTIVECAPTION)
    SYSCLR(wxSYS_COLOUR_INACTIVECAPTION)
    SYSCLR(wxSYS_COLOUR_MENU)
    SYSCLR(wxSYS_COLOUR_WINDOW)
    SYSCLR(wxSYS_COLOUR_WINDOWFRAME)
    SYSCLR(wxSYS_COLOUR_MENUTEXT)
    SYSCLR(wxSYS_COLOUR_WINDOWTEXT)
    SYSCLR(wxSYS_COLOUR_CAPTIONTEXT)
    SYSCLR(wxSYS_COLOUR_ACTIVEBORDER)
    SYSCLR(wxSYS_COLOUR_INACTIVEBORDER)
    SYSCLR(wxSYS_COLOUR_APPWORKSPACE)
    SYSCLR(wxSYS_COLOUR_HIGHLIGHT)
    SYSCLR(wxSYS_COLOUR_HIGHLIGHTTEXT)
    SYSCLR(wxSYS_COLOUR_BTNFACE)
    SYSCLR(wxSYS_COLOUR_3DFACE)
    SYSCLR(wxSYS_COLOUR_BTNSHADOW)
    SYSCLR(wxSYS_COLOUR_3DSHADOW)
    SYSCLR(wxSYS_COLOUR_GRAYTEXT)
    SYSCLR(wxSYS_COLOUR_BTNTEXT)
    SYSCLR(wxSYS_COLOUR_INACTIVECAPTIONTEXT)
    SYSCLR(wxSYS_COLOUR_BTNHIGHLIGHT)
    SYSCLR(wxSYS_COLOUR_BTNHILIGHT)
    SYSCLR(wxSYS_COLOUR_3DHIGHLIGHT)
    SYSCLR(wxSYS_COLOUR_3DHILIGHT)
    SYSCLR(wxSYS_COLOUR_3DDKSHADOW)
    SYSCLR(wxSYS_COLOUR_3DLIGHT)
    SYSCLR(wxSYS_COLOUR_INFOTEXT)
    SYSCLR(wxSYS_COLOUR_INFOBK)
    SYSCLR(wxSYS_COLOUR_LISTBOX)
    SYSCLR(wxSYS_COLOUR_HOTLIGHT)
    SYSCLR(wxSYS_COLOUR_GRADIENTACTIVECAPTION)
    SYSCLR(wxSYS_COLOUR_GRADIENTINACTIVECAPTION)
    SYSCLR(wxSYS_COLOUR_MENUHILIGHT)
    SYSCLR(wxSYS_COLOUR_MENUBAR)
    {
        return false;
    }
    #undef SYSCLR

    return true;
}

bool wxsColourProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    long Colour;
    unsigned long Type;
    Stream->SubCategory(GetDataName());
    if ( !Stream->GetULong(_T("type"),Type,wxsCOLOUR_DEFAULT) ) Ret = false;
    VALUE.m_type = Type;
    if ( !Stream->GetLong(_T("value"),Colour,0) ) Ret = false;
    if ( Type == wxsCOLOUR_DEFAULT )
    {
        VALUE.m_colour = wxColour(0,0,0);
    }
    else if ( Type == wxPG_COLOUR_CUSTOM )
    {
        VALUE.m_colour = Colour;
    }
    else
    {
        VALUE.m_colour = wxSystemSettings::GetColour((wxSystemColour)VALUE.m_type);
    }
    Stream->PopCategory();
    return Ret;
}

bool wxsColourProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    bool Ret = true;
    long Colour =
        ( (unsigned int)VALUE.m_colour.Red()   << 16 ) |
        ( (unsigned int)VALUE.m_colour.Green() <<  8 ) |
        ( (unsigned int)VALUE.m_colour.Blue() );
    unsigned long Type = VALUE.m_type;

    Stream->SubCategory(GetDataName());

    if ( !Stream->PutULong(_T("type"),Type,wxsCOLOUR_DEFAULT) ) Ret = false;
    VALUE.m_type = Type;

    if ( !Stream->PutLong(_T("value"),Colour,0) ) Ret = false;

    if ( VALUE.m_type == wxsCOLOUR_DEFAULT )
    {
        VALUE.m_colour = wxColour(0,0,0);
    }
    else if ( VALUE.m_type == wxPG_COLOUR_CUSTOM )
    {
        VALUE.m_colour = Colour;
    }
    else
    {
        VALUE.m_colour = wxSystemSettings::GetColour((wxSystemColour)VALUE.m_type);
    }

    Stream->PopCategory();
    return Ret;
}


