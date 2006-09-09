#include "wxsitemmanager.h"
#include "wxsitemfactory.h"
#include "wxsglobals.h"

bool wxsItemManager::RegisterInFactory()
{
    // Make sure factory exists
    wxsItemFactory::Create();
    
    // Checking if creation was successfull
    if ( !wxsFACTORY() ) return false;
    
    // Registering this manager
    wxsFACTORY()->RegisterManager(this);
    
    return true;
}
