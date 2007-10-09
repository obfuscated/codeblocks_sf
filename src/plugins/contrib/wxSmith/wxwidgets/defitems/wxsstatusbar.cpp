/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <wx/statusbr.h>
#include "wxsstatusbar.h"
#include "../wxsitemresdata.h"

namespace
{
    wxsRegisterItem<wxsStatusBar> Reg(_T("StatusBar"),wxsTTool,_T("Tools"),50);

    WXS_ST_BEGIN(wxsStatusBarStyles,_T(""))
        WXS_ST_CATEGORY("wxStatusBar")
        WXS_ST(wxST_SIZEGRIP)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    const wxChar* FieldStyles[] = { _T("wxSB_NORMAL"), _T("wxSB_FLAT"), _T("wxSB_RAISED"), 0 };
    const long FieldStylesVal[] = { wxSB_NORMAL, wxSB_FLAT, wxSB_RAISED };
}

wxsStatusBar::wxsStatusBar(wxsItemResData* Data):
    wxsTool(Data,&Reg.Info,0,wxsStatusBarStyles,flVariable|flId|flSubclass|flExtraCode),
    m_Fields(1)
{
    UpdateArraysSize(m_Fields);
}

void wxsStatusBar::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
            AddHeader(_T("<wx/statusbr.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %T, %N);\n"));
            if ( m_Fields>0 )
            {
                wxString& CounterVal = GetCoderContext()->m_Extra[_T("wxStatusBarCount")];
                wxString WidthsVarName = wxString::Format(_T("__wxStatusBarWidths_%d"),CounterVal.Length());
                wxString StylesVarName = wxString::Format(_T("__wxStatusBarStyles_%d"),CounterVal.Length());
                CounterVal.Append(_T('*'));
                Codef(_T("int %v[%d] = { "),WidthsVarName.c_str(),m_Fields);
                for ( int i=0; i<m_Fields; i++ )
                {
                    Codef( _T("%d%s"),
                        m_VarWidth[i]?-m_Widths[i]:m_Widths[i],
                        i==(m_Fields-1) ? _T(" };\n") : _T(", "));
                }
                Codef(_T("int %v[%d] = { "),StylesVarName.c_str(),m_Fields);
                for ( int i=0; i<m_Fields; i++ )
                {
                    Codef(_T("%s%s"),
                        m_Styles[i] == wxSB_FLAT ?   _T("wxSB_FLAT") :
                        m_Styles[i] == wxSB_RAISED ? _T("wxSB_RAISED") :
                                                     _T("wxSB_NORMAL"),
                        i==(m_Fields-1) ? _T(" };\n") : _T(", "));
                }
                Codef(_T("%ASetFieldsCount(%d,%v);\n"),m_Fields,WidthsVarName.c_str());
                Codef(_T("%ASetStatusStyles(%d,%v);\n"),m_Fields,StylesVarName.c_str());
                Codef(_T("SetStatusBar(%O);\n"));
            }
            BuildSetupWindowCode();
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsStatusBar::OnBuildCreatingCode"),GetLanguage());
    }
}

void wxsStatusBar::OnEnumToolProperties(long Flags)
{
}

bool wxsStatusBar::OnCanAddToResource(wxsItemResData* Data,bool ShowMessage)
{
    if ( Data->GetClassType() != _T("wxFrame") )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_("wxStatusBar can be added to wxFrame only"));
        }
        return false;
    }

    for ( int i=0; i<Data->GetToolsCount(); i++ )
    {
        if ( Data->GetTool(i)->GetClassName() == _T("wxStatusBar") )
        {
            if ( ShowMessage )
            {
                cbMessageBox(_("Can not add two or more wxStatusBar classes\ninto one wxFrame"));
            }
            return false;
        }
    }

    return true;
}

bool wxsStatusBar::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( ShowMessage )
    {
        cbMessageBox(_("wxsStatusBar can have no children"));
    }
    return false;
}

