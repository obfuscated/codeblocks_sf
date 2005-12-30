#ifndef WXSENUMPROPERTY_H
#define WXSENUMPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"

class wxsEnumProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsEnumProperty(wxsProperties* Properties,int &Value,const wxChar** Names,const long* Values);

		/** DCtor */
		virtual ~wxsEnumProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        int& Value;
        const wxChar** Names;
        const long* Values;

        wxPGId PGId;
};

#endif
