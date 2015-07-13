# $Id$
import sys, cPickle, re, os

sys.path.append( "../glapi_parser" )
import apiutil

# mode is "header" or "defs"
mode = sys.argv[1]

keys = apiutil.GetDispatchedFunctions(sys.argv[3]+"/APIspec.txt")

# Any new function implemented in the DLM has to have an entry added here.
# Each function has its return type, function name, and parameters provided.
# We'll use these to generate both a header file, and a definition file.
additionalFunctions = [
	('CRDLM DLM_APIENTRY *', 'crDLMNewDLM', 'unsigned int configSize, const CRDLMConfig *config'),
	('CRDLMContextState DLM_APIENTRY *', 'crDLMNewContext', 'CRDLM *dlm'),
	('void DLM_APIENTRY', 'crDLMFreeContext', 'CRDLMContextState *state, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMUseDLM', 'CRDLM *dlm'),
	('void DLM_APIENTRY','crDLMFreeDLM', 'CRDLM *dlm, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMSetCurrentState', 'CRDLMContextState *state'),
	('CRDLMContextState DLM_APIENTRY *', 'crDLMGetCurrentState', 'void'),
	('void DLM_APIENTRY', 'crDLMSetupClientState', 'SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMRestoreClientState', 'CRClientState *clientState, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMSendAllDLMLists', 'CRDLM *dlm, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMSendAllLists', 'SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMSendDLMList', 'CRDLM *dlm, unsigned long listIdentifier, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMSendList', 'unsigned long listIdentifier, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayDLMList', 'CRDLM *dlm, unsigned long listIdentifier, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayList', 'unsigned long listIdentifier, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayDLMListState', 'CRDLM *dlm, unsigned long listIdentifier, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayListState', 'unsigned long listIdentifier, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayDLMLists', 'CRDLM *dlm, GLsizei n, GLenum type, const GLvoid *lists, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayLists', 'GLsizei n, GLenum type, const GLvoid *lists, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayDLMListsState', 'CRDLM *dlm, GLsizei n, GLenum type, const GLvoid *lists, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMReplayListsState', 'GLsizei n, GLenum type, const GLvoid *lists, SPUDispatchTable *dispatchTable'),
	('CRDLMError DLM_APIENTRY', 'crDLMDeleteListContent', 'CRDLM *dlm, unsigned long listIdentifier'),
	('void DLM_APIENTRY', 'crDLMComputeBoundingBox', 'unsigned long listId'),
	('GLuint DLM_APIENTRY', 'crDLMGetCurrentList', 'void'),
	('GLenum DLM_APIENTRY', 'crDLMGetCurrentMode', 'void'),
	('void DLM_APIENTRY', 'crDLMErrorFunction', 'CRDLMErrorCallback callback'),
	('void DLM_APIENTRY', 'crDLMNewList', 'GLuint list, GLenum mode, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMEndList', 'SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMCallList', 'GLuint list, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMCallLists', 'GLsizei n, GLenum type, const GLvoid *lists, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMDeleteLists', 'GLuint list, GLsizei range, SPUDispatchTable *dispatchTable'),
	('void DLM_APIENTRY', 'crDLMListBase', 'GLuint base, SPUDispatchTable *dispatchTable'),
	('GLboolean DLM_APIENTRY', 'crDLMIsList', 'GLuint list, SPUDispatchTable *dispatchTable'),
	('GLuint DLM_APIENTRY', 'crDLMGenLists', 'GLsizei range, SPUDispatchTable *dispatchTable'),
	('int32_t DLM_APIENTRY', 'crDLMSaveState', 'CRDLM *dlm, PSSMHANDLE pSSM'),
	('bool DLM_APIENTRY', 'crDLMLoadState', 'CRDLM *dlm, PSSMHANDLE pSSM, SPUDispatchTable *dispatchTable'),
	#('void DLM_APIENTRY', 'crDLMListSent', 'CRDLM *dlm, unsigned long listIdentifier'),
	#('GLboolean DLM_APIENTRY', 'crDLMIsListSent', 'CRDLM *dlm, unsigned long listIdentifier'),
	#('GLint DLM_APIENTRY', 'crDLMListSize', 'CRDLM *dlm, unsigned long listIdentifier'),
]

