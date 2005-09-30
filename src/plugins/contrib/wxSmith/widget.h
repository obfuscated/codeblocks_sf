#ifndef __WIDGET_H
#define __WIDGET_H


#include <wx/window.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <tinyxml/tinyxml.h>
#include <settings.h>
#include <manager.h>
#include <messagemanager.h>

#include "wxsglobals.h"
#include "wxsproperties.h"
#include "wxswindoweditor.h"
#include "wxsstyle.h"

#define DebLog Manager::Get()->GetMessageManager()->DebugLog

class wxsWidgetManager;
class wxsWidget;
class wxsWidgetEvents;
class wxsEventDesc;

/** Structure containing info aboul widget */
struct wxsWidgetInfo
{
    wxString Name;                  ///< Widget's name
    wxString License;               ///< Widget's license
    wxString Author;                ///< Widget's author
    wxString AuthorEmail;           ///< Widget's authos's email
    wxString AuthorSite;            ///< Widget's author's site
    wxString WidgetsSite;           ///< Site about this widget
    wxString Category;              ///< Widget's category
    wxString DefaultVarName;        ///< Prefix for default variable name
    bool Container;                 ///< True if this widget can have other widgets inside
    bool Sizer;                     ///< True if this widget is a sizer (Container must also be true)
    bool Spacer;                    ///< True if this is a spacer
    unsigned short VerHi;           ///< Lower number of version
    unsigned short VerLo;           ///< Higher number of version
    wxBitmap* Icon;                 ///< Icon used in pallette
    wxsWidgetManager* Manager;      ///< Manager handling this widget
    int Id;                         ///< Identifier used inside manager to handle this widget, must be same as 'Number' in GetWidgetInfo call
    int TreeIconId;
    wxsStyle* Styles;               ///< Set of available styles, ending with NULL-named style
    wxsEventDesc *Events;           ///< Set of events supported by this widget, enging with NULL-named entry
    wxString HeaderFile;            ///< Header file (including '<' and '>' or '"') for this file
    wxString ExtHeaderFile;         ///< Additional header file
    
    /** Types of extended widgets */
    enum ExTypeT
    {
        exNone = 0,                 ///< This is widget from standard set (no new coded nor additional library needed)
        exCode,                     ///< This widget provides it's source code, currently not supported
        exLibrary                   ///< This widget is provided in additional library, currently not supported
    };
    
    ExTypeT ExType;                 ///< Type of extended widget
    wxString WidgetCodeDefinition;  ///< Code with definition of class for this widget
    wxString WidgetCodeDeclaration; ///< Code with declaration of class for this widget
    wxString WidgetLibrary;         ///< Library including this widget (empty for no library)
};

/** Structure describing default widget's options */
struct wxsWidgetBaseParams
{
    wxString IdName;                ///< Widget's identifier
    wxString VarName;               ///< Widget's variable used inside main window
    bool IsMember;                  ///< True if widget's variable won't be stored inside main window
    int PosX, PosY;                 ///< Widget's position
    bool DefaultPosition;           ///< Widget has default position
    int SizeX, SizeY;               ///< Widget's size
    bool DefaultSize;               ///< Widget has default size
    int Style;                      ///< Current style
    
    bool Enabled;                   ///< If false, widget is disabled (true by deefault)
    bool Focused;                   ///< If true, widget is focused (false by default)
    bool Hidden;                    ///< If true, widget is hidden (false by default)
    
    wxUint32 FgType;                ///< Type of Fg colour (wxSYS_COLOUR_XXX or wxsCUSTOM_COLOUR or wxsNO_COLOUR)
    wxColour Fg;                    ///< Foreground colour when using custom colour
    wxUint32 BgType;                ///< Type of Bg colour (wxSYS_COLOUR_XXX or wxsCUSTOM_COLOUR or wxsNO_COLOUR)
    wxColour Bg;                    ///< Background colour when using custom colour

    bool UseFont;                   ///< Must be true to allow using font
    wxFont Font;                    ///< Font
    
    wxString ToolTip;               ///< Tooltip
    
