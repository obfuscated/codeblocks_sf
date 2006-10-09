#ifndef LIBRARYCONFIGMANAGER_H
#define LIBRARYCONFIGMANAGER_H

#include <wx/dynarray.h>

#include "libraryconfig.h"

class wxArrayString;
class wxString;
class TiXmlElement;

class LibraryConfigManager
{
    public:

        /** Ctor */
        LibraryConfigManager();

        /** Dctor */
        ~LibraryConfigManager();

        /** Getting singleton object */
        static LibraryConfigManager* Get() { return &Singleton; }

        /** Function loading xml configuration files from specified directory */
        void LoadXmlConfig(const wxString& Dir);

        /** Function returning number of loaded library configurations */
        inline int GetLibraryCount() const { return (int)Libraries.Count(); }

        /** Function returning configuration for one library */
        const LibraryConfig* GetLibrary(int Index);

        /** Function clearing current library set */
        void Clear();

    private:

        void LoadXmlFile(const wxString& Name);

        void LoadXmlDefaults(
            TiXmlElement* Elem,
            wxArrayString& Files,
            wxArrayString& Includes,
            wxArrayString& Libs,
            wxArrayString& Objs,
            wxString& CFlags,
            wxString& LFlags);

        bool CheckConfig(const LibraryConfig* Cfg) const;

        WX_DEFINE_ARRAY(LibraryConfig*,LCArray);

        LCArray Libraries;

        static LibraryConfigManager Singleton;
};

#define LCM() LibraryConfigManager::Get()

#endif
