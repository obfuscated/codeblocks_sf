#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <wx/string.h>
#include <wx/confbase.h>
#include <wx/dynarray.h>
#include "settings.h"

struct ConfigurationPath
{
    wxString key; ///< The configuration key
    wxString desc; ///< The description
};
WX_DECLARE_OBJARRAY(ConfigurationPath, Configurations);

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
          * @param index The Configurations' index to export.
          * @return True if succesful, false if not.
          */
        static bool ExportToFile(const wxString& filename, int index);
        /** Export configuration to file.
          * This is an overloaded version provided for convenience.
          * @param filename The file to export.
          * @param configuration The configuration to export.
          * @return True if succesful, false if not.
          */
        static bool ExportToFile(const wxString& filename, const ConfigurationPath& configuration);
        /** Import configuration from file.
          * @param filename The file to import.
          * @param index The Configurations' index to import.
          * @return True if succesful, false if not.
          */
        static bool ImportFromFile(const wxString& filename, int index);
        /** Import configuration from file.
          * This is an overloaded version provided for convenience.
          * @param filename The file to import.
          * @param configuration The configuration to import.
          * @return True if succesful, false if not.
          */
        static bool ImportFromFile(const wxString& filename, const ConfigurationPath& configuration);
        /** Get the configurations array */
        static const Configurations& GetConfigurations(){ return s_Configurations; }
        /** Add a configuration in the configurations array */
        static void AddConfiguration(const wxString& desc, const wxString& key);
    private:
        ConfigManager();
        ~ConfigManager();
        static Configurations s_Configurations;
};

#endif // CONFIGMANAGER_H

