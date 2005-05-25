#ifndef __WXSBUTTON_H
#define __WXSBUTTON_H

#include "../widget.h"

WXS_ST_DECLARE(wxsButtonStyles)


class wxsButton: public wxsWidget
{
    public:
        
        /** Default constructor */
        wxsButton(wxsWidgetManager* Man):
            wxsWidget(Man,propWidget),
            Text("Button"),
            Default(false)
        {
        }
        
        /** Destructor */
        virtual ~wxsButton() ;
        
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual const char* GetProducingCode(wxsCodeParams& Params);
        
        /** Getting button's label */
        inline const wxString& GetLabel() { return Text; }
        
        /** Getting default flag */
        inline bool GetDefault() { return Default; }
        
   protected:
        /** This function should create preview window for widget */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
        /** Fucntion updating current preview */
        virtual void MyUpdatePreview();
        
        /** Loading xml data */
        virtual bool MyXmlLoad();
        
        /** Saving xml data */
        virtual bool MyXmlSave();
        
        /** Properties loading codee */
        virtual void CreateObjectProperties();
        
    private:
        /** Button's text */
        wxString Text;
        bool Default;
        
        friend class wxsButtonPreview;
};
        

#endif
