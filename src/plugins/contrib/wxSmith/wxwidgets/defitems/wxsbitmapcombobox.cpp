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


#include <wx/bmpbuttn.h>
#include "wxsbitmapcombobox.h"


//------------------------------------------------------------------------------


namespace
{


    wxsRegisterItem<wxsBitmapComboBox> Reg(_T("BitmapComboBox"),wxsTWidget,_T("Standard"),50);
//    wxsRegisterItem<wxsBitmapComboBox> Reg(
//        _T("wxBitmapComboBox"),         // Class name
//        wxsTWidget,                     // Item type
//        _T("wxWindows"),                // License
//        _T("Ron Collins"),              // Author
//        _T("rcoll@theriver.com"),       // Author's email
//        _T(""),                         // Item's homepage
//        _T("Standard"),                 // Category in palette
//        50,                             // Priority in palette
//        _T("ComboBox"),                 // Base part of names for new items
//        wxsCPP,                         // List of coding languages supported by this item
//        1, 0,                           // Version
//        _T("wxBitmapCombo32.png"),    // 32x32 bitmap
//        _T("wxBitmapCombo16.png"),    // 16x16 bitmap
//        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsBitmapComboBoxStyles,_T("wxBU_AUTODRAW"))
        WXS_ST_CATEGORY("wxBitmapComboBox")
        WXS_ST(wxCB_SIMPLE)
        WXS_ST(wxCB_SORT)
        WXS_ST(wxCB_READONLY)
        WXS_ST(wxCB_DROPDOWN)
        WXS_ST(wxTE_PROCESS_ENTER)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsBitmapComboBoxEvents)
        WXS_EVI(EVT_COMBOBOX,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEvent,Select)
        WXS_EVI(EVT_TEXT,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEvent,Text)
        WXS_EVI(EVT_TEXT_ENTER,wxEVT_COMMAND_TEXT_ENTER,wxCommandEvent,TextEnter)
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsBitmapComboBox::wxsBitmapComboBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsBitmapComboBoxEvents,
        wxsBitmapComboBoxStyles)
{
// default values

    mImageList = _("<none>");
    mItems.Clear();

}

//------------------------------------------------------------------------------

void wxsBitmapComboBox::OnBuildCreatingCode() {
int         i,n;
wxString    ss, tt, vv;
bool        ilist;

// we only handle C++ constructs here

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsBitmapComboBox"),GetLanguage());

// header files

    AddHeader(_T("<wx/bmpcbox.h>"),GetInfo().ClassName,hfInPCH);

// the basic constructor

    vv = GetVarName();
    Codef(_T("%C(%W, %I, wxEmptyString, %P, %S, 0, NULL, %T, %V, %N);\n"));

// was a valid image-list specified?

    ilist = (wxsImageListEditorDlg::FindTool(this, mImageList) != NULL);

// add all text items, and the bitmaps at the bottom of the code
// bitmaps have to added after the wxsImages' and wxsImageList's were added
// note: first 2 items in mItems are used only in the dialog

    for(i=2; i<(int)mItems.GetCount(); ++i) {
        ss = mItems.Item(i);
        ParseComboItem(ss, tt, n);

// add the text item

        Codef(_T("%s->Append(_T(\"%s\"));\n"), vv.wx_str(), tt.wx_str());

// add the bitmap at the bottom of the code

        if ((ilist) && (n >= 0)) {
            tt.Printf(_T("%s->SetItemBitmap(%d, %s->GetBitmap(%d));\n"), vv.wx_str(), i-2, mImageList.wx_str(), n);
            AddEventCode(tt);
        };
    };

    AddEventCode(_T("\n"));

// finish setup

    BuildSetupWindowCode();

}

//------------------------------------------------------------------------------

