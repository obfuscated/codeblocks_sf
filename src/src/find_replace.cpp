#include "find_replace.h"

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include <wx/regex.h>

    #include "cbauibook.h"
    #include "cbeditor.h"
    #include "cbproject.h"
    #include "editormanager.h"
    #include "infowindow.h"
    #include "projectfile.h"
#endif

#include <wx/progdlg.h>

#include "annoyingdialog.h"
#include "cbstyledtextctrl.h"
#include "confirmreplacedlg.h"
#include "encodingdetector.h"
#include "findreplacedlg.h"
#include "searchresultslog.h"

struct cbFindReplaceData
{
    int start;
    int end;
    wxString findText;
    wxString replaceText;
    bool initialreplacing;
    bool findInFiles;
    bool delOldSearches;
    bool sortSearchResult;
    bool matchWord;
    bool startWord;
    bool startFile; //!< To be implemented.
    bool matchCase;
    bool regEx;
    bool directionDown;
    bool originEntireScope;
    int scope;
    wxString searchPath;
    wxString searchMask;
    int searchProject;
    int searchTarget;
    bool recursiveSearch;
    bool hiddenSearch;
    bool NewSearch;     //!< only true when a new search has been started
    int SearchInSelectionStart; //!< keep track of the start of a 'search' selection
    int SearchInSelectionEnd;  //!< keep track of the end of a 'search' selection
    bool autoWrapSearch;
    bool findUsesSelectedText;
    bool multiLine; //!< for multi-line S&R.
    bool fixEOLs; //!< for multi-line S&R. Fixes EOLs in all the files searched.
    int eolMode; //!< for multi-line S&R

    void ConvertEOLs(int newmode);
    bool IsMultiLine();

    cbFindReplaceData()
    {
        eolMode = wxSCI_EOL_LF;
        fixEOLs = false;
    }
};

void cbFindReplaceData::ConvertEOLs(int newmode)
{
    if (eolMode != newmode)
    {
        const wxChar* eol_lf = _T("\n");
        const wxChar* eol_crlf = _T("\r\n");
        const wxChar* eol_cr = _T("\r");

        const wxChar* eol_from = eol_lf;
        const wxChar* eol_to = eol_lf;
        switch(eolMode)
        {
            case wxSCI_EOL_CR: eol_from = eol_cr; break;
            case wxSCI_EOL_CRLF: eol_from = eol_crlf; break;
            default: ;
        }
        switch(newmode)
        {
            case wxSCI_EOL_CR: eol_to = eol_cr; break;
            case wxSCI_EOL_CRLF: eol_to = eol_crlf; break;
            default: newmode = wxSCI_EOL_LF;
        }
        findText.Replace(eol_from, eol_to, true);
        replaceText.Replace(eol_from, eol_to, true);
        eolMode = newmode;
    }
}

bool cbFindReplaceData::IsMultiLine()
{
    if (regEx) // For regex always assume multiline if the multiline checkbox is enabled because the user can enter "\n" to search for newlines
        return multiLine;
    // otherwise only treat the search as a multiline search only if there are newline characters in the search string
    return  ((findText.Find(_T("\n")) != wxNOT_FOUND) || (findText.Find(_T("\r")) != wxNOT_FOUND));
}

FindReplace::FindReplace() :
    m_LastFindReplaceData(nullptr)
{
}

FindReplace::~FindReplace()
{
    delete m_LastFindReplaceData;
}

void FindReplace::CreateSearchLog()
{
    if (Manager::IsBatchBuild())
        return;

    wxArrayInt widths;
    wxArrayString titles;
    titles.Add(_("File"));
    titles.Add(_("Line"));
    titles.Add(_("Text"));
    widths.Add(128);
    widths.Add(48);
    widths.Add(640);

    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/16x16/");
    wxBitmap * bmp = new wxBitmap(cbLoadBitmap(prefix + _T("filefind.png"), wxBITMAP_TYPE_PNG));

    m_pSearchLog = new cbSearchResultsLog(titles, widths);
    CodeBlocksLogEvent evt(cbEVT_ADD_LOG_WINDOW, m_pSearchLog, _("Search results"), bmp);
    Manager::Get()->ProcessEvent(evt);
    Manager::Get()->SetSearchResultLogger(m_pSearchLog);
}

void FindReplace::LogSearch(const wxString& file, int line, const wxString& lineText)
{
    wxArrayString values;
    wxString lineTextL;
    wxString lineStr;

    // line number -1 is used for empty string
    if (line != -1)
        lineStr.Printf(_T("%d"), line);
    else
        lineStr.Printf(_T(" "));

    lineTextL = lineText;
    lineTextL.Replace(_T("\t"), _T(" "));
    lineTextL.Replace(_T("\r"), _T(" "));
    lineTextL.Replace(_T("\n"), _T(" "));
    lineTextL.Trim(false);
    lineTextL.Trim(true);

    values.Add(file);
    values.Add(lineStr);
    values.Add(lineTextL);

    m_pSearchLog->Append(values, line == -1 ? Logger::caption : Logger::info);
}

