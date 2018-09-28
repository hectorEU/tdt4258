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
    bl enable_GPIO_irq        @Set up GPIO interrupts
    bl low_power              @Initiate power-saving features
.Lwfi:
    wfi                       @Wait for interrupts
    b .Lwfi                   @Do nothing
#else
.Lpoll:                       @If baseline solution, use polling
    bl read_buttons
    bl update_leds
    b .Lpoll                  @Do nothing
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
    push {LR}             @Push Link-Register to stack
    bl read_buttons       @Read_buttons
    bl update_leds        @Update LEDs

    ldr r0, =GPIO_BASE
    ldr r1, [r0, GPIO_IF]   @Set GPIO Interrupt flag
    str r1, [r0, GPIO_IFC]  @Clear GPIO Interrupt flag
    pop {PC}              @Pop Link-Register address into Program Counter Register

@/////////////////////////////////////////////////////////////////////////////

.thumb_func
dummy_handler:
    bx LR  @ do nothing

low_power:
    push {LR}
    bl RAM_block        @Disable unused ram_block to reduce current leakage
    bl disable_LFCLK    @Disable Low-Frequency Clock
    bl sleepmode        @Setup SleepMode
    pop {PC}


@ out:
@   R0 = buttons status
read_buttons:
    push {r4, LR}
    ldr r4, =GPIO_PC_BASE
    ldr r0, [r4, GPIO_DIN]      @Load the values of GPIO input buttons into r0
    pop {r4, PC}

@ in:
@   R0 = button status
update_leds:
    push {r4, r5, LR}
    ldr r4, =GPIO_PA_BASE
    lsl r5, r0, 8         @ Shift state from Port C pins 0-7 to Port A pins 8-15
    str r5, [r4, GPIO_DOUT] @Store the shifted values of GPIO inputs to GPIO LED outputs
    pop {r4, r5, PC}

enable_GPIO_clk:
    push {r4-r6, LR}                        @ Use stack for modularity. Convention R0-R3 parameters
    ldr r4, =CMU_BASE
    ldr r5, [r4, CMU_HFPERCLKEN0]!          @Preindex and load current value of HFPERCLKENABLE
    orr r6, r5, CMU_HFPERCLKEN0_GPIO
    str r6, [r4]                            @Enable CLK for GPIO
    pop {r4-r6, PC}

setup_leds:
    push {r4-r7, LR}
    ldr r4, =GPIO_PA_BASE
    mov r5, 0x1
    str r5, [r4, GPIO_CTRL]    @ LOWEST drive strength  0.5mA drive current per pin
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

sleepmode:
    push {r4-r6, LR}
    ldr r4, =SCR
    mov r5, SLEEPDEEP         @Power-saving sleepmode M2
    orr r6, r5, SLEEPONEXIT   @When exiting an event, go back to deepsleep
    str r6, [r4]
    pop {r4-r6, PC}

RAM_block:
    push {r4-r6, LR}
    mov r6, #7                  @POWERDOWN bits are 0 - 2 = 7 decimal
    ldr r4, =EMU_BASE           @Load register Energy Management Unit
    str r6, [r4, #EMU_MEMCTRL]  @Store the disabled bit values in register EMU with offset 0x004 for
    pop {r4-r6, PC}

disable_LFCLK:
  push {r4-r5, LR}
  ldr r4, =CMU_BASE               @Clock Management Unit
  mov r5, 0
  str r5, [r4, #CMU_LFCLKSEL]     @Set all bits to zero, disable all
  pop {r4-r5, PC}



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
    mov r8, IRQ_GPIO_EVEN          @ Enable interrupt from even #pins
    orr r9, r8, IRQ_GPIO_ODD       @ Enable interrupt from odd #pins
    str r9, [r7]
    pop {r4-r9, PC}
