/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#include "goto_file.h"

#ifndef WX_PRECOMP
    //(*InternalHeadersPCH(GotoFile)
    #include <wx/sizer.h>
    #include <wx/listctrl.h>
    #include <wx/string.h>
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/textctrl.h>
    //*)

    #include <algorithm>
    #include <cmath>

    #include "cbexception.h"
#endif
//(*InternalHeaders(GotoFile)
//*)

//(*IdInit(GotoFile)
const long GotoFile::ID_TEXTCTRL1 = wxNewId();
const long GotoFile::ID_RESULT_LIST = wxNewId();
//*)

BEGIN_EVENT_TABLE(GotoFile,wxDialog)
    //(*EventTable(GotoFile)
    //*)
END_EVENT_TABLE()

GotoFile::GotoFile(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                   const wxString &message) :
    m_handler(this, iterator)
{
    cbAssert(parent);

    BuildContent(parent, iterator, title, message);
}

void GotoFile::BuildContent(wxWindow* parent, IncrementalSelectIterator *iterator, const wxString &title,
                            const wxString &message)
{
    //(*Initialize(GotoFile)
    wxStaticText* labelCtrl;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX, _T("wxID_ANY"));
    m_sizer = new wxBoxSizer(wxVERTICAL);
    labelCtrl = new wxStaticText(this, wxID_ANY, _("Some text"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    m_sizer->Add(labelCtrl, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_Text = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_Text->SetFocus();
    m_sizer->Add(m_Text, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    m_ResultList = new IncrementalListCtrl(this, ID_RESULT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxVSCROLL|wxHSCROLL, wxDefaultValidator, _T("ID_RESULT_LIST"));
    m_ResultList->SetMinSize(wxSize(500,300));
    m_sizer->Add(m_ResultList, 1, wxALL|wxEXPAND, 5);
    SetSizer(m_sizer);
    m_sizer->Fit(this);
    m_sizer->SetSizeHints(this);
    //*)

    SetTitle(title);
    labelCtrl->SetLabel(message);

    // Call this here to make sure the column widths are correctly calculated.
    m_handler.Init(m_ResultList, m_Text);

    const int columnWidth = iterator->GetColumnWidth(0);

    // Add first column
    wxListItem column;
    column.SetId(0);
    column.SetText( _("Column") );
    column.SetWidth(columnWidth);
    m_ResultList->InsertColumn(0, column);
    m_ResultList->SetIterator(iterator);

    // Call Fit to make sure all GetSize methods return correct values.
    m_sizer->Fit(this);

    {
        // Use GetItemRect to account for the spacing between rows. GetCharHeight is used just as
        // precaution if GetItemRect fails.
        wxRect itemRect;
        if (!m_ResultList->GetItemRect(0, itemRect))
            itemRect = wxRect();
        const int charHeight = std::max(m_ResultList->GetCharHeight(), itemRect.GetHeight());
        const int totalHeight = charHeight * m_ResultList->GetItemCount() + charHeight / 2;

        const wxSize minSize = m_ResultList->GetMinSize();
        int minYCorrected = minSize.y;

        // Make the list taller if there are many items in it. This should make it a bit easier to find
        // stuff. The height would be something like 50% of the display's client area height.
        if (totalHeight > minSize.y)
        {
            const wxRect monitorRect = cbGetMonitorRectForWindow(parent);
            const int monitorHeight = int(std::lround(monitorRect.GetHeight() * 0.5));
            minYCorrected = std::max(minYCorrected, std::min(monitorHeight, totalHeight));
        }

        // Resize the window to maximise visible items. Do this using SetSize instead of using
        // SetMinSize to allow the user to make the window smaller if he/she wishes to do so.
        const wxSize windowSize = GetSize();
        // GetSize for the list control could return a window smaller than the minSize, but the
        // window size could be accounting for list control's min size. This means that sizeDiff
        // could be calculated larger than needed. Account for this using std::max. This seems to
        // happen in wx2.8 builds.
        const wxSize listSize(std::max(m_ResultList->GetSize().x, minSize.x),
                              std::max(m_ResultList->GetSize().y, minSize.y));
        // This accounts for non-list UI elements present in the window.
        const wxSize sizeDiff = windowSize - listSize;
        SetSize(wxSize(std::max(columnWidth + sizeDiff.x, windowSize.x), minYCorrected + sizeDiff.y));
    }
}

GotoFile::~GotoFile()
{
    m_handler.DeInit(this);

    //(*Destroy(GotoFile)
    //*)
}

int GotoFile::GetSelection()
{
    return m_handler.GetSelection();
}

void GotoFile::AddControlBelowList(wxControl *control)
{
    wxSize sz = GetSize();
    m_sizer->Add(control, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_RIGHT, 8);
    Fit();
    SetMinSize(GetSize());
    SetSize(sz);
}
