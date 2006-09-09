#include "wxscorrector.h"
#include "../wxsitem.h"
#include "../wxsparent.h"
#include "../resources/wxswindowres.h"


wxsCorrector::wxsCorrector(wxsWindowRes* Resource): Res(Resource), NeedRebuild(true)
{
}

bool wxsCorrector::GlobalCheck()
{
    // It will be done in two passes,
    // first will correct all invalid names, second will fill empty names

    wxsItem* RootItem = Res->GetRootItem();
    Vars.clear();
    Ids.clear();
    bool AreInvalid = FixAfterLoadCheckNames(RootItem);
    bool AreEmpty = FillEmpty(RootItem);

    NeedRebuild = false;

    if ( AreInvalid || AreEmpty ) return true;
    return false;
}

bool wxsCorrector::FixAfterLoadCheckNames(wxsItem* Item)
{
    bool Ret = false;
    if ( Item->GetPropertiesFlags() & wxsFLVariable )
    {
        wxString VarName = Item->GetVarName();
        if ( FixVarName(VarName) )
        {
            Ret = true;
            Item->SetVarName(VarName);
        }
        if ( Vars.find(VarName)!=Vars.end() )
        {
            Ret = true;
            Item->SetVarName(wxEmptyString);
        }
        else
        {
            Vars.insert(VarName);
        }
    }
    if ( Item->GetPropertiesFlags() & wxsFLId )
    {
        wxString IdName = Item->GetIdName();
        if ( FixIdName(IdName) )
        {
            Ret = true;
            Item->SetIdName(IdName);
        }
        if ( !wxsPredefinedIDs::Check(IdName) )
        {
            if ( Ids.find(IdName)!=Ids.end() )
            {
                Ret = true;
                Item->SetIdName(wxEmptyString);
            }
            else
            {
                Ids.insert(IdName);
            }
        }
    }

    wxsParent* Parent = Item->ToParent();
    if ( Parent )
    {
        int Count = Parent->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            if ( FixAfterLoadCheckNames(Parent->GetChild(i)) ) Ret = true;
        }
    }
    return Ret;
}

bool wxsCorrector::FillEmpty(wxsItem* Item)
{
    bool Ret = false;
    if ( Item->GetPropertiesFlags() & wxsFLVariable )
    {
        if ( Item->GetVarName().empty() )
        {
            Ret = true;
            SetNewVarName(Item);
            Vars.insert(Item->GetVarName());
        }
    }
    if ( Item->GetPropertiesFlags() & wxsFLId )
    {
        if ( Item->GetIdName().empty() )
        {
            Ret = true;
            SetNewIdName(Item);
            Ids.insert(Item->GetIdName());
        }
    }

    wxsParent* Parent = Item->ToParent();
    if ( Parent )
    {
        int Count = Parent->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            if ( FillEmpty(Parent->GetChild(i)) ) Ret = true;
        }
    }
    return Ret;
}

void wxsCorrector::AfterChange(wxsItem* Item)
{
    // Building new sets without given item
    wxsItem* RootItem = Res->GetRootItem();
    Vars.clear();
    Ids.clear();
    RebuildSetsReq(RootItem,Item);

    if ( Item->GetPropertiesFlags() & wxsFLVariable )
    {
        wxString VarName = Item->GetVarName();
        if ( FixVarName(VarName) )
        {
            Item->SetVarName(VarName);
        }
        if ( VarName.empty() || (Vars.find(VarName) != Vars.end()) )
        {
            SetNewVarName(Item);
        }
        Vars.insert(Item->GetVarName());
    }

    if ( Item->GetPropertiesFlags() & wxsFLId )
    {
        wxString IdName = Item->GetIdName();
        if ( FixIdName(IdName) )
        {
            Item->SetIdName(IdName);
        }
        if ( !wxsPredefinedIDs::Check(IdName) )
        {
            if ( Ids.empty() || (Ids.find(IdName) != Ids.end()) )
            {
                SetNewIdName(Item);
            }
            Ids.insert(Item->GetIdName());
        }
    }

    NeedRebuild = false;
}

void wxsCorrector::RebuildSets()
{
    // TODO: Uncomment when data integrity is sure
//    if ( !NeedRebuild ) return;
    Vars.clear();
    Ids.clear();
    RebuildSetsReq(Res->GetRootItem(),NULL);
    NeedRebuild = false;
}

