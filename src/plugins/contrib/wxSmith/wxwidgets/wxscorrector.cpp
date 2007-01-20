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

#include "wxscorrector.h"
#include "wxsitem.h"
#include "wxsparent.h"
#include "wxsitemresdata.h"

wxsCorrector::wxsCorrector(wxsItemResData* Data):
    m_Data(Data),
    m_NeedRebuild(true)
{
}

bool wxsCorrector::GlobalCheck()
{
    // It will be done in two passes,
    // first will correct all invalid names, second will fill empty names

    wxsItem* RootItem = m_Data->GetRootItem();
    m_Vars.clear();
    m_Ids.clear();
    bool AreInvalid = FixAfterLoadCheckNames(RootItem);
    bool AreEmpty = FillEmpty(RootItem);

    m_NeedRebuild = false;

    if ( AreInvalid || AreEmpty ) return true;
    return false;
}

bool wxsCorrector::FixAfterLoadCheckNames(wxsItem* Item)
{
    bool Ret = false;
    if ( Item->GetPropertiesFlags() & wxsItem::flVariable )
    {
        wxString VarName = Item->GetVarName();
        if ( FixVarName(VarName) )
        {
            Ret = true;
            Item->SetVarName(VarName);
        }
        if ( m_Vars.find(VarName) != m_Vars.end() )
        {
            Ret = true;
            Item->SetVarName(wxEmptyString);
        }
        else
        {
            m_Vars.insert(VarName);
        }
    }
    if ( Item->GetPropertiesFlags() & wxsItem::flId )
    {
        wxString IdName = Item->GetIdName();
        if ( FixIdName(IdName) )
        {
            Ret = true;
            Item->SetIdName(IdName);
        }

        if ( m_Ids.find(IdName)!=m_Ids.end() )
        {
            Ret = true;
            Item->SetIdName(wxEmptyString);
        }
        else
        {
            m_Ids.insert(IdName);
        }
    }

    wxsParent* Parent = Item->ConvertToParent();
    if ( Parent )
    {
        int Count = Parent->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            if ( FixAfterLoadCheckNames(Parent->GetChild(i)) )
            {
                Ret = true;
            }
        }
    }
    return Ret;
}

bool wxsCorrector::FillEmpty(wxsItem* Item)
{
    bool Ret = false;
    if ( Item->GetPropertiesFlags() & wxsItem::flVariable )
    {
        if ( Item->GetVarName().empty() )
        {
            Ret = true;
            SetNewVarName(Item);
            m_Vars.insert(Item->GetVarName());
        }
    }
    if ( Item->GetPropertiesFlags() & wxsItem::flId )
    {
        if ( Item->GetIdName().empty() )
        {
            Ret = true;
            SetNewIdName(Item);
            m_Ids.insert(Item->GetIdName());
        }
    }

    wxsParent* Parent = Item->ConvertToParent();
    if ( Parent )
    {
        int Count = Parent->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            if ( FillEmpty(Parent->GetChild(i)) ) Ret = true;
        }
    }
    return Ret;
}

void wxsCorrector::AfterChange(wxsItem* Item)
{
    // Building new sets without given item
    wxsItem* RootItem = m_Data->GetRootItem();
    m_Vars.clear();
    m_Ids.clear();
    RebuildSetsReq(RootItem,Item);

    if ( Item->GetPropertiesFlags() & wxsItem::flVariable )
    {
        wxString VarName = Item->GetVarName();
        if ( FixVarName(VarName) )
        {
            Item->SetVarName(VarName);
        }
        if ( VarName.empty() || (m_Vars.find(VarName) != m_Vars.end()) )
        {
            SetNewVarName(Item);
        }
        m_Vars.insert(Item->GetVarName());
    }

    if ( Item->GetPropertiesFlags() & wxsItem::flId )
    {
        wxString IdName = Item->GetIdName();
        if ( FixIdName(IdName) )
        {
            Item->SetIdName(IdName);
        }

        if ( m_Ids.empty() || (m_Ids.find(IdName) != m_Ids.end()) )
        {
            SetNewIdName(Item);
        }
        m_Ids.insert(Item->GetIdName());
    }

    m_NeedRebuild = false;
}

void wxsCorrector::RebuildSets()
{
    // TODO: Uncomment when data integrity is sure
//    if ( !m_NeedRebuild ) return;
    m_Vars.clear();
    m_Ids.clear();
    RebuildSetsReq(m_Data->GetRootItem(),NULL);
    m_NeedRebuild = false;
}

