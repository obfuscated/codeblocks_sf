#include "wxsheaders.h"
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

wxsPalette* wxsPalette::Singleton = NULL;

wxsPalette::wxsPalette(wxWindow* Parent,int PN):
    wxPanel(Parent),
    SelectedRes(NULL),
    SelectedWidget(NULL),
    Header(this),
    PageNum(PN)
{
	wxScrolledWindow* Scroll = new wxScrolledWindow(this,-1);
	Scroll->SetScrollRate(5,5);

	wxFlexGridSizer* Sizer = new wxFlexGridSizer(0,1,5,5);
	Sizer->AddGrowableCol(0);
	Sizer->AddGrowableRow(1);

	Sizer->Add(&Header,0,wxALL|wxGROW,0);

	wxPanel* WidgetsSpace = new wxPanel(Scroll,-1);

	CreateWidgetsPalette(WidgetsSpace);

	Sizer->Add(WidgetsSpace,0,wxALL|wxGROW,10);

	Scroll->SetSizer(Sizer);
	Sizer->SetVirtualSizeHints(Scroll);

	wxSizer* TopSizer = new wxBoxSizer(wxVERTICAL);

	TopSizer->Add(Scroll,1,wxGROW);
	SetSizer(TopSizer);

	Singleton = this;

	SetInsertionTypeMask(0);

	SelectedRes = NULL;
	Header.ButtonPreview->Disable();

	Timer.SetOwner(this);

	Timer.Start(100,false);
}

wxsPalette::~wxsPalette()
{
	//dtor
}

namespace {
struct ltstr {  bool operator()(const wxChar* s1, const wxChar* s2) const { return wxStricmp(s1, s2) < 0; } };
};

void wxsPalette::CreateWidgetsPalette(wxWindow* Wnd)
{
    // First we need to split all widgets into groups
    // it will be done using multimap


    typedef std::multimap<const wxChar*,const wxsWidgetInfo*,ltstr> MapT;
    typedef MapT::iterator MapI;

    MapT Map;

    for ( const wxsWidgetInfo* Info = wxsFACTORY()->GetFirstInfo(); Info; Info = wxsFACTORY()->GetNextInfo() )
        Map.insert(std::pair<const wxChar*,const wxsWidgetInfo*>(Info->Category,Info));

    // Creatign main sizer inside window
    wxFlexGridSizer* Sizer = new wxFlexGridSizer(0,1,0,0);
    Sizer->AddGrowableCol(0);

    wxFlexGridSizer* RowSizer = NULL;

    const wxChar* PreviousGroup = _T("");

    for ( MapI i = Map.begin(); i != Map.end(); ++i )
    {
        if ( !(*i).first || wxStricmp(PreviousGroup,(*i).first) )
        {
            if ( RowSizer ) Sizer->Add(RowSizer,0,wxALL|wxGROW,5);
            Sizer->Add(new wxStaticLine(Wnd,-1),0,wxGROW);

            if ( (*i).first && (*i).first[0] )
            {
                // Need to create new group
                RowSizer = new wxFlexGridSizer(1,0,5,0);
                RowSizer->Add(new wxStaticText(Wnd,-1,(*i).first),0,wxALIGN_CENTER|wxALL,5);
                PreviousGroup = (*i).first;
            }
            else
            {
                RowSizer = NULL;
            }
        }

        if ( RowSizer )
        {
            if ( (*i).second->Icon )
            {
                wxBitmapButton* Btn =
                    new wxBitmapButton(Wnd,-1,*(*i).second->Icon,
                        wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW,
                        wxDefaultValidator, (*i).second->Name);
                RowSizer->Add(Btn,0,wxGROW);
                Btn->SetToolTip((*i).second->Name);
            }
            else
            {
                wxButton* Btn = new wxButton(Wnd,-1,(*i).second->Name,
                    wxDefaultPosition,wxDefaultSize,0,
                    wxDefaultValidator,(*i).second->Name);
                RowSizer->Add(Btn,0,wxGROW);
                Btn->SetToolTip((*i).second->Name);
            }
        }
    }

    if ( RowSizer ) Sizer->Add(RowSizer,0,wxALL|wxGROW,5);

    Wnd->SetSizer(Sizer);
    Sizer->SetSizeHints(Wnd);
}

void wxsPalette::OnButton(wxCommandEvent& event)
{
    wxWindow* Btn = (wxWindow*)event.GetEventObject();
    if ( Btn )
    {
        InsertRequest(Btn->GetName());
    }
}

void wxsPalette::InsertRequest(const wxString& Name)
{
    wxsWidget* Current = wxsPropertiesMan::Get()->GetActiveWidget();
    if ( Current == NULL )
    {
        DebLog(_("wxSmith: No widget selected - couldn't create new widget"));
        return;
    }

    if ( !GetInsertionType() )
    {
        return;
    }

    wxsWindowEditor* Edit = Current->GetEditor();

    wxsWidget* NewWidget = wxsGEN(Name,Current->GetResource());
    if ( NewWidget == NULL )
    {
        DebLog(_("wxSmith: Culdn't generate widget inside factory"));
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
            wxsKILL(NewWidget);
            DebLog(_("Something went wrong"));
            break;
    }


    if ( Edit )
    {
        Edit->BuildPreview();
    }

    if ( SelectedRes )
    {
		SelectedRes->NotifyChange();
    }
}