    wxsWidgetBaseParams():
        IdName(_T("")),
        VarName(_T("")),
        IsMember(true),
        PosX(-1), PosY(-1),
        DefaultPosition(true),
        SizeX(-1), SizeY(-1),
        DefaultSize(true),
        Style(0),
        Enabled(true),
        Focused(false),
        Hidden(false),
        FgType(wxsNO_COLOUR),
        Fg(0,0,0),
        BgType(wxsNO_COLOUR),
        Bg(0,0,0),
        UseFont(false),
        Font(wxNullFont),
        ToolTip(_T(""))
        {}
};


/** Structure containing all data needed while generating code */
struct wxsCodeParams
{
    wxString ParentName;
    bool IsDirectParent;
    int UniqueNumber;
};

/** Class representing one widget */
class wxsWidget
{
    public:
    
        /** Type representing base properties set while creating widget */
        typedef unsigned long BasePropertiesType;
        
        static const BasePropertiesType bptPosition  = 0x0001;  ///< this widget is using position
        static const BasePropertiesType bptSize      = 0x0002;  ///< this widget is using size
        static const BasePropertiesType bptId        = 0x0004;  ///< this widget is using identifier
        static const BasePropertiesType bptVariable  = 0x0008;  ///< this widget is using variable
        static const BasePropertiesType bptStyle     = 0x0010;  ///< this widget is using style
        static const BasePropertiesType bptEnabled   = 0x0020;  ///< this widget uses Enabled property
        static const BasePropertiesType bptFocused   = 0x0040;  ///< this widget uses Focused property
        static const BasePropertiesType bptHidden    = 0x0080;  ///< this widget uses Hidden property
        static const BasePropertiesType bptColours   = 0x0100;  ///< this widget uses colour properties (Fg and Bg)
        static const BasePropertiesType bptToolTip   = 0x0200;  ///< this widget uses tooltips
        static const BasePropertiesType bptFont      = 0x0400;  ///< this widget uses font
        
        /** BasePropertiesType with no default properties */
        static const BasePropertiesType propNone     = 0;        
        
        /** VasePropertiesTyue usede by common windows */
        static const BasePropertiesType propWindow   = bptStyle;
        
        /** BasePropertiesType used by common widgets */
        static const BasePropertiesType propWidget   = bptPosition | bptSize | bptId | bptVariable | bptStyle | bptEnabled | bptFocused | bptHidden | bptColours | bptToolTip | bptFont;
        
        /** BasePropertiesType used by common sizers */
        static const BasePropertiesType propSizer    = bptVariable;
        
        /** BasePropertiesType used by spacer */
        static const BasePropertiesType propSpacer   = bptSize;
    
        /** Default constructor */
        wxsWidget(wxsWidgetManager* Man,wxsWindowRes* Res,BasePropertiesType pType = propNone);
        
        /** Constructor used by containers 
         *
         *  \param Man - widgets manager 
         *  \param ISwxWindow - true if this container is an wxWindow object,
         *                      false otherwise (usually for wxSizer objects
         *  \param MaxChildren - maximal number of children which can be handled by this container
         */
        wxsWidget(wxsWidgetManager* Man, wxsWindowRes* Res, bool ISwxWindow, int MaxChild,BasePropertiesType pType = propNone);
        
        /** Destructor */
        virtual ~wxsWidget(); 
        
        /** Getting widget's info */
        virtual const wxsWidgetInfo& GetInfo() = 0;
        
        /** Getting manager of this widget */
        inline wxsWidgetManager* GetManager() { return Manager; }
        
        /** Getting parent widget of this one */
        inline wxsWidget* GetParent() { return Parent; }
        
        /** Getting resource tree of this widget */
        inline wxTreeItemId GetTreeId() { return TreeId; }
        
        /** Getting BasePropertiesType for this widget */
        inline BasePropertiesType GetBPType() { return BPType; }
        
        /** Getting resource owning this widget */
        inline wxsWindowRes* GetResource() { return Resource; }

        /** Getting default properties object */
        inline wxsProperties& GetPropertiesObj() { return PropertiesObject; }
        
        /** Getting editor for this widget (or NULL if there's no editor) */
        wxsWindowEditor* GetEditor();
        
        /** Getting events object */
        wxsWidgetEvents* GetEvents();

