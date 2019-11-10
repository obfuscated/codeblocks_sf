class ClassA
{
public:
    int a;
    struct A
    {
        int c;
    } z;

    // unnamed struct
    struct
    {
        int s;

        union
        {
            int u;
            short v;
        };

        enum EnumS
        {
            STRUCT_A,
            STRUCT_B,
        };
    };

    enum
    {
        UNNAMED_A,
        UNNAMED_B
    };

    enum EnumA
    {
        ENUM_A,
        ENUM_B
    };
};

// Hovering on following member variables should
// show tooltip: a, s, u, v, all the enums like STRUCT_A, etc.
// not c

//ClassA:: //a,s,u,v,STRUCT_A,STRUCT_B,UNNAMED_A,UNNAMED_B,ENUM_A,ENUM_B