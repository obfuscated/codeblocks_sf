/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/string.h>
    #include <wx/utils.h>
    #include "globals.h"
    #include "manager.h"
    #include "configmanager.h"
#endif
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include "compilererrors.h"
#include "compilermessages.h"
#include "prep.h"

namespace
{
    int idList = wxNewId();
    int idMenuFit = wxNewId();
    int idMenuAutoFit = wxNewId();
}

BEGIN_EVENT_TABLE(CompilerMessages, wxEvtHandler)
END_EVENT_TABLE()

CompilerMessages::CompilerMessages(const wxArrayString& titles_in, const wxArrayInt& widths_in)
    : ListCtrlLogger(titles_in, widths_in, true)
{
    m_autoFit = Manager::Get()->GetConfigManager(wxT("compiler"))->ReadBool(wxT("/autofit_during_build"), false);
    //ctor
}

CompilerMessages::~CompilerMessages()
{
	//dtor
	Disconnect(idList, -1, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnClick);
    Disconnect(idList, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnDoubleClick);
    Disconnect(idMenuFit, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnFit);
    Disconnect(idMenuAutoFit, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnAutoFit);
}

wxWindow* CompilerMessages::CreateControl(wxWindow* parent)
{
    ListCtrlLogger::CreateControl(parent);
    control->SetId(idList);
    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnClick);
    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnDoubleClick);
    Connect(idMenuFit, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnFit);
    Connect(idMenuAutoFit, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &CompilerMessages::OnAutoFit);
    Manager::Get()->GetAppWindow()->PushEventHandler(this);
    return control;
}

void CompilerMessages::DestroyControls()
{
    if ( !Manager::Get()->IsAppShuttingDown() )
    {
        Manager::Get()->GetAppWindow()->RemoveEventHandler(this);
    }
}

bool CompilerMessages::HasFeature(Feature::Enum feature) const
{
    if (feature == Feature::Additional)
        return true;
    else
        return ListCtrlLogger::HasFeature(feature);
}

void CompilerMessages::AppendAdditionalMenuItems(wxMenu &menu)
{
    menu.Append(idMenuFit, _("Fit text"), _("Makes the whole text visible"));
    menu.AppendCheckItem(idMenuAutoFit, _("Fit automatically"),
                         _("Automatically makes the whole text visible during compilation"));
    menu.Check(idMenuAutoFit, m_autoFit);
}

void CompilerMessages::FocusError(int nr)
{
    if (nr < 0 or nr >= control->GetItemCount())
        return;
    control->SetItemState(nr, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    control->EnsureVisible(nr);
}

void CompilerMessages::OnClick(cb_unused wxCommandEvent& event)
{
    // single and double-click, behave the same

    // a compiler message has been clicked
    // go to the relevant file/line
    if (control->GetSelectedItemCount() == 0 || !m_pErrors)
        return;

    // find selected item index
    int index = control->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);

    // call the CompilerErrors* ptr; it 'll do all the hard work ;)
    m_pErrors->GotoError(index);
}

void CompilerMessages::OnDoubleClick(wxCommandEvent& event)
{
    // single and double-click, behave the same
    OnClick(event);
    return;
}

void CompilerMessages::OnFit(wxCommandEvent& WXUNUSED(event))
{
    FitColumns();
}

void CompilerMessages::OnAutoFit(wxCommandEvent& event)
{
    m_autoFit = event.IsChecked();
    Manager::Get()->GetConfigManager(wxT("compiler"))->Write(wxT("/autofit_during_build"), m_autoFit);
}

void CompilerMessages::AutoFitColumns(int cb_unused column)
{
    if (m_autoFit)
        FitColumns();
}

void CompilerMessages::FitColumns()
{
    if (!control)
        return;
    int count = control->GetColumnCount();
    for (int ii = 0; ii < count; ++ii)
        control->SetColumnWidth(ii, wxLIST_AUTOSIZE);
    int ctrlWidth = control->GetClientSize().x;
    for (int ii = 1; ii < count; ++ii)
        ctrlWidth -= control->GetColumnWidth(ii);
    int width = control->GetColumnWidth(0);
    if (width > ctrlWidth)
        control->SetColumnWidth(0, ctrlWidth);
}
