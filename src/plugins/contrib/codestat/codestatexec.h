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

#include <vector>

#include <wx/filename.h>

#include "scrollingdialog.h"
#include "language_def.h"

class wxWindow;
class wxProgressDialog;

struct ProjectCodeStats
{
    ProjectCodeStats() :
        numFiles(0),
        numFilesNotFound(0),
        numSkippedFiles(0),
        codeLines(0),
        emptyLines(0),
        commentLines(0),
        codeAndCommentLines(0),
        totalLines(0),
        isParsed(false)
    {}

    long numFiles;
    long numFilesNotFound;
    long numSkippedFiles;

    long codeLines;
    long emptyLines;
    long commentLines;
    long codeAndCommentLines;
    long totalLines;

    bool isParsed;
};

/** This class computes the statistics of the project's files and display them.
 *  @see CodeStat, CodeStatConfigDlg, CodeStatExecDlg, LanguageDef
 */
class CodeStatExecDlg : public wxScrollingDialog
{
public:
    typedef std::set<wxString> ParsedFileNamesSet;
public:
    CodeStatExecDlg(wxWindow* parent);
    virtual ~CodeStatExecDlg();
    int Execute(LanguageDef languages[NB_FILETYPES_MAX], int numLanguages);
private:
    void EndModal(int retCode);
    void OnSelectProject(wxCommandEvent& evt);
    void OnIdle(wxIdleEvent& evt);
    ProjectCodeStats ParseProject(int index, ParsedFileNamesSet *parsedFileNames);
    void DoParseProject(int index);
    void DoParseWorkspace();
    void UpdateProgress();
    void ShowResults(int index);

private:
    wxChoice* m_choice;
    wxProgressDialog* m_progress;
    std::vector<ProjectCodeStats> m_cache;
    LanguageDef* m_languages;
    int m_numLanguages;
    int m_numFiles;
    int m_currentFile;
    bool m_changed;
};

#endif // CODESTATEXEC_H
