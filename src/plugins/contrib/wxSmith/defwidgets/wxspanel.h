#ifndef WXSPANEL_H
#define WXSPANEL_H

#include "../wxscontainer.h"

class wxsPanel : public wxsContainer
{
	public:
	
		wxsPanel(wxsWidgetManager* Manager);
		
		virtual ~wxsPanel();
		
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual const char* GetProducingCode(wxsCodeParams& Params);
        
    protected:
        
        /** This function should create preview window for widget */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
    private:
};

#endif // WXSPANEL_H