        /** Function building resource tree for this widget */
        void BuildTree(wxTreeCtrl* Tree,wxTreeItemId WhereToAdd,int InsertIndex=-1);
        
        /** Deleting tree for this widget and for all it's children */
        void KillTree(wxTreeCtrl* Tree);
        
/******************************************************************************/
/* Preview                                                                    */
/******************************************************************************/
    
    public:

        /** This should create new preview object */
        wxWindow* CreatePreview(wxWindow* Parent,wxsWindowEditor* Editor);
        
        /** This should kill preview object */
        void KillPreview();
        
        /** Function returning current pereview window */
        inline wxWindow* GetPreview() { return Preview; }
        
        /** Function applying some default properties to preview
         *
         * Properties applied: Enabled, Focused, Hidden, Colours, Font and ToolTip
         */
        void PreviewApplyDefaults(wxWindow* Preview);
        
        /** Function processing mouse event passed from preview */
        virtual void PreviewMouseEvent(wxMouseEvent& event) { }
        
        /** Function ensuring that given child widget is visible
         *
         *  This function should be propagated into root widget, the easiest
         *  way is to call wxsWidget::EnsurePreviewVisible at the end of masking
         *  function.
         */
        virtual void EnsurePreviewVisible(wxsWidget* Child) { if ( GetParent() ) GetParent()->EnsurePreviewVisible(this); }
            
    protected:
    
        /** This function should create preview window for widget.
         *  It is granted that this funcntion will be called when there's no
         *  preview yet.
         */
        virtual wxWindow* MyCreatePreview(wxWindow* Parent) = 0;
        
        /** This function should delete preview object. Usually should be
         *  left as default. Put here to aviod conflicts between different
         *  memory heaps.
         */
        virtual void MyDeletePreview(wxWindow* Preview) { delete Preview; }
        
        /** This fuunction can be used to update all properties for preview
         *  after creating it's children.
         */
        virtual void MyFinalUpdatePreview(wxWindow* Preview) {}
        
/******************************************************************************/
/* Properties                                                                 */
/******************************************************************************/

    public:
    
        /** Function returning base configuration params for this widget */
        inline wxsWidgetBaseParams& GetBaseParams() { return BaseParams; }

        /** Getting properties window for this widget */
        inline wxWindow* CreatePropertiesWindow(wxWindow* Parent)
        {
            if ( !PropertiesCreated )
            {
                CreateObjectProperties();
                AddParentProperties();
                PropertiesCreated = true;
            }
            if ( !Properties ) Properties = MyCreatePropertiesWindow(Parent);
            return Properties;
        }
        
        /** getting properties which are currently used */
        inline wxWindow* GetProperties() { return Properties; }
        
        /** This should kill properties window */
        inline void KillProperties()
        {
            if ( Properties != NULL )
            {
                DeleteProperties(Properties);
                Properties = NULL;
            }
        }
        
        /** Function which should update content of current properties window */
        virtual void UpdateProperties()
        {
            if ( Updating ) return;
            Updating = true;
            MyUpdateProperties();
            Updating = false;
        }
        
        /** Function notifying that properties were changed inside properties editor
         *  \param Validate - if true, changed properties should be validated
         *  \param Correct  - if true, invalid properties should be automatically corrected
         *  \return true - properties valid, false - properties invalid (before correction)
         *          always returns true if Validate == false
         */
        bool PropertiesUpdated(bool Validate,bool Correct);

    protected:
    
        /** This function should create properties view for widget. It is granted
         *  that there are no properties created yet.
         */
        virtual wxWindow* MyCreatePropertiesWindow(wxWindow* Parent)
        { 
            return GenBaseParamsConfig(Parent);
        }
        
        /** This function should delete properties window. Usually should be
         *  left as default. Put here to aviod conflicts between different
         *  memory heaps.
         */
        virtual void DeleteProperties(wxWindow* Properties) { delete Properties; }
        
        /** Function which should update content of current properties window */
        virtual void MyUpdateProperties()
        {
            if ( GetProperties() ) PropertiesObject.UpdateProperties();
        }
        
