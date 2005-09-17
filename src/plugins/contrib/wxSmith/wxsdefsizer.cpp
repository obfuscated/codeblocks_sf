#include "wxsdefsizer.h"

#include "properties/wxsborderproperty.h"
#include "properties/wxsplacementproperty.h"
#include "wxswidgetfactory.h"
#include <wx/tokenzr.h>

class wxsDefSizerPreview: public wxPanel
{
    public:
        wxsDefSizerPreview(wxWindow* Parent,wxsDefSizer* wxsSizer,wxSizer* _Sizer):
            wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL),
            Sizer(_Sizer),
            sSizer(wxsSizer)
        {
        	InheritAttributes();
        }
        
        
        wxSizer* Sizer;
        wxsDefSizer* sSizer;
        
        void UpdatePreview()
        {
            assert ( sSizer != NULL );
            
            int Cnt = sSizer->GetChildCount();
            for ( int i=0; i<Cnt; i++ )
            {
                wxsWidget* Child = sSizer->GetChild(i);
                const wxsSizerExtraParams& Par = *sSizer->GetExtraParams(i);
                int Flags = 0;
                if ( Par.BorderFlags & Par.Top )    Flags |= wxTOP;
                if ( Par.BorderFlags & Par.Bottom ) Flags |= wxBOTTOM;
                if ( Par.BorderFlags & Par.Left )   Flags |= wxLEFT;
                if ( Par.BorderFlags & Par.Right )  Flags |= wxRIGHT;
                switch ( Par.Placement )
                {
                    case wxsSizerExtraParams::LeftTop:      Flags |= wxALIGN_LEFT | wxALIGN_TOP;  break;
                    case wxsSizerExtraParams::LeftCenter:   Flags |= wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL; break;
                    case wxsSizerExtraParams::LeftBottom:   Flags |= wxALIGN_LEFT | wxALIGN_BOTTOM; break;
                    case wxsSizerExtraParams::CenterTop:    Flags |= wxALIGN_CENTER_HORIZONTAL | wxTOP; break;
                    case wxsSizerExtraParams::Center:       Flags |= wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL; break;
                    case wxsSizerExtraParams::CenterBottom: Flags |= wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM; break;
                    case wxsSizerExtraParams::RightTop:     Flags |= wxALIGN_RIGHT | wxALIGN_TOP;  break;
                    case wxsSizerExtraParams::RightCenter:  Flags |= wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL; break;
                    case wxsSizerExtraParams::RightBottom:  Flags |= wxALIGN_RIGHT | wxALIGN_BOTTOM; break;
                }
                if ( Par.Expand ) Flags |= wxEXPAND;
                if ( Par.Shaped ) Flags |= wxSHAPED;
                Sizer->Add(
                    Child->GetPreview(),
                    Par.Proportion,
                    Flags,
                    Par.Border
                );
            }
            SetSizer(Sizer);
            Sizer->SetSizeHints(this);
            Layout();
            SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        }
        
        virtual bool HasTransparentBackground() const { return true; }
        
    private:
    
        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC DC(this);
            int W, H;
            GetSize(&W,&H);
            DC.SetBrush(GetParent()->GetBackgroundColour());
            DC.SetPen(*wxRED_PEN);
            DC.DrawRectangle(0,0,W,H);
        }
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsDefSizerPreview,wxPanel)
    EVT_PAINT(wxsDefSizerPreview::OnPaint)
//    EVT_ERASE_BACKGROUND(wxsGridSizerPreview::OnEraseBack)
END_EVENT_TABLE()

wxsDefSizer::wxsDefSizer(wxsWidgetManager* Man,wxsWindowRes* Res,BasePropertiesType pType):
    wxsContainer(Man,Res,false,0,pType)
{
}

wxsDefSizer::~wxsDefSizer()
{
}

