#include "Widget.h"

#include "properties/wxsborderproperty.h"
#include "properties/wxsplacementproperty.h"
#include "properties/wxsstyleproperty.h"
#include "wxswidgetfactory.h"
#include "wxsresource.h"
#include "wxsdefevthandler.h"
#include <wx/tokenzr.h>
#include <wx/list.h>

wxsWidget::~wxsWidget()
{
    if ( Preview    ) KillPreview();
    if ( Properties ) KillProperties();

    while ( GetChildCount() )
    {
        wxsWidgetFactory::Get()->Kill(GetChild(GetChildCount()-1));
    }
}

void wxsWidget::AddDefaultProperties(BasePropertiesType pType)
{
    // Adding standard items

    if ( pType & bptVariable )
    {
        PropertiesObject.AddProperty(wxT("Var Name:"),BaseParams.VarName);
        PropertiesObject.AddProperty(wxT("Is Member:"),BaseParams.IsMember);
    }
    
    if ( pType & bptId )
    {
        PropertiesObject.AddProperty(wxT("Id:"),BaseParams.IdName);
    }
    
    if ( pType & bptPosition )
    {
        PropertiesObject.Add2IProperty(wxT("Position:"),BaseParams.PosX,BaseParams.PosY);
        PropertiesObject.AddProperty(wxT(" Default:"),BaseParams.DefaultPosition);
    }
    
    if ( pType & bptSize )
    {
        PropertiesObject.Add2IProperty(wxT("Size:"),BaseParams.SizeX,BaseParams.SizeY);
        PropertiesObject.AddProperty(wxT(" Default:"),BaseParams.DefaultSize);
    }
    
    // Adding sizer configuration

    PropertiesObject.AddProperty(wxT("Proportion:"),BaseParams.Proportion);
    PropertiesObject.AddProperty(wxT("Border:"),new wxsBorderProperty(&PropertiesObject,BaseParams.BorderFlags));
    PropertiesObject.AddProperty(wxT("Border size:"),BaseParams.Border);
    PropertiesObject.AddProperty(wxT("Placement:"),new wxsPlacementProperty(&PropertiesObject,BaseParams.Placement,BaseParams.Expand,BaseParams.Shaped));

    // Adding style property

    if ( pType & bptStyle )
    {
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
    }

}

wxWindow* wxsWidget::CreatePreview(wxWindow* Parent,wxsWindowEditor* Editor)
{
    if ( Preview ) KillPreview();
    
    /* Creating widget */
    CurEditor = Editor;
    Preview = MyCreatePreview(Parent);
    if ( !Preview ) return NULL;
    
    Preview->PushEventHandler(Handler = new wxsDefEvtHandler(this));

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
    
    CurEditor->RecreatePreview();
    CurEditor->GetResource()->NotifyChange();

    /*
    if ( NeedRecreate )
    {
        IsReshaped = true;
        wxsWindowEditor* Editor = CurEditor;
        KillPreview();
        CreatePreview(GetParentPreview(),Editor);
    }

    MyUpdatePreview();
    
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
    
    */
    
    Updating = false;
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
        if ( Handler )
        {
            Preview->RemoveEventHandler(Handler);
            delete Handler;
            Handler = NULL;
        }
        MyDeletePreview(Preview);
        Preview = NULL;
    }
    
    CurEditor = NULL;
}

void wxsWidget::XmlAssignElement(TiXmlElement* Elem)
{
    XmlElement = Elem;
}

bool wxsWidget::XmlLoadDefaultsT(BasePropertiesType pType)
{
    assert ( XmlElem() != NULL );

    /* Processing position */
    if ( pType & bptPosition )
    {
        BaseParams.DefaultPosition = !XmlGetIntPair("pos",BaseParams.PosX,BaseParams.PosY);
    }
        
    /* Processing size */
    if ( pType & bptSize )
    {
        BaseParams.DefaultSize = !XmlGetIntPair("size",BaseParams.SizeX,BaseParams.SizeY);
    }
    
    /* Processing id */
    if ( pType & bptId )
    {
        const char* IdName = XmlElem()->Attribute("name");
        BaseParams.IdName = IdName ? IdName : wxT("");
    }
    
    /* Processing variable name and locality */
    if ( pType & bptVariable )
    {
        const char* VarName = XmlElem()->Attribute("variable");
        BaseParams.VarName = VarName ? VarName : wxT("");
        const char* IsMember = XmlElem()->Attribute("member");
        BaseParams.IsMember = IsMember ? ( strcasecmp(IsMember,"no") != 0 ) : true;
    }

    /* Processing style */
    if ( pType & bptStyle )
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
    
    if ( IsContainer() )
    {
        if ( !XmlLoadChildren() ) return false;
    }
    
    
    return true;
}

