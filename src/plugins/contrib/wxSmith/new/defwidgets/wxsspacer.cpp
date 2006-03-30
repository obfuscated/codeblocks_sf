#include "wxsspacer.h"

#include "../wxssizer.h"

#include <messagemanager.h>

namespace
{
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

}

wxsItemInfo wxsSpacer::Info =
{
    _T("spacer"),
    wxsTSizer,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Layout"),
    30,
    _T(""),
    2, 6,
    NULL,
    NULL,
    0
};

void wxsSpacer::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    wxASSERT_MSG( GetParent()!=NULL, _T("Spacer must have parent") );
    wxASSERT_MSG( GetParent()->GetType() == wxsTSizer, _T("Spacer's parent must be sizer") );

    int Index = GetParent()->GetChildIndex(this);
    wxsSizerExtra* Extra = (wxsSizerExtra*) GetParent()->GetChildExtra(Index);
    wxString ParentName = GetParent()->GetVarName();

    if ( Extra == NULL ) return;

    switch ( Language )
    {
        case wxsCPP:
        {
            if ( Size.DialogUnits )
            {
                wxString SizeName = ParentName + wxString::Format(_T("SpacerSize%d"),Index);
                Code << _T("wxSize ") << SizeName << _T(" = ") << Size.GetSizeCode(WindowParent,wxsCPP) << _T(";\n")
                     << ParentName << _T("->Add(")
                     << SizeName << _T(".GetWidth(),")
                     << SizeName << _T(".GetHeight(),")
                     << Extra->AllParamsCode(WindowParent,wxsCPP) << _T(");\n");
            }
            else
            {
                Code << ParentName << wxString::Format(_T("->Add(%d,%d,"),Size.X,Size.Y)
                     << Extra->AllParamsCode(WindowParent,wxsCPP) << _T(");\n");
            }

            break;
        }
    }

    wxsLANGMSG(wxsSpacer::BuildCreatingCode,Language);
}

long wxsSpacer::GetPropertiesFlags()
{
    return wxsItem::GetPropertiesFlags() & ~(wxsFLVariable|wxsFLId);
}

wxObject* wxsSpacer::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxASSERT_MSG( GetParent()!=NULL, _T("Spacer must have parent") );
    wxASSERT_MSG( GetParent()->GetType() == wxsTSizer, _T("Spacer's parent must be sizer") );

    if ( Exact )
    {
        wxSize Sz = Size.GetSize(Parent);
        return new wxSizerItem(Sz.GetWidth(),Sz.GetHeight(),0,0,0,NULL);
    }
    return new wxsSpacerPreview(Parent,Size.GetSize(Parent));
}
