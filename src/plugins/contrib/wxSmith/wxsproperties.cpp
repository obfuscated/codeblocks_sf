#include "wxsproperties.h"

#include "properties/wxsstringproperty.h"
#include "properties/wxsintproperty.h"
#include "properties/wxsboolproperty.h"
#include "properties/wxs2intproperty.h"
#include "widget.h"

#include <wx/stattext.h>

void wxsProperty::ValueChanged()
{
    if ( Props ) Props->NotifyChange(this);
}

wxsProperties::wxsProperties(wxsWidget* _Widget):
    Widget(_Widget),
    BlockUpdates(false)
{
}

wxsProperties::~wxsProperties()
{
    ClearArray();
}

void wxsProperties::AddProperty(const wxString& Name,wxString& Value,int Position,bool Rs,bool Rc)
{
    AddProperty(Name,new wxsStringProperty(this,Value,true),Position,Rs,Rc);
}

void wxsProperties::AddProperty(const wxString& Name,int& Value,int Position,bool Rs,bool Rc)
{
    AddProperty(Name,new wxsIntProperty(this,Value,true),Position,Rs,Rc);
}

void wxsProperties::AddProperty(const wxString& Name,bool& Value,int Position,bool Rs,bool Rc)
{
    AddProperty(Name,new wxsBoolProperty(this,Value),Position,Rs,Rc);
}
		
void wxsProperties::Add2IProperty(const wxString& Name,int& Value1,int& Value2,int Position,bool Rs,bool Rc)
{
    AddProperty(Name,new wxs2IntProperty(this,Value1,Value2),Position,Rs,Rc);
}
		
void wxsProperties::AddProperty(const wxString& Name,wxsProperty* Prop,int Position,bool Rs,bool Rc)
{
    if ( !Prop ) return;
    if ( !Name.Length() ) return;
    
    VectorElem* NewElem = new VectorElem;;
    NewElem->Name = Name;
    NewElem->Property = Prop;
    Prop->IsReshaping = Rs;
    Prop->IsRecreating = Rc;
    
    if ( Position < 0 || Position >= (int)Properties.size() ) 
    {
        Properties.push_back(NewElem);
    }
    else
    {
        Properties.insert(Properties.begin()+Position,NewElem);
    }
}

void wxsProperties::UpdateProperties()
{
    if ( BlockUpdates ) return;
    BlockUpdates = true;
    for ( VectorI i = Properties.begin(); i!=Properties.end(); ++i )
    {
        (*i)->Property->UpdateEditWindow();
    }
    BlockUpdates = false;
}

wxWindow* wxsProperties::GenerateWindow(wxWindow* Parent,wxSizer** SizerPtr)
{
    wxPanel* Panel = new wxPanel(Parent,-1);
    wxFlexGridSizer* Sizer = new wxFlexGridSizer(2,5,5);
    Sizer->AddGrowableCol(1);
    for ( VectorI i = Properties.begin(); i!=Properties.end(); ++i )
    {
        wxWindow* Editor = (*i)->Property->BuildEditWindow(Panel);
        if ( Editor )
        {
            Sizer->Add(new wxStaticText(Panel,-1,(*i)->Name),0,wxALIGN_CENTRE_VERTICAL);
            Sizer->Add(Editor,0,wxGROW);
        }
    }
    Panel->SetSizer(Sizer);
    Panel->Layout();
    Sizer->SetSizeHints(Panel);
    
    if ( SizerPtr ) *SizerPtr = Sizer;
    return Panel;
}

void wxsProperties::ClearArray()
{
    for ( VectorI i = Properties.begin(); i!=Properties.end(); ++i )
        delete (*i);
        
    Properties.clear();
}

void wxsProperties::NotifyChange(wxsProperty* Prop)
{
    if ( !BlockUpdates ) 
    {
        assert ( Widget != NULL );
        assert ( Prop != NULL );
        BlockUpdates = true;
        Widget->UpdatePreview(Prop->IsReshaping,Prop->IsRecreating);
        BlockUpdates = false;
    }
}
        
