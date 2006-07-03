#include "../wxsheaders.h"
#include "wxshtmlwindow.h"

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"
#include <wx/html/htmlwin.h>
#include <wx/filesys.h>

WXS_ST_BEGIN(wxsHtmlWindowStyles)
    WXS_ST_CATEGORY("wxHtmlWindow")
    WXS_ST(wxHW_SCROLLBAR_NEVER)
    WXS_ST(wxHW_SCROLLBAR_AUTO)
    WXS_ST(wxHW_NO_SELECTION)
WXS_ST_END(wxsHtmlWindowStyles)

WXS_EV_BEGIN(wxsHtmlWindowEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsHtmlWindowEvents)

wxsDWDefineBegin(wxsHtmlWindowBase,wxHtmlWindow,
    WXS_THIS = new wxHtmlWindow(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,WXS_STYLE,WXS_NAME);
    )
    wxsDWDefInt(borders,"Borders:",0);
    wxsDWDefStr(url,"Url:","");
    wxsDWDefLongStr(htmlcode,"Html Code:","");
wxsDWDefineEnd()

wxWindow* wxsHtmlWindow::MyCreatePreview(wxWindow* Parent)
{
    wxHtmlWindow* Wnd = new wxHtmlWindow(Parent,-1L,GetPosition(),GetSize(),GetStyle());
    if ( borders )
    {
        Wnd->SetBorders(borders);
    }
    if ( !url.empty() )
    {
        Wnd->SetPage(
         wxString(_T("<body><center>")) +
         _("Following url will be used:") +
         _T("<br>") + url + _T("</center></body>"));
    }
    else if ( !htmlcode.empty() )
    {
        Wnd->SetPage(htmlcode);
    }

    return Wnd;
}

bool wxsHtmlWindow::MyXmlLoad()
{
    borders = XmlGetInteger(_T("borders"));
	url = XmlGetVariable(_T("url"));
	htmlcode = XmlGetVariable(_T("htmlcode"));

    return true;
}

bool wxsHtmlWindow::MyXmlSave()
{
    if ( borders )
        XmlSetInteger(_T("borders"), borders);

    if ( !url.empty() )
        XmlSetVariable(_T("url"), url);

    if ( !htmlcode.empty() )
        XmlSetVariable(_T("htmlcode"), htmlcode);

    return true;
}

wxString wxsHtmlWindow::GetProducingCode(const wxsCodeParams& Params)
{
    wxString Code;

    Code.Printf(_T("%s = new wxHtmlWindow(%s,%s,%s,%s,%s);\n"),
        Params.VarName.c_str(),
        Params.ParentName.c_str(),
        Params.IdName.c_str(),
        Params.Pos.c_str(),
        Params.Size.c_str(),
        Params.Style.c_str());

    if ( borders )
    {
        Code << wxString::Format(_T("%s->SetBorders(%d);\n"),
                Params.VarName.c_str(),
                borders);
    }
    if ( !url.empty() )
    {
        Code << wxString::Format(_T("%s->LoadPage(%s);\n"),
                Params.VarName.c_str(),
                wxsGetWxString(url).c_str());
    }
    else if (!htmlcode.empty())
    {
        Code << wxString::Format(_T("%s->SetPage(%s);\n"),
                Params.VarName.c_str(),
                wxsGetWxString(htmlcode).c_str());
    }

    Code << Params.InitCode;

    return Code;
}
