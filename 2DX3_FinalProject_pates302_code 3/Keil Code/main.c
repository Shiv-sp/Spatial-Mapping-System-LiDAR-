/*
Shiv Patel 
pates302
400530101

Assigned Bus Speed: 16 MHz

Digital I/O LED Status: Measurement: PF4 (LED D3), UART Tx: PN1 (LED D1), Additional (UART Recieved): PF0 (LED D4)
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"
#include "SysTick.h"
#include "PLL.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "i2c.h"
#include "lidar.h"
#include "startStop.h"
#include "stepperMotor.h"

// Function to wait for a start signal over UART
// The function continuously reads from UART until the received character is '1' (0x31 in ASCII)
void waitForStart() {
    int input = UART_InChar();  // Read a character from UART

    while (input != 0x31) {     // Keep reading until '1' is received
        input = UART_InChar();
    }
}

// Global variable to track x displacement in millimeters
double xDistance = 0.0; 

int main(void) {
    // Initialize all necessary peripherals
    StartStop_Init();   // Initialize start/stop button (Port J)
    PortH_Init();       // Initialize GPIO Port H for user-defined LEDs
    PLL_Init();         // Set system clock to 12 MHz using PLL
    SysTick_Init();     // Initialize SysTick timer for delays
    onboardLEDs_Init(); // Initialize onboard LEDs (e.g., D1, D2, D3)
    I2C_Init();         // Initialize I2C protocol for sensor communication
    UART_Init();        // Initialize UART for serial communication (Tx/Rx)
    lidar_Init();       // Initialize VL53L1X Lidar sensor

    // Motor and control variables
    int direction = -1;   // Motor rotation direction (-1 = reverse, 1 = forward)
    int delay = 2;        // Delay between motor steps (affects speed)
    int counter = 0;      // Keeps track of number of steps taken

    bool run = false;     // Flag to indicate if system is currently running

    // Turn off all Port H LEDs initially
    GPIO_PORTH_DATA_R = 0b00000000;

    // Send message to UART terminal to prompt user to press start button
    UART_printf("Press GPIO PJ1 to start\n");

    // Variables to detect button state changes for toggling run mode
    uint16_t startStop = 1, prevStartStop = 1;

    while (1) {
        // Read the current state of GPIO PJ1 (Start/Stop button)
        startStop = GPIO_PORTJ_DATA_R & 0x02;

        // If a button press is detected (0 -> 1 transition)
        if ((!prevStartStop && startStop) && !run) {
            UART_printf("Starting\n");  // Inform user that system is starting
            run = true;                 // Set run flag
        } else if ((!prevStartStop && startStop) && run) {
            UART_printf("Stopping\r\n");  // Inform user system is stopping
            run = false;                  // Clear run flag
        }

        // Main operation when system is running
        if (run) {
            outputLidarData(counter, xDistance, degPerStep); // Send Lidar scan data to UART
            rotate(delay, direction);  // Rotate motor one step
            counter++;                 // Track how many steps have been taken

            // Blink a LED every full angular step sweep
            if (counter % angleInSteps == 0) {
                setLED(true);   // Turn LED on
            } else {
                setLED(false);  // Turn LED off
            }
        }

        // Once a full rotation (e.g., 512 steps) is completed
        if (counter == stepsPerRot) {
            run = false;           // Stop the motor
            counter = 0;           // Reset counter for next scan
            xDistance += 333;      // Move x-coordinate by 333mm (simulate lateral shift)
            setLED(false);         // Ensure LED is turned off

            // Reverse direction and rotate back to origin to prevent cable tangling
            for (int i = 0; i < 512; i++) {
                rotate(delay, -direction);  // Return to original position
            }
        }

        // Update previous state of the button for edge detection
        prevStartStop = startStop;
    }
}


/*
// Initialize Port L (example/test code)
void PortL_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R10;  // Enable clock for Port L
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R10) == 0);  // Wait for Port L to be ready

    GPIO_PORTL_DIR_R |= 0x01;  // Set PL0 as output
    GPIO_PORTL_DEN_R |= 0x01;  // Enable digital function for PL0
    GPIO_PORTL_DATA_R &= ~0x01;  // Initialize to LOW
}

int main(void) {
    PortL_Init();  // Initialize PL0 (used for toggling output pin)
    
    while (1) {
        GPIO_PORTL_DATA_R |= 0x01;  // Set PL0 HIGH
        SysTick_Wait1ms(10);        // Wait 10ms
        GPIO_PORTL_DATA_R &= ~0x01; // Set PL0 LOW
        SysTick_Wait1ms(10);        // Wait 10ms
    }
}
*/
