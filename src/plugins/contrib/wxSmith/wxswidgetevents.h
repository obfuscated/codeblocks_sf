#ifndef WXSWIDGETEVENTS_H
#define WXSWIDGETEVENTS_H

#include <tinyxml/tinyxml.h>
#include <wx/string.h>
#include <vector>

#include "wxsglobals.h"

class wxsWidget;

/** Structure describing one event */
struct wxsEventDesc
{
    wxString EventEntry;        ///< Macro used inside event array
    wxString EventTypeName;     ///< Name of type of argument passed to event-processing function
    bool WithId;                ///< If true, event entry contains also identifier
    wxString NewFuncNameBase;   ///< Base for new function name, it will be created as "On" + VarName + NewFuncName
    wxString FunctionName;      ///< Name of function handling this event
};

/** Macro declaring list of events for widget */
#define WXS_EV_DECLARE(name)                                    \
    extern wxsEventDesc* name;

/** Beginning definition of events array (source file) */
#define WXS_EV_BEGIN(name)                                      \
    static wxsEventDesc __##name##__[] = {

/** Adding new event into list (entry without id) */
#define WXS_EV(entry,type,funcbase)                             \
    { _T(#entry), _T(#type), false, _T(#funcbase), _T("") } ,

/** Adding new event into list (entry with id) */
#define WXS_EVI(entry,type,funcbase)                            \
    { _T(#entry), _T(#type), true, _T(#funcbase), _T("") } ,

/** Ending creation of list */
#define WXS_EV_END(name)                                        \
    { _T(""), _T(""), false, _T(""), _T("") } };                \
    wxsEventDesc* name = __##name##__;

/** Beginning new  category */
#define WXS_EV_CATEGORY(name)                                   \
    { _T(""), _T(#name), false, _T(""), _T("") },

/** adding all default events */
#define WXS_EV_DEFAULTS()                                       \

/** This class is responsible for handling events for widget.
 *  Events are processed by root resource's class
 */
class wxsWidgetEvents
{
	public:

        /** Ctor */
		wxsWidgetEvents(wxsWidget* Wdg);

		/** Dctor */
		virtual ~wxsWidgetEvents();

        /** Getting number of handled events */
        inline int GetEventCount()
        {
        	return (int)Events.size();
        }

        /** Getting event deescription at given position */
        inline wxsEventDesc* GetEvent(int Pos)
        {
        	return ( Pos>=0 && Pos<GetEventCount() ) ? Events[Pos] : NULL;
        }

        /** Adding new event entry to list of events
         * \param Event which will be added
         * \param ClearFuncName - if true, FunctionName member of Event won't be copied
         * \return index of new event of -1 if error occured
         */
        int AddEvent(const wxsEventDesc& Event,bool ClearFuncName=true);

        /** Searching for event by function name */
        wxsEventDesc* GetEventByFunction(const wxString& Name);

        /** Searching for event by event array entry */
        wxsEventDesc* GetEventByEntry(const wxString& Entry);

        /** Function returning enteries in event table for this item */
        wxString GetArrayEnteries();

        /** Copying operator */
        wxsWidgetEvents& operator=(const wxsWidgetEvents& Source);

        /** Clearing all function names */
        void ClearFunctionNames();

        /** Function loading associated function names from Xml node. */
        void XmlLoadFunctions(TiXmlElement* Element);

        /** Function adding handlers to given Xml element */
        void XmlSaveFunctions(TiXmlElement* Element);

        /** Getting widget associated with these events */
        inline wxsWidget* GetWidget() { return Widget; }

	private:
        typedef std::vector<wxsEventDesc*> EventsT;
        typedef EventsT::iterator EventsI;
        typedef EventsT::const_iterator EventsCI;

        EventsT Events;
        wxsWidget* Widget;

        void ClearEvents();
};

#endif // WXSWIDGETEVENTS_H
