#include "wxsheaders.h"
#include "widget.h"

#include "properties/wxsstyleproperty.h"
#include "properties/wxscolourproperty.h"
#include "properties/wxsfontproperty.h"
#include "resources/wxswindowres.h"
#include "wxswidgetfactory.h"
#include "wxsresource.h"
#include "wxswindoweditor.h"
#include "wxswidgetevents.h"
#include "wxsstandardqp.h"
#include <wx/tokenzr.h>
#include <wx/list.h>

#define COLOUR_ENTRY(Name) { _T(#Name), Name },

/** All system colours */
static struct { wxChar* Name; wxUint32 Value; } wxsSystemColours[] =
{
    COLOUR_ENTRY(wxSYS_COLOUR_SCROLLBAR)
    COLOUR_ENTRY(wxSYS_COLOUR_BACKGROUND)
    COLOUR_ENTRY(wxSYS_COLOUR_DESKTOP)
    COLOUR_ENTRY(wxSYS_COLOUR_ACTIVECAPTION)
    COLOUR_ENTRY(wxSYS_COLOUR_INACTIVECAPTION)
    COLOUR_ENTRY(wxSYS_COLOUR_MENU)
    COLOUR_ENTRY(wxSYS_COLOUR_WINDOW)
    COLOUR_ENTRY(wxSYS_COLOUR_WINDOWFRAME)
    COLOUR_ENTRY(wxSYS_COLOUR_MENUTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_WINDOWTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_CAPTIONTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_ACTIVEBORDER)
    COLOUR_ENTRY(wxSYS_COLOUR_INACTIVEBORDER)
    COLOUR_ENTRY(wxSYS_COLOUR_APPWORKSPACE)
    COLOUR_ENTRY(wxSYS_COLOUR_HIGHLIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_HIGHLIGHTTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_BTNFACE)
    COLOUR_ENTRY(wxSYS_COLOUR_3DFACE)
    COLOUR_ENTRY(wxSYS_COLOUR_BTNSHADOW)
    COLOUR_ENTRY(wxSYS_COLOUR_3DSHADOW)
    COLOUR_ENTRY(wxSYS_COLOUR_GRAYTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_BTNTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_INACTIVECAPTIONTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_BTNHIGHLIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_BTNHILIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_3DHIGHLIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_3DHILIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_3DDKSHADOW)
    COLOUR_ENTRY(wxSYS_COLOUR_3DLIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_INFOTEXT)
    COLOUR_ENTRY(wxSYS_COLOUR_INFOBK)
    COLOUR_ENTRY(wxSYS_COLOUR_LISTBOX)
    COLOUR_ENTRY(wxSYS_COLOUR_HOTLIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_GRADIENTACTIVECAPTION)
    COLOUR_ENTRY(wxSYS_COLOUR_GRADIENTINACTIVECAPTION)
    COLOUR_ENTRY(wxSYS_COLOUR_MENUHILIGHT)
    COLOUR_ENTRY(wxSYS_COLOUR_MENUBAR)
    { NULL, 0 }
};

/** Number of items in system colours array */
static const int wxsSystemColoursCount = sizeof(wxsSystemColours) / sizeof(wxsSystemColours[0]);

wxsWidget::wxsWidget(wxsWidgetManager* Man,wxsWindowRes* Res,wxsBasePropertiesType pType):
    PropertiesObject(this),
    Manager(Man),
    Preview(NULL),
    Resource(Res),
    Properties(NULL),
    Parent(NULL),
    MaxChildren(0),
    XmlElement(NULL),
    ContainerType(NoContainer),
    Updating(false),
    PropertiesCreated(false),
    AssignedToTree(false),
    Events(NULL)
{
    for ( int i=0; i<wxsREMCount; i++ )
    {
        BPTypes[i] = pType;
    }
}

wxsWidget::wxsWidget(wxsWidgetManager* Man, wxsWindowRes* Res, bool ISwxWindow, int MaxChild,wxsBasePropertiesType pType):
    PropertiesObject(this),
    Manager(Man),
    Preview(NULL),
    Resource(Res),
    Properties(NULL),
    Parent(NULL),
    MaxChildren(MaxChild),
    XmlElement(NULL),
    ContainerType(ISwxWindow ? ContainerWindow : ContainerSizer ),
    Updating(false),
    PropertiesCreated(false),
    AssignedToTree(false),
    Events(NULL)
{
    for ( int i=0; i<wxsREMCount; i++ )
    {
        BPTypes[i] = pType;
    }
}

wxsWidget::~wxsWidget()
{
	assert ( Preview == NULL );
	assert ( Properties == NULL );

    while ( GetChildCount() )
    {
        wxsKILL(GetChild(GetChildCount()-1));
    }

    if ( Events )
    {
    	delete Events;
    	Events = NULL;
    }
}

