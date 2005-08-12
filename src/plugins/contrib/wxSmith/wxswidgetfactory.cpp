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

const wxsWidgetInfo * wxsWidgetFactory::GetInfo(const wxString& Name)
{
    WidgetsMapI i = Widgets.find(&Name);
    if ( i == Widgets.end() ) return NULL;
    return (*i).second;
}

wxsWidget * wxsWidgetFactory::Generate(const wxString& Name,wxsWindowRes* Res)
{
    WidgetsMapI i = Widgets.find(&Name);
    if ( i == Widgets.end() ) return NULL;
    const wxsWidgetInfo* Info = (*i).second;
    return Info->Manager->ProduceWidget(Info->Id,Res);
}

void wxsWidgetFactory::Kill(wxsWidget* Widget)
{
    if ( Widget )
    {
        // Closing properties if are set to given widget
        wxsUnselectWidget(Widget);
        
        // First unbinding it from parent
        wxsWidget* Parent = Widget->GetParent();
        if ( Parent )
        {
            Parent->DelChild(Widget);
        }
        
        // Deleting resource tree entrry
        wxTreeCtrl* Tree = wxSmith::Get()->GetResourceTree();
        if ( Tree && Widget->AssignedToTree )
        {
            Tree->Delete(Widget->TreeId);
        }

        if ( Widget->GetPreview() ) Widget->KillPreview();
        if ( Widget->GetProperties() ) Widget->KillProperties();
        
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
            Widgets[&Info->Name] = Info;
        }
    }
}

/** \brief set of names which can not be used as widget names
 *
 * This names must be placed in alphabetical order
 */
static const wxChar* DeadNames[] = 
{
    _T("asm"), _T("auto"), _T("bool"), _T("break"), _T("case"), _T("catch"), 
    _T("char"), _T("class"), _T("const"), _T("const_cast"), _T("continue"), 
    _T("default"), _T("delete"), _T("do"), _T("double"), _T("dynamic_cast"), 
    _T("else"), _T("enum"), _T("explicit"), _T("export"), _T("extern"), 
    _T("false"), _T("float"), _T("for"), _T("friend"), _T("goto"), _T("if"),
    _T("inline"), _T("int"), _T("long"), _T("mutable"), _T("namespace"), 
    _T("new"), _T("operator"), _T("private"), _T("protected"), _T("public"), 
    _T("register"), _T("reinterpret_cast"), _T("return"), _T("short"), 
    _T("signed"), _T("sizeof"), _T("sizeritem"), _T("static"), 
    _T("static_cast"), _T("struct"), _T("switch"), _T("template"), _T("this"), 
    _T("throw"), _T("true"), _T("try"), _T("typedef"), _T("typeid"), 
    _T("typename"), _T("union"), _T("unsigned"), _T("using"), _T("virtual"), 
    _T("void"), _T("volatile"), _T("wchar_t"), _T("while")
};

static const int DeadNamesLen = sizeof(DeadNames) / sizeof(DeadNames[0]);

bool wxsWidgetFactory::ValidateName(const wxString& NameStr)
{
    const wxChar* Name = NameStr.c_str();
    if ( !Name ) return false;
    
    if (( *Name < _T('a') || *Name > _T('z') ) &&
        ( *Name < _T('A') || *Name > _T('Z') ) &&
        ( *Name != _T('_') ))
    {
        return false;
    }
    
    while ( *++Name )
    {
        if (( *Name < _T('a') || *Name > _T('z') ) &&
            ( *Name < _T('A') || *Name > _T('Z') ) &&
            ( *Name < _T('0') || *Name > _T('9') ) &&
            ( *Name != _T('_') ))
        {
            return false;
        }
    }
   
    int Begin = 0;
    int End = DeadNamesLen-1;
    
    Name = NameStr.c_str();
    
    while ( Begin <= End )
    {
        int Middle = ( Begin + End ) >> 1;
        
        int Res = wxStrcmp(DeadNames[Middle],Name);
        
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
