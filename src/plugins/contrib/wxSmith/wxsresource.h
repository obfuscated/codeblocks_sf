#ifndef WXSRESOURCE_H
#define WXSRESOURCE_H

#include "wxseditor.h"

class wxSmith;

class wxsResource
{
	public:
	
        /** Ctor */
		wxsResource(wxSmith* Plugin);
		
		/** Dctor */
		virtual ~wxsResource();
		
		/** Getting current wxSmith plugin */
		inline wxSmith* GetPlugin() { return Plugin; }
		
        /** Function opening this resource in eeditor window,
         *  if editor window already exists, it must be activated
         */
        void EditOpen();
        
        /** Function closing editor window, if window is closed, no action
         *  should be performed
         */
        void EditClose();
        
    protected:
    
        /** Function shich should create editor window.
         *
         *  Editor window shouldn't be created when call to this function is made
         */
        virtual wxsEditor* CreateEditor() = 0;
		
    private:
    
        void EditorSaysHeIsClosing();
        friend class wxsEditor;
    
        wxsEditor* Editor;
        wxSmith* Plugin;
};

#endif // WXSRESOURCE_H
