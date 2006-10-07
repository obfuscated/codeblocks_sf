#ifndef LIBRARYCONFIGMANAGER_H
#define LIBRARYCONFIGMANAGER_H

#include <tinyxml/tinyxml.h>
#include <wx/dynarray.h>

#include "libraryconfig.h"

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
        inline int GetLibraryCount() { return (int)Libraries.Count(); }

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

        bool CheckConfig(LibraryConfig* Cfg);

        WX_DEFINE_ARRAY(LibraryConfig*,LCArray);

        LCArray Libraries;

        static LibraryConfigManager Singleton;
};

#define LCM() LibraryConfigManager::Get()

#endif
