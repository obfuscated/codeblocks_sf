/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#include "ccmanager.h"

#include <wx/html/htmlwin.h>
#include <wx/display.h>

#include "cbstyledtextctrl.h"
#include "editor_hooks.h"

namespace CCManagerHelper
{
    inline void RipplePts(int& ptA, int& ptB, int len, int delta)
    {
        if (ptA > len - delta)
            ptA += delta;
        if (ptB > len - delta)
            ptB += delta;
    }

    // wxScintilla::FindColumn seems to be broken; re-implement:
    // Find the position of a column on a line taking into account tabs and
    // multi-byte characters. If beyond end of line, return line end position.
    int FindColumn(int line, int column, wxScintilla* stc)
    {
        int lnEnd = stc->GetLineEndPosition(line);
        for (int pos = stc->PositionFromLine(line); pos < lnEnd; ++pos)
        {
            if (stc->GetColumn(pos) == column)
                return pos;
        }
        return lnEnd;
    }

    bool IsPosVisible(int pos, wxScintilla* stc)
    {
        const int dist = stc->VisibleFromDocLine(stc->LineFromPosition(pos)) - stc->GetFirstVisibleLine();
        return !(dist < 0 || dist > stc->LinesOnScreen()); // caret is off screen
    }

    // (shamelessly stolen from mime handler plugin ;) )
    // build all HTML font sizes (1..7) from the given base size
    static void BuildFontSizes(int *sizes, int size)
    {
        // using a fixed factor (1.2, from CSS2) is a bad idea as explained at
        // http://www.w3.org/TR/CSS21/fonts.html#font-size-props but this is by far
        // simplest thing to do so still do it like this for now
        sizes[0] = int(size * 0.75); // exception to 1.2 rule, otherwise too small
        sizes[1] = int(size * 0.83);
        sizes[2] = size;
        sizes[3] = int(size * 1.2);
        sizes[4] = int(size * 1.44);
        sizes[5] = int(size * 1.73);
        sizes[6] = int(size * 2);
    }

    // (shamelessly stolen from mime handler plugin ;) )
    static int GetDefaultHTMLFontSize()
    {
        // base the default font size on the size of the default system font but
        // also ensure that we have a font of reasonable size, otherwise small HTML
        // fonts are unreadable
        int size = wxNORMAL_FONT->GetPointSize();
        if ( size < 9 )
            size = 9;
        return size;
    }
}

template<> CCManager* Mgr<CCManager>::instance = nullptr;
template<> bool Mgr<CCManager>::isShutdown = false;

const int idCallTipTimer = wxNewId();
const int idAutoLaunchTimer = wxNewId();
const int idAutocompSelectTimer = wxNewId();

// milliseconds
#define CALLTIP_REFRESH_DELAY 90
#define AUTOCOMP_SELECT_DELAY 35
#define SCROLL_REFRESH_DELAY 500

#define FROM_TIMER 1

enum ACLaunchState
{
    lsTknStart,
    lsCaretStart
};


//{ Unfocusable popup

// imported with small changes from PlatWX.cpp
class UnfocusablePopupWindow :
#if wxUSE_POPUPWIN
    public wxPopupWindow
#else
     public wxFrame
#endif // wxUSE_POPUPWIN
{
public:
#if wxUSE_POPUPWIN
    typedef wxPopupWindow BaseClass;

    UnfocusablePopupWindow(wxWindow* parent, int style = wxBORDER_NONE) :
        wxPopupWindow(parent, style)
#else
    typedef wxFrame BaseClass;

    UnfocusablePopupWindow(wxWindow* parent, int style = 0) :
        wxFrame(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                style | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT | wxNO_BORDER | wxFRAME_SHAPED
#ifdef __WXMAC__
                | wxPOPUP_WINDOW
#endif // __WXMAC__
                )
#endif // wxUSE_POPUPWIN
    {
        Hide();
    }

    bool Destroy();
    void OnFocus(wxFocusEvent& event);
    void ActivateParent();

    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
    virtual bool Show(bool show = true);

private:
    DECLARE_EVENT_TABLE()
};

// On OSX and (possibly others) there can still be pending
// messages/events for the list control when Scintilla wants to
// close it, so do a pending delete of it instead of destroying
// immediately.
bool UnfocusablePopupWindow::Destroy()
{
#ifdef __WXMAC__
    // The bottom edge of this window is not getting properly
    // refreshed upon deletion, so help it out...
    wxWindow* p = GetParent();
    wxRect r(GetPosition(), GetSize());
    r.SetHeight(r.GetHeight()+1);
    p->Refresh(false, &r);
#endif
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);
    return true;
}

