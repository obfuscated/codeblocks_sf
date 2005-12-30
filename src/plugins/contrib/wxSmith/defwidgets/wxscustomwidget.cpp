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
    CreatingCode = _T("$(THIS) = new $(CLASS)($(PARENT),$(ID),$(POS),$(SIZE),$(STYLE));");
    Style = _T("0");
}

wxsCustomWidget::~wxsCustomWidget()
{
}

wxString wxsCustomWidget::GetProducingCode(wxsCodeParams& Params)
{
    const CodeDefines& CD = GetCodeDefines();
    wxString Result = CreatingCode;
    wxsCodeReplace(Result,_T("$(POS)"),CD.Pos);
    wxsCodeReplace(Result,_T("$(SIZE)"),CD.Size);
    wxsCodeReplace(Result,_T("$(STYLE)"),Style);
    wxsCodeReplace(Result,_T("$(ID)"),GetBaseProperties().IdName);
    wxsCodeReplace(Result,_T("$(THIS)"),GetBaseProperties().VarName);
    wxsCodeReplace(Result,_T("$(PARENT)"),Params.ParentName);
    wxsCodeReplace(Result,_T("$(CLASS)"),ClassName);
    Result << CD.InitCode;
    return Result;
}

wxString wxsCustomWidget::GetDeclarationCode(wxsCodeParams& Params)
{
    return wxString::Format(_T("%s* %s;"),
        ClassName.c_str(),
        GetBaseProperties().VarName.c_str());
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
    }
    else
    {
        ClassName = _U(XmlElem()->Attribute("class"));
        Style = XmlGetVariable(_T("style"));
        
        TiXmlDocument Doc;
        for ( TiXmlElement* Elem = XmlElem()->FirstChildElement();
              Elem;
              Elem = Elem->NextSiblingElement() )
        {
            // Skipping all standard elements
            wxString Name = _U(Elem->Value());
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
                Doc.InsertEndChild(*Elem);
            }
        }
        
        #ifdef TIXML_USE_STL
            std::ostringstream buffer;
            buffer << Doc;
            XmlData = _U(buffer.str().c_str());
        #else
            TiXmlOutStream buffer;
            buffer << Doc;
            XmlData = _U(buffer.c_str());
        #endif
    }
    
    return true;
}

bool wxsCustomWidget::MyXmlSave()
{
    if ( GetResource()->GetEditMode() == wxsREMSource )
    {
        XmlSetVariable(_T("class"),ClassName);
        XmlSetVariable(_T("creating_code"),CreatingCode);
    }
    else
    {
        XmlElem()->SetAttribute("class",ClassName.mb_str());
        XmlSetVariable(_T("style"),Style);
        
        TiXmlDocument Doc;
        Doc.Parse(XmlData.mb_str());
        
        for ( TiXmlElement* Elem = Doc.FirstChildElement();
              Elem;
              Elem = Elem->NextSiblingElement() )
        {
            // Skipping all standard elements
            wxString Name = _U(Elem->Value());
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

void wxsCustomWidget::CreateObjectProperties()
{
    wxsWidget::CreateObjectProperties();
    PropertiesObject.AddProperty(_T("Class name:"),ClassName,0);
    if ( GetResource()->GetEditMode() == wxsREMSource )
    {
        PropertiesObject.AddProperty(
            _T("Creating code:"),
                new wxsStringProperty(&PropertiesObject,CreatingCode,true,true),1);
    }
    else
    {
        PropertiesObject.AddProperty(
            _T("Xml Data:"),
                new wxsStringProperty(&PropertiesObject,XmlData,true,true),1);
    }
    PropertiesObject.AddProperty(_T("Style:"),Style,2);
}

wxWindow* wxsCustomWidget::MyCreatePreview(wxWindow* Parent)
{
    wxStaticText* Wnd = new wxStaticText(Parent,-1,_T("???"),
        GetPosition(),GetSize(),wxST_NO_AUTORESIZE|wxALIGN_CENTRE);
    Wnd->SetBackgroundColour(wxColour(0,0,0));
    Wnd->SetForegroundColour(wxColour(0xFF,0xFF,0xFF));
    return Wnd;
}