if mode == 'header':
    print """#ifndef CR_DLM_H

/* DO NOT EDIT.  This file is auto-generated by %s. */
#define CR_DLM_H

#if defined(WINDOWS)
#define DLM_APIENTRY
#else
#define DLM_APIENTRY
#endif

#include "chromium.h"
#include "state/cr_client.h"
#include "cr_spu.h"
#include "cr_hash.h"
#include "cr_threads.h"
#include "cr_pack.h"
#ifdef CHROMIUM_THREADSAFE
#include "cr_threads.h"
#endif
#include <VBox/types.h>
""" % os.path.basename(sys.argv[0])

    # Generate operation codes enum to be used for saving and restoring lists.
    print "/* OpCodes codes enum to be used for saving and restoring lists. */"
    print "typedef enum {"

    for func_name in keys:
        if apiutil.CanCompile(func_name) and not apiutil.FindSpecial("dlm", func_name):
            print "    VBOX_DL_OPCODE_%s," % func_name

    print "    VBOX_DL_OPCODE_MAX,"
    print "} VBoxDLOpCode;"

    print """
/* 3D bounding box */
typedef struct {
	double xmin, xmax, ymin, ymax, zmin, zmax;
} CRDLMBounds;

/* Indicates whether we're currently involved in playback or not */
typedef enum {
	CRDLM_IMMEDIATE = 0,
	CRDLM_REPLAY_STATE_FUNCTIONS = 1,
	CRDLM_REPLAY_ALL_FUNCTIONS = 2
} CRDLMReplayState;

/* This is enough information to hold an instance of a single function call. */
typedef struct DLMInstanceList {
	struct DLMInstanceList *next;
	struct DLMInstanceList *stateNext;
	int                     cbInstance;
	VBoxDLOpCode            iVBoxOpCode; /* This field name should not interfere w/ OpenGL function parameters names (for example w/ param 'opcode' for glLogicOp()). */
	void (*execute)(struct DLMInstanceList *instance, SPUDispatchTable *dispatchTable);
} DLMInstanceList;

typedef struct {
    DLMInstanceList *first, *last;
    uint32_t         numInstances;
    DLMInstanceList *stateFirst, *stateLast;
    GLuint           hwid;
} DLMListInfo;

typedef struct {
	/* This holds all the display list information, hashed by list identifier. */
	CRHashTable *displayLists;

	/* This is a count of the number of contexts/users that are using
	 * this DLM.
	 */
	unsigned int userCount;

#ifdef CHROMIUM_THREADSAFE
	/* This mutex protects the displayLists hash table from simultaneous
	 * updates by multiple contexts.
	 */
	CRmutex dlMutex;
	CRtsd tsdKey;
#endif

	/* Configuration information - see the CRDLMConfig structure below
	 * for details.
	 */
	unsigned int bufferSize;
} CRDLM;

/* This structure holds thread-specific state.  Each thread can be
 * associated with one (and only one) context; and each context can
 * be associated with one (and only one) DLM.  Making things interesting,
 * though, is that each DLM can be associated with multiple contexts.
 *
 * So the thread-specific data key is associated with each context, not
 * with each DLM.  Two different threads can, through two different
 * contexts that share a single DLM, each have independent state and
 * conditions.
 */

typedef struct {
	CRDLM *dlm;			/* the DLM associated with this state */
	unsigned long currentListIdentifier;	/* open display list */
	DLMListInfo *currentListInfo;	/* open display list data */
	GLenum currentListMode;		/* GL_COMPILE or GL_COMPILE_AND_EXECUTE */
	GLuint listBase;

} CRDLMContextState;

/* These additional structures are for passing information to and from the 
 * CRDLM interface routines.
 */
typedef struct {
	/* The size, in bytes, that the packer will initially allocate for
	 * each new buffer.
	 */
#define CRDLM_DEFAULT_BUFFERSIZE (1024*1024)
	unsigned int bufferSize;	/* this will be allocated for each buffer */
} CRDLMConfig;

/* Positive values match GL error values.
 * 0 (GL_NO_ERROR) is returned for success
 * Negative values are internal errors.
 * Possible positive values (from GL/gl.h) are:
 * GL_NO_ERROR (0x0)
 * GL_INVALID_ENUM (0x0500)
 * GL_INVALID_VALUE (0x0501)
 * GL_INVALID_OPERATION (0x0502)
 * GL_STACK_OVERFLOW (0x0503)
 * GL_STACK_UNDERFLOW (0x0504)
 * GL_OUT_OF_MEMORY (0x0505)
 */
typedef int CRDLMError;

/* This error reported if there's no current state. The caller is responsible
 * for appropriately allocating context state with crDLMNewContext(), and
 * for making it current with crDLMMakeCurrent().
 */
#define CRDLM_ERROR_STATE	(-1)


typedef void (*CRDLMErrorCallback)(int line, const char *file, GLenum error, const char *info);


#ifdef __cplusplus
extern "C" {
#endif
"""
elif mode == 'defs':
	apiutil.CopyrightDef()
	print '''\t; DO NOT EDIT.  This code is generated by %s.

EXPORTS''' % os.path.basename(sys.argv[0])
else:
	raise "unknown generation mode '%s'" % mode

