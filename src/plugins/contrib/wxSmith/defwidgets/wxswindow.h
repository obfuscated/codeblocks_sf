#ifndef WXSWINDOW_H
#define WXSWINDOW_H

#include "../wxscontainer.h"

/** This class is a base for wxsDialog, wxsFrame and wxsPanel classes */
class wxsWindow : public wxsContainer
{
	public:
	
        /** Ctor */
		wxsWindow(wxsWidgetManager* Man,BasePropertiesType pType);
		
		/** Dctor */
		virtual ~wxsWindow();

   protected:
   
        /** This function should create preview window for resource.
         * All resources are drawn using wxPanel
         */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Function shich should update content of current widget */
        virtual void MyUpdatePreview();
};

#endif
