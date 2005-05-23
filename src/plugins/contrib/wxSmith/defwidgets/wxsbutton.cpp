#include "wxsbutton.h"

#include "wxsstdmanager.h"
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/settings.h>
#include <wx/log.h>
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsButtonStyles)
    WXS_ST_CATEGORY("wxButton")
    WXS_ST(wxBU_LEFT)
    WXS_ST(wxBU_TOP)
    WXS_ST(wxBU_RIGHT)
    WXS_ST(wxBU_BOTTOM)
    WXS_ST(wxBU_EXACTFIT)
    WXS_ST(wxNO_BORDER)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsButtonStyles)

class wxsButtonPreview: public wxButton
{
    public:
        wxsButtonPreview(wxWindow* Parent,wxsButton* O):
            wxButton(Parent,-1,O->GetLabel(),O->GetPosition(),O->GetSize(),O->GetStyle()), Object(O)
        {
        }
        
        void UpdateContent()
        {
            assert ( Object != NULL );
            SetPosition(Object->GetPosition());
            SetSize(Object->GetSize());
            SetLabel(Object->GetLabel());
            SetWindowStyle(Object->GetStyle());
            // TODO (SpOoN#1#): Do something with default flag
        }
        
    private:
    
        void OnLClick(wxMouseEvent& event)
        {
            wxsPropertiesMan::Get().SetActiveWidget(Object);
            event.Skip();
        }
        
        void OnLDClick(wxMouseEvent& event)
        {
        // TODO (SpOoN#1#): Automatically insert event handler (or go to handler function)
        }
        
        
        wxsButton* Object;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsButtonPreview,wxButton)
    EVT_LEFT_DCLICK(wxsButtonPreview::OnLDClick)
    EVT_LEFT_DOWN(wxsButtonPreview::OnLClick)
END_EVENT_TABLE()


wxsButton::~wxsButton() 
{
}

const wxsWidgetInfo& wxsButton::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsButtonId);
}

const char* wxsButton::GetProducingCode(wxsCodeParams& Params)
{
    static wxString Code;
    
    const CodeDefines Defs = GetCodeDefines();
    
    Code = wxString::Format(
        "%s = new wxButton(%s,%s,wxT(%s),%s,%s,%s);",
        BaseParams.VarName.c_str(),
        Params.ParentName,
        BaseParams.IdName.c_str(),
        GetCString(Text.c_str()).c_str(),
        Defs.Pos.c_str(),
        Defs.Size.c_str(),
        Defs.Style.c_str()
    );
    
    if ( GetDefault() )
    {
        Code += wxString::Format(wxT("\n%s->SetDefault()"),BaseParams.VarName.c_str());
    }
    
    return Code.c_str();
}


wxWindow* wxsButton::MyCreatePreview(wxWindow* Parent)
{
    return new wxsButtonPreview(Parent,this);
}


void wxsButton::MyUpdatePreview()
{
    if ( GetPreview() )
    {
        dynamic_cast<wxsButtonPreview*> (GetPreview()) -> UpdateContent();
    }
}

void wxsButton::CreateObjectProperties()
{
    wxsWidget::CreateObjectProperties();
    PropertiesObject.AddProperty("Label:",Text,0);
    PropertiesObject.AddProperty("Default:",Default,1);
}


bool wxsButton::MyXmlLoad(TiXmlElement* Element)
{
    wxsWidget::MyXmlLoad(Element);
    Text = XmlGetVariable("label");
    return true;
}
