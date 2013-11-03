/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h> // Code::Blocks SDK

#ifndef CB_PRECOMP
    #include <wx/listctrl.h>
    #include <configmanager.h>
    #include <cbeditor.h>
    #include <editormanager.h>
#endif

#include <configurationpanel.h>

#include <wx/listctrl.h>

#include "cbcolourmanager.h"
#include "cbstyledtextctrl.h"

#include "occurrenceshighlighting.h"
#include "highlighter.h"
#include "occurrencespanel.h"
#include "occurrenceshighlightingconfigurationpanel.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<OccurrencesHighlighting> reg(_T("OccurrencesHighlighting"));

    const int idViewOccurencesPanel               = wxNewId();
    const int idMenuEntryPermanent                = wxNewId();
    const int idMenuEntryRemove                   = wxNewId();
    const int idContextRemove                     = wxNewId();
}


// events handling
BEGIN_EVENT_TABLE(OccurrencesHighlighting, cbPlugin)
    // add any events you want to handle here
    EVT_MENU(idViewOccurencesPanel,      OccurrencesHighlighting::OnViewOccurrencesPanel)
    EVT_UPDATE_UI(idViewOccurencesPanel, OccurrencesHighlighting::OnUpdateViewMenu      )
END_EVENT_TABLE()

// constructor
OccurrencesHighlighting::OccurrencesHighlighting():
    m_pHighlighter(NULL),
    m_pPanel(NULL),
    m_pViewMenu(NULL)
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("occurrenceshighlighting.zip")))
        NotifyMissingFile(_T("occurrenceshighlighting.zip"));

    ColourManager* cm = Manager::Get()->GetColourManager();
    cm->RegisterColour(_("Editor"), _("Highlihgt occurrence"),                wxT("editor_highlight_occurrence"),             *wxRED  );
    cm->RegisterColour(_("Editor"), _("Permanently highlighted occurrences"), wxT("editor_highlight_occurrence_permanently"), *wxGREEN);
}

// destructor
OccurrencesHighlighting::~OccurrencesHighlighting()
{
}

void OccurrencesHighlighting::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    m_pHighlighter = new Highlighter(m_texts);

    EditorHooks::HookFunctorBase *editor_hook = new EditorHooks::HookFunctor<OccurrencesHighlighting>(this, &OccurrencesHighlighting::OnEditorHook);
    m_FunctorId = EditorHooks::RegisterHook(editor_hook);

    m_pPanel = new OccurrencesPanel(Manager::Get()->GetAppWindow());

    // add the foldpanel to the docking system
    CodeBlocksDockEvent dockevt(cbEVT_ADD_DOCK_WINDOW);
    dockevt.name = _T("HighlightedOccurrences");
    dockevt.title = _("Highlighted Occurrences");
    dockevt.pWindow = m_pPanel;
    dockevt.minimumSize.Set(50, 50);
    dockevt.desiredSize.Set(150, 100);
    dockevt.floatingSize.Set(100, 150);
    dockevt.dockSide = CodeBlocksDockEvent::dsLeft;
    dockevt.stretch = true;
    Manager::Get()->ProcessEvent(dockevt);

    m_pPanel->GetListCtrl()->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(OccurrencesHighlighting::OnListKeyDown), NULL, this);
    Connect(idMenuEntryPermanent, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OccurrencesHighlighting::OnHighlightPermanently), NULL, this);
    Connect(idMenuEntryRemove,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OccurrencesHighlighting::OnHighlightRemove),      NULL, this);

    m_pPanel->GetListCtrl()->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OccurrencesHighlighting::OnPanelPopupMenu), NULL, this);
    Connect(idContextRemove, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OccurrencesHighlighting::OnRemove), NULL, this);
}

void OccurrencesHighlighting::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...

    EditorHooks::UnregisterHook(m_FunctorId);

    delete m_pHighlighter;
    m_pHighlighter = NULL;

    m_pPanel->GetListCtrl()->Disconnect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(OccurrencesHighlighting::OnListKeyDown), NULL, this);
    Disconnect(idMenuEntryPermanent, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OccurrencesHighlighting::OnHighlightPermanently), NULL, (wxEvtHandler*)this);
    Disconnect(idMenuEntryRemove,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OccurrencesHighlighting::OnHighlightRemove),      NULL, (wxEvtHandler*)this);

    m_pPanel->GetListCtrl()->Disconnect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OccurrencesHighlighting::OnPanelPopupMenu), NULL, this);
    Disconnect(idContextRemove, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OccurrencesHighlighting::OnRemove), NULL, this);

    // remove panel from docking system
    if ( m_pPanel && !appShutDown )
    {
        CodeBlocksDockEvent docevt(cbEVT_REMOVE_DOCK_WINDOW);
        docevt.pWindow = m_pPanel;
        Manager::Get()->ProcessEvent(docevt);
        // and destroy the panel
        m_pPanel->Destroy();
        m_pPanel = NULL;
    }
}

