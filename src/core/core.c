#include "core.h"
#include "../drivers/rp2040.h"

#define START_SMALL_MEMORY 0x0
#define START_MEDIUM_MEMORY 0x400
#define START_LARGE_MEMORY 0xc00

#define FZM(num) (~(num) & ((num) + 1))  
#define FZI(num) (FZM(num) ? __builtin_ctz(FZM(num)) : NONE)

#define FOM(num) (num & (-num))
#define FOI(num) (FOM(num) ? __builtin_ctz(FOM(num)) : NONE)

TCB TCBs[16];
TCB idleTCB;

TCB *currentTCB;
TCB *nextTCB;

EVENT nullEvent;

uint32_t idleStack[32] __attribute__((aligned(8)));
uint32_t stacks[7168] __attribute__((aligned(8)));
uint16_t usedTCB = 0;

uint16_t taskQueue[4] = {0,0,0,0};

uint8_t usedSmallMemory = 0;
uint8_t usedMediumMemory = 0;
uint8_t usedLargeMamory = 0;

void idleTask(void)
{
    while (1)  __asm volatile("wfi"); 
}

void schedule(void)
{
    nextTCB = &idleTCB;
    uint8_t i = 3;
    while (i > 0 && taskQueue[i] == 0) i--;
    
    if (taskQueue[i] == 0) return;

    uint8_t index = FOI(taskQueue[i]);

    nextTCB = &TCBs[index];
}

void SysTick_Handler(void)
{
    
	schedule();
    //SIO->GPIO_OUT_SET= (1 << 25);
	if (nextTCB != currentTCB) SCB_ICSR = PENDSV_SET;
}

void eventEmit(EVENT *event)
{
    uint8_t index = 0;

    while (event->queue != 0)
    {
        index = FOI(event->queue);

        event->queue &= ~(1 << index);

        TCBs[index].state = READY;

        taskQueue[TCBs[index].priority] |= (1 << index);
    }
}

void eventWait(EVENT *event)
{
    event->queue |= (1 << currentTCB->index);

    currentTCB->state = BLOCKED;

    taskQueue[currentTCB->priority] &= ~(1 << currentTCB->index);
}

void initIdleTask(void)
{
    // Вказуємо на кінець нашого масиву
    uint32_t *sp = &idleStack[32];

    // Формуємо початковий стек (16 слів)
    *(--sp) = 0x01000000;             // xPSR (Thumb mode)
    *(--sp) = (uint32_t)idleTask | 1; // PC (Адреса функції)
    *(--sp) = 0xFFFFFFFD;             // LR (Повернення в Thread Mode + PSP)

    // Заповнюємо нулями R0-R12
    for (int i = 0; i < 13; i++) *(--sp)= 0;
    
    idleTCB = (TCB){.index = 0, .priority = 0, .stackPointer = sp, .state = READY};
}

void initTask(void (*task)(void), uint8_t priority, uint8_t size)
{
    uint32_t *sp;
    uint16_t start;
    uint16_t memorySize;
    uint8_t *useMemory;

    if (size == SMALL)
    {
        start = START_SMALL_MEMORY;
        memorySize = 128;
        useMemory = &usedSmallMemory;
    }
    else if (size == MEDIUM)
    {
        start = START_MEDIUM_MEMORY;
        memorySize = 512;
        useMemory = &usedMediumMemory;
    }
    else
    {
        start = START_LARGE_MEMORY;
        memorySize = 1024;
        useMemory = &usedLargeMamory;
    }

    if (FZM(*useMemory) == NONE) return;

    uint8_t freeIndex = FZI(*useMemory);

    (*useMemory) |= (1 << freeIndex);

    sp = stacks + (freeIndex * memorySize) + memorySize + start;

    *(--sp) = 0x01000000;
    *(--sp) = (uint32_t)task | 1;
    *(--sp) = 0xFFFFFFFD;

    for (int i = 0; i < 13; i++) *(--sp)= 0;

    uint8_t tcbIndex = FZI(usedTCB);
    usedTCB |= (1 << tcbIndex);

    TCBs[tcbIndex] = (TCB){.stackPointer = sp,.sleepTime = 10 , .priority = priority%4, .state = READY, .index = tcbIndex,};
    taskQueue[priority] |= (1 << tcbIndex);
}

