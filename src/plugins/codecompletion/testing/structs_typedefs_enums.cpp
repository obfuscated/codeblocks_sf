#include <cstdarg>
#include <string>

struct _s
{
  int   x;
  float y;
};
typedef struct _s t_s;
typedef _s (*t_ptr_s)(int a, int b);
//typedef _s_ptr *(*ptr_t_ptr_s)(long l, int a, int b);
typedef std::basic_string<char> my_string;

typedef struct _s_inner
{
	int z_inner;
} t_s_inner;

typedef struct _s_outer
{
	t_s_inner z;
	void*     pVoid;
	char 			c;
} t_s_outer;

class _c
{
  int   l;
  float m;
};
typedef class _c t_c;

typedef class _c_inner
{
  int   l;
  float m;
} t_c_inner;

typedef class _c_mult
{
    long  l;
    float f;
} t_c_mult, *p_t_c_mult;

typedef unsigned int i_uinteger;

typedef void   t_void   (int i, const char *c, va_list v_l);
typedef void (*t_p_void)(int i, const char *c, va_list v_l);

// a hard (stripped) example from windef.h and wtypes.h:
#ifndef _ANONYMOUS_STRUCT
  #define _ANONYMOUS_STRUCT
  #define _STRUCT_NAME(x) x
#else
  #define _STRUCT_NAME(x)
#endif
typedef union tagCY {
	_ANONYMOUS_STRUCT struct {
		unsigned long Lo;
		long Hi;
	}_STRUCT_NAME(s);
	long long int64;
} CY;

// a very hard (stripped) sample from mmsystems.h:
#ifndef NONAMELESSUNION
  #ifdef __GNUC__
    #if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
      #define _ANONYMOUS_UNION __extension__
      #define _ANONYMOUS_STRUCT __extension__
    #else
      #if defined(__cplusplus)
        #define _ANONYMOUS_UNION __extension__
      #endif /* __cplusplus */
    #endif /* __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95) */
  #endif /* __GNUC__ */
#endif /* NONAMELESSUNION */
typedef struct tagMIXERCONTROLA {
	long cbStruct;
	char szShortName[1024];
	union {
		_ANONYMOUS_STRUCT struct {
			long lMinimum;
			long lMaximum;
		}_STRUCT_NAME(s);
		_ANONYMOUS_STRUCT struct {
			int dwMinimum;
			int dwMaximum;
		}_STRUCT_NAME(s1);
		int dwReserved[6];
	} Bounds;
	union {
		long cSteps;
		long dwReserved[6];
	} Metrics;
} MIXERCONTROLA,*PMIXERCONTROLA,*LPMIXERCONTROLA;

// First comment
enum EGlobalCommented // Here can be a comment, too
// Here, too
{ /* even here */
                       egcOne,                 //!< Typical Doxygen comment
  // In between comment
                       egcTwo,                 /** another version */
                       egcThree = 5 /* Foo */, // Inside comment
/* Leading comment */  egcFour
}; // Final comment

enum EGlobal
{
  egOne, egTwo, egThree
};

enum EGlobalAssign
{
  egaOne = 1, egaTwo = 2, egaThree = egaTwo
};

enum class EEnumClassGlobal
{
	ecgOne,
	ecgTwo,
	ecgThree
};

struct SEnum
{
	enum ELocal
	{
		elOne, elTwo, elThree,
	};

	enum { eUnnamed };

	enum class EEnumClassLocal
	{
		eclOne,
		eclTwo,
		eclThree
  };
};

int main (void)
{
    struct _s  s;
    struct _s& rs = s;
    struct _s* ps = &s;
    t_s        ts;
    t_s&       rts = ts;
    t_s*       pts = &ts;

//    s.
//    rs.
//    ps.
//    ps->
//    ts.
//    rts.
//    pts.
//    pts->

    std::string ss;
    my_string   ms;
//    ss.
//    ms.

//    t_ptr_s(
//    t_ptr_s(3,3).

//    ptr_t_ptr_s(
//    ptr_t_ptr_s(100,3,3).

    t_s_outer  ts_outer;
    t_s_outer* pts_outer;

//    ts_outer.
//    ts_outer.z.
//    ts_outer.z->
//    ts_outer->
//    ts_outer->z.
//    ts_outer->z->
//
//    pts_outer.
//    pts_outer.z.
//    pts_outer.z->
//    pts_outer->
//    pts_outer->z.
//    pts_outer->z->

    t_c_inner  tc_inner;
    t_c_inner* ptc_inner;

//    tc_inner.
//    tc_inner->
//    ptc_inner.
//    ptc_inner->

    t_c_mult   tc_mult;
    p_t_c_mult ptc_mult;

//    tc_mult.
//    ptc_mult->

//    i_

//    t_void(
//    t_p_void(

//    EGlobalCommented::
//    EGlobal::
//    EGlobalAssign::
//    EEnumClassGlobal::
//    SEnum::
//    SEnum::ELocal::
//    SEnum::eEnumClassLocal::

    return 0;
}
