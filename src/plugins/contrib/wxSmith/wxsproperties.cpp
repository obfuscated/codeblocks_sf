#include "wxsheaders.h"
#include "wxsproperties.h"

#include "properties/wxsstringproperty.h"
#include "properties/wxsintproperty.h"
#include "properties/wxsboolproperty.h"
#include "properties/wxs2intproperty.h"
#include "properties/wxsstringlistproperty.h"
#include "widget.h"

#include <wx/stattext.h>

#ifndef __NO_PROPGRGID

class wxsPropertyGrid: public wxPropertyGrid
{
	public:
        wxsPropertyGrid(wxWindow* Parent,wxsProperties* _Props):
            wxPropertyGrid(Parent,-1),
            Props(_Props)
        {}

	private:

        void OnChange(wxPropertyGridEvent& event);

        wxsProperties* Props;

        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsPropertyGrid,wxPropertyGrid)
    EVT_PG_CHANGED(-1,wxsPropertyGrid::OnChange)
END_EVENT_TABLE()

void wxsPropertyGrid::OnChange(wxPropertyGridEvent& event)
{
	bool Refresh = false;
	for ( wxsProperties::VectorI i = Props->Properties.begin(); i != Props->Properties.end(); ++i )
	{
		if ( !(*i)->Property->PropGridChanged(this,event.GetProperty()) )
		{
			SelectProperty(event.GetProperty(),true);
		}
		else
		{
			Refresh = true;
		}
	}
}

#endif

bool wxsProperty::ValueChanged(bool Change)
{
    if ( Props ) return Props->NotifyChange(Change);
    return true;
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

void wxsProperties::AddProperty(const wxString& Name,wxString& Value,int Position)
{
    AddProperty(Name,new wxsStringProperty(this,Value,true),Position);
}

void wxsProperties::AddProperty(const wxString& Name,int& Value,int Position)
{
    AddProperty(Name,new wxsIntProperty(this,Value,true),Position);
}

void wxsProperties::AddProperty(const wxString& Name,bool& Value,int Position)
{
    AddProperty(Name,new wxsBoolProperty(this,Value),Position);
}

void wxsProperties::Add2IProperty(const wxString& Name,int& Value1,int& Value2,int Position)
{
    AddProperty(Name,new wxs2IntProperty(this,Value1,Value2),Position);
}

void wxsProperties::AddProperty(const wxString& Name,wxArrayString& Array,int Position)
{
	AddProperty(Name,new wxsStringListProperty(this,Array),Position);
}

void wxsProperties::AddProperty(const wxString& Name,wxArrayString& Array,int& Selected,int SortedFlag,int Position)
{
	AddProperty(Name,new wxsStringListProperty(this,Array,Selected,SortedFlag),Position);
}

void wxsProperties::AddProperty(const wxString& Name,wxsProperty* Prop,int Position)
{
    if ( !Prop ) return;
    if ( !Name.Length() ) return;

    VectorElem* NewElem = new VectorElem;;
    NewElem->Name = Name;
    NewElem->Property = Prop;

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
        #ifdef __NO_PROPGRGID
            (*i)->Property->UpdateEditWindow();
        #else
            (*i)->Property->UpdatePropGrid(Grid);
        #endif
    }
    BlockUpdates = false;
}

wxWindow* wxsProperties::GenerateWindow(wxWindow* Parent)
{
    #ifdef __NO_PROPGRGID
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

        return Panel;

    #else

        wxsPropertyGrid* PG = new wxsPropertyGrid(Parent,this);

        for ( VectorI i = Properties.begin(); i!=Properties.end(); ++i )
        {
        	(*i)->Property->AddToPropGrid(PG,(*i)->Name);
        }

        Grid = PG;
        return PG;

    #endif
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
    bool Result = Widget->PropertiesUpdated(Check,false);
    BlockUpdates = false;

    return Result;
}
