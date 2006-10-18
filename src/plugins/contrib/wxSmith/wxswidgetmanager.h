#ifndef WXSWIDGETMANAGER_H
#define WXSWIDGETMANAGER_H

#include <wx/bitmap.h>
#include "wxswidgetevents.h"
#include "wxsstyle.h"

class wxsWidget;
class wxsWindowRes;
class wxsWidgetManager;

/** Structure containing info about widget */
struct wxsWidgetInfo
{
    wxString Name;                  ///< Widget's name
    wxString License;               ///< Widget's license
    wxString Author;                ///< Widget's author
    wxString AuthorEmail;           ///< Widget's authos's email
    wxString AuthorSite;            ///< Widget's author's site
    wxString WidgetsSite;           ///< Site about this widget
    wxString Category;              ///< Widget's category
    wxString DefaultVarName;        ///< Prefix for default variable name
    bool Container;                 ///< True if this widget can have other widgets inside
    bool Sizer;                     ///< True if this widget is a sizer (Container must also be true)
    bool Spacer;                    ///< True if this is a spacer
    unsigned short VerHi;           ///< Lower number of version
    unsigned short VerLo;           ///< Higher number of version
    wxBitmap* Icon;                 ///< Icon used in palette (32x32 pixels)
    wxBitmap* Icon16;               ///< Icon used in palette (16x16 pixels)
    wxsWidgetManager* Manager;      ///< Manager handling this widget
    int Id;                         ///< Identifier used inside manager to handle this widget, must be same as 'Number' in GetWidgetInfo call
    int TreeIconId;                 ///< Identifier of image inside resource tree
    wxsStyle* Styles;               ///< Set of available styles, ending with NULL-named style
    wxsEventDesc *Events;           ///< Set of events supported by this widget, enging with NULL-named entry
    wxString HeaderFile;            ///< Header file (including '<' and '>' or '"') for this file
    wxString ExtHeaderFile;         ///< Additional header file

    /** Types of extended widgets */
    enum ExTypeT
    {
        exNone = 0,                 ///< This is widget from standard set (no new coded nor additional library needed)
        exCode,                     ///< This widget provides it's source code, currently not supported
        exLibrary                   ///< This widget is provided in additional library, currently not supported
    };

    ExTypeT ExType;                 ///< Type of extended widget
    wxString WidgetCodeDefinition;  ///< Code with definition of class for this widget
    wxString WidgetCodeDeclaration; ///< Code with declaration of class for this widget
    wxString WidgetLibrary;         ///< Library including this widget (empty for no library)
};



/** Class managing widget */
class wxsWidgetManager
{
    public:

        /** Ctor */
        wxsWidgetManager() {}

        /** Dctor */
        virtual ~wxsWidgetManager() {}

        /** Function initializing manager */
        virtual bool Initialize() { return true; }

        /** Function uninitializing manager */
        virtual bool Uninitialize() { return true; }

        /** Returns number of handled widgets */
        virtual int GetCount() = 0;

        /** Getting widget's info */
        virtual const wxsWidgetInfo* GetWidgetInfo(int Number) = 0;

        /** Getting new widget */
        virtual wxsWidget* ProduceWidget(int Id,wxsWindowRes* Res) = 0;

        /** Killing widget */
        virtual void KillWidget(wxsWidget* Widget) = 0;

        /** Fuunction registering this manager into main widget's factory */
        virtual bool RegisterInFactory();
};

#endif // WXSWIDGETMANAGER_H
