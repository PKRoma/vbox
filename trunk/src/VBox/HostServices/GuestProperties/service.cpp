/** @file
 *
 * Guest Property Service:
 * Host service entry points.
 */

/*
 * Copyright (C) 2008 Sun Microsystems, Inc.
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

/**
 * This HGCM service allows the guest to set and query values in a property
 * store on the host.  The service proxies the guest requests to the service
 * owner on the host using a request callback provided by the owner, and is
 * notified of changes to properties made by the host.  It forwards these
 * notifications to clients in the guest which have expressed interest and
 * are waiting for notification.
 *
 * The service currently consists of two threads.  One of these is the main
 * HGCM service thread which deals with requests from the guest and from the
 * host.  The second thread sends the host asynchronous notifications of
 * changes made by the guest and deals with notification timeouts.
 *
 * Guest requests to wait for notification are added to a list of open
 * notification requests and completed when a corresponding guest property
 * is changed or when the request times out.
 */

#define LOG_GROUP LOG_GROUP_HGCM

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <VBox/HostServices/GuestPropertySvc.h>

#include <memory>  /* for auto_ptr */

#include <iprt/err.h>
#include <iprt/assert.h>
#include <iprt/string.h>
#include <iprt/mem.h>
#include <iprt/autores.h>
#include <iprt/time.h>
#include <iprt/cpputils.h>
#include <iprt/req.h>
#include <iprt/thread.h>
#include <VBox/log.h>

#include <VBox/cfgm.h>

/*******************************************************************************
*   Internal functions                                                         *
*******************************************************************************/
/** Extract a pointer value from an HGCM parameter structure */
static int VBoxHGCMParmPtrGet (VBOXHGCMSVCPARM *pParm, void **ppv, uint32_t *pcb)
{
    if (pParm->type == VBOX_HGCM_SVC_PARM_PTR)
    {
        *ppv = pParm->u.pointer.addr;
        *pcb = pParm->u.pointer.size;
        return VINF_SUCCESS;
    }

    return VERR_INVALID_PARAMETER;
}

/** Set a uint32_t value to an HGCM parameter structure */
static void VBoxHGCMParmUInt32Set (VBOXHGCMSVCPARM *pParm, uint32_t u32)
{
    pParm->type = VBOX_HGCM_SVC_PARM_32BIT;
    pParm->u.uint32 = u32;
}


/** Set a uint64_t value to an HGCM parameter structure */
static void VBoxHGCMParmUInt64Set (VBOXHGCMSVCPARM *pParm, uint64_t u64)
{
    pParm->type = VBOX_HGCM_SVC_PARM_64BIT;
    pParm->u.uint64 = u64;
}

namespace guestProp {

/**
 * Class containing the shared information service functionality.
 */
class Service : public stdx::non_copyable
{
private:
    /** Type definition for use in callback functions */
    typedef Service SELF;
    /** HGCM helper functions. */
    PVBOXHGCMSVCHELPERS mpHelpers;
    /** Pointer to our configuration values node. */
    PCFGMNODE mpValueNode;
    /** Pointer to our configuration timestamps node. */
    PCFGMNODE mpTimestampNode;
    /** Pointer to our configuration flags node. */
    PCFGMNODE mpFlagsNode;
    /** @todo we should have classes for thread and request handler thread */
    /** Queue of outstanding property change notifications */
    RTREQQUEUE *mReqQueue;
    /** Thread for processing the request queue */
    RTTHREAD mReqThread;
    /** Tell the thread that it should exit */
    bool mfExitThread;
    /** Callback function supplied by the host for notification of updates
     * to properties */
    PFNHGCMSVCEXT mpfnHostCallback;
    /** User data pointer to be supplied to the host callback function */
    void *mpvHostData;

public:
    explicit Service(PVBOXHGCMSVCHELPERS pHelpers)
        : mpHelpers(pHelpers), mpValueNode(NULL), mpTimestampNode(NULL),
          mpFlagsNode(NULL), mfExitThread(false), mpfnHostCallback(NULL),
          mpvHostData(NULL)
    {
        int rc = RTReqCreateQueue(&mReqQueue);
        rc = RTThreadCreate(&mReqThread, reqThreadFn, this, 0, RTTHREADTYPE_MSG_PUMP,
                                RTTHREADFLAGS_WAITABLE, "GuestPropReq");
        if (RT_FAILURE(rc))
            throw rc;
    }

    /**
     * @copydoc VBOXHGCMSVCHELPERS::pfnUnload
     * Simply deletes the service object
     */
    static DECLCALLBACK(int) svcUnload (void *pvService)
    {
        AssertLogRelReturn(VALID_PTR(pvService), VERR_INVALID_PARAMETER);
        SELF *pSelf = reinterpret_cast<SELF *>(pvService);
        int rc = pSelf->uninit();
        AssertRC(rc);
        if (RT_SUCCESS(rc))
            delete pSelf;
        return rc;
    }

    /**
     * @copydoc VBOXHGCMSVCHELPERS::pfnConnect
     * Stub implementation of pfnConnect and pfnDisconnect.
     */
    static DECLCALLBACK(int) svcConnectDisconnect (void * /* pvService */,
                                                   uint32_t /* u32ClientID */,
                                                   void * /* pvClient */)
    {
        return VINF_SUCCESS;
    }

