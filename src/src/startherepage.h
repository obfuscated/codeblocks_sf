/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef STARTHEREPAGE_H
#define STARTHEREPAGE_H

#ifndef CB_PRECOMP
    #include <editorbase.h>
#endif

extern const wxString g_StartHereTitle;

class wxHtmlWindow;
class wxHtmlLinkInfo;
class wxFileHistory;

class StartHerePage : public EditorBase
{
    wxString revInfo;

    public:
        StartHerePage(wxEvtHandler* owner, const wxFileHistory *projects,
                      const wxFileHistory *files, wxWindow* parent);
        virtual ~StartHerePage();

        bool LinkClicked(const wxHtmlLinkInfo& link);
        virtual bool VisibleToTree() const { return false; }
        void Reload();
    protected:

        wxEvtHandler* m_pOwner;
        wxHtmlWindow* m_pWin;
        wxString m_OriginalPageContent;
        const wxFileHistory *m_projects;
        const wxFileHistory *m_files;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // STARTHEREPAGE_H
