#include "xrc/xh_ledHandler.h"

#include <wx/led.h>

// Register with wxWidgets' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(wxLedHandler, wxXmlResourceHandler)

wxLedHandler::wxLedHandler()
{
    // this call adds support for all wxWindows class styles
    // (e.g. wxBORDER_SIMPLE, wxBORDER_SUNKEN, wxWS_EX_* etc etc)
    AddWindowStyles();
}

wxObject *wxLedHandler::DoCreateResource()
{
    // the following macro will init a pointer named "control"
    // with a new instance of the MyControl class, but will NOT
    // Create() it!
    XRC_MAKE_INSTANCE(control, wxLed)

    control->Create(m_parentAsWindow, GetID(),GetColour(wxT("disable_colour")),GetColour(wxT("on_colour")), GetColour(wxT("off_colour")), GetPosition(), GetSize());
    if( GetBool(wxT("enabled"),true))
        control->Enable();
    else
        control->Disable();

    control->SetOnOrOff(GetBool(wxT("on_or_off")));

    SetupWindow(control);

    return control;
}

bool wxLedHandler::CanHandle(wxXmlNode *node)
{
    // this function tells XRC system that this handler can parse
    // the <object class="MyControl"> tags
    return IsOfClass(node, wxT("wxLed"));
}
