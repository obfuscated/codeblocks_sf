#ifndef WXSWINDOWEDITOR_H
#define WXSWINDOWEDITOR_H

#include <wx/wx.h>

#include "wxsproject.h"
#include "wxseditor.h"

class wxsWidget;

class wxsWindowEditor : public wxsEditor
{
	public:
		wxsWindowEditor(wxMDIParentFrame* parent, const wxString& title,wxsResource* Resource);
		
		virtual ~wxsWindowEditor();
		
		/** Creating preview from given window */
		virtual void BuildPreview(wxsWidget* TopWidget);
		
		/** Killing previously created preview */
		virtual void KillCurrentPreview();
		
		/** Recreating current preview making everything look just like it will
		 *  be in real program
		 */
        inline void RecreatePreview() { BuildPreview(CurrentWidget); }
        
        /** Function notifying that there's need to recalculate current editor's
         *  sizers because given widget changed it's size
         */
        void PreviewReshaped();

    protected:
    
        /** Unbinding from resource - this function is killing preview objects */
        virtual void MyUnbind();

	private:
	
        wxScrolledWindow* DrawArea;
	
        void OnMouseClick(wxMouseEvent& event);
        void OnActivate(wxActivateEvent& event);
        void OnClose(wxCloseEvent& event);
        
        wxsWidget* CurrentWidget;
	
        DECLARE_EVENT_TABLE()
};


#endif // WXSWINDOWEDITOR_H
