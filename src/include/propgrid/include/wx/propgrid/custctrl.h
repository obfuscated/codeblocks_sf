/////////////////////////////////////////////////////////////////////////////
// Name:        custctrl.h
// Purpose:     wxCustomControls (v1.0.8)
// Author:      Jaakko Salli
// Modified by:
// Created:     Oct-24-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_CUSTCTRL_H__
#define __WX_CUSTCTRL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "custctrl.cpp"
#endif

// Doxygen special
#ifndef _WX_WINDOW_H_BASE_
# include "cc_dox_mainpage.h"
#endif

// -----------------------------------------------------------------------

#include "wx/caret.h"

#include "wx/renderer.h"

// -----------------------------------------------------------------------

#if defined(WXMAKINGDLL_CUSTCTRL) || defined(WXMAKINGDLL_PROPGRID)
    #define WXDLLIMPEXP_CC WXEXPORT
//#elif defined(WXUSINGDLL)
//    #define WXDLLIMPEXP_CC WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_CC
#endif

// If given in button renderer flags, then popup arrow
// is drawn on it as well.
#define wxCONTROL_POPUP_ARROW       wxCONTROL_CHECKED

// -----------------------------------------------------------------------

//
// Here are some platform dependent defines
// (more in custctrl.cpp)
//

#if defined(__WXMSW__)
    // tested

#if wxUSE_UXTHEME
# include "wx/msw/uxtheme.h"
#endif

    #define wxCC_CUSTOM_IMAGE_MARGIN1       2  // before image
    #define wxCC_CUSTOM_IMAGE_MARGIN2       7  // after image

    #define wxCC_TEXTCTRL_YSPACING          2
    #define wxCC_TEXTCTRL_XSPACING          3

    #define wxCC_USE_POPUPWIN               1  // 1 if wxPopupWindow can be used.

#elif defined(__WXGTK__)
    // tested

    #define wxCC_CUSTOM_IMAGE_MARGIN1       2  // before image
    #define wxCC_CUSTOM_IMAGE_MARGIN2       7  // after image

    #define wxCC_TEXTCTRL_YSPACING          2
    #define wxCC_TEXTCTRL_XSPACING          3

    // Disabled because no proper border support.
    #define wxCC_USE_POPUPWIN               0  // 1 if wxPopupWindow can be used.

#elif defined(__WXMAC__)
    // *not* tested

    #define wxCC_CUSTOM_IMAGE_MARGIN1       2  // before image
    #define wxCC_CUSTOM_IMAGE_MARGIN2       7  // after image

    #define wxCC_TEXTCTRL_YSPACING          2
    #define wxCC_TEXTCTRL_XSPACING          3

    #define wxCC_USE_POPUPWIN               1  // 1 if wxPopupWindow can be used.

#else
    // defaults

    #define wxCC_CUSTOM_IMAGE_MARGIN1       2  // before image
    #define wxCC_CUSTOM_IMAGE_MARGIN2       7  // after image

    #define wxCC_TEXTCTRL_YSPACING          2
    #define wxCC_TEXTCTRL_XSPACING          3

    #define wxCC_USE_POPUPWIN               0  // 1 if wxPopupWindow can be used.

#endif

// Conform to wxUSE_POPUPWIN
#if !wxUSE_POPUPWIN
# undef wxCC_USE_POPUPWIN
# define wxCC_USE_POPUPWIN                  0
#endif

/** If 1, then controls will be moved according to scrolling
    (does not work exactly, so 0 is recommended value).
*/
#define wxCC_CORRECT_CONTROL_POSITION     0

#if !wxPG_USE_CUSTOM_CONTROLS
# undef wxCC_CUSTOM_IMAGE_MARGIN1
# undef wxCC_CUSTOM_IMAGE_MARGIN2
#endif

#if wxPG_USE_CUSTOM_CONTROLS

// -----------------------------------------------------------------------

#ifndef SWIG
class WXDLLIMPEXP_CC wxCustomControl;
class WXDLLIMPEXP_CC wxCCustomTextCtrl;
class WXDLLIMPEXP_CC wxCCustomButton;
class WXDLLIMPEXP_CC wxCCustomComboBox;
class WXDLLIMPEXP_CC wxCCustomComboBoxHandler;
class WXDLLIMPEXP_CC wxCustomControlManager;
#endif

// -----------------------------------------------------------------------

/** Each wxCustomControl uses 1+ wxCustomControlHandler.
*/
class WXDLLIMPEXP_CC wxCustomControlHandler
{
public:

    inline void SetControl ( wxCustomControl* pctrl )
    {
        m_control = pctrl;
        m_flags = 0;
    }
    inline wxCustomControl* GetControl() const { return m_control; }
    inline bool IsMouseFocused() const;
    inline void Move ( int x, int y ) { m_rect.x = x; m_rect.y = y; }
    inline void SetSize ( int width, int height ) { m_rect.width = width; m_rect.height = height; }

    inline void SetSize ( const wxPoint& pos, const wxSize& sz )
    {
        m_rect.x = pos.x; m_rect.y = pos.y;
        m_rect.width = sz.x; m_rect.height = sz.y;
    }

    inline const wxRect& GetRect() const { return m_rect; }

    inline void ClearFlag( long flag ) { m_flags &= ~(flag); }
    inline void SetFlag( long flag ) { m_flags |= flag; }

