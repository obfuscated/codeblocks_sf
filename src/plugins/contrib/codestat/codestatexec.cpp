/***************************************************************
 * Name:      codestatexec.cpp
 * Purpose:   Code::Blocks CodeStat plugin: main window
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/
#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "configmanager.h"
#include "editormanager.h"
#include "globals.h"
#include "manager.h"
#include "messagemanager.h"
#include "projectfile.h"
#include "projectmanager.h"
#endif
#include <wx/gauge.h>
#include <wx/progdlg.h>
#include <wx/textfile.h>
#include "codestatexec.h"

/** Count the lines on all project's files and display the results.
 *  @param languages Languages definitions
 *  @param nb_languages Number of languages defined in the 'languages' array
 */
int CodeStatExecDlg::Execute(LanguageDef languages[NB_FILETYPES_MAX], int nb_languages)
{
   cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
   long nb_files = project->GetFilesCount();
   //wxMessageBox(wxString::Format(_T("Nb files: %ld"), nb_files), _("Error"), wxOK);

   // Check if all files have been saved
   bool all_files_saved = true;
   for (int i=0; i<nb_files; ++i)
      if (project->GetFile(i)->GetFileState() == fvsModified)
         all_files_saved = false;
   // If not, ask user if we can save them
   if (!all_files_saved)
   {
       if (cbMessageBox(_T("Some files are not saved.\nDo you want to save them before running the plugin?"), _("Warning"), wxICON_EXCLAMATION | wxYES_NO, Manager::Get()->GetAppWindow()) == wxID_YES)
       {
           for (int i=0; i<nb_files; ++i)
           {
              if (project->GetFile(i)->GetFileState() == fvsModified)
                 Manager::Get()->GetEditorManager()->Save(project->GetFile(i)->file.GetFullPath());
           }
       }
   }

	// Count code statistics on each file
	long nb_files_not_found = 0;
	long nb_skipped_files = 0;
	long total_lines = 0;
	long code_lines = 0;
	long empty_lines = 0;
	long comment_lines = 0;
	long codecomments_lines = 0;

	wxProgressDialog progress(_("Code Statistics plugin"),_("Parsing project files. Please wait..."));
	for (int i=0; i<nb_files; ++i)
	{
		ProjectFile* pf = project->GetFile(i);
		wxFileName filename(pf->file.GetFullPath(), wxPATH_DOS);
		if (!filename.FileExists())
		{
			++nb_files_not_found;
			//Manager::Get()->GetMessageManager()->DebugLog(_T("Code Statistics: Ignoring file '%s' (file not found)"), filename.GetName());
		}
		else
		{
			// Find the language associated to the file extension
			int num_language = -1;
			for (int l = 0; l<nb_languages; ++l)
			{
				for (int j = 0; j<(int)languages[l].ext.Count(); ++j)
				{
				  if (filename.GetExt() == languages[l].ext[j])
					 num_language = l;
				}
			}

			// If the language is found, analyse the source file
			if (num_language > -1)
			  CountLines(filename, languages[num_language], code_lines, codecomments_lines, comment_lines, empty_lines, total_lines);
			else ++nb_skipped_files;
		}
		if (nb_files > 1)
			progress.Update((100*i)/(nb_files-1));
   }
   progress.Update(100);

   // Setting-up the statistics dialog box
   wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgCodeStatExec"));

   wxStaticText* txt_num_files = XRCCTRL(*this, "txt_num_files", wxStaticText);
   txt_num_files->SetLabel(wxString::Format(_("%ld"), nb_files));
   wxStaticText* txt_skipped_files = XRCCTRL(*this, "txt_skipped_files", wxStaticText);
   txt_skipped_files->SetLabel(wxString::Format(_("%ld"), nb_skipped_files));
   wxStaticText* txt_files_not_found = XRCCTRL(*this, "txt_files_not_found", wxStaticText);
   txt_files_not_found->SetLabel(wxString::Format(_("%ld"), nb_files_not_found));

   wxStaticText* txt_Code = XRCCTRL(*this, "txt_Code", wxStaticText);
   txt_Code->SetLabel(wxString::Format(_("%ld"), code_lines));
   wxStaticText* txt_Empty = XRCCTRL(*this, "txt_Empty", wxStaticText);
   txt_Empty->SetLabel(wxString::Format(_("%ld"), empty_lines));
   wxStaticText* txt_Comments = XRCCTRL(*this, "txt_Comments", wxStaticText);
   txt_Comments->SetLabel(wxString::Format(_("%ld"), comment_lines));
   wxStaticText* txt_Code_Comments = XRCCTRL(*this, "txt_Code_Comments", wxStaticText);
   txt_Code_Comments->SetLabel(wxString::Format(_("%ld"), codecomments_lines));
   wxStaticText* txt_Total = XRCCTRL(*this, "txt_Total", wxStaticText);
   txt_Total->SetLabel(wxString::Format(_("%ld"), total_lines));

   // If the project is not empty, display the main dialog box
   if(total_lines) // avoid division by zero on empty document
   {
		int icode = static_cast<int>(round(static_cast<double>(100 * code_lines) / static_cast<double>(total_lines)));
		wxGauge* Gauge_Code = XRCCTRL(*this, "Gauge_Code", wxGauge);
		Gauge_Code->SetValue(icode);
		wxStaticText* txt_Gauge_Code = XRCCTRL(*this, "txt_Gauge_Code", wxStaticText);
		txt_Gauge_Code->SetLabel(wxString::Format(_("%3d%% Code only"), icode));

		int icode_comments = static_cast<int>(round(static_cast<double>(100 * codecomments_lines) / static_cast<double>(total_lines)));
		wxGauge* Gauge_Code_Comments = XRCCTRL(*this, "Gauge_Code_Comments", wxGauge);
		Gauge_Code_Comments->SetValue(icode_comments);
		wxStaticText* txt_Gauge_Code_Comments = XRCCTRL(*this, "txt_Gauge_Code_Comments", wxStaticText);
		txt_Gauge_Code_Comments->SetLabel(wxString::Format(_("%3d%% Code + Comment"), icode_comments));

		int icomments = static_cast<int>(round(static_cast<double>(100 * comment_lines) / static_cast<double>(total_lines)));
		wxGauge* Gauge_Comments = XRCCTRL(*this, "Gauge_Comments", wxGauge);
		Gauge_Comments->SetValue(icomments);
		wxStaticText* txt_Gauge_Comments = XRCCTRL(*this, "txt_Gauge_Comments", wxStaticText);
		txt_Gauge_Comments->SetLabel(wxString::Format(_("%3d%% Comments"), icomments));

		int iempty = static_cast<int>(round(static_cast<double>(100 * empty_lines) / static_cast<double>(total_lines)));
		wxGauge* Gauge_Empty = XRCCTRL(*this, "Gauge_Empty", wxGauge);
		Gauge_Empty->SetValue(iempty);
		wxStaticText* txt_Gauge_Empty = XRCCTRL(*this, "txt_Gauge_Empty", wxStaticText);
		txt_Gauge_Empty->SetLabel(wxString::Format(_("%3d%% Empty"), iempty));

        ShowModal();
   }
   else cbMessageBox(_("The project is empty!"), _("Warning"), wxICON_EXCLAMATION | wxOK, Manager::Get()->GetAppWindow());

   return 0;
}

