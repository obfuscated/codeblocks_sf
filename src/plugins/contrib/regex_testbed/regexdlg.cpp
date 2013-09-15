/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#include "regexdlg.h"
#include <wx/regex.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <manager.h>
    #include <configmanager.h>
#endif

//(*InternalHeaders(RegExDlg)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(RegExDlg)
//*)

BEGIN_EVENT_TABLE(RegExDlg,wxScrollingDialog)
	EVT_INIT_DIALOG(RegExDlg::OnInit)
	EVT_UPDATE_UI(-1, RegExDlg::OnUpdateUI)
END_EVENT_TABLE()

RegExDlg::VisibleDialogs RegExDlg::m_visible_dialogs;

RegExDlg::RegExDlg(wxWindow* parent,wxWindowID /*id*/)
{
    //(*Initialize(RegExDlg)
    wxXmlResource::Get()->LoadObject(this,parent,_T("RegExDlg"),_T("wxScrollingDialog"));
    m_regex = (wxTextCtrl*)FindWindow(XRCID("ID_REGEX"));
    m_quoted = (wxTextCtrl*)FindWindow(XRCID("ID_QUOTED"));
    m_library = (wxChoice*)FindWindow(XRCID("ID_LIBRARY"));
    m_nocase = (wxCheckBox*)FindWindow(XRCID("ID_NOCASE"));
    m_newlines = (wxCheckBox*)FindWindow(XRCID("ID_NEWLINES"));
    m_text = (wxTextCtrl*)FindWindow(XRCID("ID_TEXT"));
    m_output = (wxHtmlWindow*)FindWindow(XRCID("ID_OUT"));

    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&RegExDlg::OnClose);
    //*)

    assert(m_regex);
    assert(m_quoted);
    assert(m_library);
    assert(m_nocase);
    assert(m_newlines);
    assert(m_text);
    assert(m_output);

    m_text->MoveAfterInTabOrder(m_quoted);

    m_library->SetSelection(0);
    m_output->SetBorders(0);
    m_quoted->SetEditable(false);

    m_visible_dialogs.insert(this);
}

RegExDlg::~RegExDlg()
{
}

void RegExDlg::OnClose(wxCloseEvent& /*event*/)
{
    VisibleDialogs::iterator it = m_visible_dialogs.find(this);
    if (it != m_visible_dialogs.end())
    {
        delete *it;
        m_visible_dialogs.erase(it);
    }
}


void RegExDlg::ReleaseAll()
{
    for(VisibleDialogs::iterator it = m_visible_dialogs.begin(); it != m_visible_dialogs.end(); ++it)
        delete *it;
    m_visible_dialogs.clear();
}

namespace
{
/**
    @brief Makes the input string to be valid html string (replaces <,>,&," with &lt;,&gt;,&amp;,&quot; respectively)
    @param [inout] s - string that will be escaped
*/
void cbEscapeHtml(wxString &s)
{
    s.Replace(wxT("&"), wxT("&amp;"));
    s.Replace(wxT("<"), wxT("&lt;"));
    s.Replace(wxT(">"), wxT("&gt;"));
    s.Replace(wxT("\""), wxT("&quot;"));
}
}

void RegExDlg::OnUpdateUI(wxUpdateUIEvent& /*event*/)
{
    static wxString regex;
    static wxString text;
    static bool nocase;
    static bool newlines;
    static int library;

//    if (event.GetId() == XRCID("ID_NOCASE") || event.GetId() == XRCID("ID_NEWLINES"))
//        regex = _T("$^"); // bullshit
//    all UI elements send events quite often (on linux on every mouse move, if the parent window
//    has the focus, on windows even without any user action). So we can not use the event Id to force a new
//    run of GetBuiltinMatches(), because every time the value of m_quoted and m_output gets upadeted a selection of text in m_quoted
//    will be reset and therefore the user can not copy it's content (linux) and m_output jumps to the top, so that the user
//    cannot scroll the text (windows and linux).
//

    if ( regex == m_regex->GetValue() &&
        text == m_text->GetValue() &&
        nocase == m_nocase->GetValue() &&
        newlines == m_newlines->GetValue() &&
        library == m_library->GetSelection())
        {
            return;
        }

    regex = m_regex->GetValue();
    text = m_text->GetValue();
    nocase = m_nocase->GetValue();
    newlines = m_newlines->GetValue();
    library = m_library->GetSelection();

    wxString tmp(regex);

    tmp.Replace(_T("\\"), _T("\\\\"));
    tmp.Replace(_T("\""), _T("\\\""));
    m_quoted->SetValue(tmp);

    wxArrayString as = GetBuiltinMatches(text);

    if (as.IsEmpty())
    {
        m_output->SetPage(_T("<html><center><b>no matches</b></center></html>"));
        return;
    }

    wxString s(_T("<html width='100%'><center><b>matches:</b><br><br><font size=-1><table width='100%' border='1' cellspacing='2'>"));

    for(size_t i = 0; i < as.GetCount(); ++i)
    {
        cbEscapeHtml(as[i]);
        tmp.Printf(_T("<tr><td width=35><b>%lu</b></td><td>%s</td></tr>"), static_cast<unsigned long>(i), as[i].wx_str());
        s.append(tmp);
    }
    s.append(_T("</table></font></html>"));

    m_output->SetPage(s);
}


void RegExDlg::RunBenchmark(wxCommandEvent& /*event*/)
{
}


void RegExDlg::EndModal(int retCode)
{
    wxScrollingDialog::EndModal(retCode);
}

void RegExDlg::OnInit(wxInitDialogEvent& /*event*/)
{
}

void RegExDlg::OnRegExItemActivated(wxListEvent& /*event*/)
{
    //
}


wxArrayString RegExDlg::GetBuiltinMatches(const wxString& text)
{
    wxArrayString ret;

    int flags = m_library->GetSelection();

    if (text.IsEmpty() || flags > 2) // should not be
        return ret;

    flags |= m_newlines->IsChecked() ? wxRE_NEWLINE : 0;
    flags |= m_nocase->IsChecked() ? wxRE_ICASE : 0;

    if (m_wxre.Compile(m_regex->GetValue(), flags))
    {
        m_regex->SetForegroundColour(wxNullColour);
        m_regex->SetBackgroundColour(wxNullColour);
        m_regex->GetParent()->Refresh();
        if (!m_wxre.Matches(text))
            return ret;
    }
    else
    {
        m_regex->SetForegroundColour(*wxWHITE);
        m_regex->SetBackgroundColour(*wxRED);
        m_regex->GetParent()->Refresh();
        return ret;
    }

    for(size_t i = 0; i < m_wxre.GetMatchCount(); ++i)
        if (!m_wxre.GetMatch(text, i).IsEmpty())
            ret.Add(m_wxre.GetMatch(text, i));

    return ret;
}

wxArrayString RegExDlg::GetPregMatches(const wxString& /*text*/)
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