void OccurrencesHighlighting::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    // Some library module is ready to display a pop-up menu.
    // Check the parameter \"type\" and see which module it is
    // and append any items you need in the menu...
    // TIP: for consistency, add a separator as the first item...

    if ( !IsAttached() ) return;
    if (type != mtEditorManager || !menu) return;

    EditorManager* emngr = Manager::Get()->GetEditorManager();
    if ( !emngr ) return;

    EditorBase *edb = emngr->GetActiveEditor();
    if ( !edb || !edb->IsBuiltinEditor() ) return;

    cbStyledTextCtrl* stc = ((cbEditor*)edb)->GetControl();
    if ( !stc ) return;

    wxString word = GetWordAtCaret();
    if ( word.IsEmpty() ) return;

    menu->AppendSeparator();

    if ( m_texts.find(word) == m_texts.end() )
        menu->Append(idMenuEntryPermanent, _T("Permanently Highlight '") + word + _T("'"));
    else
        menu->Append(idMenuEntryRemove,    _T("Don't Highlight '")       + word + _T("'"));

}
void OccurrencesHighlighting::BuildMenu(wxMenuBar* menuBar)
{
    // insert entry in the View menu
    int ViewPos = menuBar->FindMenu(_("&View"));
    if (ViewPos != wxNOT_FOUND)
    {
        m_pViewMenu = menuBar->GetMenu(ViewPos);
        wxMenuItemList& items = m_pViewMenu->GetMenuItems();
        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                m_pViewMenu->InsertCheckItem(i, idViewOccurencesPanel, _("&Highlighted Occurrences"), _("Toggle displaying the highlighted occurrences"));
                return;
            }
        }
        // not found so just append
        m_pViewMenu->AppendCheckItem(idViewOccurencesPanel, _("&Highlighted Occurrences"), _("Toggle displaying the highlighted occurrences"));
    }
}

void OccurrencesHighlighting::OnViewOccurrencesPanel(wxCommandEvent& event)
{
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pPanel;
    Manager::Get()->ProcessEvent(evt);
}

void OccurrencesHighlighting::OnUpdateViewMenu(wxUpdateUIEvent &event)
{
    if (m_pViewMenu)
    {
        bool isVis = IsWindowReallyShown((wxWindow*)m_pPanel);
        m_pViewMenu->Check(idViewOccurencesPanel, isVis);
        //event.Check(isVis);
    }

    // must do...
    event.Skip();
}

void OccurrencesHighlighting::OnListKeyDown(wxListEvent &event)
{
    switch ( event.GetKeyCode() )
    {
        case WXK_DELETE:
            RemoveSelected();
            break;

        case WXK_INSERT:
//            if ( GetWindowStyle() & wxLC_REPORT )
//            {
//                if ( GetWindowStyle() & wxLC_VIRTUAL )
//                {
//                    SetItemCount(GetItemCount() + 1);
//                }
//                else // !virtual
//                {
//                    InsertItemInReportView(event.GetIndex());
//                }
//            }
            //else: fall through

        default:

            event.Skip();
    }
}

wxString OccurrencesHighlighting::GetWordAtCaret()const
{
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

    if ( ed )
    {
        cbStyledTextCtrl *control = ed->GetControl();
        if (control)
        {
            wxString selectedText = control->GetSelectedText();

            if ( selectedText.IsEmpty() ||
                 selectedText.Contains(_T(" ")) ||selectedText.Contains(_T("\t")) )
            {
                const int pos = control->GetCurrentPos();
                const int ws = control->WordStartPosition(pos, true);
                const int we = control->WordEndPosition(pos, true);
                selectedText = control->GetTextRange(ws, we);
            }
            return selectedText;
        }
    }
    return wxEmptyString;
}

void OccurrencesHighlighting::OnHighlightPermanently(wxCommandEvent& WXUNUSED(event))
{
    wxString word = GetWordAtCaret();

    m_texts.insert(word);
    m_pHighlighter->TextsChanged();
    UpdatePanel();
}

void OccurrencesHighlighting::OnHighlightRemove(wxCommandEvent& WXUNUSED(event))
{
    wxString word = GetWordAtCaret();

    m_texts.erase(word);
    m_pHighlighter->TextsChanged();
    UpdatePanel();
}

void OccurrencesHighlighting::UpdatePanel()
{
    m_pPanel->GetListCtrl()->Freeze();
    m_pPanel->GetListCtrl()->DeleteAllItems();

    wxListItem item;
    for (std::set<wxString>::iterator it = m_texts.begin(); it != m_texts.end(); it++)
    {
        item.SetText(*it);
        m_pPanel->GetListCtrl()->InsertItem(item);
    }
    m_pPanel->GetListCtrl()->Thaw();
}

void OccurrencesHighlighting::OnPanelPopupMenu(wxContextMenuEvent& WXUNUSED(event))
{
    if (m_pPanel->GetListCtrl()->GetSelectedItemCount() > 0)
    {
        wxMenu *menu = new wxMenu;
        menu->Append(idContextRemove, _T("Remove"), _T(""));

        m_pPanel->GetListCtrl()->PopupMenu(menu);
    }
}

void OccurrencesHighlighting::OnRemove(wxCommandEvent& WXUNUSED(event))
{
    RemoveSelected();
}

void OccurrencesHighlighting::RemoveSelected()
{
    long item;

    item = m_pPanel->GetListCtrl()->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    while (item != -1)
    {
        m_texts.erase(m_pPanel->GetListCtrl()->GetItemText(item));
        m_pPanel->GetListCtrl()->DeleteItem(item);

        // -1 because the indices were shifted by DeleteItem()
        item = m_pPanel->GetListCtrl()->GetNextItem(item - 1,
                           wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }
    m_pHighlighter->TextsChanged();
}

void OccurrencesHighlighting::OnEditorHook(cbEditor* editor, wxScintillaEvent& event)
{
    m_pHighlighter->Call(editor, event);
}

cbConfigurationPanel* OccurrencesHighlighting::GetConfigurationPanel(wxWindow* parent)
{
    return new OccurrencesHighlightingConfigurationPanel(parent);
}
