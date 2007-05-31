#include <sdk.h>

#include "regexdlg.h"
#include <wx/regex.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <manager.h>
    #include <configmanager.h>
#endif

//(*InternalHeaders(regex_dialog)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(regex_dialog)
//*)

BEGIN_EVENT_TABLE(RegExDlg,wxDialog)
	EVT_INIT_DIALOG(RegExDlg::OnInit)
	EVT_UPDATE_UI(-1, RegExDlg::OnUpdateUI)
END_EVENT_TABLE()

RegExDlg::RegExDlg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(regex_dialog)
    wxXmlResource::Get()->LoadObject(this,parent,_T("regex_dialog"),_T("wxDialog"));
    m_regex = (wxTextCtrl*)FindWindow(XRCID("ID_REGEX"));
    m_quoted = (wxTextCtrl*)FindWindow(XRCID("ID_QUOTED"));
    m_library = (wxChoice*)FindWindow(XRCID("ID_LIBRARY"));
    m_nocase = (wxCheckBox*)FindWindow(XRCID("ID_NOCASE"));
    m_newlines = (wxCheckBox*)FindWindow(XRCID("ID_NEWLINES"));
    m_text = (wxTextCtrl*)FindWindow(XRCID("ID_TEXT"));
    m_output = (wxHtmlWindow*)FindWindow(XRCID("ID_OUT"));
    //*)

    assert(m_regex);
    assert(m_quoted);
    assert(m_library);
    assert(m_nocase);
    assert(m_newlines);
    assert(m_text);
    assert(m_output);

    m_library->SetSelection(0);
    m_output->SetBorders(0);
    m_quoted->SetEditable(false);
}

RegExDlg::~RegExDlg()
{
}


void RegExDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    static wxString regex;
    static wxString text;

    if(event.GetId() == XRCID("ID_NOCASE") || event.GetId() == XRCID("ID_NEWLINES"))
        regex = _T("$^"); // bullshit

    if(regex == m_regex->GetValue() && text == m_text->GetValue())
        return;

    regex = m_regex->GetValue();
    text = m_text->GetValue();

    wxString tmp(regex);

    tmp.Replace(_T("\\"), _T("\\\\"));
    tmp.Replace(_T("\""), _T("\\\""));
    m_quoted->SetValue(tmp);

    wxArrayString as = GetBuiltinMatches(text);

    if(as.IsEmpty())
        {
        m_output->SetPage(_T("<html><center><b>no matches</b></center></html>"));
        return;
        }

    wxString s(_T("<html width='100%'><center><b>matches:</b><br><br><font size=-1><table width='100%' border='1' cellspacing='2'>"));

    for(size_t i = 0; i < as.GetCount(); ++i)
    {
        tmp.Printf(_T("<tr><td width=35><b>%d</b></td><td>%s</td></tr>"), i, as[i].c_str());
        s.append(tmp);
    }
    s.append(_T("</table></font></html>"));

    m_output->SetPage(s);
}


void RegExDlg::RunBenchmark(wxCommandEvent& event)
{
}


void RegExDlg::EndModal(int retCode)
{
    wxDialog::EndModal(retCode);
}

void RegExDlg::OnInit(wxInitDialogEvent& event)
{
}

void RegExDlg::OnRegExItemActivated(wxListEvent& event)
{
    //
}


wxArrayString RegExDlg::GetBuiltinMatches(const wxString& text)
{
    wxLogNull fuckThis;

    wxArrayString ret;

    int flags = m_library->GetSelection();

    if(text.IsEmpty() || flags > 2) // should not be
        return ret;

    flags |= m_newlines->IsChecked() ? wxRE_NEWLINE : 0;
    flags |= m_nocase->IsChecked() ? wxRE_ICASE : 0;

    if(!m_wxre.Compile(m_regex->GetValue(), flags) || !m_wxre.Matches(text))
        return ret;

    for(size_t i = 0; i < m_wxre.GetMatchCount(); ++i)
        if(!!m_wxre.GetMatch(text, i))
            ret.Add(m_wxre.GetMatch(text, i));

    return ret;
}

wxArrayString RegExDlg::GetPregMatches(const wxString& text)
{
    wxArrayString ret;

//    const char *error;
//    int erroffset;
//    int flags = 0;
//    flags |= m_nocase->IsChecked() ? PCRE_CASELESS : 0;
//    flags |= m_newlines->IsChecked() ? PCRE_DOTALL : 0;
//    pcre *reg = pcre_compile(text.mb_str(), flags, &error, &erroffset, 0);

    return ret;
}



