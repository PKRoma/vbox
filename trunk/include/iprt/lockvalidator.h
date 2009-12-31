/** @file
 * IPRT - Lock Validator.
 */

/*
 * Copyright (C) 2009 Sun Microsystems, Inc.
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
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */

#ifndef ___iprt_lockvalidator_h
#define ___iprt_lockvalidator_h

#include <iprt/cdefs.h>
#include <iprt/types.h>
#include <iprt/assert.h>
#include <iprt/thread.h>


/** @defgroup grp_ldr       RTLockValidator - Lock Validator
 * @ingroup grp_rt
 * @{
 */

RT_C_DECLS_BEGIN

/** Pointer to a record union.
 * @internal  */
typedef union RTLOCKVALRECUNION *PRTLOCKVALRECUNION;

/**
 * Source position.
 */
typedef struct RTLOCKVALSRCPOS
{
    /** The file where the lock was taken. */
    R3R0PTRTYPE(const char * volatile)  pszFile;
    /** The function where the lock was taken. */
    R3R0PTRTYPE(const char * volatile)  pszFunction;
    /** Some ID indicating where the lock was taken, typically an address. */
    RTHCUINTPTR volatile                uId;
    /** The line number in the file. */
    uint32_t volatile                   uLine;
#if HC_ARCH_BITS == 64
    uint32_t                            u32Padding; /**< Alignment padding. */
#endif
} RTLOCKVALSRCPOS;
AssertCompileSize(RTLOCKVALSRCPOS, HC_ARCH_BITS == 32 ? 16 : 32);
/* The pointer types are defined in iprt/types.h. */

/** @def RTLOCKVALSRCPOS_INIT
 * Initializer for a RTLOCKVALSRCPOS variable.
 *
 * @param   pszFile         The file name.  Optional (NULL).
 * @param   uLine           The line number in that file. Optional (0).
 * @param   pszFunction     The function.  Optional (NULL).
 * @param   uId             Some location ID, normally the return address.
 *                          Optional (NULL).
 */
#if HC_ARCH_BITS == 64
# define RTLOCKVALSRCPOS_INIT(pszFile, uLine, pszFunction, uId) \
    { (pszFile), (pszFunction), (uId), (uLine), 0 }
#else
# define RTLOCKVALSRCPOS_INIT(pszFile, uLine, pszFunction, uId) \
    { (pszFile), (pszFunction), (uId), (uLine) }
#endif

/** @def RTLOCKVALSRCPOS_INIT_DEBUG_API
 * Initializer for a RTLOCKVALSRCPOS variable in a typicial debug API
 * variant.  Assumes RT_SRC_POS_DECL and RTHCUINTPTR uId as arguments.
 */
#define RTLOCKVALSRCPOS_INIT_DEBUG_API()  \
    RTLOCKVALSRCPOS_INIT(pszFile, iLine, pszFunction, uId)

/** @def RTLOCKVALSRCPOS_INIT_NORMAL_API
 * Initializer for a RTLOCKVALSRCPOS variable in a normal API
 * variant.  Assumes iprt/asm.h is included.
 */
#define RTLOCKVALSRCPOS_INIT_NORMAL_API() \
    RTLOCKVALSRCPOS_INIT(__FILE__, __LINE__, __PRETTY_FUNCTION__, (uintptr_t)ASMReturnAddress())

/** Pointer to a record of one ownership share.  */
typedef struct RTLOCKVALRECSHRD *PRTLOCKVALRECSHRD;


/**
 * Lock validator record core.
 */
typedef struct RTLOCKVALIDATORRECORE
{
    /** The magic value indicating the record type. */
    uint32_t                            u32Magic;
} RTLOCKVALRECCORE;
/** Pointer to a lock validator record core. */
typedef RTLOCKVALRECCORE *PRTLOCKVALRECCORE;
/** Pointer to a const lock validator record core. */
typedef RTLOCKVALRECCORE const *PCRTLOCKVALRECCORE;


/**
 * Record recording the exclusive ownership of a lock.
 *
 * This is typically part of the per-lock data structure when compiling with
 * the lock validator.
 */