bool wxsStatusBar::OnCanAddToParent(wxsParent* Item,bool ShowMessage)
{
    if ( ShowMessage )
    {
        cbMessageBox(_("wxsStatusBar must be added as tool"));
    }
    return false;
}


void wxsStatusBar::OnAddExtraProperties(wxsPropertyGridManager* Grid)
{
    Grid->SetTargetPage(0);
    m_FieldsId = Grid->Append(wxIntProperty(_("Fields"),wxPG_LABEL,m_Fields));

    for ( int i=0; i<m_Fields; i++ )
    {
        wxPGId ParentProp = Grid->Append(wxParentProperty(wxString::Format(_("Field %d"),i+1),wxPG_LABEL));
        m_WidthsIds[i] = Grid->AppendIn(ParentProp,wxIntProperty(_("Width"),wxPG_LABEL,m_Widths[i]));
        m_VarWidthIds[i] = Grid->AppendIn(ParentProp,wxBoolProperty(_T("Variable width"),wxPG_LABEL,m_VarWidth[i]));
        Grid->SetPropertyAttribute(m_VarWidthIds[i],wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
        m_StylesIds[i] = Grid->AppendIn(ParentProp,wxEnumProperty(_("Style"),wxPG_LABEL,FieldStyles,FieldStylesVal,m_Styles[i]));
        m_ParentIds[i] = ParentProp;
    }

    wxsItem::OnAddExtraProperties(Grid);
}

void wxsStatusBar::OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id)
{
    if ( Id == m_FieldsId )
    {
        // Number of fields is going to change...
        int NewFields = Grid->GetPropertyValueAsInt(Id);
        if ( NewFields < 1 )
        {
            NewFields = 1;
            Grid->SetPropertyValue(Id,NewFields);
        }

        // Now it's time to delete / add properties for fields
        if ( NewFields < m_Fields )
        {
            for ( int i=NewFields; i<m_Fields; i++ )
            {
                Grid->Delete(m_ParentIds[i]);
            }
        }
        else if ( NewFields > m_Fields )
        {
            // Adding new properties
            Grid->SetTargetPage(0);
            UpdateArraysSize(NewFields);
            for ( int i = m_Fields; i<NewFields; i++ )
            {
                wxPGId ParentProp = Grid->Append(wxParentProperty(wxString::Format(_("Field %d"),i+1),wxPG_LABEL));
                m_WidthsIds[i] = Grid->AppendIn(ParentProp,wxIntProperty(_("Width"),wxPG_LABEL,m_Widths[i]));
                m_VarWidthIds[i] = Grid->AppendIn(ParentProp,wxBoolProperty(_T("Variable width"),wxPG_LABEL,m_VarWidth[i]));
                Grid->SetPropertyAttribute(m_VarWidthIds[i],wxPG_BOOL_USE_CHECKBOX,1L,wxRECURSE);
                m_StylesIds[i] = Grid->AppendIn(ParentProp,wxEnumProperty(_("Style"),wxPG_LABEL,FieldStyles,FieldStylesVal,m_Styles[i]));
                m_ParentIds[i] = ParentProp;
            }
        }

        m_Fields = NewFields;
        NotifyPropertyChange(true);
        return;
    }

    for ( int i=0; i<m_Fields; i++ )
    {
        if ( m_ParentIds[i]   == Id ||
             m_WidthsIds[i]   == Id ||
             m_StylesIds[i]   == Id ||
             m_VarWidthIds[i] == Id )
        {
            m_Widths[i] = Grid->GetPropertyValueAsLong(m_WidthsIds[i]);
            m_Styles[i] = Grid->GetPropertyValueAsLong(m_StylesIds[i]);
            m_VarWidth[i] = Grid->GetPropertyValueAsBool(m_VarWidthIds[i]);
            if ( m_Widths[i] < 0 )
            {
                m_Widths[i] = -m_Widths[i];
                m_VarWidth[i] = !m_VarWidth[i];
                Grid->SetPropertyValue(m_WidthsIds[i],m_Widths[i]);
                Grid->SetPropertyValue(m_VarWidthIds[i],m_VarWidth[i]);
            }
            NotifyPropertyChange(true);
            return;
        }
    }

    wxsItem::OnExtraPropertyChanged(Grid,Id);
}