wxString wxsDefSizer::GetFinalizingCode(wxsCodeParams& Params)
{
	wxString Code;
	int Cnt = GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = GetChild(i);
		wxsSizerExtraParams* Params = GetExtraParams(i);
		if ( Child->GetInfo().Spacer )
		{
			// Spacer class is threated as a special case
			Code.Append(wxString::Format(_T("%s->Add(%d,%d,%d);"),
                GetBaseParams().VarName.c_str(),
                Child->GetBaseParams().SizeX,
                Child->GetBaseParams().SizeY,
                Params->Proportion));
		}
		else
		{
            wxString FlagsToSizer = GetFlagToSizer(Params);
            if ( !FlagsToSizer.Length() ) FlagsToSizer = _T("0");
            Code.Append(
                wxString::Format(_T("%s->Add(%s,%d,%s,%d);\n"),
                    GetBaseParams().VarName.c_str(),
                    Child->GetBaseParams().VarName.c_str(),
                    Params->Proportion,
                    FlagsToSizer.c_str(),
                    Params->Border));
		}
	}
	
    if ( Params.IsDirectParent )
    {
        Code.Append(wxString::Format(_T("%s->SetSizer(%s);"),
            Params.ParentName.c_str(),
            BaseParams.VarName.c_str()));
    }
    return Code;
}

wxWindow* wxsDefSizer::MyCreatePreview(wxWindow* Parent)
{
    wxsDefSizerPreview* Preview = new wxsDefSizerPreview(Parent,this,NULL);
    Preview->Sizer = ProduceSizer(Preview);
    return Preview;
}

void wxsDefSizer::MyFinalUpdatePreview(wxWindow* Window)
{
    ((wxsDefSizerPreview*)Window) -> UpdatePreview();
}

bool wxsDefSizer::XmlLoadChild(TiXmlElement* Element)
{
	if ( strcmp(Element->Value(),"object") ) return true;
	
	bool Ret = true;
	TiXmlElement* RealObject = Element;
    const char* Class = Element->Attribute("class");
    if ( Class && !strcmp(Class,"spacer") )
    {
        wxsWidget* Child = wxsWidgetFactory::Get()->Generate(_T("Spacer"),GetResource());
        if ( !Child ) return false;
        if ( !Child->XmlLoad(Element) ) Ret = false;
        int Index = AddChild(Child);
        if ( Index < 0 )
        {
            delete Child;
            return false;
        }
        return LoadSizerStuff(GetExtraParams(Index),Element) && Ret;
    }
    else if ( Class && !strcmp(Class,"sizeritem") )
    {
        RealObject = Element->FirstChildElement("object");
    }
	
	if ( !RealObject ) return false;
	
    const char* Name = RealObject->Attribute("class");
    
    if ( !Name || !*Name ) return false;
    
    wxsWidget* Child = wxsWidgetFactory::Get()->Generate(wxString(Name,wxConvUTF8),GetResource());
    if ( !Child ) return false;
    
    if ( !Child->XmlLoad(RealObject) ) Ret = false;
    int Index = AddChild(Child);
    if ( Index < 0 ) 
    {
        delete Child;
        return false;
    }
    
    return LoadSizerStuff(GetExtraParams(Index),Element) && Ret;
}

bool wxsDefSizer::XmlSaveChild(int ChildIndex,TiXmlElement* AddHere)
{
	bool Ret = true;
	wxsWidget* Child = GetChild(ChildIndex);
	if ( Child->GetInfo().Spacer )
	{
		TiXmlElement* SaveHere = AddHere->InsertEndChild(TiXmlElement("object"))->ToElement();
		if ( SaveHere )
		{
			SaveHere->SetAttribute("class","spacer");
			if ( ! Child->XmlSave(SaveHere) ) Ret = false;
			if ( !SaveSizerStuff(GetExtraParams(ChildIndex),SaveHere) ) Ret = false;
			return Ret;
		}
		return false;
	}
	
	TiXmlElement* SizerItem = AddHere->InsertEndChild(TiXmlElement("object"))->ToElement();
	SizerItem->SetAttribute("class","sizeritem");
    if ( !SaveSizerStuff(GetExtraParams(ChildIndex),SizerItem) ) Ret = false;
    return wxsWidget::XmlSaveChild(ChildIndex,SizerItem) && Ret;
}

