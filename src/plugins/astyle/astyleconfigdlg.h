#ifndef ASTYLECONFIGDLG_H
#define ASTYLECONFIGDLG_H

#include <cbplugin.h>

enum AStylePredefinedStyle
{
  aspsAnsi = 0,
  aspsKr,
  aspsLinux,
  aspsGnu,
  aspsJava,
  aspsCustom
};

class AstyleConfigDlg : public cbConfigurationPanel
{
	public:
		AstyleConfigDlg(wxWindow* parent);
		virtual ~AstyleConfigDlg();

	protected:
        void OnStyleChange(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);

        virtual wxString GetTitle(){ return _("Source formatter"); }
        virtual wxString GetBitmapBaseName(){ return _T("astyle-plugin"); }
        virtual void OnApply(){ SaveSettings(); }
        virtual void OnCancel(){}

        void LoadSettings();
        void SaveSettings();
        void SetStyle(AStylePredefinedStyle style);

	private:
        DECLARE_EVENT_TABLE()
};

#endif // ASTYLECONFIGDLG_H
