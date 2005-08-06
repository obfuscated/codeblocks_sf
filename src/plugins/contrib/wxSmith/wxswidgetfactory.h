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
        virtual const wxsWidgetInfo* GetInfo(const wxString& Name);
        
        /** Creating widget with given name */
        virtual wxsWidget* Generate(const wxString& Name);
        
        /** Destroying given widget */
        virtual void Kill(wxsWidget* Widget);
        
        /** Getting first widget */
        virtual const wxsWidgetInfo* GetFirstInfo();
        
        /** Getting next info */
        virtual const wxsWidgetInfo* GetNextInfo();
        
        /** Registring manager inside this factory */
        virtual void RegisterManager(wxsWidgetManager* Manager);
        
	private:
	
        bool ValidateName(const wxString& Name);
	
        /* Structure comparing strings, used in map */
        struct ltstr
        {
            bool operator()(const wxString* s1, const wxString* s2) const
            {
                return *s1 < *s2;
            }
        };
        
        /** Map used to handle all types of widgets */
        typedef std::map<const wxString*,const wxsWidgetInfo*,ltstr> WidgetsMapT;
        
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
