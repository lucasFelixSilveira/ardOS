#ifndef CONFIG_H
#define CONFIG_H

// Macros definidas pelo build system - NÃO dependem do compilador
#ifdef ARCH_AVR
    #define TARGET_AVR 1
    #define F_CPU 16000000UL
    #include <avr/io.h>
#else
    #define TARGET_AVR 0
#endif

#ifdef ARCH_XTENSA
    #define TARGET_ESP32 1
#else
    #define TARGET_ESP32 0
#endif

#ifdef ARCH_X86
    #define TARGET_X86 1
    #define USERSPACE_X11
#else
    #define TARGET_X86 0
#endif

// Macros de plataforma genéricas
#if TARGET_AVR
    #define LOW_HARDWARE 1
    #define HAS_STDIO 0
    #define HAS_FILESYSTEM 0
    #define MEMORY_SIZE_KB 2
#elif TARGET_ESP32
    #define LOW_HARDWARE 0
    #define HAS_STDIO 1
    #define HAS_FILESYSTEM 1
    #define MEMORY_SIZE_KB 512
#elif TARGET_X86
    #define LOW_HARDWARE 0
    #define HAS_STDIO 1
    #define HAS_FILESYSTEM 1
    #define MEMORY_SIZE_KB 1024
#endif

// Macros de features
#define ENABLE_KEYBOARD (TARGET_X86 || TARGET_ESP32)
#define ENABLE_VGA TARGET_X86
#define ENABLE_SERIAL (TARGET_AVR || TARGET_ESP32)
#define ENABLE_MULTITASKING (TARGET_ESP32 || TARGET_X86)

// Macros de debug
#ifdef DEBUG
    #define KERNEL_DEBUG 1
    #define LOG_LEVEL 2
#else
    #define KERNEL_DEBUG 0
    #define LOG_LEVEL 0
#endif

#endif
