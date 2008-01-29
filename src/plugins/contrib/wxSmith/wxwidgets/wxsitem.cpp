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

#include "wxsitem.h"
#include "wxsparent.h"
#include "wxsitemresdata.h"
#include "wxsitemrestreedata.h"
#include "wxseventseditor.h"
#include "wxsitemeditor.h"
#include <wx/menu.h>

using namespace wxsFlags;

namespace
{
    long IdToFront = wxNewId();
    long IdToBottom = wxNewId();
}

wxsItem::wxsItem(wxsItemResData* ResourceData,const wxsItemInfo* Info,long PropertiesFlags,const wxsEventDesc* Events,const wxsStyleSet* StyleSet):
    m_Info(Info),
    m_Events(Events,this),
    m_Parent(0),
    m_ResourceData(ResourceData),
    m_PropertiesFlags(PropertiesFlags),
    m_LastPreview(0),
    m_IsSelected(false),
    m_IsExpanded(false)
{
    m_BaseProperties.m_Subclass = GetClassName();
    m_BaseProperties.m_StyleSet = StyleSet;
    if ( StyleSet )
    {
        m_BaseProperties.m_StyleBits = StyleSet->GetDefaultBits(false);
        m_BaseProperties.m_ExStyleBits = StyleSet->GetDefaultBits(true);
    }
}

wxsItem::~wxsItem()
{
}

void wxsItem::OnEnumProperties(long Flags)
{
    if ( (Flags & flPropGrid) && (m_Parent != 0) )
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

    if ( m_Parent != 0 )
    {
        // Parent item does take care of inserting QPP Children
        m_Parent->OnAddChildQPP(this,Panel);
    }
    else
    {
        OnAddItemQPP(Panel);
    }

    // Appending qpp's from base properties
    m_BaseProperties.AddQPPChild(Panel,GetPropertiesFlags());

    return Panel;
}

long wxsItem::OnGetPropertiesFlags()
{
    long ExtraFlags = 0;

    if ( IsRootItem() )   ExtraFlags |= flRoot;
    if ( IsPointer()  )   ExtraFlags |= flPointer;
    if ( !GetIsMember() ) ExtraFlags |= flLocal;

    return m_ResourceData->GetPropertiesFilter() | m_PropertiesFlags | ExtraFlags;
}

void wxsItem::EnumItemProperties(long Flags)
{
    OnEnumItemProperties(Flags);

    // Now enumerating all properties from wxsBaseProperties
    SubContainer(m_BaseProperties,Flags);
}

void wxsItem::OnBuildDeclarationsCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
            AddDeclaration(GetUserClass() + (IsPointer() ? _T("* ") : _T(" ") ) + GetVarName() + _T(";"));
            return;

        default:
            wxsCodeMarks::Unknown(_T("wxsItem::OnBuildDeclarationsCode"),GetLanguage());
    }
}

void wxsItem::OnBuildEventsConnectingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            wxsEvents& Events = GetEvents();
            wxString IdString;
            wxString VarNameString;
            if ( IsRootItem() )
            {
                IdString = _T("wxID_ANY");
            }
            else if ( GetCoderFlags() & flSource )
            {
                IdString = GetIdName();
                VarNameString = GetVarName();
            }
            else
            {
                IdString = _T("XRCID(\"") + GetIdName() + _T("\")");
                if ( GetIsMember() )
                {
                    VarNameString = GetVarName();
                }
                else
                {
                    VarNameString = _T("FindWindow(XRCID(\"") + GetIdName() + _T("\"))");
                }
            }

            Events.GenerateBindingCode(GetCoderContext(),IdString,VarNameString);
            return;
        }
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItem::OnBuildEventsConnectingCode"),GetLanguage());
        }
    }
}

void wxsItem::OnBuildIdCode()
{
    if ( !(GetPropertiesFlags() & flId) ) return;

    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            wxString Name = GetIdName();
            if ( !wxsPredefinedIDs::Check(Name) )
            {
                wxString Enumeration = _T("static const long ") + Name + _T(";");
                wxString Initialization =  + _T("const long ") + GetResourceData()->GetClassName() + _T("::") + Name + _T(" = wxNewId();");
                if ( GetCoderContext()->m_IdEnumerations.Index(Enumeration) == wxNOT_FOUND )
                {
                    AddIdCode(Enumeration,Initialization);
                }
            }
            return;
        }

        default:
            wxsCodeMarks::Unknown(_T("wxsItem::OnBuildIdCode"),GetLanguage());
    }
}

