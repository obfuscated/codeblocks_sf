#include "widget.h"

#include "properties/wxsstyleproperty.h"
#include "properties/wxscolourproperty.h"
#include "properties/wxsfontproperty.h"
#include "resources/wxswindowres.h"
#include "wxswidgetfactory.h"
#include "wxsresource.h"
#include "wxswindoweditor.h"
#include "wxswidgetevents.h"
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

wxsWidget::wxsWidget(wxsWidgetManager* Man,wxsWindowRes* Res,BasePropertiesType pType):
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
    BPType(pType),
    AssignedToTree(false),
    Events(NULL)
{
}

wxsWidget::wxsWidget(wxsWidgetManager* Man, wxsWindowRes* Res, bool ISwxWindow, int MaxChild,BasePropertiesType pType):
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
    BPType(pType),
    AssignedToTree(false),
    Events(NULL)
{
}

wxsWidget::~wxsWidget()
{
	assert ( Preview == NULL );
	assert ( Properties == NULL );

    while ( GetChildCount() )
    {
        wxsWidgetFactory::Get()->Kill(GetChild(GetChildCount()-1));
    }

    if ( Events )
    {
    	delete Events;
    	Events = NULL;
    }
}

void wxsWidget::AddDefaultProperties(BasePropertiesType pType)
{
    // Adding standard items

    if ( pType & bptVariable )
    {
        PropertiesObject.AddProperty(_("Var Name:"),BaseParams.VarName);
        PropertiesObject.AddProperty(_("Is Member:"),BaseParams.IsMember);
    }

    if ( pType & bptId )
    {
        PropertiesObject.AddProperty(_("Id:"),BaseParams.IdName);
    }

    if ( pType & bptPosition )
    {
        PropertiesObject.Add2IProperty(_("Position:"),BaseParams.PosX,BaseParams.PosY);
        PropertiesObject.AddProperty(_(" Default:"),BaseParams.DefaultPosition);
    }

    if ( pType & bptSize )
    {
        PropertiesObject.Add2IProperty(_("Size:"),BaseParams.SizeX,BaseParams.SizeY);
        PropertiesObject.AddProperty(_(" Default:"),BaseParams.DefaultSize);
    }
    
    if ( pType & bptEnabled )
    {
    	PropertiesObject.AddProperty(_("Enabled:"),BaseParams.Enabled);
    }
    
    if ( pType & bptFocused )
    {
    	PropertiesObject.AddProperty(_("Focused:"),BaseParams.Focused);
    }
    
    if ( pType & bptHidden )
    {
    	PropertiesObject.AddProperty(_("Hidden:"),BaseParams.Hidden);
    }
    
    if ( pType & bptColours )
    {
    	PropertiesObject.AddProperty(_("Foreground colour:"),
            new wxsColourProperty(&PropertiesObject,BaseParams.FgType,BaseParams.Fg) );
    	PropertiesObject.AddProperty(_("Background colour:"),
            new wxsColourProperty(&PropertiesObject,BaseParams.BgType,BaseParams.Bg) );
    }
    
    if ( pType & bptFont )
    {
    	PropertiesObject.AddProperty(_("Font:"),
            new wxsFontProperty(&PropertiesObject,BaseParams.UseFont,BaseParams.Font) );
    }
    
    if ( pType & bptToolTip )
    {
    	PropertiesObject.AddProperty(_("Tool tip:"),BaseParams.ToolTip);
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
                    BaseParams.Style,
                    GetInfo().Styles),
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
    	GetEditor()->RecreatePreview();
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
    /* Killing child windows */
    int Cnt = GetChildCount();
    for ( int i=0; i<Cnt; i++ )
    {
        GetChild(i)->KillPreview();
    }

    /* Killing this one */
    if ( Preview != NULL )
    {
        MyDeletePreview(Preview);
        Preview = NULL;
    }
}

void wxsWidget::PreviewApplyDefaults(wxWindow* Wnd)
{
	BasePropertiesType pType = GetBPType();
	if ( pType & bptEnabled && !BaseParams.Enabled )
	{
		Wnd->Disable();
	}
	
	if ( pType & bptFocused && BaseParams.Focused )
	{
		Wnd->SetFocus();
	}
	
	if ( pType & bptHidden && BaseParams.Hidden )
	{
		Wnd->Hide();
	}
	
	if ( pType & bptColours )
	{
		if ( BaseParams.FgType != wxsNO_COLOUR )
		{
			if ( BaseParams.FgType == wxsCUSTOM_COLOUR )
			{
				Wnd->SetForegroundColour(BaseParams.Fg);
			}
			else
			{
				Wnd->SetForegroundColour(wxSystemSettings::GetColour((wxSystemColour)BaseParams.FgType));
			}
		}
		
		if ( BaseParams.BgType != wxsNO_COLOUR )
		{
			if ( BaseParams.BgType == wxsCUSTOM_COLOUR )
			{
				Wnd->SetBackgroundColour(BaseParams.Bg);
			}
			else
			{
				Wnd->SetBackgroundColour(wxSystemSettings::GetColour((wxSystemColour)BaseParams.BgType));
			}
		}
	}

    if ( pType & bptToolTip && BaseParams.ToolTip )
    {
    	Wnd->SetToolTip(BaseParams.ToolTip);
    }

    if ( pType & bptFont && BaseParams.UseFont )
    {
    	Wnd->SetFont(BaseParams.Font);
    }

}

