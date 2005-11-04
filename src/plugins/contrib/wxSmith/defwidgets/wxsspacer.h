#ifndef WXSSPACER_H
#define WXSSPACER_H

#include "../widget.h"

class WXSCLASS wxsSpacer : public wxsWidget
{
	public:
		wxsSpacer(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsSpacer();

        virtual const wxsWidgetInfo& GetInfo();

    protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
};


#endif // WXSSPACER_H
