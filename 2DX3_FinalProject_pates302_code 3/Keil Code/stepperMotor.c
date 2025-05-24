#include <stdint.h>
#include "SysTick.h"
#include "tm4c1294ncpdt.h"
#include "stepperMotor.h"

const int stepsPerRot = 512;
const double degPerStep = 360.0/stepsPerRot;
const int angleInSteps = 11.25/360.0 * stepsPerRot;

// Stepper motor
void PortH_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7;  // Enable clock for Port H

    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R7) == 0);  // Wait for Port H to be ready

    GPIO_PORTH_DIR_R |= 0xF;  // Set PH0-PH3 as output
    GPIO_PORTH_AFSEL_R &= ~0xF;  // Disable alternate functions
    GPIO_PORTH_DEN_R |= 0xF;  // Enable digital function
    GPIO_PORTH_AMSEL_R &= ~0xF;  // Disable analog function

    return;
}

// Stepper motor sequence for forward, CW
void DutyCycleForward(int delay) {    
    GPIO_PORTH_DATA_R = 0b00001100;
    SysTick_Wait1ms(delay);
    
    GPIO_PORTH_DATA_R = 0b00000110;
    SysTick_Wait1ms(delay);
    
    GPIO_PORTH_DATA_R = 0b00000011;
    SysTick_Wait1ms(delay);
    
    GPIO_PORTH_DATA_R = 0b00001001;
    SysTick_Wait1ms(delay);
}

// Stepper motor sequence for reverse, CCW
void DutyCycleReverse(int delay) {    
    GPIO_PORTH_DATA_R = 0b00001001;
    SysTick_Wait1ms(delay);
    
    GPIO_PORTH_DATA_R = 0b00000011;
    SysTick_Wait1ms(delay);
    
    GPIO_PORTH_DATA_R = 0b00000110;
    SysTick_Wait1ms(delay);
    
    GPIO_PORTH_DATA_R = 0b00001100;
    SysTick_Wait1ms(delay);
}

// @param number of 10ms delays, 1 = CCW, -1 = CW
void rotate(int delay, int dir) {
    if (dir == 1) {
        DutyCycleForward(delay);
    } else if (dir == -1) {
        DutyCycleReverse(delay);
    }
}

