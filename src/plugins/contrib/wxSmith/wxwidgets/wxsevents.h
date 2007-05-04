#ifndef WXSEVENTS_H
#define WXSEVENTS_H

#include <wx/arrstr.h>
#include <tinyxml/tinyxml.h>
#include <wx/arrstr.h>

#include "../wxscodinglang.h"

// Forward declarations
class wxsItem;

/** \brief Structure describing one event */
struct wxsEventDesc
{
    enum EntryType
    {
        Id,                     ///< \brief Event using one identifier
        IdRange,                ///< \brief Event using range of identifiers
        NoId,                   ///< \brief Event without id (can be used for root element only)
        Category,               ///< \brief Not really entry, but starts new category
        EndOfList               ///< \brief End of list of events
    };

    EntryType ET;               ///< \brief Type of entry
    wxString Entry;             ///< \brief Name of entry inside event table
    wxString Type;              ///< \brief Macro of event type used inside event table
    wxString ArgType;           ///< \brief Name of type of argument passed to event-processing function
    wxString NewFuncNameBase;   ///< \brief Base for new function name, it will be created as "On" + VarName + NewFuncName
};

/** \brief Beginning definition of events array (source file) */
#define WXS_EV_BEGIN(name)                                          \
    static wxsEventDesc name[] = {

/** \brief Adding new event into list (entry without id, will be connected to widget using wxsEvtHandler::Connect function) */
#define WXS_EV(entry,type,arg,funcbase)                             \
    { wxsEventDesc::NoId, _T(#entry), _T(#type), _T(#arg), _T(#funcbase) } ,

/** \brief Adding new event into list (entry with id, standard one) */
#define WXS_EVI(entry,type,arg,funcbase)                            \
    { wxsEventDesc::Id, _T(#entry), _T(#type), _T(#arg), _T(#funcbase) } ,

/** \brief Adding new event into list (entry using range of ids, currently not supported by wxSmith, but may be in future) */
#define WXS_EV2I(entry,type,arg,funcbase)                            \
    { wxsEventDesc::IdRange, _T(#entry), _T(#type), _T(#arg), _T(#funcbase) } ,

/** \brief Beginning new  category */
#define WXS_EV_CATEGORY(name)                                       \
    { wxsEventDesc::Category, name, _T(""), _T(""), _T("") },

/** \brief Ending creation of list */
#define WXS_EV_END()                                                \
    { wxsEventDesc::EndOfList, _T(""), _T(""), _T(""), _T("") } };

/** \brief Adding all default events */
#define WXS_EV_DEFAULTS()                                                               \
                                                                                        \
    WXS_EV_CATEGORY(_("Paint events"))                                                  \
    WXS_EV(EVT_PAINT,wxEVT_PAINT,wxPaintEvent,Paint)                                    \
    WXS_EV(EVT_ERASE_BACKGROUND,wxEVT_ERASE_BACKGROUND,wxEraseEvent,EraseBackground)    \
                                                                                        \
    WXS_EV_CATEGORY(_("Keyboard events"))                                               \
    WXS_EV(EVT_KEY_DOWN,wxEVT_KEY_DOWN,wxKeyEvent,KeyDown)                              \
    WXS_EV(EVT_KEY_UP,wxEVT_KEY_UP,wxKeyEvent,KeyUp)                                    \
    WXS_EV(EVT_CHAR,wxEVT_CHAR,wxKeyEvent,Char)                                         \
    WXS_EV(EVT_SET_FOCUS,wxEVT_SET_FOCUS,wxFocusEvent,SetFocus)                         \
    WXS_EV(EVT_KILL_FOCUS,wxEVT_KILL_FOCUS,wxFocusEvent,KillFocus)                      \
                                                                                        \
    WXS_EV_CATEGORY(_T("Mouse events"))                                                 \
    WXS_EV(EVT_LEFT_DOWN,wxEVT_LEFT_DOWN,wxMouseEvent,LeftDown)                         \
    WXS_EV(EVT_LEFT_UP,wxEVT_LEFT_UP,wxMouseEvent,LeftUp)                               \
    WXS_EV(EVT_LEFT_DCLICK,wxEVT_LEFT_DCLICK,wxMouseEvent,LeftDClick)                   \
    WXS_EV(EVT_MIDDLE_DOWN,wxEVT_MIDDLE_DOWN,wxMouseEvent,MiddleDown)                   \
    WXS_EV(EVT_MIDDLE_UP,wxEVT_MIDDLE_UP,wxMouseEvent,MiddleUp)                         \
    WXS_EV(EVT_MIDDLE_DCLICK,wxEVT_MIDDLE_DCLICK,wxMouseEvent,MiddleDClick)             \
    WXS_EV(EVT_RIGHT_DOWN,wxEVT_RIGHT_DOWN,wxMouseEvent,RightDown)                      \
    WXS_EV(EVT_RIGHT_UP,wxEVT_RIGHT_UP,wxMouseEvent,RightUp)                            \
    WXS_EV(EVT_RIGHT_DCLICK,wxEVT_RIGHT_DCLICK,wxMouseEvent,RightDClick)                \
    WXS_EV(EVT_MOTION,wxEVT_MOTION,wxMouseEvent,MouseMove)                              \
    WXS_EV(EVT_ENTER_WINDOW,wxEVT_ENTER_WINDOW,wxMouseEvent,MouseEnter)                 \
    WXS_EV(EVT_LEAVE_WINDOW,wxEVT_LEAVE_WINDOW,wxMouseEvent,MouseLeave)                 \
    WXS_EV(EVT_MOUSEWHEEL,wxEVT_MOUSEWHEEL,wxMouseEvent,MouseWheel)                     \
    WXS_EV(EVT_SET_CURSOR,wxEVT_SET_CURSOR,wxSetCursorEvent,SetCursor)                  \


/** \brief Class managing events used by item
 *
 * This class manages event used by widget (but it's not responsible for editing
 * them).
 *
 * After building new wxsEvents class, SetEventArray() should be called to
 * connect clas with specified set of events
 *
 */
class wxsEvents
{
	public:

        /** \brief Ctor */
		wxsEvents(const wxsEventDesc* Events,wxsItem* Item);

		/** \brief Getting number of events */
		inline int GetCount() { return m_Count; }

		/** \brief Getting event description */
		inline const wxsEventDesc* GetDesc(int Index) { return &m_EventArray[Index]; }

		/** \brief Getting event handler name */
		inline const wxString& GetHandler(int Index) { return m_Functions[Index]; }

		/** \brief Setting event handler name */
		inline void SetHandler(int Index,const wxString& Name) { m_Functions[Index] = Name; }

		/** \brief Function genrating code which binds events with main resource class
		 *
		 * Connecting events is done through wxsEvtHandler::Connect function.
		 * Event table is not used because not all events could be processed.
		 *
		 * \param Code Reference to string with code. New code will be appended here.
		 * \param UsingXrc if true,
		 * \param Language language of generated code
		 */
        void GenerateBindingCode(wxString& Code,const wxString& IdString,const wxString& VarNameString,bool IsRoot,wxsCodingLang Language);

        /** \brief Function loading associated function names from Xml node. */
        void XmlLoadFunctions(TiXmlElement* Element);

        /** \brief Function adding handlers to given Xml element */
        void XmlSaveFunctions(TiXmlElement* Element);

        /** \brief Function checking if this item must have variable declared
         *
         * Some event handlers are added child item, not to root one. These
         * widgets MUST be accessed through at least local variable (which is
         * not always done when loading resources from XRC file).
         */
        bool ForceVariable();

	private:

        wxsItem* m_Item;                      ///< Item whose events are managed
        const wxsEventDesc* m_EventArray;     ///< Array of events fetched from item
        wxArrayString m_Functions;            ///< Array of function names used for each entry item
        int m_Count;                          ///< Number of events
};

#endif
