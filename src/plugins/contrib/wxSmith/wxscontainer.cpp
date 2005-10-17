#include "wxscontainer.h"

#include "wxswidgetfactory.h"

wxsContainer::~wxsContainer()
{
    DeletingAll = true;
    for ( WidgetsI i = Widgets.begin(); i!=Widgets.end(); ++i )
    {
        wxsFACTORY()->Kill(*i);
    }
    for ( ExtraI i = Extra.begin(); i!=Extra.end(); ++i )
    {
    	DelExtra(*i);
    }
    DeletingAll = false;
}

int wxsContainer::FindChild(wxsWidget* Widget,int Level)
{
    if ( Level == 1 )
    {
        int Cnt = 0;
        for ( WidgetsI i = Widgets.begin(); i!=Widgets.end(); ++i, ++Cnt )
        {
            if ( (*i)==Widget )
                return Cnt;
        }
        return -1;
    }

    while ( Widget != this )
    {
        Widget = Widget->GetParent();
        if ( --Level == 0 ) break;
    }

    return ( Widget == this ) ? 0 : -1;
}

int wxsContainer::AddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
    if ( NewWidget == NULL ) return -1;
    NewWidget->Parent = this;
    if ( InsertBeforeThis < 0 || InsertBeforeThis >= (int)Widgets.size() )
    {
        Widgets.push_back(NewWidget);
        Extra.push_back(NewExtra());
        return Widgets.size() - 1;
    }
    Widgets.insert(Widgets.begin() + InsertBeforeThis,NewWidget);
    Extra.insert(Extra.begin() + InsertBeforeThis,NewExtra());
    return InsertBeforeThis;
}

bool wxsContainer::DelChildId(int Id)
{
    if ( DeletingAll ) return false;
    if ( Id<0 || Id>=(int)Widgets.size() ) return false;
    Widgets.erase(Widgets.begin()+Id);
    DelExtra(Extra[Id]);
    Extra.erase(Extra.begin()+Id);
    return true;
}

bool wxsContainer::DelChild(wxsWidget* Widget)
{
    if ( DeletingAll ) return false;
    WidgetsI i = Widgets.begin();
    ExtraI ie = Extra.begin();
    for ( ; i!=Widgets.end(); ++i, ++ie )
        if ( (*i)==Widget )
        {
            Widgets.erase(i);
            DelExtra(*ie);
            Extra.erase(ie);
            return true;
        }
    return false;
}

bool wxsContainer::ChangeChildPos(int PrevPos,int NewPos)
{
    if ( PrevPos<0 || PrevPos >= (int)Widgets.size() ) return false;
    if ( PrevPos < NewPos ) --NewPos;
    if ( NewPos<0 || NewPos>=(int)Widgets.size() ) return false;
    if ( PrevPos == NewPos ) return true;

    wxsWidget* Changing = Widgets[PrevPos];
    void* eChanging = Extra[PrevPos];

    if ( PrevPos < NewPos )
    {
        while ( PrevPos++ < NewPos )
        {
            Widgets[PrevPos-1] = Widgets[PrevPos];
            Extra[PrevPos-1] = Extra[PrevPos];
        }
    }
    else
    {
        while ( PrevPos-- > NewPos )
        {
            Widgets[PrevPos+1] = Widgets[PrevPos];
            Extra[PrevPos+1] = Extra[PrevPos];
        }
    }
    Widgets[NewPos] = Changing;
    Extra[NewPos] = eChanging;
    return true;
}
