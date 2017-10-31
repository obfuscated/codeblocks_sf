// http://sourceforge.net/p/codeblocks/tickets/127

enum e_example
{
    E_EX_1,
    E_EX_2,
    E_EX_3
};

enum e_example my_function();

enum e_example my_function()
{
    return E_EX_3;
}



//my_func   //my_function