void wxsWidget::AddDefaultProperties(wxsBasePropertiesType pType)
{
    // Adding standard items

    if ( pType & bptVariable )
    {
        PropertiesObject.AddProperty(_("Var Name:"),GetBaseProperties().VarName);
        PropertiesObject.AddProperty(_("Is Member:"),GetBaseProperties().IsMember);
    }

    if ( pType & bptId )
    {
        PropertiesObject.AddProperty(_("Id:"),GetBaseProperties().IdName);
    }

    if ( pType & bptPosition )
    {
        PropertiesObject.Add2IProperty(_("Position:"),GetBaseProperties().PosX,GetBaseProperties().PosY);
        PropertiesObject.AddProperty(_(" Default:"),GetBaseProperties().DefaultPosition);
    }

    if ( pType & bptSize )
    {
        PropertiesObject.Add2IProperty(_("Size:"),GetBaseProperties().SizeX,GetBaseProperties().SizeY);
        PropertiesObject.AddProperty(_(" Default:"),GetBaseProperties().DefaultSize);
    }

    if ( pType & bptEnabled )
    {
    	PropertiesObject.AddProperty(_("Enabled:"),GetBaseProperties().Enabled);
    }

    if ( pType & bptFocused )
    {
    	PropertiesObject.AddProperty(_("Focused:"),GetBaseProperties().Focused);
    }

    if ( pType & bptHidden )
    {
    	PropertiesObject.AddProperty(_("Hidden:"),GetBaseProperties().Hidden);
    }

    if ( pType & bptColours )
    {
    	PropertiesObject.AddProperty(_("Foreground colour:"),
            new wxsColourProperty(&PropertiesObject,GetBaseProperties().FgType,GetBaseProperties().Fg) );
    	PropertiesObject.AddProperty(_("Background colour:"),
            new wxsColourProperty(&PropertiesObject,GetBaseProperties().BgType,GetBaseProperties().Bg) );
    }

    if ( pType & bptFont )
    {
    	PropertiesObject.AddProperty(_("Font:"),
            new wxsFontProperty(&PropertiesObject,GetBaseProperties().UseFont,GetBaseProperties().Font) );
    }

    if ( pType & bptToolTip )
    {
    	PropertiesObject.AddProperty(_("Tool tip:"),GetBaseProperties().ToolTip);
    }

    // Adding style property

    if ( pType & bptStyle )
    {
        if ( GetInfo().Styles != NULL )
        {
            PropertiesObject.AddProperty(
                _T("Style:"),
                new wxsStyleProperty(
                    &PropertiesObject,
                    GetBaseProperties().StyleBits,
                    GetBaseProperties().ExStyleBits,
                    GetInfo().Styles,
                     ( GetResource()->GetEditMode() == wxsREMMixed ) ||
                     ( GetResource()->GetEditMode() == wxsREMFile )
                    ),
                -1);
        }
    }

}

wxWindow* wxsWidget::CreatePreview(wxWindow* Parent,wxsWindowEditor* Editor)
{
    if ( Preview ) KillPreview();

    /* Creating widget */
    Preview = MyCreatePreview(Parent);
    if ( !Preview ) return NULL;

    /* Creating preview of child widgets */
    int Cnt = IsContainer() ? GetChildCount() : 0;
    for ( int i=0; i<Cnt; i++ )
    {
        GetChild(i)->CreatePreview(Preview,Editor);
    }

    MyFinalUpdatePreview(Preview);

    return Preview;
}

bool wxsWidget::PropertiesUpdated(bool Validate,bool Correct)
{
    if ( Updating ) return true;
    bool Result = true;
    Updating = true;
    if ( GetEditor() )
    {
    	GetEditor()->BuildPreview();
    }
    if ( Validate )
    {
    	Result = GetResource()->CheckBaseProperties(Correct,this);
    }
    GetResource()->NotifyChange();
    Updating = false;
    return Result;
}

void wxsWidget::KillPreview()
{
    /* Killing this one */
    if ( Preview != NULL )
    {
        delete Preview;
        PreviewDestroyed();
    }
}

void wxsWidget::PreviewApplyDefaults(wxWindow* Wnd)
{
	wxsBasePropertiesType pType = GetBPType();

	if ( (pType & bptStyle) )
	{
	    long ExStyle = GetExStyle();
	    if ( ExStyle != 0 )
	    {
            Wnd->SetExtraStyle(Wnd->GetExtraStyle() | ExStyle);
	    }
	}

	if ( (pType & bptEnabled) && !GetBaseProperties().Enabled )
	{
		Wnd->Disable();
	}

	if ( (pType & bptFocused) && GetBaseProperties().Focused )
	{
		Wnd->SetFocus();
	}

	if ( (pType & bptHidden) && GetBaseProperties().Hidden )
	{
		Wnd->Hide();
	}

	if ( pType & bptColours )
	{
		if ( GetBaseProperties().FgType != wxsNO_COLOUR )
		{
			if ( GetBaseProperties().FgType == wxsCUSTOM_COLOUR )
			{
				Wnd->SetForegroundColour(GetBaseProperties().Fg);
			}
			else
			{
				Wnd->SetForegroundColour(wxSystemSettings::GetColour((wxSystemColour)GetBaseProperties().FgType));
			}
		}

		if ( GetBaseProperties().BgType != wxsNO_COLOUR )
		{
			if ( GetBaseProperties().BgType == wxsCUSTOM_COLOUR )
			{
				Wnd->SetBackgroundColour(GetBaseProperties().Bg);
			}
			else
			{
				Wnd->SetBackgroundColour(wxSystemSettings::GetColour((wxSystemColour)GetBaseProperties().BgType));
			}
		}
	}

    if ( (pType & bptToolTip) && GetBaseProperties().ToolTip )
    {
    	Wnd->SetToolTip(GetBaseProperties().ToolTip);
    }

    if ( (pType & bptFont) && GetBaseProperties().UseFont )
    {
    	Wnd->SetFont(GetBaseProperties().Font);
    }
}

void wxsWidget::XmlAssignElement(TiXmlElement* Elem)
{
    XmlElement = Elem;
}

