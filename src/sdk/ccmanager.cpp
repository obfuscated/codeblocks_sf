/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#include "cbstyledtextctrl.h"
#include "ccmanager.h"

template<> CCManager* Mgr<CCManager>::instance = 0;
template<> bool  Mgr<CCManager>::isShutdown = false;

// class constructor
CCManager::CCManager() :
    lastEditor(nullptr), lastCCPlugin(nullptr)
{
    typedef cbEventFunctor<CCManager, CodeBlocksEvent> CCEvent;
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_TOOLTIP, new CCEvent(this, &CCManager::OnEditorTooltip));
    Manager::Get()->GetAppWindow()->Connect(wxEVT_ACTIVATE, wxActivateEventHandler(CCManager::OnActivateApp));
}

// class destructor
CCManager::~CCManager()
{
    Manager::Get()->RemoveAllEventSinksFor(this);
    Manager::Get()->GetAppWindow()->Disconnect(wxEVT_ACTIVATE, wxActivateEventHandler(CCManager::OnActivateApp));
}

void CCManager::OnActivateApp(wxActivateEvent& event)
{
    if (!event.GetActive())
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
        {
            cbStyledTextCtrl* stc = ed->GetControl();
            if (stc->CallTipActive())
                stc->CallTipCancel();
        }
    }
    event.Skip();
}

cbCodeCompletionPlugin* CCManager::GetProviderFor(cbEditor* ed)
{
    if (!ed)
        ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed == lastEditor)
        return lastCCPlugin;
    lastEditor = ed;
    lastCCPlugin = nullptr;
    const PluginsArray& pa = Manager::Get()->GetPluginManager()->GetCodeCompletionOffers();
    for (size_t i = 0; i < pa.GetCount(); ++i)
    {
        if (static_cast<cbCodeCompletionPlugin*>(pa[i])->IsProviderFor(ed))
        {
            lastCCPlugin = static_cast<cbCodeCompletionPlugin*>(pa[i]);
            break;
        }
    }
    return lastCCPlugin;
}

void CCManager::OnEditorTooltip(CodeBlocksEvent& event)
{
    event.Skip();

    if (wxGetKeyState(WXK_CONTROL))
        return;

    EditorBase* base = event.GetEditor();
    cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : nullptr;
    if (!ed || ed->IsContextMenuOpened())
        return;

    cbStyledTextCtrl* stc = ed->GetControl();
    if (stc->CallTipActive() && event.GetExtraLong() == 0)
        stc->CallTipCancel();

    cbCodeCompletionPlugin* ccPlugin = GetProviderFor(ed);
    if (!ccPlugin)
        return;

    int pos = stc->PositionFromPointClose(event.GetX(), event.GetY());
    if (pos < 0 || pos >= stc->GetLength())
        return;

    const wxArrayString& tips = ccPlugin->GetToolTips(pos, event.GetInt(), ed);
    if (tips.IsEmpty())
        return;

    int maxLines = stc->LinesOnScreen() * 2 / 3;
    if (maxLines < 5)
        maxLines = 5;
    size_t maxWidth = stc->GetSize().x / stc->TextWidth(wxSCI_STYLE_LINENUMBER, wxT("W")) - 6;
    if (maxWidth < 80)
        maxWidth = 80;
    wxString tip;
    int lineCount = 0;
    for (size_t i = 0; i < tips.GetCount() && lineCount < maxLines; ++i)
    {
        if (tips[i].Length() > maxWidth + 6)
        {
            wxString tipLn = tips[i];
            while (!tipLn.IsEmpty())
            {
                wxString segment = tipLn.Mid(0, maxWidth);
                int index = segment.Find(wxT(' '), true);
                if (index < 20)
                {
                    segment = tipLn.Mid(0, maxWidth * 6 / 5);
                    index = segment.Find(wxT(' '), true);
                }
                if (index < 20 || segment == tipLn)
                {
                    tip += tipLn + wxT("\n  ");
                    tipLn.Clear();
                }
                else
                {
                    tip += segment.Mid(0, index).Trim() + wxT("\n  ");
                    tipLn = tipLn.Mid(index).Trim(false);
                }
                ++lineCount;
            }
            tip.RemoveLast(2);
        }
        else
        {
            tip += tips[i] + wxT("\n");
            ++lineCount;
        }
    }
    tip.RemoveLast();
    stc->CallTipShow(stc->WordStartPosition(pos, true), tip);
    event.SetExtraLong(1);
}



