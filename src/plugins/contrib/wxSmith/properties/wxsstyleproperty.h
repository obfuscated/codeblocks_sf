#ifndef WXSSTYLEPROPERTY_H
#define WXSSTYLEPROPERTY_H

#include "../wxsproperties.h"
#include "../widget.h"

class WXSCLASS wxsStyleProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsStyleProperty(
            wxsProperties* Properties,
            int &StyleBits,
            int &ExStyleBits,
            wxsStyle*Styles,
            bool XrcOnly);

		/** DCtor */
		virtual ~wxsStyleProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        int& StyleBits;
        int& ExStyleBits;
        wxsStyle* Styles;
        bool XrcOnly;

        wxPGId StylePGId;
        wxPGId ExStylePGId;
};

#endif // WXSSTYLEPROPERTY_H