void wxsCorrector::RebuildSetsReq(wxsItem* Item,wxsItem* Exclude)
{
    if ( Item != Exclude )
    {
        if ( Item->GetPropertiesFlags() & wxsFLVariable )
        {
            Vars.insert(Item->GetVarName());
        }

        if ( Item->GetPropertiesFlags() & wxsFLId )
        {
            wxString Id = Item->GetIdName();
            if ( !wxsPredefinedIDs::Check(Id) )
            {
                Ids.insert(Id);
            }
        }
    }

    wxsParent* P = Item->ToParent();
    if ( P )
    {
        for ( int i=P->GetChildCount(); i-->0; )
        {
            RebuildSetsReq(P->GetChild(i),Exclude);
        }
    }
}

void wxsCorrector::SetNewVarName(wxsItem* Item)
{
    wxString Prefix = Item->GetInfo().DefaultVarName;
    wxString NewName;
    for ( int i=1;; i++ )
    {
        NewName = wxString::Format(_T("%s%d"),Prefix.c_str(),i);
        if ( Vars.find(NewName) == Vars.end() ) break;
    }
    Item->SetVarName(NewName);
}

void wxsCorrector::SetNewIdName(wxsItem* Item)
{
    wxString Prefix = _T("ID_");
    Prefix << Item->GetInfo().DefaultVarName.Upper();
    wxString NewName;
    for ( int i=1;; i++ )
    {
        NewName = wxString::Format(_T("%s%d"),Prefix.c_str(),i);
        if ( Ids.find(NewName) == Ids.end() ) break;
    }
    Item->SetIdName(NewName);
}

void wxsCorrector::BeforePaste(wxsItem* Item)
{
    RebuildSets();
    BeforePasteReq(Item);
}

void wxsCorrector::BeforePasteReq(wxsItem* Item)
{
    if ( Item->GetPropertiesFlags() & wxsFLVariable )
    {
        wxString VarName = Item->GetVarName();
        if ( FixVarName(VarName) )
        {
            Item->SetVarName(VarName);
        }
        if ( VarName.empty() || (Vars.find(VarName) != Vars.end()) )
        {
            SetNewVarName(Item);
        }
        Vars.insert(Item->GetVarName());
    }

    if ( Item->GetPropertiesFlags() & wxsFLId )
    {
        wxString IdName = Item->GetIdName();
        if ( FixIdName(IdName) )
        {
            Item->SetIdName(IdName);
        }
        if ( !wxsPredefinedIDs::Check(IdName) )
        {
            if ( Ids.empty() || (Ids.find(IdName) != Ids.end()) )
            {
                SetNewIdName(Item);
            }
            Ids.insert(Item->GetIdName());
        }
    }

    wxsParent* P = Item->ToParent();
    if ( P )
    {
        int Count = P->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            BeforePasteReq(P->GetChild(i));
        }
    }
}

void wxsCorrector::AfterPaste(wxsItem* Item)
{
    // TODO: Add new items into current sets
}

bool wxsCorrector::FixVarName(wxString& Name)
{
    wxString Corrected;
    Name.Trim(true);
   	Name.Trim(false);

   	if ( !Name.empty() )
   	{
        // Validating name as C++ ideentifier

        static wxString FirstChar(
            _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
            _T("abcdefghijklmnopqrstuvwxyz")
            _T("_"));

        if ( FirstChar.Find(Name.GetChar(0)) == -1 )
        {
            DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),Name.c_str(),Name.GetChar(0),0);
        }
        else
        {
            Corrected.Append(Name.GetChar(0));
        }

        static wxString NextChars(
            _T("0123456789")
            _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
            _T("abcdefghijklmnopqrstuvwxyz")
            _T("_"));

        for ( size_t i=1; i<Name.Length(); ++i )
        {
            if ( NextChars.Find(Name.GetChar(i)) == -1 )
            {
                DBGLOG(_T("wxSmith: Variable name : \"%s\" is not a valid c++ identifier (invalid character \"%c\" at position %d)"),Name.c_str(),Name.GetChar(i),i);
            }
            else
            {
                Corrected.Append(Name.GetChar(i));
            }
        }
   	}

   	bool Diff = Name != Corrected;
   	Name = Corrected;
   	return Diff;
}

bool wxsCorrector::FixIdName(wxString& Id)
{
    Id.Trim(true);
    Id.Trim(false);

    long Tmp;
    if ( Id.ToLong(&Tmp,10) ) return false;

    // We'll use FixVarName's routines to correct identifier
    return FixVarName(Id);
}

void wxsCorrector::ClearCache()
{
    NeedRebuild = true;
    Vars.clear();
    Ids.clear();
}
