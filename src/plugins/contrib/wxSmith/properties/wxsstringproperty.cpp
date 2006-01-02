#include "../wxsheaders.h"
#include "wxsstringproperty.h"

#include <wx/textctrl.h>

wxsStringProperty::wxsStringProperty(wxString& String, bool Long):
    Value(String), 
    IsLong(Long),
    PGId(0)
{
}

void wxsStringProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    if ( IsLong )
    {
        wxString Str = Value;
        Str.Replace(_T("\n"),_T("\\n"));
        PGId = Grid->Append( wxLongStringProperty(Name,wxPG_LABEL,Str) );
    }
    else
    {
        PGId = Grid->Append(Name,wxPG_LABEL,Value);
    }
}

bool wxsStringProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId )
    {
        wxString Cor = Grid->GetPropertyValue(Id).GetString();
        if ( IsLong )
        {
// FIXME (SpOoN#1#): This won't work properly when using '\n' in entered text but this is in fact bug in wxPropertyGrid
            Cor.Replace(_T("\\n"),_T("\n"));
        }
        Value = CorrectValue(Cor);
        if ( Value != Cor )
        {
            Grid->SetPropertyValue(Id,Value);
        }
        return ValueChanged(true);
    }
    return true;
}

void wxsStringProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    wxString Str = Value;
    if ( IsLong )
    {
         Str.Replace(_T("\n"),_T("\\n"));
    }
    Grid->SetPropertyValue(PGId,Str);
}
