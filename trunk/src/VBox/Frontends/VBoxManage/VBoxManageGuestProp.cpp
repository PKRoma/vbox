/** @file
 *
 * VBox frontends: VBoxManage (command-line interface), Guest Properties
 */

/*
 * Copyright (C) 2006-2007 Sun Microsystems, Inc.
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/

#include <VBox/com/com.h>
#include <VBox/com/string.h>
#include <VBox/com/array.h>
#include <VBox/com/ErrorInfo.h>

#include <VBox/com/VirtualBox.h>

#include <iprt/stream.h>
#include <VBox/log.h>

#include "VBoxManage.h"

using namespace com;

void usageGuestProperty(void)
{
    RTPrintf("VBoxManage guestproperty    get <vmname>|<uuid>\n"
             "                            <property> [-verbose]\n"
             "\n");
    RTPrintf("VBoxManage guestproperty    set <vmname>|<uuid>\n"
             "                            <property> [<value> [-flags <flags>]]\n"
             "\n");
    RTPrintf("VBoxManage guestproperty    enumerate <vmname>|<uuid>\n"
             "                            [-patterns <patterns>]\n"
             "\n");
}

static int handleGetGuestProperty(int argc, char *argv[],
                                  ComPtr<IVirtualBox> aVirtualBox,
                                  ComPtr<ISession> aSession)
{
    HRESULT rc = S_OK;

    bool verbose = false;
    if ((3 == argc) && (0 == strcmp(argv[2], "-verbose")))
        verbose = true;
    else if (argc != 2)
        return errorSyntax(USAGE_GUESTPROPERTY, "Incorrect parameters");

    ComPtr<IMachine> machine;
    /* assume it's a UUID */
    rc = aVirtualBox->GetMachine(Guid(argv[0]), machine.asOutParam());
    if (FAILED(rc) || !machine)
    {
        /* must be a name */
        CHECK_ERROR(aVirtualBox, FindMachine(Bstr(argv[0]), machine.asOutParam()));
    }
    if (machine)
    {
        Guid uuid;
        machine->COMGETTER(Id)(uuid.asOutParam());

        /* open a session for the VM */
        CHECK_ERROR_RET (aVirtualBox, OpenSession(aSession, uuid), 1);

        /* get the mutable session machine */
        aSession->COMGETTER(Machine)(machine.asOutParam());

        Bstr value;
        uint64_t u64Timestamp;
        Bstr flags;
        CHECK_ERROR(machine, GetGuestProperty(Bstr(argv[1]), value.asOutParam(),
                    &u64Timestamp, flags.asOutParam()));
        if (!value)
            RTPrintf("No value set!\n");
        if (value)
            RTPrintf("Value: %lS\n", value.raw());
        if (value && verbose)
        {
            RTPrintf("Timestamp: %lld\n", u64Timestamp);
            RTPrintf("Flags: %lS\n", flags.raw());
        }
    }
    return SUCCEEDED(rc) ? 0 : 1;
}

static int handleSetGuestProperty(int argc, char *argv[],
                                  ComPtr<IVirtualBox> aVirtualBox,
                                  ComPtr<ISession> aSession)
{
    HRESULT rc = S_OK;

/*
 * Check the syntax.  We can deduce the correct syntax from the number of
 * arguments.
 */
    bool usageOK = true;
    const char *pszName = NULL;
    const char *pszValue = NULL;
    const char *pszFlags = NULL;
    if (3 == argc)
    {
        pszValue = argv[2];
    }
    else if (4 == argc)
    {
        if (strcmp(argv[2], "-flags") != 0)
            usageOK = false;
        else
            return errorSyntax(USAGE_GUESTPROPERTY,
                               "You may not specify flags without a value");
    }
    else if (5 == argc)
    {
        pszValue = argv[2];
        if (strcmp(argv[3], "-flags") != 0)
            usageOK = false;
        pszFlags = argv[4];
    }
    else if (argc != 2)
        usageOK = false;
    if (!usageOK)
        return errorSyntax(USAGE_GUESTPROPERTY, "Incorrect parameters");
    /* This is always needed. */
    pszName = argv[1];

    ComPtr<IMachine> machine;
    /* assume it's a UUID */
    rc = aVirtualBox->GetMachine(Guid(argv[0]), machine.asOutParam());
    if (FAILED(rc) || !machine)
    {
        /* must be a name */
        CHECK_ERROR(aVirtualBox, FindMachine(Bstr(argv[0]), machine.asOutParam()));
    }
    if (machine)
    {
        Guid uuid;
        machine->COMGETTER(Id)(uuid.asOutParam());

        /* open a session for the VM */
        CHECK_ERROR_RET (aVirtualBox, OpenSession(aSession, uuid), 1);

        /* get the mutable session machine */
        aSession->COMGETTER(Machine)(machine.asOutParam());

        if ((NULL == pszValue) && (NULL == pszFlags))
            CHECK_ERROR(machine, SetGuestPropertyValue(Bstr(pszName), NULL));
        else if (NULL == pszFlags)
            CHECK_ERROR(machine, SetGuestPropertyValue(Bstr(pszName), Bstr(pszValue)));
        else if (NULL == pszValue)
            CHECK_ERROR(machine, SetGuestProperty(Bstr(pszName), NULL, Bstr(pszFlags)));
        else
            CHECK_ERROR(machine, SetGuestProperty(Bstr(pszName), Bstr(pszValue), Bstr(pszFlags)));

        if (SUCCEEDED(rc))
            CHECK_ERROR(machine, SaveSettings());

        aSession->Close();
    }
    return SUCCEEDED(rc) ? 0 : 1;
}