bool wxsWidget::XmlLoadDefaultsT(wxsBasePropertiesType pType)
{
    assert ( XmlElem() != NULL );
    // Loading event handler enteries
    GetEvents()->XmlLoadFunctions(XmlElem());

    /* Processing position */
    if ( pType & bptPosition )
    {
        GetBaseProperties().DefaultPosition = !XmlGetIntPair(_T("pos"),GetBaseProperties().PosX,GetBaseProperties().PosY);
    }

    /* Processing size */
    if ( pType & bptSize )
    {
        GetBaseProperties().DefaultSize = !XmlGetIntPair(_T("size"),GetBaseProperties().SizeX,GetBaseProperties().SizeY);
    }

    /* Processing id */
    if ( pType & bptId )
    {
        const char* IdName = XmlElem()->Attribute("name");
        GetBaseProperties().IdName = IdName ? wxString ( IdName, wxConvLocal ) : _T("");
    }

    /* Processing variable name and locality */
    if ( pType & bptVariable )
    {
        GetBaseProperties().VarName = _U(XmlElem()->Attribute("variable"));
        const char* IsMember = XmlElem()->Attribute("member");
        GetBaseProperties().IsMember = IsMember ? ( strcasecmp(IsMember,"no") != 0 ) : true;
    }

    /* Processing style */
    if ( pType & bptStyle )
    {
// TODO (SpOoN#1#): Clarify this code
        GetBaseProperties().StyleBits = 0;
        wxStringTokenizer Tkn(XmlGetVariable(_T("style")),_T("| \t\n"));

        while ( Tkn.HasMoreTokens() )
        {
            wxString Style = Tkn.GetNextToken();
            wxsStyle* St = GetInfo().Styles;
            int Bit = 1;
            if ( St )
            {
                for ( ; !St->Name.empty(); St++ )
                {
                    if ( St->IsCategory() ) continue;
                    if ( St->IsExtra() ) continue;

                    if ( Style == St->Name )
                    {
                        GetBaseProperties().StyleBits |= Bit;
                        break;
                    }
                    Bit <<= 1;
                }
            }
        }

        GetBaseProperties().ExStyleBits = 0;
        Tkn.SetString(XmlGetVariable(_T("exstyle")),_T("| \t\n"));

        while ( Tkn.HasMoreTokens() )
        {
            wxString Style = Tkn.GetNextToken();
            wxsStyle* St = GetInfo().Styles;
            int Bit = 1;
            if ( St )
            {
                for ( ; !St->Name.empty(); St++ )
                {
                    if ( St->IsCategory() ) continue;
                    if ( !St->IsExtra() ) continue;

                    if ( Style == St->Name )
                    {
                        GetBaseProperties().ExStyleBits |= Bit;
                        break;
                    }
                    Bit <<= 1;
                }
            }

        }
    }

    if ( pType & bptEnabled )
    {
    	GetBaseProperties().Enabled = XmlGetInteger(_T("enabled"),1) != 0;
    }

    if ( pType & bptFocused )
    {
    	GetBaseProperties().Focused = XmlGetInteger(_T("focused"),0) != 0;
    }

    if ( pType & bptHidden )
    {
    	GetBaseProperties().Hidden = XmlGetInteger(_T("hiddedn"),0) != 0;
    }

    if ( pType & bptColours )
    {
    	wxString Colour = XmlGetVariable(_T("fg"));
    	Colour.Trim(true).Trim(false);
        GetBaseProperties().FgType = wxsNO_COLOUR;
    	if ( !Colour.empty() )
    	{
    		if ( Colour[0] == _T('#') )
    		{
    			// Got web colour
    			long Value = 0;
    			if ( Colour.Mid(1).ToLong(&Value,0x10) )
    			{
    				GetBaseProperties().FgType = wxsCUSTOM_COLOUR;
    				GetBaseProperties().Fg = wxColour(
                        ( Value >> 16 ) & 0xFF,
                        ( Value >> 8 ) & 0xFF,
                        Value & 0xFF );
    			}
    		}
    		else
    		{
    			for ( int i=0; i<wxsSystemColoursCount; i++ )
    			{
    				if ( Colour == wxsSystemColours[i].Name )
    				{
    					GetBaseProperties().FgType = wxsSystemColours[i].Value;
    					GetBaseProperties().Fg = wxSystemSettings::GetColour((wxSystemColour)GetBaseProperties().FgType);
    					break;
    				}
    			}
    		}
    	}

    	Colour = XmlGetVariable(_T("bg"));
    	Colour.Trim(true).Trim(false);
        GetBaseProperties().BgType = wxsNO_COLOUR;
    	if ( !Colour.empty() )
    	{
    		if ( Colour[0] == _T('#') )
    		{
    			// Got web colour
    			long Value = 0;
    			if ( Colour.Mid(1).ToLong(&Value,0x10) )
    			{
    				GetBaseProperties().BgType = wxsCUSTOM_COLOUR;
    				GetBaseProperties().Bg = wxColour(
                        ( Value >> 16 ) & 0xFF,
                        ( Value >> 8 ) & 0xFF,
                        Value & 0xFF );
    			}
    		}
    		else
    		{
    			for ( int i=0; i<wxsSystemColoursCount; i++ )
    			{
    				if ( Colour == wxsSystemColours[i].Name )
    				{
    					GetBaseProperties().BgType = wxsSystemColours[i].Value;
    					GetBaseProperties().Bg = wxSystemSettings::GetColour((wxSystemColour)GetBaseProperties().BgType);
    					break;
    				}
    			}
    		}
    	}
    }

    if ( pType & bptFont )
    {
    	TiXmlElement* Store = XmlElem();
    	TiXmlElement* Font = Store->FirstChildElement("font");
    	if ( Font != NULL )
    	{
    		XmlAssignElement(Font);

    		// Loading font stuff

    		int Size = XmlGetInteger(_T("size"),wxDEFAULT);

    		wxString Str = XmlGetVariable(_T("style"));
    		int Style = wxFONTSTYLE_NORMAL;
    		if ( Str == _T("italic") ) Style = wxFONTSTYLE_ITALIC;
    		else if ( Str == _T("slant") ) Style = wxFONTSTYLE_SLANT ;

    		Str = XmlGetVariable(_T("weight"));
    		int Weight = wxNORMAL;
    		if ( Str == _T("bold") ) Weight = wxBOLD;
    		else if ( Str == _T("light") ) Weight = wxLIGHT;

    		bool Underlined = XmlGetInteger(_T("underlined"),0) != 0;

    		int Family = wxDEFAULT;
            Str = XmlGetVariable(_T("family"));
            if (Str == _T("decorative")) Family = wxDECORATIVE;
            else if (Str == _T("roman")) Family = wxROMAN;
            else if (Str == _T("script")) Family = wxSCRIPT;
            else if (Str == _T("swiss")) Family = wxSWISS;
            else if (Str == _T("modern")) Family = wxMODERN;
            else if (Str == _T("teletype")) Family = wxTELETYPE;

            wxString Face = XmlGetVariable(_T("face"));

    		GetBaseProperties().UseFont = true;
    		GetBaseProperties().Font = wxFont(Size,Family,Style,Weight,Underlined,Face);
    	}
    	else
    	{
    		GetBaseProperties().UseFont = false;
    		GetBaseProperties().Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    	}
    	XmlAssignElement(Store);
    }

    if ( IsContainer() )
    {
        if ( !XmlLoadChildren() ) return false;
    }

    return true;
}

