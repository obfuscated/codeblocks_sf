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
#include "wxsbaseproperties.h"
#include "wxswidgetmanager.h"
#include "wxscodeparams.h"

class wxsWidgetManager;
class wxsWidget;
class wxsWidgetEvents;
class wxsEventDesc;

/** Class for quick properties */
class wxsQPPPanel: public wxPanel
{
    public:

        wxsQPPPanel(wxsWidget* _Owner);
        virtual ~wxsQPPPanel();

        static void NotifyWidgetDelete(wxsWidget* Widget);

    protected:

        bool Valid() { return Owner!=NULL; }

    private:
        wxsWidget* Owner;
};

/** Class representing one widget */
class wxsWidget
{
    public:

        /** Default constructor */
        wxsWidget(wxsWidgetManager* Man,wxsWindowRes* Res,wxsBasePropertiesType pType = propNone);

        /** Constructor used by containers
         *
         *  \param Man - widgets manager
         *  \param ISwxWindow - true if this container is an wxWindow object,
         *                      false otherwise (usually for wxSizer objects
         *  \param MaxChildren - maximal number of children which can be handled by this container
         */
        wxsWidget(wxsWidgetManager* Man, wxsWindowRes* Res, bool ISwxWindow, int MaxChild,wxsBasePropertiesType pType = propNone);

        /** Destructor */
        virtual ~wxsWidget();

        /** Getting widget's info */
        virtual const wxsWidgetInfo& GetInfo() = 0;

        /** Getting manager of this widget */
        inline wxsWidgetManager* GetManager() { return Manager; }

        /** Getting parent item of this one */
        inline wxsWidget* GetParent() { return Parent; }

        /** Getting resource tree item id of this widget in resource browser */
        inline wxTreeItemId GetTreeId() { return TreeId; }

        /** Getting resource owning this widget */
        inline wxsWindowRes* GetResource() { return Resource; }

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

        /** This function can be used to update all properties for preview
         *  after creating it's children.
         */
        virtual void MyFinalUpdatePreview(wxWindow* Preview) {}

    private:

        /** Function used to clear preview if it was deleted from parent level */
        void PreviewDestroyed();

/******************************************************************************/
/* Properties                                                                 */
/******************************************************************************/

    public:

        /** Getting current wxsBasePropertiesType for this widget */
        inline wxsBasePropertiesType GetBPType() { return BPType; }

        /** Creating properties window for this widget */
        wxWindow* CreatePropertiesWindow(wxWindow* Parent);

        /** This function returns current properties window */
        inline wxWindow* GetPropertiesWindow() { return PropertiesWindow; }

        /** This function destroys properties window */
        void KillPropertiesWindow();

        /** Function which should update content of current properties window */
        void UpdatePropertiesWindow();

        /** Function notifying that properties were changed inside properties editor
         *  \param Validate - if true, changed properties should be validated
         *  \param Correct  - if true, invalid properties should be automatically corrected
         *  \return true - properties valid, false - properties invalid (before correction)
         *          always returns true if Validate == false
         */
        bool PropertiesChanged(bool Validate,bool Correct);

        /** Function building quick properties panel inside editor area
         *
         * If this function returns value != NULL, window returned will be
         * added to parent's sizer automatically. Parent window can handle
         * more elements added. In such case, this function should fetch
         * parent's sizer, add all items into this sizer and return NULL.
         * Default implementation returns panel from base properties.
         */
        virtual wxWindow* BuildQuickPanel(wxWindow* Parent);

        /** Getting window's style
         *
         * This fuunction is supplied here because styles stored
         * in base properties object are not directly mapped into
         * properties used in wxWidgets.
         * \return Style usable in wxWidgets
         */
        long GetStyle();

        /** Setting window's style
         *
         * This function sets specified style in base proeprties object.
         * \param Style wxWidgets style
         */
        void SetStyle(long Style);

        /** Getting window's extra style
         *
         * \see GetStyle
         */
        long GetExStyle();

        /** Setting window's extra style
         *
         * \see SetStyle
         */
        void SetExStyle(long ExStyle);

        /** Getting window's position
         *
         * This function returns wxPoint class with current position
         * (may be wxDefaultPosition). It may be used directly when
         * creating previews.
         */
        inline wxPoint GetPosition()
        {
            return BaseProperties.DefaultPosition ?
                wxDefaultPosition :
                wxPoint(BaseProperties.PosX,BaseProperties.PosY);
        }

        /** Getting window's size
         *
         * This functino returns wxSize class with current size
         * (may be wxDefaultSize). It may be used directly when
         * creating previews.
         */
        inline wxSize GetSize()
        {
            return BaseProperties.DefaultSize ?
                wxDefaultSize :
                wxSize(BaseProperties.SizeX,BaseProperties.SizeY);
        }

    protected:

        /** This function should create properties editor for this widget
         *
         * It is granted that there are no properties created yet.
         * Default implementation creates wxPropertyGrid widget
         * handling all registered properties.
         */
        virtual wxWindow* MyCreatePropertiesWindow(wxWindow* Parent);

