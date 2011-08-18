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
#include "SpellCheckerConfig.h"
#include "SpellCheckerPlugin.h"
#include "sdk.h" // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/dir.h>
#endif

#include "configmanager.h"

#define CFG_SPELLCHECK_ENABLE_ONLINE_CHECK _T("/SpellChecker/EnableOnlineChecker")
#define CFG_SPELLCHECK_DICTIONARY_NAME     _T("/SpellChecker/Dictionary")
#define CFG_SPELLCHECK_DICTIONARY_PATH     _T("/SpellChecker/DictPath")
#define CFG_SPELLCHECK_THESAURI_PATH       _T("/SpellChecker/ThesPath")
#define CFG_SPELLCHECK_BITMAPS_PATH        _T("/SpellChecker/BitmPath")

SpellCheckerConfig::SpellCheckerConfig(SpellCheckerPlugin *plugin)
    ://m_DictionaryPath(plugin->GetDic),
    selectedDictionary(-1),
    m_pPlugin(plugin)
{
    Load();
    ScanForDictionaries();
}

SpellCheckerConfig::~SpellCheckerConfig()
{
    //dtor
}

bool SpellCheckerConfig::GetEnableOnlineChecker()
{
    return m_EnableOnlineChecker;
}
void SpellCheckerConfig::SetEnableOnlineChecker(bool val)
{
    m_EnableOnlineChecker = val;
}
const wxString SpellCheckerConfig::GetDictionaryName()const
{
    return m_strDictionaryName;
}
void SpellCheckerConfig::SetDictionaryName(wxString val)
{
    m_strDictionaryName = val;
}
int SpellCheckerConfig::GetSelectedDictionaryNumber()const
{
    return selectedDictionary;
}
void SpellCheckerConfig::ScanForDictionaries()
{
    ScanForDictionaries(GetDictionaryPath());
}
void SpellCheckerConfig::ScanForDictionaries(const wxString &path)
{
    m_dictionaries.clear();
    selectedDictionary = -1;
    //wxString filespec(_T("??_??.dic"));
    wxString filespec(_T("*.dic"));

    wxDir dir;
    if ( dir.Open(path) )
    {
        wxString strfilename;
        bool cont = dir.GetFirst(&strfilename, filespec, wxDIR_FILES );
        while ( cont )
        {
            wxFileName fname(strfilename);
            wxString afffilename = path + wxFileName::GetPathSeparator() + fname.GetName() + _T(".aff");
            if ( wxFileName::FileExists(afffilename) )
            {
                if ( fname.GetName() == m_strDictionaryName )
                    selectedDictionary = m_dictionaries.size();
                m_dictionaries.push_back(fname.GetName());

            }
            cont = dir.GetNext(&strfilename);
        }
    }
    // disable online checker if there are no dictionaries found
    if (m_dictionaries.empty())
    {
      m_EnableOnlineChecker = false;
    }
}
const std::vector<wxString> &SpellCheckerConfig::GetPossibleDictionaries()const
{
    return m_dictionaries;
}

const wxString SpellCheckerConfig::GetDictionaryPath()const{return m_DictPath;}
const wxString SpellCheckerConfig::GetThesaurusPath()const{return m_ThesPath;}
const wxString SpellCheckerConfig::GetBitmapPath()const{return m_BitmPath;}
void SpellCheckerConfig::SetDictionaryPath(const wxString &path){m_DictPath = path;}
void SpellCheckerConfig::SetThesaurusPath(const wxString &path){m_ThesPath = path;}
void SpellCheckerConfig::SetBitmapPath(const wxString &path){m_BitmPath = path;}

const wxString SpellCheckerConfig::GetPersonalDictionaryFilename()const
{
    wxString dfile = ConfigManager::LocateDataFile(GetDictionaryName() + _T("_personaldictionary.dic"), sdConfig );
    if (dfile == _T(""))
        dfile = ConfigManager::GetFolder(sdConfig) + wxFILE_SEP_PATH + GetDictionaryName() + _T("_personaldictionary.dic");
    return dfile;
}
void SpellCheckerConfig::Load()
{
    m_EnableOnlineChecker = true;
    m_strDictionaryName = _T("de_CH");
    m_DictPath = m_pPlugin->GetOnlineCheckerConfigPath();
    m_ThesPath = m_pPlugin->GetOnlineCheckerConfigPath();
    m_BitmPath = m_pPlugin->GetOnlineCheckerConfigPath();
    if (ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor")))
    {
        m_EnableOnlineChecker = cfg->ReadBool(CFG_SPELLCHECK_ENABLE_ONLINE_CHECK, true);
        m_strDictionaryName = cfg->Read(CFG_SPELLCHECK_DICTIONARY_NAME, _T("de_CH") );
        m_DictPath = cfg->Read(CFG_SPELLCHECK_DICTIONARY_PATH, m_pPlugin->GetOnlineCheckerConfigPath());
        m_ThesPath = cfg->Read(CFG_SPELLCHECK_THESAURI_PATH, m_pPlugin->GetOnlineCheckerConfigPath());
        m_BitmPath = cfg->Read(CFG_SPELLCHECK_BITMAPS_PATH, m_pPlugin->GetOnlineCheckerConfigPath());
    }
}
void SpellCheckerConfig::Save()
{
    if(ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor")))
    {
        cfg->Write(CFG_SPELLCHECK_ENABLE_ONLINE_CHECK, m_EnableOnlineChecker);
        cfg->Write(CFG_SPELLCHECK_DICTIONARY_NAME, m_strDictionaryName);
        cfg->Write(CFG_SPELLCHECK_DICTIONARY_PATH, m_DictPath);
        cfg->Write(CFG_SPELLCHECK_THESAURI_PATH, m_ThesPath);
        cfg->Write(CFG_SPELLCHECK_BITMAPS_PATH, m_BitmPath);
    }
    m_pPlugin->ReloadSettings();
}
