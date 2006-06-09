#include <sdk.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include <wx/filedlg.h>
#include <wx/textfile.h>
#include <wx/msgdlg.h>
#include <wx/app.h>
#include <globals.h>
#include "debuggergdb.h"
#include "debuggerdriver.h"
#include "debuggertree.h"
#include <manager.h>
#include <messagemanager.h>
#include <cbexception.h>

#include "editwatchdlg.h"

int cbCustom_WATCHES_CHANGED = wxNewId();
int idTree = wxNewId();
int idAddWatch = wxNewId();
int idLoadWatchFile = wxNewId();
int idSaveWatchFile = wxNewId();
int idEditWatch = wxNewId();
int idDeleteWatch = wxNewId();
int idDeleteAllWatches = wxNewId();
int idDereferenceValue = wxNewId();
int idWatchThis = wxNewId();
int idChangeValue = wxNewId();

#ifndef __WXMSW__
/*
	Under wxGTK, I have noticed that wxTreeCtrl is not sending a EVT_COMMAND_RIGHT_CLICK
	event when right-clicking on the client area.
	This is a "proxy" wxTreeCtrl descendant that handles this for us...
*/
class WatchTree : public wxTreeCtrl
{
	public:
		WatchTree(wxWindow* parent, int id)
            : wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT)
        {}
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
	EVT_MENU(idLoadWatchFile, DebuggerTree::OnLoadWatchFile)
	EVT_MENU(idSaveWatchFile, DebuggerTree::OnSaveWatchFile)
	EVT_MENU(idEditWatch, DebuggerTree::OnEditWatch)
	EVT_MENU(idDeleteWatch, DebuggerTree::OnDeleteWatch)
	EVT_MENU(idDeleteAllWatches, DebuggerTree::OnDeleteAllWatches)
	EVT_MENU(idDereferenceValue, DebuggerTree::OnDereferencePointer)
	EVT_MENU(idWatchThis, DebuggerTree::OnWatchThis)
	EVT_MENU(idChangeValue, DebuggerTree::OnChangeValue)
END_EVENT_TABLE()

