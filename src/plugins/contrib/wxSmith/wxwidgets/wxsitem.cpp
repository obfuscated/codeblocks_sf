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
        WXS_STRING(wxsItem,m_Subclass,flSubclass,_("Class name"),_T("subclass"),wxEmptyString,false,false);
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
        if ( m_Subclass.IsEmpty() )
        {
            m_Subclass = GetClassName();
        }
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
            if ( !m_Subclass.IsEmpty() && (m_Subclass!=GetClassName()) )
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

    Codef(Language,m_WindowParent,Fmt,Result,ap);

    va_end(ap);
    return Result;
}

void wxsItem::Codef(const wxChar* Fmt,...)
{
    wxString* Code = GetResourceData()->GetCurentCode();
    if ( !Code ) return;

    va_list ap;
    va_start(ap,Fmt);

    Codef(GetResourceData()->GetLanguage(),m_WindowParent,Fmt,*Code,ap);

    va_end(ap);
}

void wxsItem::Codef(wxsCodingLang Language,wxString WindowParent,const wxChar* Fmt,wxString& Result,va_list ap)
{
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
                        case _T('A'):
                        {
                            Result << GetAccessPrefix(Language);
                            break;
                        }

                        case _T('C'):
                        {
                            Result << GetCreatePrefix(Language);
                            break;
                        }

                        case _T('E'):
                        {
                            if ( GetParent() && (GetParent()->GetPropertiesFlags()&flVariable) )
                            {
                                if ( GetParent()->IsPointer() )
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << GetParent()->GetVarName(); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                                else
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << _T("(&") << GetParent()->GetVarName() << _T(")"); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                            }
                            break;
                        }

                        case _T('F'):
                        {
                            if ( GetParent() && (GetParent()->GetPropertiesFlags()&flVariable) )
                            {
                                if ( GetParent()->IsPointer() )
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << _T("(*") << GetParent()->GetVarName() << _T(")"); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                                else
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << GetParent()->GetVarName(); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                            }
                            break;
                        }

                        case _T('I'):
                        {
                            switch ( Language )
                            {
                                case wxsCPP: Result << GetIdName(); break;
                                default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                            }
                            break;
                        }

                        case _T('M'):
                        {
                            if ( GetParent() )
                            {
                                Result << GetParent()->GetAccessPrefix(Language);
                            }
                            break;
                        }

                        case _T('N'):
                        {
                            if ( GetPropertiesFlags() & flId )
                            {
                                Result << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false);
                            }
                            else
                            {
                                Result << wxsCodeMarks::WxString(wxsCPP,GetClassName(),false);
                            }
                            break;
                        }

                        case _T('O'):
                        {
                            if ( IsRootItem() )
                            {
                                switch ( Language )
                                {
                                    case wxsCPP: Result << _T("this"); break;
                                    default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                }
                            }
                            else if ( GetPropertiesFlags() & flVariable )
                            {
                                if ( IsPointer() )
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << GetVarName(); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                                else
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << _T("(&") << GetVarName() << _T(")"); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                            }
                            break;
                        }

                        case _T('P'):
                        {
                            if ( GetPropertiesFlags() & flPosition )
                            {
                                Result << GetBaseProps()->m_Position.GetPositionCode(m_WindowParent,Language);
                            }
                            else
                            {
                                Result << _T("wxDefaultPosition");
                            }
                            break;
                        }

                        case _T('R'):
                        {
                            if ( IsRootItem() )
                            {
                                switch ( Language )
                                {
                                    case wxsCPP: Result << _T("(*this)"); break;
                                    default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                }
                            }
                            else if ( GetPropertiesFlags() & flVariable )
                            {
                                if ( IsPointer() )
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << _T("(*") << GetVarName() << _T(")"); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                                else
                                {
                                    switch ( Language )
                                    {
                                        case wxsCPP: Result << GetVarName(); break;
                                        default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                    }
                                }
                            }
                            break;
                        }

                        case _T('S'):
                        {
                            if ( GetPropertiesFlags() & flSize )
                            {
                                Result << GetBaseProps()->m_Size.GetSizeCode(m_WindowParent,Language);
                            }
                            else
                            {
                                Result << _T("wxDefaultSize");
                            }
                            break;
                        }

                        /* _T('T') is processed in derived classes */

                        case _T('V'):
                        {
                            Result << _T("wxDefaultValidator");
                            break;
                        }

                        case _T('W'):
                        {
                            switch ( Language )
                            {
                                case wxsCPP: Result << m_WindowParent; break;
                                default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                            }
                            break;
                        }

                        case _T('b'):
                        {
                            bool Bool = va_arg(ap,int)!=0;
                            if ( Bool ) Result << _T("true");
                            else        Result << _T("false");
                            break;
                        }

                        case _T('d'):
                        {
                            wxChar Buff[0x20];
                            int Dec = va_arg(ap,int);
                            if ( Dec < 0 )
                            {
                                Result.Append(_T('-'));
                                Dec = -Dec;
                            }
                            int Pos = 0;
                            do Buff[Pos++] = _T('0') + (Dec%10), Dec /= 10; while ( Dec );
                            while ( --Pos>=0 ) Result.Append(Buff[Pos]);
                            break;
                        }

                        case _T('i'):
                        {
                            wxsBitmapIconData* Image = va_arg(ap,wxsBitmapIconData*);
                            if ( Image )
                            {
                                wxChar* ArtProvider = va_arg(ap,wxChar*);
                                if ( !ArtProvider ) ArtProvider = _T("wxART_OTHER");
                                if ( !Image->IsEmpty() )
                                {
                                    Result << Image->BuildCode(true,_T("wxDefaultSize"),Language,ArtProvider);
                                }
                                else
                                {
                                    Result << _T("wxNullBitmap");
                                }
                            }
                            break;
                        }

                        case _T('n'):
                        {
                            wxChar* String = va_arg(ap,wxChar*);
                            Result << wxsCodeMarks::WxString(Language,String?String:_T(""),false);
                            break;
                        }

                        case _T('p'):
                        {
                            wxsPositionData* Pos = va_arg(ap,wxsPositionData*);
                            Result << Pos->GetPositionCode(m_WindowParent,Language);
                            break;
                        }

                        case _T('s'):
                        {
                            wxChar* String = va_arg(ap,wxChar*);
                            if ( String )
                            {
                                Result << String;
                            }
                            break;
                        }

                        case _T('t'):
                        {
                            wxChar* String = va_arg(ap,wxChar*);
                            Result << wxsCodeMarks::WxString(Language,String?String:_T(""),true);
                            break;
                        }

                        case _T('v'):
                        {
                            wxChar* Var = va_arg(ap,wxChar*);
                            if ( Var )
                            {
                                switch ( Language )
                                {
                                    case wxsCPP: Result << Var; break;
                                    default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                }
                            }
                            break;
                        }

                        case _T('z'):
                        {
                            wxsSizeData* Size = va_arg(ap,wxsSizeData*);
                            Result << Size->GetSizeCode(m_WindowParent,Language);
                            break;
                        }

                        default:
                        {
                            *Result.Append(*Fmt);
                        }
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

