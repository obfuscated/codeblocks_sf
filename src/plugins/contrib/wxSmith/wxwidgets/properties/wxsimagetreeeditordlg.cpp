/** \file wxsimagetreeeditordlg.cpp
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

#include "wxsimagetreeeditordlg.h"

//(*InternalHeaders(wxsImageTreeEditDialog)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/msgdlg.h>
#include "../properties/wxsimagelisteditordlg.h"


//(*IdInit(wxsImageTreeEditorDlg)
const long wxsImageTreeEditorDlg::ID_STATICTEXT1 = wxNewId();
const long wxsImageTreeEditorDlg::ID_TREECTRL1 = wxNewId();
const long wxsImageTreeEditorDlg::ID_IMAGEBUTTON1 = wxNewId();
const long wxsImageTreeEditorDlg::ID_IMAGEBUTTON2 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT3 = wxNewId();
const long wxsImageTreeEditorDlg::ID_IMAGEBUTTON3 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT4 = wxNewId();
const long wxsImageTreeEditorDlg::ID_IMAGEBUTTON4 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT5 = wxNewId();
const long wxsImageTreeEditorDlg::ID_BUTTON3 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT6 = wxNewId();
const long wxsImageTreeEditorDlg::ID_CHECKBOX1 = wxNewId();
const long wxsImageTreeEditorDlg::ID_IMAGEBUTTON5 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT11 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT12 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT13 = wxNewId();
const long wxsImageTreeEditorDlg::ID_COMBOBOX1 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT7 = wxNewId();
const long wxsImageTreeEditorDlg::ID_COMBOBOX2 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT8 = wxNewId();
const long wxsImageTreeEditorDlg::ID_COMBOBOX3 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT9 = wxNewId();
const long wxsImageTreeEditorDlg::ID_COMBOBOX4 = wxNewId();
const long wxsImageTreeEditorDlg::ID_STATICTEXT10 = wxNewId();
const long wxsImageTreeEditorDlg::ID_BUTTON1 = wxNewId();
const long wxsImageTreeEditorDlg::ID_BUTTON2 = wxNewId();
const long wxsImageTreeEditorDlg::ID_PANEL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsImageTreeEditorDlg, wxDialog)
    //(*EventTable(wxsImageTreeEditorDlg)
    //*)
END_EVENT_TABLE()

wxsImageTreeEditorDlg::wxsImageTreeEditorDlg(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
{
    //(*Initialize(wxsImageTreeEditorDlg)
    wxGridSizer* GridSizer1;
    wxBoxSizer* BoxSizer3;
    wxBoxSizer* BoxSizer10;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer11;
    wxBoxSizer* BoxSizer13;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer9;
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer8;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer12;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;

    Create(parent, wxID_ANY, _("Tree Item Editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(480,468));
    wxFont thisFont(8,wxSWISS,wxFONTSTYLE_NORMAL,wxNORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
    SetFont(thisFont);
    Panel2 = new wxPanel(this, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(Panel2, ID_STATICTEXT1, _("Edit Tree Items"), wxPoint(0,0), wxSize(400,20), wxST_NO_AUTORESIZE|wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
    wxFont StaticText1Font(10,wxSWISS,wxFONTSTYLE_NORMAL,wxBOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
    StaticText1->SetFont(StaticText1Font);
    BoxSizer2->Add(StaticText1, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("tree-name"));
    Tree1 = new wxTreeCtrl(Panel2, ID_TREECTRL1, wxPoint(2,36), wxSize(246,359), wxTR_EDIT_LABELS|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL1"));
    StaticBoxSizer1->Add(Tree1, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer5->Add(StaticBoxSizer1, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer3->Add(BoxSizer5, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Attributes"));
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    bAddItem = new wxBitmapButton(Panel2, ID_IMAGEBUTTON1, wxNullBitmap, wxPoint(256,36), wxSize(24,23), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_IMAGEBUTTON1"));
    bAddItem->SetToolTip(_("Add A New Item"));
    BoxSizer7->Add(bAddItem, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticText2 = new wxStaticText(Panel2, wxID_ANY, _("Add Item"), wxPoint(290,40), wxDefaultSize, 0, _T("wxID_ANY"));
    BoxSizer7->Add(StaticText2, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer7, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    bAddSubItem = new wxBitmapButton(Panel2, ID_IMAGEBUTTON2, wxNullBitmap, wxPoint(256,66), wxSize(24,23), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_IMAGEBUTTON2"));
    bAddSubItem->SetToolTip(_("Add A New Child"));
    BoxSizer8->Add(bAddSubItem, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticText3 = new wxStaticText(Panel2, ID_STATICTEXT3, _("Add Sub-Item"), wxPoint(290,70), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    BoxSizer8->Add(StaticText3, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer8, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    bDelItem = new wxBitmapButton(Panel2, ID_IMAGEBUTTON3, wxNullBitmap, wxPoint(256,96), wxSize(24,23), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_IMAGEBUTTON3"));
    bDelItem->SetToolTip(_("Delete Current Item"));
    BoxSizer9->Add(bDelItem, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticText4 = new wxStaticText(Panel2, ID_STATICTEXT4, _("Delete Current Item"), wxPoint(290,100), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    BoxSizer9->Add(StaticText4, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer9, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    bDelAllItems = new wxBitmapButton(Panel2, ID_IMAGEBUTTON4, wxNullBitmap, wxPoint(256,126), wxSize(24,23), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_IMAGEBUTTON4"));
    bDelAllItems->SetToolTip(_("Delete All Items"));
    BoxSizer10->Add(bDelAllItems, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticText5 = new wxStaticText(Panel2, ID_STATICTEXT5, _("Delete All Items"), wxPoint(290,130), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    BoxSizer10->Add(StaticText5, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer10, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    bItemColor = new wxButton(Panel2, ID_BUTTON3, _("C"), wxPoint(256,156), wxSize(24,24), 0, wxDefaultValidator, _T("ID_BUTTON3"));
    wxFont bItemColorFont(10,wxSWISS,wxFONTSTYLE_NORMAL,wxBOLD,false,_T("Arial Black"),wxFONTENCODING_DEFAULT);
    bItemColor->SetFont(bItemColorFont);
    bItemColor->SetToolTip(_("Set Item Text Color"));
    BoxSizer11->Add(bItemColor, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticText6 = new wxStaticText(Panel2, ID_STATICTEXT6, _("Set Item Text Color"), wxPoint(290,160), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    BoxSizer11->Add(StaticText6, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer11, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    cxItemBold = new wxCheckBox(Panel2, ID_CHECKBOX1, _("    Set Item Text Bold"), wxPoint(262,192), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    cxItemBold->SetValue(false);
    cxItemBold->SetToolTip(_("Set Item Text Bold"));
    BoxSizer12->Add(cxItemBold, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticBoxSizer2->Add(BoxSizer12, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    bEditItem = new wxBitmapButton(Panel2, ID_IMAGEBUTTON5, wxNullBitmap, wxPoint(256,216), wxSize(24,23), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_IMAGEBUTTON5"));
    bEditItem->SetToolTip(_("Start Editor On Current Item"));
    BoxSizer13->Add(bEditItem, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticText11 = new wxStaticText(Panel2, ID_STATICTEXT11, _("Edit Current Item"), wxPoint(290,220), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    BoxSizer13->Add(StaticText11, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer13, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    GridSizer1 = new wxGridSizer(5, 2, 0, 0);
    StaticText12 = new wxStaticText(Panel2, ID_STATICTEXT12, _("Image-List"), wxPoint(256,272), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    GridSizer1->Add(StaticText12, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticText13 = new wxStaticText(Panel2, ID_STATICTEXT13, _("Label"), wxPoint(310,272), wxSize(82,14), wxST_NO_AUTORESIZE, _T("ID_STATICTEXT13"));
    StaticText13->SetForegroundColour(wxColour(0,0,255));
    GridSizer1->Add(StaticText13, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    cbNormal = new wxBitmapComboBox(Panel2, ID_COMBOBOX1, wxEmptyString, wxPoint(256,296), wxSize(48,22), 0, NULL, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX1"));
    GridSizer1->Add(cbNormal, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 3);
    StaticText7 = new wxStaticText(Panel2, ID_STATICTEXT7, _("Normal Image"), wxPoint(310,300), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    GridSizer1->Add(StaticText7, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
    cbSelected = new wxBitmapComboBox(Panel2, ID_COMBOBOX2, wxEmptyString, wxPoint(256,326), wxSize(48,22), 0, NULL, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX2"));
    GridSizer1->Add(cbSelected, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
    StaticText8 = new wxStaticText(Panel2, ID_STATICTEXT8, _("Selected Image"), wxPoint(310,330), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    GridSizer1->Add(StaticText8, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
    cbExpanded = new wxBitmapComboBox(Panel2, ID_COMBOBOX3, wxEmptyString, wxPoint(256,356), wxSize(48,22), 0, NULL, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX3"));
    GridSizer1->Add(cbExpanded, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
    StaticText9 = new wxStaticText(Panel2, ID_STATICTEXT9, _("Expanded Image"), wxPoint(310,360), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    GridSizer1->Add(StaticText9, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
    cbSelExpanded = new wxBitmapComboBox(Panel2, ID_COMBOBOX4, wxEmptyString, wxPoint(256,386), wxSize(48,22), 0, NULL, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX4"));
    GridSizer1->Add(cbSelExpanded, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 3);
    StaticText10 = new wxStaticText(Panel2, ID_STATICTEXT10, _("Sel+Exp Image"), wxPoint(310,390), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    GridSizer1->Add(StaticText10, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 3);
    StaticBoxSizer2->Add(GridSizer1, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer6->Add(StaticBoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer3->Add(BoxSizer6, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer1->Add(BoxSizer3, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    bOK = new wxButton(Panel2, ID_BUTTON1, _("OK"), wxPoint(48,440), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    bOK->SetDefault();
    BoxSizer4->Add(bOK, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer4->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    bCancel = new wxButton(Panel2, ID_BUTTON2, _("Cancel"), wxPoint(280,440), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer4->Add(bCancel, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer1->Add(BoxSizer4, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
    Panel2->SetSizer(BoxSizer1);
    BoxSizer1->Fit(Panel2);
    BoxSizer1->SetSizeHints(Panel2);
    static const char * ImageList1_0_XPM[] = {
    "16 16 3 1",
    "     c None",
    ".    c #000000",
    "+    c #00C000",
    "        ....... ",
    "        .+++++. ",
    "   ..   .+++++. ",
    "   ..   .+++++. ",
    " ...... .+++++. ",
    " ...... .+++++. ",
    "   ..   .+++++. ",
    "   ..   .+++++. ",
    "        .+++++. ",
    "        .+++++. ",
    "        .+++++. ",
    "        .+++++. ",
    "        .++.++. ",
    "        .+. .+. ",
    "        ..   .. ",
    "................"};
    static const char *ImageList1_1_XPM[] = {
    "16 16 4 1",
    "o c Black",
    ". c #000080",
    "X c #0000FF",
    "_ c None",
    "________......._",
    "________.XXXXX._",
    "___oo___.XXXXX._",
    "___oo___.XXXXX._",
    "_oooooo_.XXXXX._",
    "_oooooo_.XXXXX._",
    "___oo___.XXXXX._",
    "___oo___.XXXXX._",
    "________.XXXXX._",
    "________.XXXXX._",
    "________.XXXXX._",
    "________.XXXXX._",
    "________.XX.XX._",
    "________.X._.X._",
    "________..___.._",
    "oooooooooooooooo"
    };
    static const char *ImageList1_2_XPM[] = {
    "16 16 3 1",
    ". c Black",
    "_ c None",
    "X c #FF4040",
    "________......._",
    "________.XXXXX._",
    "__.___._.XXXXX._",
    "__.._.._.XXXXX._",
    "___...__.XXXXX._",
    "____.___.XXXXX._",
    "___...__.XXXXX._",
    "__.._.._.XXXXX._",
    "__.___._.XXXXX._",
    "________.XXXXX._",
    "________.XXXXX._",
    "________.XXXXX._",
    "________.XX.XX._",
    "________.X._.X._",
    "________..___.._",
    "................"
    };
    static const char *ImageList1_3_XPM[] = {
    "16 16 22 1",
    "4 c Black",
    "3 c #A5AEBD",
    "= c #5478B4",
    "1 c #95A3BB",
    "O c #9AA7BC",
    ": c #758EB7",
    "$ c #6986B6",
    "# c #4971B2",
    "* c #8A9CBA",
    "X c #8598B9",
    "o c #ABB2BE",
    "; c #7F95B9",
    "- c #4E74B3",
    "2 c #A0ABBC",
    "+ c #6F8AB7",
    "_ c None",
    ". c #B5B9BF",
    "@ c #3E69B1",
    "< c #90A0BA",
    "> c #6483B5",
    ", c #5A7BB4",
    "& c #5F7FB5",
    "________________",
    "____.Xo______OO_",
    "____+@#.____$@&_",
    "____*@@X__.=@=o_",
    "_____-@-_.=@=.__",
    "_____;@@X=@=.___",
    "_____.#@@@$.____",
    "______:@@>______",
    "_____:@@@+______",
    "___.,@#&@@._____",
    "__o=@=oO@@<_____",
    "_1#@=._.@@-_____",
    "*@@$____>@@2____",
    ":#*_____3#,.____",
    "________________",
    "4444444444444444"
    };
    static const char *ImageList1_4_XPM[] = {
    "16 16 2 1",
    ". c Black",
    "_ c None",
    "________________",
    "______..________",
    "______..._______",
    "_____...._______",
    "_____._...______",
    "____.._...______",
    "____.___..______",
    "___..___..._____",
    "___._____.._____",
    "___.........____",
    "__.._____...____",
    "__._______...___",
    "_.._______...___",
    "_....___......._",
    "________________",
    "................"
    };
    ImageList1 = new wxImageList(16, 16, 6);
    ImageList1->Add(wxBitmap(ImageList1_0_XPM));
    ImageList1->Add(wxBitmap(ImageList1_1_XPM));
    ImageList1->Add(wxBitmap(ImageList1_2_XPM));
    ImageList1->Add(wxBitmap(ImageList1_3_XPM));
    ImageList1->Add(wxBitmap(ImageList1_4_XPM));

    Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnTreeCtrl1SelectionChanged);
    // Set the bitmaps for bAddItem.
    bAddItem->SetBitmapLabel(ImageList1->GetBitmap(0));
    Connect(ID_IMAGEBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbAddItemClick);
    // Set the bitmaps for bAddSubItem.
    bAddSubItem->SetBitmapLabel(ImageList1->GetBitmap(1));
    Connect(ID_IMAGEBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbAddSubItemClick);
    // Set the bitmaps for bDelItem.
    bDelItem->SetBitmapLabel(ImageList1->GetBitmap(2));
    Connect(ID_IMAGEBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbDelItemClick);
    // Set the bitmaps for bDelAllItems.
    bDelAllItems->SetBitmapLabel(ImageList1->GetBitmap(3));
    Connect(ID_IMAGEBUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbDelAllItemsClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbItemColorClick);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OncxItemBoldClick);
    // Set the bitmaps for bEditItem.
    bEditItem->SetBitmapLabel(ImageList1->GetBitmap(4));
    Connect(ID_IMAGEBUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbEditItemClick);

    Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OncbNormalSelect);

    Connect(ID_COMBOBOX2,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OncbSelectedSelect);

    Connect(ID_COMBOBOX3,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OncbExpandedSelect);

    Connect(ID_COMBOBOX4,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OncbSelExpandedSelect);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbOKClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsImageTreeEditorDlg::OnbCancelClick);
    //*)
}

wxsImageTreeEditorDlg::~wxsImageTreeEditorDlg()
{
    //(*Destroy(wxsImageTreeEditorDlg)
    //*)
}

    /*! \brief Run the dialogue.
     *
     * \param aItems wxArrayString&
     * \return bool
     *
     */
