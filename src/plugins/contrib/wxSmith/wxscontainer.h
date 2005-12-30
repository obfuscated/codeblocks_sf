#ifndef WXSCONTAINER_H
#define WXSCONTAINER_H

#include "widget.h"
#include <vector>

class wxsContainer : public wxsWidget
{
	public:
		wxsContainer(wxsWidgetManager* Manager,wxsWindowRes* Res,bool IsWindow = false,int MaxChildren = -1,wxsBasePropertiesType pType=propNone):
             wxsWidget(Manager,Res,IsWindow,MaxChildren,pType),
             DeletingAll(false)
        {};

		virtual ~wxsContainer();

        /** Getting number of internal widgets */
        virtual int GetChildCount() { return (int)Widgets.size(); }

        /** Getting pointer to given child */
        virtual wxsWidget* GetChild(int Id) { return (Id>=0 && Id<(int)Widgets.size()) ? Widgets[Id] : NULL; }

        /** Checking if can add child
         *  by default all children are allowed
         */
        virtual bool CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis = -1 ) { return true; }

        /** Binding child
         *
         * \param InsertAfterThis - position where to add new widget, if -1,
         *        adding at the end
         */
        virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis = -1);

        /** Unbinding child
         *
         * This structure must inbing child window from this one, must NOT delete
         * child
         */
        virtual bool DelChildId(int Id);

        /** Unbinding child
         *
         * This structure must inbing child window from this one, must NOT delete
         * child
         */
        virtual bool DelChild(wxsWidget* Widget);

        /** Searching for specified widget
         * \param Widget - widget we're looking for
         * \param Level - max depth level (0 - unlimited, 1 - direct children only, >1 - children and subchildren)
         * \return >= 0 if found (when Level == 1, this value is Id of widget, 0<=Id<ChildCount, otherwise it's 0),
         *           -1 if there's no such child
         */
        virtual int FindChild(wxsWidget* Widget,int Level=1);

        /** Changing position of child window
         *
         */
        virtual bool ChangeChildPos(int PrevPos,int NewPos);

    protected:

        /** Function returning extra data at given position */
        inline void* GetExtra(int Pos) { return ( (Pos >= 0) && (Pos < (int)Extra.size()) ) ? Extra[Pos] : NULL; }

        /** Function which should be overridden in derived classes - returns
         *  pointer to new extra data
         */
        virtual void* NewExtra() { return NULL; }

        /** Fuunction which should be overridden in dedrived classes - destroys
         *  previously created extra data
         */
        virtual void DelExtra(void*) { }

	private:
        typedef std::vector<wxsWidget*> WidgetsT;
        typedef WidgetsT::iterator WidgetsI;
        typedef std::vector<void*> ExtraT;
        typedef ExtraT::iterator ExtraI;

        WidgetsT Widgets;
        ExtraT Extra;
        bool DeletingAll;
};

#endif // WXSCONTAINER_H
