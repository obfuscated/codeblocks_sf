/**************************************************************************//**
 * \file			wxsbmpswitcher.cpp
 * \author	Gary Harris
 * \date		19/4/2010.
 *
 * This file is part of wxSmithKWIC.
 *
 * wxSmithKWIC - an add-on for wxSmith, Code::Blocks' GUI editor.					\n
 * Copyright (C) 2010 Gary Harris.
 *
 * wxSmithKWIC is free software: you can redistribute it and/or modify
 * it under the terms of the KWIC License
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * KWIC License for more details.
 *
 * You should have received a copy of the KWIC License along with this
 * program.  If not, see <http://www.koansoftware.com/kwic/kwic-license.htm>.
 *
 *****************************************************************************/
#include "wxsbmpswitcher.h"
#include "wx/KWIC/BmpSwitcher.h"

namespace
{
    // Loading images from xpm files
#include "images/bmpswi16.xpm"
#include "images/bmpswi32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsBmpSwitcher> Reg(
        _T("kwxBmpSwitcher"),                     				// Class name
        wxsTWidget,                            						// Item type
        _T("KWIC License"),                       				// License
        _T("Andrea V. & Marco Cavallini"),               	// Author
        _T("m.cavallini@koansoftware.com"),  			// Author's email
        _T("http://www.koansoftware.com/kwic/"),    	// Item's homepage
        _T("KWIC"),                         							// Category in palette
        70,                                    							// Priority in palette
        _T("BmpSwitcher"),                           				// Base part of names for new items
        wxsCPP,                                						// List of coding languages supported by this item
        1, 0,                                  							// Version
        wxBitmap(bmpswi32_xpm),               				// 32x32 bitmap
        wxBitmap(bmpswi16_xpm),               				// 16x16 bitmap
        true);                                							// We do not allow this item inside XRC files
}

/*! \brief Constructor.
 *
 * \param Data wxsItemResData*	Pointer to a resource data object.
 *
 */
wxsBmpSwitcher::wxsBmpSwitcher(wxsItemResData *Data) :
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable | flId | flPosition | flSize | flEnabled | flFocused | flHidden | flToolTip | flHelpText | flSubclass | flMinMaxSize)
{
}

/*! \brief Destructor.
 */
wxsBmpSwitcher::~wxsBmpSwitcher()
{
    for(size_t i = 0; i < m_arrBmps.Count(); i++){
        delete m_arrBmps[i];
    }
    m_arrBmps.Clear();
}

/*! \brief Build the code that creates the control.
 *
 * \return void
 *
 */
void wxsBmpSwitcher::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
		{
			AddHeader(_T("\"wx/KWIC/BmpSwitcher.h\""), GetInfo().ClassName);
			Codef(_T("%C(%W, %I, %P, %S);\n"));

			for(size_t i = 0; i < m_arrBmps.Count(); i++){
				BmpDesc *Desc = m_arrBmps[i];
				// Escape Windows path separators.
				wxString sPath = Desc->sPath;
				sPath.Replace(wxT("\\"), wxT("\\\\"));
				Codef(_T("\t%AAddBitmap(new wxBitmap(wxImage(wxT(\"%s\"))));\n"), sPath.wx_str());
			}
			// Default is 0. If state <= 0 or > number of bitmaps, use default.
			if(m_iState > 0 && m_iState < (long)m_arrBmps.Count()){
				Codef(_T("\t%ASetState(%d);\n"), static_cast<int>(m_iState));
			}

			BuildSetupWindowCode();
			break;
		}
        default:
            wxsCodeMarks::Unknown(_T("wxsBmpSwitcher::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Build the code that creates the control preview.
 *
 * \param parent wxWindow*	The parent window.
 * \param flags long					Flags used when creating the preview.
 * \return wxObject						The control preview object.
 *
 */
wxObject *wxsBmpSwitcher::OnBuildPreview(wxWindow *parent, long flags)
{
    kwxBmpSwitcher *preview = new kwxBmpSwitcher(parent, GetId(), Pos(parent), Size(parent));

	for(size_t i = 0; i < m_arrBmps.Count(); i++){
		BmpDesc *Desc = m_arrBmps[i];
		preview->AddBitmap(new wxBitmap(wxImage(Desc->sPath)));
	}
	// Default is 0. If state <= 0 or > number of bitmaps, use default.
//	if(m_iState > 0 && m_iState < (long)m_arrBmps.Count()){
		preview->SetState((int)m_iState);
//	}

    return SetupWindow(preview, flags);
}

/*! \brief Enumerate the control's custom properties.
 *
 * \param Flags long	Flags used when creating the control.
 * \return void
 *
 */
void wxsBmpSwitcher::OnEnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsBmpSwitcher, m_iState, _("State"), _T("state"), 0)
}

// Dynamic properties.
//=================
/*! \brief Add extra control properties.
 *
 * \param Grid wxsPropertyGridManager*	A PropertyGridManager object.
 * \return void
 *
 */
