/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
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

#include "wxscustomwidget.h"
#include "../wxsitemresdata.h"

namespace
{
    wxsRegisterItem<wxsCustomWidget> Reg(
        _T("Custom"),
        wxsTWidget,
        _T(""),_T(""),_T(""),_T(""),
        _T("Standard"),
        10,
        _T("Custom"),
        wxsCPP,
        0,0,
        _T("images/wxsmith/Custom32.png"),
        _T("images/wxsmith/Custom16.png"));

    WXS_EV_BEGIN(wxsCustomWidgetEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}


wxsCustomWidget::wxsCustomWidget(wxsItemResData* Data):
    wxsWidget(Data,&Reg.Info,wxsCustomWidgetEvents),
    m_CreatingCode(_T("$(THIS) = new $(CLASS)($(PARENT),$(ID),$(POS),$(SIZE),$(STYLE),wxDefaultValidator,$(NAME));")),
    m_Style(_T("0"))
{
    SetUserClass(_("CustomClass"));
}

void wxsCustomWidget::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    wxString Result = m_CreatingCode;
    Result.Replace(_T("$(POS)"),PosCode(WindowParent,Language));
    Result.Replace(_T("$(SIZE)"),SizeCode(WindowParent,Language));
    Result.Replace(_T("$(STYLE)"),m_Style);
    Result.Replace(_T("$(ID)"),GetIdName());
    Result.Replace(_T("$(THIS)"),GetVarName());
    Result.Replace(_T("$(PARENT)"),WindowParent);
    Result.Replace(_T("$(NAME)"),wxsCodeMarks::WxString(Language,GetIdName(),false));
    Result.Replace(_T("$(CLASS)"),GetUserClass());

    Code << Result << _T("\n");
}

wxObject* wxsCustomWidget::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxPanel* Background = new wxPanel(Parent,-1,Pos(Parent),wxDefaultSize);
    wxStaticText* Wnd = new wxStaticText(Background,-1,_T("???"),
        wxDefaultPosition,Size(Parent),wxST_NO_AUTORESIZE|wxALIGN_CENTRE);
    wxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);
    Sizer->Add(Wnd,1,wxEXPAND,0);
    Background->SetSizer(Sizer);
    Sizer->SetSizeHints(Background);
    Wnd->SetBackgroundColour(wxColour(0,0,0));
    Wnd->SetForegroundColour(wxColour(0xFF,0xFF,0xFF));
    Background->SetBackgroundColour(wxColour(0,0,0));
    Background->SetForegroundColour(wxColour(0xFF,0xFF,0xFF));
    return Background;
}

void wxsCustomWidget::OnEnumWidgetProperties(long Flags)
{
    wxString XmlDataInit = m_XmlData;
    if ( GetResourceData()->GetPropertiesFilter() == flSource )
    {
        WXS_STRING(wxsCustomWidget,m_CreatingCode,_("Creating code:"),_T("creating_code"),_T(""),true);
    }
    else
    {
        if ( !(Flags&flXml) )
        {
            WXS_STRING(wxsCustomWidget,m_XmlData,_("Xml Data:"),_T(""),_T(""),false);
        }
    }

    WXS_SHORT_STRING(wxsCustomWidget,m_Style,_("Style:"),_T("style"),_T("0"),false);

    if ( Flags&flPropGrid )
    {
        if ( XmlDataInit != m_XmlData )
        {
            // We know it's propgrid operation and xml data has changed,
            // need to reparse this data
            RebuildXmlDataDoc();
        }
    }
}

void wxsCustomWidget::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
}

bool wxsCustomWidget::OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsItem::OnXmlRead(Element,IsXRC,IsExtra);

    if ( IsXRC )
    {
        if ( GetResourceData()->GetPropertiesFilter() != flSource )
        {
            SetUserClass(cbC2U(Element->Attribute("class")));
            m_XmlDataDoc.Clear();
            for ( TiXmlElement* Child = Element->FirstChildElement(); Child; Child = Child->NextSiblingElement() )
            {
                // Skipping all standard elements
                wxString Name = cbC2U(Child->Value());
                if ( Name != _T("pos") &&
                     Name != _T("size") &&
                     Name != _T("style") &&
                     Name != _T("enabled") &&
                     Name != _T("focused") &&
                     Name != _T("hidden") &&
                     Name != _T("fg") &&
                     Name != _T("bg") &&
                     Name != _T("font") &&
                     Name != _T("handler") )
                {
                    m_XmlDataDoc.InsertEndChild(*Child);
                }
            }
            RebuildXmlData();
        }
    }

    return Ret;
}

bool wxsCustomWidget::OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsItem::OnXmlWrite(Element,IsXRC,IsExtra);

    if ( IsXRC )
    {
        if ( GetResourceData()->GetPropertiesFilter() != flSource )
        {
            Element->SetAttribute("class",cbU2C(GetUserClass()));
            Element->RemoveAttribute("subclass");
            Element->InsertEndChild(TiXmlElement("style"))->InsertEndChild(TiXmlText(cbU2C(m_Style)));

            for ( TiXmlElement* Child = m_XmlDataDoc.FirstChildElement(); Child; Child = Child->NextSiblingElement() )
            {
                // Skipping all standard elements
                wxString Name = cbC2U(Child->Value());
                if ( Name != _T("pos") &&
                     Name != _T("size") &&
                     Name != _T("style") &&
                     Name != _T("enabled") &&
                     Name != _T("focused") &&
                     Name != _T("hidden") &&
                     Name != _T("fg") &&
                     Name != _T("bg") &&
                     Name != _T("font") &&
                     Name != _T("handler") )
                {
                    Element->InsertEndChild(*Child);
                }
            }
        }
    }

    return Ret;
}

void wxsCustomWidget::RebuildXmlData()
{
    TiXmlPrinter Printer;
    Printer.SetIndent("\t");
    m_XmlDataDoc.Accept(&Printer);
    m_XmlData = cbC2U(Printer.CStr());
}

bool wxsCustomWidget::RebuildXmlDataDoc()
{
    m_XmlDataDoc.Clear();
    m_XmlDataDoc.Parse(cbU2C(m_XmlData));
    if ( m_XmlDataDoc.Error() )
    {
        wxMessageBox(
            wxString::Format(
            _("Invalid Xml structure.\nError at line %d, column %d:\n\t\"%s\""),
                m_XmlDataDoc.ErrorRow(),m_XmlDataDoc.ErrorCol(),
                wxGetTranslation(cbC2U(m_XmlDataDoc.ErrorDesc()).c_str())));
        return false;
    }

    return true;
}

