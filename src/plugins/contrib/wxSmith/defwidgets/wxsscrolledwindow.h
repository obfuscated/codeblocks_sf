#ifndef WXSSCROLLEDWINDOW_H
#define WXSSCROLLEDWINDOW_H

#include "wxswindow.h"

WXS_ST_DECLARE(wxsScrolledWindowStyles)
WXS_EV_DECLARE(wxsScrolledWindowEvents)

/** Standard panel - it may be used as a widget */
class wxsScrolledWindow : public wxsWindow
{
	public:
		wxsScrolledWindow(wxsWidgetManager* Man,wxsWindowRes* Res);

		virtual ~wxsScrolledWindow();

        /** Gettign widget's info */
        virtual const wxsWidgetInfo& GetInfo();

        /** Function generating code which should produce widget */
        virtual wxString GetProducingCode(const wxsCodeParams& Params);
};

#endif
