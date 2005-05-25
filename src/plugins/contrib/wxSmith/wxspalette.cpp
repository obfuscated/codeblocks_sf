#include "wxspalette.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>
#include <map>
#include "widget.h"
#include "wxswidgetfactory.h"
#include "wxspropertiesman.h"
#include "wxsmith.h"
#include "wxsresource.h"

static const int DeleteId = wxNewId();
static const int PreviewId = wxNewId();

wxsPalette* wxsPalette::Singleton = NULL;

wxsPalette::wxsPalette(wxWindow* Parent,wxSmith* _Plugin,int PN):
    wxPanel(Parent),
    Plugin(_Plugin),
    SelectedRes(NULL),
    InsType(itBefore),
    InsTypeMask(0),
    PageNum(PN)
{
	wxFlexGridSizer* Sizer = new wxFlexGridSizer(0,1,5,5);
	Sizer->AddGrowableCol(0);
	Sizer->AddGrowableRow(1);
	
	wxFlexGridSizer* Sizer2 = new wxFlexGridSizer(2,0,5,15);
	Sizer2->AddGrowableCol(3);
	
	Sizer2->Add(new wxStaticText(this,-1,wxT("Insertion type")));
	Sizer2->Add(new wxStaticText(this,-1,wxT("Delete")));
	Sizer2->Add(new wxStaticText(this,-1,wxT("Preview")));
	Sizer2->Add(new wxStaticText(this,-1,wxT("Top list")));
	
	wxGridSizer* Sizer3 = new wxGridSizer(1,0,5,5);
	Sizer3->Add(AddBefore = new wxRadioButton(this,-1,wxT("Before")));
	Sizer3->Add(AddAfter  = new wxRadioButton(this,-1,wxT("After")));
	Sizer3->Add(AddInto   = new wxRadioButton(this,-1,wxT("Into")));
	
	Sizer2->Add(Sizer3);
	
	Sizer2->Add(new wxButton(this,DeleteId,wxT("Delete")));
	Sizer2->Add(new wxButton(this,PreviewId,wxT("Preview")));
	
	Sizer->Add(Sizer2,0,wxALL|wxGROW,10);
	
	wxScrolledWindow* WidgetsSpace = new wxScrolledWindow(this,-1);
	
	CreateWidgetsPalette(WidgetsSpace);
	
	Sizer->Add(WidgetsSpace,0,wxALL|wxGROW,10);
	
	SetSizer(Sizer);
	Sizer->SetSizeHints(this);
	
	Singleton = this;
	
	SetInsertionTypeMask(0);
	
	SelectResource(NULL);
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
    
    AddBefore->Enable( (Mask & itBefore) != 0 );
    AddAfter->Enable( (Mask & itAfter) != 0 );
    AddInto->Enable( (Mask & itInto) != 0 );
    
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
    wxFlexGridSizer* Sizer = new wxFlexGridSizer(0,1,0,0);
    Sizer->AddGrowableCol(0);
    
    wxFlexGridSizer* RowSizer = NULL;
    
    const char* PreviousGroup = "";
    
    for ( MapI i = Map.begin(); i != Map.end(); ++i )
    {
        if ( !(*i).first || strcasecmp(PreviousGroup,(*i).first) )
        {
            if ( RowSizer ) Sizer->Add(RowSizer,0,wxALL|wxGROW,5);
            Sizer->Add(new wxStaticLine(Wnd,-1),0,wxGROW);
            
            if ( (*i).first && (*i).first[0] )
            {
                // Need to create new group
                RowSizer = new wxFlexGridSizer(1,0,5,5);
                RowSizer->Add(new wxStaticText(Wnd,-1,(*i).first));
                PreviousGroup = (*i).first;
            }
            else
            {
                RowSizer = NULL;
            }
        }

        if ( RowSizer )
        {
            RowSizer->Add(new wxButton(Wnd,-1,(*i).second->Name));
        }
    }
    
    if ( RowSizer ) Sizer->Add(RowSizer,0,wxALL|wxGROW,5);
    
    Wnd->SetSizer(Sizer);
    Sizer->SetVirtualSizeHints(Wnd);
}

