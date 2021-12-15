/** @file
 * Safe way to include d3dkmthk.h.
 */

/*
 * Copyright (C) 2016-2020 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */

#ifndef IPRT_INCLUDED_win_d3dkmthk_h
#define IPRT_INCLUDED_win_d3dkmthk_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#ifdef _MSC_VER
# pragma warning(push)
# if _MSC_VER >= 1900 /*RT_MSC_VER_VC140*/
#  pragma warning(disable:4255) /* d3dkmthk.h(2061): warning C4255: 'PFND3DKMT_CHECKEXCLUSIVEOWNERSHIP': no function prototype given: converting '()' to '(void)' */
# endif
/* 10.0.22000.0 SDK: */
# pragma warning(disable:4668) /* d3dkmdt.h(405): warning C4668: 'DXGKDDI_INTERFACE_VERSION_WDDM1_3_M1' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif' */
#endif

#include <d3dkmthk.h>

#ifdef _MSC_VER
# pragma warning(pop)
#endif

#endif /* !IPRT_INCLUDED_win_d3dkmthk_h */