void UnfocusablePopupWindow::OnFocus(wxFocusEvent& event)
{
    ActivateParent();
    GetParent()->SetFocus();
    event.Skip();
}

void UnfocusablePopupWindow::ActivateParent()
{
    // Although we're a frame, we always want the parent to be active, so
    // raise it whenever we get shown, focused, etc.
    wxTopLevelWindow *frame = wxDynamicCast(
        wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
    if (frame)
        frame->Raise();
}

void UnfocusablePopupWindow::DoSetSize(int x, int y,
                       int width, int height,
                       int sizeFlags)
{
    // convert coords to screen coords since we're a top-level window
    if (x != wxDefaultCoord)
        GetParent()->ClientToScreen(&x, NULL);

    if (y != wxDefaultCoord)
        GetParent()->ClientToScreen(NULL, &y);

    BaseClass::DoSetSize(x, y, width, height, sizeFlags);
}

bool UnfocusablePopupWindow::Show(bool show)
{
    bool rv = BaseClass::Show(show);
    if (rv && show)
        ActivateParent();
#ifdef __WXMAC__
    GetParent()->Refresh(false);
#endif
    return rv;
}

BEGIN_EVENT_TABLE(UnfocusablePopupWindow, UnfocusablePopupWindow::BaseClass)
    EVT_SET_FOCUS(UnfocusablePopupWindow::OnFocus)
END_EVENT_TABLE()

//} end Unfocusable popup


// class constructor
CCManager::CCManager() :
    m_AutocompPosition(wxSCI_INVALID_POSITION),
    m_CallTipActive(wxSCI_INVALID_POSITION),
    m_LastAutocompIndex(wxNOT_FOUND),
    m_LastTipPos(wxSCI_INVALID_POSITION),
    m_WindowBound(0),
    m_CallTipTimer(this, idCallTipTimer),
    m_AutoLaunchTimer(this, idAutoLaunchTimer),
    m_AutocompSelectTimer(this, idAutocompSelectTimer),
#ifdef __WXMSW__
    m_pAutocompPopup(nullptr),
#endif // __WXMSW__
    m_pLastEditor(nullptr),
    m_pLastCCPlugin(nullptr)
{
/* temporary filler */
    const wxString ctChars = wxT(",;\n()");
    m_CallTipChars.insert(ctChars.begin(), ctChars.end());
    const wxString alChars = wxT(".:<>\"#/");
    m_AutoLaunchChars.insert(alChars.begin(), alChars.end());
/* end temporary */
    m_LastACLaunchState[lsCaretStart] = wxSCI_INVALID_POSITION;
    m_pPopup = new UnfocusablePopupWindow(Manager::Get()->GetAppFrame());
    m_pHtml = new wxHtmlWindow(m_pPopup, wxID_ANY, wxDefaultPosition,
                               wxDefaultSize, wxHW_SCROLLBAR_AUTO | wxBORDER_SIMPLE);
    int sizes[7] = {};
    CCManagerHelper::BuildFontSizes(sizes, CCManagerHelper::GetDefaultHTMLFontSize());
    m_pHtml->SetFonts(wxEmptyString, wxEmptyString, &sizes[0]);
    m_pHtml->Connect(wxEVT_COMMAND_HTML_LINK_CLICKED,
                     wxHtmlLinkEventHandler(CCManager::OnHtmlLink), nullptr, this);

    typedef cbEventFunctor<CCManager, CodeBlocksEvent> CCEvent;
    Manager::Get()->RegisterEventSink(cbEVT_APP_DEACTIVATED,    new CCEvent(this, &CCManager::OnDeactivateApp));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new CCEvent(this, &CCManager::OnDeactivateEd));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN,        new CCEvent(this, &CCManager::OnEditorOpen));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE,       new CCEvent(this, &CCManager::OnEditorClose));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_TOOLTIP,     new CCEvent(this, &CCManager::OnEditorTooltip));
    Manager::Get()->RegisterEventSink(cbEVT_SHOW_CALL_TIP,      new CCEvent(this, &CCManager::OnShowCallTip));
    Manager::Get()->RegisterEventSink(cbEVT_COMPLETE_CODE,      new CCEvent(this, &CCManager::OnCompleteCode));
    m_EditorHookID = EditorHooks::RegisterHook(new EditorHooks::HookFunctor<CCManager>(this, &CCManager::OnEditorHook));
    Connect(idCallTipTimer,        wxEVT_TIMER, wxTimerEventHandler(CCManager::OnTimer));
    Connect(idAutoLaunchTimer,     wxEVT_TIMER, wxTimerEventHandler(CCManager::OnTimer));
    Connect(idAutocompSelectTimer, wxEVT_TIMER, wxTimerEventHandler(CCManager::OnTimer));
}

