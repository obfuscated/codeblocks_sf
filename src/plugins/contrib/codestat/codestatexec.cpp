/***************************************************************
 * Name:      codestatexec.cpp
 * Purpose:   Code::Blocks CodeStat plugin: main window
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/
#include "codestatexec.h"

#include "sdk.h"
#ifndef CB_PRECOMP
#   include <wx/choice.h>
#   include <wx/gauge.h>
#   include <wx/intl.h>
#   include <wx/sizer.h>
#   include <wx/stattext.h>
#   include <wx/string.h>
#   include <wx/xrc/xmlres.h>
#   include "cbproject.h"
#   include "configmanager.h"
#   include "editormanager.h"
#   include "globals.h"
#   include "manager.h"
#   include "logmanager.h"
#   include "projectfile.h"
#   include "projectmanager.h"
#endif

#include <wx/progdlg.h>
#include <wx/textfile.h>

void CountLines(ProjectCodeStats& stat, const wxFileName &filename, const LanguageDef &language);

CodeStatExecDlg::CodeStatExecDlg(wxWindow* parent) :
    m_languages(nullptr),
    m_numLanguages(0),
    m_numFiles(0),
    m_currentFile(0),
    m_changed(false)
{
    m_progress = nullptr;

    // Setting-up the statistics dialog box
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgCodeStatExec"),_T("wxScrollingDialog"));

    m_choice = XRCCTRL(*this, "ID_CHOICE1", wxChoice);
    m_choice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&CodeStatExecDlg::OnSelectProject, NULL, this);
    this->Connect(wxEVT_IDLE, (wxObjectEventFunction)&CodeStatExecDlg::OnIdle, NULL, this);
}

CodeStatExecDlg::~CodeStatExecDlg()
{
    m_choice->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&CodeStatExecDlg::OnSelectProject, NULL, this);
    this->Disconnect(wxEVT_IDLE, (wxObjectEventFunction)&CodeStatExecDlg::OnIdle, NULL, this);
}

/** Count the lines on all project's files and display the results.
 *  @param languages Languages definitions
 *  @param nb_languages Number of languages defined in the 'languages' array
 */
int CodeStatExecDlg::Execute(LanguageDef languages[NB_FILETYPES_MAX], int numLanguages)
{
    m_choice->Clear();
    m_choice->Append(_T("Entire workspace"));

    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0, length = projects->GetCount(); i < length; ++i)
    {
        m_choice->Append(projects->Item(i)->GetTitle());
    }
    m_cache.clear();
    m_cache.resize(projects->GetCount() + 1);

    m_languages = languages;
    m_numLanguages = numLanguages;

    // Check if all files have been saved
    bool all_saved = true;
    for (size_t i = 0, nb_projects = projects->GetCount(); i < nb_projects; ++i)
    {
        cbProject* project = projects->Item(i);
        for (int j = 0, nb_files = project->GetFilesCount(); j < nb_files; ++j)
        {
            ProjectFile* pf = project->GetFile(j);
            if (pf->GetFileState() == fvsModified)
            {
                all_saved = false;
                break;
            }
        }
    }

    // If not, ask user if we can save them
    if (!all_saved)
    {
        if (cbMessageBox(_T("Some files are not saved.\nDo you want to save them before running the plugin?"),
                         _("Warning"),
                         wxICON_EXCLAMATION | wxYES_NO,
                         Manager::Get()->GetAppWindow()) == wxID_YES)
        {
            for (size_t i = 0, nb_projects = projects->GetCount(); i < nb_projects; ++i)
                (*projects)[i]->SaveAllFiles();
        }
    }

    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    int index = m_choice->FindString(project->GetTitle(), true);
    m_choice->SetSelection(index);
    DoParseProject(index);

    ShowResults(index);

    ShowModal();

    return 0;
}

void CodeStatExecDlg::EndModal(int retCode)
{
    wxScrollingDialog::EndModal(retCode);
}

void CodeStatExecDlg::OnSelectProject(wxCommandEvent& evt)
{
    m_changed = true;
    evt.Skip();
}

void CodeStatExecDlg::OnIdle(wxIdleEvent& evt)
{
    if (!m_changed)
        return;

    m_changed = false;

    int index = m_choice->GetSelection();
    if (index == 0)
    {
        DoParseWorkspace();

    }
    else
        DoParseProject(index);
    ShowResults(index);

    evt.Skip();
}

