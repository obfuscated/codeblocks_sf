#ifndef WXSRESOURCE_H
#define WXSRESOURCE_H

#include "wxseditor.h"
#include "wxsglobals.h"
#include <wx/treectrl.h>

class wxsProject;
class wxSmith;

class wxsResource
{
	public:
	
        /** Ctor */
		wxsResource(wxsProject* Project,int EditMode);
		
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
        
        /** Function returning currently openeditor, NULL if there's no one */
        inline wxsEditor* GetEditor() { return Editor; }
        
        /** Ckecking if this resource ecan be Previewed */
        virtual bool CanPreview() { return false; }
        
        /** This function should show preview in modal */
        virtual void ShowPreview() { }

        /** Getting resource name */
        virtual const wxString& GetResourceName() = 0;
        
        /** Notifying resource about content change */
        virtual void NotifyChange() { }
        
        /** Getting current edit mode */
        inline int GetEditMode() { return (int)EditMode & ~(int)wxsResBroken; }
        
        /** Setting Edit mode */
        inline void SetEditMode(int Mode) { EditMode = Mode; }

        /** Checking if this resource is broken */
        inline bool IsResBroken() { return (EditMode & wxsResBroken) != 0; }
        
        /** Changing broken flag */
        inline void SetResBroken(bool Broken) { EditMode = GetEditMode() | Broken ? wxsResBroken : 0; }
        
        /** Grtting current tree item in resource browser */
        inline wxTreeItemId GetTreeItemId() { return ItemId; }
		
    protected:
    
        /** Function which should create editor window.
         *
         *  Editor window shouldn't be created when call to this function is made
         */
        virtual wxsEditor* CreateEditor() = 0;
        
        /** Function notifying that editor has just been closed */
        virtual void EditorClosed() {}
        
        /** Function setting tree item in resource browser */
        inline void SetTreeItemId(wxTreeItemId Id) { ItemId = Id; }
        
    private:
    
        void EditorSaysHeIsClosing();
        friend class wxsEditor;
    
        wxsEditor* Editor;
        wxsProject* Project;
        int EditMode;
        wxTreeItemId ItemId;
};

#endif // WXSRESOURCE_H
