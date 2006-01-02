#ifndef WXS2INTPROPERTY_H
#define WXS2INTPROPERTY_H

#include "../wxsproperties.h"

class wxs2IntProperty : public wxsProperty
{
	public:
        /** Ctor
         * \param Int1 - first variable which will be monitored
         * \param Int2 - second variable which will be monitored
         * \param SubName1 - name of subproperty for first integer
         * \param SubName2 - name of subproperty for second integer
         */
		wxs2IntProperty(
            int &Int1,
            int &Int2,
            const wxString& SubName1 = _("X"),
            const wxString& SubName2 = _("Y") );

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

        /** This function makes additional correction for value, must always
         *  return acceptable one. It can be declared inside derived classes
         *  to extend abilities of IntProperty (f.ex. inly odd numbers)
         */
        virtual void CorrectValues(int &Value1, int &Value2) { }

	private:

        int& Value1;
        int& Value2;
        wxString Name1;
        wxString Name2;

        wxPGId PGId;
        wxPGId Val1Id;
        wxPGId Val2Id;
};

#endif // WXS2INTPROPERTY_H
