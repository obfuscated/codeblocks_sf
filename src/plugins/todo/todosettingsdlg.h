#ifndef TODOSETTINGSDLG_H
#define TODOSETTINGSDLG_H

#include <wx/dialog.h>

class ToDoSettingsDlg : public wxDialog
{
	public:
		ToDoSettingsDlg();
		virtual ~ToDoSettingsDlg();
		
		virtual void EndModal(int retCode);
	protected:
	private:
};

#endif // TODOSETTINGSDLG_H