typedef struct RTLOCKVALRECEXCL
{
    /** Record core with RTLOCKVALRECEXCL_MAGIC as the magic value. */
    RTLOCKVALRECCORE                    Core;
    /** Whether it's enabled or not. */
    bool                                fEnabled;
    /** Reserved. */
    bool                                afReserved[3];
    /** Source position where the lock was taken. */
    RTLOCKVALSRCPOS                     SrcPos;
    /** The current owner thread. */
    RTTHREAD volatile                   hThread;
    /** Pointer to the lock record below us. Only accessed by the owner. */
    R3R0PTRTYPE(PRTLOCKVALRECUNION)     pDown;
    /** Recursion count */
    uint32_t                            cRecursion;
    /** The lock sub-class. */
    uint32_t volatile                   uSubClass;
    /** The lock class. */
    RTLOCKVALIDATORCLASS                hClass;
    /** Pointer to the lock. */
    RTHCPTR                             hLock;
    /** The lock name. */
    R3R0PTRTYPE(const char *)           pszName;
    /** Pointer to the next sibling record.
     * This is used to find the read side of a read-write lock.  */
    R3R0PTRTYPE(PRTLOCKVALRECUNION) pSibling;
} RTLOCKVALRECEXCL;
AssertCompileSize(RTLOCKVALRECEXCL, HC_ARCH_BITS == 32 ? 8 + 16 + 32 : 8 + 32 + 56);
/* The pointer type is defined in iprt/types.h. */

/**
 * For recording the one ownership share.
 */
typedef struct RTLOCKVALRECSHRDOWN
{
    /** Record core with RTLOCKVALRECSHRDOWN_MAGIC as the magic value. */
    RTLOCKVALRECCORE                    Core;
    /** Recursion count */
    uint32_t                            cRecursion;
    /** The current owner thread. */
    RTTHREAD volatile                   hThread;
    /** Pointer to the lock record below us. Only accessed by the owner. */
    R3R0PTRTYPE(PRTLOCKVALRECUNION)     pDown;
    /** Pointer back to the shared record. */
    R3R0PTRTYPE(PRTLOCKVALRECSHRD)      pSharedRec;
#if HC_ARCH_BITS == 32
    /** Reserved. */
    RTHCPTR                             pvReserved;
#endif
    /** Source position where the lock was taken. */
    RTLOCKVALSRCPOS                     SrcPos;
} RTLOCKVALRECSHRDOWN;
AssertCompileSize(RTLOCKVALRECSHRDOWN, HC_ARCH_BITS == 32 ? 24 + 16 : 32 + 32);
/** Pointer to a RTLOCKVALRECSHRDOWN. */
typedef RTLOCKVALRECSHRDOWN *PRTLOCKVALRECSHRDOWN;

/**
 * Record recording the shared ownership of a lock.
 *
 * This is typically part of the per-lock data structure when compiling with
 * the lock validator.
 */
typedef struct RTLOCKVALRECSHRD
{
    /** Record core with RTLOCKVALRECSHRD_MAGIC as the magic value. */
    RTLOCKVALRECCORE                    Core;
    /** The lock sub-class. */
    uint32_t volatile                   uSubClass;
    /** The lock class. */
    RTLOCKVALIDATORCLASS                hClass;
    /** Pointer to the lock. */
    RTHCPTR                             hLock;
    /** The lock name. */
    R3R0PTRTYPE(const char *)           pszName;
    /** Pointer to the next sibling record.
     * This is used to find the write side of a read-write lock.  */
    R3R0PTRTYPE(PRTLOCKVALRECUNION)     pSibling;

    /** The number of entries in the table.
     * Updated before inserting and after removal. */
    uint32_t volatile                   cEntries;
    /** The index of the last entry (approximately). */
    uint32_t volatile                   iLastEntry;
    /** The max table size. */
    uint32_t volatile                   cAllocated;
    /** Set if the table is being reallocated, clear if not.
     * This is used together with rtLockValidatorSerializeDetectionEnter to make
     * sure there is exactly one thread doing the reallocation and that nobody is
     * using the table at that point. */
    bool volatile                       fReallocating;
    /** Whether it's enabled or not. */
    bool                                fEnabled;
    /** Alignment padding. */
    bool                                afPadding[2];
    /** Pointer to a table containing pointers to records of all the owners. */
    R3R0PTRTYPE(PRTLOCKVALRECSHRDOWN volatile *) papOwners;
#if HC_ARCH_BITS == 32
    /** Alignment padding. */
    uint32_t                            u32Alignment;
#endif
} RTLOCKVALRECSHRD;
AssertCompileSize(RTLOCKVALRECSHRD, HC_ARCH_BITS == 32 ? 24 + 20 + 4 : 40 + 24);


