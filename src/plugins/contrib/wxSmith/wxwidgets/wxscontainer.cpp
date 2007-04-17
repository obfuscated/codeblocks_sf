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
#include "wxsitemresdata.h"
#include "wxstool.h"
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
    static const int Priority = 50;

    OnEnumContainerProperties(Flags);
    if ( m_StyleSet )
    {
        WXS_STYLE_P(wxsContainer,m_StyleBits,_("Style"),_T("style"),m_StyleSet,Priority);
        WXS_EXSTYLE_P(wxsContainer,m_ExStyleBits,_("Extra style"),_T("exstyle"),m_StyleSet,Priority);
    }
}

void wxsContainer::OnAddItemQPP(wxsAdvQPP* QPP)
{
    OnAddContainerQPP(QPP);
}

wxWindow* wxsContainer::SetupWindow(wxWindow* Preview,long Flags)
{
    wxsItem::SetupWindow(Preview,Flags);
    long ExStyle = m_StyleSet->GetWxStyle(m_ExStyleBits,true);
    if ( ExStyle != 0 )
    {
        Preview->SetExtraStyle(Preview->GetExtraStyle() | ExStyle);
    }
    return Preview;
}

void wxsContainer::SetupWindowCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            BuildSetupWindowCode(Code,WindowParent,wxsCPP);
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
//    for ( int i=0; i<GetChildCount(); i++ )
//    {
//        wxsItem* Child = GetChild(i);
//        Child->BuildPreview(This,Flags);
//        if ( Child->GetType() == wxsTSizer )
//        {
//
//            This->SetSizer(
//            Code << GetAccessPrefix(Language)
//                 << _T("SetSizer(")
//                 // TODO: Fix, child may not be pointer
//                 << Child->GetVarName()
//                 << _T(");\n");
//        }
//    }
//
//    if ( GetBaseProps()->m_Size.IsDefault )
//    {
//        This->Fit();
//    }

    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsItem* Child = GetChild(i);
        wxObject* ChildPreviewAsObject = Child->BuildPreview(This,Flags);
        if ( Child->GetType() == wxsTSizer )
        {
            wxSizer* ChildPreviewAsSizer = wxDynamicCast(ChildPreviewAsObject,wxSizer);
            if ( ChildPreviewAsSizer )
            {
                This->SetSizer(ChildPreviewAsSizer);
            }
        }
    }

    if ( IsRootItem() )
    {
        // Adding all tools before calling Fit and SetSizeHints()

        wxsItemResData* Data = GetResourceData();
        if ( Data )
        {
            for ( int i=0; i<Data->GetToolsCount(); i++ )
            {
                Data->GetTool(i)->BuildPreview(This,Flags);
            }
        }

    }

    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsItem* Child = GetChild(i);
        if ( Child->GetType() == wxsTSizer )
        {
            wxObject* ChildPreviewAsObject = Child->GetLastPreview();
            wxSizer*  ChildPreviewAsSizer  = wxDynamicCast(ChildPreviewAsObject,wxSizer);
            wxWindow* ChildPreviewAsWindow = wxDynamicCast(ChildPreviewAsObject,wxWindow);

            if ( ChildPreviewAsSizer )
            {
                // Child preview was created directly as sizer, we use it to
                // call Fit() and SetSizeHints() directly
                if ( GetBaseProps()->m_Size.IsDefault )
                {
                    ChildPreviewAsSizer->Fit(This);
                }
                ChildPreviewAsSizer->SetSizeHints(This);
            }
            else if ( ChildPreviewAsWindow )
            {
                // Preview of sizer is given actually as some kind of panel which paints
                // some extra data of sizer. So we have to create out own sizer to call
                // Fit and SetSizeHints

                wxSizer* IndirectSizer = new wxBoxSizer(wxHORIZONTAL);
                IndirectSizer->Add(ChildPreviewAsWindow,1,wxEXPAND,0);
                This->SetSizer(IndirectSizer);

                if ( GetBaseProps()->m_Size.IsDefault )
                {
                    IndirectSizer->Fit(This);
                }

                IndirectSizer->SetSizeHints(This);
            }
        }
    }
}

void wxsContainer::AddChildrenCode(wxString& Code,wxsCodingLang Language)
{
    // TODO: Convert for Access-safe code
    switch ( Language )
    {
        case wxsCPP:
        {
            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsItem* Child = GetChild(i);
                Child->BuildCreatingCode(Code,GetVarName(),Language);
                if ( Child->GetType() == wxsTSizer )
                {
                    Code << GetAccessPrefix(Language)
                         << _T("SetSizer(")
                         // TODO: Fix, child may not be pointer
                         << Child->GetVarName()
                         << _T(");\n");
                }
            }

            if ( IsRootItem() )
            {
                // Adding all tools before calling Fit and SetSizeHints()

                wxsItemResData* Data = GetResourceData();
                if ( Data )
                {
                    for ( int i=0; i<Data->GetToolsCount(); i++ )
                    {
                        Data->GetTool(i)->BuildCreatingCode(Code,GetVarName(),Language);
                    }
                }

            }

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsItem* Child = GetChild(i);
                if ( Child->GetType() == wxsTSizer )
                {
                    if ( GetBaseProps()->m_Size.IsDefault )
                    {
                        Code << Child->GetAccessPrefix(Language) << _T("Fit(") << Codef(Language,_T("%O")) << _T(");\n");
                    }

                    Code << Child->GetAccessPrefix(Language) << _T("SetSizeHints(") << Codef(Language,_T("%O")) << _T(");\n");
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
