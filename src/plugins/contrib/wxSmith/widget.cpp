#include "Widget.h"

#include "properties/wxsborderproperty.h"
#include "properties/wxsplacementproperty.h"
#include "properties/wxsstyleproperty.h"
#include "wxswidgetfactory.h"

wxsWidget::~wxsWidget()
{
    if ( Preview    ) KillPreview();
    if ( Properties ) KillProperties();
}

void wxsWidget::AddDefaultProperties(BasePropertiesType pType)
{
    // Adding standard items
    
    switch ( pType )
    {
        case propSizer:
            PropertiesObject.AddProperty(wxT("Variable Name:"),BaseParams.VarName);
            PropertiesObject.AddProperty(wxT("Local Variable:"),BaseParams.VarNotStored);
            break;
            
        case propWidget:
            PropertiesObject.AddProperty(wxT("Variable Name:"),BaseParams.VarName);
            PropertiesObject.AddProperty(wxT("Local Variable:"),BaseParams.VarNotStored);
            PropertiesObject.AddProperty(wxT("Identifier:"),BaseParams.IdName);
            PropertiesObject.Add2IProperty(wxT("Position:"),BaseParams.PosX,BaseParams.PosY);
            PropertiesObject.AddProperty(wxT("Default pos.:"),BaseParams.DefaultPosition);
            PropertiesObject.Add2IProperty(wxT("Size:"),BaseParams.SizeX,BaseParams.SizeY);
            PropertiesObject.AddProperty(wxT("Default size:"),BaseParams.DefaultSize);
            break;
            
        default:;
    }
    
    // Adding sizer configuration

    switch ( pType )
    {
        case propSizer:
        case propWidget:
            PropertiesObject.AddProperty(wxT("Proportion:"),BaseParams.Proportion);
            PropertiesObject.AddProperty(wxT("Border:"),new wxsBorderProperty(&PropertiesObject,BaseParams.BorderFlags));
            //PropertiesObject.AddProperty(wxT("Border:"),BaseParams.BorderFlags);
            PropertiesObject.AddProperty(wxT("Border size:"),BaseParams.Border);
            //PropertiesObject.AddProperty(wxT("Placement:"),BaseParams.Placement);
            PropertiesObject.AddProperty(wxT("Placement:"),new wxsPlacementProperty(&PropertiesObject,BaseParams.Placement,BaseParams.Expand,BaseParams.Shaped));
            break;
            
        default:;
    }

    // Adding style property

    switch ( pType )
    {
        case propWidget:
            if ( GetInfo().Styles != NULL )
            {
                PropertiesObject.AddProperty(
                    wxT("Style:"),
                    new wxsStyleProperty(
                        &PropertiesObject,
                        BaseParams.Style,
                        GetInfo().Styles),
                    -1,
                    true, true
                    );
            }
            break;
            
        default:;
    }

}

wxWindow* wxsWidget::CreatePreview(wxWindow* Parent,wxsWindowEditor* Editor)
{
    if ( Preview ) KillPreview();
    
    /* Creating widget */
    CurEditor = Editor;
    Preview = MyCreatePreview(Parent);

    /* Creating preview of child widgets */
    int Cnt = IsContainer() ? GetChildCount() : 0;
    for ( int i=0; i<Cnt; i++ )
    {
        GetChild(i)->CreatePreview(Preview,CurEditor);
    }
    
    /* Updating content of preview */
    MyUpdatePreview();
    
    return Preview;
}

void wxsWidget::UpdatePreview(bool IsReshaped,bool NeedRecreate)
{
    if ( Updating ) return;
    if ( !Preview ) return;
    assert ( CurEditor != NULL );
    
    Updating = true;
    
    if ( NeedRecreate )
    {
        IsReshaped = true;
        wxsWindowEditor* Editor = CurEditor;
        KillPreview();
        CreatePreview(GetParentPreview(),Editor);
    }
    else
    {
        MyUpdatePreview();
    }
    
    if ( IsReshaped )
    {
        if ( GetParent() )
        {
            GetParent()->UpdatePreview(true,false);
        }
        else
        {
            CurEditor->PreviewReshaped();
        }
    }
    
    Updating = false;
}

void wxsWidget::KillPreview()
{
    /* Killing child windows */
    if ( IsContainer() )
    {
        int Cnt = GetChildCount();
        for ( int i=0; i<Cnt; i++ )
        {
            GetChild(i)->KillPreview();
        }
    }
    
    /* Killing this one */
    if ( Preview != NULL )
    {
        MyDeletePreview(Preview);
        Preview = NULL;
    }
    
    CurEditor = NULL;
}

void wxsWidget::XmlAssignElement(TiXmlElement* Elem)
{
    XmlElement = Elem;
}