    void Create ( wxCustomControl* pctrl, const wxPoint& pos, const wxSize& sz );

protected:
    wxCustomControl*        m_control;
    wxRect                  m_rect;
    long                    m_flags; // barely needed
};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_CC wxCustomTextCtrlHandler : public wxCustomControlHandler
{
public:
    
    void Create ( wxCustomControl* pctrl, const wxPoint& pos, const wxSize& sz,
        const wxString& value );

    void Draw ( wxDC& dc, const wxRect& rect );

    bool OnKeyEvent ( wxKeyEvent& event );
    
    bool OnMouseEvent ( wxMouseEvent& event );

    int HitTest ( wxCoord x, int* pCol );

    bool SetInsertionPoint ( long pos, long first_visible );

    bool SetSelection ( long from, long to );

    void SetValue ( const wxString& value );

    inline int GetPosition () const
    {
        return m_position;
    }

    inline const wxString& GetValue() const { return m_text; }

    // like DEL key was pressed
    void DeleteSelection ();

    // wxCC_FL_MODIFIED

protected:

    wxString        m_text;
    //wxString        m_textAtPos; // text that begins at position
    unsigned int    m_position;
    unsigned int    m_scrollPosition;
    int             m_selStart;
    int             m_selEnd;
    int             m_itemButDown; // dragging centers around this

    wxArrayInt      m_arrExtents; // cached array of text extents

    int UpdateExtentCache ( wxString& tempstr, size_t index );

};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_CC wxCCustomButtonHandler : public wxCustomControlHandler
{
    friend class wxCCustomButton;
public:
    
    void Draw ( wxDC& dc, const wxRect& rect );

    bool OnMouseEvent ( wxMouseEvent& event );

    inline void SetButtonState ( int state ) { m_down = (unsigned char)state; }

protected:
    wxString        m_label;
    unsigned char   m_down; // 0 means button is up

};

// -----------------------------------------------------------------------

#if wxCC_USE_POPUPWIN
# include "wx/popupwin.h"
# define wxCustomComboPopupBase      wxPopupWindow
#else
# define wxCustomComboPopupBase      wxWindow
#endif

class WXDLLIMPEXP_CC wxCustomComboItem
{
public:
    wxCustomComboItem();
    virtual ~wxCustomComboItem();
protected:
};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_CC wxCustomComboPopup : public wxCustomComboPopupBase
{
    friend class wxCustomControlManager;
public:
    wxCustomComboPopup ();
    virtual ~wxCustomComboPopup();

    bool Create ( wxWindow* frame, wxCCustomComboBoxHandler* data,
        const wxRect& ctrl_rect, wxCustomControl* ctrl,
        const wxSize& size, int sizealign );

/*#if wxCC_USE_POPUPWIN
    inline wxScrolledWindow* GetWindow() const { return m_subWindow; };
#else
    inline wxScrolledWindow* GetWindow() { return this; };
#endif*/

    // kbscroll allows forcing to scroll one item at a time.
    virtual void ShowItem ( const wxCustomComboItem& item, bool kbscroll = FALSE ) = 0;
    virtual void OnKeyEvent ( wxKeyEvent& event ) = 0;
    //virtual void SetSelection ( const wxCustomComboItem& item ) = 0;

    void ForcedClose ();

    inline wxCustomControl* GetControl () const { return m_control; }

    // Event handlers.
    void OnMouseEntry( wxMouseEvent& event );

protected:
    wxCustomControl*            m_control;
    wxCustomControlManager*     m_manager;
    wxCCustomComboBoxHandler*   m_chData;

    wxArrayPtrVoid              m_labels; // holds pointers to labels

    int                         m_wheelSum;

    unsigned char               m_orientation;

    //unsigned char               m_entryStatus;
private:
    DECLARE_EVENT_TABLE()
};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_CC wxCustomComboListItem : public wxCustomComboItem
{
public:
    wxCustomComboListItem();
    wxCustomComboListItem( int index ) { m_index = index; }
    virtual ~wxCustomComboListItem();

    int m_index;

protected:
};

// -----------------------------------------------------------------------

#include "wx/scrolbar.h"

class WXDLLIMPEXP_CC wxComboPopupDefaultList : public wxCustomComboPopup
{
public:
    wxComboPopupDefaultList ( wxWindow* frame, wxCCustomComboBoxHandler* data,
        const wxRect& rect, wxCustomControl* ctrl );
    virtual ~wxComboPopupDefaultList();

    virtual void ShowItem ( const wxCustomComboItem& item, bool kbscroll = FALSE );

    inline void ShowItem ( int index, bool kbscroll = FALSE )
    {
        wxCustomComboListItem item(index);
        ShowItem (item,kbscroll);
    }

    //virtual void SetSelection ( const wxCustomComboItem& item );

    virtual void OnKeyEvent ( wxKeyEvent& event );

    int HitTest ( int y );

    inline void DrawItem ( int index )
    {
        wxCustomComboListItem item(index);
        DrawItem(item);
    }
    void DrawItem ( const wxCustomComboItem& item );
    void DrawItem ( wxDC& dc, wxRect& r, unsigned int index );

    void OnMouseWheelEvent ( wxMouseEvent& event );

    void OnPaint ( wxPaintEvent& event );

    void OnMouseDown ( wxMouseEvent& event );
    void OnMouseMove ( wxMouseEvent& event );
    void OnScrollEvent ( wxScrollEvent& event );

    void SetViewStart ( int index, bool adjust_sb );
    void RecheckHilighted ( int y );

    //void OnMouseUp ( wxMouseEvent& event );

