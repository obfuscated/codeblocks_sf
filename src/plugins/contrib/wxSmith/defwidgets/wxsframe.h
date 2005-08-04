#ifndef WXSFRAME_H
#define WXSFRAME_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsFrameStyles)

class wxsFrame : public wxsWindow
{
	public:
		wxsFrame(wxsWidgetManager* Man);
		
		virtual ~wxsFrame();
		
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Getting title */
        inline const wxString& GetTitle() { return Title; }
        
        /** Checking if it's centered */
        inline bool GetCentered() { return Centered; }

   protected:
  
        void CreateObjectProperties();

  private:
  
        wxString Title;
        bool Centered;
};

#endif
