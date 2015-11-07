/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBSTYLEDTEXTCTRL_H_INCLUDED
#define CBSTYLEDTEXTCTRL_H_INCLUDED

#include "wx/wxscintilla.h"
#include <wx/longlong.h>
#include "settings.h" // DLLIMPORT

#include <map>
#include <set>

class wxContextMenuEvent;
class wxFocusEvent;
class wxMouseEvent;
class wxPoint;

class DLLIMPORT cbStyledTextCtrl : public wxScintilla
{
    public:
        cbStyledTextCtrl(wxWindow* pParent, int id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
        virtual ~cbStyledTextCtrl();

        /** Don't use this. It throws an exception if you do. */
        void operator=(const cbStyledTextCtrl& /*rhs*/);

        wxDateTime GetLastFocusTime() const {return m_lastFocusTime;}
        wxString GetLastSelectedText() const {return m_lastSelectedText;}

        void EnableTabSmartJump(bool enable = true);
        bool IsCharacter(int style);
        bool IsString(int style);
        bool IsPreprocessor(int style);
        bool IsComment(int style);
        void CallTipCancel();
        bool IsBraceShortcutActive();

        void DoBraceCompletion(const wxChar& ch);
        bool DoSelectionBraceCompletion(const wxChar& ch);

        static std::map<int, std::set<int> > &GetCharacterLexerStyles();
        static std::map<int, std::set<int> > &GetStringLexerStyles();
        static std::map<int, std::set<int> > &GetPreprocessorLexerStyles();
        static std::map<int, std::set<int> > &GetCommentLexerStyles();

        void MakeNearbyLinesVisible(int line);
    private:
        void OnContextMenu(wxContextMenuEvent& event);
        void OnKillFocus(wxFocusEvent& event);
        void OnSetFocus(wxFocusEvent& event);
        void OnMouseMiddleDown(wxMouseEvent& event);
        void OnMouseMiddleClick(wxMouseEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnKeyUp(wxKeyEvent& event);
        void OnMouseLeftUp(wxMouseEvent& event);

        bool AllowTabSmartJump();
        void HighlightRightBrace();
        void EmulateDwellStart();

        wxWindow* m_pParent;
        wxLongLong m_lastFocusTime;
        int m_bracePosition;
        int m_lastPosition;
        int m_middleClickPos;
        bool m_tabSmartJump;
        wxString m_lastSelectedText;
        bool m_braceShortcutState;

        static std::map<int, std::set<int> > CharacterLexerStyles;
        static std::map<int, std::set<int> > StringLexerStyles;
        static std::map<int, std::set<int> > PreprocessorLexerStyles;
        static std::map<int, std::set<int> > CommentLexerStyles;

        DECLARE_EVENT_TABLE()
};

#endif // CBSTYLEDTEXTCTRL_H_INCLUDED