bool wxsWidget::XmlSaveDefaultsT(BasePropertiesType pType)
{
    assert ( XmlElem() != NULL );
   
    if ( pType & bptPosition )
    {
        if ( !BaseParams.DefaultPosition )
        {
            XmlSetIntPair("pos",BaseParams.PosX,BaseParams.PosY);
        }
    }
    
    if ( pType & bptSize )
    {
        if ( !BaseParams.DefaultSize )
        {
            XmlSetIntPair("size",BaseParams.SizeX,BaseParams.SizeY);
        }
    }

    if ( pType & bptId )
    {
        XmlElem()->SetAttribute("name",BaseParams.IdName.c_str());
    }
    
    if ( pType & bptVariable )
    {
        XmlElem()->SetAttribute("variable",BaseParams.VarName.c_str());
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
            XmlSetVariable("style",StyleString.c_str()+1);
        }
    }
    
    if ( IsContainer() )
    {
        if ( !XmlSaveChildren() ) return false;
    }
    
    return true;
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
 
bool wxsWidget::XmlSetVariable(const char* Name,const char* Value)
{
    assert ( XmlElem() != NULL );
    TiXmlNode * NewNode = XmlElem()->InsertEndChild(TiXmlElement(Name));
    if ( NewNode )
    {
        NewNode->InsertEndChild(TiXmlText(Value));
        return true;
    }
    return false;
}

bool wxsWidget::XmlSetInteger(const char* Name,int Value)
{
    return XmlSetVariable(Name,wxString::Format("%d",Value).c_str());
}

bool wxsWidget::XmlSetIntPair(const char* Name,int Val1,int Val2)
{
    return XmlSetVariable(Name,wxString::Format("%d,%d",Val1,Val2).c_str());
}

bool wxsWidget::XmlLoadChildren()
{
    assert ( XmlElem() != NULL );
    
    bool Ret = true;
    
    for ( TiXmlElement* Element = XmlElem()->FirstChildElement("object");
          Element != NULL;
          Element = Element->NextSiblingElement("object") )
    {
        const char* Name = Element->Attribute("class");
        
        if ( Name && *Name )
        {
            TiXmlElement* RealObject = Element;
            
            if ( !strcmp(Name,"sizeritem") )
            {
                RealObject = Element->FirstChildElement("object");
                if ( !RealObject ) continue;
                Name = RealObject->Attribute("class");
                if ( !Name || !*Name ) continue;
            }
                
            wxsWidget* Child = wxsWidgetFactory::Get()->Generate(Name);
            if ( !Child )
            {
                Ret = false;
                DebLog("Unknown object : '%s'",Name);
            }
            else
            {
                if ( !Child->XmlLoad(RealObject) ) Ret = false;
                Child->XmlLoadSizerStuff(Element);
                AddChild(Child);
            }
        }
    }
    return Ret;
}

bool wxsWidget::XmlSaveChildren()
{
    bool IsSizer = GetInfo().Sizer;
    bool Return = true;
    
    int Count = GetChildCount();
    
    for ( int i=0; i<Count; i++ )
    {
        wxsWidget* W = GetChild(i);
        
        if ( W && W->GetInfo().Name )
        {
            TiXmlNode* AddChildToThis = XmlElem();
            const char* ClassName = W->GetInfo().Name;
            
            if ( IsSizer && strcmp(ClassName,"spacer") )
            {
               // Adding sizeritem object
               AddChildToThis = XmlElem()->InsertEndChild(TiXmlElement("object"));
               AddChildToThis->ToElement()->SetAttribute("class","sizeritem");
            }
            
            TiXmlElement* SaveTo = AddChildToThis->InsertEndChild(TiXmlElement("object"))->ToElement();
            
            if ( SaveTo )
            {
                SaveTo->SetAttribute("class",ClassName);
                if ( !W->XmlSave(SaveTo) ) Return = false;
            }
            else
            {
                Return = false;
            }
            
            if ( IsSizer )
            {
                W->XmlSaveSizerStuff(AddChildToThis->ToElement());
            }
        }
    }
    
    return Return;
}