// class destructor
CCManager::~CCManager()
{
    m_pHtml->Disconnect(wxEVT_COMMAND_HTML_LINK_CLICKED,
                       wxHtmlLinkEventHandler(CCManager::OnHtmlLink), nullptr, this);
    m_pHtml->Destroy();
    m_pPopup->Destroy();
    Manager::Get()->RemoveAllEventSinksFor(this);
    EditorHooks::UnregisterHook(m_EditorHookID, true);
    Disconnect(idCallTipTimer);
    Disconnect(idAutoLaunchTimer);
    Disconnect(idAutocompSelectTimer);
}

cbCodeCompletionPlugin* CCManager::GetProviderFor(cbEditor* ed)
{
    if (!ed)
        ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed == m_pLastEditor)
        return m_pLastCCPlugin;
    m_pLastEditor = ed;
    m_pLastCCPlugin = nullptr;
    m_LastACLaunchState[lsCaretStart] = wxSCI_INVALID_POSITION;
    const PluginsArray& pa = Manager::Get()->GetPluginManager()->GetCodeCompletionOffers();
    for (size_t i = 0; i < pa.GetCount(); ++i)
    {
        cbCodeCompletionPlugin::CCProviderStatus status = static_cast<cbCodeCompletionPlugin*>(pa[i])->GetProviderStatusFor(ed);
        if (status == cbCodeCompletionPlugin::ccpsActive)
        {
            m_pLastCCPlugin = static_cast<cbCodeCompletionPlugin*>(pa[i]);
            break;
        }
        else if (status == cbCodeCompletionPlugin::ccpsUniversal)
            m_pLastCCPlugin = static_cast<cbCodeCompletionPlugin*>(pa[i]);
    }
    return m_pLastCCPlugin;
}

// priority, then alphabetical
struct TokenSorter
{
    bool& m_PureAlphabetical;

    TokenSorter(bool& alphabetical) : m_PureAlphabetical(alphabetical)
    {
        m_PureAlphabetical = true;
    }

    bool operator()(const cbCodeCompletionPlugin::CCToken& a, const cbCodeCompletionPlugin::CCToken& b)
    {
        int diff = a.weight - b.weight;
        if (diff == 0)
        {
            // cannot use CmpNoCase() because it compares lower case but Scintilla compares upper
            diff = a.displayName.Upper().Cmp(b.displayName.Upper());
            if (diff == 0)
                diff = a.displayName.Cmp(b.displayName);
        }
        else
            m_PureAlphabetical = false;
        return diff < 0;
    }
};

// cbEVT_COMPLETE_CODE
void CCManager::OnCompleteCode(CodeBlocksEvent& event)
{
    event.Skip();

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    cbCodeCompletionPlugin* ccPlugin = GetProviderFor(ed);
    if (!ccPlugin)
        return;

    cbStyledTextCtrl* stc = ed->GetControl();
    int tknEnd = stc->GetCurrentPos();
    if (tknEnd == m_LastACLaunchState[lsCaretStart] && !m_AutocompTokens.empty())
    {
        DoBufferedCC(stc);
        return;
    }
    int tknStart = stc->WordStartPosition(tknEnd, true);

    m_AutocompTokens = ccPlugin->GetAutocompList(event.GetInt() == FROM_TIMER,
                                                 ed, tknStart, tknEnd);
    if (m_AutocompTokens.empty())
        return;

    bool isPureAlphabetical = true;
    TokenSorter sortFunctor(isPureAlphabetical);
    std::sort(m_AutocompTokens.begin(), m_AutocompTokens.end(), sortFunctor);
    if (isPureAlphabetical)
        stc->AutoCSetOrder(wxSCI_ORDER_PRESORTED);
    else
        stc->AutoCSetOrder(wxSCI_ORDER_CUSTOM);
    wxString items;
    // experimentally, the average length per token seems to be 23 for the main CC plugin
    items.Alloc(m_AutocompTokens.size() * 20); // TODO: measure performance
    for (size_t i = 0; i < m_AutocompTokens.size(); ++i)
    {
        items += m_AutocompTokens[i].displayName;
        if (m_AutocompTokens[i].category == -1)
            items += wxT("\r");
        else
            items += F(wxT("\n%d\r"), m_AutocompTokens[i].category);
    }
    items.RemoveLast();

    if (!stc->CallTipActive() && !stc->AutoCompActive())
        m_CallTipActive = wxSCI_INVALID_POSITION;

    stc->AutoCompSetIgnoreCase(true);
    stc->AutoCompSetMaxHeight(14);
    stc->AutoCompSetTypeSeparator(wxT('\n'));
    stc->AutoCompSetSeparator(wxT('\r'));
    stc->AutoCompShow(tknEnd - tknStart, items);
    if (isPureAlphabetical)
    {
        const wxString& contextStr = stc->GetTextRange(tknStart, stc->WordEndPosition(tknEnd, true));
        std::vector<cbCodeCompletionPlugin::CCToken>::const_iterator tknIt
                = std::lower_bound(m_AutocompTokens.begin(), m_AutocompTokens.end(),
                                   cbCodeCompletionPlugin::CCToken(-1, contextStr),
                                   sortFunctor);
        if (tknIt != m_AutocompTokens.end() && tknIt->displayName.StartsWith(contextStr))
            stc->AutoCompSelect(tknIt->displayName);
    }
    m_LastACLaunchState[lsTknStart] = tknStart;
    m_LastACLaunchState[lsCaretStart] = tknEnd;
}

