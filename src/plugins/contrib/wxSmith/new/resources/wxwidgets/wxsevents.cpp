#include "wxsevents.h"
#include "wxsitem.h"

#include <messagemanager.h>

#define HandlerXmlElementName   "handler"
#define HandlerXmlEntryName     "entry"
#define HandlerXmlTypeName      "type"
#define HandlerXmlFunctionName  "function"

wxsEvents::wxsEvents(): Item(NULL), EventArray(NULL), Count(0)
{}

void wxsEvents::SetItem(wxsItem* _Item)
{
    Functions.Clear();
    Item = _Item;
    EventArray = Item ? Item->GetEventArray() : NULL;
    Count = 0;

    // Counting number of events
    if ( EventArray )
    {
        for ( const wxsEventDesc* Event = EventArray;
              Event->ET != wxsEventDesc::EndOfList;
              Event++ )
        {
            Count++;
        }
    }

    Functions.SetCount(Count);
}

void wxsEvents::XmlLoadFunctions(TiXmlElement* Element)
{
	for ( TiXmlElement* Handler = Element->FirstChildElement(HandlerXmlElementName);
          Handler;
          Handler = Handler->NextSiblingElement(HandlerXmlElementName) )
    {
    	const char* EntryName    = Handler->Attribute(HandlerXmlEntryName);
    	const char* TypeName     = Handler->Attribute(HandlerXmlTypeName);
    	const char* FunctionName = Handler->Attribute(HandlerXmlFunctionName);

        // There must be function name, otherwise this handler definition is useless
    	if ( !FunctionName ) continue;

    	if ( EntryName )
    	{
    	    // Function given by event entry
    	    wxString Name = cbC2U(EntryName);
    	    for ( int i=0; i<Count; i++ )
    	    {
    	        if ( EventArray[i].Entry == Name )
    	        {
    	            Functions[i] = cbC2U(FunctionName);
    	            break;
    	        }
    	    }
    	}
    	else
    	{
    	    // Function given by event type
    	    wxString Name = cbC2U(TypeName);
    	    for ( int i=0; i<Count; i++ )
    	    {
    	        if ( EventArray[i].Type == Name )
    	        {
    	            Functions[i] = cbC2U(FunctionName);
    	        }
    	    }
    	}
    }
}

void wxsEvents::XmlSaveFunctions(TiXmlElement* Element)
{
    for ( int i=0; i<Count; i++ )
    {
    	if ( !Functions[i].empty() )
    	{
    		TiXmlElement* Handler = Element->InsertEndChild( TiXmlElement(HandlerXmlElementName) ) -> ToElement();
    		Handler->SetAttribute(HandlerXmlFunctionName,cbU2C(Functions[i]));
    		if ( !EventArray[i].Entry.empty() )
    		{
                Handler->SetAttribute(HandlerXmlEntryName,cbU2C(EventArray[i].Entry));
    		}
    		else
    		{
    		    Handler->SetAttribute(HandlerXmlTypeName,cbU2C(EventArray[i].Type));
    		}
    	}
    }
}

void wxsEvents::GenerateBindingCode(wxString& Code,bool UsingXrc,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString RealId = UsingXrc ?
                ( _T("XRCID(") + Item->GetIdName() + _T(")") ) :
                Item->GetIdName();

            // First we have to check i
            for ( int i=0; i<Count; i++ )
            {
                if ( !Functions[i].empty() )
                {
                    switch ( EventArray[i].ET )
                    {
                        case wxsEventDesc::Id:
                            Code << _T("Connect(") << RealId << _T(",")
                                 << EventArray[i].Type << _T("(wxObjectEventFunction)")
                                 << Functions[i] << _T(");\n");
                            break;

                        case wxsEventDesc::NoId:
                            Code << Item->GetVarName() << _T("->Connect(") << RealId
                                 << _T(",") << EventArray[i].Type
                                 << _T("(wxObjectEventFunction)") << Functions[i]
                                 << _T(",NULL,this);\n");
                            break;

                        default:
                            break;
                    }
                }
            }
            return;
        }
    }

    wxsLANGMSG(wxsEvents::GenerateBindingCode,Language);
}

bool wxsEvents::ForceVariable()
{
    for ( int i=0; i<Count; i++ )
    {
        if ( !Functions[i].empty() )
        {
            return true;
        }
    }
    return false;
}
