#ifndef WXSWIDGETFACTORY_H
#define WXSWIDGETFACTORY_H

#include <map>
#include "widget.h"

class wxsWidgetFactory
{
	public:
	
        /** Ctor */
		wxsWidgetFactory();
		
		/** Dctor */
		virtual ~wxsWidgetFactory();
	
        /** Getting singleton object */
        static inline wxsWidgetFactory* Get() { return Singleton; }
		
        /** Returning info for widget with given name */
        virtual const wxsWidgetInfo* GetInfo(const char* Name);
        
        /** Creating widget with given name */
        virtual wxsWidget* Generate(const char* Name);
        
        /** Destroying given widget */
        virtual void Kill(wxsWidget* Widget);
        
        /** Getting first widget */
        virtual const wxsWidgetInfo* GetFirstInfo();
        
        /** Getting next info */
        virtual const wxsWidgetInfo* GetNextInfo();
        
        /** Registring manager inside this factory */
        virtual void RegisterManager(wxsWidgetManager* Manager);
        
	private:
	
        /* Structure comparing strings, used in map */
        struct ltstr
        {
            bool operator()(const char* s1, const char* s2) const
            {
                return strcmp(s1, s2) < 0;
            }
        };
        
        /** Map used to handle all types of widgets */
        typedef std::map<const char*,const wxsWidgetInfo*,ltstr> WidgetsMapT;
        
        /** Iterator for map */
        typedef WidgetsMapT::iterator WidgetsMapI;
        
        /** Map for widgets */
        WidgetsMapT Widgets;
        
        /** Internal iterator */
        WidgetsMapI Iterator;
        
        /* Singleton object */
        static wxsWidgetFactory* Singleton;
};

#endif // WXSWIDGETFACTORY_H
