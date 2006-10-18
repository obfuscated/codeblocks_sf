#ifndef WXSEXTRESMANAGER_H
#define WXSEXTRESMANAGER_H

#include <wx/hashmap.h>
#include <wx/treectrl.h>

class wxsResource;

/** Manager for external resources
 *
 * This class manages resources opened externally (which are edited
 * wxsREMFile mode - no coded is generated for them). */
class wxsExtResManager
{
    public:

        /** Ctor */
        wxsExtResManager();

        /** Dctor */
        ~wxsExtResManager();

        /** Opening external xrc file */
        int OpenXrc(const wxString& FileName);

        /** Function notifying that editor has been closed */
        void ResClosed(wxsResource* Res);

        /** Returning singleton object */
        static wxsExtResManager* Get() { return Singleton; }

    private:

        WX_DECLARE_STRING_HASH_MAP(wxsResource*,FilesMapT);
        typedef FilesMapT::iterator FilesMapI;

        wxTreeItemId TreeId;

        FilesMapT Files;

        static wxsExtResManager* Singleton;
};

#endif // WXSEXTRESMANAGER_H
