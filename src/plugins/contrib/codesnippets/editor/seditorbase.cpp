/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

// Get rid of some precompile warnings when using sdk
// Stop following warning:
//warning: .../trunk/src/include/sdk_precomp.h.gch: not used because `EXPORT_LIB' not defined|
#undef CB_PRECOMP

#ifdef CB_PRECOMP
    #include "sdk_precomp.h"
#endif

#ifndef CB_PRECOMP
    #include <wx/filename.h>
    #include <wx/notebook.h>
    #include <wx/menu.h>
    #include <wx/textdlg.h> // wxGetTextFromUser
    #include <wx/wfstream.h>

    #include "manager.h"
    #include "seditorbase.h"
    #include "scbeditor.h"
    #include "seditormanager.h"
    #include "pluginmanager.h"
    #include "cbproject.h" // FileTreeData
#endif

#include <wx/xrc/xmlres.h>

#include "scbeditor.h"
#include "seditorbase.h"
#include "seditormanager.h"
#include "cbstyledtextctrl.h"
#include "cbauibook.h"
#include "snippetsconfig.h"
// ----------------------------------------------------------------------------
//  resources
// ----------------------------------------------------------------------------
int idEditHighlightMode = XRCID("idEditHighlightMode");
int idEditHighlightModeText = XRCID("idEditHighlightModeText");

// needed for initialization of variables
// ----------------------------------------------------------------------------
int editorbase_RegisterId(int id)
// ----------------------------------------------------------------------------
{
    wxRegisterId(id);
    return id;
}

// ----------------------------------------------------------------------------
struct EditorBaseInternalData
// ----------------------------------------------------------------------------
{
	EditorBaseInternalData(SEditorBase* owner)
		: m_pOwner(owner),
		m_DisplayingPopupMenu(false),
		m_CloseMe(false)
	{}

	SEditorBase* m_pOwner;
	bool m_DisplayingPopupMenu;
	bool m_CloseMe;
};

// The following lines reserve 255 consecutive id's
const int EditorMaxSwitchTo = 255;
const int idSwitchFile1 = wxNewId();
const int idSwitchFileMax = editorbase_RegisterId(idSwitchFile1 + EditorMaxSwitchTo -1);

const int idCloseMe = wxNewId();
const int idCloseAll = wxNewId();
const int idCloseAllOthers = wxNewId();
const int idSaveMe = wxNewId();
const int idSaveAll = wxNewId();
const int idSwitchTo = wxNewId();
const int idGoogle = wxNewId();
const int idGoogleCode = wxNewId();
const int idMsdn = wxNewId();

BEGIN_EVENT_TABLE(SEditorBase, wxPanel)
    EVT_MENU_RANGE(idSwitchFile1, idSwitchFileMax,SEditorBase::OnContextMenuEntry)
    EVT_MENU(idCloseMe, SEditorBase::OnContextMenuEntry)
    EVT_MENU(idCloseAll, SEditorBase::OnContextMenuEntry)
    EVT_MENU(idCloseAllOthers, SEditorBase::OnContextMenuEntry)
    EVT_MENU(idSaveMe, SEditorBase::OnContextMenuEntry)
    EVT_MENU(idSaveAll, SEditorBase::OnContextMenuEntry)
    EVT_MENU(idGoogle, SEditorBase::OnContextMenuEntry)
    EVT_MENU(idGoogleCode, SEditorBase::OnContextMenuEntry)
    EVT_MENU(idMsdn, SEditorBase::OnContextMenuEntry)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
void SEditorBase::InitFilename(const wxString& filename)
// ----------------------------------------------------------------------------
{
    if (filename.IsEmpty())
        m_Filename = realpath(CreateUniqueFilename());
    else
        m_Filename = realpath(filename);

    wxFileName fname;
    fname.Assign(m_Filename);
    m_Shortname = fname.GetFullName();
    //    Manager::Get()->GetLogManager()->DebugLog("ctor: Filename=%s\nShort=%s", m_Filename.c_str(), m_Shortname.c_str());
}

// ----------------------------------------------------------------------------
wxString SEditorBase::CreateUniqueFilename()
// ----------------------------------------------------------------------------
{
    const wxString prefix = _("Untitled");
    const wxString path = wxGetCwd() + wxFILE_SEP_PATH;
    wxString tmp;
    int iter = 0;
    while (true)
    {
        tmp.Clear();
        tmp << path << prefix << wxString::Format(_T("%d"), iter);
        if (! GetEditorManager()->GetEditor(tmp) &&
                !wxFileExists(path + tmp))
        {
            return tmp;
        }
        ++iter;
    }
}

// ----------------------------------------------------------------------------
SEditorBase::SEditorBase(wxWindow* parent, const wxString& filename)
// ----------------------------------------------------------------------------
        : wxPanel(parent, -1),
        m_IsBuiltinEditor(false),
        m_Shortname(_T("")),
        m_Filename(_T("")),
        m_WinTitle(filename)
{
    //ctor
    m_pParent = parent;
	m_pData = new EditorBaseInternalData(this);
    // memorize EditorManager for this editor
    m_pEditorManager = GetConfig()->GetEditorManager(::wxGetTopLevelParent(this));
    GetEditorManager()->AddCustomEditor(this);
    InitFilename(filename);
    SetTitle(m_Shortname);
}

// ----------------------------------------------------------------------------
SEditorBase::~SEditorBase()
// ----------------------------------------------------------------------------
{
    //asm("int3"); /*trap*/
    if (GetEditorManager() ) // sanity check
        GetEditorManager()->RemoveCustomEditor(this);

    if (Manager::Get()->GetPluginManager())
    {
        CodeBlocksEvent event(cbEVT_EDITOR_CLOSE);
        event.SetEditor((EditorBase*)this);
        event.SetString(m_Filename);

        ////(pecan 2008/4/22) Dont notify plugins this non-sdk editor
        //-Manager::Get()->GetPluginManager()->NotifyPlugins(event);
    }

    delete m_pData;
}

const wxString& SEditorBase::GetTitle()
{
    return m_WinTitle;
}

void SEditorBase::SetTitle(const wxString& newTitle)
{
    m_WinTitle = newTitle;
    int mypage = GetEditorManager()->FindPageFromEditor(this);
    if (mypage != -1)
        GetEditorManager()->GetNotebook()->SetPageText(mypage, newTitle);
}

void SEditorBase::Activate()
{
    GetEditorManager()->SetActiveEditor(this);
}

bool SEditorBase::Close()
{
    Destroy();
    return true;
}

bool SEditorBase::IsBuiltinEditor() const
{
    return m_IsBuiltinEditor;
}

bool SEditorBase::ThereAreOthers() const
{
    bool hasOthers = false;
    hasOthers = GetEditorManager()->GetEditorsCount() > 1;
    //    for(int i = 0; i < SnippetsSearchFrame::GetEditorManager()->GetEditorsCount(); ++i)
    //    {
    //        SEditorBase* other = SnippetsSearchFrame::GetEditorManager()->GetEditor(i);
    //        if (!other || other == (SEditorBase*)this)
    //            continue;
    //        hasOthers = true;
    //        break;
    //    }
    return hasOthers;
}

wxMenu* SEditorBase::CreateContextSubMenu(int id) // For context menus
{
    wxMenu* menu = 0;

    if(id == idSwitchTo)
    {
        menu = new wxMenu;
        m_SwitchTo.clear();
        for (int i = 0; i < EditorMaxSwitchTo && i < GetEditorManager()->GetEditorsCount(); ++i)
        {
            SEditorBase* other = GetEditorManager()->GetEditor(i);
            if (!other || other == this)
                continue;
            int id = idSwitchFile1+i;
            //-m_SwitchTo[id] = (EditorBase*)other;
            m_SwitchTo[id] = (SEditorBase*)other;
            menu->Append(id, other->GetShortName());
        }
        if(!menu->GetMenuItemCount())
        {
            delete menu;
            menu = 0;
        }
    }
    return menu;
}

