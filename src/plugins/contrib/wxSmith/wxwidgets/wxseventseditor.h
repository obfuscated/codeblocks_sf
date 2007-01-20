#ifndef WXSEVENTSEDITOR_H
#define WXSEVENTSEDITOR_H

#include "../properties/wxsproperties.h"
#include "../wxscodinglang.h"

class wxsItem;
class wxsEvents;
class wxsEventDesc;
class wxsItemResData;

/** \brief This class is responsible for showing, managing and editing events in property grid */
class wxsEventsEditor
{
    public:

        /** \brief Ctor */
        wxsEventsEditor();

        /** \brief Dctor */
        ~wxsEventsEditor();

        /** \brief Getting singleton object */
        static wxsEventsEditor& Get();

        /** \brief Function adding events from given item to grid */
        void BuildEvents(wxsItem* Item,wxsPropertyGridManager* Grid);

        /** \brief Function called when content inside grid has been changed */
        void PGChanged(wxsItem* Item,wxsPropertyGridManager* Grid,wxPGId Id);

	private:

        WX_DEFINE_ARRAY(wxPGId,wxArrayPGId);

        wxsItemResData* m_Data;
        wxsItem* m_Item;
        wxsEvents* m_Events;
        wxString m_Source;
        wxString m_Header;
        wxString m_Class;
        wxsCodingLang m_Language;
        wxArrayPGId m_Ids;

        /** \brief Finding all handlers with given argument type */
        void FindFunctions(const wxString& ArgType,wxArrayString& Array);

        /** \brief Gerring input from user with new function name */
        wxString GetNewFunction(const wxsEventDesc* Event);

        /** \brief Generating proposition for new function name */
        wxString GetFunctionProposition(const wxsEventDesc* Event);

        /** \brief Creating new function inside source code */
        bool CreateNewFunction(const wxsEventDesc* Event,const wxString& NewFunctionName);
};

#endif