void wxsPalette::InsertBefore(wxsWidget* New,wxsWidget* Ref)
{
    wxsWidget* Parent = Ref->GetParent();

    int Index;

    if ( !Parent || (Index=Parent->FindChild(Ref)) < 0 || Parent->AddChild(New,Index) < 0 )
    {
        wxsKILL(New);
        return;
    }

    // Adding this new item into resource tree

    New->BuildTree(wxsTREE(),Parent->TreeId,Index);
    wxsTREE()->Refresh();
}

void wxsPalette::InsertAfter(wxsWidget* New,wxsWidget* Ref)
{
    wxsWidget* Parent = Ref->GetParent();

    int Index;

    if ( !Parent || (Index=Parent->FindChild(Ref)) < 0 || Parent->AddChild(New,Index+1) < 0 )
    {
        wxsKILL(New);
        return;
    }
    New->BuildTree(wxsTREE(),Parent->TreeId,Index+1);
    wxsTREE()->Refresh();
}

void wxsPalette::InsertInto(wxsWidget* New,wxsWidget* Ref)
{
    if ( Ref->AddChild(New) < 0 )
    {
        wxsKILL(New);
        return;
    }
    New->BuildTree(wxsTREE(),Ref->TreeId);
    wxsTREE()->Refresh();
}

void wxsPalette::DeleteRequest()
{

    wxsWidget* Current = wxsPropertiesMan::Get()->GetActiveWidget();
    if ( Current == NULL )
    {
        DebLog(_("wxSmith: No widget selecteed - couldn't delete"));
        return;
    }

    wxsWidget* Parent = Current->GetParent();

    if ( !Parent )
    {
        wxMessageBox(_("Can not delete main widget (for now ;)"));
        return;
    }

    wxsWindowEditor* Edit = Current->GetEditor();

    if ( Edit )
    {
        Edit->KillPreview();
    }

    wxsKILL(Current);

    if ( Edit )
    {
        Edit->BuildPreview();
    }

    if ( SelectedRes )
    {
		SelectedRes->NotifyChange();
    }
    wxsTREE()->Refresh();
}

void wxsPalette::PreviewRequest()
{
    if ( SelectedRes ) SelectedRes->ShowPreview();
}

void wxsPalette::OnSelectWidget(wxsEvent& event)
{
    int itMask = 0;

    if ( event.GetWidget()->GetParent() &&
         ( !event.GetWidget()->GetInfo().Sizer ||
            event.GetWidget()->GetParent()->GetInfo().Sizer ) )
    {
        itMask |= wxsPalette::itBefore | wxsPalette::itAfter;
    }

    if ( event.GetWidget()->IsContainer() )
    {
        itMask |= wxsPalette::itInto;
    }

    SetInsertionTypeMask(itMask);
    SelectedWidget = event.GetWidget();
    Header.SeletionChanged(SelectedWidget);
}

void wxsPalette::OnUnselectWidget(wxsEvent& event)
{
    if ( event.GetWidget() == SelectedWidget )
    {
        SetInsertionTypeMask(0);
        SelectedWidget = NULL;
        Header.SeletionChanged(NULL);
    }
}

void wxsPalette::OnSelectRes(wxsEvent& event)
{
    wxsResource* Res = event.GetResource();

    if ( Res )
    {
        Manager::Get()->GetMessageManager()->SetSelection(PageNum);
        Res->EditOpen();
    }

    SelectedRes = Res;

    if ( Res && Res->CanPreview() )
    {
        Header.ButtonPreview->Enable();
    }
    else
    {
        Header.ButtonPreview->Disable();
    }
}

void wxsPalette::OnUnselectRes(wxsEvent& event)
{
    if ( event.GetResource() == SelectedRes )
    {
        SelectedRes = NULL;
        Header.ButtonPreview->Disable();
    }
}

void wxsPalette::OnTimer(wxTimerEvent& event)
{
	wxsCoder::Get()->ProcessCodeQueue();
}

BEGIN_EVENT_TABLE(wxsPalette,wxPanel)
    EVT_BUTTON(-1,wxsPalette::OnButton)
    EVT_SELECT_RES(wxsPalette::OnSelectRes)
    EVT_UNSELECT_RES(wxsPalette::OnUnselectRes)
    EVT_SELECT_WIDGET(wxsPalette::OnSelectWidget)
    EVT_UNSELECT_WIDGET(wxsPalette::OnUnselectWidget)
    EVT_TIMER(-1,wxsPalette::OnTimer)
END_EVENT_TABLE()
