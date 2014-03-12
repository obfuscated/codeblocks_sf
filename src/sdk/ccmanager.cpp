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
    Manager::Get()->RegisterEventSink(cbEVT_APP_DEACTIVATED,    new CCEvent(this, &CCManager::OnDeactivateApp));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new CCEvent(this, &CCManager::OnDeactivateEd));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_TOOLTIP,     new CCEvent(this, &CCManager::OnEditorTooltip));
}

// class destructor
CCManager::~CCManager()
{
    Manager::Get()->RemoveAllEventSinksFor(this);
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

void CCManager::OnDeactivateApp(CodeBlocksEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        if (stc->CallTipActive())
            stc->CallTipCancel();
    }
    event.Skip();
}

void CCManager::OnDeactivateEd(CodeBlocksEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        if (stc->CallTipActive())
            stc->CallTipCancel();
    }
    event.Skip();
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

    const wxStringVec& tips = ccPlugin->GetToolTips(pos, event.GetInt(), ed);
    if (tips.empty())
        return;

    int maxLines = stc->LinesOnScreen() * 2 / 3;
    if (maxLines < 5)
        maxLines = 5;
    int marginWidth = stc->GetMarginWidth(wxSCI_MARGIN_SYMBOL) + stc->GetMarginWidth(wxSCI_MARGIN_NUMBER);
    int maxWidth = (stc->GetSize().x - marginWidth) / stc->TextWidth(wxSCI_STYLE_LINENUMBER, wxT("W")) - 1;
    if (maxWidth < 60)
        maxWidth = 60;
    wxString tip;
    int lineCount = 0;
    for (size_t i = 0; i < tips.size() && lineCount < maxLines; ++i)
    {
        if (tips[i].Length() > (size_t)maxWidth + 6)
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
    tip.RemoveLast(); // trailing linefeed
    // try to show the tip at the start of the token, or at the margin if we are scrolled right
    // an offset of 2 helps deal with the width of the folding bar (TODO: does an actual calculation exist?)
    int offset = stc->PointFromPosition(stc->PositionFromLine(stc->LineFromPosition(pos))).x > marginWidth ? 0 : 2;
    stc->CallTipShow(std::max(stc->WordStartPosition(pos, true),
                              stc->PositionFromPoint(wxPoint(marginWidth, stc->PointFromPosition(pos).y)) + offset), tip);
    event.SetExtraLong(1);
}
