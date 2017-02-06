/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef GOTOFUNCTIONDLG_H
#define GOTOFUNCTIONDLG_H

#ifndef CB_PRECOMP
    //(*HeadersPCH(GotoFunctionDlg)
    #include <wx/dialog.h>
    class wxBoxSizer;
    class wxCheckBox;
    class wxListCtrl;
    class wxStaticText;
    class wxTextCtrl;
    //*)
#endif
//(*Headers(GotoFunctionDlg)
//*)

#include "incremental_select_helper.h"

class GotoFunctionListCtrl;

/** The goto function dialog allow user to type a function name, and filter out the functions
 *
 * For example, you have three functions in a cpp file
 * @code
 *  int fa1();
 *  int fa2();
 *  int fb1();
 * @endcode
 * When user type "fa" in the text ctrl, only "fa1" and "fa2" will shown in the list.
 * The mode option defines how the list is shown. If user want to show the function
 * return type, function name and function parameters in different columns, then
 * the check box should be selected
 */
class GotoFunctionDlg : public wxDialog
{
    public:
        struct FunctionToken
        {
            wxString displayName, name;
            wxString paramsAndreturnType, funcName;
            unsigned line, implLine;
        };

        struct Iterator : IncrementalSelectIteratorIndexed
        {
            Iterator();

            void AddToken(const FunctionToken &token);
            const FunctionToken* GetToken(int index) const;

            int GetTotalCount() const override;
            const wxString& GetItemFilterString(int index) const override;
            wxString GetDisplayText(int index, int column) const override;

            void SetColumnMode(bool flag);
            void CalcLengths(wxListCtrl &list);

            int GetColumnWidth(int column) const override;

        private:
            std::vector<FunctionToken> m_tokens;
            int m_columnLength[3]; /// 0 is for non-column mode, 1 and 2 are for column mode.
            bool m_columnMode;
        };
    public:

        GotoFunctionDlg(wxWindow* parent, Iterator* iterator);
        virtual ~GotoFunctionDlg();

        int GetSelection();

    private:

        //(*Declarations(GotoFunctionDlg)
        GotoFunctionListCtrl* m_list;
        wxCheckBox* m_mode;
        wxTextCtrl* m_text;
        //*)

        //(*Identifiers(GotoFunctionDlg)
        static const long ID_CHECKBOX1;
        static const long ID_TEXTCTRL1;
        static const long ID_LISTCTRL1;
        //*)

        //(*Handlers(GotoFunctionDlg)
        void OnModeClick(wxCommandEvent& event);
        //*)

    private:
        void BuildContent(wxWindow* parent, Iterator* iterator);
        void SwitchMode();
    private:
        IncrementalSelectHandler m_handler;
        Iterator *m_iterator;
    private:
        DECLARE_EVENT_TABLE()
};

#endif
