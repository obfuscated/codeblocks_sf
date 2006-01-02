#ifndef WXSINTPROPERTY_H
#define WXSINTPROPERTY_H

#include "../wxsproperties.h"

class wxsIntProperty : public wxsProperty
{
	public:
        /** Ctor */
		wxsIntProperty(int &Int);

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

        /** This function makes additional correction for value, must always
         *  return acceptable one. It can be declared inside derived classes
         *  to extend abilities of IntProperty (f.ex. inly odd numbers)
         */
        virtual int CorrectValue(int Value) { return Value; }

	private:
        int& Value;
        wxPGId PGId;
};

#endif // WXSINTPROPERTY_H
