#ifndef _CODEBLOCKS_SRC_FIND_REPLACE_H_
#define _CODEBLOCKS_SRC_FIND_REPLACE_H_

struct cbFindReplaceData;
class cbStyledTextCtrl;
class wxString;
class cbSearchResultsLog;

class FindReplace
{
    public:
        FindReplace();
        ~FindReplace();

        void CreateSearchLog();

        int ShowFindDialog(bool replace,  bool explicitly_find_in_files = false);
        int FindNext(bool goingDown, cbStyledTextCtrl* control, cbFindReplaceData* data, bool selected);
        int FindSelectedText(bool goingDown);

    private:
        void LogSearch(const wxString& file, int line, const wxString& lineText);

        void CalculateFindReplaceStartEnd(cbStyledTextCtrl* control, cbFindReplaceData* data, bool replace = false);
        int Find(cbStyledTextCtrl* control, cbFindReplaceData* data);
        int FindInFiles(cbFindReplaceData* data);
        int Replace(cbStyledTextCtrl* control, cbFindReplaceData* data);
        int ReplaceInFiles(cbFindReplaceData* data);

    private:
        cbFindReplaceData   *m_LastFindReplaceData;
        cbSearchResultsLog  *m_pSearchLog;
};

#endif // _CODEBLOCKS_SRC_FIND_REPLACE_H_

