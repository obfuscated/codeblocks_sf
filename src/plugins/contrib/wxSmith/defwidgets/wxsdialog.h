#ifndef WXSDIALOG_H
#define WXSDIALOG_H

#include "../wxscontainer.h"


WXS_ST_DECLARE(wxsDialogStyles)


class wxsDialog : public wxsContainer
{
	public:
		wxsDialog(wxsWidgetManager* Man);
		
		virtual ~wxsDialog();

		
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual const char* GetProducingCode(wxsCodeParams& Params);
        
        /** Getting title */
        inline const wxString& GetTitle() { return Title; }
        
        /** Checking if it's centered */
        inline bool GetCentered() { return Centered; }

   protected:
        /** This function should create preview window for widget */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Loading data from XML tree */
        virtual bool MyXmlLoad(TiXmlElement* Element);

        /** Function shich should update content of current widget */
        virtual void MyUpdatePreview();
  
        /** Creating dedefault properties */
        void CreateObjectProperties();
  
  private:
  
        wxString Title;
        bool Centered;
};

#endif // WXSDIALOG_H
