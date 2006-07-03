#ifndef WXSDEFWIDGET_H
#define WXSDEFWIDGET_H

#include "widget.h"
#include "wxswidgetevents.h"

#include <vector>
#include <wx/event.h>


/** This macro starts declaration of new class handling one of default widgets
 *
 * \param Name - class representing widget in wxSmith
 * \param WidgetId - identifier of widget used in default widget manager
 */
#define wxsDWDeclareBegin(Name,WidgetId)                                    \
    class Name : public wxsDefWidget                               \
    {                                                                       \
        public:                                                             \
            Name(wxsWidgetManager* Man,wxsWindowRes* Res):                  \
                wxsDefWidget(Man,Res)                                       \
            { evInit(); }                                                   \
            virtual ~Name()                                                 \
            { evDestroy(); }                                                \
            virtual const wxsWidgetInfo& GetInfo()                          \
            {                                                               \
                return *GetManager()->GetWidgetInfo(WidgetId);              \
            }                                                               \
        protected:                                                          \
            virtual void BuildExtVars();                                    \
            virtual wxWindow* MyCreatePreview(wxWindow* Parent);            \
            virtual wxString GetGeneratingCodeStr();                        \
            virtual wxString GetWidgetNameStr();

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
 *
 * After this macro wxsDWDefXXX macros should be used to assign variables created in declaration
 * with widget's properties
 */
#define wxsDWDefineBegin(Name,WidgetName,Code)                              \
    wxString Name::GetGeneratingCodeStr() { return _T(#Code); }             \
    wxString Name::GetWidgetNameStr() { return _T(#WidgetName); }           \
    wxWindow* Name::MyCreatePreview(wxWindow* WXS_PARENT)                   \
    {                                                                       \
        WidgetName* WXS_THIS;                                               \
        wxWindowID WXS_ID = GetId();                                        \
        wxPoint WXS_POS = GetPosition();                                    \
        wxSize WXS_SIZE = GetSize();                                        \
        long WXS_STYLE = GetStyle();                                        \
        wxString WXS_NAME = GetName();                                      \
        Code;                                                               \
        PreviewApplyDefaults(WXS_THIS);                                     \
        return WXS_THIS;                                                    \
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
        evBool(Name,_T(#Name),_T(#Name),_T(PropName),Default);

/** Extendeed macro assigning given boolean variable with property of widget
 *
 * \param Name - name of Variable
 * \param XrcName - name of field used inside Xrc file
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefBoolX(Name,XrcName,PropName,Default)                        \
        evBool(Name,_T(#Name),_T(XrcName),_T(PropName),Default);

/** Macro assigning given integer variable with property of widget
 *
 * \param Name - name of Variable
 * \param PropName - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefInt(Name,PropName,Default)                                  \
        evInt(Name,_T(#Name),_T(#Name),_T(PropName),Default);

/** Extendeed macro assigning given integer variable with property of widget
 *
 * \param Name - name of Variable
 * \param _T(XrcName) - name of field used inside Xrc file
 * \param _T(PropName) - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefIntX(Name,XrcName,PropName,Default)                         \
        evInt(Name,_T(#Name),_T(XrcName),_T(PropName),Default);

/** Macro assigning pair of integers with property of widget
 *
 * \param V1 - name of first integer
 * \param V2 - name of second integer
 * \param Name - name of property used in code creating preview
 * \param _T(PropName) - name of property in properties manager
 * \param Def1 - default value for first integer
 * \param Def2 - default value for second integer
 */
#define wxsDWDef2Int(V1,V2,Name,PropName,Def1,Def2)                         \
        evInt(V1,V2,_T(#Name),_T(#Name),_T(PropName),Def1,Def2);

/** Extended macro assigning pair of integers with property of widget
 *
 * \param V1 - name of first integer
 * \param V2 - name of second integer
 * \param Name - name of property used in code creating preview
 * \param _T(XrcName) - name of property used in Xrc file
 * \param _T(PropName) - name of property in properties manager
 * \param Def1 - default value for first integer
 * \param Def2 - default value for second integer
 */
#define wxsDWDef2IntX(V1,V2,Name,XrcName,PropName,Def1,Def2)                \
        evInt(V1,V2,_T(#Name),_T(XrcName),_T(PropName),Def1,Def2);

/** Macro assigning given wxString variable with property of widget
 *
 * \param Name - name of Variable
 * \param _T(PropName) - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefStr(Name,PropName,Default)                                  \
        evStr(Name,_T(#Name),_T(#Name),_T(PropName),_T(Default),false);

/** Extendeed macro assigning given wxString variable with property of widget
 *
 * \param Name - name of Variable
 * \param _T(XrcName) - name of field used inside Xrc file
 * \param _T(PropName) - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefStrX(Name,XrcName,PropName,Default)                         \
        evStr(Name,_T(#Name),_T(XrcName),_T(PropName),_T(Default),false);

/** Macro assigning given wxString variable with property of widget
 *  String will be edited in long-string mode
 *
 * \param Name - name of Variable
 * \param _T(PropName) - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefLongStr(Name,PropName,Default)                              \
        evStr(Name,_T(#Name),_T(#Name),_T(PropName),_T(Default),true);

/** Extendeed macro assigning given wxString variable with property of widget
 *
 * \param Name - name of Variable
 * \param _T(XrcName) - name of field used inside Xrc file
 * \param _T(PropName) - name of property in properties manager
 * \param Default - default value
 */
#define wxsDWDefLongStrX(Name,XrcName,PropName,Default)                     \
        evStr(Name,_T(#Name),_T(XrcName),_T(PropName),_T(Default));

/** Macro assigning given wxArrayString variable with property of widget
 *
 * WARNING: Current implementation require using wxsDWDefInt with Default variable
 *          defined in ordeer to use wxsDWSelectString call while building widget
 *
 * \param Name - name of Variable
 * \param PropName - name of property in properties manager
 * \param Default - default value (currently not used)
 * \param SortFlag - flag which must be set for widget to sort list of items
 */
#define wxsDWDefStrArray(Name,PropName,Default,SortFlag)                             \
        evStrArray(Name,_T(#Name),_T(#Name),_T(#Name),_T(PropName),Default,SortFlag);

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
 * \param SortFlag - flag which must be set for widget to sort list of items
 */
#define wxsDWDefStrArrayX(Name,XrcParentName,XrcChildName,PropName,Default,SortFlag) \
        evStrArray(Name,_T(#Name),_T(XrcParentName),_T(XrcChildName),_T(PropName),Default,SortFlag);

// TODO (super##): Adopt code for AdvImageProperty

#define wxsDWDefAdvImage(Name,PropName,Default)                     \
        evImage(Name,_T(#Name),_T(#Name),_T(PropName),_T(Default));

#define wxsDWDefAdvImageX(Name,XrcName,PropName,Default)                     \
        evImage(Name,_T(#Name),_T(XrcName),_T(PropName),_T(Default));




/** Macro finalizing definition of class handling one of default widgets
 */
#define wxsDWDefineEnd()                                                    \
    }

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

/** Inline function used to select one item from wxString variable
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
		wxsDefWidget(wxsWidgetManager* Man,wxsWindowRes* Res);

        /** Destructor */
		virtual ~wxsDefWidget();

        virtual wxString GetProducingCode(const wxsCodeParams& Params);

    protected:

        virtual bool MyXmlLoad();
        virtual bool MyXmlSave();
        virtual void MyCreateProperties();

        void evBool(bool& Val,const wxString& Name,const wxString& XrcName,const wxString& PropName,bool DefValue);
        void evInt(int& Val,const wxString& Name,const wxString& XrcName,const wxString& PropName,int DefValue);
        void ev2Int(int& Val1,int& Val2,const wxString& XrcName,const wxString& Name,const wxString& PropName,int DefValue1,int DefValue2);
        void evStr(wxString& Val,const wxString& Name,const wxString& XrcName,const wxString& PropName,wxString DefValue,bool Long);
        void evStrArray(wxArrayString& Val,const wxString& Name,const wxString& XrcParentName,const wxString& XrcChildName,const wxString& PropName, int& DefValue,int SortFlag);
        void evImage(wxString& Val,const wxString& Name,const wxString& XrcName,const wxString& PropName,wxString DefValue);

        virtual void BuildExtVars() = 0;
        virtual wxString GetGeneratingCodeStr() = 0;
        virtual wxString GetWidgetNameStr() = 0;

        void evInit();
        void evDestroy();

    private:

        enum evUseType { Init, XmlL, XmlS, Code, Props, Destroy };

        evUseType evUse;
        bool Return;
        wxString CodeResult;

        void evXmlL();
        void evXmlS();
        void evCode();
        void evProps();
};

#endif // WXSDEFWIDGET_H
