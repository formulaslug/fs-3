/*
 * Nuvoton NUC472 GCC linker script file
 */

#include "../NUC472_mem.h"

#if !defined(MBED_CONF_TARGET_BOOT_STACK_SIZE)
#  if defined(MBED_BOOT_STACK_SIZE)
#    define MBED_CONF_TARGET_BOOT_STACK_SIZE MBED_BOOT_STACK_SIZE
#  else
#    define MBED_CONF_TARGET_BOOT_STACK_SIZE 0x400
#  endif
#endif

M_CRASH_DATA_RAM_SIZE = 0x100;
StackSize = MBED_CONF_TARGET_BOOT_STACK_SIZE;

MEMORY
{
    
  VECTORS (rx)          : ORIGIN = MBED_CONFIGURED_ROM_BANK_IROM1_START, LENGTH = 0x00000400
  FLASH (rx)            : ORIGIN = MBED_CONFIGURED_ROM_BANK_IROM1_START + 0x400, LENGTH = MBED_CONFIGURED_ROM_BANK_IROM1_SIZE - 0x00000400
  RAM_INTERN (rwx)      : ORIGIN = MBED_CONFIGURED_RAM_BANK_IRAM1_START, LENGTH = MBED_CONFIGURED_RAM_BANK_IRAM1_SIZE
#if NU_ENABLE_EBIRAM1
  RAM_EXTERN (rwx)      : ORIGIN = MBED_CONFIGURED_RAM_BANK_EBIRAM1_START, LENGTH = MBED_CONFIGURED_RAM_BANK_EBIRAM1_SIZE
#endif
}

/**
 * Must match cmsis_nvic.h
 */
__vector_size = 4 * (16 + 142);
 
/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 * Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 * __exidx_start
 * __exidx_end
 * __etext
 * __data_start__
 * __preinit_array_start
 * __preinit_array_end
 * __init_array_start
 * __init_array_end
 * __fini_array_start
 * __fini_array_end
 * __data_end__
 * __bss_start__
 * __bss_end__
 * __end__
 * end
 * __HeapLimit
 * __StackLimit
 * __StackTop
 * __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
    .isr_vector :
    {
        __vector_table = .;
        KEEP(*(.vector_table))
         . = ALIGN(8);
    } > VECTORS


    .text :
    {

        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
       __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
       __exidx_end = .;
    } > FLASH

    /* .stack section doesn't contains any symbols. It is only
     * used for linker to reserve space for the main stack section
     * WARNING: .stack should come immediately after the last secure memory
     * section.  This provides stack overflow detection. */
    .stack (NOLOAD):
    {
        __StackLimit = .;
        *(.stack*);
        . += StackSize - (. - __StackLimit);
    } > RAM_INTERN

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ADDR(.stack) + SIZEOF(.stack);
    __StackLimit = ADDR(.stack);
    PROVIDE(__stack = __StackTop);

    /* Relocate vector table in SRAM */
    .isr_vector.reloc (NOLOAD) :
    {
        . = ALIGN(1 << LOG2CEIL(__vector_size));
        PROVIDE(__start_vector_table__ = .);
        . += __vector_size;
        PROVIDE(__end_vector_table__ = .);
    } > RAM_INTERN

    .crash_data_ram :
    {
        . = ALIGN(8);
        __CRASH_DATA_RAM__ = .;
        __CRASH_DATA_RAM_START__ = .; /* Create a global symbol at data start */
        KEEP(*(.keep.crash_data_ram))
        *(.m_crash_data_ram)     /* This is a user defined section */
        . += M_CRASH_DATA_RAM_SIZE;
        . = ALIGN(8);
        __CRASH_DATA_RAM_END__ = .; /* Define a global symbol at data end */
    } > RAM_INTERN

    .data :
    {
        PROVIDE( __etext = LOADADDR(.data) );

        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(8);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(8);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(8);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        /* All data end */
        . = ALIGN(32);
        __data_end__ = .;

    } >RAM_INTERN AT>FLASH


    .uninitialized (NOLOAD):
    {
        . = ALIGN(32);
        __uninitialized_start = .;
        *(.uninitialized)
        KEEP(*(.keep.uninitialized))
        . = ALIGN(32);
        __uninitialized_end = .;
    } > RAM_INTERN

    .bss.extern (NOLOAD):
    {
        __bss_extern_start__ = .;
        /**
         * Place large .bss* sections into external SRAM if internal SRAM is insufficient.
         * Such memory arrangement requires initializing .bss.extern section to zero in startup file. Check startup fiile in cmsis-core-* for support or not.
         */
        *lwip_*.o(.bss*)
        *lwip_*.o(COMMON)
        *mesh_system.o(.bss*)
       __bss_extern_end__ = .;
#if NU_ENABLE_EBIRAM1
    } > RAM_EXTERN
#else
    } > RAM_INTERN
#endif

    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM_INTERN

#if NU_ENABLE_EBIRAM1
    .heap (NOLOAD):
    {
        __end__ = .;
        end = __end__;
        *(.heap*);
        . += (ORIGIN(RAM_EXTERN) + LENGTH(RAM_EXTERN) - .);
        __HeapLimit = .;
    } > RAM_EXTERN
#else
    .heap (NOLOAD):
    {
        __end__ = .;
        end = __end__;
        *(.heap*);
        . += (ORIGIN(RAM_INTERN) + LENGTH(RAM_INTERN) - .);
        __HeapLimit = .;
    } > RAM_INTERN
#endif
    PROVIDE(__heap_size = SIZEOF(.heap));
    PROVIDE(__mbed_sbrk_start = ADDR(.heap));
    PROVIDE(__mbed_krbs_start = ADDR(.heap) + SIZEOF(.heap));
    
}
