#ifndef WXSDIALOG_H
#define WXSDIALOG_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsDialogStyles)

class wxsDialog : public wxsWindow
{
	public:
		wxsDialog(wxsWidgetManager* Man,wxsWindowRes* Res);
		
		virtual ~wxsDialog();
		
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
