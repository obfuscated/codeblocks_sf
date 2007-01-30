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

#include "wxscontainer.h"
#include <messagemanager.h>

wxsContainer::wxsContainer(
    wxsItemResData* Data,
    const wxsItemInfo* Info,
    const wxsEventDesc* EventArray,
    const wxsStyleSet* StyleSet,
    long PropertiesFlags):
        wxsParent(Data,Info,PropertiesFlags,EventArray),
        m_StyleSet(StyleSet),
        m_StyleBits(0),
        m_ExStyleBits(0)
{
    if ( m_StyleSet )
    {
        m_StyleBits = m_StyleSet->GetDefaultBits(false);
        m_ExStyleBits = m_StyleSet->GetDefaultBits(true);
    }
}

bool wxsContainer::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetType() == wxsTSpacer )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("Spacer can be added into sizer only"));
        }
        return false;
    }

    if ( Item->GetType() == wxsTSizer )
    {
        if ( GetChildCount() > 0 )
        {
            if ( GetChild(0)->GetType() == wxsTSizer )
            {
                if ( ShowMessage )
                {
                    wxMessageBox(_("This item can contain only one sizer"));
                }
                return false;
            }
            else
            {
                if ( ShowMessage )
                {
                    wxMessageBox(_("Item can not contain sizer if it has other items inside"));
                }
                return false;
            }
        }
        return true;
    }

    if ( GetChildCount() > 0 )
    {
        if ( GetChild(0)->GetType() == wxsTSizer )
        {
            if ( ShowMessage )
            {
                wxMessageBox(_("Item contains sizer, can not add other items into it."));
            }
            return false;
        }
    }

    return true;
}

void wxsContainer::OnEnumItemProperties(long Flags)
{
    OnEnumContainerProperties(Flags);
    if ( m_StyleSet )
    {
        WXS_STYLE(wxsContainer,m_StyleBits,0,_("Style"),_T("style"),m_StyleSet);
        WXS_EXSTYLE(wxsContainer,m_ExStyleBits,0,_("Extra style"),_T("exstyle"),m_StyleSet);
    }
}

void wxsContainer::OnAddItemQPP(wxsAdvQPP* QPP)
{
    OnAddContainerQPP(QPP);
}

wxWindow* wxsContainer::SetupWindow(wxWindow* Preview,long Flags)
{
    GetBaseProps()->SetupWindow(Preview,Flags);
    long ExStyle = m_StyleSet->GetWxStyle(m_ExStyleBits,true);
    if ( ExStyle != 0 )
    {
        Preview->SetExtraStyle(Preview->GetExtraStyle() | ExStyle);
    }
    return Preview;
}

void wxsContainer::SetupWindowCode(wxString& Code,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            GetBaseProps()->BuildSetupWindowCode(Code,GetVarName(),GetAccessPrefix(wxsCPP),wxsCPP);
            if ( m_ExStyleBits )
            {
                wxString ExStyleStr = m_StyleSet->GetString(m_ExStyleBits,true,wxsCPP);
                if ( ExStyleStr != _T("0") )
                {
                    Code << GetAccessPrefix(wxsCPP) << _T("SetExtraStyle(") <<
                            GetAccessPrefix(wxsCPP) << _T("GetExtraStyle() | ") <<
                            ExStyleStr << _T(");\n");
                }
            }
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsContainer::SetupWindowCode"),Language);
        }
    }

}

void wxsContainer::AddChildrenPreview(wxWindow* This,long Flags)
{
    for ( int i=0; i<GetChildCount(); i++ )
    {
        GetChild(i)->BuildPreview(This,Flags);
    }

    if ( GetBaseProps()->m_Size.IsDefault )
    {
        This->Fit();
    }
}

void wxsContainer::AddChildrenCode(wxString& Code,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxString ThisName = GetParent()?GetVarName():_T("this");
                GetChild(i)->BuildCreatingCode(Code,ThisName,wxsCPP);
                if ( GetChild(i)->GetType() == wxsTSizer )
                {
                    wxString SizerName = GetChild(i)->GetVarName();
                    if ( GetParent() )
                    {
                        Code << GetVarName() << _T("->");
                    }

                    Code << _T("SetSizer(") << SizerName << _T(");\n");

                    if ( GetBaseProps()->m_Size.IsDefault )
                    {
                        Code << SizerName << _T("->Fit(") << ThisName << _T(");\n");
                    }

                    Code << SizerName << _T("->SetSizeHints(") << ThisName << _T(");\n");
                }
            }
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsContainer::AddChildrenCode"),Language);
        }
    }
}

bool wxsContainer::OnCodefExtension(wxsCodingLang Language,wxString& Result,const wxChar* &FmtChar,va_list ap)
{
    if ( *FmtChar == _T('T') )
    {
        Result << StyleCode(Language);
        FmtChar++;
        return true;
    }
    return wxsParent::OnCodefExtension(Language,Result,FmtChar,ap);
}
