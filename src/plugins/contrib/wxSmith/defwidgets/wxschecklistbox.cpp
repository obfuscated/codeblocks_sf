#include "../wxsheaders.h"
#include "wxschecklistbox.h"

#include <wx/checklst.h>

WXS_ST_BEGIN(wxsCheckListBoxStyles)
    WXS_ST_CATEGORY("wxCheckListBox")
    WXS_ST_MASK(wxLB_HSCROLL,wxsSFWin,0,true) // Windows ONLY
    WXS_ST(wxLB_SINGLE)
    WXS_ST(wxLB_MULTIPLE)
    WXS_ST(wxLB_EXTENDED)
    WXS_ST(wxLB_ALWAYS_SB)
    WXS_ST(wxLB_NEEDED_SB)
    WXS_ST(wxLB_SORT)
WXS_ST_END(wxsCheckListBoxStyles)

WXS_EV_BEGIN(wxsCheckListBoxEvents)
    WXS_EVI(EVT_CHECKLISTBOX,wxCommandEvent,Toggled)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsCheckListBoxEvents)

wxsDWDefineBegin(wxsCheckListBoxBase,wxCheckListBox,
        WXS_THIS = new wxCheckListBox(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,0,0,WXS_STYLE);
        wxsDWAddStrings(arrayChoices,WXS_THIS);
    )
wxsDWDefineEnd()

wxString wxsCheckListBox::GetProducingCode(const wxsCodeParams& Params)
{
    wxString Code;
    Code.Printf(_T("%s = new wxCheckListBox(%s,%s,%s,%s,0,NULL,%s,wxDefaultValidator,%s);\n"),
        Params.VarName.c_str(),
        Params.ParentName.c_str(),
        Params.IdName.c_str(),
        Params.Pos.c_str(),
        Params.Size.c_str(),
        Params.Style.c_str(),
        Params.Name.c_str());

    for ( size_t i = 0; i < arrayChoices.Count(); ++i )
    {
        Code << wxString::Format(_T("%s->Append(%s);\n"),
            Params.VarName.c_str(),
            wxsGetWxString(arrayChoices[i]).c_str());
    }

    for ( size_t i = 0; i < arrayChecks.Count(); ++i )
    {
        if ( arrayChecks[i] )
        {
            Code << wxString::Format(_T("%s->Check(%d,true);\n"),
                Params.VarName.c_str(), i);
        }
    }
    Code << Params.InitCode;
    return Code;
}

wxWindow* wxsCheckListBox::MyCreatePreview(wxWindow* Parent)
{
    wxCheckListBox* Wnd = new wxCheckListBox(
        Parent,-1L,GetPosition(),GetSize(),0,NULL,GetStyle(),wxDefaultValidator,GetName());

    for ( size_t i = 0; i < arrayChoices.Count(); ++i )
    {
        Wnd->Append(arrayChoices[i]);
    }

    for ( size_t i = 0; i < arrayChecks.Count(); ++i )
    {
        if ( arrayChecks[i] )
        {
            Wnd->Check(i,true);
        }
    }
    return Wnd;
}

bool wxsCheckListBox::MyXmlLoad()
{
    TiXmlElement* Content = XmlElem()->FirstChildElement("content");
    if ( !Content ) return true;

    for ( TiXmlElement* Item = Content->FirstChildElement("item");
          Item;
          Item = Item->NextSiblingElement("item") )
    {
        bool Checked = cbC2U(Item->Attribute("checked")) == _T("1");
        wxString Text;
        for ( TiXmlNode* FindText = Item->FirstChild(); FindText; FindText = FindText->NextSibling() )
        {
            TiXmlText* TextNode = FindText->ToText();
            if ( TextNode )
            {
                Text = cbC2U(TextNode->Value());
                break;
            }
        }

        arrayChoices.Add(Text);
        arrayChecks.Add(Checked);
    }

    return true;
}

bool wxsCheckListBox::MyXmlSave()
{
    TiXmlElement* Elem = XmlElem()->InsertEndChild(TiXmlElement("content"))->ToElement();
    if ( !Elem ) return false;

    for ( size_t i = 0; i<arrayChoices.Count(); ++i )
    {
        TiXmlElement* Item = Elem->InsertEndChild(TiXmlElement("item"))->ToElement();
        if ( !Item ) return false;
        Item->SetAttribute("checked",arrayChecks[i]?"1":"0");
        Item->InsertEndChild(TiXmlText(cbU2C(arrayChoices[i])));
    }

    return true;
}

void wxsCheckListBox::MyCreateProperties()
{
    Properties.AddProperty(_("Content:"),new wxsStringListCheckProperty(arrayChoices,arrayChecks,wxLB_SORT));
    wxsWidget::MyCreateProperties();
}