/** @name   Special sub-class values.
 * The range 16..UINT32_MAX is available to the user, the range 0..15 is
 * reserved for the lock validator.
 * @{ */
/** Not allowed to be taken with any other locks in the same class.
  * This is the recommended value.  */
#define RTLOCKVALIDATOR_SUB_CLASS_NONE  UINT32_C(0)
/** Any order is allowed within the class. */
#define RTLOCKVALIDATOR_SUB_CLASS_ANY   UINT32_C(1)
/** The first user value. */
#define RTLOCKVALIDATOR_SUB_CLASS_USER  UINT32_C(16)
/** @} */

/**
 * Initialize a lock validator record.
 *
 * Use RTLockValidatorRecExclDelete to deinitialize it.
 *
 * @param   pRec                The record.
 * @param   hClass              The class. If NIL, the no lock order
 *                              validation will be performed on this lock.
 * @param   uSubClass           The sub-class.  This is used to define lock
 *                              order inside the same class.  If you don't know,
 *                              then pass RTLOCKVALIDATOR_SUB_CLASS_NONE.
 * @param   pszName             The lock name (optional).
 * @param   hLock               The lock handle.
 */
RTDECL(void) RTLockValidatorRecExclInit(PRTLOCKVALRECEXCL pRec, RTLOCKVALIDATORCLASS hClass,
                                        uint32_t uSubClass, const char *pszName, void *hLock);
/**
 * Uninitialize a lock validator record previously initialized by
 * RTLockRecValidatorInit.
 *
 * @param   pRec                The record.  Must be valid.
 */
RTDECL(void) RTLockValidatorRecExclDelete(PRTLOCKVALRECEXCL pRec);

/**
 * Create and initialize a lock validator record.
 *
 * Use RTLockValidatorRecExclDestroy to deinitialize and destroy the returned
 * record.
 *
 * @return VINF_SUCCESS or VERR_NO_MEMORY.
 * @param   ppRec               Where to return the record pointer.
 * @param   hClass              The class. If NIL, the no lock order
 *                              validation will be performed on this lock.
 * @param   uSubClass           The sub-class.  This is used to define lock
 *                              order inside the same class.  If you don't know,
 *                              then pass RTLOCKVALIDATOR_SUB_CLASS_NONE.
 * @param   pszName             The lock name (optional).
 * @param   hLock               The lock handle.
 */
RTDECL(int)  RTLockValidatorRecExclCreate(PRTLOCKVALRECEXCL *ppRec, RTLOCKVALIDATORCLASS hClass,
                                          uint32_t uSubClass, const char *pszName, void *hLock);

/**
 * Deinitialize and destroy a record created by RTLockValidatorRecExclCreate.
 *
 * @param   ppRec               Pointer to the record pointer.  Will be set to
 *                              NULL.
 */
RTDECL(void) RTLockValidatorRecExclDestroy(PRTLOCKVALRECEXCL *ppRec);

/**
 * Initialize a lock validator record for a shared lock.
 *
 * Use RTLockValidatorRecSharedDelete to deinitialize it.
 *
 * @param   pRec                The shared lock record.
 * @param   hClass              The class. If NIL, the no lock order
 *                              validation will be performed on this lock.
 * @param   uSubClass           The sub-class.  This is used to define lock
 *                              order inside the same class.  If you don't know,
 *                              then pass RTLOCKVALIDATOR_SUB_CLASS_NONE.
 * @param   pszName             The lock name (optional).
 * @param   hLock               The lock handle.
 */
RTDECL(void) RTLockValidatorRecSharedInit(PRTLOCKVALRECSHRD pRec, RTLOCKVALIDATORCLASS hClass,
                                          uint32_t uSubClass, const char *pszName, void *hLock);
/**
 * Uninitialize a lock validator record previously initialized by
 * RTLockValidatorRecSharedInit.
 *
 * @param   pRec                The shared lock record.  Must be valid.
 */