DebuggerTree::DebuggerTree(wxWindow* parent, DebuggerGDB* debugger)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN),
	m_pDebugger(debugger),
	m_InUpdateBlock(false)
{
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
#ifndef __WXMSW__
	m_pTree = new WatchTree(this, idTree);
#else
	m_pTree = new wxTreeCtrl(this, idTree, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
#endif
    bs->Add(m_pTree, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

//    m_pParent->AddPage(this, _("Watches"));
//    m_PageIndex = m_pParent->GetPageCount() - 1;
}

DebuggerTree::~DebuggerTree()
{
	//dtor
//	m_pParent->RemovePage(m_PageIndex);
}

void DebuggerTree::BeginUpdateTree()
{
    if (m_InUpdateBlock)
    {
//        Manager::Get()->GetMessageManager()->DebugLogWarning(_T("DebuggerTree::BeginUpdateTree() while already in update block"));
        return;
    }

//    if (m_pTree->IsExpanded(m_pTree->GetRootItem()))
//        ::SaveTreeState(m_pTree, m_pTree->GetRootItem(), m_TreeState);
//	m_pTree->Freeze();

//	m_pTree->DeleteAllItems();
    if (!m_pTree->GetRootItem())
        m_pTree->AddRoot(_("Watches"));
    MarkAllNodes(m_pTree->GetRootItem(), false);
	m_InUpdateBlock = true;
}

void DebuggerTree::BuildTree(Watch* watch, const wxString& infoText, WatchStringFormat fmt)
{
    if (!m_InUpdateBlock)
    {
//        Manager::Get()->GetMessageManager()->DebugLogWarning(_T("DebuggerTree::BuildTree() while not in update block"));
        return;
    }

    if (fmt == wsfGDB)
        BuildTreeGDB(watch, infoText);
    else
        BuildTreeCDB(watch, infoText);
}

void DebuggerTree::EndUpdateTree()
{
    if (!m_InUpdateBlock)
    {
//        Manager::Get()->GetMessageManager()->DebugLogWarning(_T("DebuggerTree::EndUpdateTree() while not in update block"));
        return;
    }

//    ::RestoreTreeState(m_pTree, m_pTree->GetRootItem(), m_TreeState);
    ClearAllMarkedNodes(m_pTree->GetRootItem(), false);
    if (!m_pTree->IsExpanded(m_pTree->GetRootItem()))
        m_pTree->Expand(m_pTree->GetRootItem());
//    m_pTree->Thaw();
    m_InUpdateBlock = false;
}

/** Finds a child tree item, if it exists. */
bool DebuggerTree::FindChildItem(const wxString& item, const wxTreeItemId& parent, wxTreeItemId& result)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId id = m_pTree->GetFirstChild(parent, cookie);
    while (id)
    {
        if (m_pTree->GetItemText(id).Matches(item))
        {
            result = id;
            return true;
        }
        id = m_pTree->GetNextChild(parent, cookie);
    }
    return false;
}

wxTreeItemId DebuggerTree::AddItem(wxTreeItemId& parent, const wxString& text, Watch* watch, bool* newlyAdded)
{
    wxTreeItemId result;
    if (newlyAdded)
        *newlyAdded = false;

    wxString search = text;
    wxString value = text;
    int pos = search.Find(_T(" = "));
    if (pos != -1)
    {
        value = search.Right((search.Length() - pos) + 3);
        search.Truncate(pos + 3);
        search += _T('*');
    }

    if (FindChildItem(search, parent, result))
    {
        wxString oldtext = m_pTree->GetItemText(result);
        wxString oldvalue = value;
        int pos = oldtext.Find(_T(" = "));
        if (pos != -1)
            oldvalue = oldtext.Right((oldtext.Length() - pos) + 3);

        m_pTree->SetItemText(result, text);
        m_pTree->SetItemData(result, new WatchTreeData(watch));

        if (value != oldvalue)
        {
            m_pTree->SetItemTextColour(result, *wxRED);
            wxTreeItemId par = parent;
            while (par.IsOk())
            {
                m_pTree->SetItemTextColour(par, *wxRED);
                par = m_pTree->GetItemParent(par);
            }
        }
        else
            m_pTree->SetItemTextColour(result, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    }
    else
    {
        result = m_pTree->AppendItem(parent, text, -1, -1, new WatchTreeData(watch));
        if (newlyAdded)
            *newlyAdded = true;
    }
    return result;
}

void DebuggerTree::MarkAllNodes(const wxTreeItemId& parent, bool uptodate)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId id = m_pTree->GetFirstChild(parent, cookie);
    while (id)
    {
        WatchTreeData* data = dynamic_cast<WatchTreeData*>(m_pTree->GetItemData(id));
        if (data)
            data->m_UpToDate = uptodate;
        MarkAllNodes(id, uptodate);
        id = m_pTree->GetNextChild(parent, cookie);
    }
}

void DebuggerTree::ClearAllMarkedNodes(const wxTreeItemId& parent, bool uptodate)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId id = m_pTree->GetFirstChild(parent, cookie);
    while (id)
    {
        wxTreeItemId nextid = m_pTree->GetNextChild(parent, cookie);
        WatchTreeData* data = dynamic_cast<WatchTreeData*>(m_pTree->GetItemData(id));
        if (data && data->m_UpToDate == uptodate)
        {
            m_pTree->Delete(id);
        }
        else
            ClearAllMarkedNodes(id, uptodate);
        id = nextid;
    }
}

int DebuggerTree::FindCharOutsideQuotes(const wxString& str, wxChar ch)
{
    int len = str.Length();
    int i = 0;
    bool inSingleQuotes = false;
    bool inDoubleQuotes = false;
    wxChar lastChar = _T('\0');
    while (i < len)
    {
        wxChar currChar = str.GetChar(i);

        // did we find the char outside of any quotes?
        if (!inSingleQuotes && !inDoubleQuotes && currChar == ch)
            return i;

        // double quotes (not escaped)
        if (currChar == _T('"') && lastChar != _T('\\'))
        {
            // if not in single quotes, toggle the flag
            if (!inSingleQuotes)
                inDoubleQuotes = !inDoubleQuotes;
        }
        // single quotes (not escaped)
        else if (currChar == _T('\'') && lastChar != _T('\\'))
        {
            // if not in double quotes, toggle the flag
            if (!inDoubleQuotes)
                inSingleQuotes = !inSingleQuotes;
        }
        // don't be fooled by double-escape
        else if (currChar == _T('\\') && lastChar == _T('\\'))
        {
            // this will be assigned to lastChar
            // so it's not an escape char
            currChar = _T('\0');
        }

        lastChar = currChar;
        ++i;
    }
    return -1;
}

