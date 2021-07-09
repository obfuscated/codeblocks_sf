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
#if 1
    private:
        enum PCHMode
        {
            eCompilerNameColumn = 0,
            eStatusColumn,
            eCompilerPathColumn,
        };

        struct CompilerItem
        {
            wxString wxsCompilerName;
            wxString wxsStatus;
            wxString wxsCompilerPath;
            int iHighlight;
            bool bDetected;
        };
        std::vector<CompilerItem> CompilerList;

		void updateCompilerDisplayList();
#endif
	public:
		AutoDetectCompilers(wxWindow* parent);
		~AutoDetectCompilers() override;

		void OnDefaultClick(wxCommandEvent& event);
		void OnCompilerDisplayOptionClickRadioButton(wxCommandEvent& event);
		void OnCompilerPathDisplayOptionClickCheckBox(wxCommandEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
        DECLARE_EVENT_TABLE()
};

#endif // AUTODETECTCOMPILERS_H
