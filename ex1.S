.syntax unified     @ https://sourceware.org/binutils/docs/as/ARM_002dInstruction_002dSet.html#ARM_002dInstruction_002dSet

.include "efm32gg.i"

/*
////////////////////////////////////////////////////////////////////////////
//
// Exception vector table
// This table contains addresses for all exception handlers
//
////////////////////////////////////////////////////////////////////////////
*/

.section .vectors

.long   stack_top               /* Top of Stack. SP gets MSP value at start  */
.long   _reset                  /* Reset Handler                */
.long   dummy_handler           /* NMI Handler                  */
.long   dummy_handler           /* Hard Fault Handler           */
.long   dummy_handler           /* MPU Fault Handler            */
.long   dummy_handler           /* Bus Fault Handler            */
.long   dummy_handler           /* Usage Fault Handler          */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* SVCall Handler               */
.long   dummy_handler           /* Debug Monitor Handler        */
.long   dummy_handler           /* Reserved                     */
.long   dummy_handler           /* PendSV Handler               */
.long   dummy_handler           /* SysTick Handler              */

/* External Interrupts */
.long   dummy_handler
.long   gpio_handler            /* GPIO even handler */
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   gpio_handler            /* GPIO odd handler */
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler
.long   dummy_handler

.section .text
/*
////////////////////////////////////////////////////////////////////////////
//
// Reset handler
// The CPU will start executing here after a reset
//
////////////////////////////////////////////////////////////////////////////
*/

.globl  _reset
.type   _reset, %function
.thumb_func
_reset:
    bl enable_GPIO_clk
    bl setup_leds
    bl setup_buttons
#ifdef IMPROVED
    @ Setup exception mask registers (privileged software) OK in ISR _reset
    cpsie i     @ Clear PRIMASK
    cpsie f     @ Clear FAULTMASK
    bl enable_GPIO_irq
    bl low_power
.Lwfi:
    wfi
    b .Lwfi
#else
.Lpoll:
    bl read_buttons
    bl update_leds
    b .Lpoll
#endif

/*
/////////////////////////////////////////////////////////////////////////////
//
// GPIO handler
// The CPU will jump here when there is a GPIO interrupt
//
/////////////////////////////////////////////////////////////////////////////
*/

.thumb_func
gpio_handler:
    push {LR}
    bl read_buttons
    bl update_leds

    ldr r0, =GPIO_BASE
    ldr r1, [r0, GPIO_IF]
    str r1, [r0, GPIO_IFC]
    pop {PC}

@/////////////////////////////////////////////////////////////////////////////

.thumb_func
dummy_handler:
    bx LR  @ do nothing

low_power:
    push {r4-r6, LR}
    ldr r4, =SCR
    mov r5, SLEEPDEEP
    orr r6, r5, SLEEPONEXIT
    str r6, [r4]
    pop {r4-r6, PC}

@ out:
@   R0 = buttons status
read_buttons:
    push {r4, LR}
    ldr r4, =GPIO_PC_BASE
    ldr r0, [r4, GPIO_DIN]
    pop {r4, PC}

@ in:
@   R0 = button status
update_leds:
    push {r4, r5, LR}
    ldr r4, =GPIO_PA_BASE
    lsl r5, r0, 8              @ Shift state from Port C pins 0-7 to Port A pins 8-15
    str r5, [r4, GPIO_DOUT]
    pop {r4, r5, PC}

enable_GPIO_clk:
    push {r4-r6, LR}                        @ Use stack for modularity. Convention R0-R3 parameters
    ldr r4, =CMU_BASE
    ldr r5, [r4, CMU_HFPERCLKEN0]!         @ Preindex and load current value of HFPERCLKENABLE
    orr r6, r5, CMU_HFPERCLKEN0_GPIO
    str r6, [r4]        @ Enable clk for GPIO
    pop {r4-r6, PC}

setup_leds:
    push {r4-r7, LR}
    ldr r4, =GPIO_PA_BASE
    mov r5, 0x2
    str r5, [r4, GPIO_CTRL]    @ HIGH drive strength 20 mA drive current per pin
    mov r6, 0x55555555
    str r6, [r4, GPIO_MODEH]   @ Set pins 8-15 Port A to Push-pull output with drive-strength set by DRIVEMODE
    mov r7, 0
    str r7, [r4, GPIO_DOUT]    @ Set LED connected to pins 8-15 Port A to low
    pop {r4-r7, PC}

setup_buttons:
    push {r4-r6, LR}
    ldr r4, =GPIO_PC_BASE
    mov r5, 0x33333333
    str r5, [r4, GPIO_MODEL]   @ Set pins 0-7 Port C input enabled with filter. DOUT determines pull direction
    mov r6, 0xff               @ Enable internal pull-up
    str r6, [r4, GPIO_DOUT]
    pop {r4-r6, PC}

@ Enables interrupt at the GPIO pins to which the buttons are connected through
@ Port C
enable_GPIO_irq:
    push {r4-r9, LR}
    @ Enable IRQ from peripheral GPIO
    ldr r4, =GPIO_BASE
    mov r5, 0x22222222
    str r5, [r4, GPIO_EXTIPSELL]   @ Map pins 0-7 Port C to external interrupts 0-7
    mov r6, 0xff
    str r6, [r4, GPIO_EXTIFALL]    @ Trigger external interrupts pins 0-7 on falling egde
    str r6, [r4, GPIO_EXTIRISE]    @ Trigger external interrupts pins 0-7 on rising edge
    str r6, [r4, GPIO_IEN]         @ Enable external interrupts pins 0-7
    @ Enable IRQ at NVIC
    ldr r7, =ISER0
    mov r8, IRQ_GPIO_EVEN  @ Enable interrupt from even #pins
    orr r9, r8, IRQ_GPIO_ODD  @ Enable interrupt from odd #pins
    str r9, [r7]
    pop {r4-r9, PC}
