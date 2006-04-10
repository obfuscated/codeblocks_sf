#ifndef TODOSETTINGSDLG_H
#define TODOSETTINGSDLG_H

#include <wx/dialog.h>

class ToDoSettingsDlg : public wxDialog
{
	public:
		ToDoSettingsDlg();
		~ToDoSettingsDlg();

		void EndModal(int retCode);
};

#endif // TODOSETTINGSDLG_H

