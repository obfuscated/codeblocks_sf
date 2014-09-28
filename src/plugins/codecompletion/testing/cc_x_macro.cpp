/* x macro
 * https://en.wikipedia.org/wiki/X_Macro
 */


#define LIST_OF_VARIABLES \
    X(value1) \
    X(value2) \
    X(value3)

#define X(name) int name;
LIST_OF_VARIABLES
#undef X

// val  //value1,value2,value3