void wxsWidget::XmlLoadSizerStuff(TiXmlElement* Elem)
{
    TiXmlElement* Store = XmlElem();
    XmlElement = Elem;
    
    bool Temp;
    
    BaseParams.Proportion = XmlGetInteger("option",Temp,0);
    BaseParams.Border = XmlGetInteger("border",Temp,0);
    
    wxStringTokenizer tokens(XmlGetVariable("flag"),wxT("|"));
    
    int HorizPos = 0;
    int VertPos = 0;
    
    while ( tokens.HasMoreTokens() )
    {
        wxString Token = tokens.GetNextToken().Trim(true).Trim(false);

        #define BFItem(a,b)  else if ( Token == wxT(#a) ) BaseParams.BorderFlags |= wxsWidgetBaseParams::b
        #define Begin() if (false)
        #define Match(a) else if ( Token == wxT(#a) )
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
            BaseParams.BorderFlags |= 
                wxsWidgetBaseParams::Left |
                wxsWidgetBaseParams::Right |
                wxsWidgetBaseParams::Top |
                wxsWidgetBaseParams::Bottom;
        }
        Match(wxGROW) BaseParams.Expand = true;
        Match(wxEXPAND) BaseParams.Expand = true;
        Match(wxSHAPED) BaseParams.Shaped = true;
        Match(wxALIGN_CENTER) { HorizPos = wxALIGN_CENTER_HORIZONTAL; VertPos = wxALIGN_CENTER_VERTICAL; }
        Match(wxALIGN_CENTRE) { HorizPos = wxALIGN_CENTER_HORIZONTAL; VertPos = wxALIGN_CENTER_VERTICAL; }
        PlaceH(wxALIGN_RIGHT)
        PlaceV(wxALIGN_TOP)
        PlaceV(wxALIGN_BOTTOM)
        PlaceH(wxALIGN_CENTER_HORIZONTAL)
        PlaceH(wxALIGN_CENTRE_HORIZONTAL)
        PlaceV(wxALIGN_CENTER_VERTICAL)
        PlaceV(wxALIGN_CENTRE_VERTICAL)


// TODO (SpOoN#1#): Check other flags

    }

    if ( HorizPos==wxALIGN_RIGHT )
    {
        if ( VertPos==wxALIGN_BOTTOM ) BaseParams.Placement = wxsWidgetBaseParams::RightBottom;
        else if ( VertPos==wxALIGN_CENTER_VERTICAL ) BaseParams.Placement = wxsWidgetBaseParams::RightCenter;
        else BaseParams.Placement = wxsWidgetBaseParams::RightTop;
    }
    else if ( HorizPos==wxALIGN_CENTER_HORIZONTAL )
    {
        if ( VertPos==wxALIGN_BOTTOM ) BaseParams.Placement = wxsWidgetBaseParams::CenterBottom;
        else if ( VertPos==wxALIGN_CENTER_VERTICAL ) BaseParams.Placement = wxsWidgetBaseParams::Center;
        else BaseParams.Placement = wxsWidgetBaseParams::CenterTop;
    }
    else
    {
        if ( VertPos==wxALIGN_BOTTOM ) BaseParams.Placement = wxsWidgetBaseParams::LeftBottom;
        else if ( VertPos==wxALIGN_CENTER_VERTICAL ) BaseParams.Placement = wxsWidgetBaseParams::LeftCenter;
        else BaseParams.Placement = wxsWidgetBaseParams::LeftTop;
    }
        
    XmlElement = Store;
}

