/**************************************************************************//**
 * \file            wxslinearreguklartor.cpp
 * \author    Gary Harris
 * \date        19/4/2010.
 *
 * This file is part of wxSmithKWIC.
 *
 * wxSmithKWIC - an add-on for wxSmith, Code::Blocks' GUI editor.                    \n
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
#include "wxslinearregulator.h"
#include "wx/KWIC/LinearRegulator.h"

namespace
{
    // Loading images from xpm files
#include "images/linreg16.xpm"
#include "images/linreg32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsLinearRegulator> Reg(
        _T("kwxLinearRegulator"),                                 // Class name
        wxsTWidget,                                                            // Item type
        _T("KWIC License"),                                           // License
        _T("Andrea V. & Marco Cavallini"),                   // Author
        _T("m.cavallini@koansoftware.com"),              // Author's email
        _T("http://www.koansoftware.com/kwic/"),        // Item's homepage
        _T("KWIC"),                                                             // Category in palette
        30,                                                                            // Priority in palette
        _T("LinearRegulator"),                                       // Base part of names for new items
        wxsCPP,                                                                // List of coding languages supported by this item
        1, 0,                                                                          // Version
        wxBitmap(linreg32_xpm),                                   // 32x32 bitmap
        wxBitmap(linreg16_xpm),                                   // 16x16 bitmap
        true);                                                                        // We do not allow this item inside XRC files

    WXS_EV_BEGIN(wxsLinearRegulatorEvents)
        WXS_EVI(EVT_LINEARREG_CHANGED, kwxEVT_LINEARREG_CHANGE, wxCommandEvent, Changed)
    WXS_EV_END()
}

/*! \brief Constructor.
 *
 * \param Data wxsItemResData*    Pointer to a resource data object.
 *
 */
wxsLinearRegulator::wxsLinearRegulator(wxsItemResData *Data) :
    wxsWidget(
        Data,
        &Reg.Info,
        wxsLinearRegulatorEvents,
        NULL,
        flVariable | flId | flPosition | flSize | flEnabled | flFocused | flHidden | flToolTip | flHelpText | flSubclass | flMinMaxSize),
    m_iValue(0),
    m_iRangeMin(0),
    m_iRangeMax(100),
    m_bHorizontal(true),
    m_bShowVal(true),
    m_bShowLimits(true)
{
}

/*! \brief Destructor.
 */
wxsLinearRegulator::~wxsLinearRegulator()
{
    for(size_t i = 0; i < m_arrTags.Count(); i++){
        delete m_arrTags[i];
    }
    m_arrTags.Clear();
}

/*! \brief Build the code that creates the control.
 *
 * \return void
 *
 */
