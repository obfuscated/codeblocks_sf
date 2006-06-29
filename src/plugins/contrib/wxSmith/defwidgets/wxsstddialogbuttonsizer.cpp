#include "../wxsheaders.h"
#include "wxsstddialogbuttonsizer.h"

#include "../properties/wxsenumproperty.h"

wxString wxsStdDialogButtonSizer::GetProducingCode(const wxsCodeParams& Params)
{
    wxString Code = wxString::Format(_T("%s = new wxStdDialogButtonSizer();\n"),
        BaseProperties.VarName.c_str() );

    if ( chkOK ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_OK,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strOK).c_str()));
    if ( chkYES ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_YES,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strYES).c_str()));
    if ( chkSAVE ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_SAVE,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strSAVE).c_str()));
    if ( chkAPPLY ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_APPLY,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strAPPLY).c_str()));
    if ( chkNO ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_NO,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strNO).c_str()));
    if ( chkCANCEL ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_CANCEL,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strCANCEL).c_str()));
    if ( chkHELP ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_HELP,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strHELP).c_str()));
    if ( chkCONTEXT_HELP ) Code.Append(wxString::Format(_T("%s->AddButton(new wxButton(%s,wxID_CONTEXT_HELP,%s));\n"),
        BaseProperties.VarName.c_str(),Params.ParentName.c_str(),wxsGetWxString(strCONTEXT_HELP).c_str()));

    Code.Append(wxString::Format(_T("%s->Realize();\n"),
        BaseProperties.VarName.c_str()));

    return Code;
}

bool wxsStdDialogButtonSizer::MyXmlLoad()
{
    chkOK = false;
    chkYES = false;
    chkCANCEL = false;
    chkSAVE = false;
    chkAPPLY = false;
    chkNO = false;
    chkHELP = false;
    chkCONTEXT_HELP = false;
    strOK = wxEmptyString;
    strYES = wxEmptyString;
    strCANCEL = wxEmptyString;
    strSAVE = wxEmptyString;
    strAPPLY = wxEmptyString;
    strNO = wxEmptyString;
    strHELP = wxEmptyString;
    strCONTEXT_HELP = wxEmptyString;
    for ( TiXmlElement* button = XmlElem()->FirstChildElement("object"); button; button = button->NextSiblingElement("object") )
    {
        TiXmlElement* button2 = button->FirstChildElement("object");
        if ( !button2 ) continue;
        wxString type = cbC2U(button2->Attribute("class"));
        if ( type != _T("wxButton") ) continue;

        wxString id = cbC2U(button2->Attribute("name"));
        if ( id == _T("wxID_OK") )
        {
            chkOK = true;
            strOK = GetLabel(button2);
        }
        else if ( id == _T("wxID_YES") )
        {
            chkYES = true;
            strYES = GetLabel(button2);
        }
        else if ( id == _T("wxID_CANCEL") )
        {
            chkCANCEL = true;
            strCANCEL = GetLabel(button2);
        }
        else if ( id == _T("wxID_SAVE") )
        {
            chkSAVE = true;
            strSAVE = GetLabel(button2);
        }
        else if ( id == _T("wxID_APPLY") )
        {
            chkAPPLY = true;
            strAPPLY = GetLabel(button2);
        }
        else if ( id == _T("wxID_NO") )
        {
            chkNO = true;
            strNO = GetLabel(button2);
        }
        else if ( id == _T("wxID_HELP") )
        {
            chkHELP = true;
            strHELP = GetLabel(button2);
        }
        else if ( id == _T("wxID_CONTEXT_HELP") )
        {
            chkCONTEXT_HELP = true;
            strCONTEXT_HELP = GetLabel(button2);
        }

    }
    return true;
}

bool wxsStdDialogButtonSizer::MyXmlSave()
{
    if ( chkOK ) AddButton(_T("wxID_OK"),strOK);
    if ( chkYES ) AddButton(_T("wxID_YES"),strYES);
    if ( chkCANCEL ) AddButton(_T("wxID_CANCEL"),strCANCEL);
    if ( chkSAVE ) AddButton(_T("wxID_SAVE"),strSAVE);
    if ( chkAPPLY ) AddButton(_T("wxID_APPLY"),strAPPLY);
    if ( chkNO ) AddButton(_T("wxID_NO"),strNO);
    if ( chkHELP ) AddButton(_T("wxID_HELP"),strHELP);
    if ( chkCONTEXT_HELP ) AddButton(_T("wxCONTEXT_HELP"),strCONTEXT_HELP);
    return true;
}

