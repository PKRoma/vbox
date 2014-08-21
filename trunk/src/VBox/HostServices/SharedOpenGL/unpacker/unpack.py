# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import sys

import apiutil


apiutil.CopyrightC()

print """/* DO NOT EDIT!  THIS CODE IS AUTOGENERATED BY unpack.py */

#include "unpacker.h"
#include "cr_opcodes.h"
#include "cr_error.h"
#include "cr_mem.h"
#include "cr_spu.h"
#include "unpack_extend.h"
#include <stdio.h>
#include <memory.h>

#include <iprt/cdefs.h>

DECLEXPORT(const unsigned char *) cr_unpackData = NULL;
SPUDispatchTable cr_unpackDispatch;

static void crUnpackExtend(void);
static void crUnpackExtendDbg(void);

/*#define CR_UNPACK_DEBUG_OPCODES*/
/*#define CR_UNPACK_DEBUG_LAST_OPCODES*/
"""

nodebug_opcodes = [
    "CR_MULTITEXCOORD2FARB_OPCODE",
    "CR_VERTEX3F_OPCODE",
    "CR_NORMAL3F_OPCODE",
    "CR_COLOR4UB_OPCODE",
    "CR_LOADIDENTITY_OPCODE",
    "CR_MATRIXMODE_OPCODE",
    "CR_LOADMATRIXF_OPCODE",
    "CR_DISABLE_OPCODE",
    "CR_COLOR4F_OPCODE",
    "CR_ENABLE_OPCODE",
    "CR_BEGIN_OPCODE",
    "CR_END_OPCODE",
    "CR_SECONDARYCOLOR3FEXT_OPCODE"
]

nodebug_extopcodes = [
    "CR_ACTIVETEXTUREARB_EXTEND_OPCODE"
]

#
# Useful functions
#

def ReadData( offset, arg_type ):
    """Emit a READ_DOUBLE or READ_DATA call for pulling a GL function
    argument out of the buffer's operand area."""
    if arg_type == "GLdouble" or arg_type == "GLclampd":
        retval = "READ_DOUBLE( %d )" % offset
    else:
        retval = "READ_DATA( %d, %s )" % (offset, arg_type)
    return retval


def FindReturnPointer( return_type, params ):
    """For GL functions that return values (either as the return value or
    through a pointer parameter) emit a SET_RETURN_PTR call."""
    arg_len = apiutil.PacketLength( params )
    if (return_type != 'void'):
        print '\tSET_RETURN_PTR( %d );' % (arg_len + 8) # extended opcode plus packet length
    else:
        paramList = [ ('foo', 'void *', 0) ]
        print '\tSET_RETURN_PTR( %d );' % (arg_len + 8 - apiutil.PacketLength(paramList))


def FindWritebackPointer( return_type, params ):
    """Emit a SET_WRITEBACK_PTR call."""
    arg_len = apiutil.PacketLength( params )
    if return_type != 'void':
        paramList = [ ('foo', 'void *', 0) ]
        arg_len += apiutil.PacketLength( paramList )

    print '\tSET_WRITEBACK_PTR( %d );' % (arg_len + 8) # extended opcode plus packet length


def MakeNormalCall( return_type, func_name, params, counter_init = 0 ):
    counter = counter_init
    copy_of_params = params[:]

    for i in range( 0, len(params) ):
        (name, type, vecSize) = params[i]
        if apiutil.IsPointer(copy_of_params[i][1]):
            params[i] = ('NULL', type, vecSize)
            copy_of_params[i] = (copy_of_params[i][0], 'void', 0)
            if not "get" in apiutil.Properties(func_name):
                print '\tcrError( "%s needs to be special cased!" );' % func_name
        else:
            print "\t%s %s = %s;" % ( copy_of_params[i][1], name, ReadData( counter, copy_of_params[i][1] ) )
        counter += apiutil.sizeof(copy_of_params[i][1])

    if ("get" in apiutil.Properties(func_name)):
        FindReturnPointer( return_type, params )
        FindWritebackPointer( return_type, params )

    if return_type != "void":
        print "\t(void)",
    else:
        print "\t",
    print "cr_unpackDispatch.%s( %s );" % (func_name, apiutil.MakeCallString(params))


