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

#ifndef WXSBITMAPICONEDITORDLG_H
#define WXSBITMAPICONEDITORDLG_H

//(*Headers(wxsBitmapIconEditorDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include "scrollingdialog.h"
#include <wx/combobox.h>
//*)
#include <wx/timer.h>

#include "wxsbitmapiconproperty.h"

class wxsBitmapIconEditorDlg: public wxScrollingDialog
{
    public:

        wxsBitmapIconEditorDlg(wxWindow* parent,wxsBitmapIconData& Data,const wxString& DefaultClient);
        virtual ~wxsBitmapIconEditorDlg();

        //(*Identifiers(wxsBitmapIconEditorDlg)
        static const long ID_RADIOBUTTON1;
        static const long ID_RADIOBUTTON2;
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON3;
        static const long ID_RADIOBUTTON3;
        static const long ID_STATICTEXT1;
        static const long ID_COMBOBOX2;
        static const long ID_STATICTEXT2;
        static const long ID_COMBOBOX1;
        static const long ID_RADIOBUTTON4;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICBITMAP1;
        static const long ID_BUTTON1;
        //*)

    protected:

        //(*Handlers(wxsBitmapIconEditorDlg)
        void OnTimer(wxTimerEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnFileNameText(wxCommandEvent& event);
        void OnButton3Click(wxCommandEvent& event);
        void OnArtIdSelect(wxCommandEvent& event);
        void OnUpdatePreview(wxCommandEvent& event);
        void OnOtherTextText(wxCommandEvent& event);
        void OnCodeTextText(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsBitmapIconEditorDlg)
        wxStaticBoxSizer* StaticBoxSizer2;
        wxStaticBitmap* Preview;
        wxRadioButton* ImageArt;
        wxRadioButton* ImageCode;
        wxStaticText* StaticText2;
        wxTextCtrl* CodeText;
        wxButton* Button1;
        wxTextCtrl* FileName;
        wxFlexGridSizer* FlexGridSizer2;
        wxStaticText* StaticText1;
        wxButton* Button2;
        wxRadioButton* ImageFile;
        wxButton* Button3;
        wxComboBox* ArtClient;
        wxRadioButton* NoImage;
        wxComboBox* ArtId;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxFlexGridSizer* FlexGridSizer1;
        //*)
        wxTimer* Timer1;

    private:

        void UpdatePreview();
        void WriteData(wxsBitmapIconData& _Data);
        void ReadData(wxsBitmapIconData& _Data);

        wxString DefaultClient;
        wxsBitmapIconData& Data;

        DECLARE_EVENT_TABLE()
};

#endif