void wxsDefSizer::AddChildProperties(int ChildIndex)
{
	wxsWidget* Widget = GetChild(ChildIndex);
	wxsSizerExtraParams* Params = GetExtraParams(ChildIndex);
	if ( !Widget || !Params ) return;
	
    Widget->GetPropertiesObj().AddProperty(_("Proportion:"),Params->Proportion);
    Widget->GetPropertiesObj().AddProperty(_("Border:"),new wxsBorderProperty(&Widget->GetPropertiesObj(),Params->BorderFlags));
    Widget->GetPropertiesObj().AddProperty(_("Border size:"),Params->Border);
    Widget->GetPropertiesObj().AddProperty(_("Placement:"),new wxsPlacementProperty(&Widget->GetPropertiesObj(),Params->Placement,Params->Expand,Params->Shaped));
}

bool wxsDefSizer::LoadSizerStuff(wxsSizerExtraParams* Params,TiXmlElement* Elem)
{
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(Elem);

    bool Temp;

    Params->Proportion = XmlGetInteger(_T("option"),Temp,0);
    Params->Border = XmlGetInteger(_T("border"),Temp,0);
    Params->BorderFlags = 0;
    Params->Placement = wxsSizerExtraParams::Center;
    Params->Expand = false;
    Params->Shaped = false;
    Params->FixedMinSize = false;

    wxStringTokenizer tokens(XmlGetVariable(_T("flag")),_T("|"));

    int HorizPos = 0;
    int VertPos = 0;

    while ( tokens.HasMoreTokens() )
    {
        wxString Token = tokens.GetNextToken().Trim(true).Trim(false);

        #define BFItem(a,b)  else if ( Token == _T(#a) ) Params->BorderFlags |= wxsSizerExtraParams::b
        #define Begin() if (false)
        #define Match(a) else if ( Token == _T(#a) )
        #define PlaceH(a) Match(a) HorizPos = a;
        #define PlaceV(a) Match(a) VertPos = a;

        Begin();
        BFItem(wxLEFT,Left);
        BFItem(wxRIGHT,Right);
        BFItem(wxTOP,Top);
        BFItem(wxBOTTOM,Bottom);
        BFItem(wxNORTH,Top);
        BFItem(wxSOUTH,Bottom);
        BFItem(wxEAST,Right);
        BFItem(wxWEST,Left);
        Match(wxALL)
        {
            Params->BorderFlags |=
                wxsSizerExtraParams::Left |
                wxsSizerExtraParams::Right |
                wxsSizerExtraParams::Top |
                wxsSizerExtraParams::Bottom;
        }
        Match(wxGROW) Params->Expand = true;
        Match(wxEXPAND) Params->Expand = true;
        Match(wxSHAPED) Params->Shaped = true;
        Match(wxALIGN_CENTER) { HorizPos = wxALIGN_CENTER_HORIZONTAL; VertPos = wxALIGN_CENTER_VERTICAL; }
        Match(wxALIGN_CENTRE) { HorizPos = wxALIGN_CENTER_HORIZONTAL; VertPos = wxALIGN_CENTER_VERTICAL; }
        PlaceH(wxALIGN_RIGHT)
        PlaceV(wxALIGN_TOP)
        PlaceV(wxALIGN_BOTTOM)
        PlaceH(wxALIGN_CENTER_HORIZONTAL)
        PlaceH(wxALIGN_CENTRE_HORIZONTAL)
        PlaceV(wxALIGN_CENTER_VERTICAL)
        PlaceV(wxALIGN_CENTRE_VERTICAL)
    }

    if ( HorizPos==wxALIGN_RIGHT )
    {
        if ( VertPos==wxALIGN_BOTTOM ) Params->Placement = wxsSizerExtraParams::RightBottom;
        else if ( VertPos==wxALIGN_CENTER_VERTICAL ) Params->Placement = wxsSizerExtraParams::RightCenter;
        else Params->Placement = wxsSizerExtraParams::RightTop;
    }
    else if ( HorizPos==wxALIGN_CENTER_HORIZONTAL )
    {
        if ( VertPos==wxALIGN_BOTTOM ) Params->Placement = wxsSizerExtraParams::CenterBottom;
        else if ( VertPos==wxALIGN_CENTER_VERTICAL ) Params->Placement = wxsSizerExtraParams::Center;
        else Params->Placement = wxsSizerExtraParams::CenterTop;
    }
    else
    {
        if ( VertPos==wxALIGN_BOTTOM ) Params->Placement = wxsSizerExtraParams::LeftBottom;
        else if ( VertPos==wxALIGN_CENTER_VERTICAL ) Params->Placement = wxsSizerExtraParams::LeftCenter;
        else Params->Placement = wxsSizerExtraParams::LeftTop;
    }

    XmlAssignElement(Store);
    return true;
}

