#ifndef PRINTDLG_H
#define PRINTDLG_H

#include <wx/dialog.h>
#include <printing_types.h>

class PrintDialog : public wxDialog
{
	public:
		PrintDialog(wxWindow* parent);
		virtual ~PrintDialog();

        virtual void EndModal(int retCode);

		PrintScope GetPrintScope();
		PrintColorMode GetPrintColorMode();
	protected:
	private:
};

#endif // PRINTDLG_H
