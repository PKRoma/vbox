/** @file
 * PDM - Pluggable Device Manager, Tasks.
 */

/*
 * Copyright (C) 2019 Oracle Corporation
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

#ifndef VBOX_INCLUDED_vmm_pdmtask_h
#define VBOX_INCLUDED_vmm_pdmtask_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <VBox/types.h>

RT_C_DECLS_BEGIN

/** @defgroup grp_pdm_task      The PDM Tasks API
 * @ingroup grp_pdm
 *
 * A task is a predefined asynchronous procedure call that can be triggered from
 * any context.
 *
 * @{
 */

/** PDM task handle. */
typedef uint64_t            PDMTASKHANDLE;
/** NIL PDM task handle. */
#define NIL_PDMTASKHANDLE   UINT64_MAX


/**
 * Task worker callback for devices.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      The user parameter.
 * @thread  Task worker thread.
 * @remarks The device critical section will NOT be entered before calling the
 *          callback.  No other locks will be held either.
 */
typedef DECLCALLBACK(void) FNPDMTASKDEV(PPDMDEVINS pDevIns, void *pvUser);
/** Pointer to a FNPDMTASKDEV(). */
typedef FNPDMTASKDEV *PFNPDMTASKDEV;

/**
 * Task worker callback for drivers.
 *
 * @param   pDrvIns     The driver instance.
 * @param   pvUser      The user parameter.
 * @thread  Task worker thread.
 * @remarks No other locks will be held.
 */
typedef DECLCALLBACK(void) FNPDMTASKDRV(PPDMDRVINS pDrvIns, void *pvUser);
/** Pointer to a FNPDMTASKDRV(). */
typedef FNPDMTASKDRV *PFNPDMTASKDRV;

/**
 * Task worker callback for USB devices.
 *
 * @param   pUsbIns     The USB device instance.
 * @param   pvUser      The user parameter.
 * @thread  Task worker thread.
 * @remarks No other locks will be held.
 */
typedef DECLCALLBACK(void) FNPDMTASKUSB(PPDMUSBINS pUsbIns, void *pvUser);
/** Pointer to a FNPDMTASKUSB(). */
typedef FNPDMTASKUSB *PFNPDMTASKUSB;

/**
 * Task worker callback for internal components.
 *
 * @param   pVM         The cross context VM structure.
 * @param   pvUser      The user parameter.
 * @thread  Task worker thread.
 * @remarks No other locks will be held.
 */
typedef DECLCALLBACK(void) FNPDMTASKINT(PVM pVM, void *pvUser);
/** Pointer to a FNPDMTASKINT(). */
typedef FNPDMTASKINT *PFNPDMTASKINT;


/** @name PDMTASK_F_XXX - Task creation flags.
 * @{ */
/** Create a ring-0 triggerable task. */
#define PDMTASK_F_R0            RT_BIT_32(0)
/** Create a raw-mode triggerable task. */
#define PDMTASK_F_RC            RT_BIT_32(1)
/** Create a ring-0 and raw-mode triggerable task. */
#define PDMTASK_F_RZ            (PDMTASK_F_R0 | PDMTASK_F_RC)
/** Valid flags. */
#define PDMTASK_F_VALID_MASK    UINT32_C(0x00000003)
/** @} */

#ifdef VBOX_IN_VMM
/**
 * Task owner type.
 */
typedef enum PDMTASKTYPE
{
    /** Device consumer. */
    PDMTASKTYPE_DEV = 1,
    /** Driver consumer. */
    PDMTASKTYPE_DRV,
    /** USB device consumer. */
    PDMTASKTYPE_USB,
    /** Internal consumer. */
    PDMTASKTYPE_INTERNAL,
} PDMTASKTYPE;

VMMR3_INT_DECL(int) PDMR3TaskCreateInternal(PVM pVM, uint32_t fFlags, const char *pszName,
                                            PFNPDMTASKINT pfnCallback, void *pvUser, PDMTASKHANDLE *phTask);
VMMR3_INT_DECL(int) PDMR3TaskCreateGeneric(PVM pVM, uint32_t fFlags, const char *pszName, PDMTASKTYPE enmType, void *pvOwner,
                                           PFNRT pfnCallback, void *pvUser, PDMTASKHANDLE *phTask);
VMMR3_INT_DECL(int) PDMR3TaskDestroyInternal(PVM pVM, PDMTASKHANDLE hTask);
VMMR3_INT_DECL(int) PDMR3TaskDestroyAllByOwner(PVM pVM, PDMTASKTYPE enmType, void *pvOwner);
VMMR3_INT_DECL(int) PDMR3TaskDestroySpecific(PVM pVM, PDMTASKTYPE enmType, void *pvOwner, PDMTASKHANDLE hTask);

VMM_INT_DECL(int)   PDMTaskTrigger(PVM pVM, PDMTASKTYPE enmType, void *pvOwner, PDMTASKHANDLE hTask);
VMM_INT_DECL(int)   PDMTaskTriggerInternal(PVM pVM, PDMTASKHANDLE hTask);
#endif /* VBOX_IN_VMM */

/** @} */

RT_C_DECLS_END

#endif /* !VBOX_INCLUDED_vmm_pdmtask_h */