void wxsBmpSwitcher::OnAddExtraProperties(wxsPropertyGridManager *Grid)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
	m_BmpCountId = Grid->GetGrid()->Insert(_("Var name"), NEW_IN_WXPG14X wxIntProperty(_("Number Of Bitmaps"), wxPG_LABEL, (int)m_arrBmps.Count()));
    for(int i = 0; i < (int)m_arrBmps.Count(); i++){
        InsertPropertyForBmp(Grid, i);
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
void wxsBmpSwitcher::OnExtraPropertyChanged(wxsPropertyGridManager *Grid, wxPGId id)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    if(id == m_BmpCountId){
        int OldValue = (int)m_arrBmps.Count();
        int NewValue = Grid->GetPropertyValueAsInt(id);

        if(NewValue < 0){
            NewValue = 0;
            Grid->SetPropertyValue(id, NewValue);
        }

        if(NewValue > OldValue){
            // We have to generate new entries
            for(int i = OldValue; i < NewValue; i++){
                m_arrBmps.Add(new BmpDesc());
                InsertPropertyForBmp(Grid, i);
            }
        }
        else if(NewValue < OldValue){
            // We have to remove some entries
            for(int i = NewValue;i < OldValue;i++){
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_arrBmps[i]->id);
#else
                Grid->Delete(m_arrBmps[i]->id);
#endif
                delete m_arrBmps[i];
            }

            m_arrBmps.RemoveAt(NewValue, OldValue - NewValue);
        }

        NotifyPropertyChange(true);
        return;
    }

    for(int i = 0;i < (int)m_arrBmps.Count();i++){
        if(HandleChangeInBmp(Grid, id, i)) return;
    }
    wxsWidget::OnExtraPropertyChanged(Grid, id);
}

/*! \brief Read XML control data.
 *
 * \param Element 	TiXmlElement*	A pointer to the parent node of the XML block.
 * \param IsXRC 		bool						Whether this is an XRC file.
 * \param IsExtra 		bool						Whether the data is extra information not conforming to the XRC standard.
 * \return bool											Success or failure.
 *
 */
bool wxsBmpSwitcher::OnXmlRead(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    for(size_t i = 0;i < m_arrBmps.Count();i++){
        delete m_arrBmps[i];
    }
    m_arrBmps.Clear();

    TiXmlElement *BmpElem = Element->FirstChildElement("bitmaps");
	// Avoid crash if bmps element doesn't exist.
	if(BmpElem){
		for(TiXmlElement *PathElem = BmpElem->FirstChildElement();
				PathElem;
				PathElem = PathElem->NextSiblingElement())
		{
			BmpDesc *Desc = new BmpDesc;
			wxString sBmp(PathElem->GetText(), wxConvUTF8);
			Desc->sPath = sBmp;
			m_arrBmps.Add(Desc);
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
bool wxsBmpSwitcher::OnXmlWrite(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    TiXmlElement *tags = new TiXmlElement("bitmaps");
    Element->LinkEndChild(tags);

    for(size_t i = 0;i < m_arrBmps.Count();i++){
        BmpDesc *Desc = m_arrBmps[i];
        wxString s = wxString::Format(wxT("bitmap_%lu"), static_cast<unsigned long>(i + 1));
        TiXmlElement *msg = new TiXmlElement(s.mb_str());
        msg->LinkEndChild(new TiXmlText(Desc->sPath.mb_str()));
        tags->LinkEndChild(msg);
    }

    return wxsWidget::OnXmlWrite(Element, IsXRC, IsExtra);
}

/*! \brief	Insert a new bitmap property.
 *
 * \param Grid 			wxsPropertyGridManager*	A PropertyGridManager object.
 * \param Position 	int												The position of this item in the bitmap array.
 * \return void
 *
 */
void wxsBmpSwitcher::InsertPropertyForBmp(wxsPropertyGridManager *Grid, int Position)
{
    BmpDesc *Desc = m_arrBmps[Position];
    wxString sBmpName = wxString::Format(_("Bitmap %d"), Position + 1);

	Desc->id = Grid->GetGrid()->Insert(_("Var name"), NEW_IN_WXPG14X wxImageFileProperty(sBmpName, wxPG_LABEL));
	// Set the property's image path.
	Grid->SetPropertyValueString(Desc->id, Desc->sPath);
}

/*! \brief Check whether a bitmap property changed.
 *
 * \param Grid 			wxsPropertyGridManager*	A PropertyGridManager object.
 * \param id 				wxPGId										The property's ID.
 * \param Position 	int												The position of this item in the tag array.
 * \return bool			True if a change was recorded, false otherwise.
 *
 */
bool wxsBmpSwitcher::HandleChangeInBmp(wxsPropertyGridManager *Grid, wxPGId id, int Position)
{
    BmpDesc *Desc = m_arrBmps[Position];
    bool Changed = false;

	if(Desc->id == id){
		Desc->sPath = Grid->GetPropertyValueAsString(id);
        Changed = true;
    }

    if(Changed){
        NotifyPropertyChange(true);
        return true;
    }

    return false;
}

