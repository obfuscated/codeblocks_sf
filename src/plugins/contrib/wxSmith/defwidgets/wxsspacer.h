#ifndef WXSSPACER_H
#define WXSSPACER_H

#include "../widget.h"

class wxsSpacer : public wxsWidget
{
	public:
		wxsSpacer(wxsWidgetManager* Man,wxsWindowRes* Res);
		virtual ~wxsSpacer();
        virtual const wxsWidgetInfo& GetInfo();
        virtual wxString GetProducingCode(const wxsCodeParams& Params);
        virtual wxString GetDeclarationCode(const wxsCodeParams& Params);

    protected:

        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
};


#endif // WXSSPACER_H
