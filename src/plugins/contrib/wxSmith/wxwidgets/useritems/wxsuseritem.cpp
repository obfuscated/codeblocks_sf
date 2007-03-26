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

#include "wxsuseritem.h"
#include "wxsuseritemdescription.h"
#include "../wxsitemresdata.h"

wxsUserItem::wxsUserItem(
    wxsItemResData* Data,
    wxsUserItemDescription* Description,
    wxsItemInfo* Info,
    wxsGenericPropertyValueList* Properties,
    const wxBitmap& PreviewBitmap,
    const wxSize& DefaultSize):
        wxsWidget(Data,Info,NULL,NULL),
        m_PreviewBitmap(PreviewBitmap),        // Do not use GetSubBitmap to get immediate result on change of original bitmap
        m_DefaultSize(DefaultSize),
        m_Properties(Properties),
        m_Description(Description)
{
    m_Description->AddReference();
}

wxsUserItem::~wxsUserItem()
{
    delete m_Properties;
    m_Description->DecReference(false);
}

void wxsUserItem::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    wxString Generated;
    wxString VarName;
    switch ( Language )
    {
        case wxsCPP:

            for ( size_t i=0; i<m_CppCodeTemplate.Length(); i++ )
            {
                wxChar Ch = m_CppCodeTemplate.GetChar(i);

                if ( ( (Ch >= _T('A')) && (Ch <= _T('Z')) ) ||
                     ( (Ch >= _T('a')) && (Ch <= _T('z')) ) ||
                     ( (Ch >= _T('0')) && (Ch <= _T('9')) ) ||
                     ( (Ch == _T('_')) ) )
                {
                    VarName << Ch;
                }
                else
                {
                    if ( !VarName.IsEmpty() )
                    {
                        Generated << ExpandCodeVariable(VarName.MakeUpper());
                    }
                    Generated << Ch;
                    VarName.Clear();
                }
            }
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsUserItem::OnBuildCreatingCode"),Language);
            return;
    }
    Code << Generated;
}

wxString wxsUserItem::ExpandCodeVariable(const wxString& VarName)
{
    wxsCodingLang Language = GetResourceData()->GetLanguage();
    if ( VarName == _T("PARENT")    ) return Codef(Language,_T("%W"));
    if ( VarName == _T("CREATE")    ) return Codef(Language,_T("%C"));
    if ( VarName == _T("ACCESS")    ) return Codef(Language,_T("%A"));
    if ( VarName == _T("THIS")      ) return Codef(Language,_T("%O"));
    if ( VarName == _T("POS")       ) return Codef(Language,_T("%P"));
    if ( VarName == _T("SIZE")      ) return Codef(Language,_T("%S"));
    if ( VarName == _T("ID")        ) return Codef(Language,_T("%I"));
    if ( VarName == _T("VALIDATOR") ) return Codef(Language,_T("%V"));
    if ( VarName == _T("NAME")      ) return Codef(Language,_T("%N"));

    // Searching for name of variable inside properties
    wxString Value;
    if ( m_Properties->GetPropertyValue(VarName,Value,Language,Codef(Language,_T("%W"))) )
    {
        return Value;
    }

    // No translation found, returning string as it is
    return VarName;
}

wxObject* wxsUserItem::OnBuildPreview(wxWindow* Parent,long Flags)
{
    return new wxStaticBitmap(Parent,GetId(),m_PreviewBitmap,Pos(Parent),GetBaseProps()->m_Size.IsDefault ? m_DefaultSize : Size(Parent));
}

void wxsUserItem::OnEnumWidgetProperties(long Flags)
{
}

void wxsUserItem::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            WX_APPEND_ARRAY(Decl,m_CppDeclarationHeaders);
            WX_APPEND_ARRAY(Def,m_CppDefinitionHeaders);
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsUserItem::OnEnumDeclFiles"),Language);
    }
}

void wxsUserItem::AddLanguage(wxsCodingLang Language,const wxString& CodeTemplate,const wxArrayString& DeclarationHeaders,const wxArrayString& DefinitionHeaders)
{
    switch ( Language )
    {
        case wxsCPP:
            m_CppCodeTemplate = CodeTemplate;
            m_CppDeclarationHeaders = DeclarationHeaders;
            m_CppDefinitionHeaders = DefinitionHeaders;
            break;

        default:;
    }
}
