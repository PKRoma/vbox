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

#include <VBox/log.h>
#include <iprt/err.h>
#include <iprt/assert.h>
#include <iprt/string.h>
#include <iprt/mem.h>
#include <iprt/autores.h>
#include <iprt/time.h>
#include <iprt/cpputils.h>
#include <iprt/req.h>
#include <iprt/thread.h>

#include <memory>  /* for auto_ptr */
#include <string>
#include <list>
#include <vector>

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
    /** Structure for holding a property */
    struct Property
    {
        /** The name of the property */
        std::string mName;
        /** The property value */
        std::string mValue;
        /** The timestamp of the property */
        uint64_t mTimestamp;
        /** The property flags */
        uint32_t mFlags;

        /** Default constructor */
        Property() : mName(""), mValue(""), mTimestamp(0), mFlags(NILFLAG) {}
        /** Constructor with const char * */
        Property(const char *pcszName, const char *pcszValue,
                 uint64_t u64Timestamp, uint32_t u32Flags)
            : mName(pcszName), mValue(pcszValue), mTimestamp(u64Timestamp),
              mFlags(u32Flags) {}
        /** Constructor with std::string */
        Property(std::string name, std::string value, uint64_t u64Timestamp,
                 uint32_t u32Flags)
            : mName(name), mValue(value), mTimestamp(u64Timestamp),
              mFlags(u32Flags) {}
    };
    /** The properties list type */
    typedef std::list <Property> PropertyList;
    /** The property list */
    PropertyList mProperties;
    /** The list of property changes for guest notifications */
    PropertyList mGuestNotifications;
    /** Structure for holding an uncompleted guest call */
    struct GuestCall
    {
        /** The call handle */
        VBOXHGCMCALLHANDLE mHandle;
        /** The function that was requested */
        uint32_t mFunction;
        /** The number of parameters */
        uint32_t mcParms;
        /** The parameters themselves */
        VBOXHGCMSVCPARM *mParms;

        /** The standard constructor */
        GuestCall() : mFunction(0), mcParms(0) {}
        /** The normal contructor */
        GuestCall(VBOXHGCMCALLHANDLE aHandle, uint32_t aFunction,
                  uint32_t acParms, VBOXHGCMSVCPARM aParms[])
                  : mHandle(aHandle), mFunction(aFunction), mcParms(acParms),
                  mParms(aParms) {}
    };
    typedef std::vector <GuestCall> CallVector;
    /** The list of outstanding guest notification calls */
    CallVector mGuestWaiters;
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
        : mpHelpers(pHelpers), mfExitThread(false), mpfnHostCallback(NULL),
          mpvHostData(NULL)
    {
        int rc = RTReqCreateQueue(&mReqQueue);
#ifndef VBOX_GUEST_PROP_TEST_NOTHREAD
        if (RT_SUCCESS(rc))
            rc = RTThreadCreate(&mReqThread, reqThreadFn, this, 0,
                                RTTHREADTYPE_MSG_PUMP, RTTHREADFLAGS_WAITABLE,
                                "GuestPropReq");
#endif
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
    int validateValue(const char *pszValue, uint32_t cbValue);
    int setPropertyBlock(uint32_t cParms, VBOXHGCMSVCPARM paParms[]);
    int getProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[]);
    int setProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest);
    int delProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest);
    int enumProps(uint32_t cParms, VBOXHGCMSVCPARM paParms[]);
    int getNotification(VBOXHGCMCALLHANDLE callHandle, uint32_t cParms,
                        VBOXHGCMSVCPARM paParms[], bool canWait);
    void doNotifications(const char *pszProperty, uint64_t u64Timestamp);
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
    int rc = VINF_SUCCESS;
    if (RT_SUCCESS(rc) && (cbName < 2))
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = RTStrValidateEncodingEx(pszName, RT_MIN(cbName, (uint32_t) MAX_NAME_LEN),
                                     RTSTR_VALIDATE_ENCODING_ZERO_TERMINATED);

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
int Service::validateValue(const char *pszValue, uint32_t cbValue)
{
    LogFlowFunc(("cbValue=%d\n", cbValue));

    /*
     * Validate the value, checking that it's proper UTF-8 and has
     * a string terminator.
     */
    int rc = VINF_SUCCESS;
    if (RT_SUCCESS(rc) && (cbValue < 2))
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = RTStrValidateEncodingEx(pszValue, RT_MIN(cbValue, (uint32_t) MAX_VALUE_LEN),
                                     RTSTR_VALIDATE_ENCODING_ZERO_TERMINATED);
    if (RT_SUCCESS(rc))
        LogFlow(("    pszValue=%s\n", cbValue > 0 ? pszValue : NULL));
    LogFlowFunc(("returning %Rrc\n", rc));
    return rc;
}

