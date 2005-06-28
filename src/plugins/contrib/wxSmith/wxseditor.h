#ifndef WXSEDITOR_H
#define WXSEDITOR_H

#include <settings.h>
#include <editorbase.h>

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
		
		/** Default closing action */
		virtual bool Close();
		
    protected:
    
        /** This function should delete all dependencies between this window and
         *  resource object. F.ex. it could delete preview objects
         */
        virtual void MyUnbind() = 0;
        
	private:
	
        /** Function unbinding from current resurce if any */
        void Unbind();
        
        /** Currently associated resource */
        wxsResource* Resource;
};

#endif // WXSEDITOR_H
