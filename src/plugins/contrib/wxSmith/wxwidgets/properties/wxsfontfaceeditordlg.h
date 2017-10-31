/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSFONTFACEEDITORDLG_H
#define WXSFONTFACEEDITORDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(wxsFontFaceEditorDlg)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include "scrollingdialog.h"
//*)

class wxsFontFaceEditorDlg: public wxScrollingDialog
{
    public:

        wxsFontFaceEditorDlg(wxWindow* parent,wxString& Face,wxWindowID id = -1);
        virtual ~wxsFontFaceEditorDlg();

        //(*Identifiers(wxsFontFaceEditorDlg)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON1;
        //*)

    protected:

        //(*Handlers(wxsFontFaceEditorDlg)
        void OnButton2Click(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsFontFaceEditorDlg)
        wxTextCtrl* FaceName;
        wxButton* Button1;
        wxButton* Button2;
        wxButton* Button3;
        //*)

    private:

        wxString& Face;

        DECLARE_EVENT_TABLE()
};

#endif
