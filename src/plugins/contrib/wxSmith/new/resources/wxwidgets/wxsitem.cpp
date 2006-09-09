#include "wxsitem.h"
#include "wxsadvqpp.h"
#include "wxsparent.h"
#include "wxsglobals.h"
#include "wxsresourcetree.h"
#include "wxsresource.h"
#include "resources/wxswindowres.h"

#include <messagemanager.h>

wxsItem::wxsItem(wxsWindowRes* _Resource):
    Parent(NULL),
    Resource(_Resource),
    IsMember(true),
    Preview(NULL),
    m_IsSelected(false)
{
}

void wxsItem::Create()
{
    Events.SetItem(this);
}

wxsItem::~wxsItem()
{
}

void wxsItem::EnumProperties(long Flags)
{
    if ( (Flags & flPropGrid) && (Parent != NULL) )
    {
        // Parent item does take care of enumerating properties if we are
        // ceating property grid
        Parent->EnumChildProperties(this,Flags);
    }
    else
    {
        EnumItemProperties(Flags);
    }
}

wxsQuickPropsPanel* wxsItem::CreateQuickProperties(wxWindow* ParentWnd)
{
    wxsAdvQPP* Panel = new wxsAdvQPP(ParentWnd,this);

    if ( Parent != NULL )
    {
        // Parent item does take care of inserting QPP Children
        Parent->AddChildQPP(this,Panel);
    }
    else
    {
        AddItemQPP(Panel);
    }

    return Panel;
}

long wxsItem::GetPropertiesFlags()
{
    return ( Resource ? Resource->GetBasePropsFilter() : wxsFLFile )  | wxsFLVariable | wxsFLId;
}

void wxsItem::EnumItemProperties(long Flags)
{
    // Registering variable name / identifier
    // these values are skipped when storing into xml variable
    // because itis stored as element attribute
    if ( (Flags & (flPropGrid|flPropStream)) != 0 )
    {
        WXS_STRING(wxsItem,VarName,wxsFLVariable,_("Var name"),_T("var_name"),wxEmptyString,false,false);
        WXS_BOOL(wxsItem,IsMember,wxsFLVariable,_(" Is member"),_T("var_is_member"),true);
        WXS_STRING(wxsItem,IdName,wxsFLId,_("Identifier"),_T("identifier"),wxEmptyString,false,false);
    }
}

void wxsItem::BuildDeclarationCode(wxString& Code,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Code << GetInfo().Name << _T("* ") << GetVarName() << _T(";\n");
            return;
    }

    wxsLANGMSG(wxsItem::BuildDeclarationCode,Language);
}

bool wxsItem::XmlRead(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        wxsPropertyContainer::XmlRead(Element);
        IdName = cbC2U(Element->Attribute("name"));
    }

    if ( IsExtra )
    {
        VarName = cbC2U(Element->Attribute("variable"));
        const char* MbrText = Element->Attribute("member");
        IsMember = !MbrText || !strcmp(MbrText,"yes");
        Events.XmlLoadFunctions(Element);
    }

    return true;
}

bool wxsItem::XmlWrite(TiXmlElement* Element,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        wxsPropertyContainer::XmlWrite(Element);
        if ( GetPropertiesFlags() & wxsFLId )
        {
            Element->SetAttribute("name",cbU2C(IdName));
        }
    }

    if ( IsExtra )
    {
        if ( GetPropertiesFlags() & wxsFLVariable )
        {
            Element->SetAttribute("variable",cbU2C(VarName));
            Element->SetAttribute("member",IsMember ? "yes" : "no" );
        }
        Events.XmlSaveFunctions(Element);
    }

    return true;
}

void wxsItem::BuildItemTree(wxTreeCtrl* Tree,wxTreeItemId Parent,int Position)
{
    if ( Position<0 || Position>=(int)wxsTREE()->GetChildrenCount(Parent) )
    {
        LastTreeId = Tree->AppendItem(Parent,GetInfo().Name,-1,-1,new wxsResourceTreeData(this));
    }
    else
    {
        LastTreeId = Tree->InsertItem(Parent,Position,GetInfo().Name,-1,-1,new wxsResourceTreeData(this));
    }
    if ( !GetIsExpanded() )
    {
        Tree->Collapse(LastTreeId);
    }

    wxsParent* ParentItem = ToParent();
    if ( ParentItem )
    {
        int Count = ParentItem->GetChildCount();
        for ( int i=0; i<Count; i++ )
        {
            ParentItem->GetChild(i)->BuildItemTree(Tree,LastTreeId);
        }
    }
}

wxObject* wxsItem::BuildPreview(wxWindow* Parent,bool Exact)
{
    wxObject* P = DoBuildPreview(Parent,Exact);
    if ( !Exact )
    {
        // Not in exact mode, so we're building editor's content and we must
        // associate it with internal preview pointer
        Preview = P;
    }
    return P;
}

void wxsItem::InvalidatePreview()
{
    Preview = NULL;
    wxsParent* Parent = ToParent();
    if ( Parent )
    {
        for ( int i=Parent->GetChildCount(); i-->0; )
        {
            Parent->GetChild(i)->InvalidatePreview();
        }
    }
}

void wxsItem::ClearSelection()
{
    IsSelected = false;
    wxsParent* Parent = ToParent();
    if ( Parent )
    {
        for ( int i = Parent->GetChildCount(); i-->0; )
        {
            Parent->GetChild(i)->ClearSelection();
        }
    }
}

void wxsItem::PropertyChangedHandler()
{
    GetResource()->NotifyChange(this);
}

void wxsItem::SubPropertyChangedHandler(wxsPropertyContainer*)
{
    GetResource()->NotifyChange(this);
}