    /**
     * @copydoc VBOXHGCMSVCHELPERS::pfnCall
     * Wraps to the call member function
     */
    static DECLCALLBACK(void) svcCall (void * pvService,
                                       VBOXHGCMCALLHANDLE callHandle,
                                       uint32_t u32ClientID,
                                       void *pvClient,
                                       uint32_t u32Function,
                                       uint32_t cParms,
                                       VBOXHGCMSVCPARM paParms[])
    {
        AssertLogRelReturnVoid(VALID_PTR(pvService));
        SELF *pSelf = reinterpret_cast<SELF *>(pvService);
        pSelf->call(callHandle, u32ClientID, pvClient, u32Function, cParms, paParms);
    }

    /**
     * @copydoc VBOXHGCMSVCHELPERS::pfnHostCall
     * Wraps to the hostCall member function
     */
    static DECLCALLBACK(int) svcHostCall (void *pvService,
                                          uint32_t u32Function,
                                          uint32_t cParms,
                                          VBOXHGCMSVCPARM paParms[])
    {
        AssertLogRelReturn(VALID_PTR(pvService), VERR_INVALID_PARAMETER);
        SELF *pSelf = reinterpret_cast<SELF *>(pvService);
        return pSelf->hostCall(u32Function, cParms, paParms);
    }

    /**
     * @copydoc VBOXHGCMSVCHELPERS::pfnRegisterExtension
     * Installs a host callback for notifications of property changes.
     */
    static DECLCALLBACK(int) svcRegisterExtension (void *pvService,
                                                   PFNHGCMSVCEXT pfnExtension,
                                                   void *pvExtension)
    {
        AssertLogRelReturn(VALID_PTR(pvService), VERR_INVALID_PARAMETER);
        SELF *pSelf = reinterpret_cast<SELF *>(pvService);
        pSelf->mpfnHostCallback = pfnExtension;
        pSelf->mpvHostData = pvExtension;
        return VINF_SUCCESS;
    }
private:
    static DECLCALLBACK(int) reqThreadFn(RTTHREAD ThreadSelf, void *pvUser);
    int validateName(const char *pszName, uint32_t cbName);
    int validateValue(char *pszValue, uint32_t cbValue);
    int getPropValue(uint32_t cParms, VBOXHGCMSVCPARM paParms[]);
    int getProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[]);
    int setProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest);
    int delProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest);
    int enumProps(uint32_t cParms, VBOXHGCMSVCPARM paParms[]);
    void notifyHost(const char *pszProperty);
    static DECLCALLBACK(int) reqNotify(PFNHGCMSVCEXT pfnCallback,
                                       void *pvData, char *pszName,
                                       char *pszValue, uint32_t u32TimeHigh,
                                       uint32_t u32TimeLow, char *pszFlags);
    /**
     * Empty request function for terminating the request thread.
     * @returns VINF_EOF to cause the request processing function to return
     * @todo    return something more appropriate
     */
    static DECLCALLBACK(int) reqVoid() { return VINF_EOF; }

    void call (VBOXHGCMCALLHANDLE callHandle, uint32_t u32ClientID,
               void *pvClient, uint32_t eFunction, uint32_t cParms,
               VBOXHGCMSVCPARM paParms[]);
    int hostCall (uint32_t eFunction, uint32_t cParms, VBOXHGCMSVCPARM paParms[]);
    int uninit ();
};


/**
 * Thread function for processing the request queue
 * @copydoc FNRTTHREAD
 */
DECLCALLBACK(int) Service::reqThreadFn(RTTHREAD ThreadSelf, void *pvUser)
{
    SELF *pSelf = reinterpret_cast<SELF *>(pvUser);
    while (!pSelf->mfExitThread)
        RTReqProcess(pSelf->mReqQueue, RT_INDEFINITE_WAIT);
    return VINF_SUCCESS;
}


/**
 * Checking that the name passed by the guest fits our criteria for a
 * property name.
 *
 * @returns IPRT status code
 * @param   pszName   the name passed by the guest
 * @param   cbName    the number of bytes pszName points to, including the
 *                    terminating '\0'
 * @thread  HGCM
 */
int Service::validateName(const char *pszName, uint32_t cbName)
{
    LogFlowFunc(("cbName=%d\n", cbName));

    /*
     * Validate the name, checking that it's proper UTF-8 and has
     * a string terminator.
     */
    int rc = RTStrValidateEncodingEx(pszName, RT_MIN(cbName, (uint32_t) MAX_NAME_LEN),
                                     RTSTR_VALIDATE_ENCODING_ZERO_TERMINATED);
    if (RT_SUCCESS(rc) && (cbName < 2))
        rc = VERR_INVALID_PARAMETER;

    LogFlowFunc(("returning %Rrc\n", rc));
    return rc;
}


/**
 * Check that the data passed by the guest fits our criteria for the value of
 * a guest property.
 *
 * @returns IPRT status code
 * @param   pszValue  the value to store in the property
 * @param   cbValue   the number of bytes in the buffer pszValue points to
 * @thread  HGCM
 */