void wxsLinearRegulator::OnBuildCreatingCode()
{
    switch(GetLanguage())
    {
        case wxsCPP:
        {
            AddHeader(_T("\"wx/KWIC/LinearRegulator.h\""), GetInfo().ClassName);
            Codef(_T("%C(%W,%I,%P,%S, %s);\n"), wxT("wxBORDER_NONE"));

            // Default range is 0-100.
            if(m_iRangeMin != 0 || m_iRangeMax != 100){
                Codef(_T("%ASetRangeVal(%d, %d);\n"), static_cast<int>(m_iRangeMin), static_cast<int>(m_iRangeMax));
            }
            if(!m_bHorizontal){
                Codef(_T("%ASetOrizDirection(false);\n"));
            }
            if(!m_bShowVal){
                Codef(_T("%AShowCurrent(false);\n"));
            }
            if(!m_bShowLimits){
                Codef(_T("%AShowLimits(false);\n"));
            }
            wxString ss = m_cdActiveBarColour.BuildCode(GetCoderContext());
            if(!ss.IsEmpty()) Codef(_T("%ASetActiveBarColour(%s);\n"), ss.wx_str());
            ss = m_cdPassiveBarColour.BuildCode(GetCoderContext());
            if(!ss.IsEmpty()) Codef(_T("%ASetPassiveBarColour(%s);\n"), ss.wx_str());
            ss = m_cdBorderColour.BuildCode(GetCoderContext());
            if(!ss.IsEmpty()) Codef(_T("%ASetBorderColour(%s);\n"), ss.wx_str());
            ss = m_cdLimitTextColour.BuildCode(GetCoderContext());
            if(!ss.IsEmpty()) Codef(_T("%ASetTxtLimitColour(%s);\n"), ss.wx_str());
            ss = m_cdValueTextColour.BuildCode(GetCoderContext());
            if(!ss.IsEmpty()) Codef(_T("%ASetTxtValueColour(%s);\n"), ss.wx_str());
            ss = m_cdTagColour.BuildCode(GetCoderContext());
            if(!ss.IsEmpty()) Codef(_T("%ASetTagsColour(%s);\n"), ss.wx_str());
            for(size_t i = 0; i < m_arrTags.Count(); i++){
                TagDesc *Desc = m_arrTags[i];
                Codef(_T("\t%AAddTag(%d);\n"), Desc->val);
            }
            ss = GetCoderContext()->GetUniqueName(_T("LinearRegulatorFont"));
            wxString sFnt = m_fnt.BuildFontCode(ss, GetCoderContext());
            if(sFnt.Len() > 0)
            {
                Codef(_T("%s"), sFnt.wx_str());
                Codef(_T("%ASetTxtFont(%s);\n"), ss.wx_str());
            }
            // Value needs to be set after other params for correct display.
            if(m_iValue){
                Codef(_T("%ASetValue(%d);\n"), static_cast<int>(m_iValue));
            }

            BuildSetupWindowCode();
            break;
        }
        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsLinearRegulator::OnBuildCreatingCode"), GetLanguage());
    }
}

/*! \brief Build the code that creates the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long                    Flags used when creating the preview.
 * \return wxObject                        The control preview object.
 *
 */
wxObject *wxsLinearRegulator::OnBuildPreview(wxWindow *parent, long flags)
{
    kwxLinearRegulator *preview = new kwxLinearRegulator(parent, GetId(), Pos(parent), Size(parent), wxBORDER_NONE);

    // Default range is 0-100.
    if(m_iRangeMin != 0 || m_iRangeMax != 100){
        preview->SetRangeVal(m_iRangeMin, m_iRangeMax);
    }
    if(!m_bHorizontal){
        preview->SetOrizDirection(false);
    }
    if(!m_bShowVal){
        preview->ShowCurrent(false);
    }
    if(!m_bShowLimits){
        preview->ShowLimits(false);
    }
    wxColour clr = m_cdActiveBarColour.GetColour();
    if(clr.IsOk()){
        preview->SetActiveBarColour(clr);
    }
    clr = m_cdPassiveBarColour.GetColour();
    if(clr.IsOk()){
        preview->SetPassiveBarColour(clr);
    }
    clr = m_cdBorderColour.GetColour();
    if(clr.IsOk()){
        preview->SetBorderColour(clr);
    }
    clr = m_cdLimitTextColour.GetColour();
    if(clr.IsOk()){
        preview->SetTxtLimitColour(clr);
    }
    clr = m_cdValueTextColour.GetColour();
    if(clr.IsOk()){
        preview->SetTxtValueColour(clr);
    }
    clr = m_cdTagColour.GetColour();
    if(clr.IsOk()){
        preview->SetTagsColour(clr);
    }
    for(size_t i = 0; i < m_arrTags.Count(); i++){
        TagDesc *Desc = m_arrTags[i];
        preview->AddTag(Desc->val);
    }
    wxFont fnt = m_fnt.BuildFont();
    if(fnt.IsOk()){
        preview->SetTxtFont(fnt);
    }
    // Value needs to be set after other params for correct display.
    if(m_iValue){
        preview->SetValue(m_iValue);
    }

    return SetupWindow(preview, flags);
}

