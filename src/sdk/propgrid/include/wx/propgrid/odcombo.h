///////////////////////////////////////////////////////////////////////////////
// Name:        odcombo.h
// Purpose:     wxPGOwnerDrawnComboBox and related classes interface
// Author:      Jaakko Salli
// Modified by:
// Created:     Jan-25-2005
// RCS-ID:      $Id:
// Copyright:   (c) 2005 Jaakko Salli
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ODCOMBO_H_
#define _WX_ODCOMBO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "odcombobox.h"
#endif

#include "wx/choice.h"
//#include "wx/popupwin.h"

// Item counts in GUI components were changed in 2.8.0
#if wxCHECK_VERSION(2,8,0)
    #define wxODCCount  unsigned int
    #define wxODCIndex  unsigned int
#else
    #define wxODCCount  int
    #define wxODCIndex  int
#endif

class WXDLLEXPORT wxVListBox;
class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxButton;

// Temp stuff
#ifdef WXMAKINGDLL_PROPGRID
    #define WXDLLEXPORT_PGODC WXEXPORT
//#elif defined(WXUSINGDLL)
//    #define WXDLLEXPORT_PGODC WXIMPORT
#else // not making nor using DLL
    #define WXDLLEXPORT_PGODC
#endif

class WXDLLEXPORT_PGODC wxPGOwnerDrawnComboBox;

// ----------------------------------------------------------------------------
// wxComboPopupInterface is the interface class that lies between
// the wxPGOwnerDrawnComboBox and its popup window.
// ----------------------------------------------------------------------------

//
// Callback flags
//

// when set, we are painting the selected item in control, not in the popup
#define wxODCB_CB_PAINTING_CONTROL  0x0001


typedef void (wxEvtHandler::* wxComboPaintCallback)(wxPGOwnerDrawnComboBox* pCb,
                                                    int item,
                                                    wxDC& dc,
                                                    wxRect& rect,
                                                    int flags );

class WXDLLEXPORT_PGODC wxComboPopupInterface : public wxEvtHandler
{
    friend class wxPGOwnerDrawnComboBox;
public:

    wxComboPopupInterface();
    virtual ~wxComboPopupInterface();

    // default implementation does nothing
    virtual void Clear();

    // default implementation does nothing
    virtual void Delete( int item );

    // wxComboBox::FindString
    // default implementation returns wxNOT_FOUND
    virtual int FindString(const wxString& s) const;

    // default implementation returns NULL
    //virtual void* GetClientData( int item ) const;

    // default implementation returns 0
    virtual int GetCount() const;

    // wxComboBox::GetString
    // default implementation returns empty string
    virtual wxString GetString( int item ) const;

    // default implementation does nothing
    virtual void Insert( const wxString& item, int pos );

    // default implementation does nothing
    //virtual void SetClientData( int item, void* clientData );

    // wxComboBox::SetSelection
    // default implementation does nothing
    virtual void SetSelection( int item );

    // wxComboBox::SetString
    // default implementation does nothing
    virtual void SetString( int item, const wxString& str );

    // wxComboBox::SetValue
    // default implementation does nothing
    virtual void SetValueFromString ( const wxString& value );

    // default implementation returns NULL
    //virtual wxArrayString* GetArrayString();

    // default implementation returns false
    //virtual bool IsValueInt() const;

    // returns TRUE if item is hilited
    virtual bool IsHighlighted ( int item ) const;

    // called by wxOwnerDrawComboBox after assigned to it
    // If returns TRUE, then GeneratePopup is called immediately
    // (instead of at the first time it is shown). Default returns
    // TRUE.
    virtual bool Init( wxPGOwnerDrawnComboBox* combo );

    // returns pointer to created popup control
    // parent = wxPopupWindow where combo popup should be placed
    //   if NULL, then it is not necesary to create the popup
    // minWidth = preferred minimum width for window
    // maxHeight = absolute maximum height for window
    // prefHeight = preferred height. Only applies if > 0,
    //   and should only be rounded down, if necessary.
    // isVolatile = set to true if window should be destroyed
    //   when hidden. If not then this method is still called.
    //   In this case, the existing window, with adjusted
    //   dimensions should be returned.
    virtual wxWindow* GeneratePopup( wxWindow* parent, int minWidth,
                                     int maxHeight, int prefHeight ) = 0;

    // Returns pointer to the current value (if integer).
    // With the default popup, for example, this returns pointer
    // to int.
    virtual const int* GetIntPtr () const;

    // gets displayed string representation of the value
    virtual wxString GetValueAsString() const = 0;