    //void SetImagePaintFunction ( wxCustomPaintFunc paintfunc ) { m_paintFunc = paintfunc; }

protected:

    int             m_hilighted;
    int             m_itemHeight;

    // Scrolling related.
    wxScrollBar*    m_pScrollBar;
    int             m_clientWidth;
    int             m_viewStartY;
    int             m_viewStartIndex;
    int             m_sbWidth;

private:
    DECLARE_EVENT_TABLE()
};

// -----------------------------------------------------------------------

typedef wxSize (*wxCustomPaintFunc) ( wxDC&, const wxRect&, int, void* );

class WXDLLIMPEXP_CC wxCustomComboPopup;
class WXDLLIMPEXP_CC wxCustomComboItem;
class WXDLLIMPEXP_CC wxComboPopupDefaultList;

class WXDLLIMPEXP_CC wxCCustomComboBoxHandler : public wxCustomControlHandler
{
    friend class wxCCustomComboBox;
    friend class wxCustomComboPopup;
public:
    void Create ( wxCustomControl* pctrl, const wxString& value,
        const wxPoint& pos, const wxSize& sz );

    inline void SetControl ( wxCustomControl* pctrl )
    {
        wxCustomControlHandler::SetControl ( pctrl );
        m_btData.SetControl ( pctrl );
    }

    virtual void SetSelection ( const wxCustomComboItem& item ) = 0;

    void Draw ( wxDC& dc, const wxRect& rect, bool item_too );

    inline wxCustomComboPopup* GetPopupInstance () const
    {
        return m_listInstance;
    }

    virtual void OnSelect ( const wxCustomComboItem& item ) = 0;

    inline void IntOnSelect ( int index )
    {
        wxCustomComboListItem item(index);
        OnSelect (item);
    }

    virtual bool OnKeyEvent ( wxKeyEvent& event ) = 0;

    virtual wxCustomComboPopup* CreatePopup ( wxWindow* frame,
        const wxRect& ctrl_rect, wxCustomControl* ctrl ) = 0;

    virtual bool OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata );

    inline void Move ( int x, int y )
    {
        m_rect.x = x; m_rect.y = y;
        m_btData.Move ( x + m_rect.width - m_buttonWidth, y );
    }
    inline void SetSize ( int width, int height )
    {
        m_rect.width = width - m_buttonWidth;
        m_rect.height = height;
        m_btData.Move ( m_rect.x + width - m_buttonWidth, m_rect.y );
        m_btData.SetSize ( m_buttonWidth, height );
    }

    //inline const wxArrayPtrVoid& GetLabels() const { return m_labels; }
    inline wxCustomPaintFunc GetPaintFunc() const { return m_paintfunc; }
    inline void* GetPaintFuncCustomData() const { return m_paintfunc_customdata; }
    inline const wxSize& GetImageSize() const { return m_imageSize; }
    //inline int GetSelection() const { return m_selection; }

    inline void SetValue( const wxString& text ) { m_text = text; }
    inline const wxString& GetValue() const { return m_text; }

    inline wxCCustomButtonHandler* GetButtonData() { return &m_btData; }

    virtual ~wxCCustomComboBoxHandler();

protected:

    //int                     m_selection;

    int                     m_buttonWidth;

    wxSize                  m_imageSize; // size of custom image in the list

    wxString                m_text; // text currently shown

    //wxArrayPtrVoid          m_labels; // holds pointers to labels

    wxCCustomButtonHandler      m_btData;

    //wxCustomComboItem*      m_pItem;

    wxCustomComboPopup*     m_listInstance;

    wxCustomPaintFunc       m_paintfunc;
    void*                   m_paintfunc_customdata;

    unsigned char           m_prevMouseFocus; // used detect in which portion of control mouse is

};

// -----------------------------------------------------------------------

class WXDLLIMPEXP_CC wxCCustomComboBoxDefaultHandler : public wxCCustomComboBoxHandler
{
public:
    void Create ( wxCustomControl* pctrl, const wxString& value,
        const wxPoint& pos, const wxSize& sz,
        int n, const wxChar* choices[] );

    int Append( const wxString& str );

    virtual void SetSelection ( const wxCustomComboItem& item );
    virtual void OnSelect ( const wxCustomComboItem& item );
    virtual bool OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata );
    virtual bool OnKeyEvent ( wxKeyEvent& event );
    virtual wxCustomComboPopup* CreatePopup ( wxWindow* frame,
        const wxRect& ctrl_rect, wxCustomControl* ctrl );

    inline const wxArrayPtrVoid& GetLabels() const { return m_labels; }
    inline wxArrayPtrVoid& GetWritableLabels() { return m_labels; }
    inline int GetSelection() const { return m_selection; }

    virtual ~wxCCustomComboBoxDefaultHandler();

protected:

    int                     m_selection;

    wxArrayPtrVoid          m_labels; // Holds pointers to labels.

    wxArrayString           m_extraStrings; // Place extra appended strings here.

};

/** wxCustomControlManager's flags
*/

/** m_parent is wxScrolledWindow or derivative. */
#define wxCCM_FL_SCROLLEDWIN        0x0001

/** Notify for the event poller. */
#define wxCCM_FL_EVENTREADY         0x0002

/** If there is internal-to-control dragging going on. */
#define wxCCM_FL_DRAGGING           0x0004

/** When mouse cursor is actually inside mouse-focused control. */
#define wxCCM_FL_MOUSE_INSIDE       0x0008

