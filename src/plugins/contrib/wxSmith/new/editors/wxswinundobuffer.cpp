#include "wxswinundobuffer.h"

#include "../resources/wxswindowres.h"

wxsWinUndoBuffer::wxsWinUndoBuffer(wxsWindowRes* _Resource,int _MaxEnteries):
    Resource(_Resource),
    CurrentPos(0),
    SavedPos(0),
    MaxEnteries(_MaxEnteries)
{
    // Saving current resource data
    StoreChange();
    SavedPos = CurrentPos;
}

wxsWinUndoBuffer::~wxsWinUndoBuffer()
{
    Clear();
}

void wxsWinUndoBuffer::Clear()
{
    Enteries.clear();
    CurrentPos = 0;
    SavedPos = 0;
}

void wxsWinUndoBuffer::StoreChange()
{
    // Removing all undo points after current one
    if ( CurrentPos < GetCount()-1 )
    {
        Enteries.RemoveAt(CurrentPos+1,GetCount()-CurrentPos-1);
    }

    // Removing all outdated undos
    if ( MaxEnteries > 0 )
    {
        int ToRemove = GetCount()-MaxEnteries;
        if ( ToRemove > 0 )
        {
            Enteries.RemoveAt(0,ToRemove);
            CurrentPos -= ToRemove;
            SavedPos -= ToRemove;
        }
    }

    // Adding new undo
    Enteries.Add(Resource->GetXmlData());

    // TODO (SpOoN#1#): Add selection

    CurrentPos = GetCount()-1;
}

bool wxsWinUndoBuffer::Undo()
{
    if ( CurrentPos == 0 ) return false;
    return Resource->SetXmlData(Enteries[--CurrentPos]);
}

bool wxsWinUndoBuffer::Redo()
{
    if ( CurrentPos >= GetCount() - 1 ) return false;
    return Resource->SetXmlData(Enteries[++CurrentPos]);
}
