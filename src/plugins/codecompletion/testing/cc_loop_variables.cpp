
void Function1(void)
{
    int some_int;
    for (int for_int = 0; for_int < 5; ++for_int)
    {
        //some  //some_int
        //for   //for_int
    }
}

void Function2(void)
{
    int value1 = 0;
    int value2 = 0;

    // hover on "value1"
    if (!value1) {}
    // hover on "value1" and "value2"
    if (value1 && value2) {}

    // Above tooltips will be shown correctly because the statements
    // within if(...), do(...), etc are NOT parsed entirely.
    // Revision 10230 "fixes" this problem, but now, parsing these statements
    // introduces the following regression.
    //if (!value1) {}   // adds a new token "! value1"
    //if (value1 && value2){}   // adds a new token "value1&& value2"
}
