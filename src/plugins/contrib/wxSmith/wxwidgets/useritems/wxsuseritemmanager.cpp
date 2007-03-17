#include "wxsuseritemmanager.h"

wxsUserItemManager& wxsUserItemManager::Get()
{
    static wxsUserItemManager Singleton;
    return Singleton;
}


wxsUserItemManager::wxsUserItemManager()
{
    //ctor
}

wxsUserItemManager::~wxsUserItemManager()
{
    //dtor
}


