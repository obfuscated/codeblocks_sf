/** \file wxsbitmapcombobox.cpp
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
*/

#include <wx/bmpcbox.h>
#include "wxsbitmapcombobox.h"
#include "wxsimagelist.h"
#include "../properties/wxsimagelistdlg.h"


namespace
{
    wxsRegisterItem<wxsBitmapComboBox> Reg(_T("BitmapComboBox"), wxsTWidget, _T("Standard"), 350);


    WXS_ST_BEGIN(wxsBitmapComboBoxStyles, wxEmptyString)
    WXS_ST_CATEGORY("wxBitmapComboBox")
    WXS_ST(wxCB_READONLY)
    WXS_ST(wxCB_SORT)
    WXS_ST(wxTE_PROCESS_ENTER)
    WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsBitmapComboBoxEvents)
		WXS_EVI(EVT_COMBOBOX, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEvent, Selected)
		WXS_EVI(EVT_TEXT, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEvent, TextUpdated)
		WXS_EVI(EVT_TEXT_ENTER, wxEVT_COMMAND_TEXT_ENTER, wxCommandEvent, TextEnter)
    WXS_EV_END()
}

/*! \brief Ctor
 *
 * \param Data wxsItemResData*	The control's resource data.
 *
 */
wxsBitmapComboBox::wxsBitmapComboBox(wxsItemResData *Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsBitmapComboBoxEvents,
        wxsBitmapComboBoxStyles),
    m_defaultSelection(-1)
{
}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsBitmapComboBox::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
            {
                AddHeader(_T("<wx/bmpcbox.h>"), GetInfo().ClassName, hfInPCH);
                Codef(_T("%C(%W, %I, wxEmptyString, %P, %S, 0, 0, %T, %V, %N);\n"));

                for(size_t i = 0; i <  m_arrChoices.GetCount(); ++i)
                {
                    if(m_defaultSelection == (int)i)
                    {
                        Codef(_T("%ASetSelection( "));
                    }
#if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("%AAppend(%t)"), m_arrChoices[i].wx_str());
#else
                    Codef(_T("%AAppend(%t)"), m_arrChoices[i].c_str());
#endif
                    if(m_defaultSelection == (int)i)
                    {
                        Codef(_T(" )"));
                    }
                    Codef(_T(";\n"));
                }

				// Find the image list.
				wxsImageList *imageList = (wxsImageList *) wxsImageListDlg::FindTool(this, m_sImageList);
				int iItemcount = m_arrChoices.GetCount();

				wxString vv = GetVarName();
				if(imageList){
					wxString tt;
					wxString ss = imageList->GetVarName();
					int iImgCount = imageList->GetCount();
					if(iImgCount > 0){
						AddEventCode(_("\n"));
					}
					for(int i = 0;i < iImgCount;i++){
						// Keep within the size limits of the combo box.
						if(i >= iItemcount){
							break;
						}
#if wxCHECK_VERSION(2, 9, 0)
						tt.Printf(_("%s->SetItemBitmap(%d, %s->GetBitmap(%d));\n"), vv.wx_str(), i, ss.wx_str(), i);
#else
						tt.Printf(_("%s->SetItemBitmap(%d, %s->GetBitmap(%d));\n"), vv.c_str(), i, ss.c_str(), i);
#endif
						// add all the bitmaps at the bottom of the code... after the wxsImage's and wxsImageList's have been coded
						AddEventCode(tt);
					}
					if(iImgCount > 0){
						AddEventCode(_("\n"));
					}
				}

               BuildSetupWindowCode();
                return;
            }

        default:
            {
                wxsCodeMarks::Unknown(_T("wxsBitmapComboBox::OnBuildCreatingCode"), GetLanguage());
            }
    }
}

/*! \brief	Build the control preview.
 *
 * \param parent wxWindow*	The parent window.
 * \param flags long				The control flags.
 * \return wxObject* 				The constructed control.
 *
 */
wxObject *wxsBitmapComboBox::OnBuildPreview(wxWindow *Parent, long Flags)
{
    wxBitmapComboBox *preview = new wxBitmapComboBox(Parent, GetId(), wxEmptyString, Pos(Parent), Size(Parent), m_arrChoices, Style());

	// Find the image list.
	wxsImageList *imageList = (wxsImageList *) wxsImageListDlg::FindTool(this, m_sImageList);

	int count = preview->GetCount();
	if(imageList){
		for(int i = 0;i < imageList->GetCount();i++){
			// Keep within the size limits of the combo box.
			if(i >= count){
				break;
			}
			// SetItemBitmap() uses an unsigned int.
			preview->SetItemBitmap((unsigned)i, imageList->GetPreview(i));
		}
	}

    if(m_defaultSelection != -1){
		preview->SetSelection(m_defaultSelection);
    }

    return SetupWindow(preview, Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long	The control flags.
 * \return void
 *
 */
void wxsBitmapComboBox::OnEnumWidgetProperties(long Flags)
{
    WXS_ARRAYSTRING(wxsBitmapComboBox, m_arrChoices, _("Choices"), _T("content"), _T("item"))
    WXS_LONG(wxsBitmapComboBox, m_defaultSelection, _("Selection"), _T("selection"), -1)

	static const wxChar	*pImageNames[128];
	static long 					iImageNames[128];
	int									i, n, iResCount;
	wxsItemResData         *resData;
	wxsTool                			*tool;
	wxString						s;
	wxsImageList           	*imageList;
	static const wxString	sNone(_("<none>"));

	// find available images, and pointer to current imagelist
    imageList = NULL;
    resData = GetResourceData();
    n = 0;
    m_arrImageListNames[n] = sNone;
    pImageNames[n] = (const wxChar *) m_arrImageListNames[n];
    n += 1;
    iResCount = resData->GetToolsCount();
    for(i = 0;i < iResCount;i++){
        tool = resData->GetTool(i);
        s = tool->GetUserClass();

        if((s == _T("wxImageList")) && (n < 127)){
            s = tool->GetVarName();
            m_arrImageListNames[n] = s;
            pImageNames[n] = (const wxChar *) m_arrImageListNames[n];
            iImageNames[n] = n;
            n += 1;

            if(s == m_sImageList){
            	imageList = (wxsImageList *) tool;
            }
        }
    }
    pImageNames[n] = NULL;
    WXS_EDITENUM(wxsBitmapComboBox, m_sImageList, _("Image List"), _T("image_list"), pImageNames, sNone)
}

