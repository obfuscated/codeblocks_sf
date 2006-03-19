/***************************************************************
 * Name:      codestatconfig.h
 * Purpose:   Code::Blocks plugin
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef CODESTATCONFIG_H
#define CODESTATCONFIG_H

#include <cbplugin.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/tokenzr.h>
#include <wx/textdlg.h>
#include "language_def.h"

int LoadSettings(LanguageDef languages[NB_FILETYPES_MAX]);
int LoadDefaultSettings(LanguageDef languages[NB_FILETYPES_MAX]);

class CodeStatConfigDlg : public cbConfigurationPanel
{
	public:
		CodeStatConfigDlg(wxWindow* parent);
		virtual ~CodeStatConfigDlg();

	protected:
      void ComboBoxEvent(wxCommandEvent & event);
      void PrintLanguageInfo(int id);
      void SaveSettings();
      void Add(wxCommandEvent& event);
      void Remove(wxCommandEvent& event);
      void RestoreDefault(wxCommandEvent& event);
      void SaveCurrentLanguage();
      void ReInitDialog();

      virtual wxString GetTitle(){ return _("Code statistics settings"); }
      virtual wxString GetBitmapBaseName(){ return _T("codestats"); }
      virtual void OnApply(){SaveSettings();};
      virtual void OnCancel(){}

	private:
	   LanguageDef languages[NB_FILETYPES_MAX];
	   int nb_languages;
	   int selected_language;
      DECLARE_EVENT_TABLE()
};

#endif // CODESTATCONFIG_H