// cbEVT_APP_DEACTIVATED
void CCManager::OnDeactivateApp(CodeBlocksEvent& event)
{
    DoHidePopup();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        if (stc->CallTipActive())
            stc->CallTipCancel();
        m_CallTipActive = wxSCI_INVALID_POSITION;
    }
    event.Skip();
}

// cbEVT_EDITOR_DEACTIVATED
void CCManager::OnDeactivateEd(CodeBlocksEvent& event)
{
    DoHidePopup();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        if (stc->CallTipActive())
            stc->CallTipCancel();
        m_CallTipActive = wxSCI_INVALID_POSITION;
    }
    event.Skip();
}

// cbEVT_EDITOR_OPEN
void CCManager::OnEditorOpen(CodeBlocksEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());
    if (ed)
    {
        ed->GetControl()->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,
                                  wxListEventHandler(CCManager::OnAutocompleteSelect), nullptr, this);
    }
}

// cbEVT_EDITOR_CLOSE
void CCManager::OnEditorClose(CodeBlocksEvent& event)
{
    DoHidePopup();
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());
    if (ed == m_pLastEditor)
        m_pLastEditor = nullptr;
    if (ed && ed->GetControl())
    {
        // TODO: is this ever called?
        ed->GetControl()->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED,
                                     wxListEventHandler(CCManager::OnAutocompleteSelect), nullptr, this);
    }
}

// cbEVT_EDITOR_TOOLTIP
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
    cbCodeCompletionPlugin* ccPlugin = GetProviderFor(ed);
    int pos = stc->PositionFromPointClose(event.GetX(), event.GetY());
    if (!ccPlugin || pos < 0 || pos >= stc->GetLength())
    {
        if (stc->CallTipActive() && event.GetExtraLong() == 0 && m_CallTipActive == wxSCI_INVALID_POSITION)
            static_cast<wxScintilla*>(stc)->CallTipCancel();
        return;
    }

    int hlStart, hlEnd, argsPos;
    hlStart = hlEnd = argsPos = wxSCI_INVALID_POSITION;
    const std::vector<cbCodeCompletionPlugin::CCToken>& tokens = ccPlugin->GetTokenAt(pos, ed);
    std::set<wxString> uniqueTips;
    for (size_t i = 0; i < tokens.size(); ++i)
        uniqueTips.insert(tokens[i].displayName);
    wxStringVec tips(uniqueTips.begin(), uniqueTips.end());

    const int style = event.GetInt();
    if (!tips.empty())
    {
        const int tknStart = stc->WordStartPosition(pos, true);
        const int tknEnd   = stc->WordEndPosition(pos,   true);
        if (tknEnd - tknStart > 4)
        {
            hlStart = tips[0].Find(stc->GetTextRange(tknStart, tknEnd));
            if (hlStart != wxNOT_FOUND)
                hlEnd = hlStart + tknEnd - tknStart;
        }
    }
    else if (!(   stc->IsString(style)
               || stc->IsComment(style)
               || stc->IsCharacter(style)
               || stc->IsPreprocessor(style) ))
    {
        const int line = stc->LineFromPosition(pos);
        if (pos + 4 > stc->PositionFromLine(line) + (int)ed->GetLineIndentString(line).Length())
            tips = ccPlugin->GetCallTips(pos, style, ed, hlStart, hlEnd, argsPos);
    }
    if (tips.empty())
    {
        if (stc->CallTipActive() && event.GetExtraLong() == 0 && m_CallTipActive == wxSCI_INVALID_POSITION)
            static_cast<wxScintilla*>(stc)->CallTipCancel();
    }
    else
    {
        DoShowTips(tips, stc, pos, argsPos, hlStart, hlEnd);
        event.SetExtraLong(1);
    }
    m_CallTipActive = wxSCI_INVALID_POSITION;
}