    //
    // Helper methods.
    //wxPopupWindow* CreatePopupWindow () const;

    // Event handler handles this.
    //wxWindow*               m_handledWnd;

    //inline wxWindow* GetPopupControl() const { wxASSERT(m_popupAsWnd); return m_popupAsWnd; }

    inline wxComboPaintCallback GetCallback() const { return m_callback; }

protected:

    // TODO: Once popup transient window works Ok, remove these.
    void OnMouseEvent( wxMouseEvent& event );
    //void OnFocusChange ( wxFocusEvent& event );

    wxPGOwnerDrawnComboBox*   m_combo;
    //wxWindow*               m_popupAsWnd;

    // Must be in all interfaces to paint the combobox too (in some cases)
    wxComboPaintCallback    m_callback;

private:
    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// wxPGCustomComboBox: a generic control that looks like a wxComboBox but allows
// completely custom popup.
//
// Remarks:
//   - It accepts wxCB_READONLY to disable the wxTextCtrl (actually the control
//     is not created at all).
//
// ----------------------------------------------------------------------------


#define wxPGCustomComboBoxBase        wxControl


class WXDLLEXPORT_PGODC wxPGCustomComboBox : public wxPGCustomComboBoxBase
{
public:
    // ctors and such
    wxPGCustomComboBox() : wxPGCustomComboBoxBase() { Init(); }

    wxPGCustomComboBox(wxWindow *parent,
                     wxWindowID id,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxComboBoxNameStr)
        : wxPGCustomComboBoxBase()
    {
        Init();

        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    virtual ~wxPGCustomComboBox();

    // get the popup window containing the popup control
    wxWindow *GetPopupWindow() const { return m_winPopup; }

    virtual void HidePopup( bool sendEvent );
    void SendShowPopupSignal();

    // common code to be called on popup hide
    void OnPopupDismiss();

    // return true if the popup is currently shown
    bool IsPopupShown() const { return (m_isPopupShown > 0); }

    // get the popup control/panel in window
    wxWindow *GetPopupControl() const { return m_popup; }

    // Get the text control which is part of the combobox.
    wxTextCtrl *GetTextCtrl() const { return m_text; }

    // get the dropdown button which is part of the combobox
    // note: its not necessarily a wxButton or wxBitmapButton
    wxWindow *GetButton() const { return m_btn; }

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual bool SetFont(const wxFont& font);
    //virtual void SetFocus();
    //virtual void SetFocusFromKbd();
    void SetId( wxWindowID winid );

    // Return TRUE if item is hilited in the combo
    bool IsHighlighted ( int item ) const;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

    // wxTextCtrl methods
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);

    // wxPGCustomComboBox-only methods

    // Adjust popup size.
    //   extLeft = how many pixels the popup extends to the left
    //   extRight = how many pixels the popup extends to the right
    //   prefHeight = preferred popup height
    void SetPopupExtents( int extLeft, int extRight = 0, int prefHeight = -1 );

    // set width of custom paint area in writable combo
    // in read-only, used to indicate area that is not covered by the
    // focus rectangle
    void SetCustomPaintArea( int width );

    // This is called to custom paint the control itself (ie. not the popup).
    virtual void OnItemPaint( wxDC& dc, const wxRect& rect );

    /*inline void PreventNextButtonPopup()
    {
        m_isPopupShown = -1;
    }*/

protected:

    // override the base class virtuals involved into geometry calculations
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual wxSize DoGetBestSize() const;

