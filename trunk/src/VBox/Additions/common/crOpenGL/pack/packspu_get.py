# Copyright (c) 2001, Stanford University
# All rights reserved.
#
# See the file LICENSE.txt for information on redistributing this software.

import sys

import apiutil


apiutil.CopyrightC()

print """
/* DO NOT EDIT - THIS FILE AUTOMATICALLY GENERATED BY packspu_get.py SCRIPT */
#include "packspu.h"
#include "cr_packfunctions.h"
#include "cr_net.h"
#include "cr_mem.h"
#include "packspu_proto.h"
"""

print """
GLboolean crPackIsPixelStoreParm(GLenum pname)
{
    if (pname == GL_UNPACK_ALIGNMENT
        || pname == GL_UNPACK_ROW_LENGTH
        || pname == GL_UNPACK_SKIP_PIXELS
        || pname == GL_UNPACK_LSB_FIRST
        || pname == GL_UNPACK_SWAP_BYTES
#ifdef CR_OPENGL_VERSION_1_2
        || pname == GL_UNPACK_IMAGE_HEIGHT
#endif
        || pname == GL_UNPACK_SKIP_ROWS
        || pname == GL_PACK_ALIGNMENT
        || pname == GL_PACK_ROW_LENGTH
        || pname == GL_PACK_SKIP_PIXELS
        || pname == GL_PACK_LSB_FIRST
        || pname == GL_PACK_SWAP_BYTES
#ifdef CR_OPENGL_VERSION_1_2
        || pname == GL_PACK_IMAGE_HEIGHT
#endif
        || pname == GL_PACK_SKIP_ROWS)
    {
        return GL_TRUE;
    }
    return GL_FALSE;
}
"""

from get_sizes import *
from get_components import *

easy_swaps = { 
    'GenTextures': '(unsigned int) n',
    'GetClipPlane': '4',
    'GetPolygonStipple': '0'
}
    
simple_funcs = [ 'GetIntegerv', 'GetFloatv', 'GetDoublev', 'GetBooleanv' ]
simple_swaps = [ 'SWAP32', 'SWAPFLOAT', 'SWAPDOUBLE', '(GLboolean) SWAP32' ]

hard_funcs = {
    'GetLightfv': 'SWAPFLOAT',
    'GetLightiv': 'SWAP32',
    'GetMaterialfv': 'SWAPFLOAT',
    'GetMaterialiv': 'SWAP32',
    'GetTexEnvfv': 'SWAPFLOAT',
    'GetTexEnviv': 'SWAP32',
    'GetTexGendv': 'SWAPDOUBLE',
    'GetTexGenfv': 'SWAPFLOAT',
    'GetTexGeniv': 'SWAP32',
    'GetTexLevelParameterfv': 'SWAPFLOAT',
    'GetTexLevelParameteriv': 'SWAP32',
    'GetTexParameterfv': 'SWAPFLOAT',
    'GetTexParameteriv': 'SWAP32' }

keys = apiutil.GetDispatchedFunctions(sys.argv[1]+"/APIspec.txt")

