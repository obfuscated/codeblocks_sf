/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
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
