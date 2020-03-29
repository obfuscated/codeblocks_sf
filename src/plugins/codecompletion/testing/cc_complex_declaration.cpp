//Code::Blocks / Tickets / #127 Code completion fails with complex declarations/definitions - https://sourceforge.net/p/codeblocks/tickets/127/

int test01;
int *test02;
int (*test03);
int (*test04)();
int test05();
int (test06)();
int (test07);
int *const test08 = 0;
int (*const test09) = 0;
int test10[1] = {0};
int *test11[1] = {0};
int (*test12)[1];
int *test13();
int (*test14());
int (*test15())();



//test01    //test01
//test02    //test02
//test03    //test03
//test04    //test04
//test05    //test05
//test06    //test06
//test07    //test07
//test08    //test08
//test09    //test09
//test10    //test10
//test11    //test11
//test12    //test12
//test13    //test13
//test14    //test14
//test15    //test15