RTDECL(void) RTLockValidatorRecSharedDelete(PRTLOCKVALRECSHRD pRec);

/**
 * Makes the two records siblings.
 *
 * @returns VINF_SUCCESS on success, VERR_SEM_LV_INVALID_PARAMETER if either of
 *          the records are invalid.
 * @param   pRec1               Record 1.
 * @param   pRec2               Record 2.
 */
RTDECL(int) RTLockValidatorRecMakeSiblings(PRTLOCKVALRECCORE pRec1, PRTLOCKVALRECCORE pRec2);

/**
 * Check the locking order.
 *
 * This is called by routines implementing lock acquisition.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_SEM_LV_WRONG_ORDER if the order is wrong.  Will have done all
 *          necessary whining and breakpointing before returning.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRec                The validator record.
 * @param   hThread             The handle of the calling thread.  If not known,
 *                              pass NIL_RTTHREAD and this method will figure it
 *                              out.
 * @param   pSrcPos             The source position of the lock operation.
 */
RTDECL(int)  RTLockValidatorCheckOrder(PRTLOCKVALRECEXCL pRec, RTTHREAD hThread, PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Do deadlock detection before blocking on a lock.
 *
 * @retval  VINF_SUCCESS
 * @retval  VERR_SEM_LV_DEADLOCK if blocking would deadlock.  Gone thru the
 *          motions.
 * @retval  VERR_SEM_LV_NESTED if the semaphore isn't recursive and hThread is
 *          already the owner.  Gone thru the motions.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRec                The validator record we're blocing on.
 * @param   hThread             The current thread.  Shall not be NIL_RTTHREAD!
 * @param   enmState            The sleep state.
 * @param   pvBlock             Pointer to a RTLOCKVALIDATORREC structure.
 * @param   fRecursiveOk        Whether it's ok to recurse.
 * @param   pSrcPos             The source position of the lock operation.
 */
RTDECL(int) RTLockValidatorCheckBlocking(PRTLOCKVALRECEXCL pRec, RTTHREAD hThread,
                                         RTTHREADSTATE enmState, bool fRecursiveOk,
                                         PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Do order checking and deadlock detection before blocking on a read/write lock
 * for exclusive (write) access.
 *
 * @retval  VINF_SUCCESS
 * @retval  VERR_SEM_LV_DEADLOCK if blocking would deadlock.  Gone thru the
 *          motions.
 * @retval  VERR_SEM_LV_NESTED if the semaphore isn't recursive and hThread is
 *          already the owner.  Gone thru the motions.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pWrite              The validator record for the writer.
 * @param   pRead               The validator record for the readers.
 * @param   hThread             The current thread.  Shall not be NIL_RTTHREAD!
 * @param   enmState            The sleep state.
 * @param   pvBlock             Pointer to a RTLOCKVALIDATORREC structure.
 * @param   fRecursiveOk        Whether it's ok to recurse.
 * @param   pSrcPos             The source position of the lock operation.
 */
RTDECL(int) RTLockValidatorCheckWriteOrderBlocking(PRTLOCKVALRECEXCL pWrite, PRTLOCKVALRECSHRD pRead,
                                                   RTTHREAD hThread, RTTHREADSTATE enmState, bool fRecursiveOk,
                                                   PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Do order checking and deadlock detection before blocking on a read/write lock
 * for shared (read) access.
 *
 * @retval  VINF_SUCCESS
 * @retval  VERR_SEM_LV_DEADLOCK if blocking would deadlock.  Gone thru the
 *          motions.
 * @retval  VERR_SEM_LV_NESTED if the semaphore isn't recursive and hThread is
 *          already the owner.  Gone thru the motions.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRead               The validator record for the readers.
 * @param   pWrite              The validator record for the writer.
 * @param   hThread             The current thread.  Shall not be NIL_RTTHREAD!
 * @param   enmState            The sleep state.
 * @param   pvBlock             Pointer to a RTLOCKVALIDATORREC structure.
 * @param   fRecursiveOk        Whether it's ok to recurse.
 * @param   pSrcPos             The source position of the lock operation.
 */
RTDECL(int) RTLockValidatorCheckReadOrderBlocking(PRTLOCKVALRECSHRD pRead, PRTLOCKVALRECEXCL pWrite,
                                                  RTTHREAD hThread, RTTHREADSTATE enmState, bool fRecursiveOk,
                                                  PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Check the exit order and release (unset) the ownership.
 *
 * This is called by routines implementing releasing the lock.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_SEM_LV_WRONG_RELEASE_ORDER if the order is wrong.  Will have
 *          done all necessary whining and breakpointing before returning.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRec                The validator record.
 */
RTDECL(int)  RTLockValidatorCheckAndRelease(PRTLOCKVALRECEXCL pRec);

/**
 * Check the exit order and release (unset) the shared ownership.
 *
 * This is called by routines implementing releasing the read/write lock.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_SEM_LV_WRONG_RELEASE_ORDER if the order is wrong.  Will have
 *          done all necessary whining and breakpointing before returning.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRead               The validator record.
 * @param   hThread             The handle of the calling thread.
 */
RTDECL(int)  RTLockValidatorCheckAndReleaseReadOwner(PRTLOCKVALRECSHRD pRead, RTTHREAD hThread);

/**
 * Checks and records a lock recursion.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_SEM_LV_NESTED if the semaphore class forbids recursion.  Gone
 *          thru the motions.
 * @retval  VERR_SEM_LV_WRONG_ORDER if the locking order is wrong.  Gone thru
 *          the motions.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRec                The validator record.
 * @param   pSrcPos             The source position of the lock operation.
 */
RTDECL(int) RTLockValidatorRecordRecursion(PRTLOCKVALRECEXCL pRec, PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Checks and records a lock unwind (releasing one recursion).
 *
 * This should be coupled with called to RTLockValidatorRecordRecursion.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_SEM_LV_WRONG_RELEASE_ORDER if the release order is wrong.  Gone
 *          thru the motions.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRec                The validator record.
 */
RTDECL(int) RTLockValidatorUnwindRecursion(PRTLOCKVALRECEXCL pRec);

/**
 * Checks and records a read/write lock read recursion done by the writer.
 *
 * This should be coupled with called to
 * RTLockValidatorUnwindReadWriteRecursion.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_SEM_LV_NESTED if the semaphore class forbids recursion.  Gone
 *          thru the motions.
 * @retval  VERR_SEM_LV_WRONG_ORDER if the locking order is wrong.  Gone thru
 *          the motions.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRead               The validator record for the readers.
 * @param   pWrite              The validator record for the writer.
 */
RTDECL(int) RTLockValidatorRecordReadWriteRecursion(PRTLOCKVALRECEXCL pWrite, PRTLOCKVALRECSHRD pRead, PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Checks and records a read/write lock read unwind done by the writer.
 *
 * This should be coupled with called to
 * RTLockValidatorRecordReadWriteRecursion.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_SEM_LV_WRONG_RELEASE_ORDER if the release order is wrong.  Gone
 *          thru the motions.
 * @retval  VERR_SEM_LV_INVALID_PARAMETER if the input is invalid.
 *
 * @param   pRead               The validator record for the readers.
 * @param   pWrite              The validator record for the writer.
 */
RTDECL(int) RTLockValidatorUnwindReadWriteRecursion(PRTLOCKVALRECEXCL pWrite, PRTLOCKVALRECSHRD pRead);

/**
 * Record the specified thread as lock owner and increment the write lock count.
 *
 * This function is typically called after acquiring the lock.
 *
 * @returns hThread resolved.  Can return NIL_RTHREAD iff we fail to adopt the
 *          alien thread or if pRec is invalid.
 *
 * @param   pRec                The validator record.
 * @param   hThread             The handle of the calling thread.  If not known,
 *                              pass NIL_RTTHREAD and this method will figure it
 *                              out.
 * @param   pSrcPos             The source position of the lock operation.
 */
RTDECL(RTTHREAD) RTLockValidatorSetOwner(PRTLOCKVALRECEXCL pRec, RTTHREAD hThread, PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Clear the lock ownership and decrement the write lock count.
 *
 * This is typically called before release the lock.
 *
 * @returns The thread handle of the previous owner.  NIL_RTTHREAD if the record
 *          is invalid or didn't have any owner.
 * @param   pRec                The validator record.
 */
RTDECL(RTTHREAD) RTLockValidatorUnsetOwner(PRTLOCKVALRECEXCL pRec);

/**
 * Adds an owner to a shared locking record.
 *
 * Takes recursion into account.  This function is typically called after
 * acquiring the lock.
 *
 * @param   pRead               The validator record.
 * @param   hThread             The thread to add.
 * @param   pSrcPos             The source position of the lock operation.
 */
RTDECL(void) RTLockValidatorAddReadOwner(PRTLOCKVALRECSHRD pRead, RTTHREAD hThread, PCRTLOCKVALSRCPOS pSrcPos);

/**
 * Removes an owner from a shared locking record.
 *
 * Takes recursion into account.  This function is typically called before
 * releaseing the lock.
 *
 * @param   pRead               The validator record.
 * @param   hThread             The thread to to remove.
 */
RTDECL(void) RTLockValidatorRemoveReadOwner(PRTLOCKVALRECSHRD pRead, RTTHREAD hThread);

/**
 * Gets the number of write locks and critical sections the specified
 * thread owns.
 *
 * This number does not include any nested lock/critect entries.
 *
 * Note that it probably will return 0 for non-strict builds since
 * release builds doesn't do unnecessary diagnostic counting like this.
 *
 * @returns Number of locks on success (0+) and VERR_INVALID_HANDLER on failure
 * @param   Thread          The thread we're inquiring about.
 * @remarks Will only work for strict builds.
 */
RTDECL(int32_t) RTLockValidatorWriteLockGetCount(RTTHREAD Thread);

/**
 * Works the THREADINT::cWriteLocks member, mostly internal.
 *
 * @param   Thread      The current thread.
 */
RTDECL(void) RTLockValidatorWriteLockInc(RTTHREAD Thread);

/**
 * Works the THREADINT::cWriteLocks member, mostly internal.
 *
 * @param   Thread      The current thread.
 */
RTDECL(void) RTLockValidatorWriteLockDec(RTTHREAD Thread);

/**
 * Gets the number of read locks the specified thread owns.
 *
 * Note that nesting read lock entry will be included in the
 * total sum. And that it probably will return 0 for non-strict
 * builds since release builds doesn't do unnecessary diagnostic
 * counting like this.
 *
 * @returns Number of read locks on success (0+) and VERR_INVALID_HANDLER on failure
 * @param   Thread          The thread we're inquiring about.
 */
RTDECL(int32_t) RTLockValidatorReadLockGetCount(RTTHREAD Thread);

/**
 * Works the THREADINT::cReadLocks member.
 *
 * @param   Thread      The current thread.
 */
RTDECL(void) RTLockValidatorReadLockInc(RTTHREAD Thread);

/**
 * Works the THREADINT::cReadLocks member.
 *
 * @param   Thread      The current thread.
 */
RTDECL(void) RTLockValidatorReadLockDec(RTTHREAD Thread);



/*RTDECL(int) RTLockValidatorClassCreate();*/



/**
 * Enables / disables the lock validator for new locks.
 *
 * @returns The old setting.
 * @param   fEnabled    The new setting.
 */
RTDECL(bool) RTLockValidatorSetEnabled(bool fEnabled);

/**
 * Is the lock validator enabled?
 *
 * @returns True if enabled, false if not.
 */
RTDECL(bool) RTLockValidatorIsEnabled(void);

/**
 * Controls whether the lock validator should be quiet or noisy (default).
 *
 * @returns The old setting.
 * @param   fQuiet              The new setting.
 */
RTDECL(bool) RTLockValidatorSetQuiet(bool fQuiet);

/**
 * Is the lock validator quiet or noisy?
 *
 * @returns True if it is quiet, false if noisy.
 */
RTDECL(bool) RTLockValidatorAreQuiet(void);

/**
 * Makes the lock validator panic (default) or not.
 *
 * @returns The old setting.
 * @param   fPanic              The new setting.
 */
RTDECL(bool) RTLockValidatorSetMayPanic(bool fPanic);

/**
 * Can the lock validator cause panic.
 *
 * @returns True if it can, false if not.
 */
RTDECL(bool) RTLockValidatorMayPanic(void);


RT_C_DECLS_END

/** @} */

#endif


