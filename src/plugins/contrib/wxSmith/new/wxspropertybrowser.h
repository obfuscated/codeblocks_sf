#ifndef WXSPROPERTYBROWSER_H
#define WXSPROPERTYBROWSER_H

#include <wx/dynarray.h>

#include "properties/wxspropertygridmanager.h"

class wxsItem;
class wxsEvents;
class wxsEventDesc;
class wxsWindowRes;
class wxsProject;

/** \brief Main wxSmith property browser
 *
 * This class is an extension to wxsPropertyGridManager
 * with additional support for events.
 *
 * This class is like wxsPropertyGridManager, a singleton class
 * and must be declared once (not more, not less) in wxSmith
 * initialization to be usefull.
 */
class wxsPropertyBrowser : public wxsPropertyGridManager
{
    public:

        /** \brief Ctor */
        wxsPropertyBrowser(wxWindow* parent);

        /** \brief Dctor */
        virtual ~wxsPropertyBrowser();

    protected:

        /** \brief Function notifying about item change */
        virtual void OnContainerChanged(wxsPropertyContainer* NewContainer);

    private:

        WX_DEFINE_ARRAY(wxPGId,PGIdArray);

        wxsEvents* Events;
        wxsItem* Item;
        wxsWindowRes* Res;
        wxsProject* Proj;
        wxString SourceFile;
        wxString HeaderFile;
        wxString ClassName;
        PGIdArray EventIds;

        /** \brief Building proeprty grid for events */
        void BuildEvents();

        /** \brief Reading property grid for events */
        void ReadEvents();

        /** \brief Generating list of function names using specified type of argument */
        void FindFunctions(const wxString& EventType,wxArrayString& Array);

        /** \brief Getting name of new function name for event handler */
        wxString GetNewFunction(const wxsEventDesc* Event);

        /** \brief Getting proposition of function name for event handler */
        wxString GetFunctionProposition(const wxsEventDesc* Event);

        /** \brief Creating new function for event handling inside source code */
        bool CreateNewFunction(const wxsEventDesc* Event,const wxString& NewFunctionName);

        /** \brief Property change handler */
        void OnPropertyChanged(wxPropertyGridEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif
