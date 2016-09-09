
// parsing C99 designated initializer of struct
// http://forums.codeblocks.org/index.php/topic,21417.0.html
struct AAA
{
  int   x;
  float y;
};

struct AAA a1 = {.x = 1, .y=0.2}; // first line
struct AAA a2 = {.x = 1, .y=0.2}; // second line

//a  //a1,a2