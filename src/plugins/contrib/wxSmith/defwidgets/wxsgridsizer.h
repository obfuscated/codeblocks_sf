#ifndef WXSGRIDSIZER_H
#define WXSGRIDSIZER_H

#include "../wxscontainer.h"

class wxsGridSizerPreview;
class wxsGridSizer : public wxsContainer

{
	public:
	
		wxsGridSizer(wxsWidgetManager* Man);
		
		virtual ~wxsGridSizer();
		
		
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual wxString GetProducingCode(wxsCodeParams& Params);

        /** Function generating code finalizing widget's creation process */
        virtual wxString GetFinalizingCode(wxsCodeParams& Params);
        
        /** Generating sizer's declaration */
		virtual wxString GetDeclarationCode(wxsCodeParams& Params);
        
    protected:
    
        /** Loading content */
        virtual bool MyXmlLoad();
        
        /** Loading content */
        virtual bool MyXmlSave();
        
        /** This function should create preview window for widget */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Updating preview */
        virtual void MyUpdatePreview();
        
        /** Properties loading codee */
        virtual void CreateObjectProperties();
        
	private:
	
        int Cols;
        int Rows;
        int VGap;
        int HGap;
        
        friend class wxsGridSizerPreview;

};

#endif
