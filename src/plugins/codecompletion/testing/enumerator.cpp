// test parsing of enumerator assignments

enum TokenizerState
{
    tsSkipEqual         = 0x0001,         /// Skip the assignment statement
    tsSkipQuestion      = 0x0002,         /// Skip the conditional evaluation statement
    tsSkipSubScrip      = 0x0004,         /// Skip the array-subscript notation statement

    tsSingleAngleBrace  = 0x0008,         /// Reserve angle braces
    tsReadRawExpression = 0x0010,         /// Reserve every chars

    tsSkipNone          = 0x1000,         /// Skip None
    // convenient masks
    tsSkipUnWanted      = tsSkipEqual    | tsSkipQuestion | tsSkipSubScrip,
    tsTemplateArgument  = tsSkipUnWanted | tsSingleAngleBrace,
    tsAnotherState
};

enum HexAssignment
{
    haFirst = 0x0001,
    haSecond,
    haThird
};

enum Colors
{
    clRed,
    clBlue,
    clGreen
};

enum TestEnum
{
    teFirst,
    teSecond = 5,
    teThird
};