void CCManager::OnEditorHook(cbEditor* ed, wxScintillaEvent& event)
{
    wxEventType evtType = event.GetEventType();
    if (evtType == wxEVT_SCI_CHARADDED)
    {
        const wxChar ch = event.GetKey();
        if (m_CallTipChars.find(ch) != m_CallTipChars.end())
        {
            CodeBlocksEvent evt(cbEVT_SHOW_CALL_TIP);
            Manager::Get()->ProcessEvent(evt);
        }
        else
        {
            cbStyledTextCtrl* stc = ed->GetControl();
            const int pos = stc->GetCurrentPos();
            const int wordStartPos = stc->WordStartPosition(pos, true);
            // TODO: read settings
            if (   (pos - wordStartPos >= 3 && !stc->AutoCompActive())
                || pos - wordStartPos == 3 + 4 )
            {
                CodeBlocksEvent evt(cbEVT_COMPLETE_CODE);
                Manager::Get()->ProcessEvent(evt);
            }
            else if (m_AutoLaunchChars.find(ch) != m_AutoLaunchChars.end())
            {
                m_AutoLaunchTimer.Start(10, wxTIMER_ONE_SHOT);
                m_AutocompPosition = pos;
            }
        }
    }
    else if (evtType == wxEVT_SCI_UPDATEUI)
    {
        if (event.GetUpdated() & (wxSCI_UPDATE_V_SCROLL|wxSCI_UPDATE_H_SCROLL))
        {
            cbStyledTextCtrl* stc = ed->GetControl();
            if (stc->CallTipActive())
            {
                static_cast<wxScintilla*>(stc)->CallTipCancel();
                if (m_CallTipActive != wxSCI_INVALID_POSITION && CCManagerHelper::IsPosVisible(m_CallTipActive, stc))
                    m_CallTipTimer.Start(SCROLL_REFRESH_DELAY, wxTIMER_ONE_SHOT);
            }
            else if (m_CallTipTimer.IsRunning())
            {
                if (CCManagerHelper::IsPosVisible(stc->GetCurrentPos(), stc))
                    m_CallTipTimer.Start(SCROLL_REFRESH_DELAY, wxTIMER_ONE_SHOT);
                else
                {
                    m_CallTipTimer.Stop();
                    m_CallTipActive = wxSCI_INVALID_POSITION;
                }
            }
            if (m_AutoLaunchTimer.IsRunning())
            {
                if (CCManagerHelper::IsPosVisible(stc->GetCurrentPos(), stc))
                    m_AutoLaunchTimer.Start(SCROLL_REFRESH_DELAY, wxTIMER_ONE_SHOT);
                else
                    m_AutoLaunchTimer.Stop();
            }
            else if (stc->AutoCompActive())
            {
                stc->AutoCompCancel();
                m_AutocompPosition = stc->GetCurrentPos();
                if (CCManagerHelper::IsPosVisible(m_AutocompPosition, stc))
                    m_AutoLaunchTimer.Start(SCROLL_REFRESH_DELAY, wxTIMER_ONE_SHOT);
            }
        }
    }
    else if (evtType == wxEVT_SCI_KEY)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        switch (event.GetKey())
        {
            case wxSCI_KEY_LEFT:
            case wxSCI_KEY_RIGHT:
                if (!stc->CallTipActive() && !stc->AutoCompActive())
                    m_CallTipActive = wxSCI_INVALID_POSITION;
                // fall through
            case wxSCI_KEY_UP:
            case wxSCI_KEY_DOWN:
                if (m_CallTipActive != wxSCI_INVALID_POSITION && !stc->AutoCompActive())
                    m_CallTipTimer.Start(CALLTIP_REFRESH_DELAY, wxTIMER_ONE_SHOT);
                break;

            default:
                break;
        }
    }
    else if (evtType == wxEVT_SCI_MODIFIED)
    {
        if (event.GetModificationType() & wxSCI_PERFORMED_UNDO)
        {
            cbStyledTextCtrl* stc = ed->GetControl();
            if (m_CallTipActive != wxSCI_INVALID_POSITION && stc->GetCurrentPos() >= m_CallTipActive)
                m_CallTipTimer.Start(CALLTIP_REFRESH_DELAY, wxTIMER_ONE_SHOT);
            else
                static_cast<wxScintilla*>(stc)->CallTipCancel();
        }
    }
    else if (evtType == wxEVT_SCI_AUTOCOMP_SELECTION)
    {
        DoHidePopup();
        cbCodeCompletionPlugin* ccPlugin = GetProviderFor(ed);
        if (ccPlugin)
        {
            if (   m_LastAutocompIndex != wxNOT_FOUND
                && m_LastAutocompIndex < (int)m_AutocompTokens.size() )
            {
                ccPlugin->DoAutocomplete(m_AutocompTokens[m_LastAutocompIndex], ed);
            }
            else
            {
                ccPlugin->DoAutocomplete(event.GetText(), ed);
            }
        }
    }
    else if (evtType == wxEVT_SCI_AUTOCOMP_CANCELLED)
        DoHidePopup();
    event.Skip();
}