int DebuggerTree::FindCommaPos(const wxString& str)
{
    // comma is a special case because it separates the fields
    // but it can also appear in a function/template signature, where
    // we shouldn't treat it as a field separator

    // what we 'll do now, is decide if the comma is inside
    // a function signature.
    // we 'll do it by counting the opening and closing parenthesis/angled-brackets
    // *up to* the comma.
    // if they 're equal, it's a field separator.
    // if they 're not, it's in a function signature
    // ;)

    int len = str.Length();
    int i = 0;
    int parCount = 0;
    int braCount = 0;
    bool inQuotes = false;
    while (i < len)
    {
        wxChar ch = str.GetChar(i);
        switch (ch)
        {
            case _T('('):
                ++parCount; // increment on opening parenthesis
                break;

            case _T(')'):
                --parCount; // decrement on closing parenthesis
                break;

            case _T('<'):
                ++braCount; // increment on opening angle bracket
                break;

            case _T('>'):
                --braCount; // decrement on closing angle bracket
                break;

            case _T('"'):
                // fall through
            case _T('\''):
                inQuotes = !inQuotes; // toggle inQuotes flag
                break;

            default:
                break;
        }

        // if it's not inside quotes *and* we have parCount == 0, it's a field separator
        if (!inQuotes && parCount == 0 && braCount == 0 && ch == _T(','))
            return i;
        ++i;
    }
    return -1;
}

void DebuggerTree::ParseEntry(Watch* watch, wxTreeItemId& parent, wxString& text)
{
#define MIN(a,b) (a < b ? a : b)
    if (text.IsEmpty())
        return;
//    Manager::Get()->GetMessageManager()->DebugLog("DebuggerTree::ParseEntry(): Parsing '%s' (itemId=%p)", text.c_str(), &parent);
	while (1)
	{
		// trim the string from left and right
		text.Trim(true);
		text.Trim(false);

		// find position of '{', '}' and ',' ***outside*** of any quotes.
		// decide which is nearer to the start
		int braceOpenPos = FindCharOutsideQuotes(text, _T('{'));
		if (braceOpenPos == -1)	braceOpenPos = 0xFFFFFE;
		int braceClosePos = FindCharOutsideQuotes(text, _T('}'));
		if (braceClosePos == -1) braceClosePos = 0xFFFFFE;
        int commaPos = FindCommaPos(text);
		if (commaPos == -1) commaPos = 0xFFFFFE;
		int pos = MIN(commaPos, MIN(braceOpenPos, braceClosePos));

		if (pos == 0xFFFFFE)
		{
			if (text.Right(3).Matches(_T(" = ")))
				text.Truncate(text.Length() - 3);
			if (!text.IsEmpty())
			{
			    AddItem(parent, text, watch);
				text.Clear();
            }
			break;
		}
		else
		{
			wxTreeItemId newParent = parent;
			wxString tmp = text.Left(pos);
			bool newlyAdded = false;

			if (tmp.Right(3).Matches(_T(" = ")))
				tmp.Truncate(tmp.Length() - 3); // remove " = " if last in string
			if (!tmp.IsEmpty())
                newParent = AddItem(parent, tmp, watch, &newlyAdded); // add entry
			text.Remove(0, pos + 1);

			if (pos == braceOpenPos)
			{
				ParseEntry(watch, newParent, text); // proceed one level deeper
				if (newlyAdded)
                    m_pTree->Expand(newParent);
			}
			else if (pos == braceClosePos)
				break; // return one level up
		}
	}
#undef MIN
}

