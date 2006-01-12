#ifndef __WXSSTATICBITMAP_H
#define __WXSSTATICBITMAP_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsStaticBitmapStyles)
WXS_EV_DECLARE(wxsStaticBitmapEvents)

wxsDWDeclareBegin(wxsStaticBitmapBase,wxsStaticBitmapId)
    wxString ImageFile;
    wxString extension;
    long ImageType;
wxsDWDeclareEnd()

class wxsStaticBitmap: public wxsStaticBitmapBase
{
    public:
        wxsStaticBitmap(wxsWidgetManager* Man,wxsWindowRes* Res);

    protected:
        virtual bool MyPropertiesUpdated(bool Validate,bool Correct);
        virtual wxString GetProducingCode(const wxsCodeParams& Params);
        virtual wxWindow* MyCreatePreview(wxWindow* Parent);
    private:
        long GetBitmapType();
        wxString GetBitmapTypeAsString();
};

#endif
