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

#include "wxsitem.h"
#include "wxsparent.h"
#include "wxsitemresdata.h"
#include "wxsitemrestreedata.h"
#include "wxseventseditor.h"

wxsItem::wxsItem(wxsItemResData* ResourceData,const wxsItemInfo* Info,long PropertiesFlags,const wxsEventDesc* Events):
    m_Info(Info),
    m_Events(Events,this),
    m_Parent(NULL),
    m_ResourceData(ResourceData),
    m_VarName(_T("")),
    m_IdName(_T("")),
    m_IsMember(true),
    m_BaseProperties(NULL),
    m_PropertiesFlags(PropertiesFlags),
    m_LastPreview(NULL),
    m_IsSelected(false),
    m_IsExpanded(false)
{
    if ( m_PropertiesFlags &
            (flPosition|flSize|flEnabled|flFocused|flHidden|
             flColours|flToolTip|flFont|flHelpText) )
    {
        m_BaseProperties = new wxsBaseProperties;
    }
}

wxsItem::~wxsItem()
{
    delete m_BaseProperties;
}

void wxsItem::OnEnumProperties(long Flags)
{
    if ( (Flags & flPropGrid) && (m_Parent != NULL) )
    {
        // Parent item does take care of enumerating properties if we are
        // ceating property grid
        m_Parent->OnEnumChildProperties(this,Flags);
    }
    else
    {
        EnumItemProperties(Flags);
    }
}

wxsQuickPropsPanel* wxsItem::OnCreateQuickProperties(wxWindow* ParentWnd)
{
    wxsAdvQPP* Panel = new wxsAdvQPP(ParentWnd,this);

    if ( m_Parent != NULL )
    {
        // Parent item does take care of inserting QPP Children
        m_Parent->OnAddChildQPP(this,Panel);
    }
    else
    {
        OnAddItemQPP(Panel);
    }

    // Appending qpp's from base properties
    if ( m_BaseProperties )
    {
        m_BaseProperties->AddQPPChild(Panel,GetPropertiesFlags());
    }

    return Panel;
}

long wxsItem::OnGetPropertiesFlags()
{
    if ( IsRootItem() )
    {
        // Small hack - if there's no parent, this is root item
        // of resource and thus can not have id nor variable
        return m_ResourceData->GetPropertiesFilter() | m_PropertiesFlags & ~flVariable & ~flId & ~flSubclass;
    }
    return m_ResourceData->GetPropertiesFilter() | m_PropertiesFlags;
}

void wxsItem::EnumItemProperties(long Flags)
{
    // Registering variable name / identifier
    // these values are skipped when storing into xml variable
    // because itis stored as attribute of XML element
    if ( (Flags & (flPropGrid|flPropStream)) != 0 )
    {
        WXS_STRING(wxsItem,m_VarName,flVariable,_("Var name"),_T("var_name"),wxEmptyString,false,false);
        if ( IsPointer() )
        {
            // If item is not pointer it must be declared globally
            WXS_BOOL(wxsItem,m_IsMember,flVariable,_(" Is member"),_T("var_is_member"),true);
        }
        WXS_STRING(wxsItem,m_IdName,flId,_("Identifier"),_T("identifier"),wxEmptyString,false,false);
        WXS_STRING(wxsItem,m_Subclass,flSubclass,_("Custom class"),_T("subclass"),wxEmptyString,false,false);
    }

    OnEnumItemProperties(Flags);

    // Now enumerating all properties from wxsBaseProperties if any
    if ( m_BaseProperties )
    {
        SubContainer(m_BaseProperties,Flags);
    }
}

void wxsItem::OnBuildDeclarationCode(wxString& Code,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Code << GetUserClass();
            if ( IsPointer() )
            {
                Code << _T("*");
            }
            Code << _T(" ") << GetVarName() << _T(";\n");
            return;

        default:
            wxsCodeMarks::Unknown(_T("wxsItem::OnBuildDeclarationCode"),Language);
    }
}

bool wxsItem::OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        wxsPropertyContainer::XmlRead(Element);
        m_IdName = cbC2U(Element->Attribute("name"));
        m_Subclass = cbC2U(Element->Attribute("subclass"));
    }

    if ( IsExtra )
    {
        m_VarName = cbC2U(Element->Attribute("variable"));
        const char* MbrText = Element->Attribute("member");
        m_IsMember = !MbrText || !strcmp(MbrText,"yes");
        m_Events.XmlLoadFunctions(Element);
    }

    return true;
}