void systemClockInit12MHz(void)
{
    XOSC->START_UP = 47;

    XOSC->CTRL = 0xFABAA0;

    while ((XOSC->STATUS & (1 << 31)) == 0) {}

    CLOCKS->CLK_REF_CTRL = 2;
    CLOCKS->CLK_SYS_CTRL = 0;
}

void systemClockInit125MHz(void)
{
    systemClockInit12MHz();

    RESETS->RESET &= ~(1 << 12);

    while ((RESETS->RESET_DONE & (1 << 12)) == 0) {}

    PLL_SYS->FBDIV_INT = 125;

    PLL_SYS->PWR &= ~((1 << 0) | (1 << 5));

    while ((PLL_SYS->CS & (1 << 31)) == 0) {}

    // 5. Налаштовуємо дільники: 1500 MHz / (6 * 2) = 125 MHz
    // Біти [18:16] = POSTDIV1 (значення 6)
    // Біти [14:12] = POSTDIV2 (значення 2)
    PLL_SYS->PRIM = (6 << 16) | (2 << 12);

    // 6. Вмикаємо вихід PLL (скидаємо біт POSTDIVPD)
    PLL_SYS->PWR &= ~(1 << 3);

    // 7. Перемикаємо процесор на новий PLL!
    // Записуємо 1 в регістр CLK_SYS_CTRL (джерело = clk_src_pll_sys)
    CLOCKS->CLK_SYS_CTRL = 1;
}

void sysTickInit(void)
{
    // 1. Встановлюємо найнижчий пріоритет (0xFF) для PendSV (біти 16-23) 
    // та SysTick (біти 24-31). Для Cortex-M0+ це критично важливо!
    SCB_SHPR3 |= (0xFF << 16) | (0xFF << 24);

    // 2. Вимикаємо таймер для безпечного налаштування
    SYST_CSR = 0;

    // 3. Завантажуємо значення для 1 мілісекунди (при 125 MHz)
    SYST_RVR = 124999;

    // 4. Скидаємо поточний лічильник у нуль
    SYST_CVR = 0;

    // 5. Вмикаємо таймер:
    // Біт 0 (ENABLE) = 1 (увімкнути таймер)
    // Біт 1 (TICKINT) = 1 (дозволити переривання SysTick_Handler)
    // Біт 2 (CLKSOURCE) = 1 (використовувати частоту ядра, 125 МГц)
    // 1 | 2 | 4 = 7
    SYST_CSR = 7;
}

void coreInit(void)
{

    
    systemClockInit125MHz();
    

    initIdleTask();

    currentTCB = &idleTCB;

    // 4. МАГІЯ ARM: Перемикаємо процесор з MSP (системного стека) на PSP (стек задач)
    uint32_t *idle_psp = &idleStack[32];
    __asm volatile (
        "msr psp, %0 \n"      // Завантажуємо адресу стека idle-задачі в PSP
        "movs r0, #2 \n"      // Біт 1 (значення 2) у регістрі CONTROL = перехід на PSP
        "msr control, r0 \n"  // Застосовуємо налаштування
        "isb \n"              // Очищаємо конвеєр процесора (обов'язково після зміни CONTROL)
        :: "r" (idle_psp) : "r0"
    );

    // 5. Тепер БЕЗПЕЧНО запускаємо таймер ОС
    sysTickInit(); // Твоя функція, яку ми писали раніше (з SCB_SHPR3, SYST_RVR тощо)

    // 6. Стрибаємо у нескінченний цикл idle-задачі. 
    // Звідси ми більше ніколи не повернемося у main()!
    idleTask();
}