bool wxsWidget::XmlSaveDefaultsT(wxsBasePropertiesType pType)
{
    assert ( XmlElem() != NULL );
    if ( GetResource()->GetEditMode() != wxsREMFile )
    {
        GetEvents()->XmlSaveFunctions(XmlElem());
    }

    if ( pType & bptPosition )
    {
        if ( !GetBaseProperties().DefaultPosition )
        {
            XmlSetIntPair(_T("pos"),GetBaseProperties().PosX,GetBaseProperties().PosY);
        }
    }

    if ( pType & bptSize )
    {
        if ( !GetBaseProperties().DefaultSize )
        {
            XmlSetIntPair(_T("size"),GetBaseProperties().SizeX,GetBaseProperties().SizeY);
        }
    }

    if ( pType & bptId )
    {
        XmlElem()->SetAttribute("name",GetBaseProperties().IdName.mb_str());
    }

    if ( pType & bptVariable )
    {
        XmlElem()->SetAttribute("variable",GetBaseProperties().VarName.mb_str());
        XmlElem()->SetAttribute("member",GetBaseProperties().IsMember?"yes":"no");
    }

    if ( pType & bptStyle )
    {
        wxString Style;
        wxString ExStyle;

        int StyleBits = GetBaseProperties().StyleBits;
        int StyleBit = 1;

        int ExStyleBits = GetBaseProperties().ExStyleBits;
        int ExStyleBit = 1;

        wxsStyle* St = GetInfo().Styles;

        if ( St )
        {
            for ( ; !St->Name.empty(); St++ )
            {
                if ( St->IsCategory() ) continue;

                if ( St->IsExtra() )
                {
                    if ( ExStyleBits & ExStyleBit )
                    {
                        if ( !ExStyle.empty() ) ExStyle.Append(_T('|'));
                        ExStyle.Append(St->Name);
                    }
                    ExStyleBit <<= 1;
                }
                else
                {
                    if ( StyleBits & StyleBit )
                    {
                        if ( !Style.empty() ) Style.Append(_T('|'));
                        Style.Append(St->Name);
                    }
                    StyleBit <<= 1;
                }
            }

            if ( !Style.empty()   ) XmlSetVariable(_T("style"),Style);
            if ( !ExStyle.empty() ) XmlSetVariable(_T("exstyle"),ExStyle);
        }
    }

    if ( pType & bptEnabled )
    {
    	if ( !GetBaseProperties().Enabled ) XmlSetInteger(_T("enabled"),0);
    }

    if ( pType & bptFocused )
    {
    	if ( GetBaseProperties().Focused ) XmlSetInteger(_T("focused"),1);
    }

    if ( pType & bptHidden )
    {
    	if ( GetBaseProperties().Hidden ) XmlSetInteger(_T("hiddedn"),1);
    }

    if ( pType & bptColours )
    {
    	if ( GetBaseProperties().FgType == wxsCUSTOM_COLOUR )
    	{
    		XmlSetVariable( _T("fg"),
                wxString::Format(_T("#%02X%02X%02X"),
                    GetBaseProperties().Fg.Red(),
                    GetBaseProperties().Fg.Green(),
                    GetBaseProperties().Fg.Blue() ) );
    	}
    	else
    	{
    		for ( int i=0; i<wxsSystemColoursCount; i++ )
    		{
    			if ( GetBaseProperties().FgType == wxsSystemColours[i].Value )
    			{
    				XmlSetVariable( _T("fg"), wxsSystemColours[i].Name );
    				break;
    			}
    		}
    	}

    	if ( GetBaseProperties().BgType == wxsCUSTOM_COLOUR )
    	{
    		XmlSetVariable( _T("bg"),
                wxString::Format(_T("#%02X%02X%02X"),
                    GetBaseProperties().Bg.Red(),
                    GetBaseProperties().Bg.Green(),
                    GetBaseProperties().Bg.Blue() ) );
    	}
    	else
    	{
    		for ( int i=0; i<wxsSystemColoursCount; i++ )
    		{
    			if ( GetBaseProperties().BgType == wxsSystemColours[i].Value )
    			{
    				XmlSetVariable( _T("bg"), wxsSystemColours[i].Name );
    				break;
    			}
    		}
    	}
    }

    if ( pType & bptFont && GetBaseProperties().UseFont )
    {
    	TiXmlElement* Store = XmlElem();
    	XmlAssignElement(Store->InsertEndChild(TiXmlElement("font"))->ToElement());
    	wxFont& Font = GetBaseProperties().Font;
    	XmlSetInteger(_T("size"),Font.GetPointSize());

    	switch ( Font.GetStyle() )
    	{
            case wxFONTSTYLE_ITALIC: XmlSetVariable(_T("style"),_T("italic")); break;
            case wxFONTSTYLE_SLANT : XmlSetVariable(_T("style"),_T("slant")); break;
            default:;
    	}

    	switch ( Font.GetWeight() )
    	{
    		case wxFONTWEIGHT_LIGHT: XmlSetVariable(_T("weight"),_T("light")); break;
            case wxFONTWEIGHT_BOLD : XmlSetVariable(_T("weight"),_T("bold")); break;
            default:;
    	}

    	switch ( Font.GetFamily() )
    	{
    		case wxFONTFAMILY_DECORATIVE: XmlSetVariable(_T("family"),_T("decorative")); break;
    		case wxFONTFAMILY_ROMAN     : XmlSetVariable(_T("family"),_T("roman")); break;
    		case wxFONTFAMILY_SCRIPT    : XmlSetVariable(_T("family"),_T("script")); break;
    		case wxFONTFAMILY_SWISS     : XmlSetVariable(_T("family"),_T("swiss")); break;
    		case wxFONTFAMILY_MODERN    : XmlSetVariable(_T("family"),_T("modern")); break;
    		case wxFONTFAMILY_TELETYPE  : XmlSetVariable(_T("family"),_T("teletype")); break;
    		default:;
    	}
    	if ( Font.GetUnderlined() ) XmlSetInteger(_T("underlined"),1);
    	if ( !Font.GetFaceName().empty() ) XmlSetVariable(_T("face"),Font.GetFaceName());
    	XmlAssignElement(Store);
    }

    if ( IsContainer() )
    {
        if ( !XmlSaveChildren() ) return false;
    }

    return true;
}