// cbEVT_SHOW_CALL_TIP
void CCManager::OnShowCallTip(CodeBlocksEvent& event)
{
    event.Skip();

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    cbCodeCompletionPlugin* ccPlugin = GetProviderFor(ed);
    if (!ccPlugin)
        return;

    cbStyledTextCtrl* stc = ed->GetControl();
    int pos = stc->GetCurrentPos();
    int hlStart, hlEnd, argsPos;
    hlStart = hlEnd = argsPos = wxSCI_INVALID_POSITION;
    const wxStringVec& tips = ccPlugin->GetCallTips(pos, stc->GetStyleAt(pos), ed, hlStart, hlEnd, argsPos);
    if (!tips.empty() && (event.GetInt() != FROM_TIMER || argsPos == m_CallTipActive))
    {
        int lnStart = stc->PositionFromLine(stc->LineFromPosition(pos));
        while (wxIsspace(stc->GetCharAt(lnStart)))
            ++lnStart; // do not show too far left on multi-line call tips
        DoShowTips(tips, stc, std::max(pos, lnStart), argsPos, hlStart, hlEnd);
        m_CallTipActive = argsPos;
    }
    else if (m_CallTipActive != wxSCI_INVALID_POSITION)
    {
        static_cast<wxScintilla*>(stc)->CallTipCancel();
        m_CallTipActive = wxSCI_INVALID_POSITION;
    }
}

void CCManager::OnAutocompleteSelect(wxListEvent& event)
{
    event.Skip();
    m_AutocompSelectTimer.Start(AUTOCOMP_SELECT_DELAY, wxTIMER_ONE_SHOT);
    wxObject* evtObj = event.GetEventObject();
#ifdef __WXMSW__
    m_pAutocompPopup = static_cast<wxListView*>(evtObj);
#endif // __WXMSW__
    if (!evtObj)
        return;
    wxWindow* evtWin = static_cast<wxWindow*>(evtObj)->GetParent();
    if (!evtWin)
        return;
    m_DocPos = m_pPopup->GetParent()->ScreenToClient(evtWin->GetScreenPosition());
    m_DocPos.x += evtWin->GetSize().x;
    if (!m_pPopup->IsShown())
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        cbStyledTextCtrl* stc = ed->GetControl();
        int acMaxHeight = stc->AutoCompGetMaxHeight() + 1;
        int textHeight = stc->TextHeight(stc->GetCurrentLine());
        wxRect edRect = ed->GetRect();
        m_DocSize.x = edRect.width * 5 / 12;
        m_DocSize.y = acMaxHeight*textHeight;
        evtWin->Connect(wxEVT_SHOW, wxShowEventHandler(CCManager::OnAutocompleteHide), nullptr, this);

        const int idx = wxDisplay::GetFromWindow(evtWin);
        m_WindowBound = m_DocPos.x + m_DocSize.x;
        if (idx != wxNOT_FOUND)
        {
            const wxPoint& corner = m_pPopup->GetParent()->ScreenToClient(wxDisplay(idx).GetGeometry().GetBottomRight());
            m_DocSize.y = std::max(8 * textHeight,      std::min(m_DocSize.y, corner.y - m_DocPos.y - 2));
            m_DocSize.x = std::max(m_DocSize.y * 2 / 3, std::min(m_DocSize.x, corner.x - m_DocPos.x - 2));
            m_WindowBound = corner.x;
        }
    }
    if ((m_DocPos.x + m_DocSize.x) > m_WindowBound)
        m_DocPos.x -= evtWin->GetSize().x + m_DocSize.x; // show to the left instead
}

// Note: according to documentation, this event is only available under wxMSW, wxGTK, and wxOS2
void CCManager::OnAutocompleteHide(wxShowEvent& event)
{
    event.Skip();
    DoHidePopup();
    wxObject* evtObj = event.GetEventObject();
    if (evtObj)
        static_cast<wxWindow*>(evtObj)->Disconnect(wxEVT_SHOW, wxShowEventHandler(CCManager::OnAutocompleteHide), nullptr, this);
    if (m_CallTipActive != wxSCI_INVALID_POSITION && !m_AutoLaunchTimer.IsRunning())
        m_CallTipTimer.Start(CALLTIP_REFRESH_DELAY, wxTIMER_ONE_SHOT);
}

