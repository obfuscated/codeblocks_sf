#ifndef WXSPANEL_H
#define WXSPANEL_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsPanelStyles)
WXS_EV_DECLARE(wxsPanelEvents)

/** Standard panel - it may be used as a widget */
class wxsPanel : public wxsWindow
{
	public:
		wxsPanel(wxsWidgetManager* Man,wxsWindowRes* Res);
		
		virtual ~wxsPanel();
		
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual wxString GetProducingCode(wxsCodeParams& Params);
        
        /** Function generating code which generates variable containing this widget. */
        virtual wxString GetDeclarationCode(wxsCodeParams& Params);
};

WXS_ST_DECLARE(wxsPanelrStyles)
WXS_EV_DECLARE(wxsPanelrEvents)

/** Resource panel - may be used as a resourcec */
class wxsPanelr : public wxsWindow
{
	public:
		wxsPanelr(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsPanelr();
        virtual const wxsWidgetInfo& GetInfo();
};

#endif // WXSPANEL_H