        /** Function which should update content of current properties editor window.
         *
         * At the time of call, it's granted that property window exists.
         * Default implementation updates previously created property grid
         */
        virtual void MyUpdatePropertiesWindow();

        /** Function notifying about property change
         *
         * This functino is called when any property of this
         * widget has been changed in property editor.
         * Meaning of arguments and return value is same as in case
         * of PropertyChanged function.
         */
        virtual bool MyPropertiesChanged(bool Validate,bool Correct);

        /** Function initializing properties for this widget.
         *
         *  This function should add all properties for this widget.
         *  Call to this function is made when properties window is
         *  created for the first time. If this fucntion is overwritten
         *  in derived classes, it should call the original one after
         *  creating it's private properties to include all default ones.
         */
        virtual void MyCreateProperties();

        /** Changing base properties used in given edit mode
         *
         * This function should be called once for each edit mode
         * when widget has different set of base properties in different
         * modes.
         */
        void ChangeBPT(int ResourceEditMode,wxsBasePropertiesType bpType);

    public:

        /** Base parameters describing this widget */
        wxsBaseProperties BaseProperties;

        /** Base object used to handle properties */
        wxsProperties Properties;

/******************************************************************************/
/* Code generation                                                            */
/******************************************************************************/

    public:

        /** Function generating code which should produce widget
         *  It's called BEFORE widget's children are created and
         *  must set up BaseProperties.VarName variable inside code.
         */
        virtual wxString GetProducingCode(const wxsCodeParams& Params) = 0;

        /** Function generating code which finishes production process of this
         *  widget, it will be called AFTER child widgets are created
         *
         * It can be used f.ex. by sizers to bind to parent item.
         * UniqueNumber is same as in GetProducingCode
         */
        virtual wxString GetFinalizingCode(const wxsCodeParams& Params);

        /** Function generating code which generates variable containing this
         *  widget. If there's no variable (f.ex. space inside sizers), it should
         *  return empty string
         */
        virtual wxString GetDeclarationCode(const wxsCodeParams& Params);

        /** Function building wxsCodeParams object for this widget.
         *
         *  UniqueNumber, IsDirectParent and ParentName are not filled.
         */
        void BuildCodeParams(wxsCodeParams& Params);

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

        /** Function checking if given widget can be added as child,
         *
         * \param InsertBeforeThis - proposed position, shouldn't be
         *        considered as certain thing
         */
        virtual bool CanAddChild(wxsWidget* NewWidget,int InsertBeforethis = -1) { return false; }

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

        /** Function building quick configure panel for child at given position.
         *
         * This funtion should build wxPanel window with widgets for quick
         * configuration. It will be placed inside quick configure area in editor.
         */
        virtual wxWindow* BuildChildQuickPanel(wxWindow* Parent,int ChildPos) { return NULL; }

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
        virtual bool XmlGetStringArray(const wxString &ParentName,const wxString& ChildName, wxArrayString& stringArray);
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

        /** Set a series of string with the same given element name */
        virtual bool XmlSetStringArray(const wxString &ParentName,const wxString& ChildName, wxArrayString& stringArray);

        /** Reading all default values for widget */
        inline bool XmlLoadDefaults() { return XmlLoadDefaultsT(GetBPType()); }

        /** Reading default values for widget using given set of base properties */
        virtual bool XmlLoadDefaultsT(wxsBasePropertiesType pType);

        /** Saving all default values for widget */
        inline bool XmlSaveDefaults() { return XmlSaveDefaultsT(GetBPType()); }

        /** Saving default values for widget using given set of base properties */
        virtual bool XmlSaveDefaultsT(wxsBasePropertiesType pType);

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
        virtual void AddDefaultProperties(wxsBasePropertiesType Props);

        /** Invalidating tree item id for this widget and all of it's children */
        void InvalidateTreeIds();

        wxsWidgetManager* Manager;  ///< Widget's manager
        wxWindow* Preview;          ///< Currently opened preview window (NULL if there's no one)
        wxsWindowRes* Resource;     ///< Resource owning this widget
        wxWindow* PropertiesWindow; ///< Currently opened properties window (NULL if there's no one)
        wxsWidget* Parent;          ///< Parent widget of this one
        int MaxChildren;            ///< Num of max. Childs, -1 if no limit, valid for containers only

        TiXmlElement* XmlElement;   ///< Currently selected xml element

        enum CType { NoContainer, ContainerSizer, ContainerWindow };

        CType ContainerType;        ///< Type of container (or mark that it's no container)
        bool Updating;              ///< Set to true while any update is made (to avoid infinite loops and data loos)

        bool PropertiesCreated;

        wxsBasePropertiesType BPType;   ///< Flags for used base properties

        wxTreeItemId TreeId;        ///< Id of item in resource tree
        bool AssignedToTree;        ///< True if this widget has it's entry inside resource tree

        wxsWidgetEvents* Events;    ///< Events used for this widget

        friend class wxsContainer;
};

#endif
