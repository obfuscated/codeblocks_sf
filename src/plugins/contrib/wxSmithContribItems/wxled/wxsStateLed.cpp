#include "wxsStateLed.h"
#include "wx/stateLed.h"

namespace
{

    #include "images/wxstateled16.xpm"
    #include "images/wxstateled32.xpm"


    wxsRegisterItem<wxsStateLed> Reg(
        _T("wxStateLed"),
        wxsTWidget,
        _T("wxWindows"),
        _T("Jonas Zinn"),
        _T(""),
        _T(""),
        _T("Led"),
        80,
        _T("StateLed"),
        wxsCPP,
        1, 0,
        wxBitmap(wxStateLed32_xpm),
        wxBitmap(wxstateled16_xpm),
        true);
}

wxsStateLed::wxsStateLed(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable | flId|flEnabled)
{
    //ctor
    m_Disable = wxColour(128,128,128);
    GetBaseProps()->m_Enabled = false;
    m_State = 0;
    m_numberOfState = 0;
}

wxsStateLed::~wxsStateLed()
{
    //dtor
}

void wxsStateLed::OnBuildCreatingCode()
{

    wxString ss  = m_Disable.BuildCode(GetCoderContext());

    switch ( GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/stateLed.h>"),GetInfo().ClassName);
            Codef(_T("%C(%W,%I,wxColour(wxT(\"%s\")),%P,%S);\n"), ss.wx_str());

            for( std::map<int,StateDesc>::iterator k = m_StateColor.begin(); k != m_StateColor.end(); k++)
            {
                ss  = (*k).second.colour.GetAsString(wxC2S_CSS_SYNTAX);
                Codef(_T("%ARegisterState(%d,wxColour(wxT(\"%s\")));\n"),(*k).first, ss.wx_str());
            }

            if ( !GetBaseProps()->m_Enabled)
                Codef(_T("%ADisable();\n"));
            else
                Codef(_T("%ASetState(%d);\n"), m_State);
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsStateLed::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsStateLed::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxStateLed *Led = new wxStateLed(Parent,GetId(),m_Disable.GetColour(),Pos(Parent),Size(Parent));

    for( std::map<int,StateDesc>::iterator k = m_StateColor.begin(); k != m_StateColor.end(); k++)
        Led->RegisterState((*k).first, (*k).second.colour.GetAsString(wxC2S_CSS_SYNTAX));

    if( !GetBaseProps()->m_Enabled)
        Led->Disable();
    else
    {
        Led->Enable();
        if( m_State > 0)
            Led->SetState(m_State-1);
    }
    return Led;
}

void wxsStateLed::OnEnumWidgetProperties(long Flags)
{

    WXS_COLOUR(
    wxsStateLed,
    m_Disable,
    _("Disable Colour"),
    _T("disable_colour"));
}

// Dynamic properties.
//=================
/*! \brief Add extra control properties.
 *
 * \param Grid wxsPropertyGridManager*	A PropertyGridManager object.
 * \return void
 *
 */
void wxsStateLed::OnAddExtraProperties(wxsPropertyGridManager *Grid)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    m_StateCurrentId =  Grid->GetGrid()->Insert(_("Disable Colour"), NEW_IN_WXPG14X wxIntProperty(_("Current State"), wxPG_LABEL, m_State));
    m_StateCountId =  Grid->GetGrid()->Insert(_("Current State"), NEW_IN_WXPG14X wxIntProperty(_("Number Of States"), wxPG_LABEL, m_numberOfState));
    for(int i = 0; i < m_numberOfState; i++){
        InsertPropertyForState(Grid, i);
    }
    wxsWidget::OnAddExtraProperties(Grid);
}

/*! \brief	Insert a new sector colour property.
 *
 * \param Grid 		wxsPropertyGridManager*	A PropertyGridManager object.
 * \param Position 	int									The position of this item in the sector array.
 * \return void
 *
 */
void wxsStateLed::InsertPropertyForState(wxsPropertyGridManager *Grid, int Position)
{
    wxString SectorName = wxString::Format(_("State %d Colour"), Position + 1);
    wxString SectorDataName = wxString::Format(_("state_%d_colour"), Position + 1);

	m_StateColor[Position].id = Grid->GetGrid()->Insert(_("Current State"), NEW_IN_WXPG14X wxSystemColourProperty(SectorName, wxPG_LABEL, m_StateColor[Position].colour));
}

/*! \brief One of the control's extra properties changed.
 *
 * \param Grid 	wxsPropertyGridManager*	A PropertyGridManager object.
 * \param id 		wxPGId										The property's ID.
 * \return void
 *
 */