/** Set by OnParentScrollEvent handler. */
#define wxCCM_FL_VIEWSTARTCHANGED   0x0010

/** Helper caret info. */
#define wxCCM_FL_CARETVISIBLE       0x0020

// -----------------------------------------------------------------------

/** \class wxCustomControls
    \ingroup classes
    \brief Static functions for drawing custom controls.
*/
class WXDLLIMPEXP_CC wxCustomControlManager
{
    friend class wxCustomControl;
    friend class wxCustomTextCtrlHandler;
    friend class wxCCustomTextCtrl;
    friend class wxCCustomButtonHandler;
    friend class wxCCustomButton;
    friend class wxCCustomComboBoxHandler;
    friend class wxCCustomComboBox;
public:
    
    wxCustomControlManager();
    wxCustomControlManager( wxWindow* parent );
    ~wxCustomControlManager();

    void AddChild ( wxCustomControl* child );

    void AddChildData ( wxCustomControlHandler* pdata );

    void AddEvent ( wxCustomControl* child, int eventtype );

    void ClosePopup ();

    void Create ( wxWindow* parent );

    /** When xxData needs to redraw the control in question, use this
        to create the necessary device context. Then DestroyDC must be
        used to delete it.
    */
    wxDC& CreateDC ( wxPoint* palignpt );

    /** Creates frame etc. for a popup.
    */
    wxWindow* CreatePopupParent() { return m_parent; }

    /** Black text on white background, centered vertically in the rect etc.
        state 0 = no background clearing, 1 = clear bg, 3 = selected, clearbg.
    */
    void CtrlWriteText ( wxDC& dc, const wxChar* text,
        const wxRect& rect, wxCustomControl* ctrl, int state );

    /** See CreateDC.*/
    void DestroyDC ( wxDC& dc );

    wxCustomControlHandler* FindDataByPosition ( int x, int y );
    wxCustomControl* FindWindowByPosition ( int x, int y );

    inline void ForceMouseLeave ( wxMouseEvent& event )
    {
        DoMouseLeave ( event, NULL, TRUE );
    }

    inline wxEvent& GetEvent ()
    {
        m_flags &= ~(wxCCM_FL_EVENTREADY);
        return *(&m_event);
    }

    inline wxPoint GetCaretPosition () const
    {
        wxPoint p = m_pCaret->GetPosition();
        //TranslatePositionToLogical ( &p.x, &p.y );
        return p;
    }

    inline long GetFlags () const { return m_flags; }

    inline wxWindow* GetPopup () const { return m_openPopup; }

    inline wxCustomControl* GetFocused () const { return m_ctrlKbFocus; }
    inline wxCustomControlHandler* GetMouseFocusedData () const { return m_dataMouseFocus; }

    inline wxWindow* GetWindow () const { return m_parent; }

    inline const wxColour& GetWindowColour () const { return *(&m_colBackground); }
    inline const wxColour& GetWindowTextColour () const { return *(&m_colText); }
    inline const wxColour& GetSelectionColour () const { return *(&m_colSelection); }
    inline const wxColour& GetSelectionTextColour () const { return *(&m_colSelectionText); }
    inline const wxColour& GetButtonColour () const { return *(&m_colButton); }
    inline const wxColour& GetButtonTextColour () const { return *(&m_colButtonText); }

    inline wxString& GetTempString() { return *(&m_tempStr1); }

    inline bool HasEvent () const { return (m_flags & wxCCM_FL_EVENTREADY)?TRUE:FALSE; }

    inline bool HasKbFocus () const { return (m_ctrlKbFocus!=NULL)?TRUE:FALSE; }

    inline bool HasMouseFocus () const { return (m_ctrlMouseFocus!=NULL)?TRUE:FALSE; }

    inline bool IsDragging () const { return (m_flags & wxCCM_FL_DRAGGING)?TRUE:FALSE; }

    void MoveCaret ( const wxRect& ctrl_rect, int x, int y );
    //void MoveCaretRelative ( int x, int y );
    void RepositionCaret ();

    /** Must be called when parent's focus changes. state = TRUE if focused.
    */
    void OnParentFocusChange ( bool state );

    /** Must be called when parent wxScrolledWindow gets scrolling event.
    */
    void OnParentScrollWinEvent ( wxScrollWinEvent &event );

    void OnParentScrollChange ();

    bool ProcessKeyboardEvent ( wxKeyEvent& event );

    /** Relays mouse event to appropriate managed custom control. If processes
        (i.e. returns TRUE),  event's coordinates have been converted to control's
        local space.
        \returns
        Returns TRUE if event was inside some ctrl (some events may actually be processed even in this case).
    */
    bool ProcessMouseEvent ( wxMouseEvent& event );

    void RemoveChild ( wxCustomControl* child );

    void RemoveFocus ( wxCustomControl* child );

    inline void ShowCaret ()
    {
        if ( m_pCaret && !(m_flags & wxCCM_FL_CARETVISIBLE) )
        {
            m_pCaret->Show ( TRUE );
            m_flags |= wxCCM_FL_CARETVISIBLE;
            //wxLogDebug ( wxT("ShowCaret( visible_after=%i )"), (int)m_pCaret->IsVisible() );
        }
    }

    inline void ShowCaretBalanced ()
    {
        m_pCaret->Show ( TRUE );
    }

    inline void HideCaretBalanced ()
    {
        m_pCaret->Show ( FALSE );
    }

