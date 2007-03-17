#include "wxsuseritem.h"
#include "../wxsitemresdata.h"

wxsUserItem::wxsUserItem(wxsItemResData* Data,wxsItemInfo* Info):
    wxsWidget(Data,Info,NULL,NULL)
{
    //ctor
}

wxsUserItem::~wxsUserItem()
{
    //dtor
}

void wxsUserItem::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    wxString Generated;
    wxString VarName;
    switch ( Language )
    {
        case wxsCPP:

            for ( size_t i=0; i<m_CppCodeTemplate.Length(); i++ )
            {
                wxChar Ch = m_CppCodeTemplate.GetChar(i);

                if ( ( (Ch >= _T('A')) && (Ch <= _T('Z')) ) ||
                     ( (Ch >= _T('a')) && (Ch <= _T('z')) ) ||
                     ( (Ch >= _T('0')) && (Ch <= _T('9')) ) ||
                     ( (Ch == _T('_')) ) )
                {
                    VarName << Ch;
                }
                else
                {
                    if ( !VarName.IsEmpty() )
                    {
                        Generated << ExpandCodeVariable(VarName.MakeUpper());
                    }
                    Generated << Ch;
                    VarName.Clear();
                }
            }
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsUserItem::OnBuildCreatingCode"),Language);
            return;
    }
    Code << Generated;
}

wxString wxsUserItem::ExpandCodeVariable(const wxString& VarName)
{
    wxsCodingLang Language = GetResourceData()->GetLanguage();
    if ( VarName == _T("PARENT")    ) return Codef(Language,_T("%W"));
    if ( VarName == _T("CREATE")    ) return Codef(Language,_T("%C"));
    if ( VarName == _T("ACCESS")    ) return Codef(Language,_T("%A"));
    if ( VarName == _T("THIS")      ) return Codef(Language,_T("%O"));
    if ( VarName == _T("POS")       ) return Codef(Language,_T("%P"));
    if ( VarName == _T("SIZE")      ) return Codef(Language,_T("%S"));
    if ( VarName == _T("ID")        ) return Codef(Language,_T("%I"));
    if ( VarName == _T("VALIDATOR") ) return Codef(Language,_T("%V"));
    if ( VarName == _T("NAME")      ) return Codef(Language,_T("%N"));



    return VarName;
}
