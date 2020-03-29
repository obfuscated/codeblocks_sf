// bug report here: http://sourceforge.net/p/codeblocks/tickets/176

enum : int // specifying the underlying type to be int
{
    QWERT
};

enum class StrongEnum : char // specifying the underlying type to be char
{
    YUIOP
};

//QWERT //QWERT
//YUIOP //YUIOP
//StrongEnum:: //YUIOP
