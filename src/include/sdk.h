/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

/** Code::Blocks SDK precompiled headers support for plugins.
  * This is precompiled with -DBUILDING_PLUGIN
  */

#ifndef SDK_H
#define SDK_H

#ifdef __WXMSW__
	#include "sdk_common.h"
#else
	// for non-windows platforms, one PCH is enough
	#include "sdk_precomp.h"
#endif

#endif // SDK_H
