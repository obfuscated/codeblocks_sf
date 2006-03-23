#ifndef WXSEXTRESMANAGER_H
#define WXSEXTRESMANAGER_H

#include <wx/hashmap.h>
#include <wx/treectrl.h>

class wxsResource;

/** \brief Manager for external resources
 *
 * This class is responsible for editing resource files which are
 * not binded inside wxSmith project. Currently it's used to edit
 * XRC files externally.
 */
class wxsExtResManager
{
    public:

        /** \brief Opening external file */
        bool Open(const wxString& FileName);

        /** \brief Function notifying that editor has been closed */
        void ResClosed(wxsResource* Res);

        /** \brief Returning singleton object */
        static wxsExtResManager* Get() { return &Singleton; }

    private:

        /** \brief Opening external xrc file */
        bool OpenXrc(const wxString& FileName);

        /** \brief Ctor, private to forbid creating own instances */
        wxsExtResManager();

        /** \brief Dctor */
        ~wxsExtResManager();

        WX_DECLARE_STRING_HASH_MAP(wxsResource*,FilesMapT);
        typedef FilesMapT::iterator FilesMapI;

        FilesMapT Files;                        ///< \brief Map of opened files
        static wxsExtResManager Singleton;      ///< \brief Singleton object
};

#endif
