#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <wx/string.h>
#include <wx/confbase.h>
#include "settings.h"

/**
 * ConfigManager is the class to use for accessing Code::Blocks' configuration
 * file (or registry settings under Win32).
 */
class DLLIMPORT ConfigManager
{
    public:
		/** Initialize ConfigManager */
		/** Use the parameter \c config to pass your application's wxConfigBase
		  * object (). After initialization, each time you use Get(), the same
		  * wxConfigBase pointer will be returned. <b>This method is provided
		  * <em>only</em> for people building their own IDE using Code::Blocks
		  * library. Plugin authors <em>must not</em> use this method...</b>
		  */
        static void Init(wxConfigBase* config);
        static wxConfigBase* Get(); /**< Get the wxConfigBase pointer of the SDK */
        /** Export configuration to file.
          * @param filename The file to export.
          * @param topLevel Set the top-level virtual-path for the export (defaults to "/").
          * @return True if succesful, false if not.
          */
        static bool ExportToFile(const wxString& filename, const wxString& topLevel = "/");
        /** Import configuration from file.
          * @param filename The file to import.
          * @param topLevel Set the top-level virtual-path for the import (defaults to "/").
          * @return True if succesful, false if not.
          */
        static bool ImportFromFile(const wxString& filename, const wxString& topLevel = "/");
    private:
        ConfigManager();
        ~ConfigManager();
};

#endif // CONFIGMANAGER_H