bool wxsImageTreeEditorDlg::Execute(wxArrayString &aItems)
{
    int             i, n;
    int             jv, j1, j2, j3, j4;
    wxColor         jc;
    bool            jb;
    wxString        jt;
    wxTreeItemId    jp[32];

    wxString        ss, tt;
    wxTreeItemId    root;
    wxTreeItemId    item;
    wxBitmap        bmp;
    wxsImageList    *ilist;


    // get name of combo-box and image-list
    n = aItems.GetCount();
    m_sTreeName  = _("<unknown>");
    m_sImageName = _("<none>");
    if(n >= 1){
        m_sTreeName  = aItems.Item(0);
    }
    if(n >= 2){
        m_sImageName = aItems.Item(1);
    }

    // show the names
    ss = _("Tree Control: ") + m_sTreeName;
    StaticBoxSizer1->GetStaticBox()->SetLabel(ss);

    ss = m_sImageName;
    StaticText13->SetLabel(ss);

    // clear old junk
    Tree1->DeleteAllItems();

    // a valid image-list given?
    m_imageList.RemoveAll();
    ilist = (wxsImageList *) wxsImageListEditorDlg::FindTool(NULL, m_sImageName);
    if(ilist != NULL){
        ilist->GetImageList(m_imageList);
    }
    SetImageList(m_imageList);

    // add all the new items
    n = aItems.GetCount();
    for(i = 2; i < n; i++){
        ss = aItems.Item(i);
        ParseTreeItem(ss, jv, jc, jb, j1, j2, j3, j4, jt);

        if(jv == 0){
            item = Tree1->AddRoot(jt);
        }
        else{
            item = Tree1->AppendItem(jp[jv-1], jt);
        }
        jp[jv] = item;

        if(jc.IsOk()){
            Tree1->SetItemTextColour(item, jc);
        }
        Tree1->SetItemBold(item, jb);
        Tree1->SetItemImage(item, j1, wxTreeItemIcon_Normal);
        Tree1->SetItemImage(item, j2, wxTreeItemIcon_Selected);
        Tree1->SetItemImage(item, j3, wxTreeItemIcon_Expanded);
        Tree1->SetItemImage(item, j4, wxTreeItemIcon_SelectedExpanded);
    }

    Tree1->ExpandAll();

    // show the dialog and wait for a response
    n = ShowModal();

    // save all new stuff?
    if(n == wxOK){
        // must save combo-box name and image-list name
        aItems.Clear();
        aItems.Add(m_sTreeName);
        aItems.Add(m_sImageName);

        // save the root item and all it's children
        // this effectively saves every item in the tree
        // I wanted to use a simple loop here, but it works MUCH easier with a recursive function
        root = Tree1->GetRootItem();
        if(root.IsOk()){
            EncodeTreeItems(root, 0, aItems);
        }
    }

    // done
    return (n == wxOK);
}

