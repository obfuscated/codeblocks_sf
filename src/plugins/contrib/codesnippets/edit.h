//////////////////////////////////////////////////////////////////////////////
// File:        edit.h
// Purpose:     wxScintilla test module
// Maintainer:  Otto Wyss
// Created:     2003-09-01
// RCS-ID:      $Id: edit.h 1960 2006-02-07 13:36:51Z thomasdenk $
// Copyright:   (c) 2004 wxCode
// Licence:     wxWindows
//////////////////////////////////////////////////////////////////////////////

#ifndef _EDIT_H_
#define _EDIT_H_

//----------------------------------------------------------------------------
// informations
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

//! wxWindows headers

//! wxScintilla headers
#include <wx/wxscintilla.h>  // scintilla edit control

//! application headers
#include "prefs.h"       // preferences
#include "finddialogs.h"       // preferences


//============================================================================
// declarations
//============================================================================

class EditPrint;
class EditProperties;
class DragScrollEvents;


//----------------------------------------------------------------------------
//! Edit
class Edit: public wxScintilla {
    friend class EditProperties;
    friend class EditPrint;

public:
    //! constructor
    Edit (wxWindow *parent, wxWindowID id = -1,
          const wxPoint &pos = wxDefaultPosition,
          const wxSize &size = wxDefaultSize,
          long style = wxSUNKEN_BORDER|wxVSCROLL
         );

    //! destructor
    ~Edit ();

    // event handlers
    // common
    void OnSize( wxSizeEvent &event );
    // edit
    void OnEditRedo (wxCommandEvent &event);
    void OnEditUndo (wxCommandEvent &event);
    void OnEditClear (wxCommandEvent &event);
    void OnEditCut (wxCommandEvent &event);
    void OnEditCopy (wxCommandEvent &event);
    void OnEditPaste (wxCommandEvent &event);
    // find
    void OnFind (wxCommandEvent &event);
    void OnFindUI (wxUpdateUIEvent &event);
    void OnFindNext (wxCommandEvent &event);
    void OnFindNextUI (wxUpdateUIEvent &event);
    void OnFindPrev (wxCommandEvent &event);
    void OnFindPrevUI (wxUpdateUIEvent &event);
    // replace
    void OnReplace (wxCommandEvent &event);
    void OnReplaceUI (wxUpdateUIEvent &event);
    void OnReplaceNext (wxCommandEvent &event);
    void OnReplaceNextUI (wxUpdateUIEvent &event);
    void OnReplaceAll (wxCommandEvent& event);

    void OnBraceMatch (wxCommandEvent &event);
    void OnBraceMatchUI (wxUpdateUIEvent &event);


    void OnGoto (wxCommandEvent &event);
    void OnGotoUI (wxUpdateUIEvent &event);

