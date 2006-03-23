#include "wxsitemfactory.h"
#include "wxsitemmanager.h"
#include "wxsitem.h"

wxsItemFactory* wxsItemFactory::Singleton = NULL;

wxsItemFactory::wxsItemFactory()
{
}

wxsItemFactory::~wxsItemFactory()
{
}

void wxsItemFactory::Create()
{
    // Factory will be stored as static member of Create function, so
    // it will be created at first call to Create and deleted when
    // plugin will be unloaded.
    static wxsItemFactory Factory;

    Singleton = &Factory;
}

const wxsItemInfo* wxsItemFactory::GetInfo(const wxString& Name)
{
    ItemMapI i = Items.find(Name);
    if ( i == Items.end() ) return NULL;
    return (*i).second.Info;
}

wxsItem* wxsItemFactory::Generate(const wxString& Name,wxsWindowRes* Res)
{
    // Searching for entry for this item name
    ItemMapI i = Items.find(Name);
    if ( i == Items.end() ) return NULL;

    // Generating item
    wxsItem* Item = i->second.Manager->ProduceItem(i->second.Number,Res);

    // Second step of initialization
    if ( Item ) Item->Create();

    return Item;
}

void wxsItemFactory::Kill(wxsItem* Item)
{
    if ( Item )
    {
        delete Item;
    }
}

const wxsItemInfo* wxsItemFactory::GetFirstInfo()
{
    Iterator = Items.begin();
    return Iterator == Items.end() ? NULL : Iterator->second.Info;
}

const wxsItemInfo* wxsItemFactory::GetNextInfo()
{
    if ( Iterator == Items.end() ) return NULL;
    if ( ++Iterator == Items.end() ) return NULL;
    return Iterator->second.Info;
}

void wxsItemFactory::RegisterManager(wxsItemManager* Manager)
{
    if ( !Manager )
    {
        return;
    }

    int Count = Manager->GetCount();
    for ( int i = 0; i<Count; i++ )
    {
        const wxsItemInfo* Info = &Manager->GetItemInfo(i);
        if ( Info && wxsValidateIdentifier(Info->Name) )
        {
            // TODO: Check if item does exist and add higner version
            //       only
            ItemData &Data = Items[Info->Name];
            Data.Info = Info;
            Data.Number = i;
            Data.Manager = Manager;
        }
    }
}