int Service::validateValue(char *pszValue, uint32_t cbValue)
{
    LogFlowFunc(("cbValue=%d\n", cbValue));

    /*
     * Validate the value, checking that it's proper UTF-8 and has
     * a string terminator. Don't pass a 0 length request to the
     * validator since it won't find any '\0' then.
     */
    int rc = VINF_SUCCESS;
    if (cbValue)
        rc = RTStrValidateEncodingEx(pszValue, RT_MIN(cbValue, (uint32_t) MAX_VALUE_LEN),
                                     RTSTR_VALIDATE_ENCODING_ZERO_TERMINATED);
    if (RT_SUCCESS(rc))
        LogFlow(("    pszValue=%s\n", cbValue > 0 ? pszValue : NULL));
    LogFlowFunc(("returning %Rrc\n", rc));
    return rc;
}


/**
 * Retrieve a value from the property registry by name, checking the validity
 * of the arguments passed.  If the guest has not allocated enough buffer
 * space for the value then we return VERR_OVERFLOW and set the size of the
 * buffer needed in the "size" HGCM parameter.  If the name was not found at
 * all, we return VERR_NOT_FOUND.
 *
 * @returns iprt status value
 * @param   cParms  the number of HGCM parameters supplied
 * @param   paParms the array of HGCM parameters
 * @thread  HGCM
 */
int Service::getPropValue(uint32_t cParms, VBOXHGCMSVCPARM paParms[])
{
    int rc = VINF_SUCCESS;
    char *pszName, *pszValue;
    uint32_t cbName, cbValue;
    size_t cbValueActual;

    LogFlowThisFunc(("\n"));
    AssertReturn(VALID_PTR(mpValueNode), VERR_WRONG_ORDER);  /* a.k.a. VERR_NOT_INITIALIZED */
    if (   (cParms != 3)  /* Hardcoded value as the next lines depend on it. */
        || (paParms[0].type != VBOX_HGCM_SVC_PARM_PTR)   /* name */
        || (paParms[1].type != VBOX_HGCM_SVC_PARM_PTR)   /* value */
       )
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[0], (void **) &pszName, &cbName);
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[1], (void **) &pszValue, &cbValue);
    if (RT_SUCCESS(rc))
        rc = validateName(pszName, cbName);
    if (RT_SUCCESS(rc))
        rc = CFGMR3QuerySize(mpValueNode, pszName, &cbValueActual);
    if (RT_SUCCESS(rc))
        VBoxHGCMParmUInt32Set(&paParms[2], cbValueActual);
    if (RT_SUCCESS(rc) && (cbValueActual > cbValue))
        rc = VERR_BUFFER_OVERFLOW;
    if (RT_SUCCESS(rc))
        rc = CFGMR3QueryString(mpValueNode, pszName, pszValue, cbValue);
    if (RT_SUCCESS(rc))
        Log2(("Queried string %s, rc=%Rrc, value=%.*s\n", pszName, rc, cbValue, pszValue));
    else if (VERR_CFGM_VALUE_NOT_FOUND == rc)
        rc = VERR_NOT_FOUND;
    LogFlowThisFunc(("rc = %Rrc\n", rc));
    return rc;
}


/**
 * Retrieve a value from the property registry by name, checking the validity
 * of the arguments passed.  If the guest has not allocated enough buffer
 * space for the value then we return VERR_OVERFLOW and set the size of the
 * buffer needed in the "size" HGCM parameter.  If the name was not found at
 * all, we return VERR_NOT_FOUND.
 *
 * @returns iprt status value
 * @param   cParms  the number of HGCM parameters supplied
 * @param   paParms the array of HGCM parameters
 * @thread  HGCM
 */
