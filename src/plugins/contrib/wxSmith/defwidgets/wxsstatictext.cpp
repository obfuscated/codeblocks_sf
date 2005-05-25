#include "wxsstatictext.h"

#include "wxsstdmanager.h"
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/settings.h>
#include <wx/log.h>
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsStaticTextStyles)
    WXS_ST_CATEGORY("wxStaticText")
    WXS_ST(wxALIGN_RIGHT)
    WXS_ST(wxALIGN_CENTRE)
    WXS_ST(wxST_NO_AUTORESIZE)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsStaticTextStyles)

class wxsStaticTextPreview: public wxStaticText
{
    public:
        wxsStaticTextPreview(wxWindow* Parent,wxsStaticText* O):
            wxStaticText(Parent,-1,O->GetLabel(),
                wxDefaultPosition,
                wxDefaultSize,
                O->GetStyle()
                ), Object(O)
        {
        }
        
        void UpdateContent()
        {
            if ( Object )
            {
                SetLabel(Object->GetLabel());
                SetWindowStyle(Object->GetStyle());
            }
        }
        
    private:
    
        void OnLClick(wxMouseEvent& event)
        {
            wxsPropertiesMan::Get()->SetActiveWidget(Object);
            event.Skip();
        }
        
        void OnLDClick(wxMouseEvent& event)
        {
        // TODO (SpOoN#1#): Automatically insert event handler (or go to handler function)
        }
        
        
        wxsStaticText* Object;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsStaticTextPreview,wxButton)
    EVT_LEFT_DCLICK(wxsStaticTextPreview::OnLDClick)
    EVT_LEFT_DOWN(wxsStaticTextPreview::OnLClick)
END_EVENT_TABLE()


wxsStaticText::~wxsStaticText() 
{
}

const wxsWidgetInfo& wxsStaticText::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsStaticTextId);
}

const char* wxsStaticText::GetProducingCode(wxsCodeParams& Params)
{
    static wxString Code;
    
    Code = wxString::Format(
        "%s = new wxStaticText(%s,%s,wxT(%s));",
        Params.ParentName,
        BaseParams.VarName.c_str(),
        BaseParams.IdName.c_str(),
        GetCString(Text).c_str()
    );
    
    return Code.c_str();
}


wxWindow* wxsStaticText::MyCreatePreview(wxWindow* Parent)
{
    /*return new wxsStaticTextPreview(Parent,this);*/
    return new wxsStaticTextPreview(Parent,this);
}


void wxsStaticText::MyUpdatePreview()
{
    if ( GetPreview() )
    {
        dynamic_cast<wxsStaticTextPreview*> (GetPreview()) -> UpdateContent();
    }
}

bool wxsStaticText::MyXmlLoad()
{
    Text = XmlGetVariable("label");
    return true;
}

bool wxsStaticText::MyXmlSave()
{
    XmlSetVariable("label",Text.c_str());
    return true;
}