bool wxsStatusBar::OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        TiXmlElement* FieldsCnt = Element->FirstChildElement("fields");
        if ( !FieldsCnt )
        {
            m_Fields = 1;
        }
        else
        {
            m_Fields = wxAtoi(cbC2U(FieldsCnt->GetText()));
        }

        if ( m_Fields < 1 ) m_Fields = 1;
        UpdateArraysSize(m_Fields);

        TiXmlElement* WidthsElem = Element->FirstChildElement("widths");
        wxString WidthsStr = cbC2U(WidthsElem->GetText());
        TiXmlElement* StylesElem = Element->FirstChildElement("styles");
        wxString StylesStr = cbC2U(StylesElem->GetText());

        for ( int i=0; i<m_Fields; i++ )
        {
            m_Widths[i] = wxAtoi(WidthsStr.BeforeFirst(_T(',')));
            if ( WidthsStr.Find(_T(',')) != -1 )
            {
                WidthsStr.Remove(0,WidthsStr.Find(_T(','))+1);
            }
            if ( m_Widths[i] < 0 )
            {
                m_VarWidth[i] = true;
                m_Widths[i] = -m_Widths[i];
            }
            else
            {
                m_VarWidth[i] = false;
            }

            wxString Style = StylesStr.BeforeFirst(_T(','));
            if ( Style==_T("wxSB_FLAT") )
            {
                m_Styles[i] = wxSB_FLAT;
            }
            else if ( Style==_T("wxSB_RAISED") )
            {
                m_Styles[i] = wxSB_RAISED;
            }
            else
            {
                m_Styles[i] = wxSB_NORMAL;
            }

            if ( StylesStr.Find(_T(',')) != -1 )
            {
                StylesStr.Remove(0,StylesStr.Find(_T(','))+1);
            }
        }
    }

    return wxsParent::OnXmlRead(Element,IsXRC,IsExtra);
}

bool wxsStatusBar::OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        Element->InsertEndChild(TiXmlElement("fields"))->
                 InsertEndChild(TiXmlText(cbU2C(wxString::Format(_T("%d"),m_Fields))));

        wxString Widths;
        wxString Styles;
        for ( int i=0; i<m_Fields; i++ )
        {
            if ( i>0 )
            {
                Widths << _T(',');
                Styles << _T(',');
            }

            Widths << wxString::Format(_T("%d"),m_VarWidth[i]?-m_Widths[i]:m_Widths[i]);
            if ( m_Styles[i] == wxSB_FLAT )
            {
                Styles << _T("wxSB_FLAT");
            }
            else if ( m_Styles[i] == wxSB_RAISED )
            {
                Styles << _T("wxSB_RAISED");
            }
            else
            {
                Styles << _T("wxSB_NORMAL");
            }
        }

        Element->InsertEndChild(TiXmlElement("widths"))->
                 InsertEndChild(TiXmlText(cbU2C(Widths)));
        Element->InsertEndChild(TiXmlElement("styles"))->
                 InsertEndChild(TiXmlText(cbU2C(Styles)));
    }

    return wxsParent::OnXmlWrite(Element,IsXRC,IsExtra);
}

void wxsStatusBar::UpdateArraysSize(int Size)
{
    m_Widths.SetCount(Size,10);
    m_Styles.SetCount(Size,wxSB_NORMAL);
    while ( (int)m_VarWidth.GetCount() < Size )
    {
        // SetCount didn't work for wxArrayBool
        m_VarWidth.Add(true);
    }
    m_ParentIds.SetCount(Size);
    m_WidthsIds.SetCount(Size);
    m_StylesIds.SetCount(Size);
    m_VarWidthIds.SetCount(Size);
}