int Service::getProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[])
{
    int rc = VINF_SUCCESS;
    char *pszName, *pchBuf;
    uint32_t cchName, cchBuf;
    size_t cchValue, cchFlags, cchBufActual;
    char szFlags[MAX_FLAGS_LEN];
    uint32_t fFlags;

    /*
     * Get and validate the parameters
     */
    LogFlowThisFunc(("\n"));
    AssertReturn(VALID_PTR(mpValueNode), VERR_WRONG_ORDER);  /* a.k.a. VERR_NOT_INITIALIZED */
    if (   (cParms != 4)  /* Hardcoded value as the next lines depend on it. */
        || (paParms[0].type != VBOX_HGCM_SVC_PARM_PTR)    /* name */
        || (paParms[1].type != VBOX_HGCM_SVC_PARM_PTR)    /* buffer */
       )
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[0], (void **) &pszName, &cchName);
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[1], (void **) &pchBuf, &cchBuf);
    if (RT_SUCCESS(rc))
        rc = validateName(pszName, cchName);

    /*
     * Read and set the values we will return
     */

    /* Get the value size */
    if (RT_SUCCESS(rc))
        rc = CFGMR3QuerySize(mpValueNode, pszName, &cchValue);
    /* Get the flags and their size */
    if (RT_SUCCESS(rc))
        CFGMR3QueryU32(mpFlagsNode, pszName, (uint32_t *)&fFlags);
    if (RT_SUCCESS(rc))
        rc = writeFlags(fFlags, szFlags);
    if (RT_SUCCESS(rc))
        cchFlags = strlen(szFlags);
    /* Check that the buffer is big enough */
    if (RT_SUCCESS(rc))
    {
        cchBufActual = cchValue + cchFlags;
        VBoxHGCMParmUInt32Set(&paParms[3], cchBufActual);
    }
    if (RT_SUCCESS(rc) && (cchBufActual > cchBuf))
        rc = VERR_BUFFER_OVERFLOW;
    /* Write the value */
    if (RT_SUCCESS(rc))
        rc = CFGMR3QueryString(mpValueNode, pszName, pchBuf, cchBuf);
    /* Write the flags */
    if (RT_SUCCESS(rc))
        strcpy(pchBuf + cchValue, szFlags);
    /* Timestamp */
    uint64_t u64Timestamp = 0;
    if (RT_SUCCESS(rc))
        CFGMR3QueryU64(mpTimestampNode, pszName, &u64Timestamp);
    VBoxHGCMParmUInt64Set(&paParms[2], u64Timestamp);

    /*
     * Done!  Do exit logging and return.
     */
    if (RT_SUCCESS(rc))
        Log2(("Queried string %S, value=%.*S, timestamp=%lld, flags=%.*S\n",
              pszName, cchValue, pchBuf, u64Timestamp, cchFlags,
              pchBuf + cchValue));
    else if (VERR_CFGM_VALUE_NOT_FOUND == rc)
        rc = VERR_NOT_FOUND;
    LogFlowThisFunc(("rc = %Rrc\n", rc));
    return rc;
}


/**
 * Set a value in the property registry by name, checking the validity
 * of the arguments passed.
 *
 * @returns iprt status value
 * @param   cParms  the number of HGCM parameters supplied
 * @param   paParms the array of HGCM parameters
 * @thread  HGCM
 */
int Service::setProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest)
{
    int rc = VINF_SUCCESS;
    char *pszName, *pszValue;
    uint32_t cchName, cchValue;
    uint32_t fFlags = NILFLAG;

    LogFlowThisFunc(("\n"));
    AssertReturn(VALID_PTR(mpValueNode), VERR_WRONG_ORDER);  /* a.k.a. VERR_NOT_INITIALIZED */
    /*
     * First of all, make sure that we won't exceed the maximum number of properties.
     */
    {
        unsigned cChildren = 0;
        for (PCFGMNODE pChild = CFGMR3GetFirstChild(mpValueNode); pChild != 0; pChild = CFGMR3GetNextChild(pChild))
            ++cChildren;
        if (cChildren >= MAX_PROPS)
            rc = VERR_TOO_MUCH_DATA;
    }
    /*
     * General parameter correctness checking.
     */
    if (   RT_SUCCESS(rc)
        && (   (cParms < 2) || (cParms > 4)  /* Hardcoded value as the next lines depend on it. */
            || (paParms[0].type != VBOX_HGCM_SVC_PARM_PTR)   /* name */
            || (paParms[1].type != VBOX_HGCM_SVC_PARM_PTR)   /* value */
            || ((3 == cParms) && (paParms[2].type != VBOX_HGCM_SVC_PARM_PTR))  /* flags */
           )
       )
        rc = VERR_INVALID_PARAMETER;
    /*
     * Check the values passed in the parameters for correctness.
     */
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[0], (void **) &pszName, &cchName);
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[1], (void **) &pszValue, &cchValue);
    if (RT_SUCCESS(rc))
        rc = validateName(pszName, cchName);
    if (RT_SUCCESS(rc))
        rc = validateValue(pszValue, cchValue);

    /*
     * If the property already exists, check its flags to see if we are allowed
     * to change it.
     */
    if (RT_SUCCESS(rc))
    {
        CFGMR3QueryU32(mpFlagsNode, pszName, &fFlags);  /* Failure is no problem here. */
        if (   (isGuest && (fFlags & RDONLYGUEST))
            || (!isGuest && (fFlags & RDONLYHOST))
           )
            rc = VERR_PERMISSION_DENIED;
    }

    /*
     * Check whether the user supplied flags (if any) are valid.
     */
    if (RT_SUCCESS(rc) && (3 == cParms))
    {
        char *pszFlags;
        uint32_t cchFlags;
        rc = VBoxHGCMParmPtrGet(&paParms[2], (void **) &pszFlags, &cchFlags);
        if (RT_SUCCESS(rc))
            rc = validateFlags(pszFlags, &fFlags);
    }
    /*
     * Set the actual value
     */
    if (RT_SUCCESS(rc))
    {
        RTTIMESPEC time;
        CFGMR3RemoveValue(mpValueNode, pszName);
        CFGMR3RemoveValue(mpTimestampNode, pszName);
        CFGMR3RemoveValue(mpFlagsNode, pszName);
        rc = CFGMR3InsertString(mpValueNode, pszName, pszValue);
        if (RT_SUCCESS(rc))
            rc = CFGMR3InsertInteger(mpTimestampNode, pszName,
                                     RTTimeSpecGetNano(RTTimeNow(&time)));
        if (RT_SUCCESS(rc))
            rc = CFGMR3InsertInteger(mpFlagsNode, pszName, fFlags);
        /* If anything goes wrong, make sure that we leave a clean state
         * behind. */
        if (RT_FAILURE(rc))
        {
            CFGMR3RemoveValue(mpValueNode, pszName);
            CFGMR3RemoveValue(mpTimestampNode, pszName);
            CFGMR3RemoveValue(mpFlagsNode, pszName);
        }
    }
    /*
     * Send a notification to the host and return.
     */
    if (RT_SUCCESS(rc))
    {
        if (isGuest)
            notifyHost(pszName);
        Log2(("Set string %s, rc=%Rrc, value=%s\n", pszName, rc, pszValue));
    }
    LogFlowThisFunc(("rc = %Rrc\n", rc));
    return rc;
}