void wxsWidget::XmlAssignElement(TiXmlElement* Elem)
{
    XmlElement = Elem;
}

bool wxsWidget::XmlLoadDefaultsT(BasePropertiesType pType)
{
    assert ( XmlElem() != NULL );
    // Loading event handler enteries
    GetEvents()->XmlLoadFunctions(XmlElem());

    /* Processing position */
    if ( pType & bptPosition )
    {
        BaseParams.DefaultPosition = !XmlGetIntPair(_T("pos"),BaseParams.PosX,BaseParams.PosY);
    }

    /* Processing size */
    if ( pType & bptSize )
    {
        BaseParams.DefaultSize = !XmlGetIntPair(_T("size"),BaseParams.SizeX,BaseParams.SizeY);
    }

    /* Processing id */
    if ( pType & bptId )
    {
        const char* IdName = XmlElem()->Attribute("name");
        BaseParams.IdName = IdName ? wxString ( IdName, wxConvLocal ) : _T("");
    }

    /* Processing variable name and locality */
    if ( pType & bptVariable )
    {
        const char* VarName = XmlElem()->Attribute("variable");
        BaseParams.VarName = VarName ? wxString ( VarName, wxConvUTF8 ) : _T("");
        const char* IsMember = XmlElem()->Attribute("member");
        BaseParams.IsMember = IsMember ? ( strcasecmp(IsMember,"no") != 0 ) : true;
    }

    /* Processing style */
    if ( pType & bptStyle )
    {
        wxString StyleStr = XmlGetVariable(_T("style"));
        wxsStyle* Styles = GetInfo().Styles;
        BaseParams.Style = 0;
        if ( Styles )
        {
            for ( ; Styles->Name != _T(""); Styles++ )
            {
                int Pos = StyleStr.Find(Styles->Name);
                int Len = (int)Styles->Name.Length();
                if ( Pos < 0 ) continue;

                // One character after style in StyleStr should be checked - it
                // must be '|'. Same for character before.

                if ( ( Pos + Len >= (int)StyleStr.Len() || StyleStr.GetChar(Pos+Len) == _T('|') ) &&
                     ( Pos == 0 || StyleStr.GetChar(Pos-1) == _T('|') ) )
                {
                    BaseParams.Style |= Styles->Value;
                }
            }
        }
    }

    if ( pType & bptEnabled )
    {
    	BaseParams.Enabled = XmlGetInteger(_T("enabled"),1) != 0;
    }
    
    if ( pType & bptFocused )
    {
    	BaseParams.Focused = XmlGetInteger(_T("focused"),0) != 0;
    }
    
    if ( pType & bptHidden )
    {
    	BaseParams.Hidden = XmlGetInteger(_T("hiddedn"),0) != 0;
    }
    
    if ( pType & bptColours )
    {
    	wxString Colour = XmlGetVariable(_T("fg"));
    	Colour.Trim(true).Trim(false);
        BaseParams.FgType = wxsNO_COLOUR;
    	if ( !Colour.empty() )
    	{
    		if ( Colour[0] == _T('#') )
    		{
    			// Got web colour
    			long Value = 0;
    			if ( Colour.Mid(1).ToLong(&Value,0x10) )
    			{
    				BaseParams.FgType = wxsCUSTOM_COLOUR;
    				BaseParams.Fg = wxColour(
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
    					BaseParams.FgType = wxsSystemColours[i].Value;
    					BaseParams.Fg = wxSystemSettings::GetColour((wxSystemColour)BaseParams.FgType);
    					break;
    				}
    			}
    		}
    	}
    	
    	Colour = XmlGetVariable(_T("bg"));
    	Colour.Trim(true).Trim(false);
        BaseParams.BgType = wxsNO_COLOUR;
    	if ( !Colour.empty() )
    	{
    		if ( Colour[0] == _T('#') )
    		{
    			// Got web colour
    			long Value = 0;
    			if ( Colour.Mid(1).ToLong(&Value,0x10) )
    			{
    				BaseParams.BgType = wxsCUSTOM_COLOUR;
    				BaseParams.Bg = wxColour(
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
    					BaseParams.BgType = wxsSystemColours[i].Value;
    					BaseParams.Bg = wxSystemSettings::GetColour((wxSystemColour)BaseParams.BgType);
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
    		
    		BaseParams.UseFont = true;
    		BaseParams.Font = wxFont(Size,Family,Style,Weight,Underlined,Face);
    	}
    	else
    	{
    		BaseParams.UseFont = false;
    		BaseParams.Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    	}
    	XmlAssignElement(Store);
    }
    
    if ( IsContainer() )
    {
        if ( !XmlLoadChildren() ) return false;
    }

    return true;
}

bool wxsWidget::XmlSaveDefaultsT(BasePropertiesType pType)
{
    assert ( XmlElem() != NULL );
    if ( GetResource()->GetEditMode() != wxsResFile )
    {
        GetEvents()->XmlSaveFunctions(XmlElem());
    }

    if ( pType & bptPosition )
    {
        if ( !BaseParams.DefaultPosition )
        {
            XmlSetIntPair(_T("pos"),BaseParams.PosX,BaseParams.PosY);
        }
    }

    if ( pType & bptSize )
    {
        if ( !BaseParams.DefaultSize )
        {
            XmlSetIntPair(_T("size"),BaseParams.SizeX,BaseParams.SizeY);
        }
    }

    if ( pType & bptId )
    {
        XmlElem()->SetAttribute("name",BaseParams.IdName.mb_str());
    }

    if ( pType & bptVariable )
    {
        XmlElem()->SetAttribute("variable",BaseParams.VarName.mb_str());
        XmlElem()->SetAttribute("member",BaseParams.IsMember?"yes":"no");
    }

    if ( pType & bptStyle )
    {
        int StyleBits = BaseParams.Style;

        wxString StyleString;

        // Warning: This may cause some data los if styles are not
        //          configured properly

        wxsStyle* Style = GetInfo().Styles;
        if ( Style )
        {
            for ( ; Style->Name; Style++ )
            {
                if ( ( Style->Value != 0 ) && ( ( StyleBits & Style->Value ) == Style->Value ) )
                {
                    StyleString.Append('|');
                    StyleString.Append(Style->Name);
                    StyleBits &= ~Style->Value;
                }
            }
        }

        if ( StyleString.Len() != 0 )
        {
            XmlSetVariable(_T("style"),StyleString.c_str()+1);
        }
    }

    if ( pType & bptEnabled )
    {
    	if ( !BaseParams.Enabled ) XmlSetInteger(_T("enabled"),0);
    }
    
    if ( pType & bptFocused )
    {
    	if ( BaseParams.Focused ) XmlSetInteger(_T("focused"),1);
    }
    
    if ( pType & bptHidden )
    {
    	if ( BaseParams.Hidden ) XmlSetInteger(_T("hiddedn"),1);
    }
    
    if ( pType & bptColours )
    {
    	if ( BaseParams.FgType == wxsCUSTOM_COLOUR )
    	{
    		XmlSetVariable( _T("fg"),
                wxString::Format(_T("#%02X%02X%02X"),
                    BaseParams.Fg.Red(),
                    BaseParams.Fg.Green(),
                    BaseParams.Fg.Blue() ) );
    	}
    	else
    	{
    		for ( int i=0; i<wxsSystemColoursCount; i++ )
    		{
    			if ( BaseParams.FgType == wxsSystemColours[i].Value )
    			{
    				XmlSetVariable( _T("fg"), wxsSystemColours[i].Name );
    				break;
    			}
    		}
    	}

    	if ( BaseParams.BgType == wxsCUSTOM_COLOUR )
    	{
    		XmlSetVariable( _T("bg"),
                wxString::Format(_T("#%02X%02X%02X"),
                    BaseParams.Bg.Red(),
                    BaseParams.Bg.Green(),
                    BaseParams.Bg.Blue() ) );
    	}
    	else
    	{
    		for ( int i=0; i<wxsSystemColoursCount; i++ )
    		{
    			if ( BaseParams.BgType == wxsSystemColours[i].Value )
    			{
    				XmlSetVariable( _T("bg"), wxsSystemColours[i].Name );
    				break;
    			}
    		}
    	}
    }
    
    if ( pType & bptFont && BaseParams.UseFont )
    {
    	TiXmlElement* Store = XmlElem();
    	XmlAssignElement(Store->InsertEndChild(TiXmlElement("font"))->ToElement());
    	wxFont& Font = BaseParams.Font;
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
            return wxString(Text->Value(), wxConvUTF8);
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
        wxsWidget* Child = wxsWidgetFactory::Get()->Generate(wxString(Name,wxConvUTF8),GetResource());
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
// TODO (SpOoN#1#): Support font
    CDefines.Style = _T("");
    CDefines.InitCode = _T("");

    // Filling up styles

    wxsStyle* Style = GetInfo().Styles;
    int StyleV = BaseParams.Style;
    if ( Style )
    {
        for ( ; Style->Name; Style++ )
        {
            if ( Style->Value && ( ( StyleV & Style->Value ) == Style->Value ) )
            {
                StyleV &= ~Style->Value;

                if ( CDefines.Style.Length() ) CDefines.Style.Append('|');
                CDefines.Style.Append(Style->Name);
            }
        }
    }

    if ( CDefines.Style.Len() == 0 ) CDefines.Style = _T("0");

    // Creating position

    if ( BaseParams.DefaultPosition ) CDefines.Pos = _T("wxDefaultPosition");
    else CDefines.Pos.Printf(_T("wxPoint(%d,%d)"),BaseParams.PosX,BaseParams.PosY);

    // Creating size

    if ( BaseParams.DefaultSize ) CDefines.Size = _T("wxDefaultSize");
    else CDefines.Size.Printf(_T("wxSize(%d,%d)"),BaseParams.SizeX,BaseParams.SizeY);
    
    // Creating colours

    BasePropertiesType pType = GetBPType();

    if ( pType & bptColours )
    {
        wxString FColour;
        wxString BColour;
        
        if ( BaseParams.FgType == wxsCUSTOM_COLOUR )
        {
        	FColour.Printf(_T("wxColour(%d,%d,%d)"),
                BaseParams.Fg.Red(),
                BaseParams.Fg.Green(),
                BaseParams.Fg.Blue());
        }
        else
        {
        	for ( int i=0; i<wxsSystemColoursCount; i++ )
        	{
        		if ( BaseParams.FgType == wxsSystemColours[i].Value )
        		{
        			FColour.Printf(_T("wxSystemSettings::GetColour(%s)"),wxsSystemColours[i].Name);
        			break;
        		}
        	}
        }
        
        if ( BaseParams.BgType == wxsCUSTOM_COLOUR )
        {
        	FColour.Printf(_T("wxColour(%d,%d,%d)"),
                BaseParams.Bg.Red(),
                BaseParams.Bg.Green(),
                BaseParams.Bg.Blue());
        }
        else
        {
        	for ( int i=0; i<wxsSystemColoursCount; i++ )
        	{
        		if ( BaseParams.BgType == wxsSystemColours[i].Value )
        		{
        			BColour.Printf(_T("wxSystemSettings::GetColour(%s)"),wxsSystemColours[i].Name);
        			break;
        		}
        	}
        }
        
        
        if ( !FColour.empty() )
        {
            CDefines.InitCode << BaseParams.VarName << _T("->SetForegroundColour(")
                              << FColour << _T(");");
        }
        
        if ( !BColour.empty() )
        {
            CDefines.InitCode << BaseParams.VarName << _T("->SetBackgroundColour(")
                              << BColour << _T(");");
        }
    }
    
    if ( pType & bptFont && BaseParams.UseFont )
    {
    	wxFont& Font = BaseParams.Font;
    	CDefines.InitCode << BaseParams.VarName << _T("->SetFont(wxFont(") <<
                             Font.GetPointSize() << _T(',');
                             
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
    
    if ( pType & bptEnabled && !BaseParams.Enabled )
    {
    	CDefines.InitCode <<  BaseParams.VarName << _T("->Disable();");
    }
    
    if ( pType & bptFocused && BaseParams.Focused )
    {
        CDefines.InitCode <<  BaseParams.VarName << _T("->SetFocus();");
    }
    
    if ( pType & bptHidden && BaseParams.Hidden )
    {
    	CDefines.InitCode <<  BaseParams.VarName << _T("->Hide();");
    }
    
    if ( pType & bptToolTip && !BaseParams.ToolTip.empty() )
    {
    	CDefines.InitCode << BaseParams.VarName << _T("->SetToolTop(")
    	                  << GetWxString(BaseParams.ToolTip) << _T(");");
    }
        
    
    
    return CDefines;
}

bool wxsWidgetManager::RegisterInFactory()
{
    if ( !wxsWidgetFactory::Get() ) return false;
    wxsWidgetFactory::Get()->RegisterManager(this);
    return true;
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
              stringArray.Add(wxString(Node->ToText()->Value(),wxConvUTF8));

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
