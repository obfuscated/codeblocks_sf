#ifndef EDITWATCHDLG_H
#define EDITWATCHDLG_H

#include <wx/dialog.h>
#include "debuggertree.h"

class EditWatchDlg : public wxDialog
{
    public:
        EditWatchDlg(Watch* w = 0, wxWindow* parent = 0);
        virtual ~EditWatchDlg();

        virtual const Watch& GetWatch(){ return m_Watch; }
    protected:
        void EndModal(int retCode);

        Watch m_Watch;
    private:
};

#endif // EDITWATCHDLG_H