/*! \brief Enumerate the control's custom properties.
 *
 * \param Flags long    Flags used when creating the control.
 * \return void
 *
 */
void wxsLinearRegulator::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_LONG(wxsLinearRegulator, m_iValue, _("Value"), _T("value"), 0)
    WXS_BOOL(wxsLinearRegulator, m_bShowVal, _("Show Value"), _T("show_value"), true)
    WXS_LONG(wxsLinearRegulator, m_iRangeMin, _("Range Minimum"), _T("range_min"), 0)
    WXS_LONG(wxsLinearRegulator, m_iRangeMax, _("Range Maximum"), _T("range_max"), 100)
    WXS_BOOL(wxsLinearRegulator, m_bShowLimits, _("Show Range Values"), _T("show_limit_value"), true)
    WXS_BOOL(wxsLinearRegulator, m_bHorizontal, _("Horizontal"), _T("horizontal"), true)
    WXS_COLOUR(wxsLinearRegulator, m_cdActiveBarColour, _("Bar Colour"), _T("bar_colour"))
    WXS_COLOUR(wxsLinearRegulator, m_cdPassiveBarColour, _("Background Colour"), _T("background_colour"))
    WXS_COLOUR(wxsLinearRegulator, m_cdBorderColour, _("Border Colour"), _T("border_colour"))
    WXS_COLOUR(wxsLinearRegulator, m_cdLimitTextColour, _("Range Text Colour"), _T("range_text_colour"))
    WXS_COLOUR(wxsLinearRegulator, m_cdValueTextColour, _("Value Text Colour"), _T("value_text_colour"))
    WXS_COLOUR(wxsLinearRegulator, m_cdTagColour, _("Tag Colour"), _T("tag_colour"))
    WXS_FONT(wxsLinearRegulator, m_fnt, _("Font"), _T("font"))
}

// Dynamic properties.
//=================
/*! \brief Add extra control properties.
 *
 * \param Grid wxsPropertyGridManager*    A PropertyGridManager object.
 * \return void
 *
 */
void wxsLinearRegulator::OnAddExtraProperties(wxsPropertyGridManager *Grid)
{
#if wxCHECK_VERSION(3, 0, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    m_TagCountId = Grid->GetGrid()->Insert(_("Bar Colour"), NEW_IN_WXPG14X wxIntProperty(_("Number Of Tags"), wxPG_LABEL, (int)m_arrTags.Count()));
    for(int i = 0; i < (int)m_arrTags.Count(); i++){
        InsertPropertyForTag(Grid, i);
    }
    wxsWidget::OnAddExtraProperties(Grid);
}

/*! \brief One of the control's extra properties changed.
 *
 * \param Grid     wxsPropertyGridManager*    A PropertyGridManager object.
 * \param id         wxPGId                                        The property's ID.
 * \return void
 *
 */
void wxsLinearRegulator::OnExtraPropertyChanged(wxsPropertyGridManager *Grid, wxPGId id)
{
#if wxCHECK_VERSION(3, 0, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    if(id == m_TagCountId){
        int OldValue = (int)m_arrTags.Count();
        int NewValue = Grid->GetPropertyValueAsInt(id);

        if(NewValue < 0){
            NewValue = 0;
            Grid->SetPropertyValue(id, NewValue);
        }

        if(NewValue > OldValue){
            // We have to generate new entries
            for(int i = OldValue; i < NewValue; i++)
            {
                m_arrTags.Add(new TagDesc());
                InsertPropertyForTag(Grid, i);
            }
        }
        else if(NewValue < OldValue){
            // We have to remove some entries
            for(int i = NewValue;i < OldValue;i++){
#if wxCHECK_VERSION(3, 0, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_arrTags[i]->id);
#else
                Grid->Delete(m_arrTags[i]->id);
#endif
                delete m_arrTags[i];
            }

            m_arrTags.RemoveAt(NewValue, OldValue - NewValue);
        }

        NotifyPropertyChange(true);
        return;
    }

    for(int i = 0;i < (int)m_arrTags.Count();i++){
        if(HandleChangeInTag(Grid, id, i)) return;
    }
    wxsWidget::OnExtraPropertyChanged(Grid, id);
}

