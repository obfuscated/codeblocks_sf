/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef AUTODETECTCOMPILERS_H
#define AUTODETECTCOMPILERS_H

#include "scrollingdialog.h"

class wxCommandEvent;
class wxUpdateUIEvent;

class AutoDetectCompilers : public wxScrollingDialog
{
    private:
        enum CompilerHighlightColor
        {
            eHighlightNone = 0,
            eHighlightGrey,
            eHighlightGreen,
            eHighlightRed,
            eHighlightYellow
        };

        enum CompilerColumnsNumber
        {
            ccnNameColumn = 0,
            ccnStatusColumn,
            ccnDetectedPathColumn,
        };

        struct CompilerItem
        {
            wxString wxsCompilerName;
            wxString wxsStatus;
            wxString wxsCompilerPath;
            CompilerHighlightColor eHighlight;
            bool bDetected;
        };
        std::vector<CompilerItem> vCompilerList;

		void UpdateCompilerDisplayList();

	public:
		AutoDetectCompilers(wxWindow* parent);
		~AutoDetectCompilers() override;

		void OnDefaultCompilerClick(wxCommandEvent& event);
		void OnUdateCompilerListUI(wxCommandEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
        DECLARE_EVENT_TABLE()
};

#endif // AUTODETECTCOMPILERS_H