wxString wxsWidget::XmlGetVariable(const wxString& name)
{
    assert ( XmlElem() != NULL );

    if ( !name || !*name ) return _T("");
    TiXmlElement* Elem = XmlElem()->FirstChildElement(name.mb_str());
    if ( !Elem ) return _T("");

    TiXmlNode* Node = Elem->FirstChild();
    while ( Node )
    {
        TiXmlText* Text = Node->ToText();
        if ( Text )
        {
            return _U(Text->Value());
        }
        Node = Node->NextSibling();
    }

    return _T("");
}

int wxsWidget::XmlGetInteger(const wxString& name,bool& IsInvalid,int DefaultValue)
{
    wxString Tmp = XmlGetVariable(name);
    long value;
    if ( !Tmp.Length() || !Tmp.ToLong(&value) )
    {
        IsInvalid = true;
        return DefaultValue;
    }
    IsInvalid = false;
    return (int)value;
}

bool wxsWidget::XmlGetIntPair(const wxString& Name,int& P1,int& P2,int DefP1,int DefP2)
{
    long _P1, _P2;
    wxString Tmp = XmlGetVariable(Name);
    if ( Tmp.Length() &&
         Tmp.BeforeFirst(_T(',')).ToLong(&_P1) &&
         Tmp.AfterLast(_T(',')).ToLong(&_P2) )
    {
        P1 = (int)_P1;
        P2 = (int)_P2;
        return true;
    }

    P1 = DefP1;
    P2 = DefP2;
    return false;
}

bool wxsWidget::XmlSetVariable(const wxString& Name,const wxString& Value)
{
    assert ( XmlElem() != NULL );
    TiXmlNode * NewNode = XmlElem()->InsertEndChild(TiXmlElement(Name.mb_str()));
    if ( NewNode )
    {
        NewNode->InsertEndChild(TiXmlText(Value.mb_str()));
        return true;
    }
    return false;
}

bool wxsWidget::XmlSetInteger(const wxString& Name,int Value)
{
    return XmlSetVariable(Name,wxString::Format(_T("%d"),Value));
}

bool wxsWidget::XmlSetIntPair(const wxString& Name,int Val1,int Val2)
{
    return XmlSetVariable(Name,wxString::Format(_T("%d,%d"),Val1,Val2));
}

bool wxsWidget::XmlLoadChildren()
{
    assert ( XmlElem() != NULL );

    bool Ret = true;

    for ( TiXmlElement* Element = XmlElem()->FirstChildElement();
          Element != NULL;
          Element = Element->NextSiblingElement() )
    {
    	if ( !XmlLoadChild(Element) )
    	{
    		Ret = false;
    	}
    }

    return Ret;
}

