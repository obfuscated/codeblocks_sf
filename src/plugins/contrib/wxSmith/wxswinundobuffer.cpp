#include "wxswinundobuffer.h"

#include "resources/wxswindowres.h"
#include "widget.h"
#include "wxswidgetfactory.h"
#include <sstream>

wxsWinUndoBuffer::wxsWinUndoBuffer(wxsWindowRes* _Resource,int _MaxEnteries):
    Resource(_Resource),
    CurrentPos(0),
    SavedPos(-1),
    MaxEnteries(_MaxEnteries)
{
}

wxsWinUndoBuffer::~wxsWinUndoBuffer()
{
    Clear();
}

void wxsWinUndoBuffer::Clear()
{
    for ( EnteriesI i = Enteries.begin(); i!=Enteries.end(); ++i )
    {
    	delete *i;
    }
    Enteries.clear();
}

void wxsWinUndoBuffer::StoreChange()
{
    wxsWidget* RootWidget = Resource->GetRootWidget();
    std::ostringstream buffer;
    
    TiXmlDocument Doc;
    TiXmlElement* Elem = Doc.InsertEndChild(TiXmlElement("object"))->ToElement();
    if ( !Elem ) return;
    Elem->SetAttribute("class",RootWidget->GetInfo().Name.mb_str());
    if ( !RootWidget->XmlSave(Elem) ) return;
    buffer << Doc;
    
    // Removing all undo points after current one
    
    int Size = GetCount();
    while ( --Size > CurrentPos )
    {
    	delete Enteries[Size];
    	Enteries.erase(Enteries.begin() + Size);
    }
    
    // Removing all outdated undos
    
    if ( MaxEnteries > 0 )
    {
        Size = GetCount();
        while ( Size-- > MaxEnteries-1 )
        {
            delete Enteries[0];
            Enteries.erase(Enteries.begin());
            SavedPos--;
        }
    }
    
    // Adding new undo
    
    UndoEntry* NewEntry = new UndoEntry;
    NewEntry->XmlData = wxString(buffer.str().c_str(),wxConvUTF8);
    NewEntry->XmlData.Shrink();
// TODO (SpOoN#1#): Add selection

    Enteries.push_back(NewEntry);
    CurrentPos = Enteries.size()-1;
}

wxsWidget *wxsWinUndoBuffer::Undo()
{
    if ( CurrentPos == 0 ) return NULL;
    CurrentPos--;
    return BuildResourceFromEntry(Enteries[CurrentPos]);
}

wxsWidget *wxsWinUndoBuffer::Redo()
{
    if ( CurrentPos >= GetCount() - 1 ) return NULL;
    CurrentPos++;
    return BuildResourceFromEntry(Enteries[CurrentPos]);
}

wxsWidget* wxsWinUndoBuffer::BuildResourceFromEntry(UndoEntry* Entry)
{
	std::istringstream buffer(std::string(Entry->XmlData.mb_str()));
	TiXmlDocument Doc;
	buffer >> Doc;
	
	TiXmlElement* Root = Doc.FirstChildElement("object");
	const char* Class = Root->Attribute("class");
	if ( !Class || !*Class ) return NULL;
	
	wxsWidget* RootWidget = wxsWidgetFactory::Get()->Generate(wxString(Class,wxConvUTF8),Resource);
	if ( !RootWidget ) return NULL;
	
	RootWidget->XmlLoad(Root);
	return RootWidget;
}
