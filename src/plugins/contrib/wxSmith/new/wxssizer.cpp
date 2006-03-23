#include "wxssizer.h"

#include <wx/dcclient.h>
#include <messagemanager.h>
#include "properties/wxsproperties.h"
#include "wxssizerparentqp.h"

namespace 
{
    class wxsSizerPreview: public wxPanel
    {
        public:
            wxsSizerPreview(wxWindow* Parent): wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL)
            {
                InheritAttributes();
                Connect(wxID_ANY,wxEVT_PAINT,(wxObjectEventFunction)&wxsSizerPreview::OnPaint);
            }

        private:

            void OnPaint(wxPaintEvent& event)
            {
                // Drawing additional border around te panel
                wxPaintDC DC(this);
                int W, H;
                GetSize(&W,&H);
                DC.SetBrush(wxNullBrush);
                DC.SetPen(*wxRED_PEN);
                DC.DrawRectangle(0,0,W,H);
            }
    };
}

void wxsSizerExtra::EnumProperties(long Flags)
{
    WXS_SIZERFLAGS(wxsSizerExtra,Flags,0);
    WXS_DIMENSION(wxsSizerExtra,Border,BorderInDU,0,_("Border"),_("  Dialog Units"),_T("border"),0,false);
    WXS_LONG(wxsSizerExtra,Proportion,0,_("Proportion"),_T("option"),0);
}

wxsSizer::wxsSizer(wxsWindowRes* Resource): wxsParent(Resource)
{
}

void wxsSizer::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    BuildSizerCreatingCode(Code,Language);
    
    bool UnknownLang = false;
    int Count = GetChildCount();
    for ( int i=0; i<Count; i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsSizerExtra* Extra = (wxsSizerExtra*)GetChildExtra(i);
        
        // Using same parent as we got, sizer is not a parent window
        Child->BuildCreatingCode(Code,WindowParent,Language);
        
        switch ( Child->GetType() )
        {
            case wxsTWidget:
            case wxsTContainer:
            case wxsTSizer:
                switch ( Language )
                {
                    case wxsCPP:
                    {
                        Code << GetVarName() << _T("->Add(") << Child->GetVarName()
                             << wxString::Format(_T(",%d,"),Extra->Proportion)
                             << wxsSizerFlagsProperty::GetString(Extra->Flags)
                             << _T(",") << wxsDimensionProperty::GetPixelsCode(
                                        Extra->Border,Extra->BorderInDU,WindowParent,wxsCPP)
                             << _T(");\n");
                        break;
                    }
                    
                    default:
                    {
                        UnknownLang = true;
                    }
                }
                break;
            
            case wxsTSpacer:
                // Spacer added using wxSizer::Add(width,height,...)
                // TODO: Code it when wxsSpacer is done
                break;
                
            default:;
        }
    }
    
    if ( UnknownLang )
    {
        DBGLOG(_T("wxSmith: Unknown coding language (id: %d)"),Language);
    }
}

wxObject* wxsSizer::BuildPreview(wxWindow* Parent,bool Exact)
{
    wxWindow* NewParent = Parent;
    
    if ( !Exact )
    {
        NewParent = new wxsSizerPreview(Parent);
    }
    
    wxSizer* Sizer = BuildSizerPreview();
    int Count = GetChildCount();
    for ( int i=0; i<Count; i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsSizerExtra* Extra = (wxsSizerExtra*)GetChildExtra(i);
        
        // We pass either Parent passed to current BuildPreview function
        // or pointer to additional parent currently created
        wxObject* ChildPreview = Child->BuildPreview(NewParent,Exact);
        if ( ChildPreview ) continue;
        
        wxSizer*  ChildAsSizer = wxDynamicCast(ChildPreview,wxSizer);
        if ( ChildAsSizer )
        {
            Sizer->Add(ChildAsSizer,Extra->Proportion,
                wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
                wxsDimensionProperty::GetPixels(
                    Extra->Border,Extra->BorderInDU,Parent));
        }
        else
        {
            wxWindow* ChildAsWindow = wxDynamicCast(ChildPreview,wxWindow);
            if ( ChildAsWindow )
            {
                Sizer->Add(ChildAsWindow,Extra->Proportion,
                    wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
                    wxsDimensionProperty::GetPixels(
                        Extra->Border,Extra->BorderInDU,Parent));
            }
        }
    }

    if ( !Exact )
    {
        NewParent->SetSizer(Sizer);
        Sizer->Fit(NewParent);
        Sizer->SetSizeHints(NewParent);
        return NewParent;
    }
    
    return Sizer;
}

wxsPropertyContainer* wxsSizer::BuildExtra()
{
    return new wxsSizerExtra();
}

void wxsSizer::AddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    Child->AddItemQPP(QPP);
    int Index = GetChildIndex(Child);
    if ( Index >= 0 )
    {
        QPP->Register(new wxsSizerParentQP(QPP,(wxsSizerExtra*)GetChildExtra(Index)),_("Sizer"));
    }
}
