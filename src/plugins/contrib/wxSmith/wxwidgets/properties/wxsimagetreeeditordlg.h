/** \file wxsimagetreeeditordlg.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
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
* This code was taken from the wxSmithImage plug-in, copyright Ron Collins
* and released under the GPL.
*
*/

#ifndef WXIMAGETREEEDITORDLG_H
#define WXIMAGETREEEDITORDLG_H

//(*Headers(wxTreeDialog)
#include <wx/treectrl.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/bmpcbox.h>
#include <wx/bmpbuttn.h>
#include <wx/imaglist.h>
#include <wx/button.h>
#include <scrollingdialog.h>
//*)

#include <wx/imaglist.h>
#include <wx/colordlg.h>

#include "../defitems/wxsimagelist.h"

class wxsImageTreeEditorDlg: public wxScrollingDialog
{
    public:

        wxsImageTreeEditorDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~wxsImageTreeEditorDlg();

        bool Execute(wxArrayString &aItems);
        static void ParseTreeItem(wxString aSource, int &outLevel, wxColour &outColour, bool &outBold, int &outImage1, int &outImage2, int &outImage3, int &outImage4, wxString &outText);



        //(*Declarations(wxsImageTreeEditorDlg)
        wxStaticBoxSizer* StaticBoxSizer2;
        wxBitmapComboBox* cbSelected;
        wxStaticText* StaticText13;
        wxCheckBox* cxItemBold;
        wxButton* bCancel;
        wxBitmapButton        *bDelAllItems;
        wxBitmapButton        *bAddItem;
        wxBitmapComboBox* cbNormal;
        wxBitmapButton        *bEditItem;
        wxStaticText* StaticText1;
        wxStaticText* StaticText10;
        wxPanel* Panel2;
        wxStaticText* StaticText3;
        wxBitmapButton        *bDelItem;
        wxBitmapComboBox* cbSelExpanded;
        wxStaticText* StaticText8;
        wxStaticText* StaticText12;
        wxStaticText* StaticText7;
        wxBitmapButton        *bAddSubItem;
        wxImageList* ImageList1;
        wxStaticText* StaticText4;
        wxStaticText* StaticText5;
        wxStaticText* StaticText2;
        wxButton* bItemColor;
        wxStaticText* StaticText6;
        wxTreeCtrl* Tree1;
        wxButton* bOK;
        wxStaticText* StaticText9;
        wxStaticText* StaticText11;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxBitmapComboBox* cbExpanded;
        //*)



    protected:

        //(*Identifiers(wxsImageTreeEditorDlg)
        static const long ID_STATICTEXT1;
        static const long ID_TREECTRL1;
        static const long ID_IMAGEBUTTON1;
        static const long ID_IMAGEBUTTON2;
        static const long ID_STATICTEXT3;
        static const long ID_IMAGEBUTTON3;
        static const long ID_STATICTEXT4;
        static const long ID_IMAGEBUTTON4;
        static const long ID_STATICTEXT5;
        static const long ID_BUTTON3;
        static const long ID_STATICTEXT6;
        static const long ID_CHECKBOX1;
        static const long ID_IMAGEBUTTON5;
        static const long ID_STATICTEXT11;
        static const long ID_STATICTEXT12;
        static const long ID_STATICTEXT13;
        static const long ID_COMBOBOX1;
        static const long ID_STATICTEXT7;
        static const long ID_COMBOBOX2;
        static const long ID_STATICTEXT8;
        static const long ID_COMBOBOX3;
        static const long ID_STATICTEXT9;
        static const long ID_COMBOBOX4;
        static const long ID_STATICTEXT10;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_PANEL2;
        //*)

    private:

        //(*Handlers(wxsImageTreeEditorDlg)
        void OnbAddItemClick(wxCommandEvent& event);
        void OnbAddSubItemClick(wxCommandEvent& event);
        void OnbDelItemClick(wxCommandEvent& event);
        void OnbDelAllItemsClick(wxCommandEvent& event);
        void OnbItemColorClick(wxCommandEvent& event);
        void OncxItemBoldClick(wxCommandEvent& event);
        void OnbEditItemClick(wxCommandEvent& event);
        void OncbNormalSelect(wxCommandEvent& event);
        void OncbSelectedSelect(wxCommandEvent& event);
        void OncbExpandedSelect(wxCommandEvent& event);
        void OncbSelExpandedSelect(wxCommandEvent& event);
        void OnTreeCtrl1SelectionChanged(wxTreeEvent& event);
        void OnbOKClick(wxCommandEvent& event);
        void OnbCancelClick(wxCommandEvent& event);
        //*)

        void        SetImageList(wxImageList &inImageList);
        void        AddItem(wxTreeItemId &inParent);
        void        EncodeTreeItems(wxTreeItemId inParent, int inLevel, wxArrayString &outList);



        wxString                m_sTreeName;            //!< The tree name.
        wxString                m_sImageName;        //!< The image list's name.
        wxImageList            m_imageList;                //!< The image list used to provide state images.

        wxColourDialog    m_colourDialog;

        DECLARE_EVENT_TABLE()
};

#endif
