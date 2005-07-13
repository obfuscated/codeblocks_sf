#ifndef WXSDEFWIDGET_H
#define WXSDEFWIDGET_H

#include "widget.h"

#include <vector>
#include <wx/event.h>


/** This macro starts declaration of new class handling one of default widgets
 *
 * \param Name - class representing widget in wxSmith
 * \param pType - type of widget (for most cases it should be propWidget)
 * \param WidgetId - identifier of widget used in default widget manager
 */
#define wxsDWDeclareBegin(Name,pType,WidgetId)                              \
    class Name : public wxsDefWidget                                        \
    {                                                                       \
        public:                                                             \
            Name(wxsWidgetManager* Man): wxsDefWidget(Man,pType)            \
            { evInit(); }                                                   \
            virtual ~Name()                                                 \
            {                                                               \
              evDestroy();                                                  \
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
 
/** This macro finishes declaration of new class handling one of default widgets
 *
 * Betwen wxsDWDeclareBegin() and wxsDWDDeclareEnd() declarations of 
 * addional variables describing widget should be placed. 
 */
#define wxsDWDeclareEnd()                                                   \
    };

/** This macro begins definition of class handling one of default widgets
 *
 * \param Name - class representing widget in wxSmith
 * \param WidgetName - class representing this widget in real application
 * \param Code - code generating preview which will also be used while generating code on-the-fly
 * \param SkipEvents - true if preview of this widget should skip all default mouse events, false if mouse events are processed
 *
 * After this macro wxsDWDefXXX macros should be used to assign variables created in declaration
 * with widget's properties
 */
#define wxsDWDefineBeginExt(Name,WidgetName,Code,SkipEvents)                \
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
        Code;                                                               \
        if ( SkipEvents )													\
        {																	\
			ThisWidget->PushEventHandler(&wxsStopMouseEvents::GetObject());	\
        }																	\
        return ThisWidget;                                                  \
    }                                                                       \
    void Name::BuildExtVars()                                               \
    {


/** Macro assigning given boolean variable with property of widget
 *
 * \param Name - name of Variable
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefBool(Name,PropName,Default)                                 \
        evBool(Name,#Name,#Name,PropName,Default);

/** Extendeed macro assigning given boolean variable with property of widget
 *
 * \param Name - name of Variable
 * \param XrcName - name of field used inside Xrc file
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefBoolX(Name,XrcName,PropName,Default)                        \
        evBool(Name,#Name,XrcName,PropName,Default);

/** Macro assigning given integer variable with property of widget
 *
 * \param Name - name of Variable
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefInt(Name,PropName,Default)                                  \
        evInt(Name,#Name,#Name,PropName,Default);

/** Extendeed macro assigning given integer variable with property of widget
 *
 * \param Name - name of Variable
 * \param XrcName - name of field used inside Xrc file
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefIntX(Name,XrcName,PropName,Default)                         \
        evInt(Name,#Name,XrcName,PropName,Default);

/** Macro assigning pair of integers with property of widget
 *
 * \param V1 - name of first integer
 * \param V2 - name of second integer
 * \param Name - name of property used in code creating preview
 * \param PropName - name of property in properties manager
 * \param Def1 - default value for first integer
 * \param Def2 - default value for second integer
 */
#define wxsDWDef2Int(V1,V2,Name,PropName,Def1,Def2)                         \
        evInt(V1,V2,#Name,#Name,PropName,Def1,Def2);

/** Extended macro assigning pair of integers with property of widget
 *
 * \param V1 - name of first integer
 * \param V2 - name of second integer
 * \param Name - name of property used in code creating preview
 * \param XrcName - name of property used in Xrc file
 * \param PropName - name of property in properties manager
 * \param Def1 - default value for first integer
 * \param Def2 - default value for second integer
 */
#define wxsDWDef2IntX(V1,V2,Name,XrcName,PropName,Def1,Def2)                \
        evInt(V1,V2,#Name,XrcName,PropName,Def1,Def2);

/** Macro assigning given wxString variable with property of widget
 *
 * \param Name - name of Variable
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefStr(Name,PropName,Default)                                  \
        evStr(Name,#Name,#Name,PropName,Default);
        
/** Extendeed macro assigning given wxString variable with property of widget
 *
 * \param Name - name of Variable
 * \param XrcName - name of field used inside Xrc file
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefStrX(Name,XrcName,PropName,Default)                         \
        evStr(Name,#Name,XrcName,PropName,Default);

/** Macro assigning given wxArrayString variable with property of widget
 *
 * WARNING: Current implementation require using wxsDWDefInt with Default variable
 *          defined in ordeer to use wxsDWSelectString call while building widget
 *
 * \param Name - name of Variable
 * \param PropName - name of property in properties manager
 * \param Default - default value (currently not used)
 */
#define wxsDWDefStrArray(Name,PropName,Default)                             \
        evStrArray(Name,#Name,#Name,#Name,PropName,Default);

/** Extended macro assigning given wxArrayString variable with property of widget
 *
 * WARNING: Current implementation require using wxsDWDefInt with Default variable
 *          defined in ordeer to use wxsDWSelectString call while building widget
 *
 * \param Name - name of Variable
 * \param XrcParentName - name of Xml element handling all items in Xrc file
 * \param XrcChildName - name of Xml element handling one item in Xrc file
 * \param PropName - name of property in properties manager
 * \param Default - default value (currently not used)
 */
#define wxsDWDefStrArrayX(Name,XrcParentName,XrcChildName,PropName,Default) \
        evStrArray(Name,#Name,XrcParentName,XrcChildName,PropName,Default);
        
/** Macro finalizing definition of class handling one of default widgets
 */
#define wxsDWDefineEnd()                                                    \
    }

/** Macro which can be replacement for wxsDWDefineBeginExt setting all extended
 * variables to default values
 */
#define wxsDWDefineBegin(Name,WidgetName,Code)                				\
	wxsDWDefineBeginExt(Name,WidgetName,Code,false)


/** Inline function used to insert all items from wxString variable into widget
 *  dedrived from wxControllWithItems
 *
 * \param Array - array of strings
 * \param Ctrl - control into which strings will be addeed
 */
inline void wxsDWAddStrings(const wxArrayString& Array,wxControlWithItems* Ctrl)
{
    for ( size_t i=0; i<Array.GetCount(); i++ )
        Ctrl->Append(Array[i]);
}

/** Inline procedure used to select one item from wxString variable
 *
 * \param Array - array of strings
 * \param Index - index of item to use
 * \param Ctrl - control which will be used
 */
 
inline void wxsDWSelectString(const wxArrayString& Array,size_t Index,wxControlWithItems* Ctrl)
{
    Ctrl->SetSelection(Index);
}


/** Base class for all default widgets */
class wxsDefWidget: public wxsWidget
{
	public:
	
        /** Default costroctor, arguments are passed directly to wxsWidget */
		wxsDefWidget(wxsWidgetManager* Man,BasePropertiesType pType = propWidget);
            
        /** Destructor - it calls evDestroy() alowing all variables to be released */
		virtual ~wxsDefWidget();
		
        virtual const char* GetProducingCode(wxsCodeParams& Params);
		
        virtual const char* GetDeclarationCode(wxsCodeParams& Params);
        
    protected:

        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();
        virtual void CreateObjectProperties();

        void evBool(bool& Val,char* Name,char* XrcName,char* PropName,bool DefValue);
        void evInt(int& Val,char* Name,char* XrcName,char* PropName,int DefValue);
        void ev2Int(int& Val1,int& Val2,char* XrcName,char* Name,char* PropName,int DefValue1,int DefValue2);
        void evStr(wxString& Val,char* Name,char* XrcName,char* PropName,wxString DefValue);
        void evStrArray(wxArrayString& Val,char* Name,char* XrcParentName,char* XrcChildName,char* PropName, int& DefValue);
        
        virtual void BuildExtVars() = 0;
        virtual const char* GetGeneratingCodeStr() = 0;
        virtual const char* GetWidgetNameStr() = 0;
        
        void evInit();
        // Added by cyberkoa
        void evDestroy();
        // End Added
    private:
        // Modified by cyberkoa
        enum evUseType { Init, XmlL, XmlS, Code, Props, Destroy };
        // End Modified
        
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


/** Declaration of class which stops processing mouse events */
class wxsStopMouseEvents: public wxEvtHandler
{
	public:
	
		virtual ~wxsStopMouseEvents() {}
		inline static wxsStopMouseEvents& GetObject() { return Object; }
		
	private:
		
		static wxsStopMouseEvents Object;
        void SkipEvent(wxMouseEvent& event);
		DECLARE_EVENT_TABLE()
};




#endif // WXSDEFWIDGET_H