    void OnEditIndentInc (wxCommandEvent &event);
    void OnEditIndentRed (wxCommandEvent &event);
    void OnEditSelectAll (wxCommandEvent &event);
    void OnEditSelectLine (wxCommandEvent &event);
    //! view
    void OnHilightLang (wxCommandEvent &event);
    void OnDisplayEOL (wxCommandEvent &event);
    void OnIndentGuide (wxCommandEvent &event);
    void OnLineNumber (wxCommandEvent &event);
    void OnLongLineOn (wxCommandEvent &event);
    void OnWhiteSpace (wxCommandEvent &event);
    void OnFoldToggle (wxCommandEvent &event);
    void OnSetOverType (wxCommandEvent &event);
    void OnSetReadOnly (wxCommandEvent &event);
    void OnWrapmodeOn (wxCommandEvent &event);
    void OnUseCharset (wxCommandEvent &event);
    //! extra
    void OnChangeCase (wxCommandEvent &event);
    void OnConvertEOL (wxCommandEvent &event);
    // styled text
    void OnMarginClick (wxScintillaEvent &event);
    void OnCharAdded  (wxScintillaEvent &event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void OnScroll(wxScrollEvent& event);
    void OnScrollWin(wxScrollWinEvent& event);


    //! language/lexer
    wxString DeterminePrefs (const wxString &filename);
    bool InitializePrefs (const wxString &filename);
    bool UserSettings (const wxString &filename);
    LanguageInfo const* GetLanguageInfo () {return m_language;};

    //! load/save file
    bool LoadFile ();
    bool LoadFile (const wxString &filename);
    bool SaveFile ();
    bool SaveFile (const wxString &filename);
    bool Modified ();
    wxString GetFilename () {return m_filename;};
    void SetFilename (const wxString &filename) {m_filename = filename;};
    int GetLongestLinePixelWidth( int top_line = -1, int bottom_line = -1); //(pecan 2007/4/04)

private:

    int FindString(const wxString &str, int flags);

    void    InitDragScroller();
    DragScrollEvents* pDragScroller;        //(pecan 2007/3/29)
    wxColour          m_SysWinBkgdColour;   //(pecan 2007/3/27)
    // file
    wxString m_filename;

    // lanugage properties
    LanguageInfo const* m_language;

    // margin variables
    int m_LineNrID;
    int m_LineNrMargin;
    int m_FoldingID;
    int m_FoldingMargin;
    int m_DividerID;

    // find variables
    myFindReplaceDlg* m_FindReplaceDlg;
    int         m_startpos;
    bool        m_replace;
    myGotoDlg*  m_GotoDlg;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
//! EditProperties
class EditProperties: public wxDialog {

public:

    //! constructor
    EditProperties (Edit *edit, long style = 0);

private:

};

//----------------------------------------------------------------------------
//! EditPrint
class EditPrint: public wxPrintout {

public:

    //! constructor
    EditPrint (Edit *edit, wxChar *title = _T(""));

    //! event handlers
    bool OnPrintPage (int page);
    bool OnBeginDocument (int startPage, int endPage);

    //! print functions
    bool HasPage (int page);
    void GetPageInfo (int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

private:
    Edit *m_edit;
    int m_printed;
    wxRect m_pageRect;
    wxRect m_printRect;

    bool PrintScaling (wxDC *dc);
};

#endif // _EDIT_H_

#ifndef DRAGSCROLL_H
#define DRAGSCROLL_H

// ----------------------------------------------------------------------------
//      MOUSE DRAG and SCROLL CLASS
// ----------------------------------------------------------------------------
// Dragging modes
#define DRAG_NONE     0
#define DRAG_START    1
#define DRAG_DRAGGING 2
// ----------------------------------------------------------------------------
class DragScrollEvents : public wxEvtHandler
// ----------------------------------------------------------------------------
{

    public:
        DragScrollEvents(wxWindow *window);
        ~DragScrollEvents();

        void OnMouseEvent(wxMouseEvent& event);
    private:


        wxWindow*   m_Window;
        wxWindow*   m_pMS_Window;
        int         m_DragMode;
        wxPoint     m_DragStartPos;
        wxObject*   m_pEvtObject;
        bool        m_MouseHasMoved;
        double      m_MouseMoveToLineMoveRatio;
        double      m_RatioX, m_RatioY;
        int         m_StartX, m_StartY;
        int         m_InitX,  m_InitY;
        // Scroll Direction move -1(mouse direction) +1(reverse mouse direction)
        int         m_Direction;
        unsigned    m_gtkContextDelay;

        bool MouseDragScrollEnabled ;   //Enable/Disable mouse event handler
        bool MouseEditorFocusEnabled;   //Enable/Disable mouse focus() editor
        bool MouseFocusEnabled      ;   //focus follows mouse
        int  MouseDragDirection     ;   //Move with or opposite mouse
        int  MouseDragKey           ;   //Right or Middle mouse key
        int  MouseDragSensitivity   ;   //Adaptive speed sensitivity
        int  MouseToLineRatio       ;   //Percentage of mouse moves that make a line
        //bool MouseRightKeyCtrl    ;   //Hide Right mouse down from ListCtrl windows
        int  MouseContextDelay      ;   //Linux context menu delay to catch possible mouse scroll move

        bool GetMouseDragScrollEnabled() { return MouseDragScrollEnabled; }
        bool GetMouseEditorFocusEnabled(){ return MouseEditorFocusEnabled; }
        int  GetMouseDragDirection()     { return MouseDragDirection; }
        bool GetMouseFocusEnabled()      { return MouseFocusEnabled; }
        int  GetMouseDragKey()           { return MouseDragKey; }
        int  GetMouseDragSensitivity()   { return MouseDragSensitivity; }
        int  GetMouseToLineRatio()       { return MouseToLineRatio; }
        //int  GetMouseRightKeyCtrl()      { return MouseRightKeyCtrl; }
        int  GetMouseContextDelay()      { return MouseContextDelay; }


        bool KeyDown(wxMouseEvent& event)
            { if ( 0 ==  GetMouseDragKey() )
                return event.RightDown();
                return event.MiddleDown();
            }
        bool KeyIsDown(wxMouseEvent& event)
            { if ( 0 ==  GetMouseDragKey() )
                return event.RightIsDown();
                return event.MiddleIsDown();
            }
        bool KeyUp(wxMouseEvent& event)
            { if ( 0 ==  GetMouseDragKey() )
                return event.RightUp();
                return event.MiddleUp();
            }
        int GetUserDragKey()
            { return ( GetMouseDragKey() ? wxMOUSE_BTN_MIDDLE:wxMOUSE_BTN_RIGHT );}

        DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
#endif //DragScrollEvents
