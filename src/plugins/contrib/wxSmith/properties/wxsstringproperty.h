#ifndef WXSSTRINGPROPERTY_H
#define WXSSTRINGPROPERTY_H

#include "../wxsproperties.h"

class wxsStringProperty : public wxsProperty
{
	public:
        /** Ctor */
		wxsStringProperty(wxString& String, bool Long=false);

    protected:
        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

        virtual wxString CorrectValue(const wxString& Value) { return Value; }
        
	private:
        wxString& Value;
        bool IsLong;
        wxPGId PGId;
};

#endif // WXSSTRINGPROPERTY_H
