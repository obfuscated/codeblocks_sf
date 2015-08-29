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

#ifndef WXSSIZERPARENTQP_H
#define WXSSIZERPARENTQP_H

//(*Headers(wxsSizerParentQP)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/radiobut.h>
#include <wx/panel.h>
#include <wx/statline.h>
//*)

#include "wxssizer.h"
#include "../wxsadvqppchild.h"

class wxsSizerParentQP: public wxsAdvQPPChild
{
    public:

        wxsSizerParentQP(wxsAdvQPP* parent,wxsSizerExtra* Extra,wxWindowID id = -1);
        virtual ~wxsSizerParentQP();

        //(*Identifiers(wxsSizerParentQP)
        static const long ID_CHECKBOX1;
        static const long ID_CHECKBOX2;
        static const long ID_CHECKBOX8;
        static const long ID_CHECKBOX3;
        static const long ID_CHECKBOX4;
        static const long ID_SPINCTRL1;
        static const long ID_CHECKBOX7;
        static const long ID_RADIOBUTTON4;
        static const long ID_RADIOBUTTON5;
        static const long ID_RADIOBUTTON6;
        static const long ID_RADIOBUTTON7;
        static const long ID_RADIOBUTTON8;
        static const long ID_RADIOBUTTON9;
        static const long ID_RADIOBUTTON10;
        static const long ID_RADIOBUTTON11;
        static const long ID_RADIOBUTTON12;
        static const long ID_STATICLINE1;
        static const long ID_CHECKBOX6;
        static const long ID_CHECKBOX5;
        static const long ID_SPINCTRL2;
        //*)

    protected:

        //(*Handlers(wxsSizerParentQP)
        void OnBrdChange(wxCommandEvent& event);
        void OnBrdSizeChange(wxSpinEvent& event);
        void OnPlaceChange(wxCommandEvent& event);
        void OnProportionChange(wxSpinEvent& event);
        void OnBrdDlgChange(wxCommandEvent& event);
        void OnBrdAll(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsSizerParentQP)
        wxRadioButton* PlaceRB;
        wxCheckBox* BrdDlg;
        wxRadioButton* PlaceCB;
        wxRadioButton* PlaceRT;
        wxRadioButton* PlaceLT;
        wxCheckBox* BrdLeft;
        wxCheckBox* BrdTop;
        wxSpinCtrl* Proportion;
        wxRadioButton* PlaceLB;
        wxStaticLine* StaticLine1;
        wxCheckBox* PlaceShp;
        wxCheckBox* PlaceExp;
        wxRadioButton* PlaceRC;
        wxRadioButton* PlaceCC;
        wxCheckBox* BrdAll;
        wxRadioButton* PlaceLC;
        wxCheckBox* BrdRight;
        wxSpinCtrl* BrdSize;
        wxCheckBox* BrdBottom;
        wxRadioButton* PlaceCT;
        //*)

        virtual void Update();

    private:

        void ReadData();
        void SaveData();

        wxsSizerExtra* m_Extra;
        long m_ParentOrientation;

        DECLARE_EVENT_TABLE()
};

#endif
