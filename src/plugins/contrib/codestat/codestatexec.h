/***************************************************************
 * Name:      codestatexec.h
 * Purpose:   Code::Blocks CodeStat plugin: main window
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef CODESTATEXEC_H
#define CODESTATEXEC_H

#include "scrollingdialog.h"
#include <wx/filename.h>
#include "language_def.h"

class wxWindow;

/** This class computes the statistics of the project's files and display them.
 *  @see CodeStat, CodeStatConfigDlg, CodeStatExecDlg, LanguageDef
 */
class CodeStatExecDlg : public wxScrollingDialog
{
	public:
		CodeStatExecDlg(wxWindow* parent) : parent(parent){}
		virtual ~CodeStatExecDlg();
		int Execute(LanguageDef languages[NB_FILETYPES_MAX], int nb_languages);
	private:
      void EndModal(int retCode);
      void CountLines(wxFileName filename, LanguageDef &language,
                      long int &code_lines, long int &codecomments_lines,
                      long int &comment_lines, long int &empty_lines, long int &total_lines);
      void AnalyseLine(LanguageDef &language, wxString line, bool &comment, bool &code, bool &multi_line_comment);
      wxWindow* parent;
};

#endif // CODESTATEXEC_H
