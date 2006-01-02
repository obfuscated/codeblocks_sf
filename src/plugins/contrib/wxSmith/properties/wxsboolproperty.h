#ifndef WXSBOOLPROPERTY_H
#define WXSBOOLPROPERTY_H

#include "../wxsproperties.h"

class wxsBoolProperty : public wxsProperty
{
	public:
		wxsBoolProperty(bool &Bool);

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        bool& Value;
        wxPGId PGId;
};

#endif // WXSBOOLPROPERTY_H