        /** Function initializing properties for this widget.
         *  This should add all properties.
         *  Call to this function is made when properties window is created for the
         *  first time
         */
        virtual void CreateObjectProperties()
        {
            AddDefaultProperties(BPType);
        }

        /** Getting wingow's style */
        inline int GetStyle() { return BaseParams.Style; }
        
        /** Getting window's position */
        inline wxPoint GetPosition()
        {
            return BaseParams.DefaultPosition ?
                wxDefaultPosition :
                wxPoint(BaseParams.PosX,BaseParams.PosY);
        }
        
        /** Getting window's size */
        inline wxSize GetSize()
        {
            return BaseParams.DefaultSize ?
                wxDefaultSize :
                wxSize(BaseParams.SizeX,BaseParams.SizeY);
        }
        
    private:
        
        inline void AddParentProperties()
        {
        	if ( GetParent() )
        	{
        		GetParent()->AddChildProperties(GetParent()->FindChild(this));
        	}
        }

/******************************************************************************/
/* Code generation                                                            */
/******************************************************************************/

    public:
    
        /** Function generating code which should produce widget
         *  It's called BEFORE widget's children are created and
         *  must set up BaseParams.VarName variable inside code.
         */
        virtual wxString GetProducingCode(wxsCodeParams& Params) { return _T(""); }
            
        /** Function generating code which finishes production process of this
         *  widget, it will be called AFTER child widgets are created
         *
         * It can be used f.ex. by sizers to bind to parent item.
         * UniqueNumber is same as in GetProducingCode
         */
        virtual wxString GetFinalizingCode(wxsCodeParams& Params) { return _T(""); }
        
        /** Function generating code which generates variable containing this 
         *  widget. If there's no variable (f.ex. space inside sizers), it should
         *  return empty string
         */
        virtual wxString GetDeclarationCode(wxsCodeParams& Params) { return _T(""); }

        /** Structure deeclaring some code-defines which could be usefull while
         *  creating widget's code
         */
        struct CodeDefines
        {
            wxString Style;         ///< Widget's style in form 'wxSTYLE1|wxSTYLE2'
            wxString Pos;           ///< Widget's position
            wxString Size;          ///< Widget's size
            wxString InitCode;      ///< Code initializing Enabled / Focused / Hidden flags, Colours, ToolTip and Font
        };
        
        /** Function creating current coded defines */
        virtual const CodeDefines& GetCodeDefines();
        
/**********************************************************************/
/* Support for containers                                             */
/**********************************************************************/
        
        /** Checking if this widget is an container */
        inline bool IsContainer() { return ContainerType != NoContainer; }
        
        /** Getting max number of children */
        inline int GetMaxChildren() { return IsContainer() ? MaxChildren : 0; }
        
        /** Checking if this container is a window (children are connected
         *  directly to it)
         */
        inline bool IsContWindow() { return ContainerType == ContainerWindow; }
        
        /** Getting number of internal widgets */
        virtual int GetChildCount() { return 0; }
        
        /** Getting pointer to given child */
        virtual wxsWidget* GetChild(int Id) { return NULL; }
        
        /** Binding child
         *
         * \param InsertAfterThis - position where to add new widget, if -1,
         *        adding at the end
         */
        virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis = -1) { return -1; }
        
        /** Unbinding child
         *
         * This structure must inbing child window from this one, must NOT delete
         * child
         */
        virtual bool DelChildId(int Id) { return false; }

        /** Unbinding child
         *
         * This structure must inbing child window from this one, must NOT delete
         * child
         */
        virtual bool DelChild(wxsWidget* Widget) { return false; }
        
        /** Searching for specified widget 
         * \param Widget - widget we're looking for
         * \param Level - max depth level (0 - unlimited, 1 - direct children only, >1 - children and subchildren)
         * \return >= 0 if found (when Level == 1, this value is Id of widget, 0<=Id<ChildCount, otherwise it's 0),
         *           -1 if there's no such child
         */
        virtual int FindChild(wxsWidget* Widget,int Level = 1) { return -1; }
        
        /** Changing position of widget in child list */
        virtual bool ChangeChildPos(int PrevPos, int NewPos) { return false; }
        
    protected:
    
        /** Adding additional properties to child object */
        virtual void AddChildProperties(int ChildIndex) { }
        
        /** Loading child object from xml node
         *
         *  Note that node passed to this function may contain different
         *  data. In such case this funcntino should return and only real
         *  children should be processed.
         *  Default implementation process <object> Xml elements.
         */
        virtual bool XmlLoadChild(TiXmlElement* Element);
        
        /** Saving child to Xml node
         *
         * Default implementation save <object> Xml element
         */
        virtual bool XmlSaveChild(int ChildIndex,TiXmlElement* AddHere);
        
