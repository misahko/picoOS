#include <stdint.h>

extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;

extern uint32_t _sbss;
extern uint32_t _ebss;

extern uint32_t _estack;

extern void main(void);
extern void Reset_Handler(void);
extern void HardFault_Handler(void);

void HardFault_Handler(void) {
    while(1) {
       
    }
}

// Атрибут змушує компілятор покласти цей масив у секцію ".is_vector"
__attribute__((section(".isr_vector"),used, aligned(256)))
void (*const vector_table[])(void) = {
    (void (*)(void))(&_estack), 
    Reset_Handler,            // 1: Вектор скидання (Reset Vector)
    
    0, // 2: NMI
    HardFault_Handler, // 3: HardFault
    0, // 4: Memory Management
    0, // 5: Bus Fault
    0, // 6: Usage Fault
    0, 0, 0, 0, // 7-10: Резерв
    0, // 11: SVCall
    0, // 12: Debug Monitor
    0, // 13: Резерв
    0, // 14: PendSV
    0  // 15: SysTick
};


__attribute__((noreturn))
void Reset_Handler(void)
{
    __asm volatile (
    "ldr r0, =_estack\n"
    "mov sp, r0\n"
    ::: "r0", "memory"
    );

    *((volatile uint32_t *)0xE000ED08) = (uint32_t)vector_table;

    volatile uint32_t *src;
    volatile uint32_t *dest;

    src = &_sidata;
    dest = &_sdata;

    while (dest < &_edata) *dest++ = *src++;

    dest = &_sbss;

    while (dest < &_ebss) *dest++ = 0;

    main();
    while (1) {__asm volatile("nop");};

}