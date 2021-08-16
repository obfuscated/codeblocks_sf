/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef AUTODETECTCOMPILERS_H
#define AUTODETECTCOMPILERS_H

#include "scrollingdialog.h"
#include <vector>

class wxCommandEvent;
class wxUpdateUIEvent;

class AutoDetectCompilers : public wxScrollingDialog
{
	public:
		AutoDetectCompilers(wxWindow* parent);
		~AutoDetectCompilers() override;

        bool closeCheckOkay();
        void OnClose(wxCloseEvent& event);
        void OnCloseClicked(wxCommandEvent& event);
		void OnDefaultCompilerClick(wxCommandEvent& event);
		void OnUdateCompilerListUI(wxCommandEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
        DECLARE_EVENT_TABLE()
		void UpdateCompilerDisplayList();
    private:
        enum CompilerHighlightColor
        {
            colorHighlightNone = 0,
            colorHighlightGrey,
            colorHighlightGreen,
            colorHighlightRed,
            colorHighlightYellow
        };

        enum CompilerColumnsNumber
        {
            ccnNameColumn = 0,
            ccnStatusColumn,
            ccnDetectedPathColumn,
        };

        struct CompilerItem
        {
            wxString compilerName;
            wxString status;
            wxString compilerPath;
            CompilerHighlightColor colorHighlight;
            bool detected;
        };

        std::vector<CompilerItem> m_CompilerList;
};


#endif // AUTODETECTCOMPILERS_H