void wxsItem::OnBuildXRCFetchingCode()
{
    long Flags = GetPropertiesFlags();
    if ( (Flags&flVariable) && (Flags&flId) )
    {
        AddXRCFetchingCode(
            GetVarName() + _T(" = ")
            _T("(") + GetUserClass() + _T("*)")
            _T("FindWindow(XRCID(\"") + GetIdName() + _T("\"));\n"));
    }
}

void wxsItem::BuildSetupWindowCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            long PropertiesFlags = GetPropertiesFlags();

            if ( PropertiesFlags&flMinMaxSize )
            {
                if ( !m_BaseProperties.m_MinSize.IsDefault )
                {
                    Codef(_T("%ASetMinSize(%z);\n"),&m_BaseProperties.m_MinSize);
                }

                if ( !m_BaseProperties.m_MaxSize.IsDefault )
                {
                    Codef(_T("%ASetMaxSize(%z);\n"),&m_BaseProperties.m_MaxSize);
                }
            }

            if ( (PropertiesFlags&flEnabled) && !m_BaseProperties.m_Enabled ) Codef(_T("%ADisable();\n"));
            if ( (PropertiesFlags&flFocused) && m_BaseProperties.m_Focused  ) Codef(_T("%ASetFocus();\n"));
            if ( (PropertiesFlags&flHidden)  && m_BaseProperties.m_Hidden   ) Codef(_T("%AHide();\n"));

            if ( PropertiesFlags&flColours )
            {
                wxString FGCol = m_BaseProperties.m_Fg.BuildCode(GetCoderContext());
                if ( !FGCol.empty() ) Codef(_T("%ASetForegroundColour(%s);\n"),FGCol.c_str());

                wxString BGCol = m_BaseProperties.m_Bg.BuildCode(GetCoderContext());
                if ( !BGCol.empty() ) Codef(_T("%ASetBackgroundColour(%s);\n"),BGCol.c_str());
            }

            if ( PropertiesFlags&flFont )
            {
                wxString FontVal = m_BaseProperties.m_Font.BuildFontCode(GetVarName() + _T("Font"), GetCoderContext());
                if ( !FontVal.empty() )
                {
                    Codef(_T("%s%ASetFont(%sFont);\n"),FontVal.c_str(),GetVarName().c_str());
                }
            }

            if ( (PropertiesFlags&flToolTip)  && !m_BaseProperties.m_ToolTip.IsEmpty()  )   Codef(_T("%ASetToolTip(%t);\n"),m_BaseProperties.m_ToolTip.c_str());
            if ( (PropertiesFlags&flHelpText) && !m_BaseProperties.m_HelpText.IsEmpty() )   Codef(_T("%ASetHelpText(%t);\n"),m_BaseProperties.m_HelpText.c_str());
            if ( (PropertiesFlags&flExtraCode) && !m_BaseProperties.m_ExtraCode.IsEmpty() )
            {
                wxString& ExtraCode = m_BaseProperties.m_ExtraCode;
                AddBuildingCode(ExtraCode);
                // Adding extra \n character if it's not presend in extra code
                if ( ExtraCode.GetChar(ExtraCode.Length()-1) != _T('\n') )
                {
                    AddBuildingCode(_T("\n"));
                }
            }

            if ( m_BaseProperties.m_StyleSet && m_BaseProperties.m_ExStyleBits )
            {
                wxString ExStyleStr = m_BaseProperties.m_StyleSet->GetString(m_BaseProperties.m_ExStyleBits,true,wxsCPP);
                if ( ExStyleStr != _T("0") )
                {
                    Codef(_T("%ASetExtraStyle( %AGetExtraStyle() | %s );\n"),ExStyleStr.c_str());
                }
            }

            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsItem::BuildSetupWindowCode"),GetLanguage());
        }
    }
}

bool wxsItem::OnXmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        wxsPropertyContainer::XmlRead(Element);
    }

    if ( IsExtra )
    {
        m_Events.XmlLoadFunctions(Element);
    }

    m_BaseProperties.SpecialXmlRead(Element,IsXRC,IsExtra);

    // Some small fix to subclass property
    if ( m_BaseProperties.m_Subclass.IsEmpty() )
    {
        m_BaseProperties.m_Subclass = GetClassName();
    }

    return true;
}

