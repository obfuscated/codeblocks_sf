// Code::Blocks / Tickets / #762 Code completion fails on struct instances declared on same statement
// — https://sourceforge.net/p/codeblocks/tickets/762/
struct Point {
    int x;
    int y;
};


struct Point point1, point2;
struct Point point3;
struct Point point4;


//point1. //x
//point2. //x
//point3. //x
//point4. //x