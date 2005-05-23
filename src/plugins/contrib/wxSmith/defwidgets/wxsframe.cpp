#include "wxsframe.h"


#include <wx/frame.h>
#include <wx/sizer.h>

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"

//#define FOM_FRAME

class wxsFramePreview: 
    #ifdef FROM_FRAME
        public wxFrame
    #else
        public wxPanel
    #endif
{
    public:
        wxsFramePreview(wxWindow* Parent,wxsFrame* _Frame):
            #ifdef FROM_FRAME        
                wxFrame(Parent,-1,"No title",wxPoint(100,100),wxSize(150,150),
                    wxFRAME_FLOAT_ON_PARENT|wxDEFAULT_FRAME_STYLE)
            #else
                wxPanel(Parent,-1,wxPoint(10,10),wxSize(150,150),
                    wxRAISED_BORDER)
            #endif
                , Frame(_Frame)
        {}
        
        void UpdatePreview()
        {
            int Cnt = Frame->GetChildCount();
            
            for ( int i=0; i<Cnt; i++ )
            {
                wxsWidget* Widget = Frame->GetChild(i);
                if ( Widget->GetInfo().Sizer && Widget->GetPreview() )
                {
                    wxSizer* Sizer = new wxGridSizer(1);
                    Sizer->Add(Widget->GetPreview(),0,wxGROW);
                    SetSizer(Sizer);
                }
            }
        }
        
    private:
        
        wxsFrame* Frame;
        void OnMouse(wxMouseEvent& event)
        {
            wxsPropertiesMan::Get().SetActiveWidget(Frame);
            event.Skip();
        }
        
        void OnClose(wxCloseEvent& event)
        {
            if ( event.CanVeto() ) event.Veto();
        }
    
        DECLARE_EVENT_TABLE()
};

#ifdef FROM_FRAME
BEGIN_EVENT_TABLE(wxsFramePreview,wxFrame)
#else
BEGIN_EVENT_TABLE(wxsFramePreview,wxPanel)
#endif
    EVT_LEFT_DOWN(wxsFramePreview::OnMouse)
    EVT_CLOSE(wxsFramePreview::OnClose)
END_EVENT_TABLE()


wxsFrame::wxsFrame(wxsWidgetManager* Man):
    wxsContainer(Man,true,1)
{
	PropertiesObject.AddProperty("Class name:",ClassName);
}

wxsFrame::~wxsFrame()
{
	//dtor
}

const wxsWidgetInfo& wxsFrame::GetInfo()
{ 
    return *wxsStdManager.GetWidgetInfo(wxsFrameId);
}

/** Function generating code which should produce widget */
const char* wxsFrame::GetProducingCode(wxsCodeParams& Params)
{
    static wxString Str;
    Str = wxString::Format(
        "%s = new wxFrame(%s,%s,\"No title\");",
        BaseParams.VarName.c_str(),
        Params.ParentName,
        BaseParams.IdName.c_str());
    return Str.c_str();
}

/** Function shich should update content of current widget
 *
 * This function should update current content of widget if it's created
 * to keep it's content up to date
 */
void wxsFrame::MyUpdatePreview()
{
    if ( GetPreview() )
    {
        dynamic_cast<wxsFramePreview*> (GetPreview()) -> UpdatePreview();
    }
}

/** This function should create preview window for widget */
wxWindow* wxsFrame::MyCreatePreview(wxWindow* Parent)
{
   return new wxsFramePreview(Parent,this);
}
