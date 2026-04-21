    .syntax unified
    .cpu cortex-m0plus
    .thumb

    // Імпортуємо змінні з твого C-файлу
    .extern currentTCB
    .extern nextTCB

    // Робимо функцію видимою для всієї системи
    .global PendSV_Handler

    .align 2 
    // 2. Кажемо лінкеру, що це ФУНКЦІЯ
    .type PendSV_Handler, %function 
    // 3. Кажемо асемблеру зробити адресу непарною для таблиці векторів!
    .thumb_func

PendSV_Handler:
    cpsid i
    mrs r0, psp

    subs r0, r0, #32

    ldr r1 , =currentTCB
    ldr r2 , [r1]
    str r0, [r2]
    
    stmia r0!, {r4-r7}

    mov r4, r8 
    mov r5, r9
    mov r6, r10
    mov r7, r11

    stmia r0!, {r4-r7}

    ldr r1, =nextTCB
    ldr r2, [r1]

    ldr r3, =currentTCB
    str r2, [r3]

    ldr r0, [r2]        

    adds r0, r0, #16    
    ldmia r0!, {r4-r7}  
    mov r8, r4
    mov r9, r5
    mov r10, r6
    mov r11, r7
    
    
    subs r0, r0, #32    
    ldmia r0!, {r4-r7}  

    adds r0,r0,#16
    msr psp, r0

    cpsie i
    
    bx lr