ProjectCodeStats CodeStatExecDlg::ParseProject(int index, ParsedFileNamesSet *parsedFileNames)
{
    ProjectCodeStats stat;
    cbProject* project = Manager::Get()->GetProjectManager()->GetProjects()->Item(index - 1);
    stat.numFiles = project->GetFilesCount();

    for (int i = 0 ; i < stat.numFiles; ++i)
    {
        ProjectFile* pf = project->GetFile(i);
        wxFileName filename(pf->file.GetFullPath(), wxPATH_DOS);

        if (parsedFileNames)
        {
            if (parsedFileNames->find(filename.GetFullPath())==parsedFileNames->end())
                parsedFileNames->insert(filename.GetFullPath());
            else
                continue;
        }

        if (!filename.FileExists())
            ++stat.numFilesNotFound;
        else
        {
            // Find the language associated to the file extension
            int language = -1;
            for (int l = 0; l < m_numLanguages; ++l)
            {
                for (int j = 0; j < (int)m_languages[l].ext.Count(); ++j)
                {
                    if (filename.GetExt() == m_languages[l].ext[j])
                    {
                        language = l;
                        break;
                    }
                }
            }

            // If the language is found, analyse the source file
            if (language > -1)
                CountLines(stat, filename, m_languages[language]);
            else
                ++stat.numSkippedFiles;
        }
        if (stat.numFiles > 1)
        {
            ++m_currentFile;
            UpdateProgress();
        }
    }
    return stat;
}

void CodeStatExecDlg::DoParseProject(int index)
{
    if (m_cache[index].isParsed || index == 0)
        return;

    m_progress = new wxProgressDialog(_("Code Statistics plugin"),_("Parsing project files. Please wait..."));

    cbProject* project = Manager::Get()->GetProjectManager()->GetProjects()->Item(index - 1);
    m_currentFile = 0;
    m_numFiles = project->GetFilesCount();
    m_cache[index]=ParseProject(index, nullptr);

    m_progress->Update(100);
    delete m_progress;
    m_progress = nullptr;
    m_cache[index].isParsed = true;
}

void CodeStatExecDlg::DoParseWorkspace()
{
    ProjectCodeStats& statWS = m_cache[0];
    if (statWS.isParsed)
        return;

    m_progress = new wxProgressDialog(_("Code Statistics plugin"),_("Parsing workspace files. Please wait..."));

    m_currentFile = 0;
    m_numFiles = 0;

    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0, count = projects->GetCount(); i < count; ++i)
        m_numFiles += projects->Item(i)->GetFilesCount();

    ParsedFileNamesSet parsedFileNames;

    for (size_t i = 1, count = projects->GetCount(); i < count + 1; ++i)
    {
        ProjectCodeStats statProject = ParseProject(i, &parsedFileNames);

        statWS.numFiles           += statProject.numFiles;
        statWS.numFilesNotFound   += statProject.numFilesNotFound;
        statWS.numSkippedFiles    += statProject.numSkippedFiles;
        statWS.codeLines          += statProject.codeLines;
        statWS.emptyLines         += statProject.emptyLines;
        statWS.commentLines       += statProject.commentLines;
        statWS.codeAndCommentLines += statProject.codeAndCommentLines;
        statWS.totalLines         += statProject.totalLines ;
    }

    statWS.isParsed = true;

    m_progress->Update(100);
    delete m_progress;
    m_progress = nullptr;
}

void CodeStatExecDlg::UpdateProgress() {
    if (m_progress)
        m_progress->Update((100 * m_currentFile)/(m_numFiles - 1));
}

