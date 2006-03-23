#include "wxswindowresdataobject.h"
#include "../wxsitem.h"
#include "../wxsitemfactory.h"
#include <messagemanager.h>
#include <sstream>

wxsWindowResDataObject::wxsWindowResDataObject(): ItemCount(0)
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

bool wxsWindowResDataObject::GetDataHere(const wxDataFormat& format,void *buf) const
{
	wxString XmlData = GetXmlData();
	wxWX2MBbuf str = cbU2C(XmlData);
    memcpy(buf,str,strlen(str)+1);
   	return true;
}

size_t wxsWindowResDataObject::GetDataSize(const wxDataFormat& format) const
{
	wxString XmlData = GetXmlData();
	wxWX2MBbuf str = cbU2C(XmlData);
    return strlen(str)+1;
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

    // Just to make sure we won't go out of buffer
    CharBuff[len] = '\0';

    bool Ret = SetXmlData(cbC2U(CharBuff));
    delete[] CharBuff;
    return Ret;
}

void wxsWindowResDataObject::Clear()
{
	XmlDoc.Clear();
	XmlElem = XmlDoc.InsertEndChild(TiXmlElement("resource"))->ToElement();
	ItemCount = 0;
}

bool wxsWindowResDataObject::AddItem(wxsItem* Item)
{
	if ( !Item ) return false;
    TiXmlElement* Elem = XmlElem->InsertEndChild(TiXmlElement("object"))->ToElement();
    if ( !Elem ) return false;
    Elem->SetAttribute("class",cbU2C(Item->GetInfo().Name));
    if ( !Item->XmlWrite(Elem,true,true) )
    {
        XmlElem->RemoveChild(Elem);
        return false;
    }
	ItemCount++;
	return true;
}

int wxsWindowResDataObject::GetItemCount() const
{
	return ItemCount;
}

wxsItem* wxsWindowResDataObject::BuildItem(wxsWindowRes* Resource,int Index) const
{
	if ( Index < 0 || Index >= ItemCount ) return NULL;

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

	wxsItem* Item = wxsGEN(cbC2U(Class),Resource);
	if ( !Item )
	{
        Item = wxsGEN(_T("Custom"),Resource);
        if ( !Item ) return false;
	}

	Item->XmlRead(Root,true,true);
	return Item;
}

bool wxsWindowResDataObject::SetXmlData(const wxString& Data)
{
    XmlDoc.Clear();
    ItemCount = 0;
	XmlDoc.Parse(_C(Data));
    if ( XmlDoc.Error() )
    {
        DBGLOG(_T("wxSmith: Error loading Xml data -> ") + _U(XmlDoc.ErrorDesc()));
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
    	ItemCount++;
    }

    return true;
}

wxString wxsWindowResDataObject::GetXmlData() const
{
    #ifdef TIXML_USE_STL
        std::ostringstream buffer;
        buffer << XmlDoc;
        return _U(buffer.str().c_str());
    #else
        TiXmlOutStream buffer;
        buffer << XmlDoc;
        return _U(buffer.c_str());
    #endif
}
