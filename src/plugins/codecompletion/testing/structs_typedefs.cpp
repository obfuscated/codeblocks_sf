#include <cstdarg>

struct _s
{
  int   x;
  float y;
};
typedef struct _s t_s;

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

typedef unsigned int i_uinteger;

typedef void   t_void   (int errnum, const char *msg, va_list ap);
typedef void (*t_p_void)(int errnum, const char *msg, va_list ap);

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

//    i_

    t_void     tv;
    t_p_void   tpv;

//    tp(
//    tpv(

    return 0;
}
