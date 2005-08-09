#ifndef WXSCONTAINER_H
#define WXSCONTAINER_H

#include "widget.h"
#include <vector>

class wxsContainer : public wxsWidget
{
	public:
		wxsContainer(wxsWidgetManager* Manager,wxsWindowRes* Res,bool IsWindow = false,int MaxChildren = -1,BasePropertiesType pType=propNone):
             wxsWidget(Manager,Res,IsWindow,MaxChildren,pType),
             DeletingAll(false)
        {};
        
		virtual ~wxsContainer();
		
        /** Getting number of internal widgets */
        virtual int GetChildCount() { return (int)Widgets.size(); }
        
        /** Getting pointer to given child */
        virtual wxsWidget* GetChild(int Id) { return (Id>=0 && Id<(int)Widgets.size()) ? Widgets[Id] : NULL; }
        
        /** Binding child
         *
         * \param InsertAfterThis - position where to add new widget, if -1,
         *        adding at the end
         */
        virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis = -1)
        {
            if ( NewWidget == NULL ) return -1;
            NewWidget->Parent = this;
            if ( InsertBeforeThis < 0 || InsertBeforeThis >= (int)Widgets.size() )
            {
                Widgets.push_back(NewWidget);
                return Widgets.size() - 1;
            }
            Widgets.insert(Widgets.begin() + InsertBeforeThis,NewWidget);
            return InsertBeforeThis;
        }
        
        /** Unbinding child
         *
         * This structure must inbing child window from this one, must NOT delete
         * child
         */
        virtual bool DelChildId(int Id)
        {
            if ( DeletingAll ) return false;
            if ( Id<0 || Id>=(int)Widgets.size() ) return false;
            Widgets.erase(Widgets.begin()+Id);
            return true;
        }

        /** Unbinding child
         *
         * This structure must inbing child window from this one, must NOT delete
         * child
         */
        virtual bool DelChild(wxsWidget* Widget)
        {
            if ( DeletingAll ) return false;
            for ( WidgetsI i = Widgets.begin(); i!=Widgets.end(); ++i )
                if ( (*i)==Widget ) 
                {
                    Widgets.erase(i);
                    return true;
                }
            return false;
        }
        
        /** Searching for specified widget 
         * \returns >= 0 if found (this value is Id of widget, 0<=Id<ChildCount),
         *          -1 if there's no such child
         */
        virtual int FindChild(wxsWidget* Widget)
        {
            int Cnt = 0;
            for ( WidgetsI i = Widgets.begin(); i!=Widgets.end(); ++i, ++Cnt )
                if ( (*i)==Widget ) 
                    return Cnt;
            return -1;
        }
         
        /** Changing position of child window
         *
         */
        virtual bool ChangeChildPos(int PrevPos,int NewPos)
        {
            if ( PrevPos<0 || PrevPos >= (int)Widgets.size() ) return false;
            if ( NewPos<0 || NewPos>=(int)Widgets.size() ) return false;
            if ( PrevPos == NewPos ) return true;
            
            wxsWidget* Changing = Widgets[PrevPos];
            
            if ( PrevPos < NewPos )
            {
                while ( PrevPos++ < NewPos )
                    Widgets[PrevPos-1] = Widgets[PrevPos];
            }
            else
            {
                while ( PrevPos-- > NewPos )
                    Widgets[PrevPos+1] = Widgets[PrevPos];
            }
            Widgets[NewPos] = Changing;
            return true;
        }
        
	private:
        typedef std::vector<wxsWidget*> WidgetsT;
        typedef WidgetsT::iterator WidgetsI;
        
        WidgetsT Widgets;
        bool DeletingAll;
};

#endif // WXSCONTAINER_H
