21#ifndef WXSWNDDIALOG_H
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
        wxCheckBox CheckBox1;
        wxButton Button2;
        wxCheckBox CheckBox3;
        wxCheckBox CheckBox4;
        wxGridSizer* GridSizer1
        wxStaticText StaticText1;
        wxComboBox ComboBox1;
        //*)

    private:
    
		DECLARE_EVENT_TABLE()
};

#endif // WXSWNDDIALOG_H
