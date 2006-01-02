#ifndef WXSFONTPROPERTY_H
#define WXSFONTPROPERTY_H

#include "../wxsproperties.h"

class wxsFontProperty : public wxsProperty
{
	public:
        /** Ctor
         * \param Use - flag indicating wheder font is used dor not
         * \param Font - edited font
         * \param UseSubName - name of subproperty for Use param
         * \param FontSubName - name of subproperty for editing font
         */
		wxsFontProperty(
            bool& Use,
            wxFont& Font,
            const wxString& UseSubName = _("Use font"),
            const wxString& FontSubName = _("Font") );

    protected:
        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        bool& Use;
        wxFont& Font;
        wxString UseName;
        wxString FontName;

        wxPGId PGId;
        wxPGId UseId;
        wxPGId FontId;
};

#endif // WXSFONTPROPERTY_H
