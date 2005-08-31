#include "wxswidgetevents.h"

#include "widget.h"
#include "resources/wxswindowres.h"
#include <editormanager.h>

#define HandlerXmlElementName   "handler"
#define HandlerXmlEntryName     "entry"
#define HandlerXmlFunctionName  "function"

wxsWidgetEvents::wxsWidgetEvents(wxsWidget* Wdg):
    Widget(Wdg)
{
}

wxsWidgetEvents::~wxsWidgetEvents()
{
	ClearEvents();
}

void wxsWidgetEvents::ClearEvents()
{
    for ( size_t i = 0; i<Events.size(); ++i )
	{
		delete Events[i];
	}
	Events.clear();
}

int wxsWidgetEvents::AddEvent(const wxsEventDesc& Event,bool Clear)
{
    wxsEventDesc* New = new wxsEventDesc;
    New->EventEntry = Event.EventEntry;
    New->EventTypeName = Event.EventTypeName;
    New->FunctionName = Clear ? _T("") : Event.FunctionName;
    New->NewFuncNameBase = Event.NewFuncNameBase;
    New->WithId = Event.WithId;
    Events.push_back(New);
    return Events.size()-1;
}

wxsEventDesc * wxsWidgetEvents::GetEventByFunction(const wxString& Name)
{
    for ( EventsI i = Events.begin(); i!=Events.end(); ++i )
    {
    	if ( (*i)->FunctionName == Name )
    	{
    		return *i;
    	}
    }

    return NULL;
}

wxsEventDesc * wxsWidgetEvents::GetEventByEntry(const wxString& Entry)
{
    for ( EventsI i = Events.begin(); i!=Events.end(); ++i )
    {
    	if ( (*i)->EventEntry == Entry )
    	{
    		return *i;
    	}
    }

    return NULL;
}

wxString wxsWidgetEvents::GetArrayEnteries(int TabSize)
{
    wxString Code;
    for ( EventsI i = Events.begin(); i!=Events.end(); ++i )
    {
    	wxsEventDesc* Event = *i;
    	if ( Event->FunctionName.Length() )
    	{
    		Code.Append(_T(' '),TabSize);
    		Code.Append(Event->EventEntry);
    		Code.Append(_T('('));
    		if ( Event->WithId )
    		{
    			Code.Append(Widget->GetBaseParams().IdName);
    			Code.Append(_T(','));
    		}
    		Code.Append(Widget->GetResource()->GetClassName());
    		Code.Append(_T("::"));
    		Code.Append(Event->FunctionName);
    		Code.Append(_T(")\n"));
    	}
    }

    return Code;
}

wxsWidgetEvents& wxsWidgetEvents::operator=(const wxsWidgetEvents& Source)
{
	ClearEvents();
	for ( EventsCI i = Source.Events.begin(); i!=Source.Events.end(); ++i )
	{
		AddEvent(*(*i));
	}
	return *this;
}

void wxsWidgetEvents::ClearFunctionNames()
{
	ClearFunctionNames();
    for ( EventsI i = Events.begin(); i!=Events.end(); ++i )
    {
    	(*i)->FunctionName = _T("");
    }
}

void wxsWidgetEvents::XmlLoadFunctions(TiXmlElement* Element)
{
	for ( TiXmlElement* Handler = Element->FirstChildElement(HandlerXmlElementName);
          Handler;
          Handler = Handler->NextSiblingElement(HandlerXmlElementName) )
    {
    	const char* HandlerName;
    	const char* FunctionName;
    	wxsEventDesc* Event;

    	if ( ! ( HandlerName  = Handler->Attribute(HandlerXmlEntryName) ) ) continue;
    	if ( ! ( FunctionName = Handler->Attribute(HandlerXmlFunctionName) ) ) continue;
    	if ( ! ( Event = GetEventByEntry(wxString(HandlerName,wxConvUTF8)) ) ) continue;
    	if ( GetEventByFunction(wxString(FunctionName,wxConvUTF8)) ) continue;
    	Event->FunctionName = wxString(FunctionName,wxConvUTF8);
    }
}

void wxsWidgetEvents::XmlSaveFunctions(TiXmlElement* Element)
{
    for ( EventsI i = Events.begin(); i != Events.end(); ++i )
    {
    	wxsEventDesc* Event = *i;
    	if ( Event->FunctionName.Length() )
    	{
    		TiXmlElement* Handler = Element->InsertEndChild( TiXmlElement(HandlerXmlElementName) ) -> ToElement();
    		Handler->SetAttribute(HandlerXmlEntryName,Event->EventEntry.mb_str());
    		Handler->SetAttribute(HandlerXmlFunctionName,Event->FunctionName.mb_str());
    	}
    }
}