void wxsPalette::OnButton(wxCommandEvent& event)
{
    wxWindowID Id = event.GetId();
    if ( Id == DeleteId )
    {
        DeleteRequest();
    }
    else if ( Id == PreviewId )
    {
        PreviewRequest();
    }
    else
    {
        wxButton* Btn = (wxButton*)event.GetEventObject();
        if ( Btn )
        {
            InsertRequest(Btn->GetLabel().c_str());
        }
    }
}

void wxsPalette::InsertRequest(const char* Name)
{
    wxsWidget* Current = wxsPropertiesMan::Get()->GetActiveWidget();
    if ( Current == NULL )
    {
        DebLog("wxSmith: No widget selected - couldn't create new widget");
        return;
    }

    if ( !GetInsertionType() )
    {
        return;
    }
    
    wxsWindowEditor* Edit = (wxsWindowEditor*)Current->CurEditor;
    
    wxsWidget* NewWidget = wxsWidgetFactory::Get()->Generate(Name);
    if ( NewWidget == NULL )
    {
        DebLog("wxSmith: Culdn't generate widget inside factory");
        return;
    }
    
    switch ( GetInsertionType() )
    {
        case itBefore:
            InsertBefore(NewWidget,Current);
            break;
            
        case itAfter:
            InsertAfter(NewWidget,Current);
            break;
            
        case itInto:
            InsertInto(NewWidget,Current);
            break;
            
        default:
            wxsWidgetFactory::Get()->Kill(NewWidget);
            DebLog("Something gone wrong");
            break;
    }
    
    
    if ( Edit )
    {
        Edit->RecreatePreview();
    }
}

void wxsPalette::InsertBefore(wxsWidget* New,wxsWidget* Ref)
{
    wxsWidget* Parent = Ref->GetParent();
    
    int Index;
    
    if ( !Parent || (Index=Parent->FindChild(Ref)) < 0 || Parent->AddChild(New,Index) < 0 )
    {
        wxsWidgetFactory::Get()->Kill(New);
        return;
    }

    // Adding this new item into resource tree
    
    New->BuildTree(Plugin->GetResourceTree(),Parent->TreeId,Index);
}   

void wxsPalette::InsertAfter(wxsWidget* New,wxsWidget* Ref)
{
    wxsWidget* Parent = Ref->GetParent();
    
    int Index;
    
    if ( !Parent || (Index=Parent->FindChild(Ref)) < 0 || Parent->AddChild(New,Index+1) < 0 )
    {
        wxsWidgetFactory::Get()->Kill(New);
        return;
    }
    New->BuildTree(Plugin->GetResourceTree(),Parent->TreeId,Index+1);
}   

void wxsPalette::InsertInto(wxsWidget* New,wxsWidget* Ref)
{
    if ( Ref->AddChild(New) < 0 )
    {
        wxsWidgetFactory::Get()->Kill(New);
        return;
    }
    New->BuildTree(Plugin->GetResourceTree(),Ref->TreeId);
}   

void wxsPalette::DeleteRequest()
{
    wxsWidget* Current = wxsPropertiesMan::Get()->GetActiveWidget();
    if ( Current == NULL )
    {
        DebLog("wxSmith: No widget selecteed - couldn't delete");
        return;
    }

    wxsWidget* Parent = Current->GetParent();
    
    if ( !Parent )
    {
        wxMessageBox("Can not delete main widget (for now ;)");
        return;
    }
    
    wxsWidgetFactory::Get()->Kill(Current);
}

void wxsPalette::PreviewRequest()
{
    if ( SelectedRes ) SelectedRes->ShowPreview();
}

void wxsPalette::SelectResource(wxsResource* Res)
{
    if ( Res )
    {
        Manager::Get()->GetMessageManager()->SetSelection(PageNum);
    }
    
    SelectedRes = Res;
    
    if ( Res && Res->CanPreview() )
    {
        FindWindow(PreviewId)->Enable(true);
    }
    else
    {
        FindWindow(PreviewId)->Enable(false);
    }
}

void wxsPalette::ResourceClosed(wxsResource* Res)
{
    if ( Res == SelectedRes )
    {
        SelectResource(NULL);
    }
}

BEGIN_EVENT_TABLE(wxsPalette,wxPanel)
    EVT_RADIOBUTTON(-1,wxsPalette::OnRadio)
    EVT_BUTTON(-1,wxsPalette::OnButton)
END_EVENT_TABLE()
