#ifndef WXSPLACEMENTPROPERTY_H
#define WXSPLACEMENTPROPERTY_H

#include "../wxsproperties.h"

class wxsPlacementProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsPlacementProperty(wxsProperties* Properties,int& Placement,bool& Expand,bool& Shaped);

        /** Dctor */
		virtual ~wxsPlacementProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        int &PlacementType;
        bool &Expand;
        bool &Shaped;

        wxPGId PGId;
        wxPGId ExpandId;
        wxPGId ShapedId;
};

#endif
