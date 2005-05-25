#ifndef __WXSSTATICTEXT_H
#define __WXSSTATICTEXT_H

#include "../widget.h"

WXS_ST_DECLARE(wxsStaticTextStyles)


class wxsStaticText: public wxsWidget
{
    public:
        
        /** Default constructor */
        wxsStaticText(wxsWidgetManager* Man): wxsWidget(Man,propWidget), Text("StaticText")
        {
            PropertiesObject.AddProperty("Label:",Text,0);
        }
        
        /** Destructor */
        virtual ~wxsStaticText() ;
        
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual const char* GetProducingCode(wxsCodeParams& Params);
        
        /** Getting text's label */
        inline const wxString& GetLabel() { return Text; }
        
   protected:
        /** This function should create preview window for widget */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Fucntion updating current preview */
        virtual void MyUpdatePreview();
        
        /** Loading xml data */
        virtual bool MyXmlLoad();
        
        /** Saving xml data */
        virtual bool MyXmlSave();
        
        
    private:
        /** Button's text */
        wxString Text;
        
        friend class wxsStaticTextPreview;
};
        

#endif