    inline void HideCaret ()
    {
        if ( m_pCaret && ( m_flags & wxCCM_FL_CARETVISIBLE ) )
        {
            m_pCaret->Show ( FALSE );
            m_flags &= ~(wxCCM_FL_CARETVISIBLE);
            //wxLogDebug ( wxT("HideCaret( visible_after=%i )"), (int)m_pCaret->IsVisible() );
        }
    }

    inline bool IsCaretVisible () const
    {
        //if ( m_pCaret )
            return m_pCaret->IsVisible();
        //return FALSE;
    }

    void SetDoubleBuffer ( wxBitmap* buffer ) { m_bmpDoubleBuffer = buffer; }

    void SetFocus ( wxCustomControl* child );

    void SetFont ( const wxFont& font ) { m_font = font; }

    void SetPopup ( wxCustomComboPopup* popup );

    inline void StartDragging () { m_flags |= wxCCM_FL_DRAGGING; }
    
    inline void StopDragging () { m_flags &= ~(wxCCM_FL_DRAGGING); }

    /** Translates coordinate translation necessary for scrolled window.
    */
    void TranslatePositionToPhysical ( int* x, int* y ) const;
    void TranslatePositionToLogical ( int* x, int* y ) const;

protected:

    // returns index to first data of a control
    int GetControlsFirstData ( wxCustomControl* ctrl );

protected:


    wxWindow*               m_parent;

    wxBitmap*               m_bmpDoubleBuffer; // never owns; just borrows parent's, if available;

    //wxArrayPtrVoid          m_controls;

    wxArrayPtrVoid          m_handlers; // one control may comprise of more than one controldata

    wxCustomControl*    m_ctrlMouseFocus; // which control has mouse focus
    wxCustomControlHandler*    m_dataMouseFocus; // which controldata has mouse focus
    wxCustomControl*    m_ctrlKbFocus; // which control has keyboard focus

    wxCaret*                m_pCaret;

    wxPoint                 m_caretRelativePos; // caret x inside m_ctrlKbFocus

    wxColour                m_colBackground;
    wxColour                m_colText;
    wxColour                m_colSelection;
    wxColour                m_colSelectionText;
    wxColour                m_colButton;
    wxColour                m_colButtonText;

    wxFont                  m_font;

    wxCursor                m_cursorIBeam;

    wxString                m_tempStr1;

    long                    m_flags;

    wxCommandEvent          m_event;

#if wxCC_CORRECT_CONTROL_POSITION
    wxSize                  m_prevViewStart; // for wxScrolledWindow scroll management
#endif

    wxWindow*               m_openPopup;

    void DoMouseLeave ( wxMouseEvent& event, wxCustomControlHandler* newdata, bool force );

};

// -----------------------------------------------------------------------

#define wxCC_FL_KBFOCUS         0x0001

#define wxCC_FL_MOUSEFOCUS      0x0002 // When mouse is over the control and it is not dragging for another control

#define wxCC_FL_NEEDSCARET      0x0004

#define wxCC_FL_DRAWN           0x0008 // Set by drawing function

#define wxCC_FL_MODIFIED        0x0010 // May be needed internally by various controls/datas

#define wxCC_FL_REMOVED         0x0020 // Manager's RemoveChild already called on this.

#define wxCC_EXTRA_FLAG(N)      (1<<(16+N))

// -----------------------------------------------------------------------

void wxRendererNative_DrawButton (wxWindow* win, wxDC& dc, const wxRect& rect, int flags = wxUP );

// -----------------------------------------------------------------------

class WXDLLIMPEXP_CC wxCustomControl : public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxCustomControl)

    friend class wxCustomControlManager;
    friend class wxCustomTextCtrlHandler;
    friend class wxCCustomButtonHandler;
    friend class wxCCustomComboBoxHandler;
public:

    wxCustomControl();
    wxCustomControl ( wxCustomControlManager* manager, wxWindowID id,
        const wxPoint& pos, const wxSize& size, long style = 0 );
    virtual ~wxCustomControl();

    virtual void DoMove ( int x, int y );

    inline const wxColour& GetBackgroundColour() const { return *(&m_manager->m_colBackground);}

    inline wxFont& GetFont () const { return *m_pFont; }
    inline int GetFontHeight () const { return m_fontHeight; }

    inline wxWindowID GetId () const { return m_id; }

    inline wxCustomControlManager* GetManager () const { return m_manager; }

    inline wxWindow* GetParent () const { return m_manager->m_parent; }
    
    inline wxPoint GetPosition () const { return wxPoint(m_rect.x,m_rect.y); }
    
    inline wxRect GetRect () const { return m_rect; }
    
    inline wxSize GetSize () const { return wxSize(m_rect.width,m_rect.height); }

    inline int GetX () const { return m_rect.x; }
    
    inline long GetWindowStyle () const { return m_windowStyle; }

    inline void Move ( const wxPoint& pos ) { DoMove (pos.x,pos.y); }

    inline void Move ( wxCoord x, wxCoord y ) { DoMove (x,y); }

    /** Called on keyboard focus.
    */
    virtual void OnFocus ( bool focused );

    /** Returns TRUE if processed.
    */
    virtual bool OnKeyEvent ( wxKeyEvent& event );

    /** entry is TRUE if mouse enters, FALSE if leaves.
    */
    virtual bool OnMouseFocus ( bool entry );

    virtual bool OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata );

    /** Draw control on parent. If dc is NULL, creates one. */
    void DrawPDC ( wxDC* dc = NULL );

    inline void Draw () { DrawPDC ( NULL ); }

    void Draw ( wxDC& dc );

    /** Draw control on dc. align is control position adjustment on dc (incase
        of a double-buffer, for example).
    */
    void Draw ( wxDC& dc, const wxPoint* align );

    /**
    Draws control in to the dc.
    \remarks
    - On both entry and exit, nothing can be assumed of current Pen or Brush.
    - May set clipping region to the control rectangle. If so, calls
      DestroyClippingRegion before exiting.
    */
    virtual void DoDraw ( wxDC& dc, const wxRect& rect );

    virtual void DoSetSize ( int width, int height );

    wxCommandEvent& GetEvent () const { return *(&m_manager->m_event); }

    inline bool HasFocus () const { return (m_manager->m_ctrlKbFocus == this )?TRUE:FALSE; }

    inline void Refresh ( bool = FALSE, const wxRect* = (const wxRect*) NULL ) { Draw(); }

    inline void RemoveFocus ()
    {
        m_manager->RemoveFocus ( this );
    }

    inline void ResetFont () { m_pFont = &m_manager->m_font; }

    inline void SetFocus ()
    {
        m_manager->SetFocus ( this );
    }

    inline void SetFocusFromKbd ()
    {
        m_manager->SetFocus ( this );
    }

    /** Sets font for the control.
        \remarks
        Does not copy the font for itself, just pointer to it.
    */
    virtual void SetFont ( wxFont& font, wxDC* pdc = NULL );

    inline void SetSize ( int x, int y, int width, int height )
    {
        DoMove ( x, y );
        DoSetSize ( width, height );
    }

    inline void SetSize ( const wxRect& rect )
    {
        DoMove ( rect.x, rect.y );
        DoSetSize ( rect.width, rect.height );
    }

    inline void SetSize ( int width, int height )
    { 
        DoSetSize ( width, height );
    }

    inline void SetSize ( const wxSize& size )
    {
        DoSetSize ( size.x, size.y );
    }

    inline void Show() { }

protected:
    wxCustomControlManager  *m_manager;
    wxFont                  *m_pFont;
    wxRect                  m_rect;
    //wxSize                  m_imageSize; // Size of image in front of the control
    wxWindowID              m_id;
    long                    m_windowStyle;
    wxUint32                m_flags;
    int                     m_fontHeight;
    int                     m_xSplit; // splitting the control into two parts (wxCCustomComboBox needs)
    unsigned char           m_curPart; // which part of the control is focused? (only if m_xSplit)
};

// -----------------------------------------------------------------------

inline bool wxCustomControlHandler::IsMouseFocused() const
{
    return m_control->GetManager()->GetMouseFocusedData() == this;
}

// -----------------------------------------------------------------------

class WXDLLIMPEXP_CC wxCCustomTextCtrl : public wxCustomControl
{
	DECLARE_DYNAMIC_CLASS(wxCCustomTextCtrl)

public:

    wxCCustomTextCtrl () : wxCustomControl() { }

    wxCCustomTextCtrl ( wxCustomControlManager* manager, wxWindowID id, const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = 0 );

    virtual ~wxCCustomTextCtrl();

    inline wxString GetValue() const { return m_data.GetValue(); }

    inline void SetSelection(long from, long to)
    {
        if ( m_data.SetSelection(from,to) )
            Draw ();
    }

    void SetValue ( const wxString& value );

    inline void SetInsertionPoint ( long pos, long first_visible = -1 )
    {
        if ( m_data.SetInsertionPoint (pos,first_visible) )
            Draw ();
    }

    virtual void DoDraw ( wxDC& dc, const wxRect& rect );
    virtual void DoMove ( int x, int y );
    virtual void DoSetSize ( int width, int height );
    virtual void OnFocus ( bool focused );
    virtual bool OnKeyEvent ( wxKeyEvent& event );
    virtual bool OnMouseFocus ( bool entry );
    virtual bool OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata );
    virtual void SetFont ( wxFont& font, wxDC* pdc );

protected:
    wxCustomTextCtrlHandler    m_data;
};

// -----------------------------------------------------------------------

//#define wxCC_BT_DOWN    wxCC_EXTRA_FLAG(0)

class WXDLLIMPEXP_CC wxCCustomButton : public wxCustomControl
{
	DECLARE_DYNAMIC_CLASS(wxCCustomButton)

public:

    wxCCustomButton () : wxCustomControl() { }

    wxCCustomButton ( wxCustomControlManager* manager, wxWindowID id, const wxChar* label,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
    virtual ~wxCCustomButton();

    inline void SetLabel ( const wxChar* label ) { m_data.m_label = label; }

    virtual void DoDraw ( wxDC& dc, const wxRect& rect );
    virtual void DoMove ( int x, int y );
    virtual void DoSetSize ( int width, int height );
    virtual bool OnKeyEvent ( wxKeyEvent& event );
    virtual bool OnMouseFocus ( bool entry );
    virtual bool OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata );
    //virtual void SetFont ( wxFont& font );

protected:
    wxCCustomButtonHandler      m_data;
};

// -----------------------------------------------------------------------

#define wxCH_CC_DOUBLE_CLICK_CYCLES     0x0010 // double-clicking cycles the selection
#define wxCH_CC_IMAGE_EXTENDS           0x0020 // image is drawn outside in non-popup portion
#define wxCH_CC_DROPDOWN_ANCHOR_RIGHT   0x0040 // anchor drop-down to right edge of parent window.

