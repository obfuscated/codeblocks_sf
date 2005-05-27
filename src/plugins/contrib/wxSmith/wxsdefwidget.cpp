#include "wxsdefwidget.h"

wxsDefWidget::wxsDefWidget(wxsWidgetManager* Man,BasePropertiesType pType):
    wxsWidget(Man,pType)
{
    evInit();
}

wxsDefWidget::~wxsDefWidget()
{
}

bool wxsDefWidget::MyXmlLoad()
{
    Return = true;
    evXmlL();
    return Return;
}

bool wxsDefWidget::MyXmlSave()
{
    Return = true;
    evXmlS();
    return Return;
}

void wxsDefWidget::CreateObjectProperties()
{
    evProps();
    wxsWidget::CreateObjectProperties();
}

const char* wxsDefWidget::GetProducingCode(wxsCodeParams& Params)
{
    CodeResult = GetGeneratingCodeStr();
    
    CodeReplace(wxT("ThisWidget"),BaseParams.VarName);
    CodeReplace(wxT("parent"),Params.ParentName);
    CodeReplace(wxT("id"),BaseParams.IdName);
    CodeReplace(wxT("pos"),GetCodeDefines().Pos);
    CodeReplace(wxT("size"),GetCodeDefines().Size);
    CodeReplace(wxT("style"),GetCodeDefines().Style);
    CodeReplace(wxT("font"),GetCodeDefines().Font);
    CodeReplace(wxT("fcolour"),GetCodeDefines().FColour);
    CodeReplace(wxT("bcolour"),GetCodeDefines().BColour);
    
    evCode();
    return CodeResult.c_str();
}

void wxsDefWidget::evInit()
{
    evUse = Init;
    BuildExtVars();
}

void wxsDefWidget::evXmlL()
{
    evUse = XmlL;
    BuildExtVars();
}

void wxsDefWidget::evXmlS()
{
    evUse = XmlS;
    BuildExtVars();
}

void wxsDefWidget::evCode()
{
    evUse = Code;
    BuildExtVars();
}

void wxsDefWidget::evProps()
{
    evUse = Props;
    BuildExtVars();
}

void wxsDefWidget::CodeReplace(const wxString& Old,const wxString& New)
{
// TODO (SpOoN#1#): Create something more intelligent
    CodeResult.Replace(Old,New,true);
}

const char* wxsDefWidget::GetDeclarationCode(wxsCodeParams& Params)
{
    static wxString Tmp;
    Tmp = wxT(GetWidgetNameStr());
    Tmp.Append(' ');
    Tmp += BaseParams.VarName;
    Tmp.Append(';');
    return Tmp.c_str();
}


void wxsDefWidget::evBool(bool& Val,char* Name,char* PropName,bool DefValue)
{
    switch ( evUse )
    {
        case Init:
        {
            Val = DefValue;
            break;
        }
        
        case XmlL:
        {
            const char* Var = XmlGetVariable(Name);
            if ( Var ) Val = strcasecmp(Var,"false") ? true : false;
            else Val = DefValue;
            break;
        }
        
        case XmlS:
        {
            if ( Val != DefValue )
            {
                XmlSetVariable(Name,Val?"true":"false");
            }
            break;
        }
        
        case Code:
        {
            CodeReplace(Name,wxString::Format("%s",Val?"true":"false"));
            break;
        }
        
        case Props:
        {
            PropertiesObject.AddProperty(Name,Val);
            break;
        }
    }
}

void wxsDefWidget::evInt(int& Val,char* Name,char* PropName,int DefValue)
{
    switch ( evUse )
    {
        case Init:
        {
            Val = DefValue;
            break;
        }
        
        case XmlL:
        {
            Val = XmlGetInteger(Name,DefValue);
            break;
        }
        
        case XmlS:
        {
            if ( Val != DefValue )
            {
                XmlSetInteger(Name,Val);
            }
            break;
        }
        
        case Code:
        {
            CodeReplace(Name,wxString::Format("%d",Val));
            break;
        }
        
        case Props:
        {
            PropertiesObject.AddProperty(Name,Val);
            break;
        }
    }
}

void wxsDefWidget::ev2Int(int& Val1,int& Val2,char* Name,char* PropName,int DefValue1,int DefValue2)
{
    switch ( evUse )
    {
        case Init:
        {
            Val1 = DefValue1;
            Val2 = DefValue2;
            break;
        }
        
        case XmlL:
        {
            XmlGetIntPair(Name,Val1,Val2,DefValue1,DefValue2);
            break;
        }
        
        case XmlS:
        {
            if ( Val1!=DefValue1 || Val2!=DefValue2 )
            {
                XmlSetIntPair(Name,Val1,Val2);
            }
            break;
        }
        
        case Code:
        {
            CodeReplace(Name,wxString::Format("wxPoint(%d,%d)",Val1,Val2));
            break;
        }
        
        case Props:
        {
            PropertiesObject.Add2IProperty(Name,Val1,Val2);
            break;
        }
    }
}

void wxsDefWidget::evStr(wxString& Val,char* Name,char* PropName,wxString DefValue)
{
    switch ( evUse )
    {
        case Init:
        {
            Val = DefValue;
            break;
        }
        
        case XmlL:
        {
            const char* Value = XmlGetVariable(Name);
            if ( Value ) Val = Value;
            else Val = DefValue;
            break;
        }
        
        case XmlS:
        {
            if ( Val != DefValue )
            {
                XmlSetVariable(Name,Val);
            }
            break;
        }
        
        case Code:
        {
            CodeReplace(Name,wxString::Format("wxT(%s)",GetCString(Val).c_str()));
            break;
        }
        
        case Props:
        {
            PropertiesObject.AddProperty(Name,Val);
        }
    }
}

