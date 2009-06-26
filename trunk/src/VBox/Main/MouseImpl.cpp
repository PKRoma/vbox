/* $Id$ */

/** @file
 *
 * VirtualBox COM class implementation
 */

/*
 * Copyright (C) 2006-2008 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */

#include "MouseImpl.h"
#include "DisplayImpl.h"
#include "VMMDev.h"

#include "Logging.h"

#include <VBox/pdmdrv.h>
#include <iprt/asm.h>
#include <VBox/VBoxDev.h>

/**
 * Mouse driver instance data.
 */
typedef struct DRVMAINMOUSE
{
    /** Pointer to the mouse object. */
    Mouse                      *pMouse;
    /** Pointer to the driver instance structure. */
    PPDMDRVINS                  pDrvIns;
    /** Pointer to the mouse port interface of the driver/device above us. */
    PPDMIMOUSEPORT              pUpPort;
    /** Our mouse connector interface. */
    PDMIMOUSECONNECTOR          Connector;
} DRVMAINMOUSE, *PDRVMAINMOUSE;

/** Converts PDMIMOUSECONNECTOR pointer to a DRVMAINMOUSE pointer. */
#define PDMIMOUSECONNECTOR_2_MAINMOUSE(pInterface) ( (PDRVMAINMOUSE) ((uintptr_t)pInterface - OFFSETOF(DRVMAINMOUSE, Connector)) )


// constructor / destructor
/////////////////////////////////////////////////////////////////////////////

DEFINE_EMPTY_CTOR_DTOR (Mouse)

HRESULT Mouse::FinalConstruct()
{
    mpDrv = NULL;
    mLastAbsX = 0;
    mLastAbsY = 0;
    return S_OK;
}

void Mouse::FinalRelease()
{
    uninit();
}

// public methods only for internal purposes
/////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the mouse object.
 *
 * @returns COM result indicator
 * @param parent handle of our parent object
 */
HRESULT Mouse::init (Console *parent)
{
    LogFlowThisFunc (("\n"));

    ComAssertRet (parent, E_INVALIDARG);

    /* Enclose the state transition NotReady->InInit->Ready */
    AutoInitSpan autoInitSpan (this);
    AssertReturn (autoInitSpan.isOk(), E_FAIL);

    unconst(mParent) = parent;

#ifdef RT_OS_L4
    /* L4 console has no own mouse cursor */
    uHostCaps = VMMDEV_MOUSEHOSTCANNOTHWPOINTER;
#else
    uHostCaps = 0;
#endif

    /* Confirm a successful initialization */
    autoInitSpan.setSucceeded();

    return S_OK;
}

/**
 *  Uninitializes the instance and sets the ready flag to FALSE.
 *  Called either from FinalRelease() or by the parent when it gets destroyed.
 */
void Mouse::uninit()
{
    LogFlowThisFunc (("\n"));

    /* Enclose the state transition Ready->InUninit->NotReady */
    AutoUninitSpan autoUninitSpan (this);
    if (autoUninitSpan.uninitDone())
        return;

    if (mpDrv)
        mpDrv->pMouse = NULL;
    mpDrv = NULL;

    unconst (mParent).setNull();
}

// IMouse properties
/////////////////////////////////////////////////////////////////////////////

/**
 * Returns whether the current setup can accept absolute mouse
 * events.
 *
 * @returns COM status code
 * @param absoluteSupported address of result variable
 */
STDMETHODIMP Mouse::COMGETTER(AbsoluteSupported) (BOOL *absoluteSupported)
{
    if (!absoluteSupported)
        return E_POINTER;

    AutoCaller autoCaller (this);
    CheckComRCReturnRC (autoCaller.rc());

    AutoWriteLock alock (this);

    CHECK_CONSOLE_DRV (mpDrv);

    ComAssertRet (mParent->getVMMDev(), E_FAIL);
    ComAssertRet (mParent->getVMMDev()->getVMMDevPort(), E_FAIL);

    *absoluteSupported = FALSE;
    uint32_t mouseCaps;
    mParent->getVMMDev()->getVMMDevPort()->pfnQueryMouseCapabilities(mParent->getVMMDev()->getVMMDevPort(), &mouseCaps);
    *absoluteSupported = mouseCaps & VMMDEV_MOUSEGUESTWANTSABS;

    return S_OK;
}

/**
 * Returns whether the current setup can accept relative mouse
 * events.
 *
 * @returns COM status code
 * @param absoluteSupported address of result variable
 */
STDMETHODIMP Mouse::COMGETTER(NeedsHostCursor) (BOOL *needsHostCursor)
{
    if (!needsHostCursor)
        return E_POINTER;

    AutoCaller autoCaller (this);
    CheckComRCReturnRC (autoCaller.rc());

    AutoWriteLock alock (this);

    CHECK_CONSOLE_DRV (mpDrv);

    ComAssertRet (mParent->getVMMDev(), E_FAIL);
    ComAssertRet (mParent->getVMMDev()->getVMMDevPort(), E_FAIL);

    *needsHostCursor = FALSE;
    uint32_t mouseCaps;
    mParent->getVMMDev()->getVMMDevPort()->pfnQueryMouseCapabilities(mParent->getVMMDev()->getVMMDevPort(), &mouseCaps);
    *needsHostCursor = mouseCaps & VMMDEV_MOUSEGUESTNEEDSHOSTCUR;

    return S_OK;
}

// IMouse methods
/////////////////////////////////////////////////////////////////////////////

/**
 * Send a mouse event.
 *
 * @returns COM status code
 * @param dx          X movement
 * @param dy          Y movement
 * @param dz          Z movement
 * @param buttonState The mouse button state
 */
STDMETHODIMP Mouse::PutMouseEvent(LONG dx, LONG dy, LONG dz, LONG buttonState)
{
    HRESULT rc = S_OK;

    AutoCaller autoCaller (this);
    CheckComRCReturnRC (autoCaller.rc());

    AutoWriteLock alock (this);

    CHECK_CONSOLE_DRV (mpDrv);

    ComAssertRet (mParent->getVMMDev(), E_FAIL);
    ComAssertRet (mParent->getVMMDev()->getVMMDevPort(), E_FAIL);

    uint32_t mouseCaps;
    mParent->getVMMDev()->getVMMDevPort()
        ->pfnQueryMouseCapabilities(mParent->getVMMDev()->getVMMDevPort(),
                                    &mouseCaps);
    /*
     * This method being called implies that the host no
     * longer wants to use absolute coordinates. If the VMM
     * device isn't aware of that yet, tell it.
     */
    if (mouseCaps & VMMDEV_MOUSEHOSTWANTSABS)
    {
        mParent->getVMMDev()->getVMMDevPort()->pfnSetMouseCapabilities(
            mParent->getVMMDev()->getVMMDevPort(), uHostCaps);
    }

    uint32_t fButtons = 0;
    if (buttonState & MouseButtonState_LeftButton)
        fButtons |= PDMIMOUSEPORT_BUTTON_LEFT;
    if (buttonState & MouseButtonState_RightButton)
        fButtons |= PDMIMOUSEPORT_BUTTON_RIGHT;
    if (buttonState & MouseButtonState_MiddleButton)
        fButtons |= PDMIMOUSEPORT_BUTTON_MIDDLE;

    int vrc = mpDrv->pUpPort->pfnPutEvent(mpDrv->pUpPort, dx, dy, dz, fButtons);
    if (RT_FAILURE (vrc))
        rc = setError (VBOX_E_IPRT_ERROR,
            tr ("Could not send the mouse event to the virtual mouse (%Rrc)"),
                vrc);

    return rc;
}

/**
 * Send an absolute mouse event to the VM. This only works
 * when the required guest support has been installed.
 *
 * @returns COM status code
 * @param x          X position (pixel)
 * @param y          Y position (pixel)
 * @param dz         Z movement
 * @param buttonState The mouse button state
 */
STDMETHODIMP Mouse::PutMouseEventAbsolute(LONG x, LONG y, LONG dz,
                                          LONG buttonState)
{
    HRESULT rc = S_OK;

    AutoCaller autoCaller (this);
    CheckComRCReturnRC (autoCaller.rc());

    AutoWriteLock alock (this);

    CHECK_CONSOLE_DRV (mpDrv);

    ComAssertRet (mParent->getVMMDev(), E_FAIL);
    ComAssertRet (mParent->getVMMDev()->getVMMDevPort(), E_FAIL);

    uint32_t mouseCaps;
    mParent->getVMMDev()->getVMMDevPort()
        ->pfnQueryMouseCapabilities(mParent->getVMMDev()->getVMMDevPort(),
                                    &mouseCaps);
    /*
     * This method being called implies that the host wants
     * to use absolute coordinates. If the VMM device isn't
     * aware of that yet, tell it.
     */
    if (!(mouseCaps & VMMDEV_MOUSEHOSTWANTSABS))
    {
        mParent->getVMMDev()->getVMMDevPort()->pfnSetMouseCapabilities(
            mParent->getVMMDev()->getVMMDevPort(),
            uHostCaps | VMMDEV_MOUSEHOSTWANTSABS);
    }

    Display *pDisplay = mParent->getDisplay();
    ComAssertRet (pDisplay, E_FAIL);

    ULONG displayWidth;
    ULONG displayHeight;
    rc = pDisplay->COMGETTER(Width)(&displayWidth);
    ComAssertComRCRet (rc, rc);
    rc = pDisplay->COMGETTER(Height)(&displayHeight);
    ComAssertComRCRet (rc, rc);

    uint32_t mouseXAbs = displayWidth? (x * 0xFFFF) / displayWidth: 0;
    uint32_t mouseYAbs = displayHeight? (y * 0xFFFF) / displayHeight: 0;

    /*
     * Send the absolute mouse position to the VMM device.
     */
    int vrc = 0;
    if ((mLastAbsX != mouseXAbs) || (mLastAbsY != mouseYAbs))
    {
        vrc = mParent->getVMMDev()->getVMMDevPort()
                  ->pfnSetAbsoluteMouse(mParent->getVMMDev()->getVMMDevPort(),
                                        mouseXAbs, mouseYAbs);
        ComAssertRCRet (vrc, E_FAIL);
    }

    // Check if the guest actually wants absolute mouse positions.
    if (mouseCaps & VMMDEV_MOUSEGUESTWANTSABS)
    {
        uint32_t fButtons = 0;
        if (buttonState & MouseButtonState_LeftButton)
            fButtons |= PDMIMOUSEPORT_BUTTON_LEFT;
        if (buttonState & MouseButtonState_RightButton)
            fButtons |= PDMIMOUSEPORT_BUTTON_RIGHT;
        if (buttonState & MouseButtonState_MiddleButton)
            fButtons |= PDMIMOUSEPORT_BUTTON_MIDDLE;

        /* This is a workaround.  In order to alert the Guest Additions to the
         * fact that the absolute pointer position has changed, we send a
         * a minute movement event to the PS/2 mouse device.  But in order
         * to avoid the mouse jiggling every time the use clicks, we check to
         * see if the position has really changed since the last mouse event.
         */
        if (   ((mLastAbsX == mouseXAbs) && (mLastAbsY == mouseYAbs))
            || (mouseCaps & VBOXGUEST_MOUSE_GUEST_USES_VMMDEV))
            vrc = mpDrv->pUpPort->pfnPutEvent(mpDrv->pUpPort, 0, 0, dz,
                                              fButtons);
        else
            vrc = mpDrv->pUpPort->pfnPutEvent(mpDrv->pUpPort, 1, 1, dz,
                                              fButtons);
        mLastAbsX = mouseXAbs;
        mLastAbsY = mouseYAbs;
        if (RT_FAILURE (vrc))
            rc = setError (VBOX_E_IPRT_ERROR,
                tr ("Could not send the mouse event to the virtual mouse (%Rrc)"),
                    vrc);
    }

    return rc;
}

// private methods
/////////////////////////////////////////////////////////////////////////////

/**
 * Queries an interface to the driver.
 *
 * @returns Pointer to interface.
 * @returns NULL if the interface was not supported by the driver.
 * @param   pInterface          Pointer to this interface structure.
 * @param   enmInterface        The requested interface identification.
 */
DECLCALLBACK(void *)  Mouse::drvQueryInterface(PPDMIBASE pInterface, PDMINTERFACE enmInterface)
{
    PPDMDRVINS pDrvIns = PDMIBASE_2_PDMDRV(pInterface);
    PDRVMAINMOUSE pDrv = PDMINS_2_DATA(pDrvIns, PDRVMAINMOUSE);
    switch (enmInterface)
    {
        case PDMINTERFACE_BASE:
            return &pDrvIns->IBase;
        case PDMINTERFACE_MOUSE_CONNECTOR:
            return &pDrv->Connector;
        default:
            return NULL;
    }
}


/**
 * Destruct a mouse driver instance.
 *
 * @returns VBox status.
 * @param   pDrvIns     The driver instance data.
 */
DECLCALLBACK(void) Mouse::drvDestruct(PPDMDRVINS pDrvIns)
{
    PDRVMAINMOUSE pData = PDMINS_2_DATA(pDrvIns, PDRVMAINMOUSE);
    LogFlow(("Mouse::drvDestruct: iInstance=%d\n", pDrvIns->iInstance));
    if (pData->pMouse)
    {
        AutoWriteLock mouseLock (pData->pMouse);
        pData->pMouse->mpDrv = NULL;
    }
}


/**
 * Construct a mouse driver instance.
 *
 * @returns VBox status.
 * @param   pDrvIns     The driver instance data.
 *                      If the registration structure is needed, pDrvIns->pDrvReg points to it.
 * @param   pCfgHandle  Configuration node handle for the driver. Use this to obtain the configuration
 *                      of the driver instance. It's also found in pDrvIns->pCfgHandle, but like
 *                      iInstance it's expected to be used a bit in this function.
 */
DECLCALLBACK(int) Mouse::drvConstruct(PPDMDRVINS pDrvIns, PCFGMNODE pCfgHandle)
{
    PDRVMAINMOUSE pData = PDMINS_2_DATA(pDrvIns, PDRVMAINMOUSE);
    LogFlow(("drvMainMouse_Construct: iInstance=%d\n", pDrvIns->iInstance));

    /*
     * Validate configuration.
     */
    if (!CFGMR3AreValuesValid(pCfgHandle, "Object\0"))
        return VERR_PDM_DRVINS_UNKNOWN_CFG_VALUES;
    PPDMIBASE pBaseIgnore;
    int rc = pDrvIns->pDrvHlp->pfnAttach(pDrvIns, &pBaseIgnore);
    if (rc != VERR_PDM_NO_ATTACHED_DRIVER)
    {
        AssertMsgFailed(("Configuration error: Not possible to attach anything to this driver!\n"));
        return VERR_PDM_DRVINS_NO_ATTACH;
    }

    /*
     * IBase.
     */
    pDrvIns->IBase.pfnQueryInterface        = Mouse::drvQueryInterface;

    /*
     * Get the IMousePort interface of the above driver/device.
     */
    pData->pUpPort = (PPDMIMOUSEPORT)pDrvIns->pUpBase->pfnQueryInterface(pDrvIns->pUpBase, PDMINTERFACE_MOUSE_PORT);
    if (!pData->pUpPort)
    {
        AssertMsgFailed(("Configuration error: No mouse port interface above!\n"));
        return VERR_PDM_MISSING_INTERFACE_ABOVE;
    }

    /*
     * Get the Mouse object pointer and update the mpDrv member.
     */
    void *pv;
    rc = CFGMR3QueryPtr(pCfgHandle, "Object", &pv);
    if (RT_FAILURE(rc))
    {
        AssertMsgFailed(("Configuration error: No/bad \"Object\" value! rc=%Rrc\n", rc));
        return rc;
    }
    pData->pMouse = (Mouse *)pv;        /** @todo Check this cast! */
    pData->pMouse->mpDrv = pData;

    return VINF_SUCCESS;
}


/**
 * Main mouse driver registration record.
 */
const PDMDRVREG Mouse::DrvReg =
{
    /* u32Version */
    PDM_DRVREG_VERSION,
    /* szDriverName */
    "MainMouse",
    /* pszDescription */
    "Main mouse driver (Main as in the API).",
    /* fFlags */
    PDM_DRVREG_FLAGS_HOST_BITS_DEFAULT,
    /* fClass. */
    PDM_DRVREG_CLASS_MOUSE,
    /* cMaxInstances */
    ~0,
    /* cbInstance */
    sizeof(DRVMAINMOUSE),
    /* pfnConstruct */
    Mouse::drvConstruct,
    /* pfnDestruct */
    Mouse::drvDestruct,
    /* pfnIOCtl */
    NULL,
    /* pfnPowerOn */
    NULL,
    /* pfnReset */
    NULL,
    /* pfnSuspend */
    NULL,
    /* pfnResume */
    NULL,
    /* pfnDetach */
    NULL
};
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
