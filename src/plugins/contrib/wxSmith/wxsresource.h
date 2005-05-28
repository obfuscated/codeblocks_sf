#ifndef WXSRESOURCE_H
#define WXSRESOURCE_H

#include "wxseditor.h"

class wxsProject;
class wxSmith;

class wxsResource
{
	public:
	
        /** Ctor */
		wxsResource(wxsProject* Project);
		
		/** Dctor */
		virtual ~wxsResource();
		
		/** Getting current project */
		inline wxsProject* GetProject() { return Project; }
		
		/** Getting current wxSmith plugin */
		wxSmith* GetPlugin();
		
        /** Function opening this resource in eeditor window,
         *  if editor window already exists, it must be activated
         */
        void EditOpen();
        
        /** Function closing editor window, if window is closed, no action
         *  should be performed
         */
        void EditClose();
        
        /** Ckecking if this resource ecan be Previewed */
        virtual bool CanPreview() { return false; }
        
        /** This function should show preview in modal */
        virtual void ShowPreview() { }

        /** Getting resource name */
        virtual const wxString& GetResourceName() = 0;
        
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
        wxsProject* Project;
};

#endif // WXSRESOURCE_H