void DebuggerTree::BuildTreeGDB(Watch* watch, const wxString& infoText)
{
//    Manager::Get()->GetMessageManager()->DebugLog("DebuggerTree::BuildTree(): Parsing '%s'", infoText.c_str());
	wxString buffer = infoText;
	// remove CRLFs (except if inside quotes)
	int len = buffer.Length();
	bool inQuotes = false;
	for (int i = 0; i < len; ++i)
	{
        if (buffer.GetChar(i) == _T('"') && (i == 0 || (i > 0 && buffer.GetChar(i - 1) != _T('\\'))))
            inQuotes = !inQuotes;
        if (!inQuotes)
        {
            if (buffer.GetChar(i) == _T('\r'))
                buffer.SetChar(i, _T(' '));
            else if (buffer.GetChar(i) == _T('\n'))
                buffer.SetChar(i, _T(','));
        }
	}
	wxTreeItemId id = m_pTree->GetRootItem();
	ParseEntry(watch, id, buffer);
}

void DebuggerTree::BuildTreeCDB(Watch* watch, const wxString& infoText)
{
    wxTreeItemId parent = m_pTree->GetRootItem();
    wxTreeItemId node = parent;

    wxArrayString lines = GetArrayFromString(infoText, _T('\n'), false);
    size_t col = 0;
    for (unsigned int i = 0; i < lines.GetCount(); ++i)
    {
        size_t thiscol = lines[i].find_first_not_of(_T(" \t"));
        size_t nextcol = i < lines.GetCount() - 1 ? lines[i + 1].find_first_not_of(_T(" \t")) : wxString::npos;
        if (thiscol > col)
        {
            // add child node
            parent = node;
            col = thiscol;
        }
        else if (thiscol < col)
        {
            // go one level up
            parent = m_pTree->GetItemParent(parent);
            col = thiscol;
        }
        wxString actual;
        int sep = lines[i].First(_T(" = "));
        if (sep != -1)
            actual = lines[i].SubString(0, sep).Strip(wxString::both) +
                    _T(" = ") +
                    lines[i].SubString(sep + 2, lines[i].Length()).Strip(wxString::both);
        else
            actual = lines[i].Strip(wxString::both);

        node = AddItem(parent, actual, watch);

        // if this node doesn't have any children, delete any existing
        if (nextcol == thiscol || nextcol == wxString::npos)
            m_pTree->DeleteChildren(node);
    }
}

void DebuggerTree::ClearWatches()
{
	m_Watches.Clear();
	NotifyForChangedWatches();
}

int SortWatchesByName(Watch** first, Watch** second)
{
    return (*first)->keyword.Cmp((*second)->keyword);
}

void DebuggerTree::AddWatch(const wxString& watch, WatchFormat format, bool notify)
{
    if (FindWatchIndex(watch, format) != wxNOT_FOUND)
        return; // already there
	m_Watches.Add(Watch(watch, format));
	m_Watches.Sort(SortWatchesByName);

	if (notify)
        NotifyForChangedWatches();
}

void DebuggerTree::SetWatches(const WatchesArray& watches)
{
	m_Watches = watches;
	NotifyForChangedWatches();
}

void DebuggerTree::NotifyForChangedWatches()
{
	wxCommandEvent event(cbCustom_WATCHES_CHANGED);
	wxPostEvent(m_pDebugger, event);
}

const WatchesArray& DebuggerTree::GetWatches()
{
	return m_Watches;
}

void DebuggerTree::DeleteWatch(Watch* watch, bool notify)
{
    DeleteWatch(watch->keyword, watch->format, notify);
}

void DebuggerTree::DeleteWatch(const wxString& watch, WatchFormat format, bool notify)
{
    int idx = FindWatchIndex(watch, format);
    if (idx != wxNOT_FOUND)
    {
        m_Watches.RemoveAt(idx);
        if (notify)
            NotifyForChangedWatches();
    }
}

void DebuggerTree::DeleteAllWatches()
{
    m_pTree->DeleteAllItems();
    m_Watches.Clear();
	NotifyForChangedWatches();
}

Watch* DebuggerTree::FindWatch(const wxString& watch, WatchFormat format)
{
    int idx = FindWatchIndex(watch, format);
    if (idx != wxNOT_FOUND)
        return &m_Watches[idx];
    return 0;
}

int DebuggerTree::FindWatchIndex(const wxString& watch, WatchFormat format)
{
    size_t wc = m_Watches.GetCount();
    for (size_t i = 0; i < wc; ++i)
    {
        Watch& w = m_Watches[i];
        if (w.keyword.Matches(watch) && (format == Any || w.format == format))
        {
            return i;
        }
    }
    return wxNOT_FOUND;
}