for func_name in keys:
    params = apiutil.Parameters(func_name)
    return_type = apiutil.ReturnType(func_name)
    if apiutil.FindSpecial( "packspu", func_name ):
        continue

    if "get" in apiutil.Properties(func_name):
        print '%s PACKSPU_APIENTRY packspu_%s( %s )' % ( return_type, func_name, apiutil.MakeDeclarationString( params ) )
        print '{'
        print '\tGET_THREAD(thread);'
        print '\tint writeback = 1;'
        if return_type != 'void':
            print '\t%s return_val = (%s) 0;' % (return_type, return_type)
            params.append( ("&return_val", "foo", 0) )
        if (func_name in easy_swaps.keys() and easy_swaps[func_name] != '0') or func_name in simple_funcs or func_name in hard_funcs.keys():
            print '\tunsigned int i;'
        print '\tif (!(pack_spu.thread[pack_spu.idxThreadInUse].netServer.conn->actual_network))'
        print '\t{'
        print '\t\tcrError( "packspu_%s doesn\'t work when there\'s no actual network involved!\\nTry using the simplequery SPU in your chain!" );' % func_name
        print '\t}'
        if func_name in simple_funcs:
            print """
    if (crPackIsPixelStoreParm(pname)
        || pname == GL_DRAW_BUFFER
#ifdef CR_OPENGL_VERSION_1_3
        || pname == GL_ACTIVE_TEXTURE
#endif
#ifdef CR_ARB_multitexture
        || pname == GL_ACTIVE_TEXTURE_ARB
#endif
        || pname == GL_TEXTURE_BINDING_1D
        || pname == GL_TEXTURE_BINDING_2D
#ifdef CR_NV_texture_rectangle
        || pname == GL_TEXTURE_BINDING_RECTANGLE_NV
#endif
#ifdef CR_ARB_texture_cube_map
        || pname == GL_TEXTURE_BINDING_CUBE_MAP_ARB
#endif
        )
        {
#ifdef DEBUG
            if (!crPackIsPixelStoreParm(pname))
            {
                %s localparams;
                localparams = (%s) crAlloc(__numValues(pname) * sizeof(*localparams));
                crState%s(pname, localparams);
                crPack%s(%s, &writeback);
                packspuFlush( (void *) thread );
                while (writeback)
                    crNetRecv();
                for (i=0; i<__numValues(pname); ++i)
                {
                    if (localparams[i] != params[i])
                    {
                        crWarning("Incorrect local state in %s for %%x param %%i", pname, i);
                        crWarning("Expected %%i but got %%i", (int)localparams[i], (int)params[i]);
                    }
                }
                crFree(localparams);
                return;
            }
            else
#endif
            {
                crState%s(pname, params);
                return;
            }
            
        }
            """ % (params[-1][1], params[-1][1], func_name, func_name, apiutil.MakeCallString(params), func_name, func_name)
        params.append( ("&writeback", "foo", 0) )
        print '\tif (pack_spu.swap)'
        print '\t{'
        print '\t\tcrPack%sSWAP( %s );' % (func_name, apiutil.MakeCallString( params ) )
        print '\t}'
        print '\telse'
        print '\t{'
        print '\t\tcrPack%s( %s );' % (func_name, apiutil.MakeCallString( params ) )
        print '\t}'
        print '\tpackspuFlush( (void *) thread );'
        print '\twhile (writeback)'
        print '\t\tcrNetRecv();'



        lastParamName = params[-2][0]
        if return_type != 'void':
            print '\tif (pack_spu.swap)'
            print '\t{'
            print '\t\treturn_val = (%s) SWAP32(return_val);' % return_type
            print '\t}'
            print '\treturn return_val;'
        if func_name in easy_swaps.keys() and easy_swaps[func_name] != '0':
            limit = easy_swaps[func_name]
            print '\tif (pack_spu.swap)'
            print '\t{'
            print '\t\tfor (i = 0 ; i < %s ; i++)' % limit
            print '\t\t{'
            if params[-2][1].find( "double" ) > -1:
                print '\t\t\t%s[i] = SWAPDOUBLE(%s[i]);' % (lastParamName, lastParamName)
            else:
                print '\t\t\t%s[i] = SWAP32(%s[i]);' % (lastParamName, lastParamName)
            print '\t\t}'
            print '\t}'
        for index in range(len(simple_funcs)):
            if simple_funcs[index] == func_name:
                print '\tif (pack_spu.swap)'
                print '\t{'
                print '\t\tfor (i = 0 ; i < __numValues( pname ) ; i++)'
                print '\t\t{'
                if simple_swaps[index] == 'SWAPDOUBLE':
                    print '\t\t\t%s[i] = %s(%s[i]);' % (lastParamName, simple_swaps[index], lastParamName)
                else:
                    print '\t\t\t((GLuint *) %s)[i] = %s(%s[i]);' % (lastParamName, simple_swaps[index], lastParamName)
                print '\t\t}'
                print '\t}'
        if func_name in hard_funcs.keys():
            print '\tif (pack_spu.swap)'
            print '\t{'
            print '\t\tfor (i = 0 ; i < lookupComponents(pname) ; i++)'
            print '\t\t{'
            if hard_funcs[func_name] == 'SWAPDOUBLE':
                print '\t\t\t%s[i] = %s(%s[i]);' % (lastParamName, hard_funcs[func_name], lastParamName)
            else:
                print '\t\t\t((GLuint *) %s)[i] = %s(%s[i]);' % (lastParamName, hard_funcs[func_name], lastParamName)
            print '\t\t}'
            print '\t}'
        print '}\n'
