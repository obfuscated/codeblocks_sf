#ifndef WXSFRAME_H
#define WXSFRAME_H

#include "../wxscontainer.h"

class wxsFrame : public wxsContainer
{
	public:
		wxsFrame(wxsWidgetManager* Parent);
		
		virtual ~wxsFrame();
		
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual const char* GetProducingCode(wxsCodeParams& Params);

   protected:
        /** This function should create preview window for widget */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Function shich should update content of current widget */
        virtual void MyUpdatePreview();
  
  
        wxString ClassName;
};

#endif // WXSFRAME_H
