#ifndef WXSWINDOW_H
#define WXSWINDOW_H

#include "../wxscontainer.h"
#include "../wxswidgetevents.h"

/** This class is a base for wxsDialog, wxsFrame and wxsPanel classes */
class WXSCLASS wxsWindow : public wxsContainer
{
	public:

        /** Ctor */
		wxsWindow(wxsWidgetManager* Man,wxsWindowRes* Res, wxsBasePropertiesType pType=propNone);

		/** Dctor */
		virtual ~wxsWindow();

		/** Checking if can add new child */
		virtual bool CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis);

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
