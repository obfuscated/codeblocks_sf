/** \file wxsimagecomboeditordlg.cpp
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
* This code was taken from the wxSmithImage plug-in, copyright Ron CollinswxsImageComboEditorDlg
* and released under the GPL.
*
*/


//(*InternalHeaders(wxsImageComboEditorDlg)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)
#include "../wxsitemresdata.h"
#include "../wxsitem.h"
#include "wxsimagelisteditordlg.h"
#include "wxsimagecomboeditordlg.h"

//(*IdInit(wxsImageComboEditorDlg)
const long wxsImageComboEditorDlg::ID_STATICTEXT1 = wxNewId();
const long wxsImageComboEditorDlg::ID_TREECTRL1 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT2 = wxNewId();
const long wxsImageComboEditorDlg::ID_BITMAPBUTTON1 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT3 = wxNewId();
const long wxsImageComboEditorDlg::ID_BITMAPBUTTON2 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT4 = wxNewId();
const long wxsImageComboEditorDlg::ID_BITMAPBUTTON3 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT5 = wxNewId();
const long wxsImageComboEditorDlg::ID_BITMAPBUTTON4 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT6 = wxNewId();
const long wxsImageComboEditorDlg::ID_BITMAPBUTTON5 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT7 = wxNewId();
const long wxsImageComboEditorDlg::ID_BITMAPBUTTON6 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT8 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT9 = wxNewId();
const long wxsImageComboEditorDlg::ID_COMBOBOX1 = wxNewId();
const long wxsImageComboEditorDlg::ID_STATICTEXT10 = wxNewId();
const long wxsImageComboEditorDlg::ID_BUTTON1 = wxNewId();
const long wxsImageComboEditorDlg::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsImageComboEditorDlg,wxScrollingDialog)
    //(*EventTable(wxsImageComboEditorDlg)
    //*)
