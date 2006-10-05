#include "../wxsheaders.h"
#include "wxswindowresdataobject.h"
#include "wxswindowres.h"
#include "../widget.h"
#include "../wxswidgetfactory.h"
#include <sstream>

wxsWindowResDataObject::wxsWindowResDataObject():
    WidgetsCount(0)
{
	Clear();
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
	wxString XmlData = GetXmlData();
    memcpy(buf,cbU2C(XmlData),XmlData.Length()+1);
   	return true;
}

size_t wxsWindowResDataObject::GetDataSize(const wxDataFormat& format) const
{
    return GetXmlData().Length()+1;
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
    char* CharBuff = new char[len+1];
    memcpy(CharBuff,buf,len);
    CharBuff[len] = '\0';      // Adding padding zero
    bool Ret = SetXmlData(cbC2U(CharBuff));
    delete[] CharBuff;
    return Ret;
}

void wxsWindowResDataObject::Clear()
{
	XmlDoc.Clear();
	XmlElem = XmlDoc.InsertEndChild(TiXmlElement("resource"))->ToElement();
	WidgetsCount = 0;
}

bool wxsWindowResDataObject::AddWidget(wxsWidget* Widget)
{
	if ( !Widget ) return false;
    TiXmlElement* Elem = XmlElem->InsertEndChild(TiXmlElement("object"))->ToElement();
    if ( !Elem ) return false;
    Elem->SetAttribute("class",cbU2C(Widget->GetInfo().Name));
    if ( !Widget->XmlSave(Elem) )
    {
        XmlElem->RemoveChild(Elem);
        return false;
    }
	WidgetsCount++;
	return true;
}

int wxsWindowResDataObject::GetWidgetCount() const
{
	return WidgetsCount;
}

wxsWidget* wxsWindowResDataObject::BuildWidget(wxsWindowRes* Resource,int Index) const
{
	if ( Index < 0 || Index > WidgetsCount ) return NULL;

	TiXmlElement* Root = XmlElem->FirstChildElement("object");
	if ( !Root ) return NULL;
	while ( Index )
	{
		Index--;
		Root = Root->NextSiblingElement("object");
		if ( !Root ) return NULL;
	}
	const char* Class = Root->Attribute("class");
	if ( !Class || !*Class ) return NULL;

	wxsWidget* Widget = wxsGEN(cbC2U(Class),Resource);
	if ( !Widget )
	{
	    if ( Resource->GetEditMode() == wxsREMSource ) return false;
        Widget = wxsGEN(_T("Custom"),Resource);
        if ( !Widget ) return false;
	}

	Widget->XmlLoad(Root);
	return Widget;
}

bool wxsWindowResDataObject::SetXmlData(const wxString& Data)
{
    XmlDoc.Clear();
    WidgetsCount = 0;
	XmlDoc.Parse(cbU2C(Data));
    if ( XmlDoc.Error() )
    {
        DBGLOG(_T("wxSmith: Error loading Xml data -> ") + cbC2U(XmlDoc.ErrorDesc()));
    	Clear();
    	return false;
    }

    XmlElem = XmlDoc.FirstChildElement("resource");
    if ( !XmlElem )
    {
    	Clear();
    	return false;
    }

    for ( TiXmlElement* Elem = XmlElem->FirstChildElement("object");
          Elem;
          Elem = Elem->NextSiblingElement("object") )
    {
    	WidgetsCount++;
    }

    return true;
}

wxString wxsWindowResDataObject::GetXmlData() const
{
    #ifdef TIXML_USE_STL
        std::ostringstream buffer;
        buffer << XmlDoc;
        return cbC2U(buffer.str().c_str());
    #else
        TiXmlPrinter Printer;
        XmlDoc.Accept(&Printer);
        return cbC2U(Printer.CStr());
    #endif
}
