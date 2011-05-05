/* $Id$ */
/** @file
 * USB Power state Handling
 */
/*
 * Copyright (C) 2011 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef ___VBoxUsbPwr_h___
#define ___VBoxUsbPwr_h___

typedef struct VBOXUSB_PWRSTATE
{
    POWER_STATE PowerState;
    ULONG PowerDownLevel;
} VBOXUSB_PWRSTATE, *PVBOXUSB_PWRSTATE;

DECLHIDDEN(VOID) vboxUsbPwrStateInit(PVBOXUSBDEV_EXT pDevExt);
DECLHIDDEN(NTSTATUS) vboxUsbDispatchPower(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);

#endif /* #ifndef ___VBoxUsbPwr_h___ */
