#ifndef WXSCOLOURPROPERTY_H
#define WXSCOLOURPROPERTY_H

#include "../wxsproperties.h"

class wxsColourProperty : public wxsProperty
{
	public:
        /** Ctor
         * \param Type - colour type (system / custom / no colour)
         * \param Colour - colour value for custom colour
         * \param UseSubName - name of sub property - use colour check box
         * \param ColSubName - name of sub property - colour selection
         */
		wxsColourProperty(
            wxUint32& Type,
            wxColour& Colour,
            const wxString& UseSubName = _("Use colour"),
            const wxString& ColSubName = _("Colour"));

    protected:
        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:
        wxUint32& Type;
        wxColour& Colour;
        wxString UseName;
        wxString ColName;
        wxPGId PGId;
        wxPGId UseColId;
        wxPGId ColId;
};

#endif // WXSCOLOURPROPERTY_H