void wxsWidget::XmlReadDefaultsT(BasePropertiesType pType)
{
    assert ( XmlElem() != NULL );
    
/*
    if ( IsSizer )
    {
    }
    else
    */
    {
        /* Processing styles */
        {
            wxString StyleStr = XmlGetVariable("style");
            wxsStyle* Styles = GetInfo().Styles;
            BaseParams.Style = 0;
            if ( Styles )
            {
                for ( ; Styles->Name != NULL; Styles++ )
                {
                    int Pos = StyleStr.Find(Styles->Name);
                    int Len = (int)strlen(Styles->Name);
                    if ( Pos < 0 ) continue;
                    
                    // One character after style in StyleStr should be checked - it
                    // must be '|'. Same for character before.
                    
                    if ( ( Pos + Len >= (int)StyleStr.Len() || StyleStr.GetChar(Pos+Len) == '|' ) &&
                         ( Pos == 0 || StyleStr.GetChar(Pos-1) == '|' ) )
                    {
                        BaseParams.Style |= Styles->Value;
                    }
                }
            }
        }
        
        /* Processing position */
        BaseParams.DefaultPosition = !XmlGetIntPair("pos",BaseParams.PosX,BaseParams.PosY);
        
        /* Processing size */
        BaseParams.DefaultSize = !XmlGetIntPair("size",BaseParams.SizeX,BaseParams.SizeY);
    }
    
}

const char* wxsWidget::XmlGetVariable(const char* name)
{
    assert ( XmlElem() != NULL );
    
    if ( !name || !*name ) return NULL;
    TiXmlElement* Elem = XmlElem()->FirstChildElement(name);
    if ( !Elem ) return NULL;

    TiXmlNode* Node = Elem->FirstChild();
    while ( Node )
    {
        TiXmlText* Text = Node->ToText();
        if ( Text )
        {
            return Text->Value();
        }
        Node = Node->NextSibling();
    }
    
    return NULL;
}

int wxsWidget::XmlGetInteger(const char* name,bool& IsInvalid,int DefaultValue)
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

bool wxsWidget::XmlGetIntPair(const char* Name,int& P1,int& P2,int DefP1,int DefP2)
{
    long _P1, _P2;
    wxString Tmp = XmlGetVariable(Name);
    if ( Tmp.Length() &&
         Tmp.BeforeFirst(',').ToLong(&_P1) && 
         Tmp.AfterLast(',').ToLong(&_P2) )
    {
        P1 = (int)_P1;
        P2 = (int)_P2;
        return true;
    }
    
    P1 = DefP1;
    P2 = DefP2;
    return false;
}
        
void wxsWidget::XmlLoadChildren()
{
    assert ( wxsWidgetFactory::Get() != NULL );
    
    for ( TiXmlElement* Element = XmlElem()->FirstChildElement("object");
          Element != NULL;
          Element = Element->NextSiblingElement("object") )
    {
        const char* Name = Element->Attribute("class");
        if ( Name && *Name )
        {
            DebLog("  * found %s",Name);
            wxsWidget* Child = wxsWidgetFactory::Get()->Generate(Name);
            if ( !Child )
            {
                DebLog("Unknown object : '%s'",Name);
            }
            else
            {
                if ( Child->XmlLoad(Element) )
                {
                    AddChild(Child);
                }
                else
                {
                    wxsWidgetFactory::Get()->Kill(Child);
                }
            }
        }
    }
}

const wxsWidget::CodeDefines& wxsWidget::GetCodeDefines()
{
// TODO (SpOoN#1#): Support for font and colours
    CDefines.FColour = "";
    CDefines.BColour = "";
    CDefines.Font = "";
    CDefines.Style = "";
    
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
    
    // Creating position
    
    if ( BaseParams.DefaultPosition ) CDefines.Pos = wxT("wxDefaultPosition");
    else CDefines.Pos = wxString::Format(wxT("wxPoint(%d,%d)"),BaseParams.PosX,BaseParams.PosY);
    
    // Creating size
    
    if ( BaseParams.DefaultSize ) CDefines.Size = wxT("wwxDefaultSize");
    else CDefines.Size = wxString::Format(wxT("wxSize(%d,%d)"),BaseParams.SizeX,BaseParams.SizeY);
    
    return CDefines;
}

bool wxsWidgetManager::RegisterInFactory()
{
    if ( !wxsWidgetFactory::Get() ) return false;
    wxsWidgetFactory::Get()->RegisterManager(this);
    return true;
}

const wxString& wxsWidget::GetCString(const wxString& Source)
{
    static wxString Result;
    Result = wxT("\"");
    
    int Len = Source.Length();
    
    for ( int i=0; i<Len; i++ )
    {
        char ch = Source.GetChar(i);

        if ( ch < 0x20 )
        {
            switch ( ch )
            {
                case '\n' : Result.Append("\\n"); break;
                case '\t' : Result.Append("\\t"); break;
                case '\v' : Result.Append("\\v"); break;
                case '\b' : Result.Append("\\b"); break;
                case '\r' : Result.Append("\\r"); break;
                case '\f' : Result.Append("\\f"); break;
                case '\a' : Result.Append("\\a"); break;
                default   :
                    {
                        wxString Formater = wxString::Format("\\%d%d%d",
                            ( ch >> 6 ) & 7,
                            ( ch >> 3 ) & 7,
                            ( ch >> 0 ) & 7 );
                        Result.Append(Formater.c_str());
                    }
            }
        }
        else 
        {
            switch ( ch )
            {
                case '\\': Result.Append("\\\\"); break;
                case '\?': Result.Append("\\\?"); break;
                case '\'': Result.Append("\\\'"); break;
                case '\"': Result.Append("\\\""); break;
                default  : Result.Append(ch);
            }
        }
    }

    Result.Append('\"');
    return Result;
}
