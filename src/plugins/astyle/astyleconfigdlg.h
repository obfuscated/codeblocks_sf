#ifndef ASTYLECONFIGDLG_H
#define ASTYLECONFIGDLG_H

#include <wx/dialog.h>

enum AStylePredefinedStyle
{
  aspsAnsi = 0,
  aspsKr,
  aspsLinux,
  aspsGnu,
  aspsJava,
  aspsCustom
};

class AstyleConfigDlg : public wxDialog
{
	public:
		AstyleConfigDlg(wxWindow* parent);
		virtual ~AstyleConfigDlg();

	protected:
    void OnStyleChange(wxCommandEvent& event);
    void OnPreview(wxCommandEvent& event);

    void LoadSettings();
    void SaveSettings();
    void SetStyle(AStylePredefinedStyle style);
    void EndModal(int retCode);

	private:
    DECLARE_EVENT_TABLE()
};

#endif // ASTYLECONFIGDLG_H