def MakeVectorCall( return_type, func_name, arg_type ):
    """Convert a call like glVertex3f to glVertex3fv."""
    vec_func = apiutil.VectorFunction(func_name)
    params = apiutil.Parameters(vec_func)
    assert len(params) == 1
    (arg_name, vecType, vecSize) = params[0]

    if arg_type == "GLdouble" or arg_type == "GLclampd":
        print "#ifdef CR_UNALIGNED_ACCESS_OKAY"
        print "\tcr_unpackDispatch.%s((%s) cr_unpackData);" % (vec_func, vecType)
        print "#else"
        for index in range(0, vecSize):
            print "\tGLdouble v" + `index` + " = READ_DOUBLE(", `index * 8`, ");"
        if return_type != "void":
            print "\t(void) cr_unpackDispatch.%s(" % func_name,
        else:
            print "\tcr_unpackDispatch.%s(" % func_name,
        for index in range(0, vecSize):
            print "v" + `index`,
            if index != vecSize - 1:
                print ",",
        print ");"
        print "#endif"
    else:
        print "\tcr_unpackDispatch.%s((%s) cr_unpackData);" % (vec_func, vecType)



keys = apiutil.GetDispatchedFunctions(sys.argv[1]+"/APIspec.txt")


#
# Generate unpack functions for all the simple functions.
#
for func_name in keys:
    if (not "pack" in apiutil.ChromiumProps(func_name) or
        apiutil.FindSpecial( "unpacker", func_name )):
        continue

    params = apiutil.Parameters(func_name)
    return_type = apiutil.ReturnType(func_name)
    
    print "static void crUnpack%s(void)" % func_name
    print "{"

    vector_func = apiutil.VectorFunction(func_name)
    if (vector_func and len(apiutil.Parameters(vector_func)) == 1):
        MakeVectorCall( return_type, func_name, params[0][1] )
    else:
        MakeNormalCall( return_type, func_name, params )
    packet_length = apiutil.PacketLength( params )
    if packet_length == 0:
        print "\tINCR_DATA_PTR_NO_ARGS( );"
    else:
        print "\tINCR_DATA_PTR( %d );" % packet_length
    print "}\n"


#
# Emit some code
#
print """ 
typedef struct __dispatchNode {
    const unsigned char *unpackData;
    struct __dispatchNode *next;
} DispatchNode;

static DispatchNode *unpackStack = NULL;

static SPUDispatchTable *cr_lastDispatch = NULL;

void crUnpackPush(void)
{
    DispatchNode *node = (DispatchNode*)crAlloc( sizeof( *node ) );
    node->next = unpackStack;
    unpackStack = node;
    node->unpackData = cr_unpackData;
}

void crUnpackPop(void)
{
    DispatchNode *node = unpackStack;

    if (!node)
    {
        crError( "crUnpackPop called with an empty stack!" );
    }
    unpackStack = node->next;
    cr_unpackData = node->unpackData;
    crFree( node );
}

CR_UNPACK_BUFFER_TYPE crUnpackGetBufferType(const void *opcodes, unsigned int num_opcodes)
{
    const uint8_t *pu8Codes = (const uint8_t *)opcodes;

    uint8_t first;
    uint8_t last;

    if (!num_opcodes)
        return CR_UNPACK_BUFFER_TYPE_GENERIC;

    first = pu8Codes[0];
    last = pu8Codes[1-(int)num_opcodes];
    
    switch (last)
    {
        case CR_CMDBLOCKFLUSH_OPCODE:
            return CR_UNPACK_BUFFER_TYPE_CMDBLOCK_FLUSH;
        case CR_CMDBLOCKEND_OPCODE:
            return (first == CR_CMDBLOCKBEGIN_OPCODE) ? CR_UNPACK_BUFFER_TYPE_GENERIC : CR_UNPACK_BUFFER_TYPE_CMDBLOCK_END;
        default:
            return (first != CR_CMDBLOCKBEGIN_OPCODE) ? CR_UNPACK_BUFFER_TYPE_GENERIC : CR_UNPACK_BUFFER_TYPE_CMDBLOCK_BEGIN;
    } 
}

void crUnpack( const void *data, const void *opcodes, 
        unsigned int num_opcodes, SPUDispatchTable *table )
{
    unsigned int i;
    const unsigned char *unpack_opcodes;
    if (table != cr_lastDispatch)
    {
        crSPUCopyDispatchTable( &cr_unpackDispatch, table );
        cr_lastDispatch = table;
    }

    unpack_opcodes = (const unsigned char *)opcodes;
    cr_unpackData = (const unsigned char *)data;

#if defined(CR_UNPACK_DEBUG_OPCODES) || defined(CR_UNPACK_DEBUG_LAST_OPCODES)
    crDebug("crUnpack: %d opcodes", num_opcodes);
#endif

    for (i = 0 ; i < num_opcodes ; i++)
    {
    
        CRDBGPTR_CHECKZ(writeback_ptr);
        CRDBGPTR_CHECKZ(return_ptr);
    
        /*crDebug(\"Unpacking opcode \%d\", *unpack_opcodes);*/
        switch( *unpack_opcodes )
        {"""

