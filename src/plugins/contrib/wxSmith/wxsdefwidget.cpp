#include "wxsheaders.h"
#include "wxsdefwidget.h"

#include "properties/wxsstringlistproperty.h"
#include "properties/wxsstringproperty.h"
#include "wxsglobals.h"

wxsDefWidget::wxsDefWidget(wxsWidgetManager* Man,wxsWindowRes* Res,BasePropertiesType pType):
    wxsWidget(Man,Res,pType)
{
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

wxString wxsDefWidget::GetProducingCode(wxsCodeParams& Params)
{
    const CodeDefines& CD = GetCodeDefines();

    CodeResult = GetGeneratingCodeStr();

    CodeReplace(_T("ThisWidget"),BaseParams.VarName);
    CodeReplace(_T("parent"),Params.ParentName);
    CodeReplace(_T("id"),BaseParams.IdName);
    CodeReplace(_T("pos"),CD.Pos);
    CodeReplace(_T("size"),CD.Size);
    CodeReplace(_T("style"),CD.Style);

    evCode();

    // Applying default initializing code

    CodeResult << CD.InitCode;

    return CodeResult;
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

void wxsDefWidget::evDestroy()
{
    evUse = Destroy;
    BuildExtVars();
}

void wxsDefWidget::CodeReplace(const wxString& Old,const wxString& New)
{
    CodeResult.Replace(Old,New,true);
}

wxString wxsDefWidget::GetDeclarationCode(wxsCodeParams& Params)
{
    static wxString Tmp;
    Tmp = GetWidgetNameStr();
    Tmp.Append(_T("* "));
    Tmp += BaseParams.VarName;
    Tmp.Append(_T(';'));
    return Tmp;
}


void wxsDefWidget::evBool(bool& Val,const wxString& Name,const wxString& XrcName,const wxString& PropName,bool DefValue)
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
            Val = XmlGetInteger(XrcName,DefValue?1:0) != 0;
            break;
        }

        case XmlS:
        {
            if ( Val != DefValue )
            {
                XmlSetInteger(XrcName,Val?1:0);
            }
            break;
        }

        case Destroy:
        {
            break;
        }

        case Code:
        {
            CodeReplace(Name,Val?_T("true"):_T("false"));
            break;
        }

        case Props:
        {
        	if ( PropName.Length() )
        	{
                PropertiesObject.AddProperty(PropName,Val);
        	}
            break;
        }
    }
}

void wxsDefWidget::evInt(int& Val,const wxString& Name,const wxString& XrcName,const wxString& PropName,int DefValue)
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
            Val = XmlGetInteger(XrcName,DefValue);
            break;
        }

        case XmlS:
        {
            if ( Val != DefValue )
            {
                XmlSetInteger(XrcName,Val);
            }
            break;
        }

        case Destroy:
        {
            break;
        }

        case Code:
        {
            CodeReplace(Name,wxString::Format(_T("%d"),Val));
            break;
        }

        case Props:
        {
        	if ( PropName.Length() )
        	{
                PropertiesObject.AddProperty(PropName,Val);
        	}
            break;
        }
    }
}

void wxsDefWidget::ev2Int(int& Val1,int& Val2,const wxString& Name,const wxString& XrcName,const wxString& PropName,int DefValue1,int DefValue2)
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
            XmlGetIntPair(XrcName,Val1,Val2,DefValue1,DefValue2);
            break;
        }

        case XmlS:
        {
            if ( Val1!=DefValue1 || Val2!=DefValue2 )
            {
                XmlSetIntPair(XrcName,Val1,Val2);
            }
            break;
        }

        case Destroy:
        {
            break;
        }

        case Code:
        {
            CodeReplace(Name,wxString::Format(_T("wxPoint(%d,%d)"),Val1,Val2));
            break;
        }

        case Props:
        {
        	if ( PropName.Length() )
        	{
                PropertiesObject.Add2IProperty(PropName,Val1,Val2);
        	}
            break;
        }
    }
}

void wxsDefWidget::evStr(wxString& Val,const wxString& Name,const wxString& XrcName,const wxString& PropName,wxString DefValue,bool Long)
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
            const wxString& Value = XmlGetVariable(XrcName);
            Val = Value;
            break;
        }

        case XmlS:
        {
            XmlSetVariable(XrcName,Val);
            break;
        }

        case Destroy:
        {
            break;
        }

        case Code:
        {
            CodeReplace(Name,GetWxString(Val));
            break;
        }

        case Props:
        {
        	if ( PropName.Length() )
        	{
                PropertiesObject.AddProperty(PropName, new wxsStringProperty(&PropertiesObject,Val,true,Long) );
        	}
        }
    }
}

void wxsDefWidget::evStrArray(wxArrayString& Val,const wxString& Name,const wxString& XrcParentName,const wxString& XrcChildName,const wxString& PropName, int& DefValue,int SortFlag)
{
    switch ( evUse )
    {
        case Init:
        {
            Val.Clear();
            break;
        }

        case XmlL:
        {
			if( !XmlGetStringArray(XrcParentName,XrcChildName,Val) )
			{
				Val.Clear();
			}
            break;
        }

        case XmlS:
        {
			if( !XmlSetStringArray(XrcParentName,XrcChildName,Val) )
			{
				Val.Clear();
			}
            break;
        }

        case Destroy:
        {
			Val.Clear();
            break;
        }

        case Code:
        {
            // Replacing wxsDWAddStrings function calls

            wxString CodeToSearch = wxString::Format(_T("wxsDWAddStrings(%s,%s);"),Name.c_str(),GetBaseParams().VarName.c_str());
            wxString ReplaceWith;
            for ( size_t i = 0; i<Val.GetCount(); i++ )
            {
            	ReplaceWith.Append(GetBaseParams().VarName);
            	ReplaceWith.Append(_T("->Append("));
            	ReplaceWith.Append(GetWxString(Val[i]));
            	ReplaceWith.Append(_T(");\n"));
            }
            CodeReplace(CodeToSearch,ReplaceWith);

            // Replacing wxsDWSelectString function calls

            CodeToSearch.Printf(_T("wxsDWSelectString(%s,%d,%s)"),Name.c_str(),DefValue,GetBaseParams().VarName.c_str());
            ReplaceWith.Printf(_T("%s->SetSelection(%d)"),GetBaseParams().VarName.c_str(),DefValue);
            CodeReplace(CodeToSearch,ReplaceWith);

            break;
        }

        case Props:
        {
        	if ( PropName.Length() )
        	{
                PropertiesObject.AddProperty(PropName,Val,DefValue,SortFlag,-1);
        	}
        }
    }
}
