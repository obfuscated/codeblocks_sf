#ifndef STARTHEREPAGE_H
#define STARTHEREPAGE_H

#ifndef CB_PRECOMP
    #include <editorbase.h>
#endif

extern const wxString g_StartHereTitle;

class wxHtmlWindow;
class wxHtmlLinkInfo;

class StartHerePage : public EditorBase
{
	wxString revInfo;

	public:
		StartHerePage(wxEvtHandler* owner, wxWindow* parent);
		virtual ~StartHerePage();

		bool LinkClicked(const wxHtmlLinkInfo& link);
		void SetPageContent(const wxString& buffer); // set the HTML content
        virtual bool VisibleToTree() const { return false; }
        void Reload();
	protected:

        wxEvtHandler* m_pOwner;
        wxHtmlWindow* m_pWin;
        wxString m_OriginalPageContent;
	private:
        DECLARE_EVENT_TABLE()
};

#endif // STARTHEREPAGE_H
