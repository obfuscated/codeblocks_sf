#ifndef WXSCUSTOMWIDGETXMLHANDLER_H
#define WXSCUSTOMWIDGETXMLHANDLER_H

#include <wx/xrc/xmlres.h>

class wxsCustomWidgetXmlHandler : public wxXmlResourceHandler
{
    public:
        wxsCustomWidgetXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);
};

#endif
