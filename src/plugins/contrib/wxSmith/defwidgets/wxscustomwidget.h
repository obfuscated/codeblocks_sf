#ifndef WXSCUSTOMWIDGET_H
#define WXSCUSTOMWIDGET_H

#include "../widget.h"

WXS_EV_DECLARE(wxsCustomWidgetEvents)

/** @brief Class for custom widgets
 *
 * Using custom widget class, User can add it's own widgets here.
 * Because widgets in XRC must be threated differently from the ones
 * created using source code, XRC will use additional xml configuration,
 * source code will must have creating code defined.
 */
class wxsCustomWidget: public wxsWidget
{
    public:
        wxsCustomWidget(wxsWidgetManager* Man,wxsWindowRes* Res);
        virtual ~wxsCustomWidget();
        virtual wxString GetProducingCode(wxsCodeParams& Params);
        virtual wxString GetDeclarationCode(wxsCodeParams& Params);
        virtual const wxsWidgetInfo& GetInfo();
        
    protected:
    
        wxString ClassName;
        wxString CreatingCode;
        wxString Style;
        wxString XmlData;
        
        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();
        virtual void CreateObjectProperties();
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
        
};

#endif // WXSCUSTOMWIDGET_H
