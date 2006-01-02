#ifndef WXSBORDERPROPERTY_H
#define WXSBORDERPROPERTY_H

#include "../wxsproperties.h"

class wxsBorderProperty : public wxsProperty
{
	public:

        /** Ctor */
		wxsBorderProperty(int& Flag);

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        int &BorderFlags;
        wxPGId PGId;
};

#endif // WXSBORDERPROPERTY_H
