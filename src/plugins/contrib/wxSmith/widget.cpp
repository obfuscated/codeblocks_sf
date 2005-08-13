#include "Widget.h"

#include "properties/wxsborderproperty.h"
#include "properties/wxsplacementproperty.h"
#include "properties/wxsstyleproperty.h"
#include "resources/wxswindowres.h"
#include "wxswidgetfactory.h"
#include "wxsresource.h"
#include "wxswindoweditor.h"
#include <wx/tokenzr.h>
#include <wx/list.h>

wxsWidget::~wxsWidget()
{
	assert ( Preview == NULL );
	assert ( Properties == NULL );

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
    
    // Adding sizer configuration

    PropertiesObject.AddProperty(_("Proportion:"),BaseParams.Proportion);
    PropertiesObject.AddProperty(_("Border:"),new wxsBorderProperty(&PropertiesObject,BaseParams.BorderFlags));
    PropertiesObject.AddProperty(_("Border size:"),BaseParams.Border);
    PropertiesObject.AddProperty(_("Placement:"),new wxsPlacementProperty(&PropertiesObject,BaseParams.Placement,BaseParams.Expand,BaseParams.Shaped));

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
            else if ( !strcmp(Name,"spacer") )
            {
            	// Small fixup
            	Name = "Spacer";
            }
                
            wxsWidget* Child = wxsWidgetFactory::Get()->Generate(wxString(Name,wxConvUTF8),GetResource());
            if ( !Child )
            {
                Ret = false;
            }
            else
            {
                if ( !Child->XmlLoad(RealObject) ) Ret = false;
                Child->XmlLoadSizerStuff(Element);
                if ( AddChild(Child) < 0 )
                {
                	Ret = false;
                	delete Child;
                }
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
            wxString ClassName = W->GetInfo().Name;
            bool ChildSpacer = W->GetInfo().Spacer;
            
            if ( ChildSpacer )
            {
            	ClassName = _T("spacer");
            }
            
            if ( IsSizer && ClassName != _T("spacer") )
            {
               // Adding sizeritem object
               AddChildToThis = XmlElem()->InsertEndChild(TiXmlElement("object"));
               AddChildToThis->ToElement()->SetAttribute("class","sizeritem");
            }
            
            TiXmlElement* SaveTo = AddChildToThis->InsertEndChild(TiXmlElement("object"))->ToElement();
            
            if ( SaveTo )
            {
                SaveTo->SetAttribute("class",ClassName.mb_str());
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
    
    BaseParams.Proportion = XmlGetInteger(_T("option"),Temp,0);
    BaseParams.Border = XmlGetInteger(_T("border"),Temp,0);
    BaseParams.BorderFlags = 0;
    BaseParams.Placement = wxsWidgetBaseParams::Center;
    BaseParams.Expand = false;
    BaseParams.Shaped = false;
    BaseParams.FixedMinSize = false;
    
    wxStringTokenizer tokens(XmlGetVariable(_T("flag")),_T("|"));
    
    int HorizPos = 0;
    int VertPos = 0;
    
    while ( tokens.HasMoreTokens() )
    {
        wxString Token = tokens.GetNextToken().Trim(true).Trim(false);

        #define BFItem(a,b)  else if ( Token == _T(#a) ) BaseParams.BorderFlags |= wxsWidgetBaseParams::b
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
    
    if ( BaseParams.Proportion ) XmlSetInteger(_T("option"),BaseParams.Proportion);
    if ( BaseParams.Border ) XmlSetInteger(_T("border"),BaseParams.Border);
    
    wxString Flags = GetFlagToSizer(); 
    if ( Flags.Len() )
    {
        XmlSetVariable(_T("flag"),Flags);
    }
    
    XmlElement = Store;
}

const wxsWidget::CodeDefines& wxsWidget::GetCodeDefines()
{
// TODO (SpOoN#1#): Support for font and colours
    CDefines.FColour = _T("");
    CDefines.BColour = _T("");
    CDefines.Font = _T("");
    CDefines.Style = _T("");
    
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
    else CDefines.Pos = wxString::Format(_T("wxPoint(%d,%d)"),BaseParams.PosX,BaseParams.PosY);
    
    // Creating size
    
    if ( BaseParams.DefaultSize ) CDefines.Size = _T("wxDefaultSize");
    else CDefines.Size = wxString::Format(_T("wxSize(%d,%d)"),BaseParams.SizeX,BaseParams.SizeY);
    
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
    Result = _T("\"");
    
    int Len = Source.Length();
    
    for ( int i=0; i<Len; i++ )
    {
        wxChar ch = Source.GetChar(i);

        if ( ch < _T(' ') )
        {
            switch ( ch )
            {
                case _T('\n') : Result.Append(_T("\\n")); break;
                case _T('\t') : Result.Append(_T("\\t")); break;
                case _T('\v') : Result.Append(_T("\\v")); break;
                case _T('\b') : Result.Append(_T("\\b")); break;
                case _T('\r') : Result.Append(_T("\\r")); break;
                case _T('\f') : Result.Append(_T("\\f")); break;
                case _T('\a') : Result.Append(_T("\\a")); break;
                default   :
                    {
                        wxString Formater = wxString::Format(_T("\\%d%d%d"),
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
                case _T('\\'): Result.Append(_T("\\\\")); break;
                case _T('\?'): Result.Append(_T("\\\?")); break;
                case _T('\''): Result.Append(_T("\\\'")); break;
                case _T('\"'): Result.Append(_T("\\\"")); break;
                default  : Result.Append(ch);
            }
        }
    }

    Result.Append(_T('\"'));
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

wxString wxsWidget::GetFlagToSizer()
{
    wxString Flags = _T("");
    
    int BF = BaseParams.BorderFlags;

    if ( ( BF & wxsWidgetBaseParams::Left ) &&
         ( BF & wxsWidgetBaseParams::Right ) &&
         ( BF & wxsWidgetBaseParams::Top ) &&
         ( BF & wxsWidgetBaseParams::Bottom ) )
    {
        Flags.Append(_T("|wxALL"));
    }
    else
    {
        if ( ( BF & wxsWidgetBaseParams::Left   ) ) Flags.Append(_T("|wxLEFT"));
        if ( ( BF & wxsWidgetBaseParams::Right  ) ) Flags.Append(_T("|wxRIGHT"));
        if ( ( BF & wxsWidgetBaseParams::Top    ) ) Flags.Append(_T("|wxTOP"));
        if ( ( BF & wxsWidgetBaseParams::Bottom ) ) Flags.Append(_T("|wxBOTTOM"));
    }
    
    switch ( BaseParams.Placement )
    {
        case wxsWidgetBaseParams::LeftTop:      Flags.Append(_T("|wxALIGN_LEFT|wxALIGN_TOP")); break;
        case wxsWidgetBaseParams::CenterTop:    Flags.Append(_T("|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP")); break;
        case wxsWidgetBaseParams::RightTop:     Flags.Append(_T("|wxALIGN_RIGHT|wxALIGN_TOP")); break;
        case wxsWidgetBaseParams::LeftCenter:   Flags.Append(_T("|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL")); break;
        case wxsWidgetBaseParams::Center:       Flags.Append(_T("|wxALIGN_CENTER")); break;
        case wxsWidgetBaseParams::RightCenter:  Flags.Append(_T("|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL")); break;
        case wxsWidgetBaseParams::LeftBottom:   Flags.Append(_T("|wxALIGN_LEFT|wxALIGN_BOTTOM")); break;
        case wxsWidgetBaseParams::CenterBottom: Flags.Append(_T("|wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM")); break;
        case wxsWidgetBaseParams::RightBottom:  Flags.Append(_T("|wxALIGN_RIGHT|wxALIGN_BOTTOM")); break;
    }
    
    if ( BaseParams.Expand ) Flags.Append(_T("|wxEXPAND"));
    if ( BaseParams.Shaped ) Flags.Append(_T("|wxSHAPED"));
    

    return Flags.Length() ? Flags.Mid(1) : _T("");
}

wxsWindowEditor* wxsWidget::GetEditor()
{
	return Resource ? (wxsWindowEditor*)Resource->GetEditor() : NULL;
}