/**
 * Enumerates the properties in the guest property store.
 *
 * @returns 0 on success, 1 on failure
 * @note see the command line API description for parameters
 */
static int handleEnumGuestProperty(int argc, char *argv[],
                                   ComPtr<IVirtualBox> aVirtualBox, ComPtr<ISession> aSession)
{
/*
 * Check the syntax.  We can deduce the correct syntax from the number of
 * arguments.
 */
    if ((argc < 1) || (2 == argc) ||
        ((argc > 3) && strcmp(argv[1], "-patterns") != 0))
        return errorSyntax(USAGE_GUESTPROPERTY, "Incorrect parameters");

/*
 * Pack the patterns
 */
    Utf8Str Utf8Patterns(argc > 2 ? argv[2] : "");
    for (ssize_t i = 3; i < argc; ++i)
        Utf8Patterns = Utf8StrFmt ("%s,%s", Utf8Patterns.raw(), argv[i]);

/*
 * Make the actual call to Main.
 */
    ComPtr<IMachine> machine;
    /* assume it's a UUID */
    HRESULT rc = aVirtualBox->GetMachine(Guid(argv[0]), machine.asOutParam());
    if (FAILED(rc) || !machine)
    {
        /* must be a name */
        CHECK_ERROR(aVirtualBox, FindMachine(Bstr(argv[0]), machine.asOutParam()));
    }
    if (machine)
    {
        Guid uuid;
        machine->COMGETTER(Id)(uuid.asOutParam());

        /* open a session for the VM */
        CHECK_ERROR_RET (aVirtualBox, OpenSession(aSession, uuid), 1);

        /* get the mutable session machine */
        aSession->COMGETTER(Machine)(machine.asOutParam());

        com::SafeArray <BSTR> names;
        com::SafeArray <BSTR> values;
        com::SafeArray <ULONG64> timestamps;
        com::SafeArray <BSTR> flags;
        CHECK_ERROR(machine, EnumerateGuestProperties(Bstr(Utf8Patterns),
                                                      ComSafeArrayAsOutParam(names),
                                                      ComSafeArrayAsOutParam(values),
                                                      ComSafeArrayAsOutParam(timestamps),
                                                      ComSafeArrayAsOutParam(flags)));
        if (SUCCEEDED(rc))
        {
            if (names.size() == 0)
                RTPrintf("No properties found.\n");
            for (unsigned i = 0; i < names.size(); ++i)
                RTPrintf("Name: %lS, value: %lS, timestamp: %lld, flags: %lS\n",
                         names[i], values[i], timestamps[i], flags[i]);
        }
    }
    return SUCCEEDED(rc) ? 0 : 1;
}

/**
 * Access the guest property store.
 *
 * @returns 0 on success, 1 on failure
 * @note see the command line API description for parameters
 */
int handleGuestProperty(int argc, char *argv[],
                        ComPtr<IVirtualBox> aVirtualBox, ComPtr<ISession> aSession)
{
    if (0 == argc)
        return errorSyntax(USAGE_GUESTPROPERTY, "Incorrect parameters");
    if (0 == strcmp(argv[0], "get"))
        return handleGetGuestProperty(argc - 1, argv + 1, aVirtualBox, aSession);
    else if (0 == strcmp(argv[0], "set"))
        return handleSetGuestProperty(argc - 1, argv + 1, aVirtualBox, aSession);
    else if (0 == strcmp(argv[0], "enumerate"))
        return handleEnumGuestProperty(argc - 1, argv + 1, aVirtualBox, aSession);
    /* else */
    return errorSyntax(USAGE_GUESTPROPERTY, "Incorrect parameters");
}

