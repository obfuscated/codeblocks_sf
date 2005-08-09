///////////////////////////////////////////////////////////////////////////////
// Name:        odcombo.h
// Purpose:     wxPGComboBox and related classes interface
// Author:      Jaakko Salli
//              (loosely based on wxUniv combo.h by Vadim Zeitlin)
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

class WXDLLEXPORT_PGODC wxPGComboBox;

// ----------------------------------------------------------------------------
// wxComboPopupInterface is the interface class that lies between
// the wxPGComboBox and its popup window.
// ----------------------------------------------------------------------------

//
// Callback flags
//

// when set, we are painting the selected item in control, not in the popup
#define wxODCB_CB_PAINTING_CONTROL  0x0001

typedef void (wxEvtHandler::* wxComboPaintCallback)(wxPGComboBox* pCb,
                                                    int item,
                                                    wxDC& dc,
                                                    wxRect& rect,
                                                    int flags );

class WXDLLEXPORT_PGODC wxComboPopupInterface : public wxEvtHandler
{
    friend class wxPGComboBox;
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
    virtual bool Init( wxPGComboBox* combo );

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

    wxPGComboBox*   m_combo;
    //wxWindow*               m_popupAsWnd;

    // Must be in all interfaces to paint the combobox too (in some cases)
    wxComboPaintCallback    m_callback;

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxPGComboBox: a generic wxComboBox that allows custom paint items
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


#define wxOwnerDrawnComboBoxBase wxChoiceBase

class WXDLLEXPORT_PGODC wxPGComboBox : public wxOwnerDrawnComboBoxBase
{
    friend class wxComboPopupWindow;
public:
    // ctors and such
    wxPGComboBox() { Init(); }

    wxPGComboBox(wxWindow *parent,
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
    {
        Init();

        (void)Create(parent, id, value, pos, size, n, choices,
                     callback, style, validator, name);
    }

    wxPGComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               wxComboPopupInterface* iface = (wxComboPopupInterface*) NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr)
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

    wxPGComboBox(wxWindow *parent,
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

    virtual ~wxPGComboBox();

// a combo control needs a control for popup window it displays
    //void SetPopupControl(wxComboPopup *popup);
    wxComboPopupInterface* GetPopupInterface() const { return m_popupInterface; }

    // show/hide popup window
    void ShowPopup();
    void HidePopup( bool sendEvent = true );

    // common code to be called on popup hide
    void OnPopupDismiss();

    // return true if the popup is currently shown
    bool IsPopupShown() const { return (m_isPopupShown > 0); }

    // get the popup window containing the popup control
    wxWindow *GetPopupWindow() const { return m_winPopup; }

    // get the popup control/panel in window
    wxWindow *GetPopupControl() const { return m_popup; }

    // get the text control which is part of the combobox
    wxTextCtrl *GetText() const { return m_text; }

    // get the dropdown button which is part of the combobox
    wxWindow *GetButton() const { return m_btn; }

    // implementation only from now on
    // -------------------------------

    // notifications from wxComboPopup (shouldn't be called by anybody else)

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual bool SetFont(const wxFont& font);
    virtual void SetFocus();
    virtual void SetFocusFromKbd();
    void SetId( wxWindowID winid );

    // Return TRUE if item is hilited in the combo
    bool IsHighlighted ( int item ) const;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

    // the wxUniversal-specific methods
    // --------------------------------

    // implement the combobox interface

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
    //virtual void SetEditable(bool editable);

    // wxControlWithItems methods
    virtual void Clear();
    virtual void Delete(int n);
    virtual int GetCount() const;
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& s);
    virtual int FindString(const wxString& s) const;
    virtual void Select(int n);
    virtual int GetSelection() const;
    void SetSelection(int n) { Select(n); }

    // wxPGComboBox-only methods

    // Adjust popup size.
    //   extLeft = how many pixels the popup extends to the left
    //   extRight = how many pixels the popup extends to the right
    //   prefHeight = preferred popup height
    void SetPopupExtents( int extLeft, int extRight = 0, int prefHeight = -1 );

    // set width of custom paint area in writable combo
    // in read-only, used to indicate area that is not covered by the
    // "blue" colour drawn when control is focused.
    void SetCustomPaintArea( int width );

    void SetPopupInterface( wxComboPopupInterface* iface );

    inline void PreventNextButtonPopup()
    {
        m_isPopupShown = -1;
    }

    //void SetStringSelection(const wxString& WXUNUSED(s)) {  }

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

    //inline bool HasClientObjectData () const { return false; }

protected:

    // override the base class virtuals involved into geometry calculations
    //virtual wxSize DoGetBestClientSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);
    /*virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);*/
    virtual wxSize DoGetBestSize() const;

    // event handlers
    void OnMouseEvent( wxMouseEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnResize( wxSizeEvent& event );
    //void OnKey(wxKeyEvent& event);
    void OnFocusEvent( wxFocusEvent& event );
    void OnButtonClick(wxCommandEvent& event);

    // common part of all ctors
    void Init();

    // clears all allocated client datas
    void ClearClientDatas();

    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, int pos);
    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;

    // get the associated listbox
    //wxListBox *GetLBox() const { return m_lbox; }

    // This is used when m_text is hidden (readonly).
    wxString    m_valueString;

    // the text control and button we show all the time
    wxTextCtrl *m_text;
    wxWindow *m_btn;

    // the popup control/panel
    wxWindow *m_popup;

    // popup interface
    wxComboPopupInterface*  m_popupInterface;

    // popup window containing the window managed by the interface .
    wxWindow *m_winPopup;

    // this is for this control itself
    wxEvtHandler *m_extraEvtHandler;

    // this is for text
    wxEvtHandler *m_textEvtHandler;

    // this is for the top level window
    wxEvtHandler *m_toplevEvtHandler;

    // holder for client datas
    wxArrayPtrVoid m_clientDatas;

    // how much popup should expand to the left/right of the control
    wxCoord m_extLeft;
    wxCoord m_extRight;

    // preferred popup height. default is -1.
    wxCoord m_heightPopup;

    // how much of writable combo is custom-paint by callback?
    // also used to indicate area that is not covered by "blue"
    // selection indicator.
    wxCoord m_widthCustomPaint;

    // FIXME: Only necessary for GTK and its kind
    wxCoord m_widthCustomBorder;

    // is the popup window currenty shown?
    // < 0 is special value
    int m_isPopupShown;

    // is the data type of interface 'int'?
    bool m_hasIntValue;

    // TODO: Remove after real popup works ok.
    unsigned char m_fakePopupUsage;

    // should popup be destroyed on close?
    //bool m_isPopupVolatile;

private:
    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxPGComboBox)
};

#endif // _WX_ODCOMBO_H_
