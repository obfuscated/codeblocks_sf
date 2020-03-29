/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WXSAUIMANAGERPARENTQP_H
#define WXSAUIMANAGERPARENTQP_H

//(*Headers(wxsAuiManagerParentQP)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/radiobox.h>
//*)

#include "wxsAuiManager.h"
#include <wxsadvqppchild.h>

class wxsAuiManagerParentQP: public wxsAdvQPPChild
{
    public:

        wxsAuiManagerParentQP(wxsAdvQPP* parent,wxsAuiPaneInfoExtra* Extra, wxWindowID id = -1);
        virtual ~wxsAuiManagerParentQP();

        //(*Declarations(wxsAuiManagerParentQP)
        wxFlexGridSizer* FlexGridSizer1;
        wxCheckBox* BottomDockable;
        wxSpinCtrl* Row;
        wxStaticLine* StaticLine2;
        wxSpinCtrl* Layer;
        wxCheckBox* DestroyOnClose;
        wxCheckBox* MaximizeButton;
        wxCheckBox* DockTop;
        wxCheckBox* LeftDockable;
        wxCheckBox* PaneBorder;
        wxRadioBox* Gripper;
        wxCheckBox* DockCenter;
        wxStaticText* StaticText1;
        wxCheckBox* CaptionVisible;
        wxCheckBox* TopDockable;
        wxTextCtrl* Caption;
        wxStaticText* StaticText3;
        wxCheckBox* Movable;
        wxStaticLine* StaticLine1;
        wxCheckBox* CloseButton;
        wxCheckBox* Visible;
        wxCheckBox* MinimizeButton;
        wxSpinCtrl* Position;
        wxCheckBox* RightDockable;
        wxCheckBox* DockRight;
        wxStaticText* StaticText4;
        wxCheckBox* PinButton;
        wxCheckBox* Floatable;
        wxStaticText* StaticText2;
        wxCheckBox* DockFixed;
        wxTextCtrl* Name;
        wxCheckBox* DockBottom;
        wxChoice* StandardPane;
        wxCheckBox* Docked;
        wxCheckBox* DockLeft;
        wxCheckBox* Resizable;
        //*)

    protected:

        //(*Identifiers(wxsAuiManagerParentQP)
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL1;
        static const long ID_CHECKBOX18;
        static const long ID_CHECKBOX15;
        static const long ID_CHECKBOX20;
        static const long ID_CHECKBOX21;
        static const long ID_CHECKBOX19;
        static const long ID_CHECKBOX22;
        static const long ID_CHOICE1;
        static const long ID_TEXTCTRL2;
        static const long ID_CHECKBOX7;
        static const long ID_CHECKBOX9;
        static const long ID_CHECKBOX11;
        static const long ID_CHECKBOX10;
        static const long ID_CHECKBOX12;
        static const long ID_CHECKBOX6;
        static const long ID_CHECKBOX8;
        static const long ID_STATICTEXT1;
        static const long ID_SPINCTRL1;
        static const long ID_STATICLINE1;
        static const long ID_STATICTEXT2;
        static const long ID_SPINCTRL2;
        static const long ID_STATICLINE2;
        static const long ID_STATICTEXT3;
        static const long ID_SPINCTRL3;
        static const long ID_CHECKBOX1;
        static const long ID_CHECKBOX2;
        static const long ID_CHECKBOX5;
        static const long ID_CHECKBOX3;
        static const long ID_CHECKBOX4;
        static const long ID_CHECKBOX13;
        static const long ID_CHECKBOX14;
        static const long ID_CHECKBOX16;
        static const long ID_CHECKBOX17;
        static const long ID_RADIOBOX1;
        //*)

        virtual void Update();

    private:

        //(*Handlers(wxsAuiManagerParentQP)
        void OnDockChange(wxCommandEvent& event);
        void OnDockDirectionChange(wxCommandEvent& event);
        void OnDockSiteChange(wxSpinEvent& event);
        void OnNameChange(wxCommandEvent& event);
        void OnCaptionChange(wxCommandEvent& event);
        void OnCaptionButtonClick(wxCommandEvent& event);
        void OnDockableChange(wxCommandEvent& event);
        void OnGripperSelect(wxCommandEvent& event);
        void OnGeneralChange(wxCommandEvent& event);
        void OnCaptionVisibleClick(wxCommandEvent& event);
        void OnStandardPaneChange(wxCommandEvent& event);
        //*)

        void ReadData();

        wxsAuiPaneInfoExtra* m_Extra;

        DECLARE_EVENT_TABLE()
};

#endif
