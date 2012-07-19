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

#ifndef WXSTOOLBAREDITOR_H
#define WXSTOOLBAREDITOR_H

#include <wx/wxprec.h>

#include "wxstoolbar.h"

//(*Headers(wxsToolBarEditor)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/radiobut.h>
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
//*)

class wxsToolBarEditor: public wxPanel
{
    public:

        wxsToolBarEditor(wxWindow* parent,wxsToolBar* ToolBar);
        ~wxsToolBarEditor();

        void ApplyChanges();

    private:

        //(*Identifiers(wxsToolBarEditor)
        static const long ID_LISTBOX1;
        static const long ID_RADIOBUTTON1;
        static const long ID_RADIOBUTTON2;
        static const long ID_RADIOBUTTON3;
        static const long ID_RADIOBUTTON4;
        static const long ID_STATICLINE1;
        static const long ID_STATICTEXT6;
        static const long ID_TEXTCTRL4;
        static const long ID_STATICTEXT1;
        static const long ID_TEXTCTRL1;
        static const long ID_STATICTEXT4;
        static const long ID_BITMAPBUTTON1;
        static const long ID_STATICTEXT5;
        static const long ID_BITMAPBUTTON2;
        static const long ID_STATICTEXT2;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL3;
        static const long ID_STATICLINE2;
        static const long ID_BUTTON3;
        static const long ID_BUTTON4;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        //*)

        //(*Handlers(wxsToolBarEditor)
        void OnTypeChanged(wxCommandEvent& event);
        void Onm_ContentSelect(wxCommandEvent& event);
        void Onm_LabelText(wxCommandEvent& event);
        void OnUpClick(wxCommandEvent& event);
        void OnDownClick(wxCommandEvent& event);
        void OnNewClick(wxCommandEvent& event);
        void OnDelClick(wxCommandEvent& event);
        void OnBitmapClick(wxCommandEvent& event);
        void OnBitmap2Click(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsToolBarEditor)
        wxStaticBoxSizer* StaticBoxSizer2;
        wxBitmapButton* m_Bitmap2;
        wxButton* Button4;
        wxStaticText* StaticText2;
        wxTextCtrl* m_HelpText;
        wxRadioButton* m_TypeNormal;
        wxButton* Button1;
        wxStaticText* StaticText6;
        wxRadioButton* m_TypeRadio;
        wxStaticText* StaticText1;
        wxBoxSizer* BoxSizer2;
        wxStaticText* StaticText3;
        wxButton* Button2;
        wxBitmapButton* m_Bitmap;
        wxStaticLine* StaticLine2;
        wxRadioButton* m_TypeCheck;
        wxRadioButton* m_TypeSeparator;
        wxButton* Button3;
        wxGridSizer* GridSizer1;
        wxStaticText* StaticText5;
        wxStaticLine* StaticLine1;
        wxBoxSizer* BoxSizer1;
        wxTextCtrl* m_ToolTip;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxFlexGridSizer* FlexGridSizer1;
        wxBoxSizer* BoxSizer3;
        wxStaticText* StaticText4;
        wxListBox* m_Content;
        wxTextCtrl* m_Label;
        wxTextCtrl* m_Id;
        //*)

        enum ItemType
        {
            Normal,
            Check,
            Radio,
            Separator,
            Control
        };

        class ToolBarItem: public wxClientData
        {
            public:

                ToolBarItem(): m_OriginalPos(-1), m_Type(Normal) {}

                ToolBarItem(const ToolBarItem& CopyFrom):
                    m_OriginalPos(CopyFrom.m_OriginalPos),
                    m_Type(CopyFrom.m_Type),
                    m_Id(CopyFrom.m_Id),
                    m_Label(CopyFrom.m_Label),
                    m_Bitmap(CopyFrom.m_Bitmap),
                    m_Bitmap2(CopyFrom.m_Bitmap2),
                    m_ToolTip(CopyFrom.m_ToolTip),
                    m_HelpText(CopyFrom.m_HelpText),
                    m_Handler1(CopyFrom.m_Handler1),
                    m_Handler2(CopyFrom.m_Handler2) {}

                int m_OriginalPos;
                ItemType m_Type;
                wxString m_Id;
                wxString m_Label;
                wxsBitmapData m_Bitmap;
                wxsBitmapData m_Bitmap2;
                wxString m_ToolTip;
                wxString m_HelpText;

                wxString m_Handler1;
                wxString m_Handler2;
        };


        wxString GetItemLabel(ToolBarItem* Item);
        void SelectItem(ToolBarItem* Item);


        ToolBarItem* m_Selected;
        wxsToolBar* m_ToolBar;
        bool m_BlockTextChange;
        bool m_BlockSelect;

        DECLARE_EVENT_TABLE()
};

#endif
