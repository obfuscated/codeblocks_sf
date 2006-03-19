/***************************************************************
 * Name:      codestatexec.h
 * Purpose:   Code::Blocks Profiler plugin: main window
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef CODESTATEXEC_H
#define CODESTATEXEC_H

#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/gauge.h>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/fs_zip.h>
#include <wx/textfile.h>
#include <wx/stattext.h>
#include <cbproject.h>
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include <messagemanager.h>
#include "language_def.h"

class CodeStatExecDlg : public wxDialog
{
	public:
		CodeStatExecDlg(wxWindow* parent)
            : parent(parent){}
		virtual ~CodeStatExecDlg();
		int Execute(LanguageDef languages[NB_FILETYPES_MAX], int nb_languages);
	protected:
      void EndModal(int retCode);
   private:
      void CountLines(wxFileName filename, LanguageDef &language,
                      long int &code_lines, long int &codecomments_lines,
                      long int &comment_lines, long int &empty_lines, long int &total_lines);
      void AnalyseLine(LanguageDef &language, wxString line, bool &comment, bool &code, bool &multi_line_comment);
      wxWindow* parent;
};

#endif // CODESTATEXEC_H