void wxsStateLed::OnExtraPropertyChanged(wxsPropertyGridManager *Grid, wxPGId id)
{
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
    Grid->SelectPage(0);
#else
    Grid->SetTargetPage(0);
#endif
    if(id == m_StateCountId){
        int NewValue = Grid->GetPropertyValueAsInt(id);

        // There must be at least one sector.
        if(NewValue < 0){
            NewValue = 0;
            Grid->SetPropertyValue(id, NewValue);
        }

        if(NewValue > m_numberOfState){
            // We have to generate new entries
            for(int i = m_numberOfState; i < NewValue; i++)
            {
                StateDesc temp;
                temp.colour = *wxWHITE;
                m_StateColor[i] = temp;
                InsertPropertyForState(Grid, i);
            }
        }
        else if( NewValue < m_numberOfState){
            // We have to remove some entries
            for(int i = NewValue; i < m_numberOfState; i++){
#if wxCHECK_VERSION(2, 9, 0) || wxCHECK_PROPGRID_VERSION(1, 4, 0)
                Grid->DeleteProperty(m_StateColor[i].id);
#else
                Grid->Delete(m_StateColor[i].id);
#endif
            }

            if( NewValue < m_numberOfState)
            {
                for( int i = NewValue; i < m_numberOfState; i++)
                    m_StateColor.erase(i);
                if( m_State > NewValue)
                {
                    m_State = NewValue;
                    Grid->SetPropertyValue(m_StateCurrentId,m_State);
                }
            }
            else if( NewValue > 0 && m_numberOfState == 0)
            {
                m_State = 1;
                Grid->SetPropertyValue(m_StateCurrentId,m_State);
            }
        }
        m_numberOfState = NewValue;
        NotifyPropertyChange(true);
        return;
    }
    else if( id == m_StateCurrentId)
    {
        m_State = Grid->GetPropertyValueAsInt(id);
        if( m_State > m_numberOfState)
        {
            m_State = m_numberOfState;
            Grid->SetPropertyValue(id, m_State);
        }
        else if(m_State < 0)
        {
            m_State = 0;
            Grid->SetPropertyValue(id, m_State);
        }
        NotifyPropertyChange(true);
    }
    else
    {

        for(int i = 0; i < m_numberOfState; i++){
            if(HandleChangeInState(Grid, id, i)) return;
        }
    }
    wxsWidget::OnExtraPropertyChanged(Grid, id);
}

/*! \brief Check whether a sector colour property changed.
 *
 * \param Grid 			wxsPropertyGridManager*	A PropertyGridManager object.
 * \param id 				wxPGId										The property's ID.
 * \param Position 	int												The position of this item in the tag array.
 * \return bool			True if a change was recorded, false otherwise.
 *
 */
bool wxsStateLed::HandleChangeInState(wxsPropertyGridManager *Grid, wxPGId id, int Position)
{
    bool Changed = false;

	if(m_StateColor[Position].id == id){

#if wxCHECK_VERSION(2,9,0)
        wxVariant var = Grid->GetPropertyValue(id);
        wxString sPropType = var.GetType();
		if(sPropType.IsSameAs(wxT("wxColourPropertyValue"))){
			wxColourPropertyValue pcolval;
			pcolval << var;
			m_StateColor[Position].colour = pcolval.m_colour;
		}
#else
    #if wxCHECK_PROPGRID_VERSION(1, 3, 0)
		wxString sPropType = Grid->GetPropertyValueType(id);
		if(sPropType.IsSameAs(wxT("wxColourPropertyValue"))){
    #else
		if(Grid->IsPropertyValueType(id, CLASSINFO(wxColourPropertyValue))){
    #endif
			wxColourPropertyValue* pcolval = wxDynamicCast(Grid->GetPropertyValueAsWxObjectPtr(id), wxColourPropertyValue);
			m_StateColor[Position].colour = pcolval->m_colour;
		}
#endif
        Changed = true;
    }

    if(Changed){
        NotifyPropertyChange(true);
        return true;
    }

    return false;
}

bool wxsStateLed::OnXmlWrite(TiXmlElement* Element, bool IsXRC, bool IsExtra)
{

    TiXmlElement* ColourElem = Element->InsertEndChild(TiXmlElement("colour"))->ToElement();
    ColourElem->SetAttribute("count",cbU2C(wxString::Format(wxT("%d"), m_numberOfState)));
    ColourElem->SetAttribute("current",cbU2C(wxString::Format(wxT("%d"), m_State)));
    for( int i = 0; i < m_numberOfState; i++)
    {
        TiXmlElement* msg = new TiXmlElement(cbU2C(wxString::Format(wxT("colour_%d"), i)));
        TiXmlText*value = new TiXmlText(cbU2C(m_StateColor[i].colour.GetAsString(wxC2S_HTML_SYNTAX)));
        msg->LinkEndChild(value);
        ColourElem->LinkEndChild(msg);
    }

    return wxsWidget::OnXmlWrite(Element, IsXRC, IsExtra);
}

bool wxsStateLed::OnXmlRead(TiXmlElement* Element, bool IsXRC, bool IsExtra)
{

    TiXmlElement* ColourElem = Element->FirstChildElement("colour");
    if(ColourElem != NULL)
    {
        TiXmlAttribute* att = ColourElem->FirstAttribute();
        m_numberOfState = wxAtoi(cbC2U(att->Value()));
        att = att->Next();
        m_State = wxAtoi(cbC2U(att->Value()));
        int i = 0;
        for( TiXmlElement* data = ColourElem->FirstChildElement(); data != NULL; data = data->NextSiblingElement())
        {
            m_StateColor[i++].colour.Set(cbC2U(data->GetText()));
        }
    }

    return wxsWidget::OnXmlRead(Element, IsXRC, IsExtra);
}

