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
        virtual const char* GetProducingCode(wxsCodeParams& Params);

        /** Function generating code finalizing widget's creation process */
        virtual const char* GetFinalizingCode(wxsCodeParams& Params);
        
    protected:
    
        /** Loading content */
        virtual bool MyXmlLoad(TiXmlElement* Element);
        
        /** This function should create preview window for widget */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Updating preview */
        virtual void MyUpdatePreview();
        
	private:
	
        int Cols;
        int Rows;
        int VGap;
        int HGap;
        
        friend class wxsGridSizerPreview;

};

#endif
