#ifndef WXLEDHANDLER_H
#define WXLEDHANDLER_H

#include <wx/xrc/xmlres.h>


class wxLedHandler : public wxXmlResourceHandler
{
    public:
        /** Default constructor */
        wxLedHandler();

        // Creates the control and returns a pointer to it.
        virtual wxObject *DoCreateResource();

        // Returns true if we know how to create a control for the given node.
        virtual bool CanHandle(wxXmlNode *node);

        // Register with wxWidgets' dynamic class subsystem.
        DECLARE_DYNAMIC_CLASS(wxLedHandler)
};

#endif // WXLEDHANDLER_H
