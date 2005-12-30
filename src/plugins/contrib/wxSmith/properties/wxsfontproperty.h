#ifndef WXSFONTPROPERTY_H
#define WXSFONTPROPERTY_H

#include "../wxsproperties.h"

class wxsFontProperty : public wxsProperty
{
	public:
		wxsFontProperty(wxsProperties* Properties,bool& Use,wxFont& Font);
		virtual ~wxsFontProperty();
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        wxPGId PGId;
        wxPGId UseId;
        wxPGId FontId;

        bool& Use;
        wxFont& Font;
};

#endif // WXSFONTPROPERTY_H
