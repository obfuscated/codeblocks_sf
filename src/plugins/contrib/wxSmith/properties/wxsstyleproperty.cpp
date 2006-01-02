#include "../wxsheaders.h"
#include "wxsstyleproperty.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <vector>

wxsStyleProperty::wxsStyleProperty(
    int &_StyleBits,
    int &_ExStyleBits,
    wxsStyle *_Styles,
    bool _XrcOnly,
    const wxString& ExPropName):
        StyleBits(_StyleBits),
        ExStyleBits(_ExStyleBits),
        Styles(_Styles),
        XrcOnly(_XrcOnly),
        ExName(ExPropName),
        StylePGId(0),
        ExStylePGId(0)
{
}

void wxsStyleProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    wxPGConstants Consts;
    wxPGConstants ExConsts;

    int StCount = 0;
    int ExCount = 0;
    for ( wxsStyle* St = Styles; St->Name.Length(); ++St )
    {
        if ( St->IsCategory() ) continue;

        if ( St->IsExtra() )
        {
            if ( !XrcOnly || (St->Flags & wxsSFXRC) )
            {
                ExConsts.Add(St->Name,(1<<ExCount));
            }
            ExCount++;
            if ( ExCount == 32 )
            {
                DBGLOG(_T("Widget has over 31 extra styles, extra style property is corrupted!!!"));
            }
        }
        else
        {
            if ( !XrcOnly || (St->Flags & wxsSFXRC) )
            {
                Consts.Add(St->Name,(1<<StCount));
            }
            StCount++;
            if ( StCount == 32 )
            {
                DBGLOG(_T("Widget has over 31 styles, style property is corrupted!!!"));
            }
        }
    }

    StylePGId = Grid->Append( wxFlagsProperty(Name,wxPG_LABEL,Consts,StyleBits) );
    Grid->SetPropertyAttribute(StylePGId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
    if ( ExCount )
    {
        ExStylePGId = Grid->Append( wxFlagsProperty(ExName,wxPG_LABEL,ExConsts,ExStyleBits) );
        Grid->SetPropertyAttribute(ExStylePGId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
    }
}

bool wxsStyleProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( ( Id == StylePGId ) || ( Id == ExStylePGId ) )
    {
        StyleBits = Grid->GetPropertyValue(StylePGId).GetLong();
        if ( ExStylePGId != 0 )
        {
            ExStyleBits = Grid->GetPropertyValue(ExStylePGId).GetLong();
        }
        return ValueChanged(true);
    }
    return true;
}

void wxsStyleProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(StylePGId,StyleBits);
    if ( ExStylePGId != 0 )
    {
        Grid->SetPropertyValue(ExStylePGId,ExStyleBits);
    }
}
