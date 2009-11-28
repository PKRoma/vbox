/*
 * Copyright (c) 2002 by The XFree86 Project, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the copyright holder(s)
 * and author(s) shall not be used in advertising or otherwise to promote
 * the sale, use or other dealings in this Software without prior written
 * authorization from the copyright holder(s) and author(s).
 */

/*
 * Structs used to hold the pre-parsed pci.ids data.  These are private
 * to the scanpci and pcidata modules.
 */

#ifdef HAVE_XORG_CONFIG_H
#include <xorg-config.h>
#endif

#ifndef _XF86_PCISTR_H
#define _XF86_PCISTR_H

typedef struct {
    unsigned short VendorID;
    unsigned short SubsystemID;
    const char *SubsystemName;
    unsigned short class;
} pciSubsystemInfo;

typedef struct {
    unsigned short DeviceID;
    const char *DeviceName;
    const pciSubsystemInfo **Subsystem;
    unsigned short class;
} pciDeviceInfo;

typedef struct {
    unsigned short VendorID;
    const char *VendorName;
    const pciDeviceInfo **Device;
} pciVendorInfo;

typedef struct {
    unsigned short VendorID;
    const char *VendorName;
    const pciSubsystemInfo **Subsystem;
} pciVendorSubsysInfo;

#endif /* _XF86_PCISTR_H */
