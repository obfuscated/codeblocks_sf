#include "wxswindowresdataobject.h"
#include "../widget.h"
#include "../wxswidgetfactory.h"
#include <sstream>

#define wxsDF_WIDGET   _T("wxSmith XML")

wxsWindowResDataObject::wxsWindowResDataObject()
{
}

wxsWindowResDataObject::~wxsWindowResDataObject()
{
}

void wxsWindowResDataObject::GetAllFormats(wxDataFormat *formats, Direction dir) const
{
    formats[0] = wxDataFormat(wxDF_TEXT);
    formats[1] = wxDataFormat(wxsDF_WIDGET);
}

bool wxsWindowResDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    memcpy(buf,XmlData.mb_str(),XmlData.Length()+1);
   	return true;
}

size_t wxsWindowResDataObject::GetDataSize(const wxDataFormat& format) const
{
    return XmlData.Length()+1;
}

size_t wxsWindowResDataObject::GetFormatCount(Direction dir) const
{
    return 2;
}

wxDataFormat wxsWindowResDataObject::GetPreferredFormat(Direction dir) const
{
    return wxDataFormat(wxsDF_WIDGET);
}

bool wxsWindowResDataObject::SetData(const wxDataFormat& format, size_t len, const void *buf)
{
	if ( !len ) return false;
    char* CharBuff = new char[len];
    memcpy(CharBuff,buf,len);
    CharBuff[len-1] = '\0';      // Adding padding zero, just for sure
    XmlData = wxString(CharBuff,wxConvUTF8);
    return true;
}

void wxsWindowResDataObject::MakeFromWidget(wxsWidget* Widget)
{
	if ( !Widget ) return;
	
    std::ostringstream buffer;
    
    TiXmlDocument Doc;
    TiXmlElement* Elem = Doc.InsertEndChild(TiXmlElement("object"))->ToElement();
    if ( !Elem ) return;
    Elem->SetAttribute("class",Widget->GetInfo().Name.mb_str());
    if ( !Widget->XmlSave(Elem) ) return;
    buffer << Doc;
    XmlData = wxString(buffer.str().c_str(),wxConvUTF8);
}

wxsWidget* wxsWindowResDataObject::BuildWidget(wxsWindowRes* Resource) const
{
    std::istringstream buffer(std::string(XmlData.mb_str()));
	TiXmlDocument Doc;
	buffer >> Doc;
	
	TiXmlElement* Root = Doc.FirstChildElement("object");
	const char* Class = Root->Attribute("class");
	if ( !Class || !*Class ) return NULL;
	
	wxsWidget* Widget = wxsWidgetFactory::Get()->Generate(wxString(Class,wxConvUTF8),Resource);
	if ( !Widget ) return NULL;
	
	Widget->XmlLoad(Root);
	return Widget;
}
