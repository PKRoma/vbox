/*
 * context switching
 * 2003-10 by SONE Takeshi
 */
#include <etherboot.h>

#include "segment.h"
#include "context.h"

#define MAIN_STACK_SIZE 16384
#define IMAGE_STACK_SIZE 4096

static void start_main(void); /* forward decl. */
void __exit_context(void); /* assembly routine */

/*
 * Main context structure 
 * It is placed at the bottom of our stack, and loaded by assembly routine
 * to start us up.
 */
struct context main_ctx __attribute__((section (".initctx"))) = {
    .gdt_base = (uint32_t) gdt,
    .gdt_limit = GDT_LIMIT,
    .cs = FLAT_CS,
    .ds = FLAT_DS,
    .es = FLAT_DS,
    .fs = FLAT_DS,
    .gs = FLAT_DS,
    .ss = FLAT_DS,
    .esp = (uint32_t) ESP_LOC(&main_ctx),
    .eip = (uint32_t) start_main,
    .return_addr = (uint32_t) __exit_context,
};

/* This is used by assembly routine to load/store the context which
 * it is to switch/switched.  */
struct context *__context = &main_ctx;

#if 0
/* Stack for loaded ELF image */
static uint8_t image_stack[IMAGE_STACK_SIZE];
#endif

/* Pointer to startup context (physical address) */
unsigned long __boot_ctx;

/*
 * Main starter
 * This is the C function that runs first.
 */
static void start_main(void)
{
    int retval;
    extern int filo(void);

    /* Save startup context, so we can refer to it later.
     * We have to keep it in physical address since we will relocate. */
    __boot_ctx = virt_to_phys(__context);

    /* Start the real fun */
    retval = filo();

    /* Pass return value to startup context. Bootloader may see it. */
    boot_ctx->eax = retval;

    /* Returning from here should jump to __exit_context */
    __context = boot_ctx;
}

/* Setup a new context using the given stack.
 */
struct context *
init_context(uint8_t *stack, uint32_t stack_size, int num_params)
{
    struct context *ctx;

    ctx = (struct context *)
	(stack + stack_size - (sizeof(*ctx) + num_params*sizeof(uint32_t)));
    memset(ctx, 0, sizeof(*ctx));

    /* Fill in reasonable default for flat memory model */
    ctx->gdt_base = virt_to_phys(gdt);
    ctx->gdt_limit = GDT_LIMIT;
    ctx->cs = FLAT_CS;
    ctx->ds = FLAT_DS;
    ctx->es = FLAT_DS;
    ctx->fs = FLAT_DS;
    ctx->gs = FLAT_DS;
    ctx->ss = FLAT_DS;
    ctx->esp = virt_to_phys(ESP_LOC(ctx));
    ctx->return_addr = virt_to_phys(__exit_context);

    return ctx;
}

/* Switch to another context. */
struct context *switch_to(struct context *ctx)
{
    struct context *save, *ret;

    save = __context;
    __context = ctx;
    asm ("pushl %cs; call __switch_context");
    ret = __context;
    __context = save;
    return ret;
}

#if 0
//We will use elf_start in Etherboot
/* Start ELF Boot image */
uint32_t start_elf(uint32_t entry_point, uint32_t param)
{
    struct context *ctx;

    ctx = init_context(image_stack, sizeof image_stack, 1);
    ctx->eip = entry_point;
    ctx->param[0] = param;
    ctx->eax = 0xe1fb007;
    ctx->ebx = param;

    ctx = switch_to(ctx);
    return ctx->eax;
}
#endif
