//#include "../wxsheaders.h"
#include "wxsadvimageproperty.h"

#include <wx/intl.h>

#ifndef __NO_PROPGRGID
wxsAdvImageProperty::wxsAdvImageProperty(wxString& _AdvImage):
    PGId(0),
    AdvImage(_AdvImage)
{
}

wxsAdvImageProperty::~wxsAdvImageProperty()
{
}

const wxString & wxsAdvImageProperty::GetTypeName()
{
    static wxString Type(_T("wxImage"));
    return Type;
}

void wxsAdvImageProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
   AdvImageId = Grid->Append(wxAdvImageFileProperty(_("Image:")));
   Grid->SetPropertyAttribute(AdvImageId,wxPG_FILE_SHOW_FULL_PATH,(long)1);
}

bool wxsAdvImageProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId || Id == AdvImageId )
    {
        //Retrieve full path of the image file
        Grid->SetPropertyAttribute(AdvImageId,wxPG_FILE_SHOW_FULL_PATH,(long)1);
    	AdvImage = Grid->GetPropertyValueAsString(AdvImageId);
        Grid->SetPropertyAttribute(AdvImageId,wxPG_FILE_SHOW_FULL_PATH,(long)0);

    	return ValueChanged(true);
    }
    return true;
}

void wxsAdvImageProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(AdvImageId,AdvImage);
   // Grid->EnableProperty(AdvImageId);
}

#endif