bool wxsWidget::XmlLoadChild(TiXmlElement* Element)
{
	// Processing <object> elements only
    if ( strcmp(Element->Value(),"object") ) return true;


    const char* Name = Element->Attribute("class");

    bool Ret = true;

    if ( Name && *Name )
    {
        wxsWidget* Child = wxsGEN(_U(Name),GetResource());
        if ( !Child )
        {
            return false;
        }

        if ( !Child->XmlLoad(Element) ) Ret = false;
        if ( AddChild(Child) < 0 )
        {
            delete Child;
            return false;
        }
    }
    return Ret;
}

bool wxsWidget::XmlSaveChildren()
{
    bool Ret = true;
    TiXmlElement* Elem = XmlElem();
    int Count = GetChildCount();
    for ( int i=0; i<Count; i++ )
    {
    	if ( !XmlSaveChild(i,Elem) ) Ret = false;
    }
    return Ret;
}

bool wxsWidget::XmlSaveChild(int ChildIndex,TiXmlElement* AddHere)
{
    wxsWidget* W = GetChild(ChildIndex);

    if ( W && W->GetInfo().Name )
    {
        TiXmlElement* SaveTo = AddHere->InsertEndChild(TiXmlElement("object"))->ToElement();

        if ( !SaveTo ) return false;

        SaveTo->SetAttribute("class",W->GetInfo().Name.mb_str());
        if ( !W->XmlSave(SaveTo) ) return false;
    }

    return true;
}

const wxsWidget::CodeDefines& wxsWidget::GetCodeDefines()
{
    CDefines.Style = _T("");
    CDefines.InitCode = _T("");

    wxString SelectCode = _T("");
    if ( GetParent() )
    {
        SelectCode << GetBaseProperties().VarName << _T("->");
    }

    // Filling up styles

    wxString StyleStr;
    wxString ExStyleStr;

    CDefines.Style = _T("0");
    wxsStyle* Style = GetInfo().Styles;
    if ( Style )
    {
        int StyleBits = GetBaseProperties().StyleBits;
        int StyleBit = 1;

        int ExStyleBits = GetBaseProperties().ExStyleBits;
        int ExStyleBit = 1;

        for ( ; Style->Name; Style++ )
        {
            if ( Style->IsCategory() ) continue;

            if ( Style->IsExtra() )
            {
                if ( ExStyleBits & ExStyleBit )
                {
                    if ( !ExStyleStr.empty() ) ExStyleStr.Append(_T('|'));
                    ExStyleStr.Append(Style->Name);
                }
                ExStyleBit<<=1;
            }
            else
            {
                if ( StyleBits & StyleBit )
                {
                    if ( !StyleStr.empty() ) StyleStr.Append(_T('|'));
                    StyleStr.Append(Style->Name);
                }
                StyleBit<<=1;
            }

        }
    }
    if ( !StyleStr.empty() ) CDefines.Style = StyleStr;
    if ( !ExStyleStr.empty() )
    {
        CDefines.InitCode << SelectCode
                          << _T("SetExtraStyle(")
                          << SelectCode
                          << _T("GetExtraStyle() | ")
                          << ExStyleStr
                          << _T(");\n");
    }

    // Creating position

    if ( GetBaseProperties().DefaultPosition ) CDefines.Pos = _T("wxDefaultPosition");
    else CDefines.Pos.Printf(_T("wxPoint(%d,%d)"),GetBaseProperties().PosX,GetBaseProperties().PosY);

    // Creating size

    if ( GetBaseProperties().DefaultSize ) CDefines.Size = _T("wxDefaultSize");
    else CDefines.Size.Printf(_T("wxSize(%d,%d)"),GetBaseProperties().SizeX,GetBaseProperties().SizeY);

    // Creating colours

    wxsBasePropertiesType pType = GetBPType();

    if ( pType & bptColours )
    {
        wxString FColour;
        wxString BColour;

        if ( GetBaseProperties().FgType == wxsCUSTOM_COLOUR )
        {
        	FColour.Printf(_T("wxColour(%d,%d,%d)"),
                GetBaseProperties().Fg.Red(),
                GetBaseProperties().Fg.Green(),
                GetBaseProperties().Fg.Blue());
        }
        else
        {
        	for ( int i=0; i<wxsSystemColoursCount; i++ )
        	{
        		if ( GetBaseProperties().FgType == wxsSystemColours[i].Value )
        		{
        			FColour.Printf(_T("wxSystemSettings::GetColour(%s)"),wxsSystemColours[i].Name);
        			break;
        		}
        	}
        }

        if ( GetBaseProperties().BgType == wxsCUSTOM_COLOUR )
        {
        	BColour.Printf(_T("wxColour(%d,%d,%d)"),
                GetBaseProperties().Bg.Red(),
                GetBaseProperties().Bg.Green(),
                GetBaseProperties().Bg.Blue());
        }
        else
        {
        	for ( int i=0; i<wxsSystemColoursCount; i++ )
        	{
        		if ( GetBaseProperties().BgType == wxsSystemColours[i].Value )
        		{
        			BColour.Printf(_T("wxSystemSettings::GetColour(%s)"),wxsSystemColours[i].Name);
        			break;
        		}
        	}
        }


        if ( !FColour.empty() )
        {
            CDefines.InitCode << SelectCode << _T("SetForegroundColour(")
                              << FColour    << _T(");");
        }

        if ( !BColour.empty() )
        {
            CDefines.InitCode << SelectCode << _T("SetBackgroundColour(")
                              << BColour    << _T(");");
        }
    }

    if ( pType & bptFont && GetBaseProperties().UseFont )
    {
    	wxFont& Font = GetBaseProperties().Font;
    	CDefines.InitCode.Append(
            wxString::Format(_T("%sSetFont(wxFont(%d,"),
                SelectCode.c_str(),Font.GetPointSize()));

    	switch ( Font.GetFamily() )
    	{
    		case wxFONTFAMILY_DECORATIVE: CDefines.InitCode << _T("wxFONTFAMILY_DECORATIVE,"); break;
    		case wxFONTFAMILY_ROMAN     : CDefines.InitCode << _T("wxFONTFAMILY_ROMAN,"); break;
    		case wxFONTFAMILY_SCRIPT    : CDefines.InitCode << _T("wxFONTFAMILY_SCRIPT,"); break;
    		case wxFONTFAMILY_SWISS     : CDefines.InitCode << _T("wxFONTFAMILY_SWISS,"); break;
    		case wxFONTFAMILY_MODERN    : CDefines.InitCode << _T("wxFONTFAMILY_MODERN,"); break;
    		case wxFONTFAMILY_TELETYPE  : CDefines.InitCode << _T("wxFONTFAMILY_TELETYPE,"); break;
    		default                     : CDefines.InitCode << _T("wxFONTFAMILY_DEFAULT,");
    	}

    	switch ( Font.GetStyle() )
        {
    		case wxFONTSTYLE_SLANT  : CDefines.InitCode << _T("wxFONTSTYLE_SLANT,"); break;
    		case wxFONTSTYLE_ITALIC : CDefines.InitCode << _T("wxFONTSTYLE_ITALIC,"); break;
    		default                 : CDefines.InitCode << _T("wxFONTSTYLE_NORMAL,");
    	}

        switch ( Font.GetWeight() )
        {
        	case wxFONTWEIGHT_BOLD : CDefines.InitCode << _T("wxFONTWEIGHT_BOLD,"); break;
        	case wxFONTWEIGHT_LIGHT: CDefines.InitCode << _T("wxFONTWEIGHT_LIGHT,"); break;
        	default                : CDefines.InitCode << _T("wxFONTWEIGHT_NORMAL,");
        }

        if ( Font.GetUnderlined() )
        {
        	CDefines.InitCode << _T("true,");
        }
        else
        {
        	CDefines.InitCode << _T("false,");
        }

        if ( Font.GetFaceName().empty() )
        {
        	CDefines.InitCode << _T("_T(\"\")");
        }
        else
        {
            CDefines.InitCode << GetWxString(Font.GetFaceName());
        }

        CDefines.InitCode << _T("));");
    }

    if ( pType & bptEnabled && !GetBaseProperties().Enabled )
    {
    	CDefines.InitCode << SelectCode << _T("Disable();");
    }

    if ( pType & bptFocused && GetBaseProperties().Focused )
    {
        CDefines.InitCode << SelectCode << _T("SetFocus();");
    }

    if ( pType & bptHidden && GetBaseProperties().Hidden )
    {
    	CDefines.InitCode << SelectCode << _T("Hide();");
    }

    if ( pType & bptToolTip && !GetBaseProperties().ToolTip.empty() )
    {
    	CDefines.InitCode << SelectCode << _T("SetToolTip(")
    	                  << GetWxString(GetBaseProperties().ToolTip) << _T(");");
    }

    return CDefines;
}

