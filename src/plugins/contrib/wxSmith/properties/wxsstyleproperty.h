#ifndef WXSSTYLEPROPERTY_H
#define WXSSTYLEPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"

class wxsStyleProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsStyleProperty(
            int &StyleBits,
            int &ExStyleBits,
            wxsStyle*Styles,
            bool XrcOnly,
            const wxString& ExPropName=_("Extra style"));

    protected:
        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        int& StyleBits;
        int& ExStyleBits;
        wxsStyle* Styles;
        bool XrcOnly;
        wxString ExName;

        wxPGId StylePGId;
        wxPGId ExStylePGId;
};

#endif // WXSSTYLEPROPERTY_H
