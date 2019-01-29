// test case for SF #759 Autocomplete failing for typedef struct with attributes
// https://sourceforge.net/p/codeblocks/tickets/759/

typedef struct {
    int nValue;
} __attribute__((packed)) MY_TYPE;

MY_TYPE myType;

//myType.          //nValue