int FindReplace::ShowFindDialog(bool replace, bool explicitly_find_in_files)
{
    wxString phraseAtCursor;
    bool hasSelection = false;
    cbStyledTextCtrl* control = 0;

    EditorManager *editorMgr = Manager::Get()->GetEditorManager();

    cbEditor* ed = editorMgr->GetBuiltinEditor(editorMgr->GetActiveEditor());
    if (ed)
    {
        control = ed->GetControl();

        hasSelection = control->GetSelectionStart() != control->GetSelectionEnd();
        int wordStart = control->WordStartPosition(control->GetCurrentPos(), true);
        int wordEnd = control->WordEndPosition(control->GetCurrentPos(), true);
        wxString wordAtCursor = control->GetTextRange(wordStart, wordEnd);
        phraseAtCursor = control->GetSelectedText();
        // if selected text is part of a single line, don't suggest "search in selection"
        if (control->LineFromPosition(control->GetSelectionStart())
            == control->LineFromPosition(control->GetSelectionEnd()))
            hasSelection = false;

        if ( phraseAtCursor.IsEmpty())
            phraseAtCursor = wordAtCursor;

        int selstartline = control->LineFromPosition(control->GetSelectionStart());
        int selendline   = control->LineFromPosition(control->GetSelectionEnd());
        // the selection of several lines is not proposed as search pattern
        if ( selstartline != selendline )
            phraseAtCursor = wxEmptyString;

    }

    FindReplaceBase* dlg = new FindReplaceDlg(Manager::Get()->GetAppWindow(), phraseAtCursor, hasSelection,
                                              !replace, !ed, explicitly_find_in_files);

    PlaceWindow(dlg);
    if (dlg->ShowModal() == wxID_CANCEL)
    {
        dlg->Destroy();
        return -2;
    }

    // Don't look for empty strings:
    if (dlg->GetFindString().empty())
    {
        dlg->Destroy();
        cbMessageBox(_("Can't look for an empty search criterion!"), _("Error"), wxOK | wxICON_EXCLAMATION, Manager::Get()->GetAppWindow());
        return -2;
    }

    if (!m_LastFindReplaceData)
        m_LastFindReplaceData = new cbFindReplaceData;

    m_LastFindReplaceData->start = 0;
    m_LastFindReplaceData->end = 0;
    m_LastFindReplaceData->findText = dlg->GetFindString();
    m_LastFindReplaceData->replaceText = dlg->GetReplaceString();
    m_LastFindReplaceData->eolMode = wxSCI_EOL_LF;
    m_LastFindReplaceData->multiLine = dlg->GetMultiLine();
    m_LastFindReplaceData->fixEOLs = dlg->GetFixEOLs();
    m_LastFindReplaceData->startFile = dlg->GetStartFile();

    m_LastFindReplaceData->findInFiles = dlg->IsFindInFiles();
    if (!m_LastFindReplaceData->findInFiles)
    {
        //AutoWrapSearch does not exist in FindInFiles dialog
        m_LastFindReplaceData->autoWrapSearch = dlg->GetAutoWrapSearch();

        //FindUsesSelectedText does not exist in Replace dialogs
        if (!replace)
            m_LastFindReplaceData->findUsesSelectedText = dlg->GetFindUsesSelectedText();
    }
    m_LastFindReplaceData->delOldSearches = dlg->GetDeleteOldSearches();
    m_LastFindReplaceData->sortSearchResult = dlg->GetSortSearchResult();
    m_LastFindReplaceData->matchWord = dlg->GetMatchWord();
    m_LastFindReplaceData->startWord = dlg->GetStartWord();
    m_LastFindReplaceData->matchCase = dlg->GetMatchCase();
    m_LastFindReplaceData->regEx = dlg->GetRegEx();
    m_LastFindReplaceData->directionDown = dlg->GetDirection() == 1;
    m_LastFindReplaceData->originEntireScope = dlg->GetOrigin() == 1;
    m_LastFindReplaceData->scope = dlg->GetScope();
    m_LastFindReplaceData->searchPath = dlg->GetSearchPath();
    m_LastFindReplaceData->searchMask = dlg->GetSearchMask();
    m_LastFindReplaceData->recursiveSearch = dlg->GetRecursive();
    m_LastFindReplaceData->searchProject = dlg->GetProject();
    m_LastFindReplaceData->searchTarget = dlg->GetTarget();
    m_LastFindReplaceData->hiddenSearch = dlg->GetHidden();
    m_LastFindReplaceData->initialreplacing = false;
    m_LastFindReplaceData->NewSearch = true;

    if (control)
    {   // if editor : store the selection start/end
        // only use this in case of !findInFiles and scope==1 (search in selection)
        m_LastFindReplaceData->SearchInSelectionStart = control->GetSelectionStart();
        m_LastFindReplaceData->SearchInSelectionEnd = control->GetSelectionEnd();
    }
    dlg->Destroy();

    if ( m_LastFindReplaceData->regEx )
    {
        // Match nasty regexes
        if ( m_LastFindReplaceData->findText.IsSameAs('^') || m_LastFindReplaceData->findText.IsSameAs('$') )
        {
            cbMessageBox(_T("Bad regex entered!\nPlease correct regex and try again!"), _T("Error!"), wxICON_ERROR);
            return 0;
        }
    }

    int ReturnValue = 0;
    if (!replace)
    {
        if (m_LastFindReplaceData->findInFiles)
            ReturnValue = FindInFiles(m_LastFindReplaceData);
        else
            ReturnValue = Find(control, m_LastFindReplaceData);
    }
    else
    {
        m_LastFindReplaceData->initialreplacing = true;

        if (m_LastFindReplaceData->findInFiles)
            ReturnValue = ReplaceInFiles(m_LastFindReplaceData);
        else
            ReturnValue = Replace(control, m_LastFindReplaceData);
    }
    m_LastFindReplaceData->NewSearch = false; // we have searched, so no longer new search

    //Default back to find or replace in Editor
    if (m_LastFindReplaceData->findInFiles)
    {
        m_LastFindReplaceData->findInFiles = false;
    }
    return ReturnValue;
}