/**********************************************************************/
/* Support for base widget's parameters                               */
/**********************************************************************/
        
   protected:
   
        /** Function creating wxPanel object which contains panel with
         * configuration of base widget's properties
         */
        inline wxWindow* GenBaseParamsConfig(wxWindow* Parent)
        {
            return PropertiesObject.GenerateWindow(Parent);
        }
        
        /** This function updates content of given base properties panel,
         *  should be called when any operation on preview changes any of 
         *  base properties.
         */
        inline void UpdateBaseParamsConfig()
        {
            PropertiesObject.UpdateProperties();
        }
        
        /** Base parameters describing this widget */
        wxsWidgetBaseParams BaseParams;
        
        /** Base object used to handle properties */
        wxsProperties PropertiesObject;
        
    
/******************************************************************************/
/* XML Operations                                                             */
/******************************************************************************/
     
    public:
        /** Loading widget from xml source */
        inline bool XmlLoad(TiXmlElement* Element)
        {
            XmlAssignElement(Element);
            bool Result = MyXmlLoad();
            if ( !XmlLoadDefaults() ) Result = false;
            XmlAssignElement(NULL);
            return Result;
        }
        
        inline bool XmlSave(TiXmlElement* Element)
        {
            XmlAssignElement(Element);
            bool Result = MyXmlSave();
            if ( !XmlSaveDefaults() ) Result = false;
            XmlAssignElement(NULL);
            return Result;
        }
        
    protected:

        /** Internal function loading data from xml tree
         *
         * This functino can be overriden inside derivedd classes
         * to allow loading additional data. This data should be loadedd
         * from node returned by XmlElem function (all XmlGet... functions
         * are also using this one). See bptxxx and propxxx to find out which
         * settings are loaded automatically.
         */
        virtual bool MyXmlLoad() { return true; }
        
        /** Internal function saving xml tree.
         *
         * This function can be overriden inside derived classes
         * to allow saving additional data. This data should be saved
         * from node returned by XmlElem function (all XmlSet... functions
         * are also using this one). See bptxxx and propxxx to find out which
         * settings are saved automatically.
         */
        virtual bool MyXmlSave() { return true; }

        /** Getting currenlty associated xml element */
        inline TiXmlElement* XmlElem() { return XmlElement; }
        
