#ifndef _M_C_H_
#define _M_C_H_

#include "P/C.h"

namespace P
{
	class M_C : public C
	{
  public:
             M_C();
		virtual ~M_C() {};

		virtual void init() const {};
	};
}

#endif //#ifndef _M_C_H_