void FindReplace::CalculateFindReplaceStartEnd(cbStyledTextCtrl* control, cbFindReplaceData* data, bool replace)
{
    if (!control || !data)
        return;
    if (data->startFile) // Beginning-of-file needs the entire scope
    {
        int clen = control->GetLength();
        int slen = data->findText.Len();

        data->start = 0;
        data->end = std::min(slen, clen);
    }
    else if (!data->findInFiles)   // Find in current Editor
    {
        int ssta = control->GetSelectionStart();
        int send = control->GetSelectionEnd();
        int cpos = control->GetCurrentPos();
        int clen = control->GetLength();

        // when the user initially had a selection, but then changed the scope
        // to entire scope, the values of ssta and send will have a bad influence in
        // the following calculations, therefor check for the scenario
        // and set the ssta en send to cpos (in the case there would be no selection
        // that's the value they have [no selection : ssta=send=cpos])
        // only do this when it's a new search (when the search is continued (F3/Shift-F3)
        // there can be a selection, the last found match)
        if ((data->scope == 0) && data->NewSearch && (ssta != cpos || send != cpos))
        {
            // Don't do this in replace mode, because we want to start the replacement
            // with the current selection, not the first match after the selection.
            if (!replace)
            {
                ssta = cpos;
                send = cpos;
            }
        }



        data->start = 0;
        data->end   = clen;

        if (!data->originEntireScope || !data->NewSearch)   // from pos or next/prev search
        {
            if (!data->directionDown)   // up
                // initial replacing mode - include selection end : normal mode - skip until selection start
                data->start = (data->initialreplacing)? std::max(send, cpos) : std::min(ssta, cpos);
            else                      // down
                // initial replacing mode - include selection start : normal mode - skip until selection end
                data->start = (data->initialreplacing)? std::min(ssta, cpos) : std::max(send, cpos);
        }
        else                            // entire scope
        {
            if (!data->directionDown)   // up
                data->start = clen;
        }

        if (!data->directionDown)       // up
            data->end = 0;

        // selected text, if user has deslected since last, then change scope
        if (data->scope == 1 &&
            control->GetSelectionStart()==control->GetSelectionEnd())
                data->scope = 0; // global

        if (data->scope == 1) // selected text
        {
            if (data->NewSearch)
            {
                if (!data->directionDown)   // up
                {
                    data->start = std::max(ssta, send);
                    data->end   = std::min(ssta, send);
                }
                else // down
                {
                    data->start = std::min(ssta, send);
                    data->end   = std::max(ssta, send);
                }
            }
            else
            {   // this is the result of a next/previous search
                // rebase depending on the cursor position
                ssta = data->SearchInSelectionStart;
                send = data->SearchInSelectionEnd;
                if (cpos < ssta || cpos > send)
                {   // regular reset (this also provide some sort of wrap around) (other editors also did it like that)
                    data->start = ssta;
                    data->end = send;
                }
                else
                {
                    data->start = cpos;
                    data->end = (data->directionDown)?send:ssta;
                }
            }
        }
    }
    else        // FindInFiles
    {           // searching direction down, entire scope
        //Replace needs the entire scope, while find can wrap around.
        data->start = ( replace ? 0 : control->GetCurrentPos() );
        data->end   = control->GetLength();
    }
}

int FindReplace::Replace(cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control || !data)
        return -1;

    if (control->GetReadOnly())
    {
        cbMessageBox(_("This file is read-only.\nReplacing in a read-only file is not possible."),
                     _("Warning"), wxICON_EXCLAMATION);
        return -1;
    }

    bool advRegex = false;
    bool advRegexNewLinePolicy =! data->IsMultiLine();
    int replacecount = 0;
    int foundcount   = 0;
    int flags        = 0;

    {
        int eolMode = control->GetEOLMode();
        data->ConvertEOLs(eolMode); // Convert our S&R data to the file's EOL mode.
        if (data->IsMultiLine() && data->fixEOLs)
        {
            // First we must ensure that the file has consistent line endings.
            // As all the file's lines are affected, we disable change history for this step.

            control->BeginUndoAction();
            control->SetChangeCollection(false);
            control->ConvertEOLs(eolMode);
            control->SetChangeCollection(true);
            control->EndUndoAction();
        }
    }
    control->BeginUndoAction(); // The undo is set at this point in case we need to convert the EOLs.

    CalculateFindReplaceStartEnd(control, data, true);

    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
    {
        flags |= wxSCI_FIND_REGEXP;
        if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_posix_style_regexes"), false))
            flags |= wxSCI_FIND_POSIX;
        #ifdef wxHAS_REGEX_ADVANCED
        advRegex = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_advanced_regexes"), false);
        #endif
    }

    wxRegEx re;
    #ifdef wxHAS_REGEX_ADVANCED
    if (advRegex)
    {
        if (data->matchCase)
            re.Compile(data->findText,wxRE_ADVANCED|(wxRE_NEWLINE*advRegexNewLinePolicy));
        else
            re.Compile(data->findText,wxRE_ADVANCED|(wxRE_NEWLINE*advRegexNewLinePolicy)|wxRE_ICASE);
    }
    #endif

    int pos = -1;
    bool replace = false;
    bool confirm = true;
    bool stop = false;
    wxPoint LastDlgPosition;
    bool HaveLastDlgPosition = false;
    bool wrapAround = false;
    int  data_start_initial = data->start;
    bool wrapAroundNotification = false;

    while (!stop)
    {
        int lengthFound = 0;
        if (!advRegex)
            pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
        else
        {
            wxString text=control->GetTextRange(data->start, data->end);
            if (re.Matches(text))
            {
                size_t start,len;
                re.GetMatch(&start,&len,0);
                pos=start+data->start;
                lengthFound=len;
                if ((start==0) && (len==0)) //For searches for "^" or "$" (and null returning variants on this) need to make sure we have forward progress and not simply matching on a previous BOL/EOL find
                {
                    text=text.Mid(1);
                    if (re.Matches(text))
                    {
                        re.GetMatch(&start, &len, 0);
                        pos = start+data->start + 1;
                        lengthFound = len;
                    } else
                        pos=-1;
                }
            } else
                pos=-1;
        }

        if (data->startFile && pos > 0)
            pos = -1; // Not found at the beginning of file

        if (pos != -1 && data->start!=data->end)
        {
            control->GotoPos(pos);
            control->EnsureVisible(control->LineFromPosition(pos));
            control->SetSelectionVoid(pos, pos + lengthFound);
            data->start = pos;
            data->initialreplacing = false;  // special treatment only necessary the first time
        }
        else if (!wrapAround)
        {
            if ( (data->scope == 0) &&      // scope = global text
                    ((data->directionDown && data_start_initial != 0) ||
                     (!data->directionDown && data_start_initial != control->GetLength())))
            {
                wxString msg;
                if (data->directionDown)
                    msg = _("Text not found.\nSearch from the start of the document?");
                else
                    msg = _("Text not found.\nSearch from the end of the document?");

                bool auto_wrap_around = data->autoWrapSearch;
                if (auto_wrap_around)
                    wrapAroundNotification = true;
                if (auto_wrap_around || cbMessageBox(msg, _("Result"), wxOK | wxCANCEL | wxICON_QUESTION) == wxID_OK)
                {
                    data->end = data_start_initial;
                    data->start = (data->directionDown)? 0 : control->GetLength();
                    wrapAround = true; // signal the wrap-around
                    continue;
                }
                else
                    break;  // done - user doesn't want to wrap around
            }
            else
                break; // done - we're replacing in a selection of text
        }
        else
            break; // done - already wrapped around once

        if (wrapAroundNotification)
        {
            wxBell();
            InfoWindow::Display(_("Find action"), _("Reached the end of the document"), 1000);
            wrapAroundNotification = false;
        }

        foundcount++;

        if (confirm)
        {
            ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow());
            // dlg.CalcPosition(control);
            // TODO (thomas#1#): Check whether the existing code actually works with twin view
            // else, we need something like:
            // PlaceWindow(&dlg, pdlRelative);

            // NOTE (Tiwag#1#): dlg.CalcPosition doesn't work for me with dual monitor setup,
            //     workaround : remember last dialog position, user can position
            //                  it outside of text where he wants
            // Move dialog to last position if already available,
            // else place it according to environments settings
            if ( HaveLastDlgPosition )
                dlg.Move(LastDlgPosition);
            else
                dlg.CalcPosition(control);

            int ans = dlg.ShowModal();
            LastDlgPosition = dlg.GetPosition();
            HaveLastDlgPosition = true;
            switch (ans)
            {
            case crYes:
                replace = true;
                break;
            case crNo:
                replace = false;
                break;
            case crAll:
                replace = true;
                confirm = false;
                control->Freeze();
                break;
            case crCancel:
                stop = true;
                break;
            default:
                break;
            }
        }

        if (!stop)
        {
            if (replace)
            {
                int lengthReplace = data->replaceText.Length();
                replacecount++;
                if (data->regEx)
                {
                    // set target same as selection
                    control->SetTargetStart(control->GetSelectionStart());
                    control->SetTargetEnd(control->GetSelectionEnd());
                    if (advRegex)
                    {
                        wxString text=control->GetSelectedText();
                        re.Replace(&text,data->replaceText,1);
                        lengthReplace=text.Len();
                        control->ReplaceSelection(text);
                    }
                    else
                    {
                        // replace with regEx support
                        lengthReplace = control->ReplaceTargetRE(data->replaceText);
                    }

                    // reset target
                    control->SetTargetStart(0);
                    control->SetTargetEnd(0);
                }
                else
                    control->ReplaceSelection(data->replaceText);
                if (data->directionDown)
                    data->start += lengthReplace;

                // adjust end pos by adding the length difference between find and replace strings
                int diff = lengthReplace - lengthFound;
                if (data->directionDown)
                    data->end += diff;
                else
                {
                    if (data->end < diff)
                       data->end = 0;
                    else
                       data->end -= diff;
                }
            }
            else
            {
                if (data->directionDown)
                    data->start += lengthFound;
                else
                    data->start -= lengthFound;
            }
        }
    }
    if (control->IsFrozen())
        control->Thaw();
    control->EndUndoAction();
    wxString msg;
    if (foundcount == 0)
        msg = _T("No matches found for \"") + data->findText + _T("\"");
    else if (replacecount == 0 && foundcount == 1)
        msg = _T("One match found but not replaced");
    else
        msg.Printf(_("Replaced %i of %i matches"), replacecount, foundcount);
    cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
    control->SetSCIFocus(true);

    return pos;
}

