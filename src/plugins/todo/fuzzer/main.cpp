#include "../todo_parser.h"

#include <stdio.h>
#include <wx/app.h>
#include <wx/arrstr.h>
#include <wx/file.h>

int main(int argc, char *argv[])
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
        return -1;
    }

    wxFile file(argv[1]);

    wxString buffer;
    if (!file.ReadAll(&buffer))
        return 1;

    TodoItemsMap outItemsMap;
    ToDoItems outItems;

    wxArrayString allowedTypes;
    allowedTypes.push_back(L"TODO");
    allowedTypes.push_back(L"@todo");
    allowedTypes.push_back(L"\\todo");
    allowedTypes.push_back(L"FIXME");
    allowedTypes.push_back(L"@fixme");
    allowedTypes.push_back(L"\\fixme");
    allowedTypes.push_back(L"NOTE");
    allowedTypes.push_back(L"@note");
    allowedTypes.push_back(L"\\note");

    wxArrayString startStrings;
    startStrings.push_back(L"#warning");
    startStrings.push_back(L"#error");
    startStrings.push_back(L"///");
    startStrings.push_back(L"/**");
    startStrings.push_back(L"//");
    startStrings.push_back(L"/*");

    ParseBufferForTODOs(outItemsMap, outItems, startStrings, allowedTypes, buffer, "test.cpp");

    for (TodoItemsMap::const_iterator it = outItemsMap.begin(); it != outItemsMap.end(); ++it)
    {
        for (const ToDoItem &item : it->second)
            printf("%s -> %s %s %s %s %s %s %s %d %d\n",
                   it->first.utf8_str().data(),
                   item.type.utf8_str().data(),
                   item.text.utf8_str().data(),
                   item.user.utf8_str().data(),
                   item.filename.utf8_str().data(),
                   item.lineStr.utf8_str().data(),
                   item.priorityStr.utf8_str().data(),
                   item.date.utf8_str().data(),
                   item.line,
                   item.priority);
    }

    for (size_t ii = 0; ii < outItems.GetCount(); ii++)
    {
        const ToDoItem &item = outItems[ii];
        printf("%d -> %s %s %s %s %s %s %s %d %d\n",
               int(ii),
               item.type.utf8_str().data(),
               item.text.utf8_str().data(),
               item.user.utf8_str().data(),
               item.filename.utf8_str().data(),
               item.lineStr.utf8_str().data(),
               item.priorityStr.utf8_str().data(),
               item.date.utf8_str().data(),
               item.line,
               item.priority);
    }

    return 0;
}
