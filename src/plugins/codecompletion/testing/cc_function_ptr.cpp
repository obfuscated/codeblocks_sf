
void (*foo)(int);
foo = &my_int_func;
foo();

int (*FuncArray[10][20]) ();

void (*foo_assign)(int) = &my_int_func;
int *(*func_array_assign[10][20])(int a, int b) = &my_array_func;

//foo  //foo
//Fun  //FuncArray

//foo  //foo_assign
//fun  //func_array_assign
