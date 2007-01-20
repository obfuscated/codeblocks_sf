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

#include "wxsproperty.h"
#include "wxspropertygridmanager.h"
#include "wxspropertycontainer.h"

#include <globals.h>

wxsProperty::wxsProperty(const wxString& _PGName, const wxString& _DataName):
    PGName(_PGName), DataName(_DataName)
{
}

long wxsProperty::PGRegister(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId ID,long Index)
{
    wxsPropertyGridManager* wxsGrid = wxDynamicCast(Grid,wxsPropertyGridManager);
    if ( !wxsGrid ) return -1;
    return wxsGrid->Register(Object,this,ID,Index);
}

long wxsProperty::GetPropertiesFlags(wxsPropertyContainer* Object)
{
    return Object->GetPropertiesFlags();
}

bool wxsProperty::XmlGetString(TiXmlElement* Element,wxString& Value,const wxString& SubChild)
{
    Value.Clear();

    if ( !Element )
    {
        return false;
    }

    if ( !SubChild.empty() )
    {
        Element = Element->FirstChildElement(cbU2C(SubChild));
        if ( !Element ) return false;
    }

    const char* Text = Element->GetText();
    if ( !Text )
    {
        // Element does exist but doesn't contain text - in this
        // case we return true, because it's case of empty string
        return true;
    }
    Value = cbC2U(Text);
    return true;
}

void wxsProperty::XmlSetString(TiXmlElement* Element,const wxString& Value,const wxString& SubChild)
{
    if ( !Element )
    {
        return;
    }

    if ( !SubChild.empty() )
    {
        Element = Element->InsertEndChild(TiXmlElement(cbU2C(SubChild)))->ToElement();
        if ( !Element ) return;
    }

    Element->InsertEndChild(TiXmlText(cbU2C(Value)));
}
