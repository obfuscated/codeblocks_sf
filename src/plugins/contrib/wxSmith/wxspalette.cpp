#include "wxspalette.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>
#include <map>
#include "widget.h"
#include "wxswidgetfactory.h"

wxsPalette* wxsPalette::Singleton = NULL;

wxsPalette::wxsPalette(wxWindow* Parent,wxSmith* _Plugin):
    wxPanel(Parent),
    Plugin(_Plugin),
    InsType(itBefore)
{
	wxFlexGridSizer* Sizer = new wxFlexGridSizer(0,1,5,5);
	Sizer->AddGrowableCol(0);
	Sizer->AddGrowableRow(1);
	
	wxFlexGridSizer* Sizer2 = new wxFlexGridSizer(2,0,5,15);
	Sizer2->AddGrowableCol(1);
	
	Sizer2->Add(new wxStaticText(this,-1,wxT("Insertion type")));
	Sizer2->Add(new wxStaticText(this,-1,wxT("Top list")));
	
	wxGridSizer* Sizer3 = new wxGridSizer(1,0,5,5);
	Sizer3->Add(AddBefore = new wxRadioButton(this,-1,wxT("Before")));
	Sizer3->Add(AddAfter  = new wxRadioButton(this,-1,wxT("After")));
	Sizer3->Add(AddInto   = new wxRadioButton(this,-1,wxT("Into")));
	
	Sizer2->Add(Sizer3);
	
	Sizer->Add(Sizer2,0,wxALL|wxGROW,10);
	
	wxScrolledWindow* WidgetsSpace = new wxScrolledWindow(this,-1);
	
	CreateWidgetsPalette(WidgetsSpace);
	
	Sizer->Add(WidgetsSpace,0,wxALL|wxGROW,10);
	
	SetSizer(Sizer);
	Sizer->SetSizeHints(this);
	
	Singleton = this;
}

wxsPalette::~wxsPalette()
{
	//dtor
}

int wxsPalette::GetInsertionType()
{
    int IT = InsType & InsTypeMask;
    
    if ( !IT )
    {
        if ( InsTypeMask & itBefore ) IT = itBefore;
        else if ( InsType & itAfter ) IT = itAfter;
        else if ( InsType & itInto ) IT = itInto;
    }    
    
    return IT;
}

void wxsPalette::SetInsertionTypeMask(int Mask)
{
    InsTypeMask = Mask;
    
    int CurrentIT = GetInsertionType();
    
    AddBefore->SetValue( (CurrentIT & itBefore) != 0 );
    AddAfter->SetValue( (CurrentIT & itAfter) != 0 );
    AddInto->SetValue( (CurrentIT & itInto) != 0 );
}

void wxsPalette::OnRadio(wxCommandEvent& event)
{
    int CurrentIT =
        ( AddBefore->GetValue() ? itBefore : 0 ) |
        ( AddAfter->GetValue()  ? itAfter  : 0 ) |
        ( AddInto->GetValue()   ? itInto   : 0 );
        
    InsType = CurrentIT;
}

struct ltstr {  bool operator()(const char* s1, const char* s2) const { return strcasecmp(s1, s2) < 0; } };

void wxsPalette::CreateWidgetsPalette(wxWindow* Wnd)
{
    // First we need to split all widgets into groups
    // it will be done using multimap
 
 
    typedef std::multimap<const char*,const wxsWidgetInfo*,ltstr> MapT;
    typedef MapT::iterator MapI;
    
    MapT Map;
    
    for ( const wxsWidgetInfo* Info = wxsWidgetFactory::Get()->GetFirstInfo(); Info; Info = wxsWidgetFactory::Get()->GetNextInfo() )
        Map.insert(std::pair<const char*,const wxsWidgetInfo*>(Info->Category,Info));
        
    // Creatign main sizer inside window
    wxBoxSizer* Sizer = new wxBoxSizer(wxVERTICAL);
    
    wxFlexGridSizer* RowSizer = NULL;
    
    const char* PreviousGroup = "";
    
    for ( MapI i = Map.begin(); i != Map.end(); ++i )
    {
        if ( strcasecmp(PreviousGroup,(*i).first) )
        {
            if ( RowSizer ) Sizer->Add(RowSizer,0,wxALL|wxGROW,5);
            Sizer->Add(new wxStaticLine(Wnd),0,wxGROW);
            
            // Need to create new group
            RowSizer = new wxFlexGridSizer(1,0,5,5);
            RowSizer->Add(new wxStaticText(Wnd,-1,(*i).first));
            PreviousGroup = (*i).first;
        }
        
        RowSizer->Add(new wxButton(Wnd,-1,(*i).second->Name));
    }
    
    if ( RowSizer ) Sizer->Add(RowSizer,0,wxALL|wxGROW,5);
    
    Wnd->SetSizer(Sizer);
    Sizer->SetVirtualSizeHints(Wnd);
}

void wxsPalette::OnButton(wxCommandEvent& event)
{
    wxButton* Btn = (wxButton*)event.GetEventObject();
    
    if ( Btn )
    {
        InsertRequest(Btn->GetLabel().c_str());
    }
}

void wxsPalette::InsertRequest(const char* Name)
{
}

BEGIN_EVENT_TABLE(wxsPalette,wxPanel)
    EVT_RADIOBUTTON(-1,wxsPalette::OnRadio)
    EVT_BUTTON(-1,wxsPalette::OnButton)
END_EVENT_TABLE()