void SEditorBase::BasicAddToContextMenu(wxMenu* popup,ModuleType type)   //pecan 2006/03/22
{
    bool noeditor = (type != mtEditorManager);                          //pecan 2006/03/22
    if (type == mtOpenFilesList)                                        //pecan 2006/03/22
    {
      popup->Append(idCloseMe, _("Close"));
      popup->Append(idCloseAll, _("Close all"));
      popup->Append(idCloseAllOthers, _("Close all others"));
      popup->AppendSeparator();
      popup->Append(idSaveMe, _("Save"));
      popup->Append(idSaveAll, _("Save all"));
      popup->AppendSeparator();
      // enable/disable some items, based on state
      popup->Enable(idSaveMe, GetModified());

      bool hasOthers = ThereAreOthers();
      popup->Enable(idCloseAll, hasOthers);
      popup->Enable(idCloseAllOthers, hasOthers);
    }
    if(!noeditor)
    {
        wxMenu* switchto = CreateContextSubMenu(idSwitchTo);
        if(switchto)
            popup->Append(idSwitchTo, _("Switch to"), switchto);
    }
}

void SEditorBase::DisplayContextMenu(const wxPoint& position, ModuleType type)   //pecan 2006/03/22
{
    bool noeditor = (type != mtEditorManager);                                  //pecan 2006/03/22
    // noeditor:
    // True if context menu belongs to open files tree;
    // False if belongs to cbEditor

    // inform the editors we 're just about to create a context menu
    if (!OnBeforeBuildContextMenu(position, type))              //pecan 2006/03/22
        return;

    wxMenu* popup = new wxMenu;

    if(!noeditor && wxGetKeyState(WXK_CONTROL))
    {
        cbStyledTextCtrl* control = GetEditorManager()->GetBuiltinActiveEditor()->GetControl();
        wxString text = control->GetSelectedText();
        if (text.IsEmpty())
        {
            const int pos = control->GetCurrentPos();
            text = control->GetTextRange(control->WordStartPosition(pos, true), control->WordEndPosition(pos, true));
        }

        if(wxMinimumVersion<2,6,1>::eval)
        {
            popup->Append(idGoogle, _("Search the Internet for \"") + text + _("\""));
            popup->Append(idMsdn, _("Search MSDN for \"") + text + _("\""));
            popup->Append(idGoogleCode, _("Search Google Code for \"") + text + _("\""));
        }
        lastWord = text;

        wxMenu* switchto = CreateContextSubMenu(idSwitchTo);
        if(switchto)
        {
            popup->AppendSeparator();
            popup->Append(idSwitchTo, _("Switch to"), switchto);
        }
    }
    else if(!noeditor && wxGetKeyState(WXK_ALT))
    { // run a script
    }
    else
    {
        // Basic functions
        BasicAddToContextMenu(popup, type);         //pecan 2006/03/22

        // Extended functions, part 1 (virtual)
        AddToContextMenu(popup, type, false);       //pecan 2006/03/22

        // ask other editors / plugins if they need to add any entries in this menu...
        FileTreeData* ftd = new FileTreeData(0, FileTreeData::ftdkUndefined);
        ftd->SetFolder(m_Filename);
        Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(type, popup, ftd);              //pecan 2006/03/22
        delete ftd;

        popup->AppendSeparator();
        // Extended functions, part 2 (virtual)
        AddToContextMenu(popup, type, true);            //pecan 2006/03/22
    }
    // inform the editors we 're done creating a context menu (just about to show it)
    OnAfterBuildContextMenu(type);              //pecan 2006/03/22

    // display menu
    wxPoint clientpos;
    if (position==wxDefaultPosition) // "context menu" key
    {
        // obtain the caret point (on the screen) as we assume
        // that the user wants to work with the keyboard
        cbStyledTextCtrl* const control = GetEditorManager()->GetBuiltinActiveEditor()->GetControl();
        clientpos = control->PointFromPosition(control->GetCurrentPos());
    }
    else
    {
        clientpos = ScreenToClient(position);
    }

	m_pData->m_DisplayingPopupMenu = true;
    PopupMenu(popup, clientpos);
    delete popup;
    m_pData->m_DisplayingPopupMenu = false;

    // this code *must* be the last code executed by this function
    // because it *will* invalidate 'this'
    if (m_pData->m_CloseMe)
		GetEditorManager()->Close(this);
}