int FindReplace::ReplaceInFiles(cbFindReplaceData* data)
{
    if (!data) return 0;
    if (data->findText.IsEmpty()) return 0;

    EditorManager *editorMgr = Manager::Get()->GetEditorManager();

    bool IsMultiLine = data->IsMultiLine();

    // let's make a list of all the files to search in
    wxArrayString filesList;

    if (data->scope == 0) // find in open files
    {
        // fill the search list with the open files
        for (int i = 0; i < editorMgr->GetEditorsCount(); ++i)
        {
            cbEditor* ed = editorMgr->GetBuiltinEditor(i);
            if (ed)
                filesList.Add(ed->GetFilename());
        }
    }
    else if (data->scope == 1) // find in project files
    {
        // fill the search list with all the project files
        if (data->searchProject<0)
        {
            cbMessageBox(_("No project to search in!"), _("Error"), wxICON_WARNING);
            return 0;
        }
        cbProject* prj = (*Manager::Get()->GetProjectManager()->GetProjects())[data->searchProject];
        wxString target;
        wxString fullpath = _T("");
        if (data->searchTarget >= 0)
            target = prj->GetBuildTarget(data->searchTarget)->GetTitle();
        for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (pf)
            {
                if (target!=wxEmptyString && pf->buildTargets.Index(target)<0)
                    continue;
                fullpath = pf->file.GetFullPath();
                if (filesList.Index(fullpath) >= 0) // avoid adding duplicates
                    continue;
                if (wxFileExists(fullpath))  // Does the file exist?
                    filesList.Add(fullpath);
            }
        }
    }
    else if (data->scope == 2) // find in workspace
    {
        // loop over all the projects in the workspace (they are contained in the ProjectManager)
        const ProjectsArray* pProjects = Manager::Get()->GetProjectManager()->GetProjects();
        if (pProjects)
        {
            int count = pProjects->GetCount();
            for (int idxProject = 0; idxProject < count; ++idxProject)
            {
                cbProject* pProject = pProjects->Item(idxProject);
                if (pProject)
                {
                    wxString fullpath = _T("");

                    for (FilesList::iterator it = pProject->GetFilesList().begin(); it != pProject->GetFilesList().end(); ++it)
                    {
                        ProjectFile* pf = *it;
                        if (pf)
                        {
                            fullpath = pf->file.GetFullPath();
                            if (filesList.Index(fullpath) == -1) // avoid adding duplicates
                            {
                                if (wxFileExists(fullpath))  // Does the file exist?
                                    filesList.Add(fullpath);
                            }
                        }
                    } // end for : idx : idxFile
                }
            } // end for : idx : idxProject
        }
    }
    else if (data->scope == 3) // replace in custom search path and mask
     {
        // fill the search list with the files found under the search path
        int flags = wxDIR_FILES |
                    (data->recursiveSearch ? wxDIR_DIRS : 0) |
                    (data->hiddenSearch ? wxDIR_HIDDEN : 0);
        wxArrayString masks = GetArrayFromString(data->searchMask);
        if (!masks.GetCount())
            masks.Add(_T("*"));
        unsigned int count = masks.GetCount();
        wxLogNull ln; // no logging
        for (unsigned int i = 0; i < count; ++i)
        {
            // wxDir::GetAllFiles() does *not* clear the array, so it suits us just fine ;)
            wxDir::GetAllFiles(data->searchPath, &filesList, masks[i], flags);
        }
    }
    // if the list is empty, leave
    int filesCount = filesList.GetCount();
    if (filesCount == 0)
    {
        cbMessageBox(_("No files to search in!"), _("Error"), wxICON_WARNING);
        return 0;
    }

    bool advRegex=false;
    bool advRegexNewLinePolicy =! data->IsMultiLine();
    int flags = 0;
    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
    {
        flags |= wxSCI_FIND_REGEXP;
        if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_posix_style_regexes"), false))
            flags |= wxSCI_FIND_POSIX;
        #ifdef wxHAS_REGEX_ADVANCED
        advRegex = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_advanced_regexes"), false);
        #endif
    }

    wxRegEx re;
    #ifdef wxHAS_REGEX_ADVANCED
    if (advRegex)
    {
        if (data->matchCase)
            re.Compile(data->findText,wxRE_ADVANCED|(wxRE_NEWLINE*advRegexNewLinePolicy));
        else
            re.Compile(data->findText,wxRE_ADVANCED|(wxRE_NEWLINE*advRegexNewLinePolicy)|wxRE_ICASE);
    }
    #endif


    bool replace = false;
    bool confirm = true;
    bool stop = false;
    bool wholeFile = false;
    bool all = false;
    int pos = -1;

    wxPoint LastDlgPosition;
    bool HaveLastDlgPosition = false;

    wxProgressDialog* progress = 0;
    wxString fileContents;
    wxString enc_name = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/default_encoding"), wxLocale::GetSystemEncodingName());
    wxFontEncoding def_encoding = wxFontMapper::GetEncodingFromName(enc_name);

    // keep a copy of the find struct
    cbFindReplaceData dataCopy = *data;

    int read_only_files_skipped = 0;
    for (int i = 0; i<filesCount && !stop; ++i)
    {
        cbEditor*         ed      = NULL;
        cbStyledTextCtrl* control = NULL;
        bool fileWasNotOpen = false;

        if (progress)
        {
            if (!progress->Update(i))
            {
                if (cbMessageBox(_("Are you sure you want to stop replacing in files?"), _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
                    break;
                else
                    progress->Resume();
            }
        }

        //Check if this file is already open
        EditorBase *eb = editorMgr->IsOpen(filesList[i]);
        if (eb)
        {
            //File was already open
            fileWasNotOpen = false;

            ed = editorMgr->GetBuiltinEditor(eb);
            if (ed) control = ed->GetControl();
        }

        //If it's still NULL, open a new editor
        if (!control)
        {
            wxFile file(filesList[i]);
            if (!file.IsOpened())
                continue;
            fileContents = cbReadFileContents(file, def_encoding);
            if (advRegex)
            {
                if (!re.Matches(fileContents))
                    continue;
            } else
            {
                int pos_str;
                if (!data->matchCase)
                    pos_str = fileContents.Upper().Find(data->findText.Upper());
                else
                    pos_str = fileContents.Find(data->findText);
                if (pos_str == -1)
                    continue;
                //TODO: handling to skip file if data->matchWord or data->startWord are set
            }

            //File was not open, i opened it.
            fileWasNotOpen = true;

            ed = editorMgr->Open(filesList[i]);
            if (ed) control = ed->GetControl();
        }
        //Still NULL?
        if (!control || !ed)
            continue;

        if (control->GetReadOnly())
        {
            read_only_files_skipped++;
            continue;
        }

        editorMgr->SetActiveEditor(ed);

        *data = dataCopy;

        bool replacementsWereMade = false;
        // If we fix the file's EOLs for multi-line S&R, we're actually modifying it.
        // What we really want to know is whether we actually did a replacement.
        // If not (and the file was not open in the first place), we can safely close the file.

        {
            // We should be checking if the data has EOLs before converting them. But searching is
            // just as expensive as doing the conversion itself, so we just convert.
            int eolMode = control->GetEOLMode();
            data->ConvertEOLs(eolMode);

            if (IsMultiLine && data->fixEOLs)
            {
                control->BeginUndoAction(); //undo
                control->SetChangeCollection(false);
                control->ConvertEOLs(eolMode);
                control->SetChangeCollection(true);
                control->EndUndoAction();
            }
        }

        control->BeginUndoAction(); // undo
        CalculateFindReplaceStartEnd(control, data, true);

        //reset bools
        wholeFile = false;
        if (!all) confirm = true;

        //Replace in this file
        while (!stop || wholeFile)
        {
            int lengthFound = 0;
            if (!advRegex)
                pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
            else
            {
                wxString text=control->GetTextRange(data->start, data->end);
                if (re.Matches(text))
                {
                    size_t start,len;
                    re.GetMatch(&start, &len, 0);
                    pos = start + data->start;
                    lengthFound = len;
                    if ((start==0) && (len==0)) //For searches for "^" or "$" (and null returning variants on this) need to make sure we have forward progress and not simply matching on a previous BOL/EOL find
                    {
                        text = text.Mid(1);
                        if (re.Matches(text))
                        {
                            re.GetMatch(&start, &len, 0);
                            pos = start + data->start + 1;
                            lengthFound = len;
                        } else
                            pos = -1;
                    }
                } else
                    pos = -1;
            }

            if (data->startFile && (pos > 0))
                pos = -1; // Not found at the beginning of file

            if ((pos == -1) || (data->start == data->end))
                break;

            if (confirm)
            {
                control->GotoPos(pos);
                control->EnsureVisible(control->LineFromPosition(pos));
            }
            control->SetSelectionVoid(pos, pos + lengthFound);
            data->start = pos;
            data->initialreplacing = false;  // special treatment only necessary the first time

            if (confirm)
            {
                ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow(), true);
                // dlg.CalcPosition(control);
                // TODO (thomas#1#): Check whether the existing code actually works with twin view
                // else, we need something like:
                // PlaceWindow(&dlg, pdlRelative);

                // NOTE (Tiwag#1#): dlg.CalcPosition doesn't work for me with dual monitor setup,
                //     workaround : remember last dialog position, user can position
                //                  it outside of text where he wants
                // Move dialog to last position if already available,
                // else place it according to environments settings
                if ( HaveLastDlgPosition )
                    dlg.Move(LastDlgPosition);
                else
                    dlg.CalcPosition(control);

                int ans = dlg.ShowModal();
                LastDlgPosition = dlg.GetPosition();
                HaveLastDlgPosition = true;
                switch (ans)
                {
                    case crYes:
                        replace = true;
                        break;
                    case crNo:
                        replace = false;
                        break;
                    case crAllInFile:
                        confirm   = false;
                        replace   = true;
                        wholeFile = true;
                        break;
                    case crSkipFile:
                        confirm   = false;
                        replace   = false;
                        wholeFile = true;
                        break;
                    case crAll:
                        replace = true;
                        confirm = false;
                        all     = true;
                        // let's create a progress dialog because it might take some time depending on the files count
                        progress = new wxProgressDialog(_("Replace in files"),
                                     _("Please wait while replacing in files..."),
                                     filesCount,
                                     Manager::Get()->GetAppWindow(),
                                     wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);
                        PlaceWindow(progress);
                        // now that we need no confirmation, freeze the app window
                        Manager::Get()->GetAppWindow()->Freeze();
                        break;
                    case crCancel:
                        stop = true;
                        break;
                    default:
                        break;
                }
            }// if

            if (!stop)
            {
                if (replace)
                {
                    replacementsWereMade = true;
                    int lengthReplace = data->replaceText.Length();
                    if (data->regEx)
                    {
                        // set target same as selection
                        control->SetTargetStart(control->GetSelectionStart());
                        control->SetTargetEnd(control->GetSelectionEnd());
                        if (advRegex)
                        {
                            wxString text=control->GetSelectedText();
                            re.Replace(&text,data->replaceText,1);
                            lengthReplace=text.Len();
                            control->ReplaceSelection(text);
                        }
                        else // replace with regEx support
                            lengthReplace = control->ReplaceTargetRE(data->replaceText);

                        // reset target
                        control->SetTargetStart(0);
                        control->SetTargetEnd(0);
                    }
                    else
                        control->ReplaceSelection(data->replaceText);

                    data->start += lengthReplace;

                    // adjust end pos by adding the length difference
                    // between find and replace strings
                    int diff = lengthReplace - lengthFound;
                    if (data->directionDown)
                        data->end += diff;
                    else
                        data->end -= diff;
                }
                else
                {
                    if (data->directionDown)
                        data->start += lengthFound;
                    else
                        data->start -= lengthFound;
                }
            }// if
        }// while

        control->EndUndoAction(); // undo

        // If opened the file and no replacement was made, close the editor
        if (!replacementsWereMade && fileWasNotOpen)
            editorMgr->Close(ed, true);
    }// for

    if (read_only_files_skipped)
    {
        wxString msg;
        msg.Printf(_("Skipped %d read-only file(s)."), read_only_files_skipped);
        InfoWindow::Display(_("Warning"), msg);
    }

    // if we showed the progress, the app window is frozen; unfreeze it
    if (progress)
        Manager::Get()->GetAppWindow()->Thaw();

    delete progress;
    AnnoyingDialog dlg(_("Replace in files"),
                       _("Replace in files has finished all operations."),
                       wxART_INFORMATION, AnnoyingDialog::OK);
    dlg.ShowModal();

    return pos;
}