#ifdef __WXMSW__
void CCManager::OnPopupScroll(wxMouseEvent& event)
{
    const wxPoint& pos = m_pLastEditor->GetControl()->ClientToScreen(event.GetPosition());
    if (m_pPopup->GetScreenRect().Contains(pos))
        m_pHtml->GetEventHandler()->ProcessEvent(event);
    else if (m_pAutocompPopup && m_pAutocompPopup->GetScreenRect().Contains(pos))
        m_pAutocompPopup->ScrollList(0, event.GetWheelRotation() / -4); // TODO: magic number... can we hook to the actual event?
    else
        event.Skip();
}
#endif // __WXMSW__

void CCManager::OnHtmlLink(wxHtmlLinkEvent& event)
{
    cbCodeCompletionPlugin* ccPlugin = GetProviderFor();
    if (!ccPlugin)
        return;

    bool dismissPopup = false;
    const wxString& html = ccPlugin->OnDocumentationLink(event, dismissPopup);
    if (dismissPopup)
        DoHidePopup();
    else if (!html.IsEmpty())
        m_pHtml->SetPage(html);
    // plugins are responsible to skip this event (if they choose to)
}

void CCManager::OnTimer(wxTimerEvent& event)
{
    if (event.GetId() == idCallTipTimer) // m_CallTipTimer
    {
        CodeBlocksEvent evt(cbEVT_SHOW_CALL_TIP);
        evt.SetInt(FROM_TIMER);
        Manager::Get()->ProcessEvent(evt);
    }
    else if (event.GetId() == idAutoLaunchTimer) // m_AutoLaunchTimer
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed && ed->GetControl()->GetCurrentPos() == m_AutocompPosition)
        {
            CodeBlocksEvent evt(cbEVT_COMPLETE_CODE);
            evt.SetInt(FROM_TIMER);
            Manager::Get()->ProcessEvent(evt);
        }
        m_AutocompPosition = wxSCI_INVALID_POSITION;
    }
    else if (event.GetId() == idAutocompSelectTimer) // m_AutocompSelectTimer
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
        {
            cbStyledTextCtrl* stc = ed->GetControl();
            if (stc->AutoCompActive())
            {
                m_LastAutocompIndex = stc->AutoCompGetCurrent();
                DoShowDocumentation(ed);
            }
        }
    }
    else // ?!
        event.Skip();
}

void CCManager::DoBufferedCC(cbStyledTextCtrl* stc)
{
    //stc->AutoCompPosStart()
    if (stc->AutoCompActive())
        return;
    wxString items;
    items.Alloc(m_AutocompTokens.size() * 20);
    for (size_t i = 0; i < m_AutocompTokens.size(); ++i)
    {
        items += m_AutocompTokens[i].displayName;
        if (m_AutocompTokens[i].category == -1)
            items += wxT("\r");
        else
            items += F(wxT("\n%d\r"), m_AutocompTokens[i].category);
    }
    items.RemoveLast();
    if (!stc->CallTipActive())
        m_CallTipActive = wxSCI_INVALID_POSITION;
    stc->AutoCompShow(m_LastACLaunchState[lsCaretStart] - m_LastACLaunchState[lsTknStart], items);
    if (   m_LastAutocompIndex != wxNOT_FOUND
        && m_LastAutocompIndex < (int)m_AutocompTokens.size() )
    {
        const cbCodeCompletionPlugin::CCToken& token = m_AutocompTokens[m_LastAutocompIndex];
        const int sepIdx = token.displayName.Find('\n', true);
        if (sepIdx == wxNOT_FOUND)
            stc->AutoCompSelect(token.displayName);
        else
            stc->AutoCompSelect(token.displayName.Mid(0, sepIdx));
    }
}

void CCManager::DoHidePopup()
{
    if (!m_pPopup->IsShown())
        return;
    m_pPopup->Hide();
#ifdef __WXMSW__
    if (m_pLastEditor && m_pLastEditor->GetControl())
        m_pLastEditor->GetControl()->Disconnect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(CCManager::OnPopupScroll), nullptr, this);
#endif // __WXMSW__
}

