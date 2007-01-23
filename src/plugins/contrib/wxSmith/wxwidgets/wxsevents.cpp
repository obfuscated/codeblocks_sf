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

#include "wxsevents.h"
#include "wxsitem.h"
#include "wxsitemresdata.h"

#include <messagemanager.h>

#define HandlerXmlElementName   "handler"
#define HandlerXmlEntryName     "entry"
#define HandlerXmlTypeName      "type"
#define HandlerXmlFunctionName  "function"

wxsEvents::wxsEvents(const wxsEventDesc* Events,wxsItem* Item):
    m_Item(Item),
    m_EventArray(Events),
    m_Count(0)
{
    // Counting number of events
    if ( m_EventArray )
    {
        for ( const wxsEventDesc* Event = m_EventArray;
              Event->ET != wxsEventDesc::EndOfList;
              Event++ )
        {
            m_Count++;
        }
    }

    m_Functions.SetCount(m_Count);
}

void wxsEvents::XmlLoadFunctions(TiXmlElement* Element)
{
	for ( TiXmlElement* Handler = Element->FirstChildElement(HandlerXmlElementName);
          Handler;
          Handler = Handler->NextSiblingElement(HandlerXmlElementName) )
    {
    	const char* EntryName    = Handler->Attribute(HandlerXmlEntryName);
    	const char* TypeName     = Handler->Attribute(HandlerXmlTypeName);
    	const char* FunctionName = Handler->Attribute(HandlerXmlFunctionName);

        // There must be function name, otherwise this handler definition is useless
    	if ( !FunctionName ) continue;

    	if ( EntryName )
    	{
    	    // Function given by event entry
    	    wxString Name = cbC2U(EntryName);
    	    for ( int i=0; i<m_Count; i++ )
    	    {
    	        if ( (m_EventArray[i].Entry == Name) && (m_EventArray[i].ET != wxsEventDesc::Category) )
    	        {
    	            m_Functions[i] = cbC2U(FunctionName);
    	            break;
    	        }
    	    }
    	}
    	else
    	{
    	    // Function given by event type
    	    wxString Name = cbC2U(TypeName);
    	    for ( int i=0; i<m_Count; i++ )
    	    {
    	        if ( (m_EventArray[i].Type == Name) && (m_EventArray[i].ET != wxsEventDesc::Category) )
    	        {
    	            m_Functions[i] = cbC2U(FunctionName);
    	        }
    	    }
    	}
    }
}

void wxsEvents::XmlSaveFunctions(TiXmlElement* Element)
{
    for ( int i=0; i<m_Count; i++ )
    {
    	if ( !m_Functions[i].empty() && (m_EventArray[i].ET != wxsEventDesc::Category) )
    	{
    		TiXmlElement* Handler = Element->InsertEndChild( TiXmlElement(HandlerXmlElementName) ) -> ToElement();
    		Handler->SetAttribute(HandlerXmlFunctionName,cbU2C(m_Functions[i]));
    		if ( !m_EventArray[i].Entry.empty() )
    		{
                Handler->SetAttribute(HandlerXmlEntryName,cbU2C(m_EventArray[i].Entry));
    		}
    		else
    		{
    		    Handler->SetAttribute(HandlerXmlTypeName,cbU2C(m_EventArray[i].Type));
    		}
    	}
    }
}

void wxsEvents::GenerateBindingCode(wxString& Code,const wxString& IdString,const wxString& VarNameString,wxsCodingLang Language)
{
    wxString ClassName = m_Item->GetResourceData()->GetClassName();
    switch ( Language )
    {
        case wxsCPP:
        {
            for ( int i=0; i<m_Count; i++ )
            {
                if ( !m_Functions[i].empty() )
                {
                    switch ( m_EventArray[i].ET )
                    {
                        case wxsEventDesc::Id:
                            Code << _T("Connect(") << IdString << _T(",")
                                 << m_EventArray[i].Type << _T(",(wxObjectEventFunction)&")
                                 << ClassName << _T("::") << m_Functions[i] << _T(");\n");
                            break;

                        case wxsEventDesc::NoId:
                            Code << VarNameString << _T("->Connect(") << IdString
                                 << _T(",") << m_EventArray[i].Type
                                 << _T(",(wxObjectEventFunction)&") << ClassName << _T("::") << m_Functions[i]
                                 << _T(",NULL,this);\n");
                            break;

                        default:
                            break;
                    }
                }
            }
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsEvents::GenerateBindingCode"),Language);
        }
    }
}

bool wxsEvents::ForceVariable()
{
    for ( int i=0; i<m_Count; i++ )
    {
        if ( !m_Functions[i].empty() )
        {
            return true;
        }
    }
    return false;
}
