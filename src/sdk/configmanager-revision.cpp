/* ------------------------------------------------------------------------------------------------------------------
*  Decouple the autorevisioning code, so we don't need to recompile the whole manager each time.
*  It's bad enough we have to re-link the SDK...
*/

#include "sdk_precomp.h" // contains "configmanager.h" and <wx/string.h>
#include "autorevision.h"

wxString ConfigManager::GetRevisionString()
{
    return wxString(autorevision::svnRevision);
}

unsigned int ConfigManager::GetRevisionNumber()
{
    return autorevision::svn_revision;
}

