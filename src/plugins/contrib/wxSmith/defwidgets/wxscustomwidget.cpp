#include "../wxsheaders.h"
#include "wxscustomwidget.h"

#include "wxsstdmanager.h"
#include "../resources/wxswindowres.h"
#include "../properties/wxsstringproperty.h"
#include <sstream>

WXS_EV_BEGIN(wxsCustomWidgetEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsCustomWidgetEvents)

wxsCustomWidget::wxsCustomWidget(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWidget(Man,Res,0)
{
    ChangeBPT(wxsREMSource,propCustomS);
    ChangeBPT(wxsREMFile,propCustomF);
    ChangeBPT(wxsREMMixed,propCustomM);
    ClassName = _("CustomClass");
    CreatingCode = _T("$(THIS) = new $(CLASS)($(PARENT),$(ID),$(POS),$(SIZE),$(STYLE),wxDefaultValidator,$(NAME));");
    Style = _T("0");
}

wxsCustomWidget::~wxsCustomWidget()
{
}

wxString wxsCustomWidget::GetProducingCode(const wxsCodeParams& Params)
{
    wxString Result = CreatingCode;
    // TODO: This may lead to some errors when string properties
    //       will contain one of items declared before, replace this code
    //       with better scanner.
    // TODO: Replace $(...) and leave ...
    wxsCodeReplace(Result,_T("$(POS)"),Params.Pos);
    wxsCodeReplace(Result,_T("$(SIZE)"),Params.Size);
    wxsCodeReplace(Result,_T("$(STYLE)"),Style);
    wxsCodeReplace(Result,_T("$(ID)"),Params.IdName);
    wxsCodeReplace(Result,_T("$(THIS)"),Params.VarName);
    wxsCodeReplace(Result,_T("$(PARENT)"),Params.ParentName);
    wxsCodeReplace(Result,_T("$(NAME)"),Params.Name);
    wxsCodeReplace(Result,_T("$(CLASS)"),ClassName);
    Result << Params.InitCode;
    return Result;
}

wxString wxsCustomWidget::GetDeclarationCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("%s* %s;"),
        ClassName.c_str(),
        Params.VarName.c_str());
}

const wxsWidgetInfo& wxsCustomWidget::GetInfo()
{
    return *GetManager()->GetWidgetInfo(wxsCustomWidgetId);
}

bool wxsCustomWidget::MyXmlLoad()
{
    if ( GetResource()->GetEditMode() == wxsREMSource )
    {
        ClassName = XmlGetVariable(_T("class"));
        CreatingCode = XmlGetVariable(_T("creating_code"));
        Style = XmlGetVariable(_T("style"));
    }
    else
    {
        ClassName = cbC2U(XmlElem()->Attribute("class"));
        Style = XmlGetVariable(_T("style"));

        XmlDataDoc.Clear();
        for ( TiXmlElement* Elem = XmlElem()->FirstChildElement();
              Elem;
              Elem = Elem->NextSiblingElement() )
        {
            // Skipping all standard elements
            wxString Name = cbC2U(Elem->Value());
            if ( Name != _T("pos") &&
                 Name != _T("size") &&
                 Name != _T("style") &&
                 Name != _T("enabled") &&
                 Name != _T("focused") &&
                 Name != _T("hidden") &&
                 Name != _T("fg") &&
                 Name != _T("bg") &&
                 Name != _T("font") &&
                 Name != _T("handler") )
            {
                XmlDataDoc.InsertEndChild(*Elem);
            }
        }

        RebuildXmlData();
    }

    return true;
}

bool wxsCustomWidget::MyXmlSave()
{
    if ( GetResource()->GetEditMode() == wxsREMSource )
    {
        XmlSetVariable(_T("class"),ClassName);
        XmlSetVariable(_T("creating_code"),CreatingCode);
        XmlSetVariable(_T("style"),Style);
    }
    else
    {
        XmlElem()->SetAttribute("class",cbU2C(ClassName));
        XmlSetVariable(_T("style"),Style);

        for ( TiXmlElement* Elem = XmlDataDoc.FirstChildElement();
              Elem;
              Elem = Elem->NextSiblingElement() )
        {
            // Skipping all standard elements
            wxString Name = cbC2U(Elem->Value());
            if ( Name != _T("pos") &&
                 Name != _T("size") &&
                 Name != _T("style") &&
                 Name != _T("enabled") &&
                 Name != _T("focused") &&
                 Name != _T("hidden") &&
                 Name != _T("fg") &&
                 Name != _T("bg") &&
                 Name != _T("font") &&
                 Name != _T("handler") )
            {
                XmlElem()->InsertEndChild(*Elem);
            }
        }
    }

    return true;
}

void wxsCustomWidget::MyCreateProperties()
{
    Properties.AddProperty(_T("Class name:"),ClassName);
    if ( GetResource()->GetEditMode() == wxsREMSource )
    {
        Properties.AddProperty(
            _T("Creating code:"),
                new wxsStringProperty(CreatingCode,true),1);
    }
    else
    {
        Properties.AddProperty(
            _T("Xml Data:"),
                new wxsStringProperty(XmlData,true),1);
    }
    Properties.AddProperty(_T("Style:"),Style,2);
    wxsWidget::MyCreateProperties();
}

wxWindow* wxsCustomWidget::MyCreatePreview(wxWindow* Parent)
{
    wxStaticText* Wnd = new wxStaticText(Parent,-1,_T("???"),
        GetPosition(),GetSize(),wxST_NO_AUTORESIZE|wxALIGN_CENTRE);
    Wnd->SetBackgroundColour(wxColour(0,0,0));
    Wnd->SetForegroundColour(wxColour(0xFF,0xFF,0xFF));
    return Wnd;
}

void wxsCustomWidget::RebuildXmlData()
{
    #ifdef TIXML_USE_STL
        std::ostringstream buffer;
        buffer << XmlDataDoc;
        XmlData = cbC2U(buffer.str().c_str());
    #else
        TiXmlOutStream buffer;
        buffer << XmlDataDoc;
        XmlData = cbC2U(buffer.c_str());
    #endif
}

bool wxsCustomWidget::RebuildXmlDataDoc(bool Validate,bool Correct)
{
    XmlDataDoc.Clear();
    XmlDataDoc.Parse(cbU2C(XmlData));
    if ( !Validate ) return true;
    if ( !XmlDataDoc.Error() ) return true;
    if ( Correct )
    {
        RebuildXmlData();
        return false;
    }
    wxMessageBox(
        wxString::Format(
            _("Invalid Xml structure.\nError at line %d, column %d:\n\t\"%s\""),
                XmlDataDoc.ErrorRow(),XmlDataDoc.ErrorCol(),
                wxGetTranslation(cbC2U(XmlDataDoc.ErrorDesc()).c_str())));
    return false;
}

bool wxsCustomWidget::MyPropertiesChanged(bool Validate,bool Correct)
{
    bool Ret = ( GetResource()->GetEditMode() == wxsREMSource ) ?
        true :
        RebuildXmlDataDoc(Validate,Correct);
    return wxsWidget::PropertiesChanged(Validate,Correct) && Ret;
}
