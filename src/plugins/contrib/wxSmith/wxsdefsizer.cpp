#include "wxsdefsizer.h"

class wxsDefSizerPreview: public wxPanel
{
    public:
        wxsDefSizerPreview(wxWindow* Parent,wxsDefSizer* wxsSizer,wxSizer* _Sizer):
            wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL),
            Sizer(_Sizer),
            sSizer(wxsSizer)
        {
        	InheritAttributes();
        }
        
        
        wxSizer* Sizer;
        wxsDefSizer* sSizer;
        
        void UpdatePreview()
        {
            assert ( sSizer != NULL );
            
            int Cnt = sSizer->GetChildCount();
            for ( int i=0; i<Cnt; i++ )
            {
                wxsWidget* Child = sSizer->GetChild(i);
                const wxsWidgetBaseParams& Par = Child->GetBaseParams();
                int Flags = 0;
                if ( Par.BorderFlags & Par.Top )    Flags |= wxTOP;
                if ( Par.BorderFlags & Par.Bottom ) Flags |= wxBOTTOM;
                if ( Par.BorderFlags & Par.Left )   Flags |= wxLEFT;
                if ( Par.BorderFlags & Par.Right )  Flags |= wxRIGHT;
                switch ( Par.Placement )
                {
                    case wxsWidgetBaseParams::LeftTop:      Flags |= wxALIGN_LEFT | wxALIGN_TOP;  break;
                    case wxsWidgetBaseParams::LeftCenter:   Flags |= wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL; break;
                    case wxsWidgetBaseParams::LeftBottom:   Flags |= wxALIGN_LEFT | wxALIGN_BOTTOM; break;
                    case wxsWidgetBaseParams::CenterTop:    Flags |= wxALIGN_CENTER_HORIZONTAL | wxTOP; break;
                    case wxsWidgetBaseParams::Center:       Flags |= wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL; break;
                    case wxsWidgetBaseParams::CenterBottom: Flags |= wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM; break;
                    case wxsWidgetBaseParams::RightTop:     Flags |= wxALIGN_RIGHT | wxALIGN_TOP;  break;
                    case wxsWidgetBaseParams::RightCenter:  Flags |= wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL; break;
                    case wxsWidgetBaseParams::RightBottom:  Flags |= wxALIGN_RIGHT | wxALIGN_BOTTOM; break;
                }
                if ( Par.Expand ) Flags |= wxEXPAND;
                if ( Par.Shaped ) Flags |= wxSHAPED;
                Sizer->Add(
                    Child->GetPreview(),
                    Par.Proportion,
                    Flags,
                    Par.Border
                );
            }
            SetSizer(Sizer);
            Sizer->SetSizeHints(this);
            Layout();
            SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        }
        
        virtual bool HasTransparentBackground() const { return true; }
        
    private:
    
        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC DC(this);
            int W, H;
            GetSize(&W,&H);
            DC.SetBrush(GetParent()->GetBackgroundColour());
            DC.SetPen(*wxRED_PEN);
            DC.DrawRectangle(0,0,W,H);
        }
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsDefSizerPreview,wxPanel)
    EVT_PAINT(wxsDefSizerPreview::OnPaint)
//    EVT_ERASE_BACKGROUND(wxsGridSizerPreview::OnEraseBack)
END_EVENT_TABLE()

wxsDefSizer::wxsDefSizer(wxsWidgetManager* Man,wxsWindowRes* Res,BasePropertiesType pType):
    wxsContainer(Man,Res,false,0,pType)
{
}

wxsDefSizer::~wxsDefSizer()
{
}

wxString wxsDefSizer::GetFinalizingCode(wxsCodeParams& Params)
{
	wxString Code;
	int Cnt = GetChildCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsWidget* Child = GetChild(i);
		if ( Child->GetInfo().Spacer )
		{
			// Spacer class is threated as a special case
			Code.Append(wxString::Format(_T("%s->Add(%d,%d,%d);"),
                GetBaseParams().VarName.c_str(),
                Child->GetBaseParams().SizeX,
                Child->GetBaseParams().SizeY,
                Child->GetBaseParams().Proportion));
		}
		else
		{
            wxString FlagsToSizer = Child->GetFlagToSizer();
            if ( !FlagsToSizer.Length() ) FlagsToSizer = _T("0");
            Code.Append(
                wxString::Format(_T("%s->Add(%s,%d,%s,%d);\n"),
                    GetBaseParams().VarName.c_str(),
                    Child->GetBaseParams().VarName.c_str(),
                    Child->GetBaseParams().Proportion,
                    FlagsToSizer.c_str(),
                    Child->GetBaseParams().Border));
		}
	}
	
    if ( Params.IsDirectParent )
    {
        Code.Append(wxString::Format(_T("%s->SetSizer(%s);"),
            Params.ParentName.c_str(),
            BaseParams.VarName.c_str()));
    }
    return Code;
}

wxWindow* wxsDefSizer::MyCreatePreview(wxWindow* Parent)
{
    wxsDefSizerPreview* Preview = new wxsDefSizerPreview(Parent,this,NULL);
    Preview->Sizer = ProduceSizer(Preview);
    return Preview;
}

void wxsDefSizer::MyFinalUpdatePreview(wxWindow* Window)
{
    ((wxsDefSizerPreview*)Window) -> UpdatePreview();
}
