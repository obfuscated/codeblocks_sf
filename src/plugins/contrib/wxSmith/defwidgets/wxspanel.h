#ifndef WXSPANEL_H
#define WXSPANEL_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsPanelStyles)

/** Standard panel - it may be used as a widget */
class wxsPanel : public wxsWindow
{
	public:
		wxsPanel(wxsWidgetManager* Man);
		
		virtual ~wxsPanel();
		
        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();
        
        /** Function generating code which should produce widget */
        virtual wxString GetProducingCode(wxsCodeParams& Params);
};

WXS_ST_DECLARE(wxsPanelrStyles)

/** Resource panel - may be used as a resourcec */
class wxsPanelr : public wxsWindow
{
	public:
		wxsPanelr(wxsWidgetManager* Man);
		virtual ~wxsPanelr();
        virtual const wxsWidgetInfo& GetInfo();
};

#endif // WXSPANEL_H
