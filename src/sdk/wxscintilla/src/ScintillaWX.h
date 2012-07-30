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
#include "SVector.h"
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

#include <wx/wx.h>
#include <wx/dataobj.h>
#include <wx/dnd.h>

//----------------------------------------------------------------------

#ifdef WXMAKINGDLL_SCI
    #define WXDLLIMPEXP_SCI WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_SCI WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_SCI
#endif

class WXDLLIMPEXP_SCI wxScintilla;
class wxSCIDropTarget;

//----------------------------------------------------------------------


#ifdef SCI_NAMESPACE
	#ifndef SCI_NAMESPACE_PREFIX
		#define SCI_NAMESPACE_PREFIX( x ) Scintilla::x
	#endif
#else
	#ifndef SCI_NAMESPACE_PREFIX
		#define SCI_NAMESPACE_PREFIX( x ) x
	#endif
#endif

class ScintillaWX : public SCI_NAMESPACE_PREFIX(ScintillaBase) {
public:

    ScintillaWX(wxScintilla* win);
    virtual ~ScintillaWX();

    // base class virtuals
    virtual void Initialise();
    virtual void Finalise();
    virtual void ScrollText(int linesToMove);
    virtual void SetVerticalScrollPos();
    virtual void SetHorizontalScrollPos();
    virtual bool ModifyScrollBars(int nMax, int nPage);
    virtual void Copy();
    virtual void CopyToClipboard(const SCI_NAMESPACE_PREFIX(SelectionText) &selectedText);
    virtual void Paste();

    virtual void ClaimSelection();
    virtual void NotifyChange();

    virtual void NotifyFocus(bool focus);
    virtual void NotifyParent(SCI_NAMESPACE_PREFIX(SCNotification) scn);
    virtual void SetTicking(bool on);
    virtual bool SetIdle(bool on);
    virtual void SetMouseCapture(bool on);
    virtual bool HaveMouseCapture();
    virtual void StartDrag();
    virtual void CancelModes();

    virtual void UpdateSystemCaret();

    virtual void CreateCallTipWindow(SCI_NAMESPACE_PREFIX(PRectangle) rc);
    virtual void AddToPopUp(const char *label, int cmd = 0, bool enabled = true);
    virtual sptr_t WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
    virtual sptr_t DefWndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam);

    static sptr_t DirectFunction(ScintillaWX *wxsci,
        unsigned int iMessage, uptr_t wParam, sptr_t lParam);

    // Event delegates
    void DoPaint(wxDC* dc, wxRect rect);
    void DoHScroll(int type, int pos);
    void DoVScroll(int type, int pos);
    void DoSize(int width, int height);
    void DoLoseFocus();
    void DoGainFocus();
    void DoSysColourChange();
    void DoLeftButtonDown(SCI_NAMESPACE_PREFIX(Point) pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
    void DoLeftButtonUp(SCI_NAMESPACE_PREFIX(Point) pt, unsigned int curTime, bool ctrl);
    void DoLeftButtonMove(SCI_NAMESPACE_PREFIX(Point) pt);
    void DoMiddleButtonUp(SCI_NAMESPACE_PREFIX(Point) pt);
    void DoMouseWheel(int rotation, int delta, int linesPerAction, int ctrlDown, bool isPageScroll);
    void DoAddChar(int key);
    int  DoKeyDown(const wxKeyEvent& event, bool* consumed);
    void DoTick() { Tick(); }
    void DoOnIdle(wxIdleEvent& evt);

#if wxUSE_DRAG_AND_DROP
    bool DoDropText(long x, long y, const wxString& data);
    wxDragResult DoDragEnter(wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult DoDragOver(wxCoord x, wxCoord y, wxDragResult def);
    void DoDragLeave();
#endif

    void DoCommand(int ID);
    void DoContextMenu(SCI_NAMESPACE_PREFIX(Point) pt);
    void DoOnListBox();


    // helpers
    void FullPaint();
    bool CanPaste();
    bool GetHideSelection() { return hideSelection; }
    void DoScrollToLine(int line);
    void DoScrollToColumn(int column);
    void ClipChildren(wxDC& dc, SCI_NAMESPACE_PREFIX(PRectangle) rect);

private:
    bool                capturedMouse;
    bool                focusEvent;
    wxScintilla*        sci;

#if wxUSE_DRAG_AND_DROP
    wxSCIDropTarget*    dropTarget;
    wxDragResult        dragResult;
    bool                dragRectangle;
#endif

    int                 wheelRotation;

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

#endif
