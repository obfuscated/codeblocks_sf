#ifndef WXSWINDOW_H
#define WXSWINDOW_H

#include "../wxscontainer.h"

/** This class is a base for wxsDialog, wxsFrame and wxsPanel classes */
class wxsWindow : public wxsContainer
{
	public:
	
        /** Ctor */
		wxsWindow(wxsWidgetManager* Man,wxsWindowRes* Res, BasePropertiesType pType);
		
		/** Dctor */
		virtual ~wxsWindow();
		
		/** Own handler for adding children */
		virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis);

   protected:
   
        /** This function should create preview window for resource.
         * All resources are drawn using wxPanel
         */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Function shich should update content of current widget */
        virtual void MyFinalUpdatePreview(wxWindow* Preview);
};

#endif
