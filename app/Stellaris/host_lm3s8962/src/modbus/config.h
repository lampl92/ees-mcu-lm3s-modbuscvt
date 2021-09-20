#ifndef _CONFIG_H_
#define _CONFIG_H_


#define PLATFORM_MCU  1
#define PLATFORM_X86  2

#define PLATFORM_USED PLATFORM_MCU

#if PLATFORM_USED == PLATFORM_X86

#define platform_malloc malloc
#define platform_free free

#elif PLATFORM_USED == PLATFORM_MCU

#define platform_malloc malloc
#define platform_free free

#endif

#endif //_CONFIG_H_