bool wxsItem::OnXmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        Element->SetAttribute("class",cbU2C(GetClassName()));
        wxsPropertyContainer::XmlWrite(Element);
    }

    if ( IsExtra )
    {
        m_Events.XmlSaveFunctions(Element);
    }

    m_BaseProperties.SpecialXmlWrite(Element,IsXRC,IsExtra,GetPropertiesFlags(),GetClassName());

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
//
//    if ( GetIsExpanded() )
//    {
//        Tree->Expand(m_LastTreeId);
//    }
//    else
//    {
//        Tree->Collapse(m_LastTreeId);
//    }
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
    UpdateTreeLabel();
}

void wxsItem::OnSubPropertyChanged(wxsPropertyContainer*)
{
    // Forwarding this to standard property changed handler
    OnPropertyChanged();
}

void wxsItem::OnAddExtraProperties(wxsPropertyGridManager* Grid)
{
    if ( !(GetPropertiesFlags() & flFile) )
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

void wxsItem::SetVarName(const wxString& NewName)
{
    m_BaseProperties.m_VarName = NewName;
    UpdateTreeLabel();
}

void wxsItem::UpdateTreeLabel()
{
    wxsResourceItemId Id;
    if ( GetResourceData()->GetTreeId(Id,this) )
    {
        int IconId;
        wxString NewLabel = OnGetTreeLabel(IconId);
        if ( wxsResourceTree::Get()->GetItemText(Id) != NewLabel )
        {
            wxsResourceTree::Get()->SetItemText(Id,NewLabel);
        }
    }
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

wxString wxsItem::Codef(wxsCoderContext* Context,const wxChar* Fmt,...)
{
    wxString Result;
    va_list ap;
    va_start(ap,Fmt);

    Codef(Context,Fmt,Result,ap);

    va_end(ap);
    return Result;
}

void wxsItem::Codef(const wxChar* Fmt,...)
{
    if ( !GetCoderContext() )
    {
        // TODO: Debug log
        return;
    }

    va_list ap;
    va_start(ap,Fmt);

    Codef(GetCoderContext(),Fmt,GetCoderContext()->m_BuildingCode,ap);

    va_end(ap);
}

void wxsItem::Codef(wxsCoderContext* Context,const wxChar* Fmt,wxString& Result,va_list ap)
{
    if ( !Context ) return;

    wxsCodingLang Language = Context->m_Language;
    wxString WindowParent = Context->m_WindowParent;
    long Flags = Context->m_Flags;

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
                            else if ( GetType()==wxsTTool && !IsRootItem() )
                            {
                                // For tools let's return this
                                switch ( Language )
                                {
                                    case wxsCPP: Result << _T("this"); break;
                                    default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
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
                            else if ( GetType()==wxsTTool && !IsRootItem() )
                            {
                                // For tools let's return *this
                                switch ( Language )
                                {
                                    case wxsCPP: Result << _T("(*this)"); break;
                                    default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
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
                            if ( Flags & flId )
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
                            else if ( Flags & flVariable )
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
                            if ( Flags & flPosition )
                            {
                                Result << m_BaseProperties.m_Position.GetPositionCode(Context);
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
                            else if ( Flags & flVariable )
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
                            if ( Flags & flSize )
                            {
                                Result << m_BaseProperties.m_Size.GetSizeCode(Context);
                            }
                            else
                            {
                                Result << _T("wxDefaultSize");
                            }
                            break;
                        }

                        case _T('T'):
                        {
                            if ( m_BaseProperties.m_StyleSet )
                            {
                                Result << m_BaseProperties.m_StyleSet->GetString(m_BaseProperties.m_StyleBits,false,Language);
                            }
                            else
                            {
                                Result << _T("0");
                            }
                            break;
                        }

                        case _T('V'):
                        {
                            Result << _T("wxDefaultValidator");
                            break;
                        }

                        case _T('W'):
                        {
                            switch ( Language )
                            {
                                case wxsCPP: Result << WindowParent; break;
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

                        case _T('c'):
                        {
                            wxChar Ch = (wxChar)va_arg(ap,int);
                            Result << Ch;
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
                                const wxChar* ArtProvider = va_arg(ap,wxChar*);
                                if ( !ArtProvider ) ArtProvider = _T("wxART_OTHER");
                                if ( !Image->IsEmpty() )
                                {
                                    Result << Image->BuildCode(true,_T("wxDefaultSize"),GetCoderContext(),ArtProvider);
                                }
                                else
                                {
                                    Result << _T("wxNullBitmap");
                                }
                            }
                            break;
                        }

                        case _T('l'):
                        {
                            wxsColourData* Data = va_arg(ap,wxsColourData*);
                            wxString Code = Data->BuildCode(GetCoderContext());
                            if ( Code.IsEmpty() )
                            {
                                Result << _T("wxNullColour");
                            }
                            else
                            {
                                Result << Code;
                            }
                            break;
                        }

                        case _T('n'):
                        {
                            wxChar* String = va_arg(ap,wxChar*);
                            Result << wxsCodeMarks::WxString(Language,String?String:_T(""),false);
                            break;
                        }

                        case _T('o'):
                        {
                            int ChildIndex = va_arg(ap,int);
                            wxsParent* This = ConvertToParent();
                            if ( This && ChildIndex>=0 && ChildIndex<This->GetChildCount() )
                            {
                                wxsItem* Child = This->GetChild(ChildIndex);
                                if ( Child->GetPropertiesFlags() & flVariable )
                                {
                                    if ( Child->IsPointer() )
                                    {
                                        switch ( Language )
                                        {
                                            case wxsCPP: Result << Child->GetVarName(); break;
                                            default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                        }
                                    }
                                    else
                                    {
                                        switch ( Language )
                                        {
                                            case wxsCPP: Result << _T("(&") << Child->GetVarName() << _T(")"); break;
                                            default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                        }
                                    }
                                    break;
                                }
                            }
                            Result << _T("0");
                            break;
                        }

                        case _T('p'):
                        {
                            wxsPositionData* Pos = va_arg(ap,wxsPositionData*);
                            Result << Pos->GetPositionCode(Context);
                            break;
                        }

                        case _T('r'):
                        {
                            int ChildIndex = va_arg(ap,int);
                            wxsParent* This = ConvertToParent();
                            if ( This && ChildIndex>=0 && ChildIndex<This->GetChildCount() )
                            {
                                wxsItem* Child = This->GetChild(ChildIndex);
                                if ( Child->GetPropertiesFlags() & flVariable )
                                {
                                    if ( Child->IsPointer() )
                                    {
                                        switch ( Language )
                                        {
                                            case wxsCPP: Result << _T("(*") << Child->GetVarName() << _T(")"); break;
                                            default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                        }
                                    }
                                    else
                                    {
                                        switch ( Language )
                                        {
                                            case wxsCPP: Result << Child->GetVarName(); break;
                                            default: wxsCodeMarks::Unknown(_T("wxString wxsItem::Codef"),Language);
                                        }
                                    }
                                    break;
                                }
                            }
                            Result << _T("0");
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
                            Result << Size->GetSizeCode(Context);
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

wxWindow* wxsItem::SetupWindow(wxWindow* Window,long Flags)
{
    bool IsExact = (Flags&pfExact) != 0;
    long PropertiesFlags = GetPropertiesFlags();

    if ( PropertiesFlags&flMinMaxSize && IsExact )
    {
        if ( !m_BaseProperties.m_MinSize.IsDefault )
        {
            Window->SetMinSize(m_BaseProperties.m_MinSize.GetSize(Window->GetParent()));
        }

        if ( !m_BaseProperties.m_MaxSize.IsDefault )
        {
            Window->SetMaxSize(m_BaseProperties.m_MaxSize.GetSize(Window->GetParent()));
        }
    }

    if ( (PropertiesFlags&flEnabled) && !m_BaseProperties.m_Enabled ) Window->Disable();
    if ( (PropertiesFlags&flFocused) && m_BaseProperties.m_Focused  ) Window->SetFocus();
    if ( (PropertiesFlags&flHidden)  && m_BaseProperties.m_Hidden && IsExact ) Window->Hide();

    if ( PropertiesFlags&flColours )
    {
        wxColour FGCol = m_BaseProperties.m_Fg.GetColour();
        if ( FGCol.Ok() ) Window->SetForegroundColour(FGCol);
        wxColour BGCol = m_BaseProperties.m_Bg.GetColour();
        if ( BGCol.Ok() ) Window->SetBackgroundColour(BGCol);
    }

    if ( PropertiesFlags&flFont )
    {
        wxFont FontVal = m_BaseProperties.m_Font.BuildFont();
        if ( FontVal.Ok() ) Window->SetFont(FontVal);
    }

    if ( (PropertiesFlags&flToolTip)  && !m_BaseProperties.m_ToolTip.empty() ) Window->SetToolTip(m_BaseProperties.m_ToolTip);
    if ( (PropertiesFlags&flHelpText) && !m_BaseProperties.m_HelpText.empty() ) Window->SetHelpText(m_BaseProperties.m_HelpText);

    long ExStyle = m_BaseProperties.m_StyleSet ? m_BaseProperties.m_StyleSet->GetWxStyle(m_BaseProperties.m_ExStyleBits,true) : 0;
    if ( ExStyle != 0 )
    {
        Window->SetExtraStyle(Window->GetExtraStyle() | ExStyle);
    }
    return Window;
}

wxString wxsItem::GetUserClass()
{
    if ( GetPropertiesFlags() & flSubclass )
    {
        if ( !m_BaseProperties.m_Subclass.IsEmpty() )
        {
            return m_BaseProperties.m_Subclass;
        }
    }
    return GetClassName();
}

wxString wxsItem::OnGetTreeLabel(int& Image)
{
    if ( GetPropertiesFlags()&flVariable && !IsRootItem() )
    {
        return GetClassName() + _T(": ") + GetVarName();
    }
    else
    {
        return GetClassName();
    }
}

wxString wxsItem::GetIdName()
{
    wxString Code = m_BaseProperties.m_IdName;
    if ( IsRootItem() )
    {
        if ( GetPropertiesFlags()&flSource && !m_BaseProperties.m_IdFromArg )
        {
            // TODO: Do we use the same identifier in other coding languages ?
            Code = _T("wxID_ANY");
        }
        else
        {
            Code = _T("id");
        }
    }
    return Code;
}

bool wxsItem::OnMouseDClick(wxWindow* Preview,int PosX,int PosY)
{
    // Search for first event (skip all categories)
    for ( int i=0; i<GetEvents().GetCount(); i++ )
    {
        switch ( GetEvents().GetDesc(i)->ET )
        {
            case wxsEventDesc::Category:
            case wxsEventDesc::EndOfList:
                break;

            default:
                // Found event, now let's try to add handler for it
                return wxsEventsEditor::Get().GotoOrBuildEvent(this,i,wxsPGRID());
        }
    }

    return false;
}

bool wxsItem::OnMouseRightClick(wxWindow* Preview,int PosX,int PosY)
{
    wxMenu Popup;
    OnPreparePopup(&Popup);
    if ( Popup.GetMenuItemCount() )
    {
        ShowPopup(&Popup);
    }
    return false;
}

void wxsItem::OnPreparePopup(wxMenu* Popup)
{
    if ( GetType() != wxsTSizer )
    {
        if ( GetParent() && GetParent()->GetType() != wxsTSizer )
        {
            wxMenuItem* Item = Popup->Append(IdToFront,_("Bring to front"));
            if ( GetParent()->GetChildIndex(this) == GetParent()->GetChildCount()-1 )
            {
                Item->Enable(false);
            }
            Item = Popup->Append(IdToBottom,_("Send to back"));
            if ( GetParent()->GetChildIndex(this) == 0 )
            {
                Item->Enable(false);
            }
        }
    }
}

bool wxsItem::OnPopup(long Id)
{
    if ( Id == IdToFront )
    {
        GetResourceData()->BeginChange();
        GetParent()->MoveChild(GetParent()->GetChildIndex(this),GetParent()->GetChildCount()-1);
        GetResourceData()->EndChange();
        return true;
    }

    if ( Id == IdToBottom )
    {
        GetResourceData()->BeginChange();
        GetParent()->MoveChild(GetParent()->GetChildIndex(this),0);
        GetResourceData()->EndChange();
        return true;
    }

    return false;
}

void wxsItem::ShowPopup(wxMenu* Menu)
{
    if ( GetResourceData()->GetEditor() )
    {
        GetResourceData()->GetEditor()->ShowPopup(this,Menu);
    }
}

void wxsItem::OnUpdateFlags(long& Flags)
{
    Flags = GetPropertiesFlags();
}