void wxsWidget::BuildTree(wxTreeCtrl* Tree,wxTreeItemId Id,int Index)
{
    wxString Name = GetInfo().Name;

    // TODO (SpOoN#1#): Add icons
    wxTreeItemId SubId;
    if ( Index < 0 || Index >= (int)Tree->GetCount() )
    {
        SubId = Tree->AppendItem(Id,Name,-1,-1,new wxsResourceTreeData(this));
    }
    else
    {
        SubId = Tree->InsertItem(Id,Index,Name,-1,-1,new wxsResourceTreeData(this));
    }
    TreeId = SubId;
    AssignedToTree = true;

    int SubCnt = GetChildCount();
    for ( int i=0; i<SubCnt; i++ )
    {
        GetChild(i)->BuildTree(Tree,SubId);
    }
}

//Added by cyberkoa
/* ======================================================================
    Function Name : XmlSetStringArray
   ======================================================================

 Description : To write a series of strings from XRC with ParentName as
               parent element and ChildName as child element
  Example :  ParentName = "content" , ChildName="item"
              <content>
                   <item>Item 1</item>
                   <item>Option 2</item>
                   <item>3rd choice</item>
              </content>

*/

bool wxsWidget::XmlSetStringArray(const wxString& ParentName,const wxString& ChildName,wxArrayString& stringArray)
{
    assert ( XmlElem() != NULL );

    int Count = stringArray.GetCount();
    // No item, return without writing <content> and <item> elements
    if(Count==0) return false;

    // Adding <ParentName>  element
    TiXmlElement* ParentElement;
    TiXmlElement* ChildElement;
    ParentElement = XmlElem()->InsertEndChild(TiXmlElement(ParentName.mb_str()))->ToElement();

    for ( int i=0; i<Count; i++ )
    {
      ChildElement = ParentElement->InsertEndChild(TiXmlElement(ChildName.mb_str()))->ToElement();

      if (ChildElement)
        ChildElement->InsertEndChild(TiXmlText(stringArray[i].mb_str()));
    }
    return true;
}
//End added

