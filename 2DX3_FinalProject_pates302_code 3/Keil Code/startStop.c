#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "startStop.h"

// Enable global interrupts
void EnableInt(void){
	__asm("    cpsie   i\n");  // Assembly instruction to enable interrupts (set I-bit in PRIMASK)
}

// Disable global interrupts
void DisableInt(void){
	__asm("    cpsid   i\n");  // Assembly instruction to disable interrupts (clear I-bit in PRIMASK)
}

// Enter low power mode and wait for interrupt
void WaitForInt(void){
	__asm("    wfi\n");  // Assembly instruction to enter low power mode and wait for interrupt
}

// GPIO Port J = Vector 67
// Bit in interrupt register = 51
void StartStop_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;  // Enable clock for Port J
	while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R8) == 0){};  // Wait for clock to stabilize
	
	GPIO_PORTJ_DIR_R &= ~0x02;    // Configure PJ1 as input

	GPIO_PORTJ_DEN_R |= 0x02;     // Enable digital functionality on PJ1
	GPIO_PORTJ_PCTL_R &= ~0x000000F0; // Configure PJ1 as GPIO (clear PCTL bits)
	GPIO_PORTJ_AMSEL_R &= ~0x02;	// Disable analog functionality on PJ1	
	GPIO_PORTJ_PUR_R |= 0x02;		// Enable weak pull-up resistor on PJ1

	// Configure PJ1 for falling edge-triggered interrupts
	GPIO_PORTJ_IS_R &= ~0x02;     // Set PJ1 as edge-sensitive (not level-sensitive)
	GPIO_PORTJ_IBE_R &= ~0x02;    // Enable single edge triggering (not both edges)
	GPIO_PORTJ_IEV_R &= ~0x02;    // Configure falling edge trigger for PJ1
	GPIO_PORTJ_ICR_R = 0x02;      // Clear any previous interrupt flag on PJ1
	GPIO_PORTJ_IM_R |= 0x02;      // Enable (arm) interrupt on PJ1

	// Set interrupt priority (priority 5)
	// Correcting a potential typo: using NVIC_PRI12 instead of NVIC_PRI13
	NVIC_PRI12_R = (NVIC_PRI12_R & 0xFF00FFFF) | 0xA0000000;  // Set priority 5 for interrupt 67

	NVIC_EN1_R |= 0x00080000;  // Enable interrupt 67 (Port J) in the NVIC
	EnableInt();  // Enable global interrupts
}

// Interrupt Service Routine (ISR) for GPIO Port J
// This function is executed when an interrupt occurs on PJ1
void GPIOJ_IRQHandler(void){
	GPIO_PORTJ_ICR_R = 0x02;  // Acknowledge and clear interrupt flag for PJ1
}


