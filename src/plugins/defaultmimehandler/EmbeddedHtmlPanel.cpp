/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#include "EmbeddedHtmlPanel.h"

#ifndef CB_PRECOMP
	//(*InternalHeadersPCH(EmbeddedHtmlPanel)
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/bmpbuttn.h>
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(EmbeddedHtmlPanel)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/html/htmlwin.h>
//*)

//(*IdInit(EmbeddedHtmlPanel)
const long EmbeddedHtmlPanel::ID_BITMAPBUTTON2 = wxNewId();
const long EmbeddedHtmlPanel::ID_BITMAPBUTTON3 = wxNewId();
const long EmbeddedHtmlPanel::ID_STATICTEXT1 = wxNewId();
const long EmbeddedHtmlPanel::ID_PANEL1 = wxNewId();
const long EmbeddedHtmlPanel::ID_HTMLWINDOW1 = wxNewId();
//*)

// (shamelessly stolen from help plugin)
// build all HTML font sizes (1..7) from the given base size
void wxBuildFontSizes(int *sizes, int size)
{
    // using a fixed factor (1.2, from CSS2) is a bad idea as explained at
    // http://www.w3.org/TR/CSS21/fonts.html#font-size-props but this is by far
    // simplest thing to do so still do it like this for now
    sizes[0] = int(size * 0.75); // exception to 1.2 rule, otherwise too small
    sizes[1] = int(size * 0.83);
    sizes[2] = size;
    sizes[3] = int(size * 1.2);
    sizes[4] = int(size * 1.44);
    sizes[5] = int(size * 1.73);
    sizes[6] = int(size * 2);
}

// (shamelessly stolen from help plugin)
int wxGetDefaultHTMLFontSize()
{
    // base the default font size on the size of the default system font but
    // also ensure that we have a font of reasonable size, otherwise small HTML
    // fonts are unreadable
    int size = wxNORMAL_FONT->GetPointSize();
    if ( size < 10 )
        size = 10;

    return size;
}

BEGIN_EVENT_TABLE(EmbeddedHtmlPanel,wxPanel)
	//(*EventTable(EmbeddedHtmlPanel)
	//*)
	EVT_UPDATE_UI(-1, EmbeddedHtmlPanel::OnUpdateUI)
    EVT_HTML_LINK_CLICKED(ID_HTMLWINDOW1, EmbeddedHtmlPanel::OnLinkClicked)
END_EVENT_TABLE()

EmbeddedHtmlPanel::EmbeddedHtmlPanel(wxWindow* parent)
{
	//(*Initialize(EmbeddedHtmlPanel)
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	btnBack = new wxBitmapButton(Panel1, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
	BoxSizer2->Add(btnBack, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	btnForward = new wxBitmapButton(Panel1, ID_BITMAPBUTTON3, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_FORWARD")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
	BoxSizer2->Add(btnForward, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	lblStatus = new wxStaticText(Panel1, ID_STATICTEXT1, _("Label"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(lblStatus, 1, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	Panel1->SetSizer(BoxSizer2);
	BoxSizer2->Fit(Panel1);
	BoxSizer2->SetSizeHints(Panel1);
	BoxSizer1->Add(Panel1, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	winHtml = new wxHtmlWindow(this, ID_HTMLWINDOW1, wxDefaultPosition, wxSize(340,180), wxHW_SCROLLBAR_AUTO, _T("ID_HTMLWINDOW1"));
	BoxSizer1->Add(winHtml, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&EmbeddedHtmlPanel::OnbtnBackClick);
	Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&EmbeddedHtmlPanel::OnbtnForwardClick);
	//*)

	int sizes[7] = {};
    wxBuildFontSizes(sizes, wxGetDefaultHTMLFontSize());
	winHtml->SetFonts(wxEmptyString, wxEmptyString, &sizes[0]);

	lblStatus->SetLabel(wxEmptyString);
}

EmbeddedHtmlPanel::~EmbeddedHtmlPanel()
{
	//(*Destroy(EmbeddedHtmlPanel)
	//*)
}

void EmbeddedHtmlPanel::OnLinkClicked(wxHtmlLinkEvent &event)
{
    Open(event.GetLinkInfo().GetHref());
}

void EmbeddedHtmlPanel::OnUpdateUI(wxUpdateUIEvent& event)
{
    btnBack->Enable(winHtml->HistoryCanBack());
    btnForward->Enable(winHtml->HistoryCanForward());

    event.Skip();
}

void EmbeddedHtmlPanel::OnbtnBackClick(wxCommandEvent& event)
{
    if (winHtml->HistoryCanBack())
    {
        lblStatus->SetLabel(_("Going back..."));
        Update();

        winHtml->HistoryBack();

        lblStatus->SetLabel(_("Ready"));
    }
}

void EmbeddedHtmlPanel::OnbtnForwardClick(wxCommandEvent& event)
{
    if (winHtml->HistoryCanForward())
    {
        lblStatus->SetLabel(_("Going forward..."));
        Update();

        winHtml->HistoryForward();

        lblStatus->SetLabel(_("Ready"));
    }
}

void EmbeddedHtmlPanel::Open(const wxString& url)
{
    lblStatus->SetLabel(_("Opening ") + url);
    Update();

    winHtml->LoadPage(url);

    lblStatus->SetLabel(_("Ready"));
}
