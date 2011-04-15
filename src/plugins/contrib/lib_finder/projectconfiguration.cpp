/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#include "projectconfiguration.h"
#include <wx/hashset.h>
#include <wx/string.h>

//namespace
//{
//    WX_DECLARE_HASH_SET(wxString,wxStringHash,wxStringEq,wxStringSet);
//}

ProjectConfiguration::ProjectConfiguration()
{
}

ProjectConfiguration::~ProjectConfiguration()
{
}

void ProjectConfiguration::XmlLoad(TiXmlElement* Node,cbProject* Project)
{
    m_GlobalUsedLibs.Clear();
    m_TargetsUsedLibs.clear();
    m_DisableAuto = false;

    TiXmlElement* LibFinder = Node->FirstChildElement("lib_finder");
    if ( !LibFinder ) return;

    int noauto = 0;
    if ( (LibFinder->QueryIntAttribute("disable_auto",&noauto) == TIXML_SUCCESS) && noauto )
    {
        m_DisableAuto = true;
    }

    for ( TiXmlElement* Elem = LibFinder->FirstChildElement("lib");
          Elem;
          Elem = Elem->NextSiblingElement("lib") )
    {
        wxString LibName = cbC2U(Elem->Attribute("name"));
        if ( !LibName.IsEmpty() && m_GlobalUsedLibs.Index(LibName)==wxNOT_FOUND )
        {
            m_GlobalUsedLibs.Add(LibName);
        }
    }

    for ( TiXmlElement* Elem = LibFinder->FirstChildElement("target");
          Elem;
          Elem = Elem->NextSiblingElement("target") )
    {
        wxString TargetName = cbC2U(Elem->Attribute("name"));
        if ( !Project->GetBuildTarget(TargetName) ) continue;

        wxArrayString& Libs = m_TargetsUsedLibs[TargetName];
        for ( TiXmlElement* LibElem = Elem->FirstChildElement("lib");
              LibElem;
              LibElem = LibElem->NextSiblingElement("lib") )
        {
            wxString LibName = cbC2U(LibElem->Attribute("name"));
            if ( !LibName.IsEmpty() && Libs.Index(LibName)==wxNOT_FOUND )
            {
                Libs.Add(LibName);
            }
        }
    }

    // TODO: Invoke library settings check
}

void ProjectConfiguration::XmlWrite(TiXmlElement* Node,cbProject* Project)
{
    TiXmlElement* LibFinder = Node->FirstChildElement("lib_finder");
    if ( !LibFinder ) LibFinder = Node->InsertEndChild(TiXmlElement("lib_finder"))->ToElement();

    LibFinder->Clear();

    if ( m_DisableAuto )
    {
        LibFinder->SetAttribute("disable_auto","1");
    }

    for ( size_t i=0; i<m_GlobalUsedLibs.Count(); i++ )
    {
        LibFinder->InsertEndChild(TiXmlElement("lib"))->ToElement()->SetAttribute("name",cbU2C(m_GlobalUsedLibs[i]));
    }

    for ( wxMultiStringMap::iterator i=m_TargetsUsedLibs.begin();
          i!=m_TargetsUsedLibs.end();
          ++i )
    {
        if ( !Project->GetBuildTarget(i->first) ) continue;

        wxArrayString& Libs = i->second;
        if ( Libs.Count() )
        {
            TiXmlElement* TargetElem = LibFinder->InsertEndChild(TiXmlElement("target"))->ToElement();
            TargetElem->SetAttribute("name",cbU2C(i->first));
            for ( size_t i=0; i<Libs.Count(); i++ )
            {
                TargetElem->InsertEndChild(TiXmlElement("lib"))->ToElement()->SetAttribute("name",cbU2C(Libs[i]));
            }
        }
    }

    if ( !LibFinder->FirstAttribute() && !LibFinder->FirstChild() )
    {
        // LibFinder is empty, let's delete it so it doesn't trash here
        Node->RemoveChild(LibFinder);
    }
}