int FindReplace::Find(cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control || !data)
        return -1;

    bool advRegex = false;
    bool advRegexNewLinePolicy =! data->IsMultiLine();
    int flags = 0;
    data->ConvertEOLs(control->GetEOLMode());
    CalculateFindReplaceStartEnd(control, data);

    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
    {
        flags |= wxSCI_FIND_REGEXP;
        if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_posix_style_regexes"), false))
            flags |= wxSCI_FIND_POSIX;
        #ifdef wxHAS_REGEX_ADVANCED
        advRegex = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_advanced_regexes"), false);
        #endif
    }

    wxRegEx re;
    #ifdef wxHAS_REGEX_ADVANCED
    if (advRegex)
    {
        if (data->matchCase)
            re.Compile(data->findText,wxRE_ADVANCED|(wxRE_NEWLINE*advRegexNewLinePolicy));
        else
            re.Compile(data->findText,wxRE_ADVANCED|(wxRE_NEWLINE*advRegexNewLinePolicy)|wxRE_ICASE);
    }
    #endif

    int pos = -1;
    // avoid infinite loop when wrapping search around, eventually crashing WinLogon O.O
    bool wrapAround = false;
    int StartPos = 0;
    int EndPos = control->GetLength();
    if (data->scope == 1) // selectd text
    {
        StartPos = data->SearchInSelectionStart;
        EndPos = data->SearchInSelectionEnd;
    }
    bool wrapAroundNotification = false;
    while (true) // loop while not found and user selects to start again from the top
    {
        int lengthFound = 0;
        if (!advRegex)
            pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
        else
        {
            wxString text = control->GetTextRange(data->start, data->end);
            if (re.Matches(text))
            {
                size_t start, len;
                re.GetMatch(&start, &len, 0);
                pos = start + data->start;
                lengthFound = len;
                if ((start==0) && (len==0)) //For searches for "^" or "$" (and null returning variants on this) need to make sure we have forward progress and not simply matching on a previous BOL/EOL find
                {
                    text = text.Mid(1);
                    if (re.Matches(text))
                    {
                        re.GetMatch(&start, &len, 0);
                        pos = start + data->start + 1;
                        lengthFound = len;
                    }
                    else
                        pos=-1;
                }
            }
            else
                pos=-1;
        }
        if (pos != -1 && data->start!=data->end)
        {
            int line = control->LineFromPosition(pos);
            int onScreen = control->LinesOnScreen() >> 1;
            int l1 = line - onScreen;
            int l2 = line + onScreen;
            for (int l=l1; l<=l2; l+=2)     // unfold visible lines on screen
                control->EnsureVisible(l);
            control->GotoLine(l1);          // center selection on screen
            control->GotoLine(l2);
            control->GotoLine(line);
            control->SetSelectionVoid(pos, pos + lengthFound);
//            Manager::Get()->GetLogManager()->DebugLog("pos=%d, selLen=%d, length=%d", pos, data->end - data->start, lengthFound);
            data->start = pos;
            break; // done
        }
        else if (!wrapAround && !data->findInFiles) // for "find in files" we don't want to show messages
        {
            if (   (data->directionDown && data->start != StartPos)
                || (!data->directionDown && data->start != EndPos) )
            {
                wxString msg;
                if (!data->scope == 1) // selected text
                {
                    if (data->directionDown)
                        msg = _("Text not found.\nSearch from the start of the document?");
                    else
                        msg = _("Text not found.\nSearch from the end of the document?");
                }
                else
                {
                    if (data->directionDown)
                        msg = _("Text not found.\nSearch from the start of the selection?");
                    else
                        msg = _("Text not found.\nSearch from the end of the selection?");
                }

                bool auto_wrap_around = data->autoWrapSearch;
                if (auto_wrap_around)
                    wrapAroundNotification = true;

                if (auto_wrap_around || cbMessageBox(msg, _("Result"), wxOK | wxCANCEL | wxICON_QUESTION) == wxID_OK)
                {
                    wrapAround = true; // signal the wrap-around
                    if (!data->scope == 1) // selected text
                    {
                        if (data->directionDown)
                        {
                            data->start = 0;
                            data->end = control->GetLength();
                        }
                        else
                        {
                            data->start = control->GetLength();
                            data->end = 0;
                        }
                    }
                    else // global
                    {
                        if (data->directionDown)
                        {
                            data->start = data->SearchInSelectionStart;
                            data->end = data->SearchInSelectionEnd;
                        }
                        else
                        {
                            data->start = data->SearchInSelectionEnd;
                            data->end = data->SearchInSelectionStart;
                        }
                    }
                }
                else
                    break; // done
            }
            else
            {
                wxString msg;
                msg.Printf(_("Not found: %s"), data->findText.c_str());
                cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
                control->SetSCIFocus(true);
                wrapAroundNotification = false;
                break; // done
            }
        }
        else if (wrapAround)
        {
            wxString msg;
            msg.Printf(_("Not found: %s"), data->findText.c_str());
            cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
            wrapAroundNotification = false;
            break; // done
        }
        else
            break; // done
    }

    if (wrapAroundNotification)
    {
        wxBell();
        InfoWindow::Display(_("Find action"), _("Reached the end of the document"), 1000);
    }

    return pos;
}

