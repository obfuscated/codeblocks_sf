#ifndef WXSWNDDIALOG_H
#define WXSWNDDIALOG_H

#include "defwidgets/wxsdialog.h"

/** This class represents one dialog */
class wxsWndDialog
{
	public:
		wxsWndDialog();
		virtual ~wxsWndDialog();
		
		inline wxsDialog* GetDialog() { return Dialog; }
		
    private:
        wxsDialog* Dialog;
};

#endif // WXSWNDDIALOG_H