//Added by cyberkoa
/* ======================================================================
    Function Name : XmlGetStringArray
   ======================================================================

 Description : To read a series of strings from XRC with ParentName as
               parent element and ChildName as child element
  Example :  ParentName = "content" , ChildName="item"
              <content>
                   <item>Item 1</item>
                   <item>Option 2</item>
                   <item>3rd choice</item>
              </content>

*/
bool wxsWidget::XmlGetStringArray(const wxString& ParentName,const wxString& ChildName,wxArrayString& stringArray)
{
    assert ( XmlElem() != NULL );

     // Empty it to make sure element added in an empty wxArrayString
      stringArray.Empty();

    TiXmlElement* ParentElement = XmlElem()->FirstChildElement(ParentName.mb_str());
    if(!ParentElement) return false;

    for (TiXmlElement* ChildElement= ParentElement->FirstChildElement(ChildName.mb_str());
          ChildElement != NULL;
          ChildElement = ChildElement->NextSiblingElement(ChildName.mb_str()))
          {

           TiXmlNode* Node = ChildElement->FirstChild();

			while(Node)
			{
			 if(Node->ToText())
              stringArray.Add(_U(Node->ToText()->Value()));

              Node=Node->NextSibling();
            }
          }

      if(stringArray.GetCount() > 0)return true; else return false;

}
//End added

void wxsWidget::KillTree(wxTreeCtrl* Tree)
{
	if ( AssignedToTree )
	{
		Tree->Delete(TreeId);
        InvalidateTreeIds();
	}
}

void wxsWidget::PreviewDestroyed()
{
    int Cnt = GetChildCount();
    for ( int i=0; i<Cnt; i++ )
    {
        GetChild(i)->PreviewDestroyed();
    }
    Preview = NULL;
}

void wxsWidget::InvalidateTreeIds()
{
	if ( !AssignedToTree ) return;
	AssignedToTree = false;
	int Cnt = GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		GetChild(i)->InvalidateTreeIds();
	}
}

wxsWindowEditor* wxsWidget::GetEditor()
{
	return Resource ? (wxsWindowEditor*)Resource->GetEditor() : NULL;
}

wxsWidgetEvents* wxsWidget::GetEvents()
{
	if ( !Events )
	{
		 Events = new wxsWidgetEvents(this);
		 // Filling up with events from info
		 wxsEventDesc* Desc = GetInfo().Events;
		 if ( Desc )
		 {
		 	while ( Desc->EventTypeName.Length() )
		 	{
		 		if ( Desc->EventEntry.Length() )
		 		{
		 			Events->AddEvent(*Desc,true);
		 		}
		 		Desc++;
		 	}
		 }
	}
	return Events;
}
wxsBasePropertiesType wxsWidget::GetBPType()
{
    if ( !GetResource() ) return wxsREMFile;
    int REM = GetResource()->GetEditMode();
    if ( ( REM < 0 ) || ( REM >= wxsREMCount ) ) return wxsREMFile;
    return BPTypes[REM];
}

void wxsWidget::ChangeBPT(int REM,wxsBasePropertiesType pType)
{
    if ( REM < 0 ) return;
    if ( REM >= wxsREMCount ) return;
    BPTypes[REM] = pType;
}

wxWindow* wxsWidget::BuildQuickPanel(wxWindow* Parent)
{
    if ( !( GetBPType() & (bptEnabled|bptHidden|bptFocused|bptId|bptVariable)) ) return NULL;
    return new wxsStandardQP(Parent,this);
}

long wxsWidget::GetStyle()
{
    long Value = 0;
    wxsStyle* St = GetInfo().Styles;
    if ( St )
    {
        int StyleBits = GetBaseProperties().StyleBits;
        int StyleBit = 1;
        for ( ; !St->Name.empty(); St++ )
        {
            if ( St->IsCategory() ) continue;
            if ( St->IsExtra() ) continue;

            if ( StyleBits & StyleBit )
            {
                // Bit is set, using this style
                Value |= St->Value;
            }
            StyleBit <<= 1;
        }
    }
    return Value;
}

long wxsWidget::GetExStyle()
{
    long Value = 0;
    wxsStyle* St = GetInfo().Styles;
    if ( St )
    {
        int StyleBits = GetBaseProperties().ExStyleBits;
        int StyleBit = 1;
        for ( ; !St->Name.empty(); St++ )
        {
            if ( St->IsCategory() ) continue;
            if ( !St->IsExtra() ) continue;

            if ( StyleBits & StyleBit )
            {
                // Bit is set, using this style
                Value |= St->Value;
            }
            StyleBit <<= 1;
        }
    }
    return Value;
}

void wxsWidget::SetStyle(long Style)
{
    wxsStyle* St = GetInfo().Styles;
    if ( St )
    {
        int StyleBits = 0;
        int StyleBit = 1;
        for ( ; !St->Name.empty(); St++ )
        {
            if ( St->IsCategory() ) continue;
            if ( St->IsExtra() ) continue;

            if ( (long)St->Value == Style )
            {
                // Bit is set, using this style
                StyleBits |= StyleBit;
            }
            StyleBit <<= 1;
        }
        GetBaseProperties().StyleBits = StyleBits;
    }
}

void wxsWidget::SetExStyle(long ExStyle)
{
    wxsStyle* St = GetInfo().Styles;
    if ( St )
    {
        int ExStyleBits = 0;
        int ExStyleBit = 1;
        for ( ; !St->Name.empty(); St++ )
        {
            if ( St->IsCategory() ) continue;
            if ( !St->IsExtra() ) continue;

            if ( (long)St->Value == ExStyle )
            {
                // Bit is set, using this style
                ExStyleBits |= ExStyleBit;
            }
            ExStyleBit <<= 1;
        }
        GetBaseProperties().ExStyleBits = ExStyleBits;
    }
}