void wxsItem::BuildSetupWindowCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            long PropertiesFlags = GetPropertiesFlags();

            if ( PropertiesFlags&flMinMaxSize )
            {
                if ( !m_BaseProperties->m_MinSize.IsDefault )
                {
                    Code << Codef(Language,_T("%ASetMinSize(%z);\n"),&m_BaseProperties->m_MinSize);
                }

                if ( !m_BaseProperties->m_MaxSize.IsDefault )
                {
                    Code << Codef(Language,_T("%ASetMaxSize(%z);\n"),&m_BaseProperties->m_MaxSize);
                }
            }

            if ( (PropertiesFlags&flEnabled) && !m_BaseProperties->m_Enabled ) Code << Codef(Language,_T("%ADisable();\n"));
            if ( (PropertiesFlags&flFocused) && m_BaseProperties->m_Focused  ) Code << Codef(Language,_T("%ASetFocus();\n"));
            if ( (PropertiesFlags&flHidden)  && m_BaseProperties->m_Hidden   ) Code << Codef(Language,_T("%AHide();\n"));

            if ( PropertiesFlags&flColours )
            {
                wxString FGCol = m_BaseProperties->m_Fg.BuildCode(Language);
                if ( !FGCol.empty() ) Code << GetAccessPrefix(Language) << _T("SetForegroundColour(") << FGCol << _T(");\n");

                wxString BGCol = m_BaseProperties->m_Bg.BuildCode(Language);
                if ( !BGCol.empty() ) Code << GetAccessPrefix(Language) << _T("SetBackgroundColour(") << BGCol << _T(");\n");
            }

            if ( PropertiesFlags&flFont )
            {
                wxString FontVal = m_BaseProperties->m_Font.BuildFontCode(GetVarName() + _T("Font"), Language);
                if ( !FontVal.empty() )
                {
                    Code << FontVal;
                    Code << GetAccessPrefix(Language) << _T("SetFont(") << GetVarName() << _T("Font);\n");
                }
            }

            if ( (PropertiesFlags&flToolTip)  && !m_BaseProperties->m_ToolTip.IsEmpty()  ) Code << GetAccessPrefix(Language) << _T("SetToolTip(") << wxsCodeMarks::WxString(wxsCPP,m_BaseProperties->m_ToolTip) << _T(");\n");
            if ( (PropertiesFlags&flHelpText) && !m_BaseProperties->m_HelpText.IsEmpty() ) Code << GetAccessPrefix(Language) << _T("SetHelpText(") << wxsCodeMarks::WxString(wxsCPP,m_BaseProperties->m_HelpText) << _T(");\n");
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItem::BuildSetupWindowCode"),Language);
        }
    }
}

