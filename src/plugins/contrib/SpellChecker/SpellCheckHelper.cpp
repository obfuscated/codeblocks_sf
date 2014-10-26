/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include "SpellCheckHelper.h"

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/wxscintilla.h>

    #include <configmanager.h>
    #include <logmanager.h>
#endif

#include "SpellCheckerPlugin.h"

SpellCheckHelper::SpellCheckHelper()
{
    //ctor
    LoadConfiguration();
}

SpellCheckHelper::~SpellCheckHelper()
{
    //dtor
}

bool SpellCheckHelper::IsWhiteSpace(const wxChar &ch)
{
    return wxIsspace(ch) || wxIspunct(ch) || wxIsdigit(ch);
}

void SpellCheckHelper::LoadConfiguration()
{
    //TiXmlDocument doc("OnlineSpellChecking.xml");
    wxString fname = SpellCheckerPlugin::GetOnlineCheckerConfigPath() + wxFILE_SEP_PATH + _T("OnlineSpellChecking.xml");
    TiXmlDocument doc( fname.char_str() );
    if(  !doc.LoadFile() )
        Manager::Get()->GetLogManager()->Log( _("SpellCheck Plugin: Error loading Online SpellChecking Configuration file \"") + fname +_T("\"") );

    TiXmlNode *rootnode =  doc.FirstChildElement( "OnlineSpellCheckingConfigurationFile" );
    if ( rootnode )
    {
        for ( TiXmlElement* LangElement = rootnode->FirstChildElement("Language");
              LangElement;
              LangElement = LangElement->NextSiblingElement("Language") )
        {
            wxString name  = wxString( LangElement->Attribute("name"), wxConvUTF8 );
            // comma-separated indices
            wxString index = wxString ( LangElement->Attribute("index"), wxConvUTF8 );
            // break-up array
            wxArrayString indices = GetArrayFromString(index, _T(","));
            std::set<long> idcs;
            for (size_t i = 0; i < indices.GetCount(); ++i)
            {
            	if (indices[i].IsEmpty())
                    continue;
                long value = 0;
                indices[i].ToLong(&value);
                idcs.insert(value);
            }
            if ( idcs.size() > 0)
                m_LanguageIndices[name] = idcs;
        }
    }
}

bool SpellCheckHelper::HasStyleToBeChecked(wxString langname, int style)const
{
    std::map<wxString, std::set<long> >::const_iterator it = m_LanguageIndices.find(langname);
    if ( it != m_LanguageIndices.end())
    {
        return it->second.find(style) != it->second.end();
    }
    return false;
}
bool SpellCheckHelper::IsEscapeSequenceStart(wxChar ch, wxString langname, int style)
{
    //Manager::Get()->GetLogManager()->Log(wxString(_T("check if '")) + ch +_T("' is an escape in \"")+langname + wxString::Format(_T("\" at style %d"), style));
    if (langname == _T("C/C++") && (style == wxSCI_C_STRING || style == wxSCI_C_CHARACTER || style == wxSCI_C_STRINGEOL))
    {
        return ch == _T('\\');
    }
    return false;
}