#
# Emit switch cases for all unextended opcodes
#
for func_name in keys:
    if "pack" in apiutil.ChromiumProps(func_name):
        print '\t\t\tcase %s:' % apiutil.OpcodeName( func_name )
        if not apiutil.OpcodeName(func_name) in nodebug_opcodes:
            print """
#ifdef CR_UNPACK_DEBUG_LAST_OPCODES
                if (i==(num_opcodes-1))
#endif
#if defined(CR_UNPACK_DEBUG_OPCODES) || defined(CR_UNPACK_DEBUG_LAST_OPCODES)
                crDebug("Unpack: %s");
#endif """ % apiutil.OpcodeName(func_name)
        print '\t\t\t\tcrUnpack%s(); \n\t\t\t\tbreak;' % func_name

print """       
            case CR_EXTEND_OPCODE:
                #ifdef CR_UNPACK_DEBUG_OPCODES 
                    crUnpackExtendDbg();
                #else
                # ifdef CR_UNPACK_DEBUG_LAST_OPCODES
                    if (i==(num_opcodes-1)) crUnpackExtendDbg();
                    else
                # endif
                    crUnpackExtend();
                #endif
                break;
            case CR_CMDBLOCKBEGIN_OPCODE:
            case CR_CMDBLOCKEND_OPCODE:
            case CR_CMDBLOCKFLUSH_OPCODE:
            case CR_NOP_OPCODE:
                INCR_DATA_PTR_NO_ARGS( );
                break;
            default:
                crError( "Unknown opcode: %d", *unpack_opcodes );
                break;
        }
        
        CRDBGPTR_CHECKZ(writeback_ptr);
        CRDBGPTR_CHECKZ(return_ptr);
        
        unpack_opcodes--;
    }
}"""


#
# Emit unpack functions for extended opcodes, non-special functions only.
#
for func_name in keys:
        if ("extpack" in apiutil.ChromiumProps(func_name)
            and not apiutil.FindSpecial("unpacker", func_name)):
            return_type = apiutil.ReturnType(func_name)
            params = apiutil.Parameters(func_name)
            print 'static void crUnpackExtend%s(void)' % func_name
            print '{'
            MakeNormalCall( return_type, func_name, params, 8 )
            print '}\n'

print 'static void crUnpackExtend(void)'
print '{'
print '\tGLenum extend_opcode = %s;' % ReadData( 4, 'GLenum' );
print ''
print '\t/*crDebug(\"Unpacking extended opcode \%d", extend_opcode);*/'
print '\tswitch( extend_opcode )'
print '\t{'


#
# Emit switch statement for extended opcodes
#
for func_name in keys:
    if "extpack" in apiutil.ChromiumProps(func_name):
        print '\t\tcase %s:' % apiutil.ExtendedOpcodeName( func_name )
#        print '\t\t\t\tcrDebug("Unpack: %s");' % apiutil.ExtendedOpcodeName( func_name )
        print '\t\t\tcrUnpackExtend%s( );' % func_name
        print '\t\t\tbreak;'

print """       default:
            crError( "Unknown extended opcode: %d", (int) extend_opcode );
            break;
    }
    INCR_VAR_PTR();
}"""

print 'static void crUnpackExtendDbg(void)'
print '{'
print '\tGLenum extend_opcode = %s;' % ReadData( 4, 'GLenum' );
print ''
print '\t/*crDebug(\"Unpacking extended opcode \%d", extend_opcode);*/'
print '\tswitch( extend_opcode )'
print '\t{'


#
# Emit switch statement for extended opcodes
#
for func_name in keys:
    if "extpack" in apiutil.ChromiumProps(func_name):
        print '\t\tcase %s:' % apiutil.ExtendedOpcodeName( func_name )
        if not apiutil.ExtendedOpcodeName(func_name) in nodebug_extopcodes:
            print '\t\t\tcrDebug("Unpack: %s");' % apiutil.ExtendedOpcodeName( func_name )
        print '\t\t\tcrUnpackExtend%s( );' % func_name
        print '\t\t\tbreak;'

print """       default:
            crError( "Unknown extended opcode: %d", (int) extend_opcode );
            break;
    }
    INCR_VAR_PTR();
}"""
