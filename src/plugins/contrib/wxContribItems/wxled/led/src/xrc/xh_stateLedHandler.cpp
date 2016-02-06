#include "xrc/xh_stateLedHandler.h"
#include "wx/stateLed.h"
#include <wx/xml/xml.h> // wxXmlNode

// Register with wxWidgets' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(wxStateLedXmlHandler , wxXmlResourceHandler)

wxStateLedXmlHandler ::wxStateLedXmlHandler ()
{
    // this call ad<ds support for all wxWindows class styles
    // (e.g. wxBORDER_SIMPLE, wxBORDER_SUNKEN, wxWS_EX_* etc etc)
    AddWindowStyles();

    // if MyControl class supports e.g. MYCONTROL_DEFAULT_STYLE
    // you should use:
    //     XRC_ADD_STYLE(MYCONTROL_DEFAULT_STYLE);
}
#include <wx/msgdlg.h>
wxObject *wxStateLedXmlHandler ::DoCreateResource()
{
    // the following macro will init a pointer named "control"
    // with a new instance of the MyControl class, but will NOT
    // Create() it!
    XRC_MAKE_INSTANCE(control, wxStateLed)

    control->Create(m_parentAsWindow, GetID(), GetColour(wxT("disable_colour")));
    wxXmlNode* attr = GetParamNode(wxT("colour"));
    wxString temp;
#if wxCHECK_VERSION(3, 0, 0)
    attr->GetAttribute(wxT("count"), &temp);
#else
    attr->GetPropVal(wxT("count"), &temp);
#endif
    int numberOfState = wxAtoi(temp);
#if wxCHECK_VERSION(3, 0, 0)
    attr->GetAttribute(wxT("current"), &temp);
#else
    attr->GetPropVal(wxT("current"), &temp);
#endif
    int state = wxAtoi(temp);
    attr = attr->GetChildren();
    for( int i = 0; i < numberOfState; i++, attr = attr->GetNext())
        control->RegisterState( i, wxColour(GetNodeContent(attr)));
    control->SetState(state-1);
    if( GetBool(wxT("enabled"),true))
        control->Enable();
    else
        control->Disable();

    SetupWindow(control);

    return control;
}

bool wxStateLedXmlHandler ::CanHandle(wxXmlNode *node)
{
    // this function tells XRC system that this handler can parse
    // the <object class="MyControl"> tags
    return IsOfClass(node, wxT("wxStateLed"));
}
