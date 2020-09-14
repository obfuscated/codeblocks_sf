#ifndef CB_TODO_PLUGIN_PARSER_H
#define CB_TODO_PLUGIN_PARSER_H

#include <map>
#include <vector>
#include <wx/dynarray.h>
#include <wx/string.h>

class wxArrayString;

// an item is one record in the file, such as a fixme, it can have several properties, such as
// the type (todo, note, fixme..), the user (who wrote the item) and the date, all its properties
// are wrappered in the ToDoItem struct
struct ToDoItem
{
    wxString type;
    wxString text;
    wxString user;
    wxString filename;
    wxString lineStr;
    wxString priorityStr;
    wxString date;
    int line;
    int priority;
};
// each source file can have several ToDoItems, so we use a Map structure to record all the items
typedef std::map<wxString,std::vector<ToDoItem> > TodoItemsMap;
WX_DECLARE_OBJARRAY(ToDoItem, ToDoItems);

/// Parses the buffer and fills itemsMap and items with the results.
void ParseBufferForTODOs(TodoItemsMap &itemsMap, ToDoItems &items,
                         const wxArrayString &startStrings,
                         const wxArrayString &allowedTypes, const wxString& buffer,
                         const wxString& filename);


#endif // CB_TODO_PLUGIN_PARSER_H
