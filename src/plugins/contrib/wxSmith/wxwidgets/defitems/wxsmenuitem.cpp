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

#include "wxsmenuitem.h"

#include <prep.h>

namespace
{
    class InfoHandler: public wxsItemInfo
    {
        public:

            InfoHandler(): m_TreeImage(_T("images/wxsmith/wxMenuItem16.png"),true)
            {
                ClassName      = _T("wxMenuItem");
                Type           = wxsTTool;
                License        = _("wxWidgets license");
                Author         = _("wxWidgets team");
                Email          = _T("");
                Site           = _T("www.wxwidgets.org");
                Category       = _T("");
                Priority       = 0;
                DefaultVarName = _T("MenuItem");
                Languages      = wxsCPP;
                VerHi          = 2;
                VerLo          = 8;
                AllowInXRC     = true;

                // TODO: This code should be more generic since it may quickly
                //       become invalid
                wxString DataPath = ConfigManager::GetDataFolder() + _T("/images/wxsmith/");
                Icon32.LoadFile(DataPath+_T("wxMenuItem32.png"),wxBITMAP_TYPE_PNG);
                Icon16.LoadFile(DataPath+_T("wxMenuItem16.png"),wxBITMAP_TYPE_PNG);
                TreeIconId = m_TreeImage.GetIndex();
            };

            wxsAutoResourceTreeImage m_TreeImage;
    } Info;

    WXS_EV_BEGIN(wxsMenuItemEvents)
        WXS_EVI(EVT_MENU,wxEVT_COMMAND_MENU_SELECTED,wxCommandEvent,Selected)
    WXS_EV_END()
}

wxsMenuItem::wxsMenuItem(wxsItemResData* Data,bool BreakOrSeparator):
    wxsTool(
        Data,
        &Info,
        BreakOrSeparator?0:wxsMenuItemEvents,
        0,
        BreakOrSeparator?0:flVariable|flId|flExtraCode),
    m_Type(Normal),
    m_Enabled(true),
    m_Checked(false)
{
}

void wxsMenuItem::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:

            switch ( m_Type )
            {
                case Normal:
                {
                    if ( GetChildCount() )
                    {
                        // Creating new wxMenu
                        if ( IsPointer() )
                        {
                            Codef(_T("%C();\n"));
                        }
                        for ( int i=0; i<GetChildCount(); i++ )
                        {
                            GetChild(i)->BuildCode(GetCoderContext());
                        }
                        // Many parameters are passed in wxMenu::Append, so we call this function
                        // here, not in wxMenu
                        Codef(_T("%MAppend(%I, %t, %O, %t)%s;\n"),
                              m_Label.wx_str(),
                              m_Help.wx_str(),
                              m_Enabled?_T(""):_T("->Enable(false)"));
                        break;
                    }
                }
                // Fall through

             /* case Normal: */
                case Radio:
                case Check:
                {
                    wxString Text = m_Label;
                    if ( !m_Accelerator.IsEmpty() )
                    {
                        Text.Append(_T('\t'));
                        Text.Append(m_Accelerator);
                    }

                    const wxChar* ItemType;
                    switch ( m_Type )
                    {
                        case Normal: ItemType = _T("wxITEM_NORMAL"); break;
                        case Radio:  ItemType = _T("wxITEM_RADIO");  break;
                        default:     ItemType = _T("wxITEM_CHECK");  break;
                    }

                    Codef(_T("%C(%E, %I, %t, %t, %s);\n"),
                          Text.wx_str(),
                          m_Help.wx_str(),
                          ItemType);

                    if ( !m_Bitmap.IsEmpty() )
                    {
                        Codef(_T("%ASetBitmap(%i);\n"), &m_Bitmap, _T("wxART_OTHER"));
                    }

                    Codef(_T("%MAppend(%O);\n"));
                    if ( !m_Enabled )
                    {
                        Codef(_T("%AEnable(false);\n"));
                    }
                    if ( m_Checked && (m_Type==Check) )
                    {
                        Codef(_T("%ACheck(true);\n"));
                    }
                    break;
                }


                case Separator:
                {
                    Codef(_T("%MAppendSeparator();\n"));
                    break;
                }

                case Break:
                {
                    Codef(_T("%MBreak();\n"));
                    break;
                }

            }
            BuildSetupWindowCode();
            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsMenuItem::OnBuildCreatingCode"),GetLanguage());
    }
}

void wxsMenuItem::OnEnumToolProperties(cb_unused long Flags)
{

    switch ( m_Type )
    {
        case Normal:
            if ( GetChildCount() )
            {
                // When there are children (wxMenuItem maps to wxMenu class),
                // only these properties are enabled
                WXS_SHORT_STRING(wxsMenuItem,m_Label,_("Label"),_T("label"),_T(""),true);
                WXS_SHORT_STRING(wxsMenuItem,m_Help,_T("Help text"),_T("help"),_T(""),false);
                WXS_BOOL(wxsMenuItem,m_Enabled,_T("Enabled"),_T("enabled"),true);
                break;
            }
            // When there are no children, we threat this item as wxMenuItem

     /* case Normal: */
        case Radio:
        case Check:
            WXS_SHORT_STRING(wxsMenuItem,m_Label,_("Label"),_T("label"),_T(""),true);
            WXS_SHORT_STRING(wxsMenuItem,m_Accelerator,_("Accelerator"),_T("accel"),_T(""),false);
            WXS_SHORT_STRING(wxsMenuItem,m_Help,_T("Help text"),_T("help"),_T(""),false);
            WXS_BOOL(wxsMenuItem,m_Enabled,_T("Enabled"),_T("enabled"),true);
            if ( m_Type == Check ) { WXS_BOOL(wxsMenuItem,m_Checked,_T("Checked"),_T("checked"),false); }
            if ( m_Type == Normal )
            {
                WXS_BITMAP(wxsMenuItem,m_Bitmap,_("Bitmap"),_T("bitmap"),_T("wxART_OTHER"))
            }
            break;

        default:;
    }
}

