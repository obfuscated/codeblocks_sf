#ifndef WXSDIALOG_H
#define WXSDIALOG_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsDialogStyles)
WXS_EV_DECLARE(wxsDialogEvents)

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
        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();
    
    private:
  
        wxString Title;
        bool Centered;
        
        friend class wxsDialogRes;
};

#endif
