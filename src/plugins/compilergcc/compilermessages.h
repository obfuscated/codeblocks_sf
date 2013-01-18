/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILERMESSAGES_H
#define COMPILERMESSAGES_H

#include "loggers.h"

class CompilerErrors;
class wxArrayString;
class wxCommandEvent;

class CompilerMessages : public wxEvtHandler, public ListCtrlLogger
{
    public:
        CompilerMessages(const wxArrayString& titles, const wxArrayInt& widths);
        virtual ~CompilerMessages();
        virtual void SetCompilerErrors(CompilerErrors* errors){ m_pErrors = errors; }
        virtual void FocusError(int nr);

        void AutoFitColumns();

        virtual wxWindow* CreateControl(wxWindow* parent);
        void DestroyControls();

        virtual bool HasFeature(Feature::Enum feature) const;
        virtual void AppendAdditionalMenuItems(wxMenu &menu);
    private:
        void OnClick(wxCommandEvent& event);
        void OnDoubleClick(wxCommandEvent& event);
        void OnFit(wxCommandEvent& event);
        void OnAutoFit(wxCommandEvent& event);

        void FitColumns();

        CompilerErrors* m_pErrors;
        bool m_autoFit;

        DECLARE_EVENT_TABLE()
};

#endif // COMPILERMESSAGES_H

