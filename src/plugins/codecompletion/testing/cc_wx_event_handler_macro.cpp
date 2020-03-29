// an entry from a static event table (simplified version)
struct  wxEventTableEntry
{
public:
    const int& m_eventType;
};

#define BEGIN_EVENT_TABLE(a,b)                         wxBEGIN_EVENT_TABLE(a,b)
#define END_EVENT_TABLE()                              wxEND_EVENT_TABLE()
#define wxEND_EVENT_TABLE() \
        wxDECLARE_EVENT_TABLE_TERMINATOR() };

        
#define wxDECLARE_EVENT_TABLE_ENTRY(type, winid, idLast, fn, obj) \
    wxEventTableEntry(type, winid, idLast, wxNewEventTableFunctor(type, fn), obj)

#define wxDECLARE_EVENT_TABLE_TERMINATOR() \
    wxEventTableEntry(wxEVT_NULL, 0, 0, 0, 0)
    
#define wxBEGIN_EVENT_TABLE(theClass, baseClass) \
    const wxEventTable theClass::sm_eventTable = \
        { &baseClass::sm_eventTable, &theClass::sm_eventTableEntries[0] }; \
    const wxEventTable *theClass::GetEventTable() const \
        { return &theClass::sm_eventTable; } \
    wxEventHashTable theClass::sm_eventHashTable(theClass::sm_eventTable); \
    wxEventHashTable &theClass::GetEventHashTable() const \
        { return theClass::sm_eventHashTable; } \
    const wxEventTableEntry theClass::sm_eventTableEntries[] = { \
 
// a simplified version of wxBEGIN_EVENT_TABLE
//#define wxBEGIN_EVENT_TABLE(theClass, baseClass) \
//     const wxEventTableEntry theClass::sm_eventTableEntries[] = { \ 
    
BEGIN_EVENT_TABLE(wx3_hangFrame,wxFrame)
    //(*EventTable(wx3_hangFrame)
    //*)
END_EVENT_TABLE()

int aaa;

//aaa //aaa