void CodeStatExecDlg::ShowResults(int index) {
    ProjectCodeStats& stat = m_cache[index];

    wxStaticText* txt_num_files = XRCCTRL(*this, "txt_num_files", wxStaticText);
    txt_num_files->SetLabel(wxString::Format(_("%ld"), stat.numFiles));
    wxStaticText* txt_skipped_files = XRCCTRL(*this, "txt_skipped_files", wxStaticText);
    txt_skipped_files->SetLabel(wxString::Format(_("%ld"), stat.numSkippedFiles));
    wxStaticText* txt_files_not_found = XRCCTRL(*this, "txt_files_not_found", wxStaticText);
    txt_files_not_found->SetLabel(wxString::Format(_("%ld"), stat.numFilesNotFound));

    wxStaticText* txt_Code = XRCCTRL(*this, "txt_Code", wxStaticText);
    txt_Code->SetLabel(wxString::Format(_("%ld"), stat.codeLines));
    wxStaticText* txt_Empty = XRCCTRL(*this, "txt_Empty", wxStaticText);
    txt_Empty->SetLabel(wxString::Format(_("%ld"), stat.emptyLines));
    wxStaticText* txt_Comments = XRCCTRL(*this, "txt_Comments", wxStaticText);
    txt_Comments->SetLabel(wxString::Format(_("%ld"), stat.commentLines));
    wxStaticText* txt_Code_Comments = XRCCTRL(*this, "txt_Code_Comments", wxStaticText);
    txt_Code_Comments->SetLabel(wxString::Format(_("%ld"), stat.codeAndCommentLines));
    wxStaticText* txt_Total = XRCCTRL(*this, "txt_Total", wxStaticText);
    txt_Total->SetLabel(wxString::Format(_("%ld"), stat.totalLines));

    // If the project is not empty, display the main dialog box
    if(stat.totalLines) // avoid division by zero on empty document
    {
        int icode = static_cast<int>(round(100.0 * stat.codeLines / stat.totalLines));
        wxGauge* Gauge_Code = XRCCTRL(*this, "Gauge_Code", wxGauge);
        Gauge_Code->SetValue(icode);
        wxStaticText* txt_Gauge_Code = XRCCTRL(*this, "txt_Gauge_Code", wxStaticText);
        txt_Gauge_Code->SetLabel(wxString::Format(_("%3d%% Code only"), icode));

        int percentCodeComments = static_cast<int>(round(100.0 * stat.codeAndCommentLines / stat.totalLines));
        wxGauge* Gauge_Code_Comments = XRCCTRL(*this, "Gauge_Code_Comments", wxGauge);
        Gauge_Code_Comments->SetValue(percentCodeComments);
        wxStaticText* txt_Gauge_Code_Comments = XRCCTRL(*this, "txt_Gauge_Code_Comments", wxStaticText);
        txt_Gauge_Code_Comments->SetLabel(wxString::Format(_("%3d%% Code + Comment"), percentCodeComments));

        int percentComments = static_cast<int>(round(100.0 * stat.commentLines / stat.totalLines));
        wxGauge* Gauge_Comments = XRCCTRL(*this, "Gauge_Comments", wxGauge);
        Gauge_Comments->SetValue(percentComments);
        wxStaticText* txt_Gauge_Comments = XRCCTRL(*this, "txt_Gauge_Comments", wxStaticText);
        txt_Gauge_Comments->SetLabel(wxString::Format(_("%3d%% Comments"), percentComments));

        int percentEmpty = static_cast<int>(round(100.0 * stat.emptyLines / stat.totalLines));
        wxGauge* Gauge_Empty = XRCCTRL(*this, "Gauge_Empty", wxGauge);
        Gauge_Empty->SetValue(percentEmpty);
        wxStaticText* txt_Gauge_Empty = XRCCTRL(*this, "txt_Gauge_Empty", wxStaticText);
        txt_Gauge_Empty->SetLabel(wxString::Format(_("%3d%% Empty"), percentEmpty));

        GetSizer()->Layout();
    }
    else
        cbMessageBox(_("The project is empty!"), _("Warning"), wxICON_EXCLAMATION | wxOK, Manager::Get()->GetAppWindow());

}

namespace
{
/** This function determines the caracteristics of a given line (code line, comment line etc...).
 *  It is called by the "CountLines" function.
 *  @see CountLines
 */
void AnalyseLine(bool &comment, bool &code, bool &multi_line_comment, const LanguageDef &language, wxString line)
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
            {
                AnalyseLine(comment, code, multi_line_comment, language,
                            line.Mid(first_multi_line_comment_end+language.multiple_line_comment[1].Length()));
            }
        }
    }
    // We are not in a multiple line comment
    else if (!multi_line_comment)
    {
        // First comment sign found is a single line comment sign
        if ( (first_single_line_comment > -1)
            && ((first_multi_line_comment_begin == -1)
                || ((first_multi_line_comment_begin>-1)&&(first_single_line_comment<first_multi_line_comment_begin)))
           )
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
            {
                AnalyseLine(comment, code, multi_line_comment, language,
                            line.Mid(first_multi_line_comment_begin+language.multiple_line_comment[0].Length()));
            }
        }
        else
            code = true;
    }
}
}

/** This function analyses a given source file and count the lines of code, comments etc...
 */
void CountLines(ProjectCodeStats& stat, const wxFileName &filename, const LanguageDef &language)
{
    wxTextFile file;
    if (file.Open(filename.GetFullPath(), wxConvFile))
    {
        bool multiLineComment = false;
        stat.totalLines += file.GetLineCount();
        for (unsigned int i=0; i<file.GetLineCount(); ++i)
        {
            wxString line(file[i]);
            line = line.Trim(true);
            line = line.Trim(false);
            bool comment = false;
            bool code = false;
            if (line.IsEmpty())
                ++stat.emptyLines;
            else
            {
                AnalyseLine(comment, code, multiLineComment, language, line);
                if (comment&&code)
                    ++stat.codeAndCommentLines;
                else if (comment)
                    ++stat.commentLines;
                else if (code)
                    ++stat.codeLines;
            }
        }
    }
}
