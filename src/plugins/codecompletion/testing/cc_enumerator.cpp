// test parsing of enumerator assignments

enum TokenizerState
{
    tsSkipEqual         = 0x0001,         //!< Skip the assignment statement
    tsSkipQuestion      = 0x0002,         //!< Skip the conditional evaluation statement
    tsSkipSubScrip      = 0x0004,         //!< Skip the array-subscript notation statement

    tsSingleAngleBrace  = 0x0008,         //!< Reserve angle braces
    tsReadRawExpression = 0x0010,         //!< Reserve every chars

    tsSkipNone          = 0x1000,         //!< Skip None
    // convenient masks
    tsSkipUnWanted      = tsSkipEqual    | tsSkipQuestion | tsSkipSubScrip,
    tsTemplateArgument  = tsSkipUnWanted | tsSingleAngleBrace,
    tsAnotherState // value unknown
};

enum HexAssignment
{
    haFirst = 0x0001,
    haSecond,
    haThird,
    haThree = haThird,
    haFourth // 0x0004
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
    teThird,
    teFourth = 3+7
};

#define OFFSET 15
enum OffsetEnum
{
    offFirst    = OFFSET,
    offSecond,
    offThird,

    offFourth   = offThird + OFFSET,
    offFifth
};

#define SHIFT(_x) (1 << (_x))
enum PowerEnum
{
    powFirst    = SHIFT(1),
    powSecond   = SHIFT(2),
    powThird    = SHIFT(3),
    powFourth   = SHIFT(4),
    powFifth    = SHIFT(5)
};

//powF   //powFirst,powFourth,powFifth
//te     //teFirst,teFourth
//ha     //haFourth,haThree