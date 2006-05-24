#include "wxscontainer.h"
#include <messagemanager.h>

wxsContainer::wxsContainer(
    wxsWindowRes* Resource,
    long _BasePropertiesFlags,
    const wxsItemInfo* _Info,
    const wxsEventDesc* _EventArray,
    const wxsStyle* _StyleSet,
    const wxString& _DefaultStyle):
        wxsParent(Resource),
        Info(_Info),
        EventArray(_EventArray),
        StyleSet(_StyleSet),
        DefaultStyle(_DefaultStyle),
        BasePropertiesFlags(_BasePropertiesFlags)
{
    if ( StyleSet )
    {
        wxsStyleProperty::SetFromString(StyleBits,DefaultStyle,StyleSet,false);
    }
    else
    {
        StyleBits = 0;
    }
}

bool wxsContainer::CanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetType() == wxsTSpacer )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("Spacer can be added into sizer only"));
        }
        return false;
    }

    if ( Item->GetType() == wxsTSizer )
    {
        if ( GetChildCount() > 0 )
        {
            if ( GetChild(0)->GetType() == wxsTSizer )
            {
                if ( ShowMessage )
                {
                    wxMessageBox(_("This item can contain only one sizer"));
                }
                return false;
            }
            else
            {
                if ( ShowMessage )
                {
                    wxMessageBox(_("Item can not contain sizer if it has other items inside"));
                }
                return false;
            }
        }
        return true;
    }

    if ( GetChildCount() > 0 )
    {
        if ( GetChild(0)->GetType() == wxsTSizer )
        {
            if ( ShowMessage )
            {
                wxMessageBox(_("Item contains sizer, can not add other items into it."));
            }
            return false;
        }
    }

    return true;
}

void wxsContainer::EnumItemProperties(long Flags)
{
    EnumContainerProperties(Flags);
    wxsItem::EnumItemProperties(Flags);
    SubContainer(BaseProps,GetBasePropertiesFlags());
    if ( StyleSet )
    {
        WXS_STYLE(wxsContainer,StyleBits,0,_("Style"),_T("style"),StyleSet,DefaultStyle);
        WXS_EXSTYLE(wxsContainer,ExStyleBits,0,_("Extra style"),_T("exstyle"),StyleSet,wxEmptyString);
    }
}

void wxsContainer::AddItemQPP(wxsAdvQPP* QPP)
{
    AddContainerQPP(QPP);
    BaseProps.AddQPPChild(QPP,GetBasePropertiesFlags());
}

wxWindow* wxsContainer::SetupWindow(wxWindow* Preview,bool IsExact)
{
    BaseProps.SetupWindow(Preview,IsExact);
    long ExStyle = wxsStyleProperty::GetWxStyle(ExStyleBits,StyleSet,true);
    if ( ExStyle != 0 )
    {
        Preview->SetExtraStyle(Preview->GetExtraStyle() | ExStyle);
    }
    return Preview;
}

void wxsContainer::SetupWindowCode(wxString& Code,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            BaseProps.BuildSetupWindowCode(Code,GetVarName(),wxsCPP);
            if ( ExStyleBits )
            {
                wxString ExStyleStr = wxsStyleProperty::GetString(ExStyleBits,StyleSet,true,wxsCPP);
                if ( ExStyleStr != _T("0") )
                {
                    wxString VarAccess = GetVarName().empty() ? _T("") : GetVarName() + _T("->");

                    Code << VarAccess << _T("SetExtraStyle(") <<
                            VarAccess << _T("GetExtraStyle() | ") <<
                            ExStyleStr << _T(");\n");
                }
            }
            return;
        }
    }

    wxsLANGMSG(wxsContainer::SetupWindowCode,Language);
}

void wxsContainer::AddChildrenPreview(wxWindow* This,bool Exact)
{
    for ( int i=0; i<GetChildCount(); i++ )
    {
        GetChild(i)->BuildPreview(This,Exact);
    }

    // TODO: Move this into child classes since it's not what this function should do
    if ( BaseProps.Size.IsDefault )
    {
        This->Fit();
    }
}

void wxsContainer::AddChildrenCode(wxString& Code,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxString ThisName = GetParent()?GetVarName():_T("this");
                GetChild(i)->BuildCreatingCode(Code,ThisName,wxsCPP);
                if ( GetChild(i)->GetType() == wxsTSizer )
                {
                    wxString SizerName = GetChild(i)->GetVarName();
                    if ( GetParent() )
                    {
                        Code << GetVarName() << _T("->");
                    }

                    Code << _T("SetSizer(") << SizerName << _T(");\n");

                    if ( BaseProps.Size.IsDefault )
                    {
                        Code << SizerName << _T("->Fit(") << ThisName << _T(");\n");
                    }

                    Code << SizerName << _T("->SetSizeHints(") << ThisName << _T(");\n");
                }
            }
            return;
        }
    }

    wxsLANGMSG(wxsContainer::AddChildrenCode,Language);
}