void DebuggerTree::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
	wxString caption;
    wxMenu menu(wxEmptyString);

    // if we right-clicked on a pointer, add a "dereference pointer" entry
    wxString itemtext = m_pTree->GetItemText(id);
    if (itemtext.Find(_T('*')) != wxNOT_FOUND)
    {
        menu.Append(idDereferenceValue, wxString::Format(_("Dereference pointer '%s'"), itemtext.BeforeFirst(_T('=')).c_str()));
        menu.AppendSeparator();
    }

	// add watch always visible
	menu.Append(idAddWatch, _("&Add watch"));
	menu.Append(idWatchThis, _("Watch '*&this'"));

	// we have to have a valid id for the following to be enabled
    WatchTreeData* data = dynamic_cast<WatchTreeData*>(m_pTree->GetItemData(id));
    if (id.IsOk() && // valid item
        data && data->m_pWatch) // *is* a watch
    {
        menu.Append(idEditWatch, _("&Edit watch"));
        menu.Append(idDeleteWatch, _("&Delete watch"));
	}
    menu.AppendSeparator();
    menu.Append(idChangeValue, _("&Change value..."));
    menu.AppendSeparator();
	menu.Append(idLoadWatchFile, _("&Load watch file"));
	menu.Append(idSaveWatchFile, _("&Save watch file"));
    menu.AppendSeparator();
    menu.Append(idDeleteAllWatches, _("Delete all watches"));

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

void DebuggerTree::OnLoadWatchFile(wxCommandEvent& event)
{
    WatchesArray fromFile = m_Watches; // copy current watches

    // ToDo:
    // - Currently each watch is imported as WatchType "Unspecified". This should
    //   be changed that the file contains another (optional) column with the type.
    // - Change "Watch files" format to XML?
    // - With the current implementation sometimes the debugger tree gets weird.
    // - (Maybe) verify that watches are not already present?

    wxString fname;
    wxFileDialog dlg (Manager::Get()->GetAppWindow(),
                    _T("Load debugger watch file"),
                    _T(""),
                    _T(""),
                    _T("Watch files (*.watch)|*.watch|Any file (*)|*"),
                    wxOPEN | wxFILE_MUST_EXIST | wxCHANGE_DIR);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxTextFile tf(dlg.GetPath());
    if (tf.Open())
    {
        // iterate over each line of file and send to debugger
        wxString cmd = tf.GetFirstLine();
        while(true)
        {
            if (!cmd.IsEmpty()) // Skip empty lines
            {
//                Manager::Get()->GetMessageManager()->DebugLog(_T("Adding watch \"%s\" to debugger:"), keyword);
                AddWatch(cmd, Undefined, false); // do not notify about new watch (we 'll do it when done)
            }
            if (tf.Eof()) break;
                cmd = tf.GetNextLine();
        }
        tf.Close(); // release file handle

        // notify about changed watches
        NotifyForChangedWatches();
    }
    else
        Manager::Get()->GetMessageManager()->DebugLog(_T("Error opening debugger watch file: %s"), fname.c_str());
}

void DebuggerTree::OnSaveWatchFile(wxCommandEvent& event)
{
    // Verify that there ARE watches to save
    size_t wc = m_Watches.GetCount();
    if (wc<1)
    {
        cbMessageBox(_("There are no watches in the list to save."),
                     _("Save Watches"), wxICON_ERROR);
        return;
    }

    wxString fname;
    wxFileDialog dlg (Manager::Get()->GetAppWindow(),
                    _T("Save debugger watch file"),
                    _T(""),
                    _T(""),
                    _T("Watch files (*.watch)|*.watch|Any file (*)|*"),
                    wxSAVE | wxOVERWRITE_PROMPT);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxTextFile tf(dlg.GetPath());
    bool bSuccess = false;

    // Create() will fail if the file exist -> must use Open() if file exist
    if (tf.Exists())
    {
        bSuccess = tf.Open();
        if (bSuccess) tf.Clear(); // remove old content (if any)
    }
    else
    {
        bSuccess = tf.Create();
    }

    if (bSuccess)
    {
        // iterate over each watch and write them to the file buffer
        for (size_t i = 0; i < wc; ++i)
        {
            Watch& w = m_Watches[i];
            tf.AddLine(w.keyword);
        }
        tf.Write(); // Write buffer to file
        tf.Close(); // release file handle
    }
    else
        Manager::Get()->GetMessageManager()->DebugLog(_T("Error opening debugger watch file: %s"), fname.c_str());
}

