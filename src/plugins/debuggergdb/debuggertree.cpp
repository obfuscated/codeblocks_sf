#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/app.h>
#include <globals.h>
#include "debuggertree.h"

int cbCustom_WATCHES_CHANGED = wxNewId();
int idTree = wxNewId();
int idAddWatch = wxNewId();
int idEditWatch = wxNewId();
int idDeleteWatch = wxNewId();

#ifndef __WXMSW__
/*
	Under wxGTK, I have noticed that wxTreeCtrl is not sending a EVT_COMMAND_RIGHT_CLICK
	event when right-clicking on the client area.
	This is a "proxy" wxTreeCtrl descendant that handles this for us...
*/
class WatchTree : public wxTreeCtrl
{
	public:
		WatchTree(wxWindow* parent, int id) : wxTreeCtrl(parent, id) {}
	protected:
		void OnRightClick(wxMouseEvent& event)
		{
		    //Manager::Get()->GetMessageManager()->DebugLog("OnRightClick");
		    int flags;
		    HitTest(wxPoint(event.GetX(), event.GetY()), flags);
		    if (flags & (wxTREE_HITTEST_ABOVE | wxTREE_HITTEST_BELOW | wxTREE_HITTEST_NOWHERE))
		    {
		    	// "proxy" the call
			    wxCommandEvent e(wxEVT_COMMAND_RIGHT_CLICK, idTree);
				wxPostEvent(GetParent(), e);
			}
			else
		    	event.Skip();
		}
		DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(WatchTree, wxTreeCtrl)
	EVT_RIGHT_DOWN(WatchTree::OnRightClick)
END_EVENT_TABLE()
#endif // !__WXMSW__

BEGIN_EVENT_TABLE(DebuggerTree, wxPanel)
    EVT_TREE_ITEM_RIGHT_CLICK(idTree, DebuggerTree::OnTreeRightClick)
    EVT_COMMAND_RIGHT_CLICK(idTree, DebuggerTree::OnRightClick)
	EVT_MENU(idAddWatch, DebuggerTree::OnAddWatch)
	EVT_MENU(idEditWatch, DebuggerTree::OnEditWatch)
	EVT_MENU(idDeleteWatch, DebuggerTree::OnDeleteWatch)
END_EVENT_TABLE()

DebuggerTree::DebuggerTree(wxEvtHandler* debugger, wxNotebook* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN),
    m_pParent(parent),
	m_pDebugger(debugger)
{
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
#ifndef __WXMSW__
	m_pTree = new WatchTree(this, idTree);
#else
	m_pTree = new wxTreeCtrl(this, idTree);
#endif
    bs->Add(m_pTree, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

    m_pParent->AddPage(this, _("Watches"));
    m_PageIndex = m_pParent->GetPageCount() - 1;
    
    BuildTree(wxEmptyString);
}

DebuggerTree::~DebuggerTree()
{
	//dtor
}

void DebuggerTree::BuildTree(const wxString& infoText)
{
	wxArrayString treeState;
	::SaveTreeState(m_pTree, m_pTree->GetRootItem(), treeState);
	m_pTree->Freeze();

	m_pTree->DeleteAllItems();
	wxTreeItemId root = m_pTree->AddRoot(_("Variables"));
	
	wxString buffer = infoText;
	wxTreeItemId parent = root;
	while (1)
	{
		int linePos = buffer.First('\n');
		if (linePos == -1)
		{
			ParseEntry(parent, buffer);
			break;
		}
		wxString tmp = buffer.Left(linePos);
		ParseEntry(parent, tmp);
		buffer.Remove(0, linePos + 1);
	}

	m_pTree->Expand(root);
	::RestoreTreeState(m_pTree, root, treeState);
	m_pTree->Thaw();
}

void DebuggerTree::ParseEntry(const wxTreeItemId& parent, wxString& text)
{
#define MIN(a,b) (a < b ? a : b)
    if (text.IsEmpty())
        return;
	while (1)
	{
		// trim the string from left and right
		text.Trim(true);
		text.Trim(false);
		
		// find position of '{', '}' and ','.
		// decide which is nearer to the start
		int braceOpenPos = text.First('{');
		if (braceOpenPos == -1)	braceOpenPos = 0xFFFFFF;
		int braceClosePos = text.First('}');
		if (braceClosePos == -1)	braceClosePos = 0xFFFFFF;
		int commaPos = text.First(',');
		if (commaPos == -1)	commaPos = 0xFFFFFF;
		int pos = MIN(commaPos, MIN(braceOpenPos, braceClosePos));
		
		if (pos == 0xFFFFFF)
		{
			if (text.Right(3).Matches(" = "))
				text.Truncate(text.Length() - 3);
			if (!text.IsEmpty())
				m_pTree->AppendItem(parent, text);
			break;
		}
		else
		{
			wxTreeItemId newParent = parent;
			wxString tmp = text.Left(pos);
			
			if (tmp.Right(3).Matches(" = "))
				tmp.Truncate(tmp.Length() - 3); // remove " = " if last in string
			if (!tmp.IsEmpty())
				newParent = m_pTree->AppendItem(parent, tmp); // add entry
			text.Remove(0, pos + 1);
			
			if (pos == braceOpenPos)
				ParseEntry(newParent, text); // proceed one level deeper
			else if (pos == braceClosePos)
				break; // return one level up
		}
	}
#undef MIN
}

void DebuggerTree::ClearWatches()
{
	m_Watches.Clear();
	wxCommandEvent event(cbCustom_WATCHES_CHANGED);
	wxPostEvent(m_pDebugger, event);
}

void DebuggerTree::AddWatch(const wxString& watch)
{
	m_Watches.Add(watch);
	m_Watches.Sort();
	wxCommandEvent event(cbCustom_WATCHES_CHANGED);
	wxPostEvent(m_pDebugger, event);
}

void DebuggerTree::SetWatches(const wxArrayString& watches)
{
	m_Watches = watches;
	wxCommandEvent event(cbCustom_WATCHES_CHANGED);
	wxPostEvent(m_pDebugger, event);
}

const wxArrayString& DebuggerTree::GetWatches()
{
	return m_Watches;
}

void DebuggerTree::DeleteWatch(const wxString& watch)
{
	m_Watches.Remove(watch);
	wxCommandEvent event(cbCustom_WATCHES_CHANGED);
	wxPostEvent(m_pDebugger, event);
}

void DebuggerTree::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
	wxString caption;
    wxMenu menu(wxEmptyString);
	
	// add watch always visible
	menu.Append(idAddWatch, _("&Add watch"));

	// we have to have a valid id for the following to be enabled
    if (id.IsOk()  && m_pTree->GetItemParent(id) == m_pTree->GetRootItem())
    {
        menu.Append(idEditWatch, _("&Edit watch"));
        menu.Append(idDeleteWatch, _("&Delete watch"));
	}
	
	PopupMenu(&menu, pt);
}

// events

void DebuggerTree::OnTreeRightClick(wxTreeEvent& event)
{
	m_pTree->SelectItem(event.GetItem());
    ShowMenu(event.GetItem(), event.GetPoint());
}

void DebuggerTree::OnRightClick(wxCommandEvent& event)
{
    wxTreeItemId tmp; // dummy var for next call
    // get right-click point
    wxPoint pt = wxGetMousePosition();
    pt = m_pTree->ScreenToClient(pt);
    
    ShowMenu(tmp, pt);
}

void DebuggerTree::OnAddWatch(wxCommandEvent& event)
{
	wxString w = wxGetTextFromUser(_("Add watch"), _("Enter the variable name to watch:"));
	if (!w.IsEmpty())
	{
		AddWatch(w);
	}
}

void DebuggerTree::OnEditWatch(wxCommandEvent& event)
{
	wxString item = m_pTree->GetItemText(m_pTree->GetSelection());
	wxString w = wxGetTextFromUser(_("Edit watch"), _("Edit the variable name:"), item);
	if (!w.IsEmpty())
	{
		DeleteWatch(item);
		AddWatch(w);
	}
}

void DebuggerTree::OnDeleteWatch(wxCommandEvent& event)
{
	wxString item = m_pTree->GetItemText(m_pTree->GetSelection());
	if (wxMessageBox(_("Delete this watched variable?"), _("Confirm"), wxYES_NO) == wxYES)
	{
		DeleteWatch(item);
	}
}
