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
                Extra.push_back(NewExtra());
                return Widgets.size() - 1;
            }
            Widgets.insert(Widgets.begin() + InsertBeforeThis,NewWidget);
            Extra.insert(Extra.begin() + InsertBeforeThis,NewExtra());
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
            DelExtra(Extra[Id]);
            Extra.erase(Extra.begin()+Id);
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
            WidgetsI i = Widgets.begin();
            ExtraI ie = Extra.begin();
            for ( ; i!=Widgets.end(); ++i, ++ie )
                if ( (*i)==Widget ) 
                {
                    Widgets.erase(i);
                    DelExtra(*ie);
                    Extra.erase(ie);
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
            void* eChanging = Extra[PrevPos];
            
            if ( PrevPos < NewPos )
            {
                while ( PrevPos++ < NewPos )
                {
                    Widgets[PrevPos-1] = Widgets[PrevPos];
                    Extra[PrevPos-1] = Extra[PrevPos];
                }
            }
            else
            {
                while ( PrevPos-- > NewPos )
                {
                    Widgets[PrevPos+1] = Widgets[PrevPos];
                    Extra[PrevPos+1] = Extra[PrevPos];
                }
            }
            Widgets[NewPos] = Changing;
            Extra[NewPos] = eChanging;
            return true;
        }
        
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