void wxsItem::SetupWindow(wxWindow* Window,long Flags)
{
    bool IsExact = (Flags&wxsItem::pfExact) != 0;
    long PropertiesFlags = GetPropertiesFlags();

    if ( PropertiesFlags&flMinMaxSize && IsExact )
    {
        if ( !m_BaseProperties->m_MinSize.IsDefault )
        {
            Window->SetMinSize(m_BaseProperties->m_MinSize.GetSize(Window->GetParent()));
        }

        if ( !m_BaseProperties->m_MaxSize.IsDefault )
        {
            Window->SetMaxSize(m_BaseProperties->m_MaxSize.GetSize(Window->GetParent()));
        }
    }

    if ( (PropertiesFlags&flEnabled) && !m_BaseProperties->m_Enabled ) Window->Disable();
    if ( (PropertiesFlags&flFocused) && m_BaseProperties->m_Focused  ) Window->SetFocus();
    if ( (PropertiesFlags&flHidden)  && m_BaseProperties->m_Hidden && IsExact ) Window->Hide();

    if ( PropertiesFlags&flColours )
    {
        wxColour FGCol = m_BaseProperties->m_Fg.GetColour();
        if ( FGCol.Ok() ) Window->SetForegroundColour(FGCol);
        wxColour BGCol = m_BaseProperties->m_Bg.GetColour();
        if ( BGCol.Ok() ) Window->SetBackgroundColour(BGCol);
    }

    if ( PropertiesFlags&flFont )
    {
        wxFont FontVal = m_BaseProperties->m_Font.BuildFont();
        if ( FontVal.Ok() ) Window->SetFont(FontVal);
    }

    if ( (PropertiesFlags&flToolTip)  && !m_BaseProperties->m_ToolTip.empty() ) Window->SetToolTip(m_BaseProperties->m_ToolTip);
    if ( (PropertiesFlags&flHelpText) && !m_BaseProperties->m_HelpText.empty() ) Window->SetHelpText(m_BaseProperties->m_HelpText);
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
