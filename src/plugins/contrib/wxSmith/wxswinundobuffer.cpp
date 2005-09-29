#include "wxswinundobuffer.h"

#include "resources/wxswindowres.h"
#include "resources/wxswindowresdataobject.h"
#include "widget.h"
#include "wxswidgetfactory.h"

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
    
	wxsWindowResDataObject Object;
	Object.MakeFromWidget(Resource->GetRootWidget());
    UndoEntry* NewEntry = new UndoEntry;
    NewEntry->XmlData = Object.GetXmlData();
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
	wxsWindowResDataObject Object;
	Object.SetXmlData(Entry->XmlData);
	return Object.BuildWidget(Resource);
}