/*! \brief Read XML control data.
 *
 * \param Element     TiXmlElement*    A pointer to the parent node of the XML block.
 * \param IsXRC         bool                        Whether this is an XRC file.
 * \param IsExtra         bool                        Whether the data is extra information not conforming to the XRC standard.
 * \return bool                                            Success or failure.
 *
 */
bool wxsLinearRegulator::OnXmlRead(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    for(size_t i = 0;i < m_arrTags.Count();i++){
        delete m_arrTags[i];
    }
    m_arrTags.Clear();

    int i = 1;
    while(1){
        wxString s = wxString::Format(wxT("tag_%d_value"), i);
        TiXmlElement *TagElem = Element->FirstChildElement(s.mb_str());
        if(!TagElem){
            break;
        }

        TagDesc *Desc = new TagDesc;
        long iVal;
        wxString sVal(TagElem->GetText(), wxConvUTF8);
        sVal.ToLong(&iVal);
        Desc->val = iVal;
        m_arrTags.Add(Desc);
        i++;
    }

    return wxsWidget::OnXmlRead(Element, IsXRC, IsExtra);
}

/*! \brief Write XML data.
 *
 * \param Element     TiXmlElement*    A pointer to the parent node of the XML block.
 * \param IsXRC         bool                        Whether this is an XRC file.
 * \param IsExtra         bool                        Whether the data is extra information not conforming to the XRC standard.
 * \return bool                                            Success or failure.
 *
 */
bool wxsLinearRegulator::OnXmlWrite(TiXmlElement *Element, bool IsXRC, bool IsExtra)
{
    for(size_t i = 0;i < m_arrTags.Count();i++){
        TagDesc *Desc = m_arrTags[i];
        wxString s = wxString::Format(wxT("tag_%lu_value"), static_cast<unsigned long>(i + 1));
        TiXmlElement *msg = new TiXmlElement(s.mb_str());
        msg->LinkEndChild(new TiXmlText(wxString::Format(wxT("%d"), Desc->val).mb_str()));
        Element->LinkEndChild(msg);
    }

    return wxsWidget::OnXmlWrite(Element, IsXRC, IsExtra);
}

/*! \brief    Insert a new tag value property.
 *
 * \param Grid             wxsPropertyGridManager*    A PropertyGridManager object.
 * \param Position     int                                                The position of this item in the tag array.
 * \return void
 *
 */
void wxsLinearRegulator::InsertPropertyForTag(wxsPropertyGridManager *Grid, int Position)
{
    TagDesc *Desc = m_arrTags[Position];
    wxString sTagName = wxString::Format(_("Tag %d Value"), Position + 1);

    Desc->id = Grid->GetGrid()->Insert(_("Bar Colour"), NEW_IN_WXPG14X wxIntProperty(sTagName, wxPG_LABEL, Desc->val));
}

/*! \brief Check whether a tag value property changed.
 *
 * \param Grid             wxsPropertyGridManager*    A PropertyGridManager object.
 * \param id                 wxPGId                                        The property's ID.
 * \param Position     int                                                The position of this item in the tag array.
 * \return bool            True if a change was recorded, false otherwise.
 *
 */
bool wxsLinearRegulator::HandleChangeInTag(wxsPropertyGridManager *Grid, wxPGId id, int Position)
{
    TagDesc *Desc = m_arrTags[Position];
    bool Changed = false;

    if(Desc->id == id){
        Desc->val = Grid->GetPropertyValueAsInt(id);
        Changed = true;
    }

    if(Changed){
        NotifyPropertyChange(true);
        return true;
    }

    return false;
}