void wxsWidget::XmlSaveSizerStuff(TiXmlElement* Elem)
{
    TiXmlElement* Store = XmlElem();
    XmlElement = Elem;
    
    if ( BaseParams.Proportion ) XmlSetInteger("option",BaseParams.Proportion);
    if ( BaseParams.Border ) XmlSetInteger("border",BaseParams.Border);
    
    wxString Flags = wxT("");
    
    int BF = BaseParams.BorderFlags;

    if ( ( BF & wxsWidgetBaseParams::Left ) &&
         ( BF & wxsWidgetBaseParams::Right ) &&
         ( BF & wxsWidgetBaseParams::Top ) &&
         ( BF & wxsWidgetBaseParams::Bottom ) )
    {
        Flags.Append("|wxALL");
    }
    else
    {
        if ( ( BF & wxsWidgetBaseParams::Left   ) ) Flags.Append("|wxLEFT");
        if ( ( BF & wxsWidgetBaseParams::Right  ) ) Flags.Append("|wxRIGHT");
        if ( ( BF & wxsWidgetBaseParams::Top    ) ) Flags.Append("|wxTOP");
        if ( ( BF & wxsWidgetBaseParams::Bottom ) ) Flags.Append("|wxBOTTOM");
    }
    
    switch ( BaseParams.Placement )
    {
        case wxsWidgetBaseParams::LeftTop:      Flags.Append("|wxALIGN_LEFT|wxALIGN_TOP"); break;
        case wxsWidgetBaseParams::CenterTop:    Flags.Append("|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP"); break;
        case wxsWidgetBaseParams::RightTop:     Flags.Append("|wxALIGN_RIGHT|wxALIGN_TOP"); break;
        case wxsWidgetBaseParams::LeftCenter:   Flags.Append("|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL"); break;
        case wxsWidgetBaseParams::Center:       Flags.Append("|wxALIGN_CENTER"); break;
        case wxsWidgetBaseParams::RightCenter:  Flags.Append("|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL"); break;
        case wxsWidgetBaseParams::LeftBottom:   Flags.Append("|wxALIGN_LEFT|wxALIGN_BOTTOM"); break;
        case wxsWidgetBaseParams::CenterBottom: Flags.Append("|wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM"); break;
        case wxsWidgetBaseParams::RightBottom:  Flags.Append("|wxALIGN_RIGHT|wxALIGN_BOTTOM"); break;
    }
    
    if ( BaseParams.Expand ) Flags.Append("|wxEXPAND");
    if ( BaseParams.Shaped ) Flags.Append("|wxSHAPED");
    
    if ( Flags.Len() )
    {
        XmlSetVariable("flag",Flags.c_str()+1);
    }
    
    XmlElement = Store;
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
    
    if ( CDefines.Style.Len() == 0 ) CDefines.Style = wxT("0");
    
    // Creating position
    
    if ( BaseParams.DefaultPosition ) CDefines.Pos = wxT("wxDefaultPosition");
    else CDefines.Pos = wxString::Format(wxT("wxPoint(%d,%d)"),BaseParams.PosX,BaseParams.PosY);
    
    // Creating size
    
    if ( BaseParams.DefaultSize ) CDefines.Size = wxT("wxDefaultSize");
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

bool wxsWidget::XmlSetStringArray(const char* ParentName,const char* ChildName,wxArrayString& stringArray)
{
    assert ( XmlElem() != NULL );
 
    int Count = stringArray.GetCount();
    // No item, return without writing <content> and <item> elements 
    if(Count==0) return false;
    
    // Adding <ParentName>  element
    TiXmlElement* ParentElement;
    TiXmlElement* ChildElement;
    ParentElement = XmlElem()->InsertEndChild(TiXmlElement(ParentName))->ToElement();
    
    for ( int i=0; i<Count; i++ )
    {
      ChildElement = ParentElement->InsertEndChild(TiXmlElement(ChildName))->ToElement();
      
      if (ChildElement)
        ChildElement->InsertEndChild(TiXmlText(stringArray[i].c_str()));
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
bool wxsWidget::XmlGetStringArray(const char* ParentName,const char* ChildName,wxArrayString& stringArray)
{
    assert ( XmlElem() != NULL );
    
     // Empty it to make sure element added in an empty wxArrayString
      stringArray.Empty(); 
     
    TiXmlElement* ParentElement = XmlElem()->FirstChildElement(ParentName);
    if(!ParentElement) return false;
    
    for (TiXmlElement* ChildElement= ParentElement->FirstChildElement(ChildName);
          ChildElement != NULL;
          ChildElement = ChildElement->NextSiblingElement(ChildName))
          {
       
           TiXmlNode* Node = ChildElement->FirstChild();
           
			while(Node)
			{
			 if(Node->ToText())
              stringArray.Add(wxString::Format("%s",(Node->ToText()->Value())));

              Node=Node->NextSibling();
            } 
          }
       
      if(stringArray.GetCount() > 0)return true; else return false;
      
}
//End added
