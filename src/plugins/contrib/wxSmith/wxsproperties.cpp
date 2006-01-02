#include "wxsheaders.h"
#include "wxsproperties.h"

#include "properties/wxsstringproperty.h"
#include "properties/wxsintproperty.h"
#include "properties/wxsboolproperty.h"
#include "properties/wxs2intproperty.h"
#include "properties/wxsstringlistproperty.h"
#include "widget.h"

#include <wx/stattext.h>

class wxsPropertyGrid: public wxPropertyGrid
{
	public:
        wxsPropertyGrid(wxWindow* Parent,wxsProperties* _Props):
            wxPropertyGrid(Parent,-1),
            Props(_Props)
        {}

        ~wxsPropertyGrid()
        {
            if ( Props ) Props->Grid = NULL;
        }

	private:

        void OnChange(wxPropertyGridEvent& event)
        {
            if ( !Props ) return;
            for ( wxsProperties::VectorI i = Props->Properties.begin(); i != Props->Properties.end(); ++i )
            {
                if ( !(*i)->Property->PropGridChanged(this,event.GetProperty()) )
                {
                    SelectProperty(event.GetProperty(),true);
                }
            }
        }

        wxsProperties* Props;
        DECLARE_EVENT_TABLE()
        friend class wxsProperties;
};

BEGIN_EVENT_TABLE(wxsPropertyGrid,wxPropertyGrid)
    EVT_PG_CHANGED(-1,wxsPropertyGrid::OnChange)
END_EVENT_TABLE()

wxsProperties::wxsProperties(wxsWidget* _Widget):
    Widget(_Widget),
    Grid(NULL),
    BlockUpdates(false)
{
}

bool wxsProperty::ValueChanged(bool Check)
{
    return Props ? Props->NotifyChange(Check) : true;
}

wxsProperties::~wxsProperties()
{
    ClearArray();
    if ( Grid ) { Grid->Props = NULL; }
}

void wxsProperties::AddProperty(const wxString& Name,wxString& Value,int Position)
{
    AddProperty(Name,new wxsStringProperty(Value,true),Position);
}

void wxsProperties::AddProperty(const wxString& Name,int& Value,int Position)
{
    AddProperty(Name,new wxsIntProperty(Value),Position);
}

void wxsProperties::AddProperty(const wxString& Name,bool& Value,int Position)
{
    AddProperty(Name,new wxsBoolProperty(Value),Position);
}

void wxsProperties::Add2IProperty(const wxString& Name,int& Value1,int& Value2,int Position)
{
    AddProperty(Name,new wxs2IntProperty(Value1,Value2),Position);
}

void wxsProperties::AddProperty(const wxString& Name,wxArrayString& Array,int Position)
{
	AddProperty(Name,new wxsStringListProperty(Array),Position);
}

void wxsProperties::AddProperty(const wxString& Name,wxArrayString& Array,int& Selected,int SortedFlag,int Position)
{
	AddProperty(Name,new wxsStringListProperty(Array,Selected,SortedFlag),Position);
}

void wxsProperties::AddProperty(const wxString& Name,wxsProperty* Prop,int Position)
{
    if ( !Prop ) return;
    if ( !Name.Length() ) return;

    VectorElem* NewElem = new VectorElem;;
    NewElem->Name = Name;
    NewElem->Property = Prop;
    Prop->Props = this;

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
    if ( !Grid ) return;
    if ( BlockUpdates ) return;
    BlockUpdates = true;
    for ( VectorI i = Properties.begin(); i!=Properties.end(); ++i )
    {
        (*i)->Property->UpdatePropGrid(Grid);
    }
    BlockUpdates = false;
}

wxWindow* wxsProperties::GenerateWindow(wxWindow* Parent)
{
    Grid = new wxsPropertyGrid(Parent,this);
    for ( VectorI i = Properties.begin(); i!=Properties.end(); ++i )
    {
        (*i)->Property->AddToPropGrid(Grid,(*i)->Name);
    }
    return Grid;
}

wxWindow* wxsProperties::GetWindow()
{
    return Grid;
}

void wxsProperties::ClearArray()
{
    for ( VectorI i = Properties.begin(); i!=Properties.end(); ++i )
        delete (*i);

    Properties.clear();
}

bool wxsProperties::NotifyChange(bool Check)
{
	if ( BlockUpdates ) return true;
    assert ( Widget != NULL );
    BlockUpdates = true;
    bool Result = Widget->PropertiesChanged(Check,false);
    BlockUpdates = false;
    return Result;
}
