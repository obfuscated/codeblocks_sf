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
#ifndef OnlineSpellChecker_h
#define OnlineSpellChecker_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#include "cbeditor.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class wxSpellCheckEngineInterface;
class SpellCheckHelper;
class cbStyledTextCtrl;


class OnlineSpellChecker
{
    public:
        OnlineSpellChecker(wxSpellCheckEngineInterface* pSpellChecker, SpellCheckHelper* pSpellHelp);
        virtual ~OnlineSpellChecker();
        virtual void Call(cbEditor* ctrl, wxScintillaEvent& event) const;

        int GetIndicator() const;
        const wxColor GetIndicatorColor() const;
        void EnableOnlineChecks(bool check = true);

    private:
        friend class SpellCheckerPlugin;

        void OnEditorChange(cbEditor* ctrl) const;
        void OnEditorChangeTextRange(cbEditor* ctrl, int start, int end) const;
        void OnEditorUpdateUI(cbEditor* ctrl) const;

        void ClearAllIndications(cbStyledTextCtrl* stc) const;
        void ClearAllIndications() const;
        void DoSetIndications(cbEditor* ctrl) const;
        void DissectWordAndCheck(cbStyledTextCtrl* stc, int wordstart, int wordend) const;

    private:
        mutable bool                 m_alreadyChecked;
        mutable cbEditor*            m_oldCtrl;
        mutable wxArrayInt           m_invalidatedRangesStart;
        mutable wxArrayInt           m_invalidatedRangesEnd;
        wxSpellCheckEngineInterface* m_pSpellChecker;
        SpellCheckHelper*            m_pSpellHelper;
        bool                         m_doChecks;
};

#endif // header guard
