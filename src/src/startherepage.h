#ifndef STARTHEREPAGE_H
#define STARTHEREPAGE_H

#include "editorbase.h"

extern wxString g_StartHereTitle;

class MainFrame;
class wxHtmlWindow;
class wxHtmlLinkInfo;

class StartHerePage : public EditorBase
{
	public:
		StartHerePage(MainFrame* owner, wxWindow* parent);
		virtual ~StartHerePage();
		
		bool LinkClicked(const wxHtmlLinkInfo& link);
	protected:
        MainFrame* m_pOwner;
        wxHtmlWindow* m_pWin;
	private:
        DECLARE_EVENT_TABLE()
};

#endif // STARTHEREPAGE_H
