
// Sourceforge ticket #724
// https://sourceforge.net/p/codeblocks/tickets/724/

extern __declspec(dllexport) void f1(int*), *f2(), f3(float);

extern unsigned int Func1(int*), *Func2(char);

unsigned int  (*fp1)(int*) = Func1,
             *(*fp2)(char) = Func2,
              (*fp3)();

class MyClass
{
    int m_a;

    // ctor
    MyClass(int a) : m_a(a) {}
};

MyClass obj1(2), obj2(20), obj3(7);




//f1      //f1
//f2      //f2
//f3      //f3
//Func    //Func1,Func2
//fp      //fp1,fp2,fp3
//obj1.   //m_a
//obj2.   //m_a
//obj3.   //m_a