#include "wxsheaders.h"
#include "wxswidgetmanager.h"
#include "wxswidgetfactory.h"

bool wxsWidgetManager::RegisterInFactory()
{
    if ( !wxsFACTORY() ) return false;
    wxsFACTORY()->RegisterManager(this);
    return true;
}
