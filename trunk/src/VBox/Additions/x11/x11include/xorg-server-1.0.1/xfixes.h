/*
 * $Id: xfixes.h,v 1.6 2005/07/03 07:02:08 daniels Exp $
 *
 * Copyright © 2002 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#ifndef _XFIXES_H_
#define _XFIXES_H_

#include "resource.h"

extern RESTYPE	RegionResType;
extern int	XFixesErrorBase;

#define VERIFY_REGION(pRegion, rid, client, mode) { \
    pRegion = SecurityLookupIDByType (client, rid, RegionResType, mode); \
    if (!pRegion) { \
	client->errorValue = rid; \
	return XFixesErrorBase + BadRegion; \
    } \
}

#define VERIFY_REGION_OR_NONE(pRegion, rid, client, mode) { \
    pRegion = 0; \
    if (rid) VERIFY_REGION(pRegion, rid, client, mode); \
}

RegionPtr
XFixesRegionCopy (RegionPtr pRegion);


#endif /* _XFIXES_H_ */
