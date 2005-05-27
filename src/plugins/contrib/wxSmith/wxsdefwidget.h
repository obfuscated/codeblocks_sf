#ifndef WXSDEFWIDGET_H
#define WXSDEFWIDGET_H

#include "widget.h"
#include "wxsdefevthandler.h"

#include <vector>


#define wxsDWDeclareBegin(Name,pType,WidgetId)                              \
    class Name : public wxsDefWidget                                        \
    {                                                                       \
        public:                                                             \
            Name(wxsWidgetManager* Man): wxsDefWidget(Man,pType)            \
            { evInit(); }                                                   \
            virtual ~Name()                                                 \
            {                                                               \
            }                                                               \
            virtual const wxsWidgetInfo& GetInfo()                          \
            {                                                               \
                return *GetManager()->GetWidgetInfo(WidgetId);              \
            }                                                               \
        protected:                                                          \
            virtual void BuildExtVars();                                    \
            virtual wxWindow* MyCreatePreview(wxWindow* Parent);            \
            virtual const char* GetGeneratingCodeStr();                     \
            virtual const char* GetWidgetNameStr();                         \
        private:;
    
#define wxsDWDeclareEnd()                                                   \
    };

#define wxsDWDefineBegin(Name,WidgetName,Code)                              \
    const char* Name::GetGeneratingCodeStr() { return #Code; }              \
    const char* Name::GetWidgetNameStr() { return #WidgetName; }            \
    wxWindow* Name::MyCreatePreview(wxWindow* parent)                       \
    {                                                                       \
        WidgetName* ThisWidget;                                             \
        wxWindowID id = -1;                                                 \
        wxPoint pos = BaseParams.DefaultPosition ?                          \
            wxDefaultPosition :                                             \
            wxPoint(BaseParams.PosX,BaseParams.PosY);                       \
        wxSize size = BaseParams.DefaultSize ?                              \
            wxDefaultSize :                                                 \
            wxSize(BaseParams.SizeX,BaseParams.SizeY);                      \
        long style = BaseParams.Style;                                      \
        DebLog("%s",#Code);                                                 \
        Code;                                                               \
        ThisWidget->PushEventHandler(new wxsDefEvtHandler(this));           \
        return ThisWidget;                                                  \
    }                                                                       \
    void Name::BuildExtVars()                                               \
    {

#define wxsDWDefBool(Name,Default)                                          \
        evBool(Name,#Name,#Name,Default);                                     

#define wxsDWDefBoolP(Name,PropName,Default)                                \
        evBool(Name,#Name,PropName,Default);

#define wxsDWDefInt(Name,Default)                                           \
        evInt(Name,#Name,#Name,Default);                                     

#define wxsDWDefIntP(Name,PropName,Default)                                 \
        evInt(Name,#Name,PropName,Default);

#define wxsDWDef2Int(V1,V2,Name,Def1,Def2)                                  \
        evInt(V1,V2,#Name,#Name,Def1,Def2);                                   

#define wxsDWDef2IntP(V1,V2,Name,PropName,Def1,Def2)                        \
        evInt(V1,V2,#Name,PropName,Def1,Def2);

#define wxsDWDefStr(Name,Default)                                           \
        evStr(Name,#Name,#Name,Default);                                     
        
#define wxsDWDefStrP(Name,PropName,Default)                                 \
        evStr(Name,#Name,PropName,Default);
        
#define wxsDWDefineEnd()                                                    \
    }

class wxsDefWidget: public wxsWidget
{
	public:
	
        /** Default costroctor, arguments are paswd directly to wxsWidget */
		wxsDefWidget(wxsWidgetManager* Man,BasePropertiesType pType = propWidget);
            
        /** Destructor - currently does nothing */
		virtual ~wxsDefWidget();
		
        virtual const char* GetProducingCode(wxsCodeParams& Params);
		
        virtual const char* GetDeclarationCode(wxsCodeParams& Params);
        
    protected:

        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();
        virtual void CreateObjectProperties();

        void evBool(bool& Val,char* Name,char* PropName,bool DefValue);
        void evInt(int& Val,char* Name,char* PropName,int DefValue);
        void ev2Int(int& Val1,int& Val2,char* Name,char* PropName,int DefValue1,int DefValue2);
        void evStr(wxString& Val,char* Name,char* PropName,wxString DefValue);
        
        virtual void BuildExtVars() = 0;
        virtual const char* GetGeneratingCodeStr() = 0;
        virtual const char* GetWidgetNameStr() = 0;
        
        void evInit();
        
    private:
    
        enum evUseType { Init, XmlL, XmlS, Code, Props };
        
        evUseType evUse;
        bool Return;
        wxString CodeResult;
        
        void evXmlL();
        void evXmlS();
        void evCode();
        void evProps();
        
        /** This function does replace the Old identifier with New content */
        void CodeReplace(const wxString& Old,const wxString& New);
        
};



#endif // WXSDEFWIDGET_H
