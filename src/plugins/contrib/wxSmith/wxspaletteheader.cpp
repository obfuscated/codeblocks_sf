#include "wxsheaders.h"
#include "wxspaletteheader.h"

#include "wxspalette.h"
#include "wxsglobals.h"
#include "widget.h"

BEGIN_EVENT_TABLE(wxsPaletteHeader,wxPanel)
//(*EventTable(wxsPaletteHeader)
    EVT_RADIOBUTTON(ID_INTO,wxsPaletteHeader::OnAddIntoChange)
    EVT_RADIOBUTTON(ID_BEFORE,wxsPaletteHeader::OnAddBeforeChange)
    EVT_RADIOBUTTON(ID_AFTER,wxsPaletteHeader::OnAddAfterChange)
    EVT_BUTTON(ID_BUTTON1,wxsPaletteHeader::OnButtonDelClick)
    EVT_BUTTON(ID_BUTTON2,wxsPaletteHeader::OnButtonPreviewClick)
//*)
END_EVENT_TABLE()

wxsPaletteHeader::wxsPaletteHeader(wxWindow* parent,wxWindowID id):
    InsType(wxsPalette::itInto),
    InsTypeMask(wxsPalette::itInto|wxsPalette::itAfter|wxsPalette::itBefore),
    ParentHeader(NULL)
{
	//(*Initialize(wxsPaletteHeader)
    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
    MainSizer = new wxFlexGridSizer(0,0,0,0);
    MainSizer->AddGrowableCol(2);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Insert"));
    AddInto = new wxRadioButton(this,ID_INTO,_("Into"),wxDefaultPosition,wxDefaultSize,0);
    AddInto->SetValue(true);
    AddBefore = new wxRadioButton(this,ID_BEFORE,_("Before"),wxDefaultPosition,wxDefaultSize,0);
    AddBefore->SetValue(false);
    AddAfter = new wxRadioButton(this,ID_AFTER,_("After"),wxDefaultPosition,wxDefaultSize,0);
    AddAfter->SetValue(false);
    StaticBoxSizer1->Add(AddInto,1,wxALIGN_CENTER|wxEXPAND,5);
    StaticBoxSizer1->Add(AddBefore,1,wxALIGN_CENTER|wxEXPAND,5);
    StaticBoxSizer1->Add(AddAfter,1,wxALIGN_CENTER|wxEXPAND,5);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Operations"));
    ButtonDel = new wxButton(this,ID_BUTTON1,_("Delete"),wxDefaultPosition,wxDefaultSize,0);
    if (false) ButtonDel->SetDefault();
    ButtonPreview = new wxButton(this,ID_BUTTON2,_("Preview"),wxDefaultPosition,wxDefaultSize,0);
    if (false) ButtonPreview->SetDefault();
    StaticBoxSizer2->Add(ButtonDel,1,wxALL|wxALIGN_CENTER,2);
    StaticBoxSizer2->Add(ButtonPreview,1,wxALL|wxALIGN_CENTER,2);
    MainSizer->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
    MainSizer->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
    this->SetSizer(MainSizer);
    MainSizer->Fit(this);
    MainSizer->SetSizeHints(this);
    //*)
}

wxsPaletteHeader::~wxsPaletteHeader()
{
}


void wxsPaletteHeader::OnAddIntoChange(wxCommandEvent& event)
{
    if ( AddInto->GetValue() ) InsType = wxsPalette::itInto;
}

void wxsPaletteHeader::OnAddBeforeChange(wxCommandEvent& event)
{
    if ( AddBefore->GetValue() ) InsType = wxsPalette::itBefore;
}

void wxsPaletteHeader::OnAddAfterChange(wxCommandEvent& event)
{
    if ( AddAfter->GetValue() ) InsType = wxsPalette::itAfter;
}

void wxsPaletteHeader::SetInsertionType(int Type)
{
    Type &= InsTypeMask;

    if ( !Type ) Type = InsTypeMask;

    if ( Type & wxsPalette::itInto )
    {
        AddInto->SetValue(true);
        InsType = wxsPalette::itInto;
    }
    else if ( Type & wxsPalette::itAfter )
    {
        AddAfter->SetValue(true);
        InsType = wxsPalette::itAfter;
    }
    else if ( Type & wxsPalette::itBefore )
    {
        AddBefore->SetValue(true);
        InsType = wxsPalette::itBefore;
    }
    else
    {
        InsType = 0;
    }
}

void wxsPaletteHeader::SetInsertionTypeMask(int Mask)
{
    InsTypeMask = Mask;
    AddBefore->Enable(Mask&wxsPalette::itBefore != 0);
    AddAfter ->Enable(Mask&wxsPalette::itAfter  != 0);
    AddInto  ->Enable(Mask&wxsPalette::itInto   != 0);
    SetInsertionType(InsType);
}

void wxsPaletteHeader::OnButtonDelClick(wxCommandEvent& event)
{
    wxsPALETTE()->DeleteRequest();
}


void wxsPaletteHeader::OnButtonPreviewClick(wxCommandEvent& event)
{
    wxsPALETTE()->PreviewRequest();
}

void wxsPaletteHeader::SeletionChanged(wxsWidget* Widget)
{
    Freeze();
    if ( ParentHeader )
    {
        MainSizer->Detach(ParentHeader);
        delete ParentHeader;
        ParentHeader = NULL;
    }

    if ( Widget && Widget->GetParent() )
    {
        wxsWidget* Parent = Widget->GetParent();
        int Index = Parent->FindChild(Widget);
        ParentHeader = Parent->GetChildPaletteHeader(this,Index);
        MainSizer->Add(ParentHeader);
    }

    MainSizer->Layout();
    Thaw();
}