void DebuggerTree::OnEditWatch(wxCommandEvent& event)
{
    WatchTreeData* data = static_cast<WatchTreeData*>(m_pTree->GetItemData(m_pTree->GetSelection()));
    Watch* w = data ? data->m_pWatch : 0;
    if (w)
    {
        EditWatchDlg dlg(w);
        PlaceWindow(&dlg);
        if (dlg.ShowModal() == wxID_OK && !dlg.GetWatch().keyword.IsEmpty())
        {
            *w = dlg.GetWatch();
            NotifyForChangedWatches();
        }
    }
}

void DebuggerTree::OnAddWatch(wxCommandEvent& event)
{
    EditWatchDlg dlg;
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK && !dlg.GetWatch().keyword.IsEmpty())
        AddWatch(dlg.GetWatch().keyword, dlg.GetWatch().format);
}

void DebuggerTree::OnDeleteWatch(wxCommandEvent& event)
{
    WatchTreeData* data = static_cast<WatchTreeData*>(m_pTree->GetItemData(m_pTree->GetSelection()));
    Watch* w = data ? data->m_pWatch : 0;
    if (w)
    {
        DeleteWatch(w);
        m_pTree->Delete(m_pTree->GetSelection());
    }
}

void DebuggerTree::OnDeleteAllWatches(wxCommandEvent& event)
{
    if (cbMessageBox(_("Are you sure you want to delete all watches?"), _("Question"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
        DeleteAllWatches();
}

void DebuggerTree::OnDereferencePointer(wxCommandEvent& event)
{
    WatchTreeData* data = static_cast<WatchTreeData*>(m_pTree->GetItemData(m_pTree->GetSelection()));
    Watch* w = data ? data->m_pWatch : 0;
    if (w)
        m_Watches.Add(Watch(_T('*') + w->keyword));
    else
    {
        wxString itemtext = m_pTree->GetItemText(m_pTree->GetSelection());
        m_Watches.Add(Watch(_T('*') + itemtext.BeforeFirst(_T('='))));
    }
    NotifyForChangedWatches();
}

void DebuggerTree::OnWatchThis(wxCommandEvent& event)
{
    m_Watches.Add(Watch(_T("*this")));
    NotifyForChangedWatches();
}

void DebuggerTree::FixupVarNameForChange(wxString& str)
{
    // remove everything from '=' and after
    str = str.BeforeFirst(_T('='));
    str.Trim(false);
    str.Trim(true);

    // if it contains invalid chars, clear it
    if (str.find_first_of(_T(" \t")) != wxString::npos)
        str.Clear();
}

void DebuggerTree::OnChangeValue(wxCommandEvent& event)
{
    DebuggerDriver* driver = m_pDebugger->GetState().GetDriver();
    if (!driver)
        return;

    wxString var;
    wxTreeItemId parent;
    wxTreeItemId item = m_pTree->GetSelection();
    if (item.IsOk())
    {
        wxString itemtext = m_pTree->GetItemText(item);
        FixupVarNameForChange(itemtext);
        if (!itemtext.IsEmpty())
            var = itemtext;
        while ((parent = m_pTree->GetItemParent(item)) && parent.IsOk() && parent != m_pTree->GetRootItem())
        {
            item = parent;
            wxString itemtext = m_pTree->GetItemText(item);

            FixupVarNameForChange(itemtext);

            if (!itemtext.IsEmpty())
            {
                if (!var.IsEmpty())
                    var.Prepend(_T('.'));
                var.Prepend(itemtext);
            }
        }
    }

    if (!var.IsEmpty())
    {
        // ask for the new value
        wxString newvalue = wxGetTextFromUser(wxString::Format(_("Please enter the new value for %s"), var.c_str()),
                                                _("Change variable's value"));
        // finally, actually change the value
        if (!newvalue.IsEmpty())
        {
            driver->SetVarValue(var, newvalue);
            NotifyForChangedWatches();
        }
    }
}