/**
 * Remove a value in the property registry by name, checking the validity
 * of the arguments passed.
 *
 * @returns iprt status value
 * @param   cParms  the number of HGCM parameters supplied
 * @param   paParms the array of HGCM parameters
 * @thread  HGCM
 */
int Service::delProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest)
{
    int rc = VINF_SUCCESS;
    char *pszName;
    uint32_t cbName;

    LogFlowThisFunc(("\n"));
    AssertReturn(VALID_PTR(mpValueNode), VERR_WRONG_ORDER);  /* a.k.a. VERR_NOT_INITIALIZED */

    /*
     * Check the user-supplied parameters.
     */
    if (   (cParms != 1)  /* Hardcoded value as the next lines depend on it. */
        || (paParms[0].type != VBOX_HGCM_SVC_PARM_PTR)   /* name */
       )
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[0], (void **) &pszName, &cbName);
    if (RT_SUCCESS(rc))
        rc = validateName(pszName, cbName);

    /*
     * If the property already exists, check its flags to see if we are allowed
     * to change it.
     */
    if (RT_SUCCESS(rc))
    {
        uint32_t fFlags = NILFLAG;
        CFGMR3QueryU32(mpFlagsNode, pszName, &fFlags);  /* Failure is no problem here. */
        if (   (isGuest && (fFlags & RDONLYGUEST))
            || (!isGuest && (fFlags & RDONLYHOST))
           )
            rc = VERR_PERMISSION_DENIED;
    }

    /*
     * And delete the property if all is well.
     */
    if (RT_SUCCESS(rc))
    {
        CFGMR3RemoveValue(mpValueNode, pszName);
        if (isGuest)
            notifyHost(pszName);
    }
    LogFlowThisFunc(("rc = %Rrc\n", rc));
    return rc;
}

/**
 * Enumerate guest properties by mask, checking the validity
 * of the arguments passed.
 *
 * @returns iprt status value
 * @param   cParms  the number of HGCM parameters supplied
 * @param   paParms the array of HGCM parameters
 * @thread  HGCM
 */
int Service::enumProps(uint32_t cParms, VBOXHGCMSVCPARM paParms[])
{
    /* We reallocate the temporary buffer in which we build up our array in
     * increments of size BLOCK: */
    enum
    {
        /* Calculate the increment, not yet rounded down */
        BLOCKINCRFULL = (MAX_NAME_LEN + MAX_VALUE_LEN + MAX_FLAGS_LEN + 2048),
        /* And this is the increment after rounding */
        BLOCKINCR = BLOCKINCRFULL - BLOCKINCRFULL % 1024
    };
    int rc = VINF_SUCCESS;

/*
 * Get the HGCM function arguments.
 */
    char *paszPatterns = NULL, *pchBuf = NULL;
    uint32_t cchPatterns = 0, cchBuf = 0;
    LogFlowThisFunc(("\n"));
    AssertReturn(VALID_PTR(mpValueNode), VERR_WRONG_ORDER);  /* a.k.a. VERR_NOT_INITIALIZED */
    if (   (cParms != 3)  /* Hardcoded value as the next lines depend on it. */
        || (paParms[0].type != VBOX_HGCM_SVC_PARM_PTR)   /* patterns */
        || (paParms[1].type != VBOX_HGCM_SVC_PARM_PTR)   /* return buffer */
       )
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[0], (void **) &paszPatterns, &cchPatterns);
    if (RT_SUCCESS(rc) && cchPatterns > MAX_PATTERN_LEN)
        rc = VERR_TOO_MUCH_DATA;
    if (RT_SUCCESS(rc))
        rc = VBoxHGCMParmPtrGet(&paParms[1], (void **) &pchBuf, &cchBuf);

/*
 * First repack the patterns into the format expected by RTStrSimplePatternMatch()
 */
    bool matchAll = false;
    char pszPatterns[MAX_PATTERN_LEN];
    if (   (NULL == paszPatterns)
        || (cchPatterns < 2)  /* An empty pattern string means match all */
       )
        matchAll = true;
    else
    {
        for (unsigned i = 0; i < cchPatterns - 1; ++i)
            if (paszPatterns[i] != '\0')
                pszPatterns[i] = paszPatterns[i];
            else
                pszPatterns[i] = '|';
        pszPatterns[cchPatterns - 1] = '\0';
    }

