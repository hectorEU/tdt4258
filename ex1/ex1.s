        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
  // Exception vector table
  // This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
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

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
  // The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////

	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset:

bl cmu_clk_enable
bl setup_leds
bl setup_input
// bl setup_interrupts

//mov r1, 0x01 ; just for testing purpose

b loop1

loop1:
	bl read_button_status
	bl update_leds
	
	//energy optimization (sleep, etc)
	//mov r8, #6
	//ldr r2, =SCR
	//str r8, [r2]
	//wfi
	
	b loop1  // do nothing

	
	b .
	
update_leds:
	ldr r1, =GPIO_PA_BASE
	lsl r4, r4, #8
	str r4, [r1, #GPIO_DOUT]	
	bx lr
	
cmu_clk_enable:
	ldr r1, =CMU_BASE
	ldr r2, [r1, #CMU_HFPERCLKEN0]
	mov r3, #1
	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO
	orr r2, r2, r3
	str r2, [r1, #CMU_HFPERCLKEN0] 
	bx lr
	
setup_leds:
	ldr r1, =GPIO_PA_BASE
	mov r2, 0x2
	str r2, [r1, #GPIO_CTRL]	
	ldr r4, =0x55555555
	str r4, [r1, #GPIO_MODEH]	
// set all leds high	
	ldr  r4, =0b1010101000000000
	str r4, [r1, #GPIO_DOUT]
	bx lr
	
setup_input:
	ldr r6, =GPIO_PC_BASE
	ldr r5, =0x33333333
	str r5, [r6, #GPIO_MODEL]
	ldr r7, =0xff
	str r7, [r6, #GPIO_DOUT]
	bx lr
	
setup_interrupts:
	ldr r2, =GPIO_BASE
	ldr r3, =0x22222222
	str r3, [r2, #GPIO_EXTIPSELL]
	mov r8, 0xff
	str r8, [r2, #GPIO_EXTIFALL]
	str r8, [r2, #GPIO_EXTIRISE]
	str r8, [r2, #GPIO_IEN]
	ldr r9, =ISER0
	movw r8, #0x802
	str r8, [r9]		
	bx lr
	
read_button_status:
	ldr r6, =GPIO_PC_BASE	
	ldr r4, [r6, GPIO_DIN]
	bx lr   

reset_interrupt:
	ldr r2, =GPIO_BASE
	ldr r3, [r2, #GPIO_IF]
	str r3, [r2, #GPIO_IFC]
	bx lr
	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  
		//add r1,#1
		
	//	bl read_button_status
	//	bl update_leds
	//	bl reset_interrupt
		b loop1
	//	bx lr
	      b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

