#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <wx/confbase.h>
#include "settings.h"

/**
 * ConfigManager is the class to use for accessing Code::Studio's configuration
 * file (or registry settings under Win32).
 */
class DLLIMPORT ConfigManager
{
    public:
		/** Initialize ConfigManager */
		/** Use the parameter \c config to pass your application's wxConfigBase
		  * object (). After initialization, each time you use Get(), the same
		  * wxConfigBase pointer will be returned. <b>This method is provided
		  * <em>only</em> for people building their own IDE using Code::Studio
		  * library. Plugin authors <em>must not</em> use this method...</b>
		  */
        static void Init(wxConfigBase* config);
        static wxConfigBase* Get(); /**< Get the wxConfigBase pointer of the SDK */
    private:
        ConfigManager();
        ~ConfigManager();
};

#endif // CONFIGMANAGER_H

