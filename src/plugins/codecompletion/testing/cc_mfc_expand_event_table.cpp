#define PTM_WARNING_DISABLE
#define PTM_WARNING_RESTORE
struct AFX_MSGMAP_ENTRY;
struct AFX_MSGMAP
{
   const AFX_MSGMAP* (*pfnGetBaseMap)();
   const AFX_MSGMAP_ENTRY* lpEntries;
};
#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
   PTM_WARNING_DISABLE \
   const AFX_MSGMAP* theClass::GetMessageMap() const \
      { return GetThisMessageMap(); } \
   const AFX_MSGMAP* theClass::GetThisMessageMap() \
   { \
      typedef theClass ThisClass;                     \
      typedef baseClass TheBaseClass;                  \
      static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
      {
#define END_MESSAGE_MAP() \
      {0, 0, 0, 0, 0, 0 } \
   }; \
      static const AFX_MSGMAP messageMap = \
      { &TheBaseClass::GetThisMessageMap, &_messageEntries[0] }; \
      return &messageMap; \
   }                          \
   PTM_WARNING_RESTORE
struct AFX_MSGMAP_ENTRY{
   int a,b,c,d,e,f;
};
class BaseClass{
   virtual AFX_MSGMAP* GetThisMessageMap();
public:
   virtual AFX_MSGMAP* GetMessageMap();
};
class SomeClass : public BaseClass{
   AFX_MSGMAP* GetThisMessageMap();
public:
   AFX_MSGMAP* GetMessageMap();
};

/// known functions
void mynonmissingfunc1(){
}
void mynonmissingfunc2(){
}

BEGIN_MESSAGE_MAP(SomeClass, BaseClass)
// [...]
END_MESSAGE_MAP()/// somehow ignored

//} }/// uncomment this to detect all functions (in my real world example, I didn't need two "}" but one)

/// unknown functions
void mymissingfunc1(){
}
void mymissingfunc2(){
}
void mymissingfunc3(){
}

// mymiss //mymissingfunc1,mymissingfunc2,mymissingfunc3