/**********************************************************************/
/* Helpful functions which could be used while operating on resources */
/**********************************************************************/
        
        /** Getting value from given name */
        virtual wxString XmlGetVariable(const wxString& Name);
        
        /** Getting integer from given name
         *
         *  \param Name - name of xml tag
         *  \param IsInvalid - redeference to boolean variable which will get
         *                     information about read success
         *  \param DefaultValue - value which will be returned in case of any error
         *                        (IsInvalid=true)
         *  \returns value of variable
         */
        virtual int XmlGetInteger(const wxString &Name,bool& IsInvalid,int DefaultValue=0);
        
        /** Getting integer from given name without returning error */
        inline int XmlGetInteger(const wxString &Name,int DefaultValue=0)
        {
            bool Temp;
            return XmlGetInteger(Name,Temp,DefaultValue);
        }
        
        /** Getting size/position from given name */
        virtual bool XmlGetIntPair(const wxString& Name,int& P1,int& P2,int DefP1=-1,int DefP2=-1);
        
        // Added by cyberkoa
        /** Getting a series of string with given parent element and child element name */
        virtual bool wxsWidget::XmlGetStringArray(const wxString &ParentName,const wxString& ChildName, wxArrayString& stringArray);
        //End Add
        
        /** Setting string value */
        virtual bool XmlSetVariable(const wxString &Name,const wxString& Value);
        
        /** Setting integer value */
        virtual bool XmlSetInteger(const wxString &Name,int Value);
        
        /** Setting 2 integers */
        virtual bool XmlSetIntPair(const wxString &Name,int Val1,int Val2);
        
        /** Function assigning element which will be used while processing
         *  xml resources. Usually this function is calleed automatically
         *  from outside so there's no need to care about currently
         *  associateed Xml element. This functino should be called when
         *  there's need to parse external xml elements using functions
         *  inside wxsWidget class.
         */
        void XmlAssignElement(TiXmlElement* Element);
        
        // Added by cyberkoa
        /** Set a series of string with the same given element name */
        virtual bool wxsWidget::XmlSetStringArray(const wxString &ParentName,const wxString& ChildName, wxArrayString& stringArray);
        //End Add
        
        /** Reading all default values for widget */
        inline bool XmlLoadDefaults() { return XmlLoadDefaultsT(BPType); }
        
        /** Reading default values for widget using given set of base properties */
        virtual bool XmlLoadDefaultsT(BasePropertiesType pType);
        
        /** Saving all default values for widget */
        inline bool XmlSaveDefaults() { return XmlSaveDefaultsT(BPType); }
        
        /** Saving default values for widget using given set of base properties */
        virtual bool XmlSaveDefaultsT(BasePropertiesType pType);
        
        /** Loading all children
         *
         *  Valid for compound objects only
         */
        virtual bool XmlLoadChildren();
        
        /** Saving all children
         *
         * Valid for compouund objects only, if current widget is a sizer,
         * additional "sizeritem" object will be created
         */
        virtual bool XmlSaveChildren();
        
    private:
    
        /** Adding default properties to properties manager */
        virtual void AddDefaultProperties(BasePropertiesType Props);
        
        /** Invalidating tree item id for this widget and all of it's children */
        void InvalidateTreeIds();
        
        wxsWidgetManager* Manager;  ///< Widget's manager
        wxWindow* Preview;          ///< Currently opened preview window (NULL if there's no one)
        wxsWindowRes* Resource;     ///< Resource owning this widget
        wxWindow* Properties;       ///< Currently opened properties window (NULL if there's no one)
        wxsWidget* Parent;          ///< Parent widget of this one
        int MaxChildren;            ///< Num of max. Childs, -1 if no limit, valid for containers only
        
        TiXmlElement* XmlElement;   ///< Currently selected xml element

        enum CType { NoContainer, ContainerSizer, ContainerWindow };

        CType ContainerType;        ///< Type of container (or mark that it's no container)
        bool Updating;              ///< Set to true while any update is made (to avoid infinite loops and data loos)
        
        bool PropertiesCreated;
        
        BasePropertiesType BPType;  ///< Set of base properties used inside constructor
        
        CodeDefines CDefines;       ///< Will be filled and returned inside GetCodedeDefines
        
        wxTreeItemId TreeId;        ///< Id of item in resource tree
        bool AssignedToTree;        ///< True if this widget has it's entry inside resource tree
        
        wxsWidgetEvents* Events;    ///< Events used for this widget
        
        friend class wxBaseParamsPanel;
        friend class wxsWindowEditor;
        friend class wxsContainer;
        friend class wxsProject;
        friend class wxsPalette;
        friend class wxsWidgetFactory;
};

/** Class managing widget */
class wxsWidgetManager
{
    public:
        wxsWidgetManager() {}
        virtual ~wxsWidgetManager() {}
        
        /** Funcntion initializing manager */
        virtual bool Initialize() { return true; }
        
        /** Returns number of handled widgets */
        virtual int GetCount() = 0;
        
        /** Getting widget's info */
        virtual const wxsWidgetInfo* GetWidgetInfo(int Number) = 0;
        
        /** Getting new widget */
        virtual wxsWidget* ProduceWidget(int Id,wxsWindowRes* Res) = 0;
        
        /** Killing widget */
        virtual void KillWidget(wxsWidget* Widget) = 0;
        
        /** Fuunction registering this manager into main widget's factory */
        virtual bool RegisterInFactory();
};

#endif
