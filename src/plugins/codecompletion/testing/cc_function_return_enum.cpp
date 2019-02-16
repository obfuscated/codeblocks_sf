// http://sourceforge.net/p/codeblocks/tickets/127

enum e_example
{
    E_EX_1,
    E_EX_2,
    E_EX_3
};

enum e_example fun1();


enum e_example fun2()
{
    return E_EX_3;
}




//fun1   //fun1
//fun2   //fun2