class WXDLLIMPEXP_CC wxCCustomComboBox : public wxCustomControl
{
	DECLARE_DYNAMIC_CLASS(wxCCustomComboBox)

public:

    wxCCustomComboBox () : wxCustomControl() { }

    wxCCustomComboBox ( wxCustomControlManager* manager, wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        int n = 0, const wxChar* choices[] = NULL, long style = 0,
        const wxSize& imagesize = wxDefaultSize );
    virtual ~wxCCustomComboBox();

    inline int Append( const wxString& str )
    {
        return ((wxCCustomComboBoxDefaultHandler*)m_chData)->Append( str );
    }

    inline wxWindow* GetPopupInstance() const { return m_chData->GetPopupInstance(); }

    inline int GetSelection () const { return ((wxCCustomComboBoxDefaultHandler*)m_chData)->GetSelection(); }

    inline const wxString& GetValue () const
    {
        return m_chData->m_text;
    }

    void SetSelection ( int n );

    inline void SetImagePaintFunction ( wxCustomPaintFunc paintfunc, void* userdata )
    {
        m_chData->m_paintfunc = paintfunc;
        m_chData->m_paintfunc_customdata = userdata;
    }

    virtual void DoDraw ( wxDC& dc, const wxRect& rect );
    virtual bool OnKeyEvent ( wxKeyEvent& event );
    virtual bool OnMouseFocus ( bool entry );
    virtual bool OnMouseEvent ( wxMouseEvent& event, wxCustomControlHandler* pdata );
    virtual void DoMove ( int x, int y );
    virtual void DoSetSize ( int width, int height );

    inline void SetValue ( const wxString& text )
    {
        m_chData->SetValue(text);
        Draw();
    }

    inline const wxString& SetValue () const
    {
        return m_chData->GetValue();
    }

protected:
    wxCCustomComboBoxHandler*    m_chData;
};

class WXDLLIMPEXP_CC wxCCustomChoice : public wxCCustomComboBox
{
	DECLARE_DYNAMIC_CLASS(wxCCustomChoice)

public:

    wxCCustomChoice () : wxCCustomComboBox() { }

    wxCCustomChoice ( wxCustomControlManager* manager, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        int n = 0, const wxChar* choices[] = NULL, long style = 0,
        const wxSize& imagesize = wxDefaultSize );
    virtual ~wxCCustomChoice();
};

// -----------------------------------------------------------------------

#endif // wxPG_USE_CUSTOM_CONTROLS

#if wxPG_USE_GENERIC_TEXTCTRL

#define wxGTextCtrlBase wxControl

class WXDLLIMPEXP_CC wxGenericTextCtrl : public wxGTextCtrlBase
{
public:
    // creation
    // --------

    wxGenericTextCtrl() { Init(); }
    wxGenericTextCtrl(wxWindow *parent, wxWindowID id,
                      const wxString& value = wxEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxTextCtrlNameStr)
    {
        Init();

        Create(parent, id, value, pos, size, style, validator, name);
    }
    ~wxGenericTextCtrl();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);

    // implement base class pure virtuals
    // ----------------------------------
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);

    virtual wxString GetRange(long from, long to) const;

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    virtual bool IsModified() const;
    virtual bool IsEditable() const;

    virtual void GetSelection(long* from, long* to) const;

    // operations
    // ----------
    virtual void Remove(long from, long to);

    // special
    void DispatchEvent( int evtId );
    bool SetInsertionPoint (long pos,
                            long first_visible = -1);
    void DeleteSelection ();
    bool SetSelection ( long from, long to );

    inline wxTextPos GetLastPosition() const
    {
        return m_text.length();
    }

    // Standard wxWindow virtual overrides
    virtual void SetFocus();

protected:

    int HitTest ( wxCoord x, int* pCol );
    void RecalculateMetrics();

    // common part of all ctors
    void Init();

    wxString        m_text;
    wxCaret*        m_pCaret;
    unsigned int    m_position;
    unsigned int    m_scrollPosition;
    int             m_selStart;
    int             m_selEnd;
    int             m_itemButDown; // dragging centers around this
    int             m_fontHeight;

    bool            m_isModified;
    bool            m_isEditable;

    wxArrayInt      m_arrExtents; // cached array of text extents

    //int UpdateExtentCache ( wxString& tempstr, size_t index );

    // event handlers
    void OnKeyEvent ( wxKeyEvent& event );
    void OnMouseEvent ( wxMouseEvent& event );
    void OnPaint ( wxPaintEvent& event );

/*    inline void ShowCaret ()
    {
        if ( m_pCaret && !(m_flags & wxCCM_FL_CARETVISIBLE) )
        {
            m_pCaret->Show ( TRUE );
            m_flags |= wxCCM_FL_CARETVISIBLE;
            //wxLogDebug ( wxT("ShowCaret( visible_after=%i )"), (int)m_pCaret->IsVisible() );
        }
    }*/

    inline void ShowCaretBalanced ()
    {
        m_pCaret->Show ( TRUE );
    }

    inline void HideCaretBalanced ()
    {
        m_pCaret->Show ( FALSE );
    }

    //void MoveCaret ( int x, int y );