END_EVENT_TABLE()

    wxsImageComboEditorDlg::wxsImageComboEditorDlg(wxWindow* parent)
    {
        //(*Initialize(wxsImageComboEditorDlg)
        wxBoxSizer* BoxSizer4;
        wxBoxSizer* BoxSizer6;
        wxBoxSizer* BoxSizer5;
        wxBoxSizer* BoxSizer10;
        wxBoxSizer* BoxSizer7;
        wxBoxSizer* BoxSizer8;
        wxBoxSizer* BoxSizer13;
        wxBoxSizer* BoxSizer2;
        wxBoxSizer* BoxSizer11;
        wxBoxSizer* BoxSizer12;
        wxBoxSizer* BoxSizer14;
        wxBoxSizer* BoxSizer1;
        wxBoxSizer* BoxSizer9;
        wxBoxSizer* BoxSizer3;

        Create(parent, wxID_ANY, _("wxBitmapComboBox Editor"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
        SetMinSize(wxSize(-1,-1));
        SetMaxSize(wxSize(-1,-1));
        BoxSizer1 = new wxBoxSizer(wxVERTICAL);
        BoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
        BoxSizer2 = new wxBoxSizer(wxVERTICAL);
        StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Combo-Box"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
        BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_LEFT, 5);
        m_pTree = new wxTreeCtrl(this, ID_TREECTRL1, wxPoint(0,32), wxSize(240,272), wxTR_EDIT_LABELS|wxTR_NO_BUTTONS|wxTR_NO_LINES|wxTR_HIDE_ROOT|wxRAISED_BORDER, wxDefaultValidator, _T("ID_TREECTRL1"));
        BoxSizer2->Add(m_pTree, 0, wxALL|wxEXPAND, 0);
        BoxSizer13->Add(BoxSizer2, 0, wxALL|wxALIGN_TOP, 5);
        BoxSizer3 = new wxBoxSizer(wxVERTICAL);
        StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Edit Items"), wxPoint(264,8), wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
        BoxSizer3->Add(StaticText2, 0, wxALL|wxALIGN_LEFT, 5);
        BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
        m_pBtnAddItem = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_ADD_BOOKMARK")),wxART_BUTTON), wxDefaultPosition, wxSize(32,32), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
        m_pBtnAddItem->SetDefault();
        BoxSizer4->Add(m_pBtnAddItem, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Add Item"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
        BoxSizer4->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer3->Add(BoxSizer4, 0, wxALL|wxEXPAND, 0);
        BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
        m_pBtnMoveUp = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_UP")),wxART_BUTTON), wxDefaultPosition, wxSize(32,32), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
        m_pBtnMoveUp->SetDefault();
        BoxSizer5->Add(m_pBtnMoveUp, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Move Item Up"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
        BoxSizer5->Add(StaticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer3->Add(BoxSizer5, 0, wxALL|wxEXPAND, 0);
        BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
        m_pBtnMoveDown = new wxBitmapButton(this, ID_BITMAPBUTTON3, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_DOWN")),wxART_BUTTON), wxDefaultPosition, wxSize(32,32), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
        m_pBtnMoveDown->SetDefault();
        BoxSizer6->Add(m_pBtnMoveDown, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Move Item Down"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
        BoxSizer6->Add(StaticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer3->Add(BoxSizer6, 0, wxALL|wxEXPAND, 0);
        BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
        m_pBtnDeleteItem = new wxBitmapButton(this, ID_BITMAPBUTTON4, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_DEL_BOOKMARK")),wxART_BUTTON), wxDefaultPosition, wxSize(32,32), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
        BoxSizer7->Add(m_pBtnDeleteItem, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Delete Item"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
        BoxSizer7->Add(StaticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer3->Add(BoxSizer7, 0, wxALL|wxEXPAND, 0);
        BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
        m_pBtnDeleteAll = new wxBitmapButton(this, ID_BITMAPBUTTON5, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_DELETE")),wxART_BUTTON), wxDefaultPosition, wxSize(32,32), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON5"));
        BoxSizer8->Add(m_pBtnDeleteAll, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Delete All Items"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
        BoxSizer8->Add(StaticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer3->Add(BoxSizer8, 0, wxALL|wxEXPAND, 0);
        BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
        m_pBtnEditItem = new wxBitmapButton(this, ID_BITMAPBUTTON6, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_HELP_SETTINGS")),wxART_BUTTON), wxDefaultPosition, wxSize(32,32), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON6"));
        m_pBtnEditItem->SetDefault();
        BoxSizer9->Add(m_pBtnEditItem, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Edit Item Text"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
        BoxSizer9->Add(StaticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer3->Add(BoxSizer9, 0, wxALL|wxEXPAND, 0);
        BoxSizer10 = new wxBoxSizer(wxVERTICAL);
        BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
        StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Image-List"), wxPoint(256,240), wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT9"));
        BoxSizer11->Add(StaticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer10->Add(BoxSizer11, 0, wxALL|wxALIGN_LEFT, 0);
        BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
        m_pCmbImage = new wxBitmapComboBox(this, ID_COMBOBOX1, wxEmptyString, wxPoint(256,272), wxSize(100,22), 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX1"));
        BoxSizer12->Add(m_pCmbImage, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
        StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Item Image"), wxPoint(352,276), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
        BoxSizer12->Add(StaticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        BoxSizer10->Add(BoxSizer12, 0, wxALL|wxALIGN_LEFT, 0);
        BoxSizer3->Add(BoxSizer10, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND, 5);
        BoxSizer13->Add(BoxSizer3, 0, wxALL|wxALIGN_TOP, 5);
        BoxSizer1->Add(BoxSizer13, 0, wxALL|wxALIGN_LEFT, 5);
        BoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
        m_pBtnOK = new wxButton(this, ID_BUTTON1, _("OK"), wxPoint(64,320), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
        m_pBtnOK->SetDefault();
        BoxSizer14->Add(m_pBtnOK, 0, wxALL|wxALIGN_BOTTOM, 5);
        BoxSizer14->Add(-1,-1,1, wxALL|wxALIGN_BOTTOM, 5);
        m_pBtnCancel = new wxButton(this, ID_BUTTON2, _("Cancel"), wxPoint(272,320), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
        BoxSizer14->Add(m_pBtnCancel, 0, wxALL|wxALIGN_BOTTOM, 5);
        BoxSizer1->Add(BoxSizer14, 0, wxALL|wxEXPAND, 5);
        SetSizer(BoxSizer1);
        BoxSizer1->Fit(this);
        BoxSizer1->SetSizeHints(this);

        Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_SEL_CHANGED,wxTreeEventHandler(wxsImageComboEditorDlg::OnTreeSelectionChanged));
        Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnAddItemClick));
        Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnMoveUpClick));
        Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnMoveDownClick));
        Connect(ID_BITMAPBUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnDeleteItemClick));
        Connect(ID_BITMAPBUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnDeleteAllClick));
        Connect(ID_BITMAPBUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnEditItemClick));
        Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(wxsImageComboEditorDlg::OnCmbImageSelect));
        Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnOKClick));
        Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(wxsImageComboEditorDlg::OnBtnCancelClick));
        //*)
    }

    wxsImageComboEditorDlg::~wxsImageComboEditorDlg()
    {
        //(*Destroy(wxsImageComboEditorDlg)
        //*)
    }

    /*! \brief Run the dialogue.
     *
     * \param aItems wxArrayString&
     * \return bool
     *
     */
    bool wxsImageComboEditorDlg::Execute(wxArrayString &aItems)
    {
        int             i,n;
        int             j,k;
        long            ll;
        wxString        ss, tt;
        wxTreeItemId    root;
        wxTreeItemId    item;
        wxTreeItemIdValue   cookie;
        wxBitmap        bmp;
        wxsImageList    *ilist;


        // get name of combo-box and image-list
        n = aItems.GetCount();
        if(n == 0){
            m_ComboName = _("<unknown>");
            m_ImageName = _("<none>");
        }
        else if(n == 1){
            m_ComboName = aItems.Item(0);
            m_ImageName = _("<none>");
        }
        else{
            m_ComboName = aItems.Item(0);
            m_ImageName = aItems.Item(1);
        }

        // show the names
        ss = _("Combo Box: ") + m_ComboName;
        StaticText1->SetLabel(ss);

        ss = _("Image List: ") + m_ImageName;
        StaticText9->SetLabel(ss);

        // a valid image-list given?
        m_ImageList.RemoveAll();
        ilist = (wxsImageList *) wxsImageListEditorDlg::FindTool(NULL, m_ImageName);
        if(ilist == NULL){
            m_pCmbImage->Enable(false);
        }
        else{
            m_pCmbImage->Enable(true);
            ilist->GetImageList(m_ImageList);
        }

        // setup the combo-box image selector
        m_pCmbImage->Clear();
        m_pCmbImage->Append(_("<none>"));

        n = m_ImageList.GetImageCount();
        for(i=0; i<n; i++){
            ss.Printf(_T("%3d"), i);
            bmp = m_ImageList.GetBitmap(i);

            m_pCmbImage->Append(ss, bmp);
        }

        m_pCmbImage->SetSelection(0);

        // clear old junk
        m_pTree->DeleteAllItems();

        // make a root item
        root = m_pTree->AddRoot(_("root"));

        // make sure we are using the image list -- even if it is empty
        m_pTree->SetImageList(&m_ImageList);

        // add all the new items
        n = aItems.GetCount();
        for(i = 2;i < n;i++){
            ss = aItems.Item(i);
            j  = ss.Find(_T(","));
            k = -1;
            if(j != wxNOT_FOUND){
                tt = ss.Left(j);
                ss.erase(0, j + 1);
                if(tt.ToLong(&ll)) k = ll;
            }
            item = m_pTree->AppendItem(root, ss, k);
        }

        // show the dialog and wait for a response
        n = ShowModal();

        // save all new stuff?
        if(n == wxOK){
            // must save combo-box name and image-list name
            aItems.Clear();
            aItems.Add(m_ComboName);
            aItems.Add(m_ImageName);

            // fetch the actual root item, it might have been recreated or
            // even deleted while executing the dialog
            root = m_pTree->GetRootItem();
            if (root.IsOk())
            {
                // save text of all children of the root item
                // these are actually the only things seen by the user
                item = m_pTree->GetFirstChild(root, cookie);
                while(item.IsOk()){
                    ss = m_pTree->GetItemText(item);
                    k  = m_pTree->GetItemImage(item, wxTreeItemIcon_Normal);

                    tt.Printf(_T("%d,"), k);
                    ss = tt + ss;

                    aItems.Add(ss);

                    item = m_pTree->GetNextChild(root, cookie);
                }
            }
        }

        // done
        return (n == wxOK);
    }

    /*! \brief The Delete All button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnDeleteAllClick(cb_unused wxCommandEvent& event)
    {
        int         n;

        n = wxMessageBox(_("Delete All Items?"), _("Confirm"), wxYES_NO );
        if(n == wxYES){
            m_pTree->DeleteAllItems();
        }
    }

    /*! \brief The Add Item button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnAddItemClick(cb_unused wxCommandEvent& event)
    {
        int             n;
        wxTreeItemId    root = m_pTree->GetRootItem();
        wxTreeItemId    item;
        wxString        ss;

        // must have a root item, even though it is hidden
        n = m_pTree->GetCount();

        if (!root.IsOk())
            root = m_pTree->AddRoot(_("root"));

        // add a new child under the root
        ss.Printf(_("new item %d"), ++n);
        item = m_pTree->AppendItem(root, ss);

        // new image index
        n = m_pCmbImage->GetSelection();
        if(n == wxNOT_FOUND)
            n  = -1;
        else
            n -=  1;

        // assign image
        m_pTree->SetItemImage(item, n, wxTreeItemIcon_Normal);

        // done
        m_pTree->SelectItem(item);
        m_pTree->EditLabel(item);
    }

    /*! \brief The Move Up button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnMoveUpClick(cb_unused wxCommandEvent& event)
    {
        int         xi, xp;
        wxString    si, sp;
        wxTreeItemId    prev;
        wxTreeItemId    item;

        // current item and the one directly above
        item = m_pTree->GetSelection();
        prev = m_pTree->GetPrevSibling(item);

        // no such thing?
        if(! item.IsOk())
            return;

        if(! prev.IsOk())
            return;

        // swap strings and image indices
        si = m_pTree->GetItemText(item);
        xi = m_pTree->GetItemImage(item, wxTreeItemIcon_Normal);

        sp = m_pTree->GetItemText(prev);
        xp = m_pTree->GetItemImage(prev, wxTreeItemIcon_Normal);

        // and swap them
        m_pTree->SetItemText(item, sp);
        m_pTree->SetItemImage(item, xp);

        m_pTree->SetItemText(prev, si);
        m_pTree->SetItemImage(prev, xi);

        // done
        m_pTree->SelectItem(prev);
        m_pTree->Refresh();
    }

    /*! \brief The Move Down button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnMoveDownClick(cb_unused wxCommandEvent& event)
    {
        int         xi, xp;
        wxString    si, sp;
        wxTreeItemId    next;
        wxTreeItemId    item;

        // current item and the one directly above
        item = m_pTree->GetSelection();
        next = m_pTree->GetNextSibling(item);

        // no such thing?
        if(!item.IsOk())
            return;

        if(!next.IsOk())
            return;

        // swap strings and image indices
        si = m_pTree->GetItemText(item);
        xi = m_pTree->GetItemImage(item, wxTreeItemIcon_Normal);

        sp = m_pTree->GetItemText(next);
        xp = m_pTree->GetItemImage(next, wxTreeItemIcon_Normal);

        // and swap them
        m_pTree->SetItemText(item, sp);
        m_pTree->SetItemImage(item, xp);

        m_pTree->SetItemText(next, si);
        m_pTree->SetItemImage(next, xi);

        // done
        m_pTree->SelectItem(next);
        m_pTree->Refresh();
    }

    /*! \brief The Delete Item button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnDeleteItemClick(cb_unused wxCommandEvent& event)
    {
        wxTreeItemId    item;

        // delete only current item
        item = m_pTree->GetSelection();
        if(item.IsOk())
            m_pTree->Delete(item);


        // done
        m_pTree->Refresh();
    }

    /*! \brief The Edit Item button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnEditItemClick(cb_unused wxCommandEvent& event)
    {
        wxTreeItemId    item;

        // delete only current item
        item = m_pTree->GetSelection();
        if(item.IsOk())
            m_pTree->EditLabel(item);


        // done
        m_pTree->Refresh();
    }

    /*! \brief The OK button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnOKClick(cb_unused wxCommandEvent& event)
    {
        EndModal(wxOK);
    }

    /*! \brief The Cancel button was clicked.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnBtnCancelClick(cb_unused wxCommandEvent& event)
    {
        EndModal(wxCANCEL);
    }

    /*! \brief Assign a new image to a text item.
     *
     * \param event wxCommandEvent&
     * \return void
     *
     */
    void wxsImageComboEditorDlg::OnCmbImageSelect(cb_unused wxCommandEvent& event)
    {
        int             n;
        wxTreeItemId    item;

        // current item
        item = m_pTree->GetSelection();
        if(! item.IsOk())
            return;


        // new image index
        n = m_pCmbImage->GetSelection();
        if(n == wxNOT_FOUND)
            n  = -1;
        else
            n -=  1;

        // assign image
        m_pTree->SetItemImage(item, n, wxTreeItemIcon_Normal);
    }

/*! \brief A list item was clicked in the item list.
 *
 * \param event wxTreeEvent&
 * \return void
 *
 * Sets the image combo to display the correct image for the item selected.
 */
void wxsImageComboEditorDlg::OnTreeSelectionChanged(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if(item.IsOk()){
            // Set the combo box to display 1 position higher than the number returned by GetItemImage() as the first item is "<none>" .
            // i.e. image 0 in the imagelist is at position 1 in the combo box, etc.
            m_pCmbImage->SetSelection(m_pTree->GetItemImage(item) + 1);
    }

}