void wxsCorrector::RebuildSetsReq(wxsItem* Item,wxsItem* Exclude)
{
    if ( Item != Exclude )
    {
        if ( Item->GetPropertiesFlags() & wxsItem::flVariable )
        {
            m_Vars.insert(Item->GetVarName());
        }

        if ( Item->GetPropertiesFlags() & wxsItem::flId )
        {
            wxString Id = Item->GetIdName();
            m_Ids.insert(Id);
        }
    }

    wxsParent* Parent = Item->ConvertToParent();
    if ( Parent )
    {
        for ( int i=Parent->GetChildCount(); i-->0; )
        {
            RebuildSetsReq(Parent->GetChild(i),Exclude);
        }
    }
}

void wxsCorrector::SetNewVarName(wxsItem* Item)
{
    wxString Prefix = Item->GetInfo().DefaultVarName;
    wxString NewName;
    for ( int i=1;; i++ )
    {
        NewName = wxString::Format(_T("%s%d"),Prefix.c_str(),i);
        if ( m_Vars.find(NewName) == m_Vars.end() ) break;
    }
    Item->SetVarName(NewName);
}

void wxsCorrector::SetNewIdName(wxsItem* Item)
{
    wxString Prefix = _T("ID_");
    Prefix << Item->GetInfo().DefaultVarName.Upper();
    wxString NewName;
    for ( int i=1;; i++ )
    {
        NewName = wxString::Format(_T("%s%d"),Prefix.c_str(),i);
        if ( m_Ids.find(NewName) == m_Ids.end() ) break;
    }
    Item->SetIdName(NewName);
}

void wxsCorrector::BeforePaste(wxsItem* Item)
{
    RebuildSets();
    BeforePasteReq(Item);
}

void wxsCorrector::BeforePasteReq(wxsItem* Item)
{
    if ( Item->GetPropertiesFlags() & wxsItem::flVariable )
    {
        wxString VarName = Item->GetVarName();
        if ( FixVarName(VarName) )
        {
            Item->SetVarName(VarName);
        }
        if ( VarName.empty() || (m_Vars.find(VarName) != m_Vars.end()) )
        {
            SetNewVarName(Item);
        }
        m_Vars.insert(Item->GetVarName());
    }

    if ( Item->GetPropertiesFlags() & wxsItem::flId )
    {
        wxString IdName = Item->GetIdName();
        if ( FixIdName(IdName) )
        {
            Item->SetIdName(IdName);
        }
        if ( m_Ids.empty() || (m_Ids.find(IdName) != m_Ids.end()) )
        {
            SetNewIdName(Item);
        }
        m_Ids.insert(Item->GetIdName());
    }

    wxsParent* Parent = Item->ConvertToParent();
    if ( Parent )
    {
        int Count = Parent->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            BeforePasteReq(Parent->GetChild(i));
        }
    }
}

void wxsCorrector::AfterPaste(wxsItem* Item)
{
    // TODO: Add new items into current sets
}

bool wxsCorrector::FixVarName(wxString& Name)
{
    wxString Corrected;
    Name.Trim(true);
   	Name.Trim(false);

   	if ( !Name.empty() )
   	{
        // Validating name as C++ ideentifier
        // TODO: Other languages ?

        static wxString FirstChar(
            _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
            _T("abcdefghijklmnopqrstuvwxyz")
            _T("_"));

        if ( FirstChar.Find(Name.GetChar(0)) == -1 )
        {
            DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),Name.c_str(),Name.GetChar(0),0);
        }
        else
        {
            Corrected.Append(Name.GetChar(0));
        }

        static wxString NextChars(
            _T("0123456789")
            _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
            _T("abcdefghijklmnopqrstuvwxyz")
            _T("_"));

        for ( size_t i=1; i<Name.Length(); ++i )
        {
            if ( NextChars.Find(Name.GetChar(i)) == -1 )
            {
                DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),Name.c_str(),Name.GetChar(i),i);
            }
            else
            {
                Corrected.Append(Name.GetChar(i));
            }
        }
   	}

   	bool Diff = Name != Corrected;
   	Name = Corrected;
   	return Diff;
}

bool wxsCorrector::FixIdName(wxString& Id)
{
    Id.Trim(true);
    Id.Trim(false);

    long Tmp;
    if ( Id.ToLong(&Tmp,10) ) return false;

    // We'll use FixVarName's routines to correct identifier
    return FixVarName(Id);
}

void wxsCorrector::ClearCache()
{
    m_NeedRebuild = true;
    m_Vars.clear();
    m_Ids.clear();
}
