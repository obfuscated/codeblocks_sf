#include "wxspropertygridmanager.h"

#include "wxspropertycontainer.h"

IMPLEMENT_CLASS(wxsPropertyGridManager,wxPropertyGridManager)

wxsPropertyGridManager::wxsPropertyGridManager(
    wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxChar* name):
        wxPropertyGridManager(parent,id,pos,size,style,name),
        MainContainer(NULL)
{
    Singleton = this;
}

wxsPropertyGridManager::~wxsPropertyGridManager()
{
    PGIDs.Clear();
    PGEnteries.Clear();
    PGIndexes.Clear();
    PGContainers.Clear();
    PGContainersSet.clear();
    ClearPage(0);
    PreviousIndex = -1;
    PreviousProperty = NULL;
    if ( Singleton == this )
    {
        Singleton = NULL;
    }
}

void wxsPropertyGridManager::OnChange(wxPropertyGridEvent& event)
{
    wxPGId ID = event.GetProperty();
    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        if ( PGIDs[i] == ID )
        {
            wxsPropertyContainer* Container = PGContainers[i];
            PGEnteries[i]->PGRead(Container,this,ID,PGIndexes[i]);

            // Notifying about property change
            Container->NotifyPropertyChangeFromPropertyGrid();

            // Notifying about sub property change
            if ( Container!=MainContainer && MainContainer!=NULL )
            {
                MainContainer->SubPropertyChangedHandler(Container);
            }
            break;
        }
    }

    Update(NULL);
}

void wxsPropertyGridManager::Update(wxsPropertyContainer* PC)
{
    if ( PC && PGContainersSet.find(PC) == PGContainersSet.end() )
    {
        // This container is not used here
        return;
    }

    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        PGEnteries[i]->PGWrite(PGContainers[i],this,PGIDs[i],PGIndexes[i]);
    }
}

void wxsPropertyGridManager::UnbindAll()
{
    PGIDs.Clear();
    PGEnteries.Clear();
    PGIndexes.Clear();
    PGContainers.Clear();
    PGContainersSet.clear();
    ClearPage(0);
    PreviousIndex = -1;
    PreviousProperty = NULL;
    SetNewMainContainer(NULL);
}

void wxsPropertyGridManager::UnbindPropertyContainer(wxsPropertyContainer* PC)
{
    if ( PGContainersSet.find(PC) == PGContainersSet.end() )
    {
        // This container is not used here
        return;
    }

    if ( PC == MainContainer )
    {
        // Main container unbinds all
        UnbindAll();
        return;
    }

    Freeze();
    for ( size_t i = PGIDs.Count(); i-- > 0; )
    {
        if ( PGContainers[i] == PC )
        {
            Delete(PGIDs[i]);
            PGIDs.RemoveAt(i);
            PGEnteries.RemoveAt(i);
            PGIndexes.RemoveAt(i);
            PGContainers.RemoveAt(i);
        }
    }
    Thaw();

    // If there are no properties, we have unbinded main property container
    if ( !PGIDs.Count() )
    {
        SetNewMainContainer(NULL);
    }
}

long wxsPropertyGridManager::Register(wxsPropertyContainer* Container,wxsProperty* Property,wxPGId Id,long Index)
{
    if ( !Property ) return -1;

    if ( Property != PreviousProperty )
    {
        PreviousIndex = -1;
    }

    if ( Index < 0 )
    {
        Index = ++PreviousIndex;
    }

    PGEnteries.Add(Property);
    PGIDs.Add(Id);
    PGIndexes.Add(Index);
    PGContainers.Add(Container);
    PGContainersSet.insert(Container);
    return Index;
}

void wxsPropertyGridManager::SetNewMainContainer(wxsPropertyContainer* Container)
{
    MainContainer = Container;
    OnContainerChanged(MainContainer);
}

wxsPropertyGridManager* wxsPropertyGridManager::Singleton = NULL;

BEGIN_EVENT_TABLE(wxsPropertyGridManager,wxPropertyGridManager)
    EVT_PG_CHANGED(-1,wxsPropertyGridManager::OnChange)
END_EVENT_TABLE()
