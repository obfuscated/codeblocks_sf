/*
* This file is part of OccurrenceHighlighter plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* OccurrenceHighlighter plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* OccurrenceHighlighter plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OccurrenceHighlighter. If not, see <http://www.gnu.org/licenses/>.
*
*/
#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <set>

#include <editor_hooks.h>

class cbStyledTextCtrl;
namespace
{
    class HighlightedEditorPositions
    {
        public:
            int linea, lineb;
            cbEditor *ed;
            HighlightedEditorPositions();
    };
}

class Highlighter
{
    public:
        Highlighter(std::set<wxString> &texts);
        virtual ~Highlighter();
        void Call(cbEditor*, wxScintillaEvent&) const;

        const int     GetIndicator()      const;
        const wxColor GetIndicatorColor() const;
        void          TextsChanged()      const;

    private:
        void OnEditorChange(cbEditor* ctrl)   const;
        void OnEditorUpdateUI(cbEditor *ctrl) const;

        void ClearAllIndications(cbStyledTextCtrl* stc)const;
        void ClearAllIndications()const;
        void DoSetIndications(cbEditor* ctrl)const;
        void OnEditorChangeTextRange(cbEditor* ctrl, int start, int end)const;
        void HighlightOccurrencesOfSelection(cbEditor* ctrl)const;

        std::set<wxString>&           m_Texts;
        mutable bool                  m_AlreadyChecked;
        mutable cbEditor*             m_OldCtrl;
        mutable wxArrayInt            m_InvalidatedRangesStart;
        mutable wxArrayInt            m_InvalidatedRangesEnd;
        mutable std::pair<long, long> m_HighlightSelectedOccurencesLastPos;
        mutable cbStyledTextCtrl*     m_OldHighlightSelectionCtrl;
};

#endif // HIGHLIGHTER_H