bool wxsMenuItem::OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsParent::OnXmlWrite(Element,IsXRC,IsExtra);

    if ( IsXRC )
    {
        // Type information is stored differently
        switch ( m_Type )
        {
            case Separator:
                Element->SetAttribute("class","separator");
                break;

            case Break:
                Element->SetAttribute("class","break");
                break;

            case Radio:
                Element->InsertEndChild(TiXmlElement("radio"))->ToElement()->InsertEndChild(TiXmlText("1"));
                break;

            case Check:
                Element->InsertEndChild(TiXmlElement("checkable"))->ToElement()->InsertEndChild(TiXmlText("1"));
                break;

            case Normal:
                break;
        }
    }

    return Ret;
}

bool wxsMenuItem::OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsParent::OnXmlRead(Element,IsXRC,IsExtra);

    if ( IsXRC )
    {
        wxString Class = cbC2U(Element->Attribute("class"));
        if ( Class == _T("separator") )
        {
            m_Type = Separator;
        }
        else if ( Class == _T("break") )
        {
            m_Type = Break;
        }
        else
        {
            // This will handle both wxMenu and wxMenuItem
            TiXmlElement* Node = Element->FirstChildElement("radio");
            if ( Node && (cbC2U(Node->GetText())==_T("1")) )
            {
                m_Type = Radio;
            }
            else
            {
                Node = Element->FirstChildElement("checkable");

                // Backward BUG-compatibility
                if ( !Node ) Node = Element->FirstChildElement("check");

                if ( Node && (cbC2U(Node->GetText())==_T("1")) )
                {
                    m_Type = Check;
                    // Now, we are going to check its state, either checked or not checked
                    TiXmlElement* checkedNode = Element->FirstChildElement("checked");
                    if (checkedNode && (cbC2U(checkedNode->GetText())==_T("1")))
                        m_Checked = true;
                    // otherwise, the m_Checked will default to false.
                }
                else
                {
                    m_Type = Normal;
                }
            }
        }
    }

    return Ret;
}

bool wxsMenuItem::OnCanAddToParent(wxsParent* Parent,bool ShowMessage)
{
    if ( Parent->GetClassName() != _T("wxMenu") &&
         Parent->GetClassName() != _T("wxMenuItem") )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_("wxMenuItem class can be used inside wxMenu only"));
        }
        return false;
    }
    return true;
}

bool wxsMenuItem::OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        wxString ClassName = cbC2U(Elem->Attribute("class"));
        if ( ClassName == _T("separator") || ClassName == _T("break") )
        {
            wxsMenuItem* Child = new wxsMenuItem(GetResourceData(),true);
            AddChild(Child);
            return Child->XmlRead(Elem,IsXRC,IsExtra);
        }

        if ( ClassName == _T("wxMenu") || ClassName == _T("wxMenuItem") )
        {
            wxsMenuItem* Child = new wxsMenuItem(GetResourceData(),false);
            AddChild(Child);
            return Child->XmlRead(Elem,IsXRC,IsExtra);
        }
    }

    return true;
}

wxString wxsMenuItem::OnGetTreeLabel(int& Image)
{
    switch ( m_Type )
    {
        case Separator:
            return _T("--------");

        case Break:
            return _("** BREAK **");

        default:
            return m_Label;
    }
}

void wxsMenuItem::OnBuildDeclarationsCode()
{
    if ( !GetChildCount() )
    {
        switch ( m_Type )
        {
            case Break:
            case Separator:
                return;

            default:;
        }
    }

    wxsItem::OnBuildDeclarationsCode();
}

const wxString& wxsMenuItem::GetClassName()
{
    if ( GetChildCount() )
    {
        // If there are any children, this item becomes wxMenu
        static const wxString ChangedClassName = _T("wxMenu");
        return ChangedClassName;
    }
    return wxsTool::GetClassName();
}

void wxsMenuItem::OnBuildXRCFetchingCode()
{
    // Menu items can not be found through FindWindow stuff, we need to
    // fetch them using wxMenuBar::FindItem.
    long Flags = GetPropertiesFlags();
    if ( (Flags&flVariable) && (Flags&flId) )
    {
        AddXRCFetchingCode(
            GetVarName() + _T(" = GetMenuBar() ? ")
            _T("(") + GetUserClass() + _T("*)")
            _T("GetMenuBar()->FindItem(XRCID(\"") + GetIdName() + _T("\")) : NULL;\n"));
    }
}

