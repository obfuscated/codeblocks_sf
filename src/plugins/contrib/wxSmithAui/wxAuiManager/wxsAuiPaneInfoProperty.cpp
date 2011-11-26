/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#include "wxsAuiPaneInfoProperty.h"

#include <wx/tokenzr.h>
#include <globals.h>

//{ wxsAuiDockableProperty

// Helper macro for fetching variables
#define DOCKABLEFLAGS   wxsVARIABLE(Object,Offset,long)

#define DOCKABLEIND     0x01

wxsAuiDockableProperty::wxsAuiDockableProperty(long _Offset,int Priority):
        wxsProperty(_("PaneInfo"),_T("dockabledirection"),Priority),
        Offset(_Offset)
{
}

void wxsAuiDockableProperty::PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent)
{
    wxPGId ID1;

    wxPGChoices PGC1;
    PGC1.Add(_("Top"), TopDockable);
    PGC1.Add(_("Bottom"), BottomDockable);
    PGC1.Add(_("Left"), LeftDockable);
    PGC1.Add(_("Right"), RightDockable);
    PGC1.Add(_("Dockable (All)"), Dockable);
    PGRegister(Object,Grid,ID1 = Grid->AppendIn(Parent,NEW_IN_WXPG14X wxFlagsProperty(_("AUI Dockable"),wxPG_LABEL,PGC1,DOCKABLEFLAGS&(DockableMask))),DOCKABLEIND);

    Grid->SetPropertyAttribute(ID1,wxPG_BOOL_USE_CHECKBOX,1L,wxPG_RECURSE);
}

bool wxsAuiDockableProperty::PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case DOCKABLEIND:
            long DockableValue;
            bool PreviousDockable;
            DockableValue = Grid->GetPropertyValue(Id).GetLong();
            PreviousDockable = DOCKABLEFLAGS & Dockable;

            DOCKABLEFLAGS &= ~DockableMask;

            if ( !(DockableValue&Dockable) && PreviousDockable)
                break;
            else if ( ((DockableValue & DockableMask) ==  DockableAll) |
                      ((DockableValue & Dockable)     && !PreviousDockable) )
            {
                DOCKABLEFLAGS |= Dockable;
            }
            else
                DOCKABLEFLAGS |= ( (DockableValue & DockableMask) & ~Dockable );
            break;

        default:
            return false;
    }
    return true;
}

bool wxsAuiDockableProperty::PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Id,long Index)
{
    switch ( Index )
    {
        case DOCKABLEIND:
            long DockableValue;
            DockableValue = DOCKABLEFLAGS&DockableMask;

            if (DockableValue == Dockable)
                Grid->SetPropertyValue(Id,DockableMask);
            else
                Grid->SetPropertyValue(Id,DockableValue);
            break;

        default:
            return false;
    }
    return true;
}

bool wxsAuiDockableProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        DOCKABLEFLAGS = DockableMask;
        return false;
    }

    const char* Text = Element->GetText();
    wxString Str;
    if ( !Text )
    {
        DOCKABLEFLAGS = DockableMask;
        return false;
    }
    DOCKABLEFLAGS = ParseString(cbC2U(Text));
    return true;
}

bool wxsAuiDockableProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if (!Element) return;
    Element->InsertEndChild(TiXmlText(cbU2C(GetString(DOCKABLEFLAGS))));
    return true;
}

bool wxsAuiDockableProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    if ( Stream && Stream->GetLong(GetDataName(),DOCKABLEFLAGS) )
        return true;

    return false;
}

bool wxsAuiDockableProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return ( Stream && Stream->PutLong(GetDataName(),DOCKABLEFLAGS) );
}

long wxsAuiDockableProperty::ParseString(const wxString& String)
{
    long Flags = DockableAll;
    wxStringTokenizer Tkn(String, _T(". \t\n"), wxTOKEN_STRTOK);

    while ( Tkn.HasMoreTokens() )
    {
        wxString Flag = Tkn.GetNextToken();

             if ( Flag == _T("TopDockable(false)")    ) Flags &= ~TopDockable;
        else if ( Flag == _T("BottomDockable(false)") ) Flags &= ~BottomDockable;
        else if ( Flag == _T("LeftDockable(false)")   ) Flags &= ~LeftDockable;
        else if ( Flag == _T("RightDockable(false)")  ) Flags &= ~RightDockable;
        else if ( Flag == _T("Dockable(false)")       ) Flags &= ~DockableMask;
    }

    if ( Flags == DockableAll ) Flags |= Dockable;
    return Flags;
}

wxString wxsAuiDockableProperty::GetString(long Flags)
{
    wxString Result;

    if ( Flags == 0 ) Result.Append(_T(".Dockable(false)"));
    else if ( !(Flags & Dockable) )
    {
        if ( !(Flags & TopDockable)    ) Result.Append(_T(".TopDockable(false)"));
        if ( !(Flags & BottomDockable) ) Result.Append(_T(".BottomDockable(false)"));
        if ( !(Flags & LeftDockable)   ) Result.Append(_T(".LeftDockable(false)"));
        if ( !(Flags & RightDockable)  ) Result.Append(_T(".RightDockable(false)"));
    }

    return Result;
}

void wxsAuiDockableProperty::GetDockableFlags(wxAuiPaneInfo& PaneInfo, long Flags)
{
    if ( Flags == 0 ) PaneInfo.Dockable(false);
    else if ( !(Flags & Dockable) )
    {
        if ( !(Flags & TopDockable)    ) PaneInfo.TopDockable(false);
        if ( !(Flags & BottomDockable) ) PaneInfo.BottomDockable(false);
        if ( !(Flags & LeftDockable)   ) PaneInfo.LeftDockable(false);
        if ( !(Flags & RightDockable)  ) PaneInfo.RightDockable(false);
    }
}

//} wxsAuiDockableProperty

//{ wxsFirstTimeProperty

// Helper macro for fetching variable
#define VALUE   wxsVARIABLE(Object,Offset,bool)

wxsFirstAddProperty::wxsFirstAddProperty(const wxString& PGName,const wxString& DataName,long _Offset,bool _Default,int Priority):
    wxsProperty(PGName,DataName,Priority),
    Offset(_Offset),
    Default(_Default)
{
}

bool wxsFirstAddProperty::XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( !Element )
    {
        VALUE = Default;
        return false;
    }
    const char* Text = Element->GetText();
    if ( !Text )
    {
        VALUE = Default;
        return false;
    }
    VALUE = atol(Text) != 0;

    return true;
}

bool wxsFirstAddProperty::XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element)
{
    if ( Element && (VALUE != Default) )
    {
        Element->InsertEndChild(TiXmlText(VALUE?"1":"0"));
        return true;
    }

    return false;
}

bool wxsFirstAddProperty::PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return ( Stream && Stream->GetBool(GetDataName(),VALUE,Default) );
}

bool wxsFirstAddProperty::PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream)
{
    return ( Stream && Stream->PutBool(GetDataName(),VALUE,Default) );
}

//} wxsFirstAddProperty
