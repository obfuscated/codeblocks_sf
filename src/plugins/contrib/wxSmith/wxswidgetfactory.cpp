#include "wxswidgetfactory.h"

#include "wxsmith.h"
#include "wxspropertiesman.h"

wxsWidgetFactory* wxsWidgetFactory::Singleton = new wxsWidgetFactory;

wxsWidgetFactory::wxsWidgetFactory()
{
}

wxsWidgetFactory::~wxsWidgetFactory()
{
}

const wxsWidgetInfo * wxsWidgetFactory::GetInfo(const char* Name)
{
    WidgetsMapI i = Widgets.find(Name);
    if ( i == Widgets.end() ) return NULL;
    return (*i).second;
}

wxsWidget * wxsWidgetFactory::Generate(const char* Name)
{
    WidgetsMapI i = Widgets.find(Name);
    if ( i == Widgets.end() ) return NULL;
    const wxsWidgetInfo* Info = (*i).second;
    return Info->Manager->ProduceWidget(Info->Id);
}

void wxsWidgetFactory::Kill(wxsWidget* Widget)
{
    if ( Widget )
    {
        // First unbinding it from parent
        wxsWidget* Parent = Widget->GetParent();
        if ( Parent )
        {
            Parent->DelChild(Widget);
        }
        
        // Deleting resource tree entrry
        wxTreeCtrl* Tree = wxSmith::Get()->GetResourceTree();
        if ( Tree )
        {
            Tree->Delete(Widget->TreeId);
        }
        
        // Closing properties if are set to given widget
        if ( wxsPropertiesMan::Get()->GetActiveWidget() == Widget )
        {
            wxsPropertiesMan::Get()->SetActiveWidget(NULL);
        }
        
        // Deleting widget
        if ( Widget->GetInfo().Manager )
        {
            Widget->GetInfo().Manager->KillWidget(Widget);
        }
        else
        {
            // Possibly unsafe
            delete Widget;
        }
        
        // Need to rearrange parent
        if ( Parent )
        {
            Parent->UpdatePreview(true,true);
        }
    }
}

const wxsWidgetInfo * wxsWidgetFactory::GetFirstInfo()
{
    Iterator = Widgets.begin();
    return Iterator == Widgets.end() ? NULL : (*Iterator).second;
}

const wxsWidgetInfo* wxsWidgetFactory::GetNextInfo()
{
    if ( Iterator == Widgets.end() ) return NULL;
    if ( ++Iterator == Widgets.end() ) return NULL;
    return (*Iterator).second;
}

void wxsWidgetFactory::RegisterManager(wxsWidgetManager* Manager)
{
    if ( !Manager ) return;
    
    int Count = Manager->GetCount();
    for ( int i = 0; i<Count; i++ )
    {
        const wxsWidgetInfo* Info = Manager->GetWidgetInfo(i);
        if ( Info && ValidateName(Info->Name) )
        {
            Widgets[Info->Name] = Info;
        }
    }
}

/** \brief set of names which can not be used as widget names
 *
 * This names must be placed in alphabetical order
 */
static const char* DeadNames[] = 
{
    "asm", "auto", "bool", "break", "case", "catch", "char", "class",
    "const", "const_cast", "continue", "default", "delete", "do",
    "double", "dynamic_cast", "else", "enum", "explicit", "export",
    "extern", "false", "float", "for", "friend", "goto", "if", "inline",
    "int", "long", "mutable", "namespace", "new", "operator", "private",
    "protected", "public", "register", "reinterpret_cast", "return",
    "short", "signed", "sizeof", "sizeritem", "static", "static_cast",
    "struct", "switch", "template", "this", "throw", "true", "try",
    "typedef", "typeid", "typename", "union", "unsigned",
    "using", "virtual", "void", "volatile", "wchar_t", "while"
};

static const int DeadNamesLen = sizeof(DeadNames) / sizeof(DeadNames[0]);

bool wxsWidgetFactory::ValidateName(const char* Name)
{
    if ( !Name ) return false;
    
    if (( *Name < 'a' || *Name > 'z' ) &&
        ( *Name < 'A' || *Name > 'Z' ) &&
        ( *Name != '_' ))
    {
        return false;
    }
    
    const char* NameStore = Name;
    
    while ( *++Name )
    {
        if (( *Name < 'a' || *Name > 'z' ) &&
            ( *Name < 'A' || *Name > 'Z' ) &&
            ( *Name < '0' || *Name > '9' ) &&
            ( *Name != '_' ))
        {
            return false;
        }
    }
   
    int Begin = 0;
    int End = DeadNamesLen-1;
    
    Name = NameStore;
    
    while ( Begin <= End )
    {
        int Middle = ( Begin + End ) >> 1;
        
        int Res = strcmp(DeadNames[Middle],Name);
        
        if ( Res < 0 )
        {
            Begin = Middle+1;
        }
        else if ( Res > 0 )
        {
            End = Middle-1;
        }
        else 
        {
            return false;
        }
        
    }
    
    return true;
}
