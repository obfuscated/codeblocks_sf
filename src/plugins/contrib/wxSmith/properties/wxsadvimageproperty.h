#ifndef WXSADVIMAGEPROPERTY_H
#define WXSADVIMAGEPROPERTY_H

#include "../wxsheaders.h"
#include "../wxsproperties.h"

class wxsAdvImageProperty : public wxsProperty
{
	public:
		wxsAdvImageProperty(wxString& AdvImage);
		virtual ~wxsAdvImageProperty();
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

	private:

        wxPGId PGId;
        //wxPGId UseId;
        wxPGId AdvImageId;
        //wxAdvImageProperty* AdvImageProperty;
        wxString& AdvImage;
};


#endif // WXSADVIMAGEPROPERTY_H
