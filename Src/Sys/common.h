#ifndef _COMMON_H
#define _COMMON_H

#ifndef __TYPEDEF__
    #ifndef _STD_USING_INT_TYPES
        #define _SYS_INT_TYPES_H
        #ifndef _STD_USING_BIT_TYPES
            #define __int8_t_defined
            typedef signed char         int8_t;
            typedef signed short        int16_t;
        #endif

        typedef unsigned char       uint8_t;
        typedef unsigned short      uint16_t;
        typedef signed long         int32_t;
        typedef unsigned long       uint32_t;

        typedef signed char int_least8_t;
        typedef signed short int_least16_t;
        typedef signed long int_least32_t;
        typedef unsigned char uint_least8_t;
        typedef unsigned short uint_least16_t;
        typedef unsigned long uint_least32_t;
    #endif

    typedef unsigned short      MD_STATUS;
    #define __TYPEDEF__
#endif

#ifndef uint64_t 
    typedef unsigned long  long  uint64_t; 
#endif


// #define UNIT_MULTI 1 //
#ifdef UNIT_MULTI
    #define UNIT_SLAVE_START 1
    #define UNIT_SALVE_END   31
    #define CLK_HSI_16MHz
    // #define CLK_HSI_20MHz_PLL_120MHz
#else
    // #define CLK_HSI_16MHz
    #define CLK_HSE_10MHz_PLL_120MHz
    // #define CLK_HSI_20MHz_PLL_120MHz
#endif





#define ON      ((uint8_t)0x01)
#define OFF     ((uint8_t)0x00)
#define TRUE    ((uint8_t)0x01)
#define FALSE   ((uint8_t)0x00)
#define NULL	((uint8_t)0)

#define DI_ORG  __clrpsw_i()    //disable interrupt
#define EI_ORG	__setpsw_i()    //enable interrupt

#define InterruptDisable()  __clrpsw_i()    //disable interrupt
#define InterruptEnable()	__setpsw_i()    //enable interrupt
#define	NOP_ORG	__nop()








//bool enum
typedef enum
{
    false,
    true

}bool;








void logout(const char* string,uint32_t data);




#endif
