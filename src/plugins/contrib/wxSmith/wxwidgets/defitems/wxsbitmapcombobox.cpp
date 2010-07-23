/** \file wxsbitmapcombobox.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2010 Gary Harris
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

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(BmpComboBitmapDataArray);


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
    m_defaultSelection(-1),
    m_iNumImages(0)
{
    UpdateArraySizes(m_iNumImages);
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
				// SetItemBitmap() uses an unsigned int.
				for(unsigned int i = 0;i < m_arrImagePaths.GetCount();i++){
                    Codef(_T("%ASetItemBitmap(%d, wxBitmap(%n, wxBITMAP_TYPE_ANY));\n"), i, m_arrImagePaths[i].c_str());
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
    // SetItemBitmap() uses an unsigned int.
    for(unsigned int i = 0;i < m_arrImagePaths.GetCount();i++){
		preview->SetItemBitmap(i, wxBitmap(m_arrImagePaths[i], wxBITMAP_TYPE_ANY));
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
}

/*! \brief Add extra control properties.
 *
 * \param Grid wxsPropertyGridManager*	A PropertyGridManager object.
 * \return void
 *
 */
 void wxsBitmapComboBox::OnAddExtraProperties(wxsPropertyGridManager *Grid)
{
    const wxString sPriorTo(_("Selection"));
    wxString sImage(_("Image"));
    wxString sImages(_("Images"));

#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    m_idNumImages = Grid->GetGrid()->Insert(sPriorTo, NEW_IN_WXPG14X wxIntProperty(_("Number of images"), wxPG_LABEL, m_iNumImages));

    if(m_iNumImages > 0) {
        m_idImages = Grid->GetGrid()->Insert(sPriorTo, NEW_IN_WXPG14X wxParentProperty(sImages.c_str(), wxPG_LABEL));
	}
    for(int i = 0; i < m_iNumImages; i++) {
        m_arrBitmapIds[i] = Grid->GetGrid()->AppendIn(m_idImages, NEW_IN_WXPG14X wxImageFileProperty(wxString::Format(wxT("%s %d"), sImage.c_str(), i + 1), wxPG_LABEL));
		Grid->SetPropertyValue(m_arrBitmapIds[i], m_arrImagePaths[i]);
    }

    wxsWidget::OnAddExtraProperties(Grid);
}

/*! \brief One of the control's extra properties changed.
 *
 * \param Grid 	wxsPropertyGridManager*	A PropertyGridManager object.
 * \param id 		wxPGId										The property's ID.
 * \return void
 *
 */
void wxsBitmapComboBox::OnExtraPropertyChanged(wxsPropertyGridManager *Grid, wxPGId Id)
{
    const wxString sPriorTo(_("Selection"));
    wxString sImage(_("Image"));
    wxString sImages(_("Images"));

    // The "Number of images" field has changed.
    if(Id == m_idNumImages) {
        // Number of fields is going to change...
        int iNewFields = Grid->GetPropertyValueAsInt(Id);
        if(iNewFields < 0) {
            iNewFields = 0;
            Grid->SetPropertyValue(Id, iNewFields);
        }

        // Now it's time to delete / add properties for fields
        if(iNewFields < m_iNumImages) {
            for(int i = iNewFields;i < m_iNumImages;i++){
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_arrBitmapIds[i]);
#else
                Grid->Delete(m_arrBitmapIds[i]);
#endif
			}
            // If there are no images, delete the parent field.
            if(iNewFields == 0) {
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_idImages);
#else
                Grid->Delete(m_idImages);
#endif
                m_idImages = NULL;
            }
        }
        else if(iNewFields > m_iNumImages){
            // Adding new properties
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
			Grid->SelectPage(0);
#else
			Grid->SetTargetPage(0);
#endif
			UpdateArraySizes(iNewFields);
            // If the parent field was previously deleted, recreate it.
            if(!m_idImages) {
                m_idImages = Grid->GetGrid()->Insert(sPriorTo, NEW_IN_WXPG14X wxParentProperty(sImages.c_str(), wxPG_LABEL));
            }
            for(int i = m_iNumImages; i < iNewFields; i++) {
                m_arrBitmapIds[i] = Grid->GetGrid()->AppendIn(m_idImages, NEW_IN_WXPG14X wxImageFileProperty(wxString::Format(wxT("%s %d"), sImage.c_str(), i + 1), wxPG_LABEL));
            }
        }

        m_iNumImages = iNewFields;
        NotifyPropertyChange(true);
        return;
    }

    // One of the image paths has changed.
    for(int i = 0; i < m_iNumImages; i++) {
        if(m_arrBitmapIds[i] == Id) {
            m_arrImagePaths[i] = Grid->GetPropertyValueAsString(Id);
            NotifyPropertyChange(true);
            return;
        }
    }

    wxsWidget::OnExtraPropertyChanged(Grid,Id);
}

/*! \brief Read XML control data.
 *
 * \param Element 	TiXmlElement*	A pointer to the parent node of the XML block.
 * \param IsXRC 		bool						Whether this is an XRC file.
 * \param IsExtra 		bool						Whether the data is extra information not conforming to the XRC standard.
 * \return bool											Success or failure.
 *
 */
 bool wxsBitmapComboBox::OnXmlRead(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    if(IsXRC)
    {
        TiXmlElement *FieldsCnt = Element->FirstChildElement("num_images");
        if(!FieldsCnt){
            m_iNumImages = 0;
        }
        else{
            m_iNumImages = wxAtoi(cbC2U(FieldsCnt->GetText()));
        }

        if(m_iNumImages < 0){
			m_iNumImages = 0;
        }
        UpdateArraySizes(m_iNumImages);

        for(int i = 0;i < m_iNumImages;i++){
			wxString s = wxString::Format(_T("image_%d"), i);
			if(TiXmlElement *ImageElem = Element->FirstChildElement(cbU2C(s))){
				m_arrImagePaths[i] = cbC2U(ImageElem->GetText());
			}
        }
    }

    return wxsWidget::OnXmlRead(Element, IsXRC, IsExtra);
}

/*! \brief Write XML data.
 *
 * \param Element 	TiXmlElement*	A pointer to the parent node of the XML block.
 * \param IsXRC 		bool						Whether this is an XRC file.
 * \param IsExtra 		bool						Whether the data is extra information not conforming to the XRC standard.
 * \return bool											Success or failure.
 *
 */
 bool wxsBitmapComboBox::OnXmlWrite(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    if(IsXRC)
    {
        Element->InsertEndChild(TiXmlElement("num_images"))->InsertEndChild(TiXmlText(cbU2C(wxString::Format(_T("%d"), m_iNumImages))));

        for(int i = 0;i < m_iNumImages;i++){
			wxString s = wxString::Format(_T("image_%d"), i);
			Element->InsertEndChild(TiXmlElement(cbU2C(s)))->InsertEndChild(TiXmlText(cbU2C(m_arrImagePaths[i])));
        }
    }

    return wxsWidget::OnXmlWrite(Element, IsXRC, IsExtra);
}

/*! \brief Update arrays sizes to match changes in the properties.
 *
 * \param 	size 	int	The new array size.
 * \return 	void
 *
 */
void wxsBitmapComboBox::UpdateArraySizes(int size)
{
    m_arrBitmapIds.SetCount(size);
    m_arrImagePaths.SetCount(size);
}
