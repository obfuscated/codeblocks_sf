#include "wxsdialog.h"

#include <wx/frame.h>
#include <wx/sizer.h>

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsDialogStyles)

    WXS_ST(wxSTAY_ON_TOP)
    WXS_ST(wxCAPTION)
    WXS_ST(wxDEFAULT_DIALOG_STYLE)
    WXS_ST(wxTHICK_FRAME)
    WXS_ST(wxSYSTEM_MENU)
    WXS_ST(wxRESIZE_BORDER)
    WXS_ST(wxRESIZE_BOX)
//    WXS_ST(wxCLOSE_BOX)
    WXS_ST(wxDIALOG_MODAL)
    WXS_ST(wxDIALOG_MODELESS)
    WXS_ST(wxDIALOG_NO_PARENT)

    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
//    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
//    WXS_ST(wxDIALOG_EX_METAL)
    WXS_ST(wxMAXIMIZE_BOX)
    WXS_ST(wxMINIMIZE_BOX)
    WXS_ST(wxFRAME_SHAPED)

WXS_ST_END(wxsDialogStyles)



class wxsDialogPreview:  public wxPanel
{
    public:
        wxsDialogPreview(wxWindow* Parent,wxsDialog* _Dialog):
            wxPanel(Parent,-1,wxPoint(10,10),wxSize(150,150), wxRAISED_BORDER),
            Dialog(_Dialog)
        {}
        
        void UpdatePreview()
        {
            int Cnt = Dialog->GetChildCount();
            
            for ( int i=0; i<Cnt; i++ )
            {
                wxsWidget* Widget = Dialog->GetChild(i);
                if ( Widget->GetInfo().Sizer && Widget->GetPreview() )
                {
                    wxSizer* Sizer = new wxGridSizer(1);
                    Sizer->Add(Widget->GetPreview(),0,wxGROW);
                    SetSizer(Sizer);
                }
            }
        }
        
    private:
        
        wxsDialog* Dialog;
        void OnMouse(wxMouseEvent& event)
        {
            wxsPropertiesMan::Get()->SetActiveWidget(Dialog);
            event.Skip();
        }
        
        void OnClose(wxCloseEvent& event)
        {
            if ( event.CanVeto() ) event.Veto();
        }
    
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsDialogPreview,wxPanel)
    EVT_LEFT_DOWN(wxsDialogPreview::OnMouse)
    EVT_CLOSE(wxsDialogPreview::OnClose)
END_EVENT_TABLE()


wxsDialog::wxsDialog(wxsWidgetManager* Man):
    wxsContainer(Man,true),
    Centered(false)
{
}

wxsDialog::~wxsDialog()
{
	//dtor
}

const wxsWidgetInfo& wxsDialog::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsDialogId);
}

/** Function generating code which should produce widget */
const char* wxsDialog::GetProducingCode(wxsCodeParams& Params)
{
    static wxString Str;
    Str = wxString::Format(
        "%s = new wxDialog(%s,%s,\"No title\");",
        BaseParams.VarName.c_str(),
        Params.ParentName,
        BaseParams.IdName.c_str());
    if ( GetCentered() )
    {
        Str += wxString::Format("\n%s->Centre();",BaseParams.VarName.c_str());
    }
    return Str.c_str();
}

/** Function shich should update content of current widget
 *
 * This function should update current content of widget if it's created
 * to keep it's content up to date
 */
void wxsDialog::MyUpdatePreview()
{
    if ( GetPreview() )
    {
        dynamic_cast<wxsDialogPreview*>
            (GetPreview()) -> UpdatePreview();
    }
}

/** This function should create preview window for widget */
wxWindow* wxsDialog::MyCreatePreview(wxWindow* Parent)
{
   return new wxsDialogPreview(Parent,this);
}

bool wxsDialog::MyXmlLoad(TiXmlElement* Element)
{
    return wxsWidget::MyXmlLoad(Element);
}

void wxsDialog::CreateObjectProperties()
{
    wxsWidget::CreateObjectProperties();
	PropertiesObject.AddProperty("Title:",Title,0,false,false);
	PropertiesObject.AddProperty("Centered:",Centered,1,false,false);
}