/*
 * Next enumerate all values in the current node into a temporary buffer.
 */
    RTMemAutoPtr<char> TmpBuf;
    uint32_t cchTmpBuf = 0, iTmpBuf = 0;
    PCFGMLEAF pLeaf = CFGMR3GetFirstValue(mpValueNode);
    while ((pLeaf != NULL) && RT_SUCCESS(rc))
    {
        /* Reallocate the buffer if it has got too tight */
        if (iTmpBuf + BLOCKINCR > cchTmpBuf)
        {
            cchTmpBuf += BLOCKINCR * 2;
            if (!TmpBuf.realloc(cchTmpBuf))
                rc = VERR_NO_MEMORY;
        }
        /* Fetch the name into the buffer and if it matches one of the
         * patterns, add its value and an empty timestamp and flags.  If it
         * doesn't match, we simply overwrite it in the buffer. */
        if (RT_SUCCESS(rc))
            rc = CFGMR3GetValueName(pLeaf, &TmpBuf[iTmpBuf], cchTmpBuf - iTmpBuf);
        /* Only increment the buffer offest if the name matches, otherwise we
         * overwrite it next iteration. */
        if (   RT_SUCCESS(rc)
            && (   matchAll
                || RTStrSimplePatternMultiMatch(pszPatterns, RTSTR_MAX,
                                                &TmpBuf[iTmpBuf], RTSTR_MAX,
                                                NULL)
               )
           )
        {
            const char *pszName = &TmpBuf[iTmpBuf];
            /* Get value */
            iTmpBuf += strlen(&TmpBuf[iTmpBuf]) + 1;
            rc = CFGMR3QueryString(mpValueNode, pszName, &TmpBuf[iTmpBuf],
                                   cchTmpBuf - iTmpBuf);
            if (RT_SUCCESS(rc))
            {
                /* Get timestamp */
                iTmpBuf += strlen(&TmpBuf[iTmpBuf]) + 1;
                uint64_t u64Timestamp = 0;
                CFGMR3QueryU64(mpTimestampNode, pszName, &u64Timestamp);
                iTmpBuf += RTStrFormatNumber(&TmpBuf[iTmpBuf], u64Timestamp,
                                             10, 0, 0, 0) + 1;
                /* Get flags */
                uint32_t fFlags = NILFLAG;
                CFGMR3QueryU32(mpFlagsNode, pszName, &fFlags);
                TmpBuf[iTmpBuf] = '\0';  /* Bad (== in)sanity, will be fixed. */
                writeFlags(fFlags, &TmpBuf[iTmpBuf]);
                iTmpBuf += strlen(&TmpBuf[iTmpBuf]) + 1;
            }
        }
        if (RT_SUCCESS(rc))
            pLeaf = CFGMR3GetNextValue(pLeaf);
    }
    if (RT_SUCCESS(rc))
    {
        /* The terminator.  We *do* have space left for this. */
        TmpBuf[iTmpBuf] = '\0';
        TmpBuf[iTmpBuf + 1] = '\0';
        TmpBuf[iTmpBuf + 2] = '\0';
        TmpBuf[iTmpBuf + 3] = '\0';
        iTmpBuf += 4;
        VBoxHGCMParmUInt32Set(&paParms[2], iTmpBuf);
        /* Copy the memory if it fits into the guest buffer */
        if (iTmpBuf <= cchBuf)
            memcpy(pchBuf, TmpBuf.get(), iTmpBuf);
        else
            rc = VERR_BUFFER_OVERFLOW;
    }
    return rc;
}

/**
 * Notify the service owner that a property has been added/deleted/changed
 * @param pszProperty the name of the property which has changed
 * @note this call allocates memory which the reqNotify request is expected to
 *       free again, using RTStrFree().
 *
 * @thread  HGCM service
 */
void Service::notifyHost(const char *pszProperty)
{
    char szValue[MAX_VALUE_LEN];
    uint64_t u64Timestamp = 0;
    uint32_t fFlags = NILFLAG;
    char szFlags[MAX_FLAGS_LEN];
    char *pszName = NULL, *pszValue = NULL, *pszFlags = NULL;

    AssertPtr(mpValueNode);
    if (NULL == mpfnHostCallback)
        return;  /* Nothing to do. */
    int rc = CFGMR3QueryString(mpValueNode, pszProperty, szValue,
                               sizeof(szValue));
    /*
     * First case: if the property exists then send the host its current value
     */
    if (rc != VERR_CFGM_VALUE_NOT_FOUND)
    {
        if (RT_SUCCESS(rc))
            rc = CFGMR3QueryU64(mpTimestampNode, pszProperty, &u64Timestamp);
        if (RT_SUCCESS(rc))
            rc = CFGMR3QueryU32(mpFlagsNode, pszProperty, &fFlags);
        if (RT_SUCCESS(rc))
            rc = writeFlags(fFlags, szFlags);
        if (RT_SUCCESS(rc))
            rc = RTStrDupEx(&pszName, pszProperty);
        if (RT_SUCCESS(rc))
            rc = RTStrDupEx(&pszValue, szValue);
        if (RT_SUCCESS(rc))
            rc = RTStrDupEx(&pszFlags, szFlags);
        if (RT_SUCCESS(rc))
            rc = RTReqCallEx(mReqQueue, NULL, 0, RTREQFLAGS_NO_WAIT,
                             (PFNRT)Service::reqNotify, 7, mpfnHostCallback,
                             mpvHostData, pszName, pszValue,
                             (uint32_t) RT_HIDWORD(u64Timestamp),
                             (uint32_t) RT_LODWORD(u64Timestamp), pszFlags);
        if (RT_FAILURE(rc)) /* clean up */
        {
            RTStrFree(pszName);
            RTStrFree(pszValue);
            RTStrFree(pszFlags);
        }
    }
    else
    /*
     * Second case: if the property does not exist then send the host an empty
     * value
     */
    {
        rc = RTStrDupEx(&pszName, pszProperty);
        if (RT_SUCCESS(rc))
            rc = RTReqCallEx(mReqQueue, NULL, 0, RTREQFLAGS_NO_WAIT,
                             (PFNRT)Service::reqNotify, 7, mpfnHostCallback,
                             mpvHostData, pszName, NULL, 0, 0, NULL);
    }
    if (RT_FAILURE(rc)) /* clean up if we failed somewhere */
    {
        RTStrFree(pszName);
        RTStrFree(pszValue);
        RTStrFree(pszFlags);
    }
}

