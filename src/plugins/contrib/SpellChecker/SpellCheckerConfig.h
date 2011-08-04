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
#ifndef SPELLCHECKERCONFIG_H
#define SPELLCHECKERCONFIG_H

#include <vector>
#include <wx/string.h>

class SpellCheckerPlugin;

class SpellCheckerConfig
{
    public:
        SpellCheckerConfig(SpellCheckerPlugin *plugin);
        virtual ~SpellCheckerConfig();

        bool GetEnableOnlineChecker();
        void SetEnableOnlineChecker(bool val);
        wxString GetDictionaryName();
        void SetDictionaryName(wxString val);
        void Load();
        void Save();

        void ScanForDictionaries();
        void ScanForDictionaries(const wxString &path);

        const std::vector<wxString> &GetPossibleDictionaries()const;
        int GetSelectedDictionaryNumber()const;

        const wxString GetDictionaryPath()const;
        const wxString GetThesaurusPath()const;
        const wxString GetBitmapPath()const;
        void SetDictionaryPath(const wxString &path);
        void SetThesaurusPath(const wxString &path);
        void SetBitmapPath(const wxString &path);
    protected:
    private:
        wxString m_DictPath;
        wxString m_ThesPath;
        wxString m_BitmPath;
        bool m_EnableOnlineChecker;
        wxString m_strDictionaryName;
        std::vector<wxString> m_dictionaries;
        int selectedDictionary;

        SpellCheckerPlugin *m_pPlugin;
};

#endif // SPELLCHECKERCONFIG_H