void wxsStdDialogButtonSizer::MyCreateProperties()
{
    Properties.AddProperty(_("OK:"),chkOK,-1);
    Properties.AddProperty(_("  Label:"),strOK,-1);
    Properties.AddProperty(_("Yes:"),chkYES,-1);
    Properties.AddProperty(_("  Label:"),strYES,-1);
    Properties.AddProperty(_("No:"),chkNO,-1);
    Properties.AddProperty(_("  Label:"),strNO,-1);
    Properties.AddProperty(_("Cancel:"),chkCANCEL,-1);
    Properties.AddProperty(_("  Label:"),strCANCEL,-1);
    Properties.AddProperty(_("Apply:"),chkAPPLY,-1);
    Properties.AddProperty(_("  Label:"),strAPPLY,-1);
    Properties.AddProperty(_("Save:"),chkSAVE,-1);
    Properties.AddProperty(_("  Label:"),strSAVE,-1);
    Properties.AddProperty(_("Help:"),chkHELP,-1);
    Properties.AddProperty(_("  Label:"),strHELP,-1);
    Properties.AddProperty(_("Context help:"),chkCONTEXT_HELP,-1);
    Properties.AddProperty(_("  Label:"),strCONTEXT_HELP,-1);
    wxsWidget::MyCreateProperties();
}

void wxsStdDialogButtonSizer::Init()
{
    chkOK = true;
    chkYES = false;
    chkCANCEL = true;
    chkSAVE = false;
    chkAPPLY = false;
    chkNO = false;
    chkHELP = false;
    chkCONTEXT_HELP = false;
    strOK = wxEmptyString;
    strYES = wxEmptyString;
    strCANCEL = wxEmptyString;
    strSAVE = wxEmptyString;
    strAPPLY = wxEmptyString;
    strNO = wxEmptyString;
    strHELP = wxEmptyString;
    strCONTEXT_HELP = wxEmptyString;
}

wxSizer* wxsStdDialogButtonSizer::ProduceSizer(wxWindow* Panel)
{
	wxStdDialogButtonSizer* Sizer = new wxStdDialogButtonSizer();

    if ( chkOK ) Sizer->AddButton(new wxButton(Panel,wxID_OK,strOK));
    if ( chkYES ) Sizer->AddButton(new wxButton(Panel,wxID_YES,strYES));
    if ( chkCANCEL ) Sizer->AddButton(new wxButton(Panel,wxID_CANCEL,strCANCEL));
    if ( chkSAVE ) Sizer->AddButton(new wxButton(Panel,wxID_SAVE,strSAVE));
    if ( chkAPPLY ) Sizer->AddButton(new wxButton(Panel,wxID_APPLY,strAPPLY));
    if ( chkNO ) Sizer->AddButton(new wxButton(Panel,wxID_NO,strNO));
    if ( chkHELP ) Sizer->AddButton(new wxButton(Panel,wxID_HELP,strHELP));
    if ( chkCONTEXT_HELP ) Sizer->AddButton(new wxButton(Panel,wxID_CONTEXT_HELP,strCONTEXT_HELP));
    Sizer->Realize();

	return Sizer;
}

wxString wxsStdDialogButtonSizer::GetLabel(TiXmlElement* button)
{
    TiXmlElement* store = XmlElem();
    XmlAssignElement(button);
    wxString Result = XmlGetVariable(_T("label"));
    XmlAssignElement(store);
    return Result;
}

void wxsStdDialogButtonSizer::AddButton(wxString id,wxString label)
{
    TiXmlElement* button = XmlElem()->InsertEndChild(TiXmlElement("object"))->ToElement();
    button->SetAttribute("class","button");
    TiXmlElement* button2 = button->InsertEndChild(TiXmlElement("object"))->ToElement();
    button2->SetAttribute("class","wxButton");
    button2->SetAttribute("name",cbU2C(id));
    if ( !label.empty() )
    {
        button2->InsertEndChild(TiXmlElement("label"))->InsertEndChild(TiXmlText(cbU2C(label)));
    }
}
