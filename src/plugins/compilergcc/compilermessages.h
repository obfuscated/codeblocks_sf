/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef COMPILERMESSAGES_H
#define COMPILERMESSAGES_H

#include "loggers.h"

class CompilerErrors;
class wxArrayString;
class wxCommandEvent;

class CompilerMessages : public ListCtrlLogger, public wxEvtHandler
{
    public:
        CompilerMessages(const wxArrayString& titles, const wxArrayInt& widths);
        virtual ~CompilerMessages();
        virtual void SetCompilerErrors(CompilerErrors* errors){ m_pErrors = errors; }
        virtual void FocusError(int nr);

        virtual wxWindow* CreateControl(wxWindow* parent);
    private:
        void OnClick(wxCommandEvent& event);
        void OnDoubleClick(wxCommandEvent& event);

        CompilerErrors* m_pErrors;

        DECLARE_EVENT_TABLE()
};

#endif // COMPILERMESSAGES_H