/**
 * Notify the service owner that a property has been added/deleted/changed.
 * asynchronous part.
 * @param pszProperty the name of the property which has changed
 * @note this call allocates memory which the reqNotify request is expected to
 *       free again, using RTStrFree().
 *
 * @thread  request thread
 */
int Service::reqNotify(PFNHGCMSVCEXT pfnCallback, void *pvData,
                       char *pszName, char *pszValue, uint32_t u32TimeHigh,
                       uint32_t u32TimeLow, char *pszFlags)
{
    HOSTCALLBACKDATA HostCallbackData;
    HostCallbackData.u32Magic     = HOSTCALLBACKMAGIC;
    HostCallbackData.pcszName     = pszName;
    HostCallbackData.pcszValue    = pszValue;
    HostCallbackData.u64Timestamp = RT_MAKE_U64(u32TimeLow, u32TimeHigh);
    HostCallbackData.pcszFlags    = pszFlags;
    AssertRC(pfnCallback(pvData, 0, reinterpret_cast<void *>(&HostCallbackData),
                         sizeof(HostCallbackData)));
    RTStrFree(pszName);
    RTStrFree(pszValue);
    RTStrFree(pszFlags);
    return VINF_SUCCESS;
}


/**
 * Handle an HGCM service call.
 * @copydoc VBOXHGCMSVCFNTABLE::pfnCall
 * @note    All functions which do not involve an unreasonable delay will be
 *          handled synchronously.  If needed, we will add a request handler
 *          thread in future for those which do.
 *
 * @thread  HGCM
 */
void Service::call (VBOXHGCMCALLHANDLE callHandle, uint32_t u32ClientID,
                    void * /* pvClient */, uint32_t eFunction, uint32_t cParms,
                    VBOXHGCMSVCPARM paParms[])
{
    int rc = VINF_SUCCESS;
    bool fCallSync = true;

    LogFlowFunc(("u32ClientID = %d, fn = %d, cParms = %d, pparms = %d\n",
                 u32ClientID, eFunction, cParms, paParms));

    switch (eFunction)
    {
        /* The guest wishes to read a property */
        case GET_PROP:
            LogFlowFunc(("GET_PROP\n"));
            rc = getProperty(cParms, paParms);
            break;

        /* The guest wishes to set a property */
        case SET_PROP:
            LogFlowFunc(("SET_PROP\n"));
            rc = setProperty(cParms, paParms, true);
            break;

        /* The guest wishes to set a property value */
        case SET_PROP_VALUE:
            LogFlowFunc(("SET_PROP_VALUE\n"));
            rc = setProperty(cParms, paParms, true);
            break;

        /* The guest wishes to remove a configuration value */
        case DEL_PROP:
            LogFlowFunc(("DEL_PROP\n"));
            rc = delProperty(cParms, paParms, true);
            break;

        /* The guest wishes to enumerate all properties */
        case ENUM_PROPS:
            LogFlowFunc(("ENUM_PROPS\n"));
            rc = enumProps(cParms, paParms);
            break;
        default:
            rc = VERR_NOT_IMPLEMENTED;
    }
    if (fCallSync)
    {
        LogFlowFunc(("rc = %Rrc\n", rc));
        mpHelpers->pfnCallComplete (callHandle, rc);
    }
}


/**
 * Service call handler for the host.
 * @copydoc VBOXHGCMSVCFNTABLE::pfnHostCall
 * @thread  hgcm
 */
