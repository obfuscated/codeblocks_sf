#ifndef XH_STATELEDHANDLER_H_INCLUDED
#define XH_STATELEDHANDLER_H_INCLUDED

#include <wx/xrc/xmlres.h>

class wxStateLedXmlHandler : public wxXmlResourceHandler
{
    public:
        // Constructor.
        wxStateLedXmlHandler ();

        // Creates the control and returns a pointer to it.
        virtual wxObject *DoCreateResource();

        // Returns true if we know how to create a control for the given node.
        virtual bool CanHandle(wxXmlNode *node);

        // Register with wxWidgets' dynamic class subsystem.
        DECLARE_DYNAMIC_CLASS(wxStateLedXmlHandler )
};


#endif // XH_LEDHANDLER_H_INCLUDED