/*! \brief Set the image list.
 *
 * \param inImageList wxImageList&
 * \return void
 *
 */
void    wxsImageTreeEditorDlg::SetImageList(wxImageList &inImageList)
{
    int         i, n;
    wxString    ss, tt;
    wxBitmap    bmp;

    // save the image list in the tree control
    Tree1->SetImageList(&inImageList);

    // valid list given?
    n = inImageList.GetImageCount();
    if(n <= 0){
        cbNormal->Enable(false);
        cbSelected->Enable(false);
        cbExpanded->Enable(false);
        cbSelExpanded->Enable(false);
    }
    else {
        cbNormal->Enable(true);
        cbSelected->Enable(true);
        cbExpanded->Enable(true);
        cbSelExpanded->Enable(true);
    }

    // set images in the drop-down lists
    cbNormal->Clear();
    cbSelected->Clear();
    cbExpanded->Clear();
    cbSelExpanded->Clear();

    ss = _("<none>");
    cbNormal->Append(ss);
    cbSelected->Append(ss);
    cbExpanded->Append(ss);
    cbSelExpanded->Append(ss);

    for(i = 0; i < n; i++){
        ss.Printf(wxT("%d"), i);
        bmp = inImageList.GetBitmap(i);

        cbNormal->Append(ss, bmp);
        cbSelected->Append(ss, bmp);
        cbExpanded->Append(ss, bmp);
        cbSelExpanded->Append(ss, bmp);
    }

    // default selections
    cbNormal->SetSelection(0);
    cbSelected->SetSelection(0);
    cbExpanded->SetSelection(0);
    cbSelExpanded->SetSelection(0);
}

