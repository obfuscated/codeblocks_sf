#include "wxsadvqpp.h"
#include "wxsadvqppchild.h"

BEGIN_EVENT_TABLE(wxsAdvQPP,wxsQuickPropsPanel)
    EVT_BUTTON(wxID_ANY,wxsAdvQPP::OnToggleButton)
END_EVENT_TABLE()

wxsAdvQPP::wxsAdvQPP(wxWindow* Parent,wxsPropertyContainer* Container):
    wxsQuickPropsPanel(Parent,Container,-1,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T("wxsAdvQPP"))
{
    Sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(Sizer);
}

wxsAdvQPP::~wxsAdvQPP()
{
}

void wxsAdvQPP::Register(wxsAdvQPPChild* Child,const wxString& Title)
{
    Freeze();
    wxButton* Button = new wxButton(this,-1,_T(" - ") + Title,wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxBU_EXACTFIT|wxBU_LEFT);
    Sizer->Add(Button,0,wxEXPAND,0);
    Sizer->Add(Child,0,wxEXPAND,0);
    Sizer->SetSizeHints(this);
    Sizer->Fit(this);
    Layout();
    Thaw();
    Buttons.Add(Button);
    Children.Add(Child);
}

void wxsAdvQPP::OnToggleButton(wxCommandEvent& event)
{
    Freeze();
    for ( size_t i = Buttons.Count(); i-- > 0; )
    {
        if ( event.GetEventObject() == Buttons[i] )
        {
            wxString Text = Buttons[i]->GetLabel();
            if ( Text.Length() > 2 )
            {
                if ( Text[1] == _T('-') )
                {
                    Text.SetChar(1,_T('+'));
                    Sizer->Show(Children[i],false);
                }
                else
                {
                    Text.SetChar(1,_T('-'));
                    Sizer->Show(Children[i],true);
                }
                Buttons[i]->SetLabel(Text);
            }
        }
    }
    Layout();
    Thaw();
}

void wxsAdvQPP::Update()
{
    for ( size_t i = Children.Count(); i-- > 0; )
    {
        Children[i]->Update();
    }
}
