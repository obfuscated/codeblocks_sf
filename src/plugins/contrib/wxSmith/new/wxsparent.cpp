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
    // associated. It will be disabled in xml opearations since
    // it's done on parent's level
    if ( !(Flags&flXml) )
    {
        int Index = GetChildIndex(Child);
        if ( (Index >= 0) && (Index < (int)Extra.Count()) && Extra[Index] )
        {
            SubContainer(Extra[Index],Flags);
        }
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

bool wxsParent::XmlRead(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsItem::XmlRead(Elem,IsXRC,IsExtra);
    if ( IsXRC )
    {
        for ( TiXmlElement* Object = Elem->FirstChildElement(); Object; Object = Object->NextSiblingElement() )
        {
            if ( strcmp(Object->Value(),"object") ) continue;
            if ( !XmlReadChild(Object,IsXRC,IsExtra) ) Ret = false;
        }
    }
    return Ret;
}

bool wxsParent::XmlWrite(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsItem::XmlWrite(Elem,IsXRC,IsExtra);
    if ( IsXRC )
    {
        for ( size_t i=0; i<Children.Count(); i++ )
        {
            TiXmlElement* Object = Elem->InsertEndChild(TiXmlElement("object"))->ToElement();
            if ( !XmlWriteChild((int)i,Object,IsXRC,IsExtra) )
            {
                Elem->RemoveChild(Object);
                Ret = false;
            }
        }
    }
    return Ret;
}

bool wxsParent::XmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    wxString ExtraName = XmlGetExtraObjectClass();
    TiXmlElement* RealElem = Elem;

    // Finding out what's real node for item
    if ( !ExtraName.empty() )
    {
        if ( cbC2U(Elem->Attribute("class")) != ExtraName ) return false;
        RealElem = Elem->FirstChildElement("object");
        if ( !RealElem ) return false;
    }

    // Creating new item from class name
    // TODO: Add support for custom classes
    wxsItem* NewItem = wxsGEN(cbC2U(RealElem->Attribute("class")),GetResource());
    if ( !NewItem ) return false;

    // Trying to add new item to this class
    if ( !AddChild(NewItem) )
    {
        delete NewItem;
        return false;
    }

    // Loading extra data
    if ( !ExtraName.empty() )
    {
        RestoreExtraData(Children.Count()-1,Elem);
    }

    // Loading item
    return NewItem->XmlRead(RealElem,IsXRC,IsExtra);
}

bool wxsParent::XmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    wxString ExtraName = XmlGetExtraObjectClass();
    TiXmlElement* RealElem = Elem;

    // Storing extra data
    if ( !ExtraName.empty() )
    {
        RealElem = Elem->InsertEndChild(TiXmlElement("object"))->ToElement();
        Elem->SetAttribute("class",cbU2C(ExtraName));
        StoreExtraData(Index,Elem);
    }

    // Saving child item
    RealElem->SetAttribute("class",cbU2C(Children[Index]->GetInfo().Name));
    return Children[Index]->XmlWrite(RealElem,IsXRC,IsExtra);
}
