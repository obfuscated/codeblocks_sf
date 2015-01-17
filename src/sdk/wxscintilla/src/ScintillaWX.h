////////////////////////////////////////////////////////////////////////////
// Name:        ScintillaWX.h
// Purpose:     A wxWidgets implementation of Scintilla.  A class derived
//              from ScintillaBase that uses the "wx platform" defined in
//              PlatWX.cpp.  This class is one end of a bridge between
//              the wx world and the Scintilla world.  It needs a peer
//              object of type wxScintilla to function.
//
// Author:      Robin Dunn
//
// Created:     13-Jan-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __ScintillaWX_h__
#define __ScintillaWX_h__

//----------------------------------------------------------------------

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Platform.h"

#include "Scintilla.h"
#include "XPM.h"
#ifdef SCI_LEXER
#include "SciLexer.h"
#include "Accessor.h"
#endif
#include "ContractionState.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "AutoComplete.h"
#include "CharClassify.h"
#include "RESearch.h"
#include "Document.h"
#include "Editor.h"
#include "ScintillaBase.h"

#include <wx/dataobj.h>
#include <wx/dnd.h>
#include <wx/timer.h>

#ifdef __WXMSW__
    #include <windows.h>
    #include <wx/msw/wrapwin.h>
#endif // __WXMSW__

//----------------------------------------------------------------------

#ifdef WXMAKINGDLL_SCI
    #define WXDLLIMPEXP_SCI WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_SCI WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_SCI
#endif


#ifdef SCI_NAMESPACE
	#ifndef SCI_NAMESPACE_PREFIX
		#define SCI_NAMESPACE_PREFIX( x ) Scintilla::x
	#endif
#else
	#ifndef SCI_NAMESPACE_PREFIX
		#define SCI_NAMESPACE_PREFIX( x ) x
	#endif
#endif

//----------------------------------------------------------------------


class wxScintilla;           // forward
class ScintillaWX;


//----------------------------------------------------------------------
// Helper classes

#if wxUSE_DRAG_AND_DROP
class wxSCIDropTarget : public wxTextDropTarget {
public:
    void SetScintilla(ScintillaWX* swx) {
        m_swx = swx;
    }

    bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
    wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void OnLeave();

private:
    ScintillaWX* m_swx;
};
#endif

//----------------------------------------------------------------------

/* C::B begin */
class ScintillaWX : public SCI_NAMESPACE_PREFIX(ScintillaBase) {
/* C::B end */
public:

    ScintillaWX(wxScintilla* win);
    ~ScintillaWX();

    // base class virtuals
    virtual void Initialise();
    virtual void Finalise();
    virtual void StartDrag();
    virtual bool SetIdle(bool on);
/* C::B begin */
    // SetTicking function is deprecated since we implement fine grained timers
/* C::B end */
    virtual void SetMouseCapture(bool on);
    virtual bool HaveMouseCapture();
    virtual void ScrollText(int linesToMove);
    virtual void SetVerticalScrollPos();
    virtual void SetHorizontalScrollPos();
    virtual bool ModifyScrollBars(int nMax, int nPage);
    virtual void Copy();
    virtual void Paste();
/* C::B begin */
    virtual void CopyToClipboard(const SCI_NAMESPACE_PREFIX(SelectionText) &selectedText);

    virtual void CreateCallTipWindow(SCI_NAMESPACE_PREFIX(PRectangle) rc);
/* C::B end */
    virtual void AddToPopUp(const char *label, int cmd = 0, bool enabled = true);
    virtual void ClaimSelection();

    virtual sptr_t DefWndProc(unsigned int iMessage,
                              uptr_t wParam,
                              sptr_t lParam);
    virtual sptr_t WndProc(unsigned int iMessage,
                           uptr_t wParam,
                           sptr_t lParam);
/* C::B begin */
    static sptr_t DirectFunction(ScintillaWX *wxsci,
                                 unsigned int iMessage,
                                 uptr_t wParam,
                                 sptr_t lParam);
/* C::B end */

    virtual void NotifyChange();
    virtual void NotifyParent(SCI_NAMESPACE_PREFIX(SCNotification) scn);
/* C::B begin */
    virtual void NotifyFocus(bool focus);
/* C::B end */

/* C::B begin */
    wxTimer* timers[tickDwell+1];
    virtual bool FineTickerAvailable();
    virtual bool FineTickerRunning(TickReason reason);
    virtual void FineTickerStart(TickReason reason, int millis, int tolerance);
    virtual void FineTickerCancel(TickReason reason);
/* C::B end */

    virtual void CancelModes();

    virtual void UpdateSystemCaret();

    // Event delegates
    void DoPaint(wxDC* dc, wxRect rect);
    void DoHScroll(int type, int pos);
    void DoVScroll(int type, int pos);
    void DoSize(int width, int height);
    void DoLoseFocus();
    void DoGainFocus();
    void DoSysColourChange();
/* C::B begin */
    void DoLeftButtonDown(SCI_NAMESPACE_PREFIX(Point) pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
    void DoLeftButtonUp(SCI_NAMESPACE_PREFIX(Point) pt, unsigned int curTime, bool ctrl);
    void DoLeftButtonMove(SCI_NAMESPACE_PREFIX(Point) pt);
    void DoMiddleButtonUp(SCI_NAMESPACE_PREFIX(Point) pt);
#if !wxCHECK_VERSION(2,9,4)
    enum wxMouseWheelAxis
    {
        wxMOUSE_WHEEL_VERTICAL,
        wxMOUSE_WHEEL_HORIZONTAL
    };
#endif
/* C::B end */
    void DoMouseWheel(wxMouseWheelAxis axis, int rotation, int delta,
                      int linesPerAction, int columnsPerAction,
                      bool ctrlDown, bool isPageScroll);
    void DoAddChar(int key);
    int  DoKeyDown(const wxKeyEvent& event, bool* consumed);
    void DoTick() { Tick(); }
    void DoOnIdle(wxIdleEvent& evt);
    void DoOnTimer(wxTimerEvent& evt);

#if wxUSE_DRAG_AND_DROP
    bool DoDropText(long x, long y, const wxString& data);
    wxDragResult DoDragEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult DoDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void DoDragLeave();
#endif

    void DoCommand(int ID);
/* C::B begin */
    void DoContextMenu(SCI_NAMESPACE_PREFIX(Point) pt);
/* C::B ebd */
    void DoOnListBox();


    // helpers
    void FullPaint();
    void FullPaintDC(wxDC* dc);
    bool CanPaste();
    bool GetHideSelection() { return view.hideSelection; }
    void DoScrollToLine(int line);
    void DoScrollToColumn(int column);
/* C::B begin */
    void ClipChildren(wxDC& dc, SCI_NAMESPACE_PREFIX(PRectangle) rect);
/* C::B end */
    void SetUseAntiAliasing(bool useAA);
    bool GetUseAntiAliasing();

private:
    bool                capturedMouse;
    bool                focusEvent;
    wxScintilla*        sci;

#if wxUSE_DRAG_AND_DROP
    wxSCIDropTarget*    dropTarget;
    wxDragResult        dragResult;
/* C::B begin */
    bool                dragRectangle;
/* C::B end */
#endif

    int                 wheelVRotation;
    int                 wheelHRotation;

    // For use in creating a system caret
    bool HasCaretSizeChanged();
    bool CreateSystemCaret();
    bool DestroySystemCaret();
#ifdef __WXMSW__
    HBITMAP sysCaretBitmap;
    int sysCaretWidth;
    int sysCaretHeight;
#endif

    friend class wxSCICallTipContent;
};

//----------------------------------------------------------------------
#endif
