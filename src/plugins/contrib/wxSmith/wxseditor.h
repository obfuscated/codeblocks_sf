#ifndef WXSEDITOR_H
#define WXSEDITOR_H

#include <editorbase.h>
#include "wxsevent.h"

class wxsResource;

class wxsEditor : public EditorBase
{
	public:
	
        /** Ctor */
		wxsEditor(wxWindow* parent, const wxString& title,wxsResource* Resource);
		
		/** Dctor */
		virtual ~wxsEditor();
		
		/** Getting current resouce */
		inline wxsResource* GetResource() { return Resource; }
		
		
    protected:
    
        /** This function should delete all dependencies between this window and
         *  resource object. F.ex. it could delete preview objects
         *
         *  NOTE: Can not use this function. This unbinding should be done
         *        inside deestrtuctor
         */
//        virtual void MyUnbind() = 0;
        
	private:
	
        /** Function unbinding from current resurce if any */
        void Unbind();
        
        /** Currently associated resource */
        wxsResource* Resource;
        
        /** Handler for all wxSmith events
         *
         * All events must be processed here or inside derived editor's class.
         * Skipping these events will cause stack overflow (events are thrown back
         * into wxSmith plugin)
         */
        void OnSmithEvent(wxsEvent& event);
        
        DECLARE_EVENT_TABLE()
};

#endif // WXSEDITOR_H