wxObject* wxsBitmapComboBox::OnBuildPreview(wxWindow* Parent,long Flags) {
int                 i,n;
wxBitmapComboBox    *combo;
wxsImageList        *ilist;
wxString            ss, tt;
wxBitmap            bmp;

// the basic button

    combo = new wxBitmapComboBox(Parent, GetId(), _T(""), Pos(Parent),Size(Parent),0, NULL, Style());

// a valid image-list specified?

    ilist = (wxsImageList *) wxsImageListEditorDlg::FindTool(this, mImageList);

// make sure there is no random junk

    combo->Clear();

// add items to combo-box
// note: first 2 items are used only in the dialog

    for(i=2; i<(int)mItems.GetCount(); ++i) {
        ss = mItems.Item(i);
        ParseComboItem(ss, tt, n);

        combo->Append(tt);

        if ((ilist != NULL) && (n >= 0)) combo->SetItemBitmap(i-2, ilist->GetPreview(n));
    };

// done

    return SetupWindow(combo, Flags);
}

//------------------------------------------------------------------------------

void wxsBitmapComboBox::OnEnumWidgetProperties(cb_unused long Flags) {
static wxString         sImageNames[128];
static const wxChar    *pImageNames[128];

int                     i,n;
wxString                ss, tt;
wxArrayString           aa;

// find available image lists and store them in our local static arrays

    FindAllImageLists(aa);
    n = aa.GetCount();
    if (n > 127) n = 127;

    for(i=0; i<n; i++) {
        ss = aa.Item(i);
        sImageNames[i] = ss;
        pImageNames[i] = (const wxChar *) sImageNames[i];
    };
    pImageNames[n] = NULL;

    WXS_EDITENUM(wxsBitmapComboBox, mImageList, _("Image List"), _("mImageList"), pImageNames, _("<none>"))

// the list of items to appear in the combo box

    UpdateComboItemList();

    WXS_IMAGECOMBO(wxsBitmapComboBox, mItems, _("Combo Items"), _("mItems"));
    WXS_ARRAYSTRING(wxsBitmapComboBox, mItems, _("Items as Text"), _("mItemsText"), _("item2"));
}

//------------------------------------------------------------------------------
// find all tools that are image lists and return their names

void wxsBitmapComboBox::FindAllImageLists(wxArrayString &aNames) {
int             i,n;
wxsItemResData  *res;
wxsTool         *tool;
wxString        ss, tt;

// start the list with a chance to de-select any old list

    aNames.Clear();
    aNames.Add(_("<none>"));

// find all tools that are "wxImageList"

    res = GetResourceData();
    n   = res->GetToolsCount();
    for(i=0; i<n; i++) {
        tool = res->GetTool(i);
        ss   = tool->GetUserClass();

        if ((ss == _("wxImageList")) && (n < 127)) {
            ss = tool->GetVarName();
            aNames.Add(ss);
        };
    };
}

//------------------------------------------------------------------------------
// update the list of combo items to send to the wsxImageComboEditDialog

void wxsBitmapComboBox::UpdateComboItemList(void) {
int             i,n;
wxString        ss, tt;
wxArrayString   aa;

// first 2 items are always our var name and the name of the image list

    aa.Clear();
    ss = GetVarName();
    aa.Add(ss);
    ss = mImageList;
    aa.Add(ss);

// then copy over everything else the user entered last time

    n = mItems.GetCount();
    for(i=2; i<n; i++) {
        ss = mItems.Item(i);
        aa.Add(ss);
    };

// then put back in original list

    mItems.Clear();
    n = aa.GetCount();
    for(i=0; i<n; i++) {
        ss = aa.Item(i);
        mItems.Add(ss);
    };

// make sure that FindTool has a valid wxsItem* to work from in the dialog

    wxsImageListEditorDlg::FindTool(this, mImageList);
}

//------------------------------------------------------------------------------

void wxsBitmapComboBox::ParseComboItem(wxString inSource, wxString &outItem, int &outIndex) {
int         i;
long        ll;
wxString    ss, tt;

// working copy

    ss = inSource;

// a "," separates the image index from the text of the item

    i = ss.Find(_(","));

// if a "," was found, parse the index from the text
// if no ",", then no index and the entire string is text

    if (i != wxNOT_FOUND) {
        tt = ss.Left(i);
        ss.erase(0, i+1);
        outItem = ss;
        outIndex = -1;
        if (tt.ToLong(&ll)) outIndex = ll;
    }
    else {
        outIndex = -1;
        outItem = ss;
    };
}
