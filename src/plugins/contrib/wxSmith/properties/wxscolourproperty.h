#ifndef WXSCOLOURPROPERTY_H
#define WXSCOLOURPROPERTY_H

#include "../wxsproperties.h"

class WXSCLASS wxsColourProperty : public wxsProperty
{
	public:

		wxsColourProperty(wxsProperties* Properties,wxUint32& Type,wxColour& Colour);
		virtual ~wxsColourProperty();
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        wxPGId PGId;
        wxPGId UseColId;
        wxPGId ColId;
        wxUint32& Type;
        wxColour& Colour;
};

#endif // WXSCOLOURPROPERTY_H
