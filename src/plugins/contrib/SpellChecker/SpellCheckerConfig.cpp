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
#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include <macrosmanager.h>
#endif

#include <configmanager.h>

#define CFG_SPELLCHECK_ENABLE_ONLINE_CHECK      _T("/SpellChecker/EnableOnlineChecker")
#define CFG_SPELLCHECK_SPELL_TOOLTIPS_CHECK     _T("/SpellChecker/SpellTooltips")
#define CFG_SPELLCHECK_THESAURUS_TOOLTIPS_CHECK _T("/SpellChecker/ThesTooltips")
#define CFG_SPELLCHECK_DICTIONARY_NAME          _T("/SpellChecker/Dictionary")
#define CFG_SPELLCHECK_DICTIONARY_PATH          _T("/SpellChecker/DictPath")
#define CFG_SPELLCHECK_THESAURI_PATH            _T("/SpellChecker/ThesPath")
#define CFG_SPELLCHECK_BITMAPS_PATH             _T("/SpellChecker/BitmPath")

SpellCheckerConfig::SpellCheckerConfig(SpellCheckerPlugin *plugin)
    ://m_DictionaryPath(plugin->GetDic),
    selectedDictionary(-1),
    m_pPlugin(plugin)
{
    Load();
    ScanForDictionaries();
    PopulateLanguageNamesMap();
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
bool SpellCheckerConfig::GetEnableSpellTooltips()
{
    return m_EnableSpellTooltips;
}
void SpellCheckerConfig::SetEnableSpellTooltips(bool val)
{
    m_EnableSpellTooltips = val;
}
bool SpellCheckerConfig::GetEnableThesaurusTooltips()
{
    return m_EnableThesaurusTooltips;
}
void SpellCheckerConfig::SetEnableThesaurusTooltips(bool val)
{
    m_EnableThesaurusTooltips = val;
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
const wxString SpellCheckerConfig::GetDictionaryPath()const
{
    wxArrayString dictPaths;
    dictPaths.Add(m_DictPath);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dictPaths[0]);
    if (platform::windows)
    {
        wxString programs = wxT("C:\\Program Files");
        wxGetEnv(wxT("ProgramFiles"), &programs);
        dictPaths.Add(programs + wxT("\\Mozilla Firefox\\dictionaries"));
        dictPaths.Add(programs + wxT("\\Mozilla\\Firefox\\dictionaries"));
        dictPaths.Add(programs + wxT("\\Mozilla Thunderbird\\dictionaries"));
        dictPaths.Add(programs + wxT("\\Mozilla\\Thunderbird\\dictionaries"));
        wxString libreOffice = wxFindFirstFile(programs + wxT("\\*LibreOffice*"), wxDIR);
        wxString openOffice = wxFindFirstFile(programs + wxT("\\*OpenOffice*"), wxDIR);
        wxArrayString langs = GetArrayFromString(wxT("en;fr;es;de"));
        for (size_t i = 0; i < langs.GetCount(); ++i)
        {
            if (!libreOffice.IsEmpty())
                dictPaths.Add(libreOffice + wxT("\\share\\extensions\\dict-") + langs[i]);
            if (!openOffice.IsEmpty())
                dictPaths.Add(openOffice + wxT("\\share\\extensions\\dict-") + langs[i]);
        }
    }
    else
    {
        dictPaths.Add(wxT("/usr/share/hunspell"));
        dictPaths.Add(wxT("/usr/share/myspell/dicts"));
    }
    dictPaths.Add(m_pPlugin->GetOnlineCheckerConfigPath());
    for (size_t i = 0; i < dictPaths.GetCount(); ++i)
    {
        if (wxDirExists(dictPaths[i]) && !wxFindFirstFile(dictPaths[i] + wxFILE_SEP_PATH + wxT("*.dic"), wxFILE).IsEmpty())
            return dictPaths[i];
    }
    return dictPaths[0];
}
const wxString SpellCheckerConfig::GetThesaurusPath()const
{
    wxArrayString thesPaths;
    thesPaths.Add(m_ThesPath);
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(thesPaths[0]);
    if (platform::windows)
    {
        wxString programs = wxT("C:\\Program Files");
        wxGetEnv(wxT("ProgramFiles"), &programs);
        wxString libreOffice = wxFindFirstFile(programs + wxT("\\*LibreOffice*"), wxDIR);
        wxString openOffice = wxFindFirstFile(programs + wxT("\\*OpenOffice*"), wxDIR);
        wxArrayString langs = GetArrayFromString(wxT("en;fr;es;de"));
        for (size_t i = 0; i < langs.GetCount(); ++i)
        {
            if (!libreOffice.IsEmpty())
                thesPaths.Add(libreOffice + wxT("\\share\\extensions\\dict-") + langs[i]);
            if (!openOffice.IsEmpty())
                thesPaths.Add(openOffice + wxT("\\share\\extensions\\dict-") + langs[i]);
        }
    }
    else
    {
        thesPaths.Add(wxT("/usr/share/myspell/dicts"));
        thesPaths.Add(wxT("/usr/share/mythes"));
    }
    thesPaths.Add(m_pPlugin->GetOnlineCheckerConfigPath());
    for (size_t i = 0; i < thesPaths.GetCount(); ++i)
    {
        if (wxDirExists(thesPaths[i]) && !wxFindFirstFile(thesPaths[i] + wxFILE_SEP_PATH + wxT("th*.dat"), wxFILE).IsEmpty())
            return thesPaths[i];
    }
    return thesPaths[0];
}
const wxString SpellCheckerConfig::GetBitmapPath()const
{
    wxString bitmPath = m_BitmPath;
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(bitmPath);
    if (wxDirExists(bitmPath) && !wxFindFirstFile(bitmPath + wxFILE_SEP_PATH + wxT("*.png"), wxFILE).IsEmpty())
        return bitmPath;
    return m_pPlugin->GetOnlineCheckerConfigPath();
}

const wxString SpellCheckerConfig::GetRawDictionaryPath()const{return m_DictPath;}
const wxString SpellCheckerConfig::GetRawThesaurusPath()const{return m_ThesPath;}
const wxString SpellCheckerConfig::GetRawBitmapPath()const{return m_BitmPath;}
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
        m_EnableSpellTooltips = cfg->ReadBool(CFG_SPELLCHECK_SPELL_TOOLTIPS_CHECK, true);
        m_EnableThesaurusTooltips = cfg->ReadBool(CFG_SPELLCHECK_THESAURUS_TOOLTIPS_CHECK, true);
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
        cfg->Write(CFG_SPELLCHECK_SPELL_TOOLTIPS_CHECK, m_EnableSpellTooltips);
        cfg->Write(CFG_SPELLCHECK_THESAURUS_TOOLTIPS_CHECK, m_EnableThesaurusTooltips);
        cfg->Write(CFG_SPELLCHECK_DICTIONARY_NAME, m_strDictionaryName);
        cfg->Write(CFG_SPELLCHECK_DICTIONARY_PATH, m_DictPath);
        cfg->Write(CFG_SPELLCHECK_THESAURI_PATH, m_ThesPath);
        cfg->Write(CFG_SPELLCHECK_BITMAPS_PATH, m_BitmPath);
    }
    m_pPlugin->ReloadSettings();
}

