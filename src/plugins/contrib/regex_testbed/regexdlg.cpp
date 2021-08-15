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
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(RegExDlg)
//*)

BEGIN_EVENT_TABLE(RegExDlg,wxScrollingDialog)
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

    Connect(XRCID("ID_REGEX"),wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&RegExDlg::OnValueChanged);
    Connect(XRCID("ID_LIBRARY"),wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&RegExDlg::OnValueChanged);
    Connect(XRCID("ID_NOCASE"),wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&RegExDlg::OnValueChanged);
    Connect(XRCID("ID_NEWLINES"),wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&RegExDlg::OnValueChanged);
    Connect(XRCID("ID_TEXT"),wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&RegExDlg::OnValueChanged);
    //*)

    assert(m_regex);
    assert(m_quoted);
    assert(m_library);
    assert(m_nocase);
    assert(m_newlines);
    assert(m_text);
    assert(m_output);

    m_text->MoveAfterInTabOrder(m_quoted);

#if wxCHECK_VERSION(3, 1, 6)
    m_library->Delete(1);  // v3.1.6 made wxRE_ADVANCED a synonym of wxRE_EXTENDED, so delete it
#endif

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

void RegExDlg::OnValueChanged(cb_unused wxCommandEvent& event)
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

    if (regex == m_regex->GetValue() &&
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

void RegExDlg::EndModal(int retCode)
{
    wxScrollingDialog::EndModal(retCode);
}

wxArrayString RegExDlg::GetBuiltinMatches(const wxString& text)
{
    wxArrayString ret;

#if wxCHECK_VERSION(3, 1, 6)
    // wxRE_ADVANCED is a synonym of wxRE_EXTENDED, so it has been deleted from the choice
    int flags = m_library->GetSelection() ? wxRE_BASIC : wxRE_EXTENDED;
#else
    int flags = m_library->GetSelection();
#endif

    if (m_newlines->IsChecked())
        flags |= wxRE_NEWLINE;

    if (m_nocase->IsChecked())
        flags |= wxRE_ICASE;

    if (!m_wxre.Compile(m_regex->GetValue(), flags))
    {
        m_regex->SetForegroundColour(*wxWHITE);
        m_regex->SetBackgroundColour(*wxRED);
        m_regex->GetParent()->Refresh();
        return ret;
    }

    m_regex->SetForegroundColour(wxNullColour);
    m_regex->SetBackgroundColour(wxNullColour);
    m_regex->GetParent()->Refresh();

    if (!text.empty() && m_wxre.Matches(text))
    {
        const size_t count = m_wxre.GetMatchCount();
        for (size_t i = 0; i < count; ++i)
            ret.Add(m_wxre.GetMatch(text, i));
    }

    return ret;
}
