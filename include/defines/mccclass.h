/* Automatically generated header! Do not edit! */

#ifndef _INLINE_MCCCLASS_H
#define _INLINE_MCCCLASS_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef MCCCLASS_BASE_NAME
#define MCCCLASS_BASE_NAME MCCClassBase
#endif /* !MCCCLASS_BASE_NAME */

#define MCC_Query(___which) __MCC_Query_WB(MCCCLASS_BASE_NAME, ___which)
#define __MCC_Query_WB(___base, ___which) \
	AROS_LC1(ULONG, MCC_Query, \
	AROS_LCA(LONG, (___which), D0), \
	struct Library *, (___base), 5, Mccclass)

#endif /* !_INLINE_MCCCLASS_H */