void SpellCheckerConfig::PopulateLanguageNamesMap()
{
    m_LanguageNamesMap[_T("af_ZA")] = _T("Afrikaans (South Africa)");
    m_LanguageNamesMap[_T("bg_BG")] = _T("Bulgarian (Bulgaria)");
    m_LanguageNamesMap[_T("ca_ES")] = _T("Catalan (Spain)");
    m_LanguageNamesMap[_T("cz_CZ")] = _T("Czech (Czech Republic)");
    m_LanguageNamesMap[_T("cy_GB")] = _T("Welsh (Wales)");
    m_LanguageNamesMap[_T("da_DK")] = _T("Danish (Denmark)");
    m_LanguageNamesMap[_T("de_AT")] = _T("German (Austria)");
    m_LanguageNamesMap[_T("de_CH")] = _T("German (Switzerland)");
    m_LanguageNamesMap[_T("de_DE")] = _T("German (Germany-orig dict)");
    m_LanguageNamesMap[_T("de_DE_comb")] =_T("German (Germany-old & neu ortho)");
    m_LanguageNamesMap[_T("de_DE_neu")] =_T("German (Germany-neu ortho)");
    m_LanguageNamesMap[_T("el_GR")] = _T("Greek (Greece)");
    m_LanguageNamesMap[_T("en")]    = _T("English");
    m_LanguageNamesMap[_T("en_AU")] = _T("English (Australia)");
    m_LanguageNamesMap[_T("en_CA")] = _T("English (Canada)");
    m_LanguageNamesMap[_T("en_GB")] = _T("English (United Kingdom)");
    m_LanguageNamesMap[_T("en_NZ")] = _T("English (New Zealand)");
    m_LanguageNamesMap[_T("en_US")] = _T("English (United States)");
    m_LanguageNamesMap[_T("en_ZA")] = _T("English (South Africa)");
    m_LanguageNamesMap[_T("eo_l3")] = _T("Esperanto (Anywhere)");
    m_LanguageNamesMap[_T("es_ES")] = _T("Spanish (Spain-etal)");
    m_LanguageNamesMap[_T("es_MX")] = _T("Spanish (Mexico)");
    m_LanguageNamesMap[_T("fo_FO")] = _T("Faroese (Faroese Islands)");
    m_LanguageNamesMap[_T("fr_FR")] = _T("French (France)");
    m_LanguageNamesMap[_T("ga_IE")] = _T("Irish (Ireland)");
    m_LanguageNamesMap[_T("gd_GB")] = _T("Scottish Gaelic (Scotland)");
    m_LanguageNamesMap[_T("gl_ES")] = _T("Galician (Spain)");
    m_LanguageNamesMap[_T("he_IL")] = _T("Hebrew (Israel)");
    m_LanguageNamesMap[_T("hr_HR")] = _T("Croatian (Croatia)");
    m_LanguageNamesMap[_T("hu_HU")] = _T("Hungarian (Hungaria)");
    m_LanguageNamesMap[_T("ia")]    =_T("Interligua (x-register)");
    m_LanguageNamesMap[_T("id_ID")] = _T("Indonesian (Indonesia)");
    m_LanguageNamesMap[_T("it_IT")] = _T("Italian (Italy)");
    m_LanguageNamesMap[_T("la")]    =_T("Latin (x-register)");
    m_LanguageNamesMap[_T("lt_LT")] = _T("Lithuanian (Lithuania)");
    m_LanguageNamesMap[_T("lv_LV")] = _T("Latvian (Latvia)");
    m_LanguageNamesMap[_T("mg_MG")] = _T("Malagasy (Madagascar)");
    m_LanguageNamesMap[_T("mi_NZ")] = _T("Maori (New Zealand)");
    m_LanguageNamesMap[_T("ms_MY")] = _T("Malay (Malaysia)");
    m_LanguageNamesMap[_T("nb_NO")] = _T("Norwegian Bokmaal (Norway)");
    m_LanguageNamesMap[_T("nl_NL")] = _T("Dutch (Nederlands)");
    m_LanguageNamesMap[_T("nn_NO")] = _T("Norwegian Nynorsk (Norway)");
    m_LanguageNamesMap[_T("ny_MW")] = _T("Chichewa (Malawi)");
    m_LanguageNamesMap[_T("pl_PL")] = _T("Polish (Poland)");
    m_LanguageNamesMap[_T("pt_BR")] = _T("Portuguese (Brazil)");
    m_LanguageNamesMap[_T("pt_PT")] = _T("Portuguese (Portugal)");
    m_LanguageNamesMap[_T("ro_RO")] = _T("Romanian (Romania)");
    m_LanguageNamesMap[_T("ru_RU")] = _T("Russian (Russia)");
    m_LanguageNamesMap[_T("ru_RU_ye")] =_T("Russian ye (Russia)");
    m_LanguageNamesMap[_T("ru_RU_yo")] =_T("Russian yo (Russia)");
    m_LanguageNamesMap[_T("rw_RW")] = _T("Kinyarwanda (Rwanda)");
    m_LanguageNamesMap[_T("sk_SK")] = _T("Slovak (Slovakia)");
    m_LanguageNamesMap[_T("sl_SI")] = _T("Slovenian (Slovenia)");
    m_LanguageNamesMap[_T("sv_SW")] = _T("Swedish (Sweden)");
    m_LanguageNamesMap[_T("sw_KE")] = _T("Swahili (Kenya)");
    m_LanguageNamesMap[_T("tet_ID")] = _T("Tetum (Indonesia)");
    m_LanguageNamesMap[_T("tl_PH")] = _T("Tagalog (Philippines)");
    m_LanguageNamesMap[_T("tn_ZA")] = _T("Tswana (South Africa)");
    m_LanguageNamesMap[_T("uk_UA")] = _T("Ukrainian (Ukraine)");
    m_LanguageNamesMap[_T("zu_ZA")] = _T("Zulu (South Africa)");
}
wxString SpellCheckerConfig::GetLanguageName(const wxString& language_id)
{
    std::map<wxString, wxString>::iterator it;

    if(language_id.empty())
        return language_id;

    it = m_LanguageNamesMap.find(language_id);
    if (it != m_LanguageNamesMap.end() )
        return it->second;

    wxString id_fix = language_id;
    id_fix.Replace(wxT("-"), wxT("_")); // some dictionaries are distributed with hyphens

    it = m_LanguageNamesMap.find(id_fix);
    if (it != m_LanguageNamesMap.end() )
        return it->second;

    id_fix = id_fix.BeforeLast(wxT('_')); // may be "*_v2", or root language may be known even if this specification is not

    it = m_LanguageNamesMap.find(id_fix);
    if (it != m_LanguageNamesMap.end() )
        return it->second + wxT(" (") + language_id + wxT(")"); // but may be incorrect, so specify the original name

    return language_id;
}
