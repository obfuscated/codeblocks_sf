#include "wxssizer.h"

#include <wx/dcclient.h>
#include <messagemanager.h>
#include "properties/wxsproperties.h"
#include "wxssizerparentqp.h"
#include "wxsitemfactory.h"

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
                DC.SetBrush(*wxTRANSPARENT_BRUSH);
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

wxString wxsSizerExtra::AllParamsCode(const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            return wxString::Format(_T("%d,"),Proportion) +
                   wxsSizerFlagsProperty::GetString(Flags) +
                   _T(",") << wxsDimensionProperty::GetPixelsCode(Border,BorderInDU,WindowParent,wxsCPP);
    }

    wxsLANGMSG(wxsSizerExtra::AllParamsCode,Language);
    return wxEmptyString;
}

wxsSizer::wxsSizer(wxsWindowRes* Resource): wxsParent(Resource)
{
}

void wxsSizer::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    BuildSizerCreatingCode(Code,WindowParent,Language);

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
                        // cyberkoa : Left out a comma after the GetVarName()
                        Code << GetVarName() << _T("->Add(") << Child->GetVarName() << _T(",")
                             << Extra->AllParamsCode(WindowParent,wxsCPP) << _T(");\n");
                        break;
                    }

                    default:
                    {
                        UnknownLang = true;
                    }
                }
                break;

            case wxsTSpacer:
                // Spacer is responsible for adding itself into sizer
                break;

            default:
                break;
        }
    }

    if ( UnknownLang )
    {
        wxsLANGMSG(wxsSizer::BuildCreatingCode,Language);
    }
}

wxObject* wxsSizer::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxWindow* NewParent = Parent;

    if ( !Exact )
    {
        NewParent = new wxsSizerPreview(Parent);
    }

    wxSizer* Sizer = BuildSizerPreview(Parent);
    int Count = GetChildCount();
    for ( int i=0; i<Count; i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsSizerExtra* Extra = (wxsSizerExtra*)GetChildExtra(i);

        // We pass either Parent passed to current BuildPreview function
        // or pointer to additional parent currently created
        wxObject* ChildPreview = Child->BuildPreview(NewParent,Exact);
        if ( !ChildPreview ) continue;

        wxSizer* ChildAsSizer = wxDynamicCast(ChildPreview,wxSizer);
        wxWindow* ChildAsWindow = wxDynamicCast(ChildPreview,wxWindow);
        wxSizerItem* ChildAsItem = wxDynamicCast(ChildPreview,wxSizerItem);
        if ( ChildAsSizer )
        {
            Sizer->Add(ChildAsSizer,Extra->Proportion,
                wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
                wxsDimensionProperty::GetPixels(
                    Extra->Border,Extra->BorderInDU,Parent));
        }
        else if ( ChildAsWindow )
        {
            Sizer->Add(ChildAsWindow,Extra->Proportion,
                wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
                wxsDimensionProperty::GetPixels(
                    Extra->Border,Extra->BorderInDU,Parent));
        }
        else if ( ChildAsItem )
        {
            ChildAsItem->SetProportion(Extra->Proportion);
            ChildAsItem->SetFlag(wxsSizerFlagsProperty::GetWxFlags(Extra->Flags));
            ChildAsItem->SetBorder(wxsDimensionProperty::GetPixels(Extra->Border,Extra->BorderInDU,Parent));
            Sizer->Add(ChildAsItem);
        }
    }

    if ( !Exact )
    {
        NewParent->SetSizer(Sizer);
        Sizer->Fit(NewParent);
        Sizer->SetSizeHints(NewParent);
        wxSizer* OutSizer = new wxBoxSizer(wxHORIZONTAL);
        OutSizer->Add(NewParent,1,wxEXPAND,0);
        Parent->SetSizer(OutSizer);
        OutSizer->SetSizeHints(Parent);
        return NewParent;
    }

    Parent->SetSizer(Sizer);
    Sizer->SetSizeHints(Parent);
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

bool wxsSizer::XmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    if ( cbC2U(Elem->Attribute("class")) == _T("spacer") )
    {
        wxsItem* Item = wxsGEN(_T("spacer"),GetResource());
        if ( !AddChild(Item) )
        {
            delete Item;
            return false;
        }
        RestoreExtraData(GetChildCount()-1,Elem);
        return Item->XmlRead(Elem,IsXRC,IsExtra);
    }

    return wxsParent::XmlReadChild(Elem,IsXRC,IsExtra);
}

bool wxsSizer::XmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    wxsItem* Child = GetChild(Index);
    if ( Child->GetType() == wxsTSpacer )
    {
        Elem->SetAttribute("class","spacer");
        StoreExtraData(Index,Elem);
        return Child->XmlWrite(Elem,IsXRC,IsExtra);
    }

    return wxsParent::XmlWriteChild(Index,Elem,IsXRC,IsExtra);
}

wxString wxsSizer::XmlGetExtraObjectClass()
{
    return _T("sizeritem");
}
