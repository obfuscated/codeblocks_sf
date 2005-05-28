#include <stdio.h>

const char* WLIB_STRING = "wlib -q -c -b -n ";

char* copy_string(char* out, const char* in)
{
    while (*in)
        *(out++) = *(in++);
    return out;
}

int main(int argc, char**argv)
{
//    printf("WLIB wrapper, syntax: wlib_wrap lib_name obj1 obj2...\n");
    argc--;

    if ( argc < 2 )
        return 0;

    // count length of args
    int len = strlen(WLIB_STRING);
    int i = 0;
    for (i = 1; i <= argc; ++i)
    {
        len += strlen(argv[i]) + 2; // +2 for a space and '+'
    }
    ++len;
    ++len; // 2 bytes more, to be sure

    // allocate result string
    char* result = (char*)malloc(len);
    memset(result, 32, len);
    result[len - 1] = 0;

    // add basic command
    char* tmp = result;
    tmp = copy_string(tmp, WLIB_STRING);

    // add lib name
    tmp = copy_string(tmp, argv[1]);
    ++tmp;

    // add args
    for(i = 2; i <= argc; ++i)
    {
        *(tmp++) = '+';
        tmp = copy_string(tmp, argv[i]);
        ++tmp;
    }

//    printf("Generated string: %s\n", result);
    int ret = system(result);
    free(result);
    return ret;
}
