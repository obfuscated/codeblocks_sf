// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
#include <wx/wxscintilla.h>  // scintilla edit control

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/sizer.h>
    #include <wx/statbox.h>
    #include <wx/stattext.h>
#endif
#include "scrollingdialog.h"
#include "editproperties.h"
#include "scbeditor.h"
#include "cbstyledtextctrl.h"
#include "seditorcolourset.h"
#include "snippetsconfig.h"

//----------------------------------------------------------------------------
// EditProperties
//----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
EditProperties::EditProperties (wxWindow* pParent, ScbEditor* pEditor, long style)
// ----------------------------------------------------------------------------
        : wxScrollingDialog (pParent, -1, wxEmptyString,
                    wxDefaultPosition, wxDefaultSize,
                    style | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    InitEditProperties( pParent, pEditor, style);
}
// ----------------------------------------------------------------------------
void EditProperties::InitEditProperties (wxWindow* pParent, ScbEditor* pEditor, long style)
// ----------------------------------------------------------------------------
{
    cbStyledTextCtrl* pSTC = pEditor->GetControl();

    // sets the application title
    SetTitle (_("Properties"));
    wxString text;

    // fullname
    wxBoxSizer *fullname = new wxBoxSizer (wxHORIZONTAL);
    fullname->Add (10, 0);
    fullname->Add (new wxStaticText (this, -1, _("Full filename"),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    fullname->Add (new wxStaticText (this, -1, pEditor->GetFilename()),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

    // text info
    wxGridSizer *textinfo = new wxGridSizer (4, 0, 2);
    textinfo->Add (new wxStaticText (this, -1, _("Language"),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    //-textinfo->Add (new wxStaticText (this, -1, pEditor->m_language->name),
    textinfo->Add (new wxStaticText(this, -1, pEditor->GetColourSet()->GetLanguageName(pEditor->GetLanguage())),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    textinfo->Add (new wxStaticText (this, -1, _("Lexer-ID: "),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), pEditor->GetControl()->GetLexer());
    textinfo->Add (new wxStaticText (this, -1, text),
                   0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    wxString EOLtype = _T("");
    switch (pEditor->GetControl()->GetEOLMode()) {
        case wxSCI_EOL_CR: {EOLtype = _T("CR (Unix)"); break; }
        case wxSCI_EOL_CRLF: {EOLtype = _T("CRLF (Windows)"); break; }
        case wxSCI_EOL_LF: {EOLtype = _T("CR (Macintosh)"); break; }
    }
    textinfo->Add (new wxStaticText (this, -1, _("Line endings"),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    textinfo->Add (new wxStaticText (this, -1, EOLtype),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);

    // text info box
    wxStaticBoxSizer *textinfos = new wxStaticBoxSizer (
                     new wxStaticBox (this, -1, _("Informations")),
                     wxVERTICAL);
    textinfos->Add (textinfo, 0, wxEXPAND);
    textinfos->Add (0, 6);

    // statistic
    wxGridSizer *statistic = new wxGridSizer (4, 0, 2);
    statistic->Add (new wxStaticText (this, -1, _("Total lines"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), pSTC->GetLineCount());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, -1, _("Total chars"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), pSTC->GetTextLength());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, -1, _("Current line"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), pSTC->GetCurrentLine());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, -1, _("Current pos"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), pSTC->GetCurrentPos());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);

    // char/line statistics
    wxStaticBoxSizer *statistics = new wxStaticBoxSizer (
                     new wxStaticBox (this, -1, _("Statistics")),
                     wxVERTICAL);
    statistics->Add (statistic, 0, wxEXPAND);
    statistics->Add (0, 6);

    // total pane
    wxBoxSizer *totalpane = new wxBoxSizer (wxVERTICAL);
    totalpane->Add (fullname, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    totalpane->Add (0, 6);
    totalpane->Add (textinfos, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    totalpane->Add (0, 10);
    totalpane->Add (statistics, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    totalpane->Add (0, 6);
    wxButton *okButton = new wxButton (this, wxID_OK, _("OK"));
    okButton->SetDefault();
    totalpane->Add (okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit (totalpane);
    GetConfig()->CenterChildOnParent(this);
    ShowModal();
}
