#ifndef WXSCIPOPUP_H__
#define WXSCIPOPUP_H__

//----------------------------------------------------------------------
// wxSCIPopupWindow

#if defined(__WXOSX_COCOA__) || defined(__WXMSW__) || defined(__WXGTK__)
    #define wxSCI_POPUP_IS_CUSTOM 1
#else
    #define wxSCI_POPUP_IS_CUSTOM 0
#endif

// Define the base class used for wxSCIPopupWindow.
#ifdef __WXOSX_COCOA__

    #include "wx/nonownedwnd.h"
    #define wxSCI_POPUP_IS_FRAME 0

    class wxSCIPopupBase:public wxNonOwnedWindow
    {
    public:
        wxSCIPopupBase(wxWindow*);
        virtual ~wxSCIPopupBase();
        virtual bool Show(bool show=true) wxOVERRIDE;

    protected:
        virtual void DoSetSize(int, int, int, int, int) wxOVERRIDE;
        void SetSCICursor(int);
        void RestoreSCICursor();
        void OnMouseEnter(wxMouseEvent&);
        void OnMouseLeave(wxMouseEvent&);
        void OnParentDestroy(wxWindowDestroyEvent& event);

    private:
        WX_NSWindow       m_nativeWin;
        wxScintilla*      m_stc;
        bool              m_cursorSetByPopup;
        int               m_prevCursor;
    };

#elif wxUSE_POPUPWIN

    #include "wx/popupwin.h"
    #define wxSCI_POPUP_IS_FRAME 0

    class wxSCIPopupBase:public wxPopupWindow
    {
    public:
        wxSCIPopupBase(wxWindow*);
        #ifdef __WXGTK__
            virtual ~wxSCIPopupBase();
        #elif defined(__WXMSW__)
            virtual bool Show(bool show=true) wxOVERRIDE;
            virtual bool MSWHandleMessage(WXLRESULT *result, WXUINT message,
                                          WXWPARAM wParam, WXLPARAM lParam)
                                          wxOVERRIDE;
        #endif
    };

#else

    #include "wx/frame.h"
    #define wxSCI_POPUP_IS_FRAME 1

    class wxSCIPopupBase:public wxFrame
    {
    public:
        wxSCIPopupBase(wxWindow*);
        #ifdef __WXMSW__
            virtual bool Show(bool show=true) wxOVERRIDE;
            virtual bool MSWHandleMessage(WXLRESULT *result, WXUINT message,
                                          WXWPARAM wParam, WXLPARAM lParam)
                                          wxOVERRIDE;
        #elif !wxSCI_POPUP_IS_CUSTOM
            virtual bool Show(bool show=true) wxOVERRIDE;
            void ActivateParent();
        #endif
    };

#endif // __WXOSX_COCOA__

class wxSCIPopupWindow:public wxSCIPopupBase
{
public:
    wxSCIPopupWindow(wxWindow*);
    virtual ~wxSCIPopupWindow();
    virtual bool Destroy() wxOVERRIDE;
    virtual bool AcceptsFocus() const wxOVERRIDE;

protected:
    virtual void DoSetSize(int x, int y, int width, int height,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
    void OnParentMove(wxMoveEvent& event);
    #if defined(__WXOSX_COCOA__) || (defined(__WXGTK__)&&!wxSCI_POPUP_IS_FRAME)
        void OnIconize(wxIconizeEvent& event);
    #elif !wxSCI_POPUP_IS_CUSTOM
        void OnFocus(wxFocusEvent& event);
    #endif

private:
    wxPoint   m_initialPosition;
    wxWindow* m_tlw;
};

#endif // WXSCIPOPUP_H__
