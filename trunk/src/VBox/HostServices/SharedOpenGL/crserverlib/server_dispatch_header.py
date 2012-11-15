# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import sys

import apiutil

apiutil.CopyrightC()

print """
/* DO NOT EDIT - THIS FILE AUTOMATICALLY GENERATED BY server_dispatch_header.py SCRIPT */
#ifndef SERVER_DISPATCH_HEADER
#define SERVER_DISPATCH_HEADER

#ifdef WINDOWS
#define SERVER_DISPATCH_APIENTRY __stdcall
#else
#define SERVER_DISPATCH_APIENTRY
#endif

#include "chromium.h"
#include "state/cr_statetypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

"""

keys = apiutil.GetDispatchedFunctions(sys.argv[1]+"/APIspec.txt")

for func_name in keys:
    if ("get" in apiutil.Properties(func_name) or
        apiutil.FindSpecial( "server", func_name ) or
        apiutil.FindSpecial( sys.argv[1]+"/../state_tracker/state", func_name )):

        params = apiutil.Parameters(func_name)
        return_type = apiutil.ReturnType(func_name)
        
        print '%s SERVER_DISPATCH_APIENTRY crServerDispatch%s( %s );' % (return_type, func_name, apiutil.MakeDeclarationString( params ))

print """
#if defined(__cplusplus)
}
#endif

#endif /* SERVER_DISPATCH_HEADER */
"""