bool wxsItem::OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        Element->SetAttribute("class",cbU2C(GetClassName()));
        wxsPropertyContainer::XmlWrite(Element);
        if ( GetPropertiesFlags() & flId )
        {
            Element->SetAttribute("name",cbU2C(m_IdName));
        }
        if ( GetPropertiesFlags() & flSubclass )
        {
            if ( !m_Subclass.IsEmpty() )
            {
                Element->SetAttribute("subclass",cbU2C(m_Subclass));
            }
        }
    }

    if ( IsExtra )
    {
        if ( GetPropertiesFlags() & flVariable )
        {
            Element->SetAttribute("variable",cbU2C(m_VarName));
            Element->SetAttribute("member",m_IsMember ? "yes" : "no" );
        }
        m_Events.XmlSaveFunctions(Element);
    }

    return true;
}

void wxsItem::BuildItemTree(wxsResourceTree* Tree,wxsResourceItemId Parent,int Position)
{
    int Image = GetInfo().TreeIconId;
    wxString Label = OnGetTreeLabel(Image);
    if ( Position<0 || Position>=(int)Tree->GetChildrenCount(Parent) )
    {
        m_LastTreeId = Tree->AppendItem(Parent,Label,Image,Image,new wxsItemResTreeData(this));
    }
    else
    {
        m_LastTreeId = Tree->InsertItem(Parent,Position,Label,Image,Image,new wxsItemResTreeData(this));
    }

    wxsParent* ParentItem = ConvertToParent();
    if ( ParentItem )
    {
        int Count = ParentItem->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            ParentItem->GetChild(i)->BuildItemTree(Tree,m_LastTreeId);
        }
    }

    if ( GetIsExpanded() )
    {
        Tree->Expand(m_LastTreeId);
    }
    else
    {
        Tree->Collapse(m_LastTreeId);
    }
}

wxObject* wxsItem::BuildPreview(wxWindow* Parent,long Flags)
{
    wxObject* Preview = OnBuildPreview(Parent,Flags);
    m_LastPreview = Preview;
    return Preview;
}

void wxsItem::ClearSelection()
{
    m_IsSelected = false;
    wxsParent* Parent = ConvertToParent();
    if ( Parent )
    {
        for ( int i = Parent->GetChildCount(); i-->0; )
        {
            Parent->GetChild(i)->ClearSelection();
        }
    }
}

void wxsItem::OnPropertyChanged()
{
    GetResourceData()->NotifyChange(this);
}

void wxsItem::OnSubPropertyChanged(wxsPropertyContainer*)
{
    GetResourceData()->NotifyChange(this);
}

void wxsItem::OnAddExtraProperties(wxsPropertyGridManager* Grid)
{
    if ( GetResourceData()->GetPropertiesFilter() != flFile )
    {
        wxsEventsEditor::Get().BuildEvents(this,Grid);
    }
}

void wxsItem::OnExtraPropertyChanged(wxsPropertyGridManager* Grid,wxPGId Id)
{
    wxsEventsEditor::Get().PGChanged(this,Grid,Id);
}

bool wxsItem::IsRootItem()
{
    return m_ResourceData->GetRootItem() == this;
}

wxString wxsItem::GetCreatePrefix(wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            if ( IsRootItem() ) return _T("Create");
            if ( IsPointer()  ) return GetVarName() + _T(" = new ") + GetUserClass();
            return GetVarName() + _T(".Create");

        default:
            wxsCodeMarks::Unknown(_T("wxsItem::GetCreatePrefix"),Language);
    }
    return wxEmptyString;
}

wxString wxsItem::GetAccessPrefix(wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            if ( IsRootItem() ) return wxEmptyString;
            if ( IsPointer()  ) return GetVarName() + _T("->");
            return GetVarName() + _T(".");

        default:
            wxsCodeMarks::Unknown(_T("wxsItem::GetAccessPrefix"),Language);
    }
    return wxEmptyString;
}

bool wxsItem::OnIsPointer()
{
    // TODO: Check in resource data
    return true;
}

