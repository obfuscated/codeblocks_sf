//Bug report. Infinite loop in ParserThread::SkipBlock - 
//http://forums.codeblocks.org/index.php/topic,20568.0.html

#define AA__( x ) #x
#define AA_( x ) AA__( prefix##x )
#define AA( x ) AA_( x )

#define BB 42

struct CC { int member; };
struct DD { CC cc; };

#define EE() g( AA(BB) )

#define FF (EE()->cc)

#define member FF.member

DD* g(const char*);

int aaa;
int f()
{
    return member; // "member" expand to " (g(AA__)->cc).member" infinitly
}

//aaa //aaa