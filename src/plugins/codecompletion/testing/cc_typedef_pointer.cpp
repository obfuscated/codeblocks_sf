// bug report here:
// Code::Blocks / Tickets / #34 Code completion does not recognize struct pointer typedef - https://sourceforge.net/p/codeblocks/tickets/34/

typedef struct foo1
{
    int bar;
} foo1;

typedef struct foo1 * foo1Ptr;

typedef struct foo2
{
    int bar;
} foo2, * foo2Ptr;

//foo1Ptr   //foo1Ptr
//foo1Ptr-> //bar
//foo2Ptr-> //bar