int Service::hostCall (uint32_t eFunction, uint32_t cParms, VBOXHGCMSVCPARM paParms[])
{
    int rc = VINF_SUCCESS;

    LogFlowFunc(("fn = %d, cParms = %d, pparms = %d\n",
                 eFunction, cParms, paParms));

    switch (eFunction)
    {
        /* Set the root CFGM node used.  This should be called when instantiating
         * the service. */
        /* This whole case is due to go away, so I will not clean it up. */
        case SET_CFGM_NODE:
        {
            LogFlowFunc(("SET_CFGM_NODE\n"));

            if (   (cParms != 3)
                || (paParms[0].type != VBOX_HGCM_SVC_PARM_PTR)   /* pValue */
                || (paParms[1].type != VBOX_HGCM_SVC_PARM_PTR)   /* pTimestamp */
                || (paParms[2].type != VBOX_HGCM_SVC_PARM_PTR)   /* pFlags */
               )
                rc = VERR_INVALID_PARAMETER;
            else
            {
                PCFGMNODE pValue = NULL, pTimestamp = NULL, pFlags = NULL;
                uint32_t cbDummy;

                rc = VBoxHGCMParmPtrGet (&paParms[0], (void **) &pValue, &cbDummy);
                if (RT_SUCCESS(rc))
                    rc = VBoxHGCMParmPtrGet (&paParms[1], (void **) &pTimestamp, &cbDummy);
                if (RT_SUCCESS(rc))
                    rc = VBoxHGCMParmPtrGet (&paParms[2], (void **) &pFlags, &cbDummy);
                if (RT_SUCCESS(rc))
                {
                    mpValueNode     = pValue;
                    mpTimestampNode = pTimestamp;
                    mpFlagsNode     = pFlags;
                }
            }
        } break;

        /* The host wishes to read a configuration value */
        case GET_PROP_HOST:
            LogFlowFunc(("GET_PROP_HOST\n"));
            rc = getProperty(cParms, paParms);
            break;

        /* The host wishes to set a configuration value */
        case SET_PROP_HOST:
            LogFlowFunc(("SET_PROP_HOST\n"));
            rc = setProperty(cParms, paParms, false);
            break;

        /* The host wishes to set a configuration value */
        case SET_PROP_VALUE_HOST:
            LogFlowFunc(("SET_PROP_VALUE_HOST\n"));
            rc = setProperty(cParms, paParms, false);
            break;

        /* The host wishes to remove a configuration value */
        case DEL_PROP_HOST:
            LogFlowFunc(("DEL_PROP_HOST\n"));
            rc = delProperty(cParms, paParms, false);
            break;

        /* The host wishes to enumerate all properties */
        case ENUM_PROPS_HOST:
            LogFlowFunc(("ENUM_PROPS\n"));
            rc = enumProps(cParms, paParms);
            break;
        default:
            rc = VERR_NOT_SUPPORTED;
            break;
    }

    LogFlowFunc(("rc = %Vrc\n", rc));
    return rc;
}

int Service::uninit()
{
    int rc;
    unsigned count = 0;

    mfExitThread = true;
    rc = RTReqCallEx(mReqQueue, NULL, 0, RTREQFLAGS_NO_WAIT, (PFNRT)reqVoid, 0);
    if (RT_SUCCESS(rc))
        do
        {
            rc = RTThreadWait(mReqThread, 1000, NULL);
            ++count;
            Assert(RT_SUCCESS(rc) || ((VERR_TIMEOUT == rc) && (count != 5)));
        } while ((VERR_TIMEOUT == rc) && (count < 300));
    if (RT_SUCCESS(rc))
        RTReqDestroyQueue(mReqQueue);
    return rc;
}

} /* namespace guestProp */

using guestProp::Service;

/**
 * @copydoc VBOXHGCMSVCLOAD
 */
extern "C" DECLCALLBACK(DECLEXPORT(int)) VBoxHGCMSvcLoad (VBOXHGCMSVCFNTABLE *ptable)
{
    int rc = VINF_SUCCESS;

    LogFlowFunc(("ptable = %p\n", ptable));

    if (!VALID_PTR(ptable))
    {
        rc = VERR_INVALID_PARAMETER;
    }
    else
    {
        LogFlowFunc(("ptable->cbSize = %d, ptable->u32Version = 0x%08X\n", ptable->cbSize, ptable->u32Version));

        if (   ptable->cbSize != sizeof (VBOXHGCMSVCFNTABLE)
            || ptable->u32Version != VBOX_HGCM_SVC_VERSION)
        {
            rc = VERR_VERSION_MISMATCH;
        }
        else
        {
            std::auto_ptr<Service> apService;
            /* No exceptions may propogate outside. */
            try {
                apService = std::auto_ptr<Service>(new Service(ptable->pHelpers));
            } catch (int rcThrown) {
                rc = rcThrown;
            } catch (...) {
                rc = VERR_UNRESOLVED_ERROR;
            }

            if (RT_SUCCESS(rc))
            {
                /* We do not maintain connections, so no client data is needed. */
                ptable->cbClient = 0;

                ptable->pfnUnload             = Service::svcUnload;
                ptable->pfnConnect            = Service::svcConnectDisconnect;
                ptable->pfnDisconnect         = Service::svcConnectDisconnect;
                ptable->pfnCall               = Service::svcCall;
                ptable->pfnHostCall           = Service::svcHostCall;
                ptable->pfnSaveState          = NULL;  /* The service is stateless by definition, so the */
                ptable->pfnLoadState          = NULL;  /* normal construction done before restoring suffices */
                ptable->pfnRegisterExtension  = Service::svcRegisterExtension;

                /* Service specific initialization. */
                ptable->pvService = apService.release();
            }
        }
    }

    LogFlowFunc(("returning %Rrc\n", rc));
    return rc;
}
