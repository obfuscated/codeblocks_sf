#include "wxsparent.h"

#include "wxsglobals.h"
#include "wxsitemfactory.h"

wxsParent::wxsParent(wxsWindowRes* Resource): wxsItem(Resource)
{
}

wxsParent::~wxsParent()
{
    for ( size_t i = Children.Count(); i-- > 0; )
    {
        wxsKILL(Children[i]);
        if ( Extra[i] )
        {
            delete Extra[i];
        }
    }
    Children.Clear();
    Extra.Clear();
}

wxsItem* wxsParent::GetChild(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= (int)Children.Count() ) return NULL;
    return Children[Index];
}

bool wxsParent::AddChild(wxsItem* Child,int Position)
{
    if ( !Child ) return false;
    if ( !CanAddChild(Child,true) ) return false;
    if ( Child->Parent != NULL )
    {
        Child->Parent->UnbindChild(Child);
    }

    Child->Parent = this;

    if ( Position<0 || Position>=GetChildCount() )
    {
        Children.Add(Child);
        Extra.Add(BuildExtra());
    }
    else
    {
        Children.Insert(Child,Position);
        Extra.Insert(BuildExtra(),Position);
    }
    return true;
}

void wxsParent::UnbindChild(int Index)
{
    if ( Index < 0 ) return;
    if ( Index >= (int)Children.Count() ) return;

    Children.RemoveAt(Index);
    if ( Extra[Index] )
    {
        delete Extra[Index];
    }
    Extra.RemoveAt(Index);
}

void wxsParent::UnbindChild(wxsItem* Child)
{
    int Index = Children.Index(Child);
    if ( Index == wxNOT_FOUND ) return;
    Children.RemoveAt(Index);
    if ( Extra[Index] )
    {
        delete Extra[Index];
    }
    Extra.RemoveAt(Index);
}

int wxsParent::MoveChild(int OldIndex,int NewIndex )
{
    if ( OldIndex < 0 ) return -1;
    if ( OldIndex >= (int)Children.Count() ) return -1;
    if ( NewIndex < 0 ) NewIndex = 0;
    if ( NewIndex >= (int)Children.Count() ) NewIndex = (int)Children.Count() - 1;

    if ( OldIndex == NewIndex ) return OldIndex;

    wxsItem* Child = Children[OldIndex];
    wxsPropertyContainer* Ext = Extra[OldIndex];
    Children.RemoveAt(OldIndex);
    Extra.RemoveAt(OldIndex);
    Children.Insert(Child,NewIndex);
    Extra.Insert(Ext,NewIndex);
    return NewIndex;
}

int wxsParent::GetChildIndex(wxsItem* Child)
{
    int Index = Children.Index(Child);
    return (Index == wxNOT_FOUND) ? -1 : Index;
}

bool wxsParent::IsGrandChild(wxsItem* Child,bool Safe)
{
    if ( !Safe )
    {
        while ( Child != NULL )
        {
            if ( Child == this ) return true;
            Child = Child->Parent;
        }
        return false;
    }

    for ( int i=0; i<GetChildCount(); i++ )
    {
        wxsItem* MyChild = GetChild(i);
        if ( MyChild == Child ) return true;
        wxsParent* MyChildParent = MyChild->ToParent();
        if ( MyChildParent && MyChildParent->IsGrandChild(Child,true) ) return true;
    }

    return false;
}

void wxsParent::StoreExtraData(int Index,TiXmlElement* Element)
{
    if ( Index < 0 ) return;
    if ( Index >= (int)Children.Count() ) return;
    if ( !Extra[Index] ) return;

    Extra[Index]->XmlWrite(Element);
}

void wxsParent::RestoreExtraData(int Index,TiXmlElement* Element)
{
    if ( Index < 0 ) return;
    if ( Index >= (int)Children.Count() ) return;
    if ( !Extra[Index] ) return;

    Extra[Index]->XmlRead(Element);
}

void wxsParent::EnumChildProperties(wxsItem* Child,long Flags)
{
    // Enumerating properties of child item
    Child->EnumItemProperties(Flags);

    // Adding properties from extra data container when there's extra data
    // associated
    int Index = GetChildIndex(Child);
    if ( (Index >= 0) && (Index < (int)Extra.Count()) && Extra[Index] )
    {
        SubContainer(Extra[Index],Flags);
    }
}

void wxsParent::AddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    // Nothing is added by default
    Child->AddItemQPP(QPP);
}

wxsPropertyContainer* wxsParent::GetChildExtra(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= (int)Extra.Count() ) return NULL;
    return Extra[Index];
}
