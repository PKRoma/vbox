#ifndef _IPXE_MALLOC_H
#define _IPXE_MALLOC_H

#include <stdint.h>

/** @file
 *
 * Dynamic memory allocation
 *
 */

FILE_LICENCE ( GPL2_OR_LATER );

/*
 * Prototypes for the standard functions (malloc() et al) are in
 * stdlib.h.  Include <ipxe/malloc.h> only if you need the
 * non-standard functions, such as malloc_dma().
 *
 */
#include <stdlib.h>
#include <ipxe/tables.h>
#include <valgrind/memcheck.h>

extern size_t freemem;

extern void * __malloc alloc_memblock ( size_t size, size_t align );
extern void free_memblock ( void *ptr, size_t size );
extern void mpopulate ( void *start, size_t len );
extern void mdumpfree ( void );

/**
 * Allocate memory for DMA
 *
 * @v size		Requested size
 * @v align		Physical alignment
 * @ret ptr		Memory, or NULL
 *
 * Allocates physically-aligned memory for DMA.
 *
 * @c align must be a power of two.  @c size may not be zero.
 */
static inline void * __malloc malloc_dma ( size_t size, size_t phys_align ) {
	void * ptr = alloc_memblock ( size, phys_align );
	if ( ptr && size )
		VALGRIND_MALLOCLIKE_BLOCK ( ptr, size, 0, 0 );
	return ptr;
}

/**
 * Free memory allocated with malloc_dma()
 *
 * @v ptr		Memory allocated by malloc_dma(), or NULL
 * @v size		Size of memory, as passed to malloc_dma()
 *
 * Memory allocated with malloc_dma() can only be freed with
 * free_dma(); it cannot be freed with the standard free().
 *
 * If @c ptr is NULL, no action is taken.
 */
static inline void free_dma ( void *ptr, size_t size ) {
	free_memblock ( ptr, size );
	VALGRIND_FREELIKE_BLOCK ( ptr, 0 );
}

/** A cache discarder */
struct cache_discarder {
	/**
	 * Discard some cached data
	 *
	 * @ret discarded	Number of cached items discarded
	 */
	unsigned int ( * discard ) ( void );
};

/** Cache discarder table */
#define CACHE_DISCARDERS __table ( struct cache_discarder, "cache_discarders" )

/** Declare a cache discarder */
#define __cache_discarder __table_entry ( CACHE_DISCARDERS, 01 )

#endif /* _IPXE_MALLOC_H */