/**
 * Set a block of properties in the property registry, checking the validity
 * of the arguments passed.
 *
 * @returns iprt status value
 * @param   cParms  the number of HGCM parameters supplied
 * @param   paParms the array of HGCM parameters
 * @thread  HGCM
 */
int Service::setPropertyBlock(uint32_t cParms, VBOXHGCMSVCPARM paParms[])
{
    char **ppNames, **ppValues, **ppFlags;
    uint64_t *pTimestamps;
    uint32_t cbDummy;
    int rc = VINF_SUCCESS;

    /*
     * Get and validate the parameters
     */
    if (   (cParms != 4)
        || RT_FAILURE(paParms[0].getPointer ((void **) &ppNames, &cbDummy))
        || RT_FAILURE(paParms[1].getPointer ((void **) &ppValues, &cbDummy))
        || RT_FAILURE(paParms[2].getPointer ((void **) &pTimestamps, &cbDummy))
        || RT_FAILURE(paParms[3].getPointer ((void **) &ppFlags, &cbDummy))
        )
        rc = VERR_INVALID_PARAMETER;

    /*
     * Add the properties to the end of the list.  If we succeed then we
     * will remove duplicates afterwards.
     */
    /* Remember the last property before we started adding, for rollback or
     * cleanup. */
    PropertyList::iterator itEnd = mProperties.end();
    if (!mProperties.empty())
        --itEnd;
    try
    {
        for (unsigned i = 0; RT_SUCCESS(rc) && ppNames[i] != NULL; ++i)
        {
            uint32_t fFlags;
            if (   !VALID_PTR(ppNames[i])
                || !VALID_PTR(ppValues[i])
                || !VALID_PTR(ppFlags[i])
              )
                rc = VERR_INVALID_POINTER;
            if (RT_SUCCESS(rc))
                rc = validateFlags(ppFlags[i], &fFlags);
            if (RT_SUCCESS(rc))
                mProperties.push_back(Property(ppNames[i], ppValues[i],
                                               pTimestamps[i], fFlags));
        }
    }
    catch (std::bad_alloc)
    {
        rc = VERR_NO_MEMORY;
    }

    /*
     * If all went well then remove the duplicate elements.
     */
    if (RT_SUCCESS(rc) && itEnd != mProperties.end())
    {
        ++itEnd;
        for (unsigned i = 0; ppNames[i] != NULL; ++i)
        {
            bool found = false;
            for (PropertyList::iterator it = mProperties.begin();
                !found && it != itEnd; ++it)
                if (it->mName.compare(ppNames[i]) == 0)
                {
                    found = true;
                    mProperties.erase(it);
                }
        }
    }

    /*
     * If something went wrong then rollback.  This is possible because we
     * haven't deleted anything yet.
     */
    if (RT_FAILURE(rc))
    {
        if (itEnd != mProperties.end())
            ++itEnd;
        mProperties.erase(itEnd, mProperties.end());
    }
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
    const char *pcszName;
    char *pchBuf;
    uint32_t cchName, cchBuf;
    size_t cchFlags, cchBufActual;
    char szFlags[MAX_FLAGS_LEN];
    uint32_t fFlags;

    /*
     * Get and validate the parameters
     */
    LogFlowThisFunc(("\n"));
    if (   cParms != 4  /* Hardcoded value as the next lines depend on it. */
        || RT_FAILURE (paParms[0].getPointer ((const void **) &pcszName, &cchName))    /* name */
        || RT_FAILURE (paParms[1].getPointer ((void **) &pchBuf, &cchBuf))    /* buffer */
       )
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = validateName(pcszName, cchName);

    /*
     * Read and set the values we will return
     */

    /* Get the value size */
    PropertyList::const_iterator it;
    bool found = false;
    if (RT_SUCCESS(rc))
        for (it = mProperties.begin(); it != mProperties.end(); ++it)
            if (it->mName.compare(pcszName) == 0)
            {
                found = true;
                break;
            }
    if (RT_SUCCESS(rc) && !found)
        rc = VERR_NOT_FOUND;
    if (RT_SUCCESS(rc))
        rc = writeFlags(it->mFlags, szFlags);
    if (RT_SUCCESS(rc))
        cchFlags = strlen(szFlags);
    /* Check that the buffer is big enough */
    if (RT_SUCCESS(rc))
    {
        cchBufActual = it->mValue.size() + 1 + cchFlags;
        paParms[3].setUInt32 (cchBufActual);
    }
    if (RT_SUCCESS(rc) && (cchBufActual > cchBuf))
        rc = VERR_BUFFER_OVERFLOW;
    /* Write the value, flags and timestamp */
    if (RT_SUCCESS(rc))
    {
        it->mValue.copy(pchBuf, cchBuf, 0);
        pchBuf[it->mValue.size()] = '\0'; /* Terminate the value */
        strcpy(pchBuf + it->mValue.size() + 1, szFlags);
        paParms[2].setUInt64 (it->mTimestamp);
    }

    /*
     * Done!  Do exit logging and return.
     */
    if (RT_SUCCESS(rc))
        Log2(("Queried string %s, value=%s, timestamp=%lld, flags=%s\n",
              pcszName, it->mValue.c_str(), it->mTimestamp, szFlags));
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
 * @param   isGuest is this call coming from the guest (or the host)?
 * @throws  std::bad_alloc  if an out of memory condition occurs
 * @thread  HGCM
 */
int Service::setProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest)
{
    int rc = VINF_SUCCESS;
    const char *pcszName, *pcszValue, *pcszFlags = NULL;
    uint32_t cchName, cchValue, cchFlags = 0;
    uint32_t fFlags = NILFLAG;
    RTTIMESPEC time;
    uint64_t u64TimeNano = RTTimeSpecGetNano(RTTimeNow(&time));

    LogFlowThisFunc(("\n"));
    /*
     * First of all, make sure that we won't exceed the maximum number of properties.
     */
    if (mProperties.size() >= MAX_PROPS)
        rc = VERR_TOO_MUCH_DATA;

    /*
     * General parameter correctness checking.
     */
    if (   RT_SUCCESS(rc)
        && (   (cParms < 2) || (cParms > 3)  /* Hardcoded value as the next lines depend on it. */
            || RT_FAILURE(paParms[0].getPointer ((const void **) &pcszName,
                                                 &cchName)) /* name */
            || RT_FAILURE(paParms[1].getPointer ((const void **) &pcszValue,
                                                 &cchValue)) /* value */
            || (   (3 == cParms)
                && RT_FAILURE(paParms[2].getPointer ((const void **) &pcszFlags,
                                                     &cchFlags)) /* flags */
               )
           )
       )
        rc = VERR_INVALID_PARAMETER;

    /*
     * Check the values passed in the parameters for correctness.
     */
    if (RT_SUCCESS(rc))
        rc = validateName(pcszName, cchName);
    if (RT_SUCCESS(rc))
        rc = validateValue(pcszValue, cchValue);
    if ((3 == cParms) && RT_SUCCESS(rc))
        rc = RTStrValidateEncodingEx(pcszFlags, cchFlags,
                                     RTSTR_VALIDATE_ENCODING_ZERO_TERMINATED);
    if ((3 == cParms) && RT_SUCCESS(rc))
        rc = validateFlags(pcszFlags, &fFlags);

    /*
     * If the property already exists, check its flags to see if we are allowed
     * to change it.
     */
    PropertyList::iterator it;
    bool found = false;
    if (RT_SUCCESS(rc))
        for (it = mProperties.begin(); it != mProperties.end(); ++it)
            if (it->mName.compare(pcszName) == 0)
            {
                found = true;
                break;
            }
    if (RT_SUCCESS(rc) && found)
        if (   (isGuest && (it->mFlags & RDONLYGUEST))
            || (!isGuest && (it->mFlags & RDONLYHOST))
           )
            rc = VERR_PERMISSION_DENIED;

    /*
     * Set the actual value
     */
    if (RT_SUCCESS(rc))
    {
        if (found)
        {
            it->mValue = pcszValue;
            it->mTimestamp = u64TimeNano;
            it->mFlags = fFlags;
        }
        else  /* This can throw.  No problem as we have nothing to roll back. */
            mProperties.push_back(Property(pcszName, pcszValue, u64TimeNano, fFlags));
    }

    /*
     * Send a notification to the host and return.
     */
    if (RT_SUCCESS(rc))
    {
        // if (isGuest)  /* Notify the host even for properties that the host
        //                * changed.  Less efficient, but ensures consistency. */
            doNotifications(pcszName, u64TimeNano);
        Log2(("Set string %s, rc=%Rrc, value=%s\n", pcszName, rc, pcszValue));
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
 * @param   isGuest is this call coming from the guest (or the host)?
 * @thread  HGCM
 */
int Service::delProperty(uint32_t cParms, VBOXHGCMSVCPARM paParms[], bool isGuest)
{
    int rc = VINF_SUCCESS;
    const char *pcszName;
    uint32_t cbName;

    LogFlowThisFunc(("\n"));

    /*
     * Check the user-supplied parameters.
     */
    if (   (cParms != 1)  /* Hardcoded value as the next lines depend on it. */
        || RT_FAILURE(paParms[0].getPointer ((const void **) &pcszName,
                                             &cbName))  /* name */
       )
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc))
        rc = validateName(pcszName, cbName);

    /*
     * If the property exists, check its flags to see if we are allowed
     * to change it.
     */
    PropertyList::iterator it;
    bool found = false;
    if (RT_SUCCESS(rc))
        for (it = mProperties.begin(); it != mProperties.end(); ++it)
            if (it->mName.compare(pcszName) == 0)
            {
                found = true;
                break;
            }
    if (RT_SUCCESS(rc) && found)
        if (   (isGuest && (it->mFlags & RDONLYGUEST))
            || (!isGuest && (it->mFlags & RDONLYHOST))
           )
            rc = VERR_PERMISSION_DENIED;

    /*
     * And delete the property if all is well.
     */
    if (RT_SUCCESS(rc) && found)
    {
        RTTIMESPEC time;
        uint64_t u64Timestamp = RTTimeSpecGetNano(RTTimeNow(&time));
        mProperties.erase(it);
        // if (isGuest)  /* Notify the host even for properties that the host
        //                * changed.  Less efficient, but ensures consistency. */
            doNotifications(pcszName, u64Timestamp);
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
    int rc = VINF_SUCCESS;

    /*
     * Get the HGCM function arguments.
     */
    char *pcchPatterns = NULL, *pchBuf = NULL;
    uint32_t cchPatterns = 0, cchBuf = 0;
    LogFlowThisFunc(("\n"));
    if (   (cParms != 3)  /* Hardcoded value as the next lines depend on it. */
        || RT_FAILURE(paParms[0].getPointer ((const void **) &pcchPatterns,
                                             &cchPatterns))  /* patterns */
        || RT_FAILURE(paParms[1].getPointer ((void **) &pchBuf, &cchBuf))  /* return buffer */
       )
        rc = VERR_INVALID_PARAMETER;
    if (RT_SUCCESS(rc) && cchPatterns > MAX_PATTERN_LEN)
        rc = VERR_TOO_MUCH_DATA;

    /*
     * First repack the patterns into the format expected by RTStrSimplePatternMatch()
     */
    bool matchAll = false;
    char pszPatterns[MAX_PATTERN_LEN];
    if (   (NULL == pcchPatterns)
        || (cchPatterns < 2)  /* An empty pattern string means match all */
       )
        matchAll = true;
    else
    {
        for (unsigned i = 0; i < cchPatterns - 1; ++i)
            if (pcchPatterns[i] != '\0')
                pszPatterns[i] = pcchPatterns[i];
            else
                pszPatterns[i] = '|';
        pszPatterns[cchPatterns - 1] = '\0';
    }

    /*
     * Next enumerate into a temporary buffer.  This can throw, but this is
     * not a problem as we have nothing to roll back.
     */
    std::string buffer;
    for (PropertyList::const_iterator it = mProperties.begin();
         RT_SUCCESS(rc) && (it != mProperties.end()); ++it)
    {
        if (   matchAll
            || RTStrSimplePatternMultiMatch(pszPatterns, RTSTR_MAX,
                                            it->mName.c_str(), RTSTR_MAX, NULL)
           )
        {
            char szFlags[MAX_FLAGS_LEN];
            char szTimestamp[256];
            size_t cchTimestamp;
            buffer += it->mName;
            buffer += '\0';
            buffer += it->mValue;
            buffer += '\0';
            cchTimestamp = RTStrFormatNumber(szTimestamp, it->mTimestamp,
                                             10, 0, 0, 0);
            buffer.append(szTimestamp, cchTimestamp);
            buffer += '\0';
            rc = writeFlags(it->mFlags, szFlags);
            if (RT_SUCCESS(rc))
                buffer += szFlags;
            buffer += '\0';
        }
    }
    buffer.append(4, '\0');  /* The final terminators */

    /*
     * Finally write out the temporary buffer to the real one if it is not too
     * small.
     */
    if (RT_SUCCESS(rc))
    {
        paParms[2].setUInt32 (buffer.size());
        /* Copy the memory if it fits into the guest buffer */
        if (buffer.size() <= cchBuf)
            buffer.copy(pchBuf, cchBuf);
        else
            rc = VERR_BUFFER_OVERFLOW;
    }
    return rc;
}

/**
 * Get the next guest notification.
 *
 * @returns iprt status value
 * @param   cParms  the number of HGCM parameters supplied
 * @param   paParms the array of HGCM parameters
 * @param   canWait can this request be enqueued
 * @thread  HGCM
 * @throws  can throw std::bad_alloc if canWait==true
 */
int Service::getNotification(VBOXHGCMCALLHANDLE callHandle, uint32_t cParms,
                             VBOXHGCMSVCPARM paParms[], bool canWait)
{
    int rc = VINF_SUCCESS;
    char *pchBuf;
    uint32_t cchBuf = 0;
    uint64_t u64Timestamp;
    bool warn = false;

    /*
     * Get the HGCM function arguments and perform basic verification.
     */
    LogFlowThisFunc(("\n"));
    if (   (cParms != 3)  /* Hardcoded value as the next lines depend on it. */
        || RT_FAILURE(paParms[0].getUInt64 (&u64Timestamp))  /* timestamp */
        || RT_FAILURE(paParms[1].getPointer ((void **) &pchBuf, &cchBuf))  /* return buffer */
        || cchBuf < 1
       )
        rc = VERR_INVALID_PARAMETER;

    /*
     * Find the change to notify of.
     */
    Property next;
    /* Return the oldest notification if no timestamp was specified. */
    if (RT_SUCCESS(rc) && !mGuestNotifications.empty() && u64Timestamp == 0)
        next = mGuestNotifications.front();
    /* Only search if the guest hasn't seen the most recent notification. */
    else if (   RT_SUCCESS(rc)
             && !mGuestNotifications.empty()
             && mGuestNotifications.back().mTimestamp != u64Timestamp)
    {
        /* We count backwards, as the guest should normally be querying the
         * most recent events. */
        PropertyList::reverse_iterator it = mGuestNotifications.rbegin();
        for ( ;    it != mGuestNotifications.rend()
                && it->mTimestamp != u64Timestamp;
             ++it
            ) {}
        /* Warn if the timestamp was not found. */
        if (it == mGuestNotifications.rend())
            warn = true;
        /* This is a reverse iterator, so --it goes up the list. */
        --it;
        next = *it;
    }

    /*
     * Format the data to write to the buffer.
     */
    std::string buffer;
    if (RT_SUCCESS(rc))
    {
        char szFlags[MAX_FLAGS_LEN];
        rc = writeFlags(next.mFlags, szFlags);
        if (RT_SUCCESS(rc))
        {
            buffer += next.mName;
            buffer += '\0';
            buffer += next.mValue;
            buffer += '\0';
            buffer += szFlags;
            buffer += '\0';
            u64Timestamp = next.mTimestamp;
        }
    }

    /*
     * Write out the data or add the caller to the notification list if there
     * is no notification available.
     */
    if (RT_SUCCESS(rc) && u64Timestamp != 0)
    {
        paParms[0].setUInt64(u64Timestamp);
        paParms[2].setUInt32(buffer.size());
        if (RT_SUCCESS(rc) && buffer.size() <= cchBuf)
            buffer.copy(pchBuf, cchBuf);
        else if (RT_SUCCESS(rc))
            rc = VERR_BUFFER_OVERFLOW;
        if (RT_SUCCESS(rc) && warn)
            rc = VWRN_NOT_FOUND;
    }
    else if (RT_SUCCESS(rc) && canWait)
    {
        mGuestWaiters.push_back(GuestCall(callHandle, GET_NOTIFICATION, cParms,
                                         paParms));
        rc = VINF_HGCM_ASYNC_EXECUTE;
    }
    else if (RT_SUCCESS(rc))
    {
        AssertFailed();
        rc = VERR_INTERNAL_ERROR;
    }
    return rc;
}

/**
 * Notify the service owner and the guest that a property has been
 * added/deleted/changed
 * @param pszProperty  the name of the property which has changed
 * @param u64Timestamp the time at which the change took place
 * @note this call allocates memory which the reqNotify request is expected to
 *       free again, using RTStrFree().
 *
 * @thread  HGCM service
 */
void Service::doNotifications(const char *pszProperty, uint64_t u64Timestamp)
{
    char szFlags[MAX_FLAGS_LEN];
    char *pszName = NULL, *pszValue = NULL, *pszFlags = NULL;
    int rc = VINF_SUCCESS;

    AssertPtrReturnVoid(pszProperty);
    /* Ensure that our timestamp is different to the last one. */
    if (   !mGuestNotifications.empty()
        && u64Timestamp == mGuestNotifications.back().mTimestamp)
        ++u64Timestamp;

    /*
     * Try to find the property.
     */
    PropertyList::const_iterator it;
    bool found = false;
    if (RT_SUCCESS(rc))
        for (it = mProperties.begin(); it != mProperties.end(); ++it)
            if (it->mName.compare(pszProperty) == 0)
            {
                found = true;
                break;
            }

    /*
     * First case: if the property exists then send its current value
     */
    if (found && mpfnHostCallback != NULL)
    {
#ifndef VBOX_GUEST_PROP_TEST_NOTHREAD
        /* Send out a host notification */
        rc = writeFlags(it->mFlags, szFlags);
        if (RT_SUCCESS(rc))
            rc = RTStrDupEx(&pszName, pszProperty);
        if (RT_SUCCESS(rc))
            rc = RTStrDupEx(&pszValue, it->mValue.c_str());
        if (RT_SUCCESS(rc))
            rc = RTStrDupEx(&pszFlags, szFlags);
        if (RT_SUCCESS(rc))
            rc = RTReqCallEx(mReqQueue, NULL, 0, RTREQFLAGS_NO_WAIT,
                             (PFNRT)Service::reqNotify, 7, mpfnHostCallback,
                             mpvHostData, pszName, pszValue,
                             (uint32_t) RT_HIDWORD(u64Timestamp),
                             (uint32_t) RT_LODWORD(u64Timestamp), pszFlags);
#endif /* VBOX_GUEST_PROP_TEST_NOTHREAD not defined */
    }
    if (found)
    {
        /* Add the change to the queue for guest notifications and release
         * waiters */
        if (RT_SUCCESS(rc))
        {
            try
            {
                mGuestNotifications.push_back(*it);
                while (mGuestWaiters.size() > 0)
                {
                    GuestCall call = mGuestWaiters.back();
                    int rc2 = getNotification(call.mHandle, call.mcParms,
                                              call.mParms, false);
                    mpHelpers->pfnCallComplete (call.mHandle, rc2);
                    mGuestWaiters.pop_back();
                }
            }
            catch (std::bad_alloc)
            {
                rc = VERR_NO_MEMORY;
            }
        }
    }

    /*
     * Second case: if the property does not exist then send the host an empty
     * value
     */
    if (!found && mpfnHostCallback != NULL)
    {
#ifndef VBOX_GUEST_PROP_TEST_NOTHREAD
        /* Send out a host notification */
        rc = RTStrDupEx(&pszName, pszProperty);
        if (RT_SUCCESS(rc))
            rc = RTReqCallEx(mReqQueue, NULL, 0, RTREQFLAGS_NO_WAIT,
                             (PFNRT)Service::reqNotify, 7, mpfnHostCallback,
                             mpvHostData, pszName, NULL,
                             (uint32_t) RT_HIDWORD(u64Timestamp),
                             (uint32_t) RT_LODWORD(u64Timestamp), NULL);
#endif /* VBOX_GUEST_PROP_TEST_NOTHREAD not defined */
    }
    if (!found)
    {
        /* Add the change to the queue for guest notifications */
        if (RT_SUCCESS(rc))
        {
            try
            {
                mGuestNotifications.push_back(Property(pszProperty, "",
                                                       u64Timestamp, NILFLAG)
                                             );
            }
            catch (std::bad_alloc)
            {
                rc = VERR_NO_MEMORY;
            }
        }
    }
    if (mGuestNotifications.size() > MAX_GUEST_NOTIFICATIONS)
        mGuestNotifications.pop_front();
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
    LogFlowFunc(("u32ClientID = %d, fn = %d, cParms = %d, pparms = %d\n",
                 u32ClientID, eFunction, cParms, paParms));

    try
    {
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

            /* The guest wishes to get the next property notification */
            case GET_NOTIFICATION:
                LogFlowFunc(("GET_NOTIFICATION\n"));
                rc = getNotification(callHandle, cParms, paParms, true);
                break;

            default:
                rc = VERR_NOT_IMPLEMENTED;
        }
    }
    catch (std::bad_alloc)
    {
        rc = VERR_NO_MEMORY;
    }
    LogFlowFunc(("rc = %Rrc\n", rc));
    if (rc != VINF_HGCM_ASYNC_EXECUTE)
    {
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

    try
    {
        switch (eFunction)
        {
            /* The host wishes to set a block of properties */
            case SET_PROPS_HOST:
                LogFlowFunc(("SET_PROPS_HOST\n"));
                rc = setPropertyBlock(cParms, paParms);
                break;

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
    }
    catch (std::bad_alloc)
    {
        rc = VERR_NO_MEMORY;
    }

    LogFlowFunc(("rc = %Rrc\n", rc));
    return rc;
}

int Service::uninit()
{
    int rc = VINF_SUCCESS;
    unsigned count = 0;

    mfExitThread = true;
#ifndef VBOX_GUEST_PROP_TEST_NOTHREAD
    rc = RTReqCallEx(mReqQueue, NULL, 0, RTREQFLAGS_NO_WAIT, (PFNRT)reqVoid, 0);
    if (RT_SUCCESS(rc))
        do
        {
            rc = RTThreadWait(mReqThread, 1000, NULL);
            ++count;
            Assert(RT_SUCCESS(rc) || ((VERR_TIMEOUT == rc) && (count != 5)));
        } while ((VERR_TIMEOUT == rc) && (count < 300));
#endif /* VBOX_GUEST_PROP_TEST_NOTHREAD not defined */
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
                ptable->pfnSaveState          = NULL;  /* The service is stateless, so the normal */
                ptable->pfnLoadState          = NULL;  /* construction done before restoring suffices */
                ptable->pfnRegisterExtension  = Service::svcRegisterExtension;

                /* Service specific initialization. */
                ptable->pvService = apService.release();
            }
        }
    }

    LogFlowFunc(("returning %Rrc\n", rc));
    return rc;
}