bool wxsDefSizer::SaveSizerStuff(wxsSizerExtraParams* Params,TiXmlElement* Elem)
{
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(Elem);

    if ( Params->Proportion ) XmlSetInteger(_T("option"),Params->Proportion);
    if ( Params->Border ) XmlSetInteger(_T("border"),Params->Border);

    wxString Flags = GetFlagToSizer(Params);
    if ( Flags.Len() )
    {
        XmlSetVariable(_T("flag"),Flags);
    }

    XmlAssignElement(Store);
    return true;
}


wxString wxsDefSizer::GetFlagToSizer(wxsSizerExtraParams* Params)
{
    wxString Flags = _T("");

    int BF = Params->BorderFlags;

    if ( ( BF & wxsSizerExtraParams::Left ) &&
         ( BF & wxsSizerExtraParams::Right ) &&
         ( BF & wxsSizerExtraParams::Top ) &&
         ( BF & wxsSizerExtraParams::Bottom ) )
    {
        Flags.Append(_T("|wxALL"));
    }
    else
    {
        if ( ( BF & wxsSizerExtraParams::Left   ) ) Flags.Append(_T("|wxLEFT"));
        if ( ( BF & wxsSizerExtraParams::Right  ) ) Flags.Append(_T("|wxRIGHT"));
        if ( ( BF & wxsSizerExtraParams::Top    ) ) Flags.Append(_T("|wxTOP"));
        if ( ( BF & wxsSizerExtraParams::Bottom ) ) Flags.Append(_T("|wxBOTTOM"));
    }

    switch ( Params->Placement )
    {
        case wxsSizerExtraParams::LeftTop:      Flags.Append(_T("|wxALIGN_LEFT|wxALIGN_TOP")); break;
        case wxsSizerExtraParams::CenterTop:    Flags.Append(_T("|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP")); break;
        case wxsSizerExtraParams::RightTop:     Flags.Append(_T("|wxALIGN_RIGHT|wxALIGN_TOP")); break;
        case wxsSizerExtraParams::LeftCenter:   Flags.Append(_T("|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL")); break;
        case wxsSizerExtraParams::Center:       Flags.Append(_T("|wxALIGN_CENTER")); break;
        case wxsSizerExtraParams::RightCenter:  Flags.Append(_T("|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL")); break;
        case wxsSizerExtraParams::LeftBottom:   Flags.Append(_T("|wxALIGN_LEFT|wxALIGN_BOTTOM")); break;
        case wxsSizerExtraParams::CenterBottom: Flags.Append(_T("|wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM")); break;
        case wxsSizerExtraParams::RightBottom:  Flags.Append(_T("|wxALIGN_RIGHT|wxALIGN_BOTTOM")); break;
    }

    if ( Params->Expand ) Flags.Append(_T("|wxEXPAND"));
    if ( Params->Shaped ) Flags.Append(_T("|wxSHAPED"));


    return Flags.Length() ? Flags.Mid(1) : _T("");
}