/*! \brief Add a new item as a sibling of the current item.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbAddItemClick(wxCommandEvent &event)
{
    int             n;
    wxTreeItemId    current;

    // how many items?
    n = Tree1->GetCount();

    // and current selection
    current = Tree1->GetSelection();

    // add a root item?
    if(n <= 0){
        current.Unset();
        AddItem(current);
    }
    // no current item?
    else if(! current.IsOk()){
        current = Tree1->GetRootItem();
        AddItem(current);
    }
    // else a sibling
    else {
        current = Tree1->GetItemParent(current);
        AddItem(current);
    }
}

/*! \brief Add a new item as a child of the current item.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbAddSubItemClick(wxCommandEvent &event)
{
    int             n;
    wxTreeItemId    current;

    // how many items?
    n = Tree1->GetCount();

    // and current selection
    current = Tree1->GetSelection();

    // add a root item?
    if(n <= 0){
        current.Unset();
        AddItem(current);
    }
    // no current item?
    else if(! current.IsOk()){
        current = Tree1->GetRootItem();
        AddItem(current);
    }
    // else a child
    else {
        AddItem(current);
    }

    // make sure it is expanded
    Tree1->Expand(current);
}

/*! \brief Add a new item to the tree.
 *
 * \param inParent wxTreeItemId&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::AddItem(wxTreeItemId &inParent){
    int             n;
    wxString        ss, tt;
    wxTreeItemId    parent, current;
    wxColour        cc;
    bool            b;

    // how many items?
    n = Tree1->GetCount();

    // add a root item?
    if(n <= 0){
        ss = _("root");
        current = Tree1->AddRoot(ss);
    }

    // bad parent?
    else if(! inParent.IsOk()){
        ss.Printf(_("item %d"), n);
        parent = Tree1->GetRootItem();
        current = Tree1->AppendItem(parent, ss);
    }
    // else a child of whatever
    else {
        ss.Printf(_("item %d"), n);
        current = Tree1->AppendItem(inParent, ss);
    }

    // if it failed, skip the rest of this
    if(! current.IsOk()){
        return;
    }

    // set text colour
    cc = bItemColor->GetForegroundColour();
    Tree1->SetItemTextColour(current, cc);

    // bold or plain
    b = cxItemBold->GetValue();
    Tree1->SetItemBold(current, b);

    // the images
    n = cbNormal->GetSelection() - 1;
    if(n >= 0){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_Normal);
    }

    n = cbSelected->GetSelection() - 1;
    if(n >= 0){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_Selected);
    }

    n = cbExpanded->GetSelection() - 1;
    if(n >= 0){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_Expanded);
    }

    n = cbSelExpanded->GetSelection() - 1;
    if(n >= 0){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_SelectedExpanded);
    }

    // redraw the whole thing
    Tree1->Refresh();
}

/*! \brief Delete a tree item.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbDelItemClick(wxCommandEvent &event)
{
    wxTreeItemId    current;

    // current selection
    current = Tree1->GetSelection();

    // delete it
    if(current.IsOk()){
        Tree1->Delete(current);
    }
}

/*! \brief Delete all tree items.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbDelAllItemsClick(wxCommandEvent &event)
{
    int         n;
    wxString    ss;

    n = wxMessageBox(_("Delete ALL Items In Tree?"), _("Clear"), wxYES_NO);
    if(n == wxYES){
        Tree1->DeleteAllItems();
    }
}

/*! \brief Select the item's colour.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbItemColorClick(wxCommandEvent &event)
{
    int             n;
    wxColourData    cd;
    wxColour        cc;
    wxTreeItemId    current;

    // ask user for a new color
    n = m_colourDialog.ShowModal();
    if(n != wxID_OK){
        return;
    }

    // get the color
    cd = m_colourDialog.GetColourData();
    cc = cd.GetColour();

    // set the button text
    bItemColor->SetForegroundColour(cc);

    // and the current item
    current = Tree1->GetSelection();
    if(current.IsOk()){
        Tree1->SetItemTextColour(current, cc);
    }
}

/*! \brief Make the item text bold.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OncxItemBoldClick(wxCommandEvent &event)
{
    bool            b;
    wxTreeItemId    current;

    // get checkbox value
    b = cxItemBold->GetValue();

    // and set the current item
    current = Tree1->GetSelection();
    if(current.IsOk()){
        Tree1->SetItemBold(current, b);
    }
}

/*! \brief Edit an item.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbEditItemClick(wxCommandEvent &event)
{
    wxTreeItemId    current;

    // current selection
    current = Tree1->GetSelection();

    // delete it
    if(current.IsOk()){
        Tree1->EditLabel(current);
    }
}

/*! \brief Select the normal state image.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OncbNormalSelect(wxCommandEvent &event)
{
    int             n;
    wxTreeItemId    current;

    n  = cbNormal->GetSelection();
    n -= 1;

    current = Tree1->GetSelection();
    if(current.IsOk()){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_Normal);
    }
}

/*! \brief Select the selected state image.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OncbSelectedSelect(wxCommandEvent &event)
{
    int             n;
    wxTreeItemId    current;

    n  = cbSelected->GetSelection();
    n -= 1;

    current = Tree1->GetSelection();
    if(current.IsOk()){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_Selected);
    }
}

/*! \brief Select the expanded state image.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OncbExpandedSelect(wxCommandEvent &event)
{
    int             n;
    wxTreeItemId    current;

    n  = cbExpanded->GetSelection();
    n -= 1;

    current = Tree1->GetSelection();
    if(current.IsOk()){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_Expanded);
    }
}

/*! \brief Select the selected and expanded state image.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OncbSelExpandedSelect(wxCommandEvent &event)
{
    int             n;
    wxTreeItemId    current;

    n  = cbSelExpanded->GetSelection();
    n -= 1;

    current = Tree1->GetSelection();
    if(current.IsOk()){
        Tree1->SetItemImage(current, n, wxTreeItemIcon_SelectedExpanded);
    }
}

/*! \brief The tree item selection was changed.
 *
 * \param event wxTreeEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnTreeCtrl1SelectionChanged(wxTreeEvent &event)
{
    int             n;
    wxTreeItemId    current;
    wxColour        cc;
    bool            b;

    // get current item
    current = Tree1->GetSelection();
    if(! current.IsOk()){
        return;
    }

    // current text colour
    cc = Tree1->GetItemTextColour(current);
    bItemColor->SetForegroundColour(cc);

    // bold or plain
    b = Tree1->IsBold(current);
    cxItemBold->SetValue(b);

    // image indices
    n = Tree1->GetItemImage(current, wxTreeItemIcon_Normal);
    n += 1;
    cbNormal->SetSelection(n);

    n = Tree1->GetItemImage(current, wxTreeItemIcon_Selected);
    n += 1;
    cbSelected->SetSelection(n);

    n = Tree1->GetItemImage(current, wxTreeItemIcon_Expanded);
    n += 1;
    cbExpanded->SetSelection(n);

    n = Tree1->GetItemImage(current, wxTreeItemIcon_SelectedExpanded);
    n += 1;
    cbSelExpanded->SetSelection(n);
}

/*! \brief Parse tree item text.
 *
 * \param aSource wxString
 * \param outLevel int&
 * \param outColour wxColour&
 * \param outBold bool&
 * \param outImage1 int&
 * \param outImage2 int&
 * \param outImage3 int&
 * \param outImage4 int&
 * \param outText wxString&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::ParseTreeItem(wxString aSource, int &outLevel, wxColour &outColour, bool &outBold, int &outImage1, int &outImage2, int &outImage3, int &outImage4, wxString &outText)
{
    int         i, n;
    long        ll;
    wxString    ss, tt;

    // working copy
    ss = aSource;

    // the depth level
    outLevel = 1;
    i  = ss.Find(wxT(","));
    if(i != wxNOT_FOUND){
        tt = ss.Left(i);
        ss.erase(0, i + 1);
        if(tt.ToLong(&ll)) outLevel = ll;
    }

    // the color
    outColour.Set(wxT("?"));
    i  = ss.Find(wxT(","));
    if(i != wxNOT_FOUND){
        tt = ss.Left(i);
        ss.erase(0, i + 1);
        outColour.Set(tt);
    }

    // bold or normal text
    n = 0;
    i  = ss.Find(wxT(","));
    if(i != wxNOT_FOUND){
        tt = ss.Left(i);
        ss.erase(0, i + 1);
        if(tt.ToLong(&ll)){
            n = ll;
        }
    }
    outBold = (n != 0);

    // 4 image indices
    outImage1 = -1;
    i  = ss.Find(wxT(","));
    if(i != wxNOT_FOUND){
        tt = ss.Left(i);
        ss.erase(0, i + 1);
        if(tt.ToLong(&ll)){
            outImage1 = ll;
        }
    }

    outImage2 = -1;
    i  = ss.Find(wxT(","));
    if(i != wxNOT_FOUND){
        tt = ss.Left(i);
        ss.erase(0, i + 1);
        if(tt.ToLong(&ll)){
            outImage2 = ll;
        }
    }

    outImage3 = -1;
    i  = ss.Find(wxT(","));
    if(i != wxNOT_FOUND){
        tt = ss.Left(i);
        ss.erase(0, i + 1);
        if(tt.ToLong(&ll)){
            outImage3 = ll;
        }
    }

    outImage4 = -1;
    i  = ss.Find(wxT(","));
    if(i != wxNOT_FOUND){
        tt = ss.Left(i);
        ss.erase(0, i + 1);
        if(tt.ToLong(&ll)){
            outImage4 = ll;
        }
    }

    // everything else is the text
    ss.Trim(true);
    ss.Trim(false);
    outText = ss;
}

/*! \brief Encode tree item text.
 *
 * \param inParent wxTreeItemId
 * \param inLevel int
 * \param outList wxArrayString&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::EncodeTreeItems(wxTreeItemId inParent, int inLevel, wxArrayString &outList)
{
    int                 n;
    wxColour            cc;
    wxString            ss, tt;
    wxTreeItemId        child;
    wxTreeItemIdValue   cookie;

    // nothing yet
    ss = wxEmptyString;

    // start with this item
    tt.Printf(wxT("%d,"), inLevel);
    ss += tt;

    cc  = Tree1->GetItemTextColour(inParent);
    tt  = cc.GetAsString(wxC2S_HTML_SYNTAX);
    tt += wxT(",");
    ss += tt;

    if(Tree1->IsBold(inParent)){
        tt = wxT("1,");
    }
    else{
        tt = wxT("0,");
    }
    ss += tt;

    n = Tree1->GetItemImage(inParent, wxTreeItemIcon_Normal);
    tt.Printf(wxT("%d,"), n);
    ss += tt;

    n = Tree1->GetItemImage(inParent, wxTreeItemIcon_Selected);
    tt.Printf(wxT("%d,"), n);
    ss += tt;

    n = Tree1->GetItemImage(inParent, wxTreeItemIcon_Expanded);
    tt.Printf(wxT("%d,"), n);
    ss += tt;

    n = Tree1->GetItemImage(inParent, wxTreeItemIcon_SelectedExpanded);
    tt.Printf(wxT("%d,"), n);
    ss += tt;

    tt = Tree1->GetItemText(inParent);
    ss += tt;

    // save it
    outList.Add(ss);

    // and all the children
    child = Tree1->GetFirstChild(inParent, cookie);
    while(child.IsOk()){
        EncodeTreeItems(child, inLevel + 1, outList);
        child = Tree1->GetNextChild(inParent, cookie);
    }
}

/*! \brief The OK button was clicked.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbOKClick(wxCommandEvent &event)
{
    EndModal(wxOK);
}

/*! \brief The Cancel button was clicked.
 *
 * \param event wxCommandEvent&
 * \return void
 *
 */
void wxsImageTreeEditorDlg::OnbCancelClick(wxCommandEvent &event)
{
    EndModal(wxCANCEL);
}