int FindReplace::FindInFiles(cbFindReplaceData* data)
{
    if (!data || data->findText.IsEmpty())
        return 0;

    // clear old search results
    if ( data->delOldSearches )
    {
        m_pSearchLog->Clear();
    }
    int oldcount = m_pSearchLog->GetItemsCount();

    EditorManager *editorMgr = Manager::Get()->GetEditorManager();

    // let's make a list of all the files to search in
    wxArrayString filesList;

    if (data->scope == 0) // find in open files
    {
        // fill the search list with the open files
        for (int i = 0; i < editorMgr->GetEditorsCount(); ++i)
        {
            cbEditor* ed = editorMgr->GetBuiltinEditor(i);
            if (ed)
                filesList.Add(ed->GetFilename());
        }
    }
    else if (data->scope == 1) // find in project files
    {
        // fill the search list with all the project files
        if(data->searchProject<0)
        {
            cbMessageBox(_("No project to search in!"), _("Error"), wxICON_WARNING);
            return 0;
        }
        cbProject* prj = (*Manager::Get()->GetProjectManager()->GetProjects())[data->searchProject];
        wxString target;
        wxString fullpath = _T("");
        if(data->searchTarget >= 0)
            target = prj->GetBuildTarget(data->searchTarget)->GetTitle();
        for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (pf)
            {
                if(target!=wxEmptyString && pf->buildTargets.Index(target)<0)
                    continue;
                fullpath = pf->file.GetFullPath();
                if (filesList.Index(fullpath) >= 0) // avoid adding duplicates
                    continue;
                if (wxFileExists(fullpath))  // Does the file exist?
                    filesList.Add(fullpath);
            }
        }
    }
    else if (data->scope == 2) // find in workspace
    {
        // loop over all the projects in the workspace (they are contained in the ProjectManager)
        const ProjectsArray* pProjects = Manager::Get()->GetProjectManager()->GetProjects();
        int count = 0;
        if (pProjects)
            count = pProjects->GetCount();
        if (!count)
        {
            cbMessageBox(_("No workspace to search in!"), _("Error"), wxICON_WARNING);
            return 0;
        }
        for (int idxProject = 0; idxProject < count; ++idxProject)
        {
            cbProject* pProject = pProjects->Item(idxProject);
            if (pProject)
            {
                wxString fullpath = _T("");
                for (FilesList::iterator it = pProject->GetFilesList().begin(); it != pProject->GetFilesList().end(); ++it)
                {
                    ProjectFile* pf = *it;
                    if (pf)
                    {
                        fullpath = pf->file.GetFullPath();
                        if (filesList.Index(fullpath) == -1) // avoid adding duplicates
                        {
                            if (wxFileExists(fullpath))  // Does the file exist?
                                filesList.Add(fullpath);
                        }
                    }
                } // end for : idx : idxFile
            }
        } // end for : idx : idxProject
    }
    else if (data->scope == 3) // find in custom search path and mask
    {
        // fill the search list with the files found under the search path
        int flags = wxDIR_FILES |
                    (data->recursiveSearch ? wxDIR_DIRS : 0) |
                    (data->hiddenSearch ? wxDIR_HIDDEN : 0);
        wxArrayString masks = GetArrayFromString(data->searchMask);
        if (!masks.GetCount())
            masks.Add(_T("*"));
        unsigned int count = masks.GetCount();

        for (unsigned int i = 0; i < count; ++i)
        {
            // wxDir::GetAllFiles() does *not* clear the array, so it suits us just fine ;)
            wxDir::GetAllFiles(data->searchPath, &filesList, masks[i], flags);
        }
    }
    else if (data->scope == 4) // find in current file only
    {
        cbEditor* ed = editorMgr->GetBuiltinActiveEditor();
        if (ed)
            filesList.Add(ed->GetFilename());
    }


    // if the list is empty, leave
    if (filesList.GetCount() == 0)
    {
        cbMessageBox(_("No files to search in!"), _("Error"), wxICON_WARNING);
        return 0;
    }

    // sort search results alphabetically if option is on
    if (m_LastFindReplaceData->sortSearchResult)
        filesList.Sort();

    // now that list is filled, we'll search
    // but first we'll create a hidden cbStyledTextCtrl to do the search for us ;)
    cbStyledTextCtrl* control = new cbStyledTextCtrl(editorMgr->GetNotebook(), -1, wxDefaultPosition, wxSize(0, 0));
    control->Show(false); //hidden

    // let's create a progress dialog because it might take some time depending on the files count
    wxProgressDialog* progress = new wxProgressDialog(_("Find in files"),
                                 _("Please wait while searching inside the files..."),
                                 filesList.GetCount(),
                                 Manager::Get()->GetAppWindow(),
                                 wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);

    PlaceWindow(progress);

    // keep a copy of the find struct
    cbFindReplaceData localData = *data;

    if ( !data->delOldSearches )
    {
        LogSearch(_T("=========="), -1, _T("=== \"") + data->findText + _T("\" ==="));
        oldcount++;
    }

    int lastline = -1;
    int count = 0;
    for (size_t i = 0; i < filesList.GetCount(); ++i)
    {
        // update the progress bar
        if (!progress->Update(i))
            break; // user pressed "Cancel"

        // re-initialize the find struct for every file searched
        *data = localData;

        // check if the file is already opened in built-in editor and do search in it
        cbEditor* ed = editorMgr->IsBuiltinOpen(filesList[i]);
        if (ed)
            control->SetText(ed->GetControl()->GetText());
        else // else load the file in the control
        {
            EncodingDetector detector(filesList[i]);
            if (!detector.IsOK())
            {
                continue; // failed
            }
            control->SetText(detector.GetWxStr());
        }

        // now search for first occurence
        if (Find(control, data) == -1)
        {
            lastline = -1;
            continue;
        }

        int line = control->LineFromPosition(control->GetSelectionStart());
        lastline = line;

        // make the filename relative
        wxString filename = filesList[i];
        if (filename.StartsWith(data->searchPath))
        {
            wxFileName fname(filename);
            fname.MakeRelativeTo(data->searchPath);
            filename = fname.GetFullPath();
        }

        // log it
        LogSearch(filename, line + 1, control->GetLine(line));
        ++count;

        // now loop finding the next occurence
        while ( FindNext(true, control, data) != -1 )
        {
            if (data->startFile) // we already found it, so break and avoid possible infinite loop
                break;
//            // log it
            line = control->LineFromPosition(control->GetSelectionStart());
            if (line == lastline)  // avoid multiple hits on the same line (try search for "manager")
                continue;

            lastline = line;
            LogSearch(filename, line + 1, control->GetLine(line));
            ++count;
        }
    }
    delete control; // done with it
    delete progress; // done here too

    if (count > 0)
    {
        m_pSearchLog->SetBasePath(data->searchPath);
        if (Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_show_search"), true))
        {
            CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_pSearchLog);
            CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);

            Manager::Get()->ProcessEvent(evtSwitch);
            Manager::Get()->ProcessEvent(evtShow);
        }
        m_pSearchLog->FocusEntry(oldcount);
    }
    else
    {
        wxString msg;
        if ( data->delOldSearches )
        {
            msg.Printf(_("Not found: %s"), data->findText.c_str());
            cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
            cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if (ed)
                ed->GetControl()->SetSCIFocus(true);
        }
        else
        {
            msg.Printf(_("not found in %lu files"), static_cast<unsigned long>(filesList.GetCount()));
            LogSearch(_T(""), -1, msg );
            m_pSearchLog->FocusEntry(oldcount);
        }
    }

    return count;
}

int FindReplace::FindNext(bool goingDown, cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control)
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
            control = ed->GetControl();
    }

    if (!control)
        return -1;

    if (!data)
    {
        data = m_LastFindReplaceData;
        // FindNext/Previous called from Search menu (F3/Shift-F3)
        if (data) data->findInFiles = false;
    }

    if (!data)
        return ShowFindDialog(false, false);

    if (!data->findInFiles)
    {
        wxString phraseAtCursor = control->GetSelectedText();

        if ( not data->findUsesSelectedText )
        {   // The mandrav find behavior
            // change findText to selected text (if any text is selected and no search text was set before)
            if (!phraseAtCursor.IsEmpty() && data->findText.IsEmpty())
                data->findText = phraseAtCursor;
        }
        else
        {   // The tiwag find behavior
            // change findText to selected text (if any text is selected)
            if (!phraseAtCursor.IsEmpty())
            {
                data->findText = phraseAtCursor;
                data->originEntireScope = false;  //search from cursor
                data->scope = 0; // global ("selected text" is useful only from Find Dialog)
            }
        }
    }

    data->directionDown = goingDown;
    return Find(control, data);
}
