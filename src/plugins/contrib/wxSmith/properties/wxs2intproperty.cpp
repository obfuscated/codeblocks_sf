#include "../wxsheaders.h"
#include "wxs2intproperty.h"

#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/stattext.h>

#include "../widget.h"

wxs2IntProperty::wxs2IntProperty(int& Int1,int& Int2,const wxString& SubName1,const wxString& SubName2):
    Value1(Int1),
    Value2(Int2),
    Name1(SubName1),
    Name2(SubName2),
    PGId(0),
    Val1Id(0),
    Val2Id(0)
{
}

void wxs2IntProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    PGId   = Grid->Append( wxParentProperty(Name,wxPG_LABEL) );
    Val1Id = Grid->AppendIn(PGId, wxIntProperty(Name1, wxPG_LABEL, Value1));
    Val2Id = Grid->AppendIn(PGId, wxIntProperty(Name2, wxPG_LABEL, Value2));
}

bool wxs2IntProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( (Id == Val1Id) || (Id == Val2Id) || (Id == PGId) )
    {
        Value1 = Grid->GetPropertyValue(Val1Id).GetLong();
        Value2 = Grid->GetPropertyValue(Val2Id).GetLong();
        int Cor1 = Value1;
        int Cor2 = Value2;
        CorrectValues(Value1,Value2);
        if ( Value1 != Cor1 )
        {
            Grid->SetPropertyValue(Val1Id,Value1);
        }
        if ( Value2 != Cor2 )
        {
            Grid->SetPropertyValue(Val2Id,Value2);
        }
        return ValueChanged(true);
    }
    return true;
}

void wxs2IntProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(Val1Id,Value1);
    Grid->SetPropertyValue(Val2Id,Value2);
}
