#ifndef WXSWNDDIALOG_H
#define WXSWNDDIALOG_H

#include "defwidgets/wxsdialog.h"

/** This class represents one dialog */
class wxsWndDialog
{
	public:
		wxsWndDialog();
		virtual ~wxsWndDialog();
		
	protected:
	
		//(*Declarations(wxsWndDialog)
        wxGridSizer* MainSizer
        wxCheckBox* CheckBox1;
        wxButton* Button1;
        wxCheckBox* CheckBox2;
        wxGridSizer* GridSizer1
        wxStaticText* StaticText1;
        //*)

    private:
    
		DECLARE_EVENT_TABLE()
};

#endif // WXSWNDDIALOG_H