void CCManager::DoShowDocumentation(cbEditor* ed)
{
    cbCodeCompletionPlugin* ccPlugin = GetProviderFor(ed);
    if (!ccPlugin)
        return;
    if (   m_LastAutocompIndex == wxNOT_FOUND
        || m_LastAutocompIndex >= (int)m_AutocompTokens.size() )
    {
        return;
    }
    const wxString& html = ccPlugin->GetDocumentation(m_AutocompTokens[m_LastAutocompIndex]);
    if (html.IsEmpty())
    {
        DoHidePopup();
        return;
    }

    m_pPopup->Freeze();
    m_pHtml->SetSize(m_DocSize);
    m_pHtml->SetPage(html);
    m_pPopup->SetClientSize(m_DocSize);
    m_pPopup->SetPosition(m_DocPos);
    m_pPopup->Thaw();
    if (!m_pPopup->IsShown())
    {
        m_pPopup->Show();
#ifdef __WXMSW__
        ed->GetControl()->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(CCManager::OnPopupScroll), nullptr, this);
#endif // __WXMSW__
    }
}

void CCManager::DoShowTips(const wxStringVec& tips, cbStyledTextCtrl* stc, int pos, int argsPos, int hlStart, int hlEnd)
{
    int maxLines = std::max(stc->LinesOnScreen() / 4, 5);
    int marginWidth = stc->GetMarginWidth(wxSCI_MARGIN_SYMBOL) + stc->GetMarginWidth(wxSCI_MARGIN_NUMBER);
    int maxWidth = (stc->GetSize().x - marginWidth) / stc->TextWidth(wxSCI_STYLE_LINENUMBER, wxT("W")) - 1;
    maxWidth = std::min(std::max(60, maxWidth), 135);
    wxString tip;
    int lineCount = 0;

    for (size_t i = 0; i < tips.size() && lineCount < maxLines; ++i)
    {
        if (tips[i].Length() > (size_t)maxWidth + 6) // line is too long, try breaking it
        {
            wxString tipLn = tips[i];
            while (!tipLn.IsEmpty())
            {
                wxString segment = tipLn.Mid(0, maxWidth);
                int index = segment.Find(wxT(' '), true); // break on a space
                if (index < 20) // no reasonable break?
                {
                    segment = tipLn.Mid(0, maxWidth * 6 / 5); // increase search width a bit
                    index = segment.Find(wxT(' '), true);
                }
                for (int commaIdx = index - 1; commaIdx > maxWidth / 2; --commaIdx) // check back for a comma
                {
                    if (segment[commaIdx] == wxT(',') && segment[commaIdx + 1] == wxT(' '))
                    {
                        index = commaIdx + 1; // prefer splitting on a comma, if that does not set us back too far
                        break;
                    }
                }
                if (index < 20 || segment == tipLn) // end of string, or cannot split
                {
                    tip += tipLn + wxT("\n");
                    CCManagerHelper::RipplePts(hlStart, hlEnd, tip.Length(), 1);
                    tipLn.Clear();
                }
                else // continue splitting
                {
                    tip += segment.Mid(0, index) + wxT("\n ");
                    CCManagerHelper::RipplePts(hlStart, hlEnd, tip.Length(), 2);
                    // already starts with a space, so all subsequent lines are prefixed by two spaces
                    tipLn = tipLn.Mid(index);
                }
                ++lineCount;
            }
        }
        else // just add the line
        {
            tip += tips[i] + wxT("\n");
            CCManagerHelper::RipplePts(hlStart, hlEnd, tip.Length(), 1);
            ++lineCount;
        }
    }
    tip.RemoveLast(); // trailing linefeed

    // try to show the tip at the start of the token/arguments, or at the margin if we are scrolled right
    // an offset of 2 helps deal with the width of the folding bar (TODO: does an actual calculation exist?)
    int line = stc->LineFromPosition(pos);
    if (argsPos == wxSCI_INVALID_POSITION)
        argsPos = stc->WordStartPosition(pos, true);
    else
        argsPos = std::min(CCManagerHelper::FindColumn(line, stc->GetColumn(argsPos), stc), stc->WordStartPosition(pos, true));
    int offset = stc->PointFromPosition(stc->PositionFromLine(line)).x > marginWidth ? 0 : 2;
    pos = std::max(argsPos, stc->PositionFromPoint(wxPoint(marginWidth, stc->PointFromPosition(pos).y)) + offset);
    pos = std::min(pos, stc->GetLineEndPosition(line)); // do not go to next line
    if (stc->CallTipActive() && m_LastTipPos != pos)
        stc->CallTipCancel(); // force tip popup to invalidate (sometimes fails to otherwise do so on Windows)
    stc->CallTipShow(pos, tip);
    if (hlStart >= 0 && hlEnd > hlStart)
        stc->CallTipSetHighlight(hlStart, hlEnd);
    m_LastTipPos = pos;
}
