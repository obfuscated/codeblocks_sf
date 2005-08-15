#include "wxsspacer.h"

#include "wxsstdmanager.h"

class wxsSpacerPreview: public wxPanel
{
	public:
        wxsSpacerPreview(wxWindow* Parent,const wxSize& Size):
            wxPanel(Parent,-1,wxDefaultPosition,Size)
        {}
        
    private:
    
        void OnPaint(wxPaintEvent& event)
        {
        	wxPaintDC DC(this);
        	DC.SetBrush(wxBrush(wxColour(0,0,0),wxCROSSDIAG_HATCH));
        	DC.SetPen(wxPen(wxColour(0,0,0),1));
        	DC.DrawRectangle(0,0,GetSize().GetWidth(),GetSize().GetHeight());
        }
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsSpacerPreview,wxPanel)
    EVT_PAINT(wxsSpacerPreview::OnPaint)
END_EVENT_TABLE()

wxsSpacer::wxsSpacer(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWidget(Man,Res,propSpacer)
{
}

wxsSpacer::~wxsSpacer()
{
}

const wxsWidgetInfo& wxsSpacer::GetInfo()
{
	return *wxsStdManager.GetWidgetInfo(wxsSpacerId);
}

wxWindow* wxsSpacer::MyCreatePreview(wxWindow* Parent)
{
	return new wxsSpacerPreview(Parent,GetSize());
}