void CodeStatExecDlg::EndModal(int retCode)
{
    wxDialog::EndModal(retCode);
}

/** This function analyses a given source file and count the lines of code, comments etc...
 */
void CodeStatExecDlg::CountLines(wxFileName filename, LanguageDef &language,
                                 long int &code_lines, long int &codecomments_lines,
                                 long int &comment_lines, long int &empty_lines, long int &total_lines)
{
	wxTextFile file;
	if (file.Open(filename.GetFullPath(),wxConvFile))
	{
		bool multi_line_comment = false;
		total_lines += file.GetLineCount();
		for (unsigned int i=0; i<file.GetLineCount(); ++i)
		{
		   wxString line(file[i]);
		   line = line.Trim(true);
           line = line.Trim(false);
		   bool comment = false;
		   bool code = false;
		   if (line.IsEmpty())
	         ++empty_lines;
		   else
		   {
		   	  AnalyseLine(language, line, comment, code, multi_line_comment);
		      if (comment&&code) ++codecomments_lines;
		      else if (comment) ++comment_lines;
		      else if (code) ++code_lines;
		   }
		}
	}
}

/** This function determines the caracteristics of a given line (code line, comment line etc...).
 *  It is called by the "CountLines" function.
 *  @see CountLines
 */
void CodeStatExecDlg::AnalyseLine(LanguageDef &language, wxString line, bool &comment, bool &code, bool &multi_line_comment)
{
   int first_single_line_comment, first_multi_line_comment_begin, first_multi_line_comment_end;

   // Delete first and trailing spaces
   line = line.Trim(true);
   line = line.Trim(false);

	if (line.IsEmpty())
	   return;

	// Searching for single and multi-lines comment signs
	if (language.single_line_comment.Length() > 0)
      first_single_line_comment = line.Find(language.single_line_comment);
   else first_single_line_comment = -1;
   if (language.multiple_line_comment[0].Length() > 0)
      first_multi_line_comment_begin = line.Find(language.multiple_line_comment[0]);
   else first_multi_line_comment_begin = -1;
   if (language.multiple_line_comment[1].Length() > 0)
      first_multi_line_comment_end = line.Find(language.multiple_line_comment[1]);
   else first_multi_line_comment_end = -1;

   // We are in a multiple line comment => finding the "end of multiple line comment" sign
   if (multi_line_comment)
   {
      comment = true;
   	if (first_multi_line_comment_end > -1)
   	{
   		multi_line_comment = false;
   		if (first_multi_line_comment_end+language.multiple_line_comment[1].Length() < line.Length())
   		   AnalyseLine(language, line.Mid(first_multi_line_comment_end+language.multiple_line_comment[1].Length()), comment, code, multi_line_comment);
   	}
   }
   // We are not in a multiple line comment
   else if (!multi_line_comment)
   {
   	// First comment sign found is a single line comment sign
      if ( (first_single_line_comment>-1)
         &&((first_multi_line_comment_begin==-1)||((first_multi_line_comment_begin>-1)&&(first_single_line_comment<first_multi_line_comment_begin))) )
      {
      	comment = true;
         if (first_single_line_comment > 0)
            code = true;
      }
      // First comment sign found is a multi-line comment begin sign
      else if (first_multi_line_comment_begin>-1)
      {
         multi_line_comment = true;
         comment = true;
         if (first_multi_line_comment_begin > 0)
            code = true;
         if (first_multi_line_comment_begin+language.multiple_line_comment[0].Length() < line.Length())
   		   AnalyseLine(language, line.Mid(first_multi_line_comment_begin+language.multiple_line_comment[0].Length()), comment, code, multi_line_comment);
      }
      else
      {
      	code = true;
      }
   }
}

CodeStatExecDlg::~CodeStatExecDlg()
{
}