void SEditorBase::OnContextMenuEntry(wxCommandEvent& event)
{
    // we have a single event handler for all popup menu entries
    // This was ported from cbEditor and used for the basic operations:
    // Switch to, close, save, etc.

    const int id = event.GetId();
    m_pData->m_CloseMe = false;

    if (id == idCloseMe)
    {
    	if (m_pData->m_DisplayingPopupMenu)
			m_pData->m_CloseMe = true; // defer delete 'this' until after PopupMenu() call returns
		else
			GetEditorManager()->Close(this);
    }
    else if (id == idCloseAll)
    {
    	if (m_pData->m_DisplayingPopupMenu)
    	{
			GetEditorManager()->CloseAllExcept(this);
			m_pData->m_CloseMe = true; // defer delete 'this' until after PopupMenu() call returns
    	}
		else
			GetEditorManager()->CloseAll();
    }
    else if (id == idCloseAllOthers)
    {
        GetEditorManager()->CloseAllExcept(this);
    }
    else if (id == idSaveMe)
    {
        Save();
    }
    else if (id == idSaveAll)
    {
        GetEditorManager()->SaveAll();
    }
    else
    if (id >= idSwitchFile1 && id <= idSwitchFileMax)
    {
        // "Switch to..." item
        SEditorBase *const ed = (SEditorBase*)m_SwitchTo[id];
        if (ed)
        {
            GetEditorManager()->SetActiveEditor(ed);
        }
        m_SwitchTo.clear();
    }
    else if(wxMinimumVersion<2,6,1>::eval)
    {
        if (id == idGoogleCode)
        {
            wxLaunchDefaultBrowser(wxString(_T("http://www.google.com/codesearch?q=")) << URLEncode(lastWord));
        }
        else if (id == idGoogle)
        {
            wxLaunchDefaultBrowser(wxString(_T("http://www.google.com/search?q=")) << URLEncode(lastWord));
        }
        else if (id == idMsdn)
        {
            wxLaunchDefaultBrowser(wxString(_T("http://search.microsoft.com/search/results.aspx?qu=")) << URLEncode(lastWord) << _T("&View=msdn"));
        }
    }
    else
    {
        event.Skip();
    }
}
// ----------------------------------------------------------------------------
void SEditorBase::SearchGotoLine()
// ----------------------------------------------------------------------------
{
    ScbEditor* ed = GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    int max = ed->GetControl()->LineFromPosition(ed->GetControl()->GetLength()) + 1;

    /**
    @remarks We use wxGetText instead of wxGetNumber because wxGetNumber *must*
    provide an initial line number...which doesn't make sense, and just keeps the
    user deleting the initial line number everytime he instantiates the dialog.
    However, this is just a temporary hack, because the default dialog used isn't
    that suitable either.
    */
    wxString strLine = wxGetTextFromUser( wxString::Format(_("Line (1 - %d): "), max),
                                        _("Goto line"),
                                        _T( "" ),
                                        this );
    long int line = 0;
    strLine.ToLong(&line);
    if ( line >= 1 && line <= max )
    {
        ed->UnfoldBlockFromLine(line - 1);
        ed->GotoLine(line - 1);
    }
}
// ----------------------------------------------------------------------------
void SEditorBase::SearchFind()
// ----------------------------------------------------------------------------
{
    bool bDoMultipleFiles ;
    bDoMultipleFiles = ! GetEditorManager()->GetBuiltinActiveEditor();
    GetEditorManager()->ShowFindDialog(false, bDoMultipleFiles);

}//SearchFind
// ----------------------------------------------------------------------------
void SEditorBase::SearchFindNext(bool next)
// ----------------------------------------------------------------------------
{
    GetEditorManager()->FindNext(next);

} // SearchFindNext
// ----------------------------------------------------------------------------
void SEditorBase::OnSearchReplace()
// ----------------------------------------------------------------------------
{
    bool bDoMultipleFiles = false;
    if(!bDoMultipleFiles)
    {
        bDoMultipleFiles = ! GetEditorManager()->GetBuiltinActiveEditor();
    }
    GetEditorManager()->ShowFindDialog(true, bDoMultipleFiles);

}//SearchReplace
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
