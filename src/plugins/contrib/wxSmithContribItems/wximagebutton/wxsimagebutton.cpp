/*
* This file is part of wxSmithContribItems plugin for Code::Blocks Studio
* Copyright (C) 2010 Ron Collins
*
* wxSmithContribItems is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithContribItems is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithContribItems. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include    <iostream>

#include <wx/bmpbuttn.h>
#include "wxsimagebutton.h"

using namespace std;

//------------------------------------------------------------------------------


namespace
{

#include "wxImageButton16.xpm"
#include "wxImageButton32.xpm"

    wxsRegisterItem<wxsImageButton> Reg(
        _T("wxImageButton"),            				// Class name
        wxsTWidget,                     					// Item type
        _T("wxWindows"),                				// License
        _T("Ron Collins"),              					// Author
        _T("rcoll@theriver.com"),       			// Author's email
        _T(""),                         							// Item's homepage
        _T("Contrib"),              							// Category in palette
        70,                             								// Priority in palette
        _T("ImageButton"),              				// Base part of names for new items
        wxsCPP,                         						// List of coding languages supported by this item
        1, 0,                           								// Version
        wxBitmap(wxImageButton32_xpm),    // 32x32 bitmap
        wxBitmap(wxImageButton16_xpm),    // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files


    WXS_ST_BEGIN(wxsImageButtonStyles,_T("wxBU_AUTODRAW"))
        WXS_ST_CATEGORY("wxImageButton")
        WXS_ST(wxBU_LEFT)
        WXS_ST(wxBU_TOP)
        WXS_ST(wxBU_RIGHT)
        WXS_ST(wxBU_BOTTOM)
        WXS_ST(wxBU_AUTODRAW)
        // cyberkoa: "The help mentions that wxBU_EXACTFIX is not used but the XRC code yes
        //  WXS_ST(wxBU_EXACTFIX)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsImageButtonEvents)
        WXS_EVI(EVT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEvent,Click)
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsImageButton::wxsImageButton(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsImageButtonEvents,
        wxsImageButtonStyles)
{
// default values

    mIsDefault      = false;
    mImageList      = _("<none>");
    mLabelIndex     = _("<none>");
    mDisabledIndex  = _("<none>");
    mSelectedIndex  = _("<none>");
    mFocusIndex     = _("<none>");

    mCount = 0;
}

//------------------------------------------------------------------------------

void wxsImageButton::OnBuildCreatingCode() {
// we only handle C++ constructs here

    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsImageButton"),GetLanguage());

// header files

    AddHeader(_T("<wx/bmpbuttn.h>"),GetInfo().ClassName,hfInPCH);

// the basic constructor

    const wxString vv = GetVarName();
    Codef(_T("%s = new wxBitmapButton(%W, %I, wxNullBitmap, %P, %S, %T, %V, %N);\n"), vv.wx_str());

// default button?

    if (mIsDefault) Codef(_T("%ASetDefault();\n"));

// add all the bitmaps at the bottom of the code... after the wxsImage's and wxsImageList's
// have been coded

    wxsImageList* ilist = (wxsImageList *) wxsImageListEditorDlg::FindTool(this, mImageList);
    if (ilist != NULL) {
        const wxString ss = ilist->GetVarName();
		// Locator comment.
        wxString tt;
		tt.Printf(_("// Set the bitmaps for %s.\n"), vv.wx_str());
        AddEventCode(tt);

        long ll = 0;
        if (mLabelIndex.ToLong(&ll)) {
			#if wxCHECK_VERSION(2, 9, 0)
            tt.Printf(_T("%s->SetBitmapLabel(%s->GetBitmap(%d));\n"), vv.wx_str(), ss.wx_str(), ll);
			#else
            tt.Printf(_T("%s->SetBitmapLabel(%s->GetBitmap(%d));\n"), vv.c_str(), ss.c_str(), ll);
			#endif
            AddEventCode(tt);
        };

        if (mDisabledIndex.ToLong(&ll)) {
			#if wxCHECK_VERSION(2, 9, 0)
            tt.Printf(_T("%s->SetBitmapDisabled(%s->GetBitmap(%d));\n"), vv.wx_str(), ss.wx_str(), ll);
			#else
            tt.Printf(_T("%s->SetBitmapDisabled(%s->GetBitmap(%d));\n"), vv.c_str(), ss.c_str(), ll);
			#endif
            AddEventCode(tt);
        };

        if (mSelectedIndex.ToLong(&ll)) {
			#if wxCHECK_VERSION(2, 9, 0)
            tt.Printf(_T("%s->SetBitmapSelected(%s->GetBitmap(%d));\n"), vv.wx_str(), ss.wx_str(), ll);
			#else
            tt.Printf(_T("%s->SetBitmapSelected(%s->GetBitmap(%d));\n"), vv.c_str(), ss.c_str(), ll);
			#endif
            AddEventCode(tt);
        };

        if (mFocusIndex.ToLong(&ll)) {
			#if wxCHECK_VERSION(2, 9, 0)
            tt.Printf(_T("%s->SetBitmapFocus(%s->GetBitmap(%d));\n"), vv.wx_str(), ss.wx_str(), ll);
			#else
            tt.Printf(_T("%s->SetBitmapFocus(%s->GetBitmap(%d));\n"), vv.c_str(), ss.c_str(), ll);
			#endif
            AddEventCode(tt);
        };
    };


// finish setup

    BuildSetupWindowCode();

}

//------------------------------------------------------------------------------

wxObject* wxsImageButton::OnBuildPreview(wxWindow* Parent,long Flags) {
// the basic button

    wxBitmapButton* button = new wxBitmapButton(Parent, GetId(), wxNullBitmap,Pos(Parent),Size(Parent),Style());

// find the image list

    wxsImageList* ilist = (wxsImageList *) wxsImageListEditorDlg::FindTool(this, mImageList);

// make the preview bitmaps

    if (ilist != NULL) {
        long ll = 0;
        if (mLabelIndex.ToLong(&ll))    button->SetBitmapLabel(ilist->GetPreview(ll));
        if (mDisabledIndex.ToLong(&ll)) button->SetBitmapDisabled(ilist->GetPreview(ll));
        if (mSelectedIndex.ToLong(&ll)) button->SetBitmapSelected(ilist->GetPreview(ll));
        if (mFocusIndex.ToLong(&ll))    button->SetBitmapFocus(ilist->GetPreview(ll));
    };

    if (mIsDefault) button->SetDefault();

// done

    return SetupWindow(button,Flags);
}

//------------------------------------------------------------------------------

void wxsImageButton::OnEnumWidgetProperties(long Flags) {
static wxString         sImageNames[128];
static const wxChar    *pImageNames[128];
static wxString         sIndexNames[1024];
static const wxChar    *pIndexNames[1024];

// find available images, and pointer to current imagelist

    wxsImageList* ilist = NULL;
    wxsItemResData* res = GetResourceData();
    int n = 0;
    sImageNames[n] = _("<none>");
    pImageNames[n] = (const wxChar *) sImageNames[n];
    n += 1;
    int k = res->GetToolsCount();
    for(int i=0; i < k; ++i) {
        wxsTool* tool = res->GetTool(i);
        wxString ss = tool->GetUserClass();

        if ((ss == _T("wxImageList")) && (n < 127)) {
            ss = tool->GetVarName();
            sImageNames[n] = ss;
            pImageNames[n] = (const wxChar *) sImageNames[n];
            n += 1;

            if (ss == mImageList) ilist = (wxsImageList *) tool;
        };
    };
    pImageNames[n] = NULL;

    WXS_EDITENUM(wxsImageButton, mImageList, _("Image List"), _T("image_list"), pImageNames, _("<none>"))

// make drop-down list for image index selection

    n = 0;
    sIndexNames[n] = _("<none>");
    pIndexNames[n] = (const wxChar *) sIndexNames[n];
    n += 1;
    if (ilist == NULL) k = 0;
    else               k = ilist->GetCount();

    for(int i = 0; i < k; ++i) {
        sIndexNames[n].Printf(_("%d"), i);
        pIndexNames[n] = (const wxChar *) sIndexNames[n];
        n += 1;
    };
    pIndexNames[n] = NULL;

    WXS_EDITENUM(wxsImageButton, mLabelIndex,    _("Label Index"),    _T("label_index"),    pIndexNames, _("<none>"));
    WXS_EDITENUM(wxsImageButton, mDisabledIndex, _("Disabled Index"), _T("disabled_index"), pIndexNames, _("<none>"));
    WXS_EDITENUM(wxsImageButton, mSelectedIndex, _("Selected Index"), _T("selected_index"), pIndexNames, _("<none>"));
    WXS_EDITENUM(wxsImageButton, mFocusIndex,    _("Focus Index"),    _T("focus_index"),    pIndexNames, _("<none>"));

// the only "normal" property here

    WXS_BOOL(wxsImageButton,mIsDefault,_("Is default"),_T("is_default"),false)
}

//------------------------------------------------------------------------------
// declare the var as a simple wxPanel

void wxsImageButton::OnBuildDeclarationsCode() {
wxString    vname;

    vname = GetVarName();
    AddDeclaration(_T("wxBitmapButton        *") + vname + _T(";"));
}
