#ifndef WXSENUMPROPERTY_H
#define WXSENUMPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"

class wxsEnumProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsEnumProperty(int &Value,const wxChar** Names,const long* Values);

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