wxString wxsItem::Codef(wxsCodingLang Language,const wxChar* Fmt,...)
{
    wxString Result;
    va_list ap;
    va_start(ap,Fmt);

    Codef(Language,Fmt,Result,ap);

    va_end(ap);
    return Result;
}

void wxsItem::Codef(const wxChar* Fmt,...)
{
    wxString* Code = GetResourceData()->GetCurentCode();
    if ( !Code ) return;

    va_list ap;
    va_start(ap,Fmt);

    Codef(GetResourceData()->GetLanguage(),Fmt,*Code,ap);

    va_end(ap);
}

void wxsItem::Codef(wxsCodingLang Language,const wxChar* Fmt,wxString& Result,va_list ap)
{
    wxChar Buff[0x20];
    int Pos;
    wxChar* Char;
    int Dec;
    bool Bool;

    while ( *Fmt )
    {
        if ( *Fmt == _T('%') )
        {
            Fmt++;
            if ( *Fmt )
            {
                if ( !OnCodefExtension(Language,Result,Fmt,ap) )
                {
                    switch ( *Fmt )
                    {
                        case _T('C'):
                            Result << GetCreatePrefix(Language);
                            break;

                        case _T('A'):
                            Result << GetAccessPrefix(Language);
                            break;

                        case _T('W'):
                            switch ( Language )
                            {
                                case wxsCPP: Result << m_WindowParent; break;
                                default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                            }
                            break;

                        case _T('I'):
                            switch ( Language )
                            {
                                case wxsCPP: Result << GetIdName(); break;
                                default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                            }
                            break;

                        case _T('P'):
                            if ( GetPropertiesFlags() && flPosition )
                            {
                                Result << GetBaseProps()->m_Position.GetPositionCode(m_WindowParent,Language);
                            }
                            else
                            {
                                Result << _T("wxDefaultPosition");
                            }
                            break;

                        case _T('S'):
                            if ( GetPropertiesFlags() && flSize )
                            {
                                Result << GetBaseProps()->m_Size.GetSizeCode(m_WindowParent,Language);
                            }
                            else
                            {
                                Result << _T("wxDefaultSize");
                            }
                            break;

                        case _T('V'):
                            Result << _T("wxDefaultValidator");
                            break;

                        case _T('N'):
                            if ( GetPropertiesFlags() && flId )
                            {
                                Result << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false);
                            }
                            else
                            {
                                Result << wxsCodeMarks::WxString(wxsCPP,GetClassName(),false);
                            }
                            break;

                        case _T('v'):
                            Char = va_arg(ap,wxChar*);
                            switch ( Language )
                            {
                                case wxsCPP: Result << Char; break;
                                default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                            }
                            break;

                        case _T('t'):
                            Char = va_arg(ap,wxChar*);
                            Result << wxsCodeMarks::WxString(Language,Char?Char:_T(""),true);
                            break;

                        case _T('u'):
                            Char = va_arg(ap,wxChar*);
                            Result << wxsCodeMarks::WxString(Language,Char?Char:_T(""),false);
                            break;

                        case _T('s'):
                            Char = va_arg(ap,wxChar*);
                            Result << Char;
                            break;

                        case _T('d'):
                            Dec = va_arg(ap,int);
                            Pos = 0;
                            do Buff[Pos++] = _T('0') + (Dec%10), Dec /= 10; while ( Dec );
                            while ( --Pos>=0 ) Result.Append(Buff[Pos]);
                            break;

                        case _T('b'):
                            Bool = va_arg(ap,int)!=0;
                            if ( Bool ) Result << _T("true");
                            else        Result << _T("false");
                            break;

                        default:
                            *Result.Append(*Fmt);
                    }
                    Fmt++;
                }
            }
        }
        else
        {
            Result.Append(*Fmt++);
        }
    }
}

bool wxsItem::OnMouseDClick(wxWindow* Preview,int PosX,int PosY)
{
    // TODO: Create new event / search for current event
    return false;
}

wxString wxsItem::GetUserClass()
{
    if ( GetPropertiesFlags() & flSubclass )
    {
        if ( !m_Subclass.IsEmpty() )
        {
            return m_Subclass;
        }
    }
    return GetClassName();
}

wxString wxsItem::OnGetTreeLabel(int& Image)
{
    if ( GetPropertiesFlags() & flVariable )
    {
        return GetClassName() + _T(": ") + GetVarName();
    }
    else
    {
        return GetClassName();
    }
}
