#include <stdint.h>
#include <stdbool.h>
#include "tm4c1294ncpdt.h"
#include "SysTick.h"
#include "onboardLEDs.h"

#define DELAY 1  // Define delay time for LED flashing

// Function to flash LED1 (PN1) a specified number of times
void FlashLED1(int count) {
	while (count--) {
		GPIO_PORTN_DATA_R ^= 0b00000010;  // Toggle LED1 (PN1)
		SysTick_Wait1ms(DELAY);           // Delay for stability
		GPIO_PORTN_DATA_R ^= 0b00000000;  // Turn LED1 off
		SysTick_Wait1ms(DELAY);           // Delay for stability
	}
}

// Function to flash LED2 (PN0) a specified number of times
void FlashLED2(int count) {
	while (count--) {
		GPIO_PORTN_DATA_R ^= 0b00000001;  // Toggle LED2 (PN0)
		SysTick_Wait1ms(DELAY);           // Delay for stability
		GPIO_PORTN_DATA_R ^= 0b00000000;  // Turn LED2 off
		SysTick_Wait1ms(DELAY);           // Delay for stability
	}
}

// Function to flash LED3 (PF4) a specified number of times
void FlashLED3(int count) {
	while (count--) {
		GPIO_PORTF_DATA_R ^= 0b00010000;  // Toggle LED3 (PF4)
		SysTick_Wait1ms(DELAY);           // Delay for stability
		GPIO_PORTF_DATA_R ^= 0b00000000;  // Turn LED3 off
		SysTick_Wait1ms(DELAY);           // Delay for stability
	}
}

// Function to flash LED4 (PF0) a specified number of times
void FlashLED4(int count) {
	while (count--) {
		GPIO_PORTF_DATA_R ^= 0b00000001;  // Toggle LED4 (PF0)
		SysTick_Wait1ms(DELAY);           // Delay for stability
		GPIO_PORTF_DATA_R ^= 0b00000000;  // Turn LED4 off
		SysTick_Wait1ms(DELAY);           // Delay for stability
	}
}

// Function to control distance indicator LED (PF4)
// If `state` is true, turn LED on; otherwise, turn it off
void setLED(bool state) {
	if (state) {
		GPIO_PORTF_DATA_R = 0b00010000; // Turn PF4 LED on
		SysTick_Wait1ms(DELAY);
	} else {
		GPIO_PORTF_DATA_R = 0b00000000; // Turn PF4 LED off
		SysTick_Wait1ms(DELAY);
	}
}

// Function to flash all onboard LEDs at once
void FlashAllLEDs() {
	GPIO_PORTN_DATA_R ^= 0b00000011; // Toggle both PN0 and PN1 LEDs
	GPIO_PORTF_DATA_R ^= 0b00010001; // Toggle both PF4 and PF0 LEDs
	SysTick_Wait1ms(25);            // 25ms delay
	GPIO_PORTN_DATA_R ^= 0b00000011; // Turn off both PN0 and PN1 LEDs
	GPIO_PORTF_DATA_R ^= 0b00010001; // Turn off both PF4 and PF0 LEDs
	SysTick_Wait1ms(25);            // 25ms delay
}

// Function to flash LED1 once (indicating I2C transmit)
void FlashI2CTx() {
	FlashLED1(1);
}

// Function to flash LED4 once (indicating I2C receive)
void FlashI2CRx() {
	FlashLED4(1);
}

// Function to flash all LEDs a specified number of times (for I2C error indication)
void FlashI2CError(int count) {
	while (count--) {
		FlashAllLEDs();
	}
}

// Function to initialize onboard LEDs on Port N and Port F
void onboardLEDs_Init(void) {
	// Enable clock for Port N
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;
	while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12) == 0) {}; // Wait for clock to stabilize

	// Configure PN0 and PN1 as output
	GPIO_PORTN_DIR_R |= 0x03;        // Set PN0 and PN1 as output
	GPIO_PORTN_AFSEL_R &= ~0x03;     // Disable alternate function
	GPIO_PORTN_DEN_R |= 0x03;        // Enable digital function
	GPIO_PORTN_AMSEL_R &= ~0x03;     // Disable analog functionality

	// Enable clock for Port F
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5) == 0) {}; // Wait for clock to stabilize

	// Configure PF0 and PF4 as output
	GPIO_PORTF_DIR_R |= 0x11;        // Set PF0 and PF4 as output
	GPIO_PORTF_AFSEL_R &= ~0x11;     // Disable alternate function
	GPIO_PORTF_DEN_R |= 0x11;        // Enable digital function
	GPIO_PORTF_AMSEL_R &= ~0x11;     // Disable analog functionality

	// Flash all LEDs once at startup for testing
	FlashAllLEDs();
}