/*    inline void HideCaret ()
    {
        if ( m_pCaret && ( m_flags & wxCCM_FL_CARETVISIBLE ) )
        {
            m_pCaret->Show ( FALSE );
            m_flags &= ~(wxCCM_FL_CARETVISIBLE);
            //wxLogDebug ( wxT("HideCaret( visible_after=%i )"), (int)m_pCaret->IsVisible() );
        }
    }*/

    inline bool IsCaretVisible () const
    {
        //if ( m_pCaret )
            return m_pCaret->IsVisible();
        //return FALSE;
    }

    /*
    // editing
    virtual void Clear();
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);

    // load the controls contents from the file
    virtual bool LoadFile(const wxString& file);

    // clears the dirty flag
    virtual void MarkDirty();
    virtual void DiscardEdits();

    virtual void SetMaxLength(unsigned long len);

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);

#ifdef __WIN32__
    virtual bool EmulateKeyPress(const wxKeyEvent& event);
#endif // __WIN32__

#if wxUSE_RICHEDIT
    // apply text attribute to the range of text (only works with richedit
    // controls)
    virtual bool SetStyle(long start, long end, const wxTextAttr& style);
    virtual bool SetDefaultStyle(const wxTextAttr& style);
    virtual bool GetStyle(long position, wxTextAttr& style);
#endif // wxUSE_RICHEDIT

    // translate between the position (which is just an index in the text ctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const;

    virtual void ShowPosition(long pos);
    virtual wxTextCtrlHitTestResult HitTest(const wxPoint& pt, long *pos) const;
    virtual wxTextCtrlHitTestResult HitTest(const wxPoint& pt,
                                            wxTextCoord *col,
                                            wxTextCoord *row) const
    {
        return wxTextCtrlBase::HitTest(pt, col, row);
    }

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();

    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;

    // Undo/redo
    virtual void Undo();
    virtual void Redo();

    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // Insertion point
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual wxTextPos GetLastPosition() const;

    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    // Caret handling (Windows only)

    bool ShowNativeCaret(bool show = true);
    bool HideNativeCaret() { return ShowNativeCaret(false); }

    // Implementation from now on
    // --------------------------

    virtual void SetWindowStyleFlag(long style);

    virtual void Command(wxCommandEvent& event);
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual WXHBRUSH MSWControlColor(WXHDC hDC);

#if wxUSE_RICHEDIT
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    int GetRichVersion() const { return m_verRichEdit; }
    bool IsRich() const { return m_verRichEdit != 0; }

    // rich edit controls are not compatible with normal ones and wem ust set
    // the colours for them otherwise
    virtual bool SetBackgroundColour(const wxColour& colour);
    virtual bool SetForegroundColour(const wxColour& colour);
#endif // wxUSE_RICHEDIT

    virtual void AdoptAttributesFromHWND();

    virtual bool AcceptsFocus() const;

    // callbacks
    void OnDropFiles(wxDropFilesEvent& event);
    void OnChar(wxKeyEvent& event); // Process 'enter' if required

    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnUpdateDelete(wxUpdateUIEvent& event);
    void OnUpdateSelectAll(wxUpdateUIEvent& event);

    // Show a context menu for Rich Edit controls (the standard
    // EDIT control has one already)
    void OnContextMenu(wxContextMenuEvent& event);

    // be sure the caret remains invisible if the user
    // called HideNativeCaret() before
    void OnSetFocus(wxFocusEvent& event);

protected:

    // intercept WM_GETDLGCODE
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    // call this to increase the size limit (will do nothing if the current
    // limit is big enough)
    //
    // returns true if we increased the limit to allow entering more text,
    // false if we hit the limit set by SetMaxLength() and so didn't change it
    bool AdjustSpaceLimit();

#if wxUSE_RICHEDIT && (!wxUSE_UNICODE || wxUSE_UNICODE_MSLU)
    // replace the selection or the entire control contents with the given text
    // in the specified encoding
    bool StreamIn(const wxString& value, wxFontEncoding encoding, bool selOnly);

    // get the contents of the control out as text in the given encoding
    wxString StreamOut(wxFontEncoding encoding, bool selOnly = false) const;
#endif // wxUSE_RICHEDIT

    // replace the contents of the selection or of the entire control with the
    // given text
    void DoWriteText(const wxString& text, bool selectionOnly = true);

    // set the selection possibly without scrolling the caret into view
    void DoSetSelection(long from, long to, bool scrollCaret = true);

    // return true if there is a non empty selection in the control
    bool HasSelection() const;

    // get the length of the line containing the character at the given
    // position
    long GetLengthOfLineContainingPos(long pos) const;

    // send TEXT_UPDATED event, return true if it was handled, false otherwise
    bool SendUpdateEvent();

    // override some base class virtuals
    virtual bool MSWShouldPreProcessMessage(WXMSG* pMsg);
    virtual wxSize DoGetBestSize() const;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

#if wxUSE_RICHEDIT
    // we're using RICHEDIT (and not simple EDIT) control if this field is not
    // 0, it also gives the version of the RICHEDIT control being used (1, 2 or
    // 3 so far)
    int m_verRichEdit;
#endif // wxUSE_RICHEDIT

    // number of EN_UPDATE events sent by Windows when we change the controls
    // text ourselves: we want this to be exactly 1
    int m_updatesCount;

    virtual wxVisualAttributes GetDefaultAttributes() const;

    wxMenu* m_privateContextMenu;

    bool m_isNativeCaretShown;
*/

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGenericTextCtrl)
};

#endif

//
// Undefine macros that are not needed outside library source files
//
#ifndef __wxCCM_SOURCE_FILE__
# undef WXDLLIMPEXP_CC
#endif

#endif // __WX_CUSTCTRL_H__