# Generate the list of functions, starting with those coded into
# the module
for (returnValue, name, parameters) in additionalFunctions:
	if mode == 'header':
		print "extern %s %s(%s);" % (returnValue, name, parameters)
	elif mode == 'defs':
		print "%s" % name

# Continue with functions that are auto-generated.

if mode == 'header':
	print 
	print "/* auto-generated compilation functions begin here */"



for func_name in keys:
	props = apiutil.Properties(func_name)
	# We're interested in intercepting all calls that:
	#   - can be put into a display list (i.e. "not ("nolist" in props)")
	#   - change client-side state that affects saving DL elements (i.e. "setclient" in props)

	if apiutil.CanCompile(func_name):
		params = apiutil.Parameters(func_name)
		argstring = apiutil.MakeDeclarationString(params)
		if "useclient" in props or "pixelstore" in props:
			argstring = argstring + ", CRClientState *c"

		if mode == 'header':
			print 'extern void DLM_APIENTRY crDLMCompile%s( %s );' % (func_name, argstring)
		elif mode == 'defs':
			print "crDLMCompile%s" % func_name

# Next make declarations for all the checklist functions.
if mode == 'header':
	print """
/* auto-generated CheckList functions begin here.  There is one for each
 * function that has a dual nature: even when there's an active glNewList,
 * sometimes they are compiled into the display list, and sometimes they
 * are treated like a control function.  The CheckList function will
 * return TRUE if the function should really be compiled into a display
 * list.  The calling SPU is responsible for checking this; but the
 * DLM will also print an error if it detects an invalid use.
 */
"""
elif mode == 'defs':
	pass

for func_name in keys:
	if "checklist" in apiutil.ChromiumProps(func_name):
		params = apiutil.Parameters(func_name)
		argstring = apiutil.MakeDeclarationString(params)
		if mode == 'header':
			print 'int DLM_APIENTRY crDLMCheckList%s( %s );' % (func_name, argstring)
		elif mode == 'defs':
			print "crDLMCheckList%s" % func_name

if mode == 'header':
	print """
#ifdef __cplusplus
}
#endif

#endif /* CR_DLM_H */"""