    // event handlers
    void OnMouseEvent( wxMouseEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnResize( wxSizeEvent& event );
    void OnFocusEvent( wxFocusEvent& event );

    // This is used when m_text is hidden (readonly).
    wxString                m_valueString;

    // the text control and button we show all the time
    wxTextCtrl*             m_text;
    wxWindow*               m_btn;

    // popup window containing the window managed by the interface .
    wxWindow*               m_winPopup;

    // the popup control/panel
    wxWindow*               m_popup;

    // this is for this control itself
    wxEvtHandler*           m_extraEvtHandler;

    // this is for text
    wxEvtHandler*           m_textEvtHandler;

    // this is for the top level window
    wxEvtHandler*   m_toplevEvtHandler;

    wxLongLong              m_timeLastMouseUp;

    // how much popup should expand to the left/right of the control
    wxCoord                 m_extLeft;
    wxCoord                 m_extRight;

    // preferred popup height. default is -1.
    wxCoord                 m_heightPopup;

    // how much of writable combo is custom-paint by callback?
    // also used to indicate area that is not covered by "blue"
    // selection indicator.
    wxCoord                 m_widthCustomPaint;

    // FIXME: Only necessary for GTK and its kind
    wxCoord                 m_widthCustomBorder;

    // is the popup window currenty shown?
    // < 0 is special value
    int                     m_isPopupShown;

    // TODO: Remove after real popup works ok.
    unsigned char m_fakePopupUsage;

    unsigned char m_ignoreNextButtonClick;

    // Set to 1 on mouse down, 0 on mouse up. Used to eliminate down-less mouse ups.
    unsigned char m_downReceived;

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxPGCustomComboBox)
};


// ----------------------------------------------------------------------------
// wxPGOwnerDrawnComboBox: a generic wxComboBox that allows custom paint items
// and even a completely custom popup control.
// ----------------------------------------------------------------------------

// New window styles

// if used, control itself is not custom paint using callback
// even if this is not used, writable combo is never custom paint
// until SetCustomPaintArea is called
#define wxODCB_STD_CONTROL_PAINT    0x0100

// Causes double-clicking to cycle the item instead of showing
// the popup. Shift-pressed causes cycling direction to reverse.
#define wxODCB_DOUBLE_CLICK_CYCLES  0x0200


class WXDLLEXPORT_PGODC wxPGOwnerDrawnComboBox : public wxPGCustomComboBox, public wxItemContainer
{
    friend class wxComboPopupWindow;
    friend class wxPGCustomComboBox;
public:
    // ctors and such
    wxPGOwnerDrawnComboBox() : wxPGCustomComboBox() { Init(); }

    wxPGOwnerDrawnComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               int n,
               const wxString choices[],
               wxComboPaintCallback callback,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr)
        : wxPGCustomComboBox()
    {
        Init();

        (void)Create(parent, id, value, pos, size, n, choices,
                     callback, style, validator, name);
    }

    wxPGOwnerDrawnComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               wxComboPopupInterface* iface = (wxComboPopupInterface*) NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr)
        : wxPGCustomComboBox()
    {
        Init();

        (void)Create(parent, id, value, pos, size, iface,
                     style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                wxComboPopupInterface* iface = (wxComboPopupInterface*) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    wxPGOwnerDrawnComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               wxComboPaintCallback callback = (wxComboPaintCallback) NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = (const wxString *) NULL,
                wxComboPaintCallback callback = (wxComboPaintCallback) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                wxComboPaintCallback callback = (wxComboPaintCallback) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    virtual ~wxPGOwnerDrawnComboBox();

// a combo control needs a control for popup window it displays
    //void SetPopupControl(wxComboPopup *popup);
    wxComboPopupInterface* GetPopupInterface() const { return m_popupInterface; }

    // show/hide popup window
    void ShowPopup();
    void HidePopup( bool sendEvent = true );

    // Return TRUE if item is hilited in the combo
    bool IsHighlighted ( int item ) const;

    // wxControlWithItems methods
    virtual void Clear();
    virtual void Delete(wxODCIndex n);
    virtual wxODCCount GetCount() const;
    virtual wxString GetString(wxODCIndex n) const;
    virtual void SetString(wxODCIndex n, const wxString& s);
    virtual int FindString(const wxString& s) const;
    virtual void Select(int n);
    virtual int GetSelection() const;
    void SetSelection(int n) { Select(n); }

    // wxPGOwnerDrawnComboBox-only methods

    // Adjust popup size.
    //   extLeft = how many pixels the popup extends to the left
    //   extRight = how many pixels the popup extends to the right
    //   prefHeight = preferred popup height
    //void SetPopupExtents( int extLeft, int extRight = 0, int prefHeight = -1 );

    void SetPopupInterface( wxComboPopupInterface* iface );

    virtual void OnItemPaint( wxDC& dc, const wxRect& rect );

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

    //inline bool HasClientObjectData () const { return false; }

protected:

    // event handlers
    void OnButtonClick(wxCommandEvent& event);

    // clears all allocated client datas
    void ClearClientDatas();

    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, int pos);
    virtual void DoSetItemClientData(wxODCIndex n, void* clientData);
    virtual void* DoGetItemClientData(wxODCIndex n) const;
    virtual void DoSetItemClientObject(wxODCIndex n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(wxODCIndex n) const;

    // popup interface
    wxComboPopupInterface*  m_popupInterface;

    // holder for client datas
    wxArrayPtrVoid          m_clientDatas;

    // is the data type of interface 'int'?
    bool                    m_hasIntValue;

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxPGOwnerDrawnComboBox)
};

#endif // _WX_ODCOMBO_H_
