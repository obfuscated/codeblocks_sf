#ifndef GenericMessageBox_H
#define GenericMessageBox_H


////class GenericMessageBox
////{
////    public:
////        GenericMessageBox();
////        virtual ~GenericMessageBox();
////    protected:
////    private:
////};

#endif // GenericMessageBox_H

/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/msgdlgg.h
// Purpose:     Generic wxMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef GenericMessageDialog_h
#define GenericMessageDialog_h

#include "wx/defs.h"
#include "scrollingdialog.h"
#include "wx/msgdlg.h"
#include "wx/generic/msgdlgg.h"

// type is an 'or' (|) of wxOK, wxCANCEL, wxYES_NO
// Returns wxYES/NO/OK/CANCEL

//-extern WXDLLEXPORT_DATA(const wxChar) wxMessageBoxCaptionStr[];

// ----------------------------------------------------------------------------
class GenericMessageDialog: public wxScrollingDialog, public wxMessageDialogBase
// ----------------------------------------------------------------------------
{
DECLARE_DYNAMIC_CLASS(GenericMessageDialog)

public:
    GenericMessageDialog(wxWindow *parent, const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    void OnYes(wxCommandEvent& event);
    void OnNo(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// GenericMessageBox: the simplest way to use GenericMessageDialog
// ----------------------------------------------------------------------------

int GenericMessageBox(const wxString& message,
                             const wxString& caption = wxMessageBoxCaptionStr,
                             long style = wxOK | wxCENTRE,
                             wxWindow *parent = ::wxGetActiveWindow(),
                             int x = wxDefaultCoord, int y = wxDefaultCoord);


#endif // GenericMessageDialog.h
