#include <stdint.h>
#include "vl53l1x_api.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "tm4c1294ncpdt.h"
#include "SysTick.h"
#include "lidar.h"

// Global variables
volatile int IntCount;    // Interrupt count variable
int status = 0;           // Status variable for function returns
uint16_t dev = 0x52;      // I2C device address for the VL53L1X sensor

// Define whether the device operates in interrupt mode
#define isInterrupt 1  // 1 = Interrupt mode, 0 = Polling mode

/**
 * Initializes the VL53L1X Lidar sensor.
 */
void lidar_Init(void) {
	uint8_t byteData, sensorState = 0, myByteArray[10] = {0xFF};  // Array to store sensor data
	uint16_t wordData;
	uint8_t ToFSensor = 1; // 0 = Left, 1 = Center (default), 2 = Right
	
	// Print initialization message
	int mynumber = 1;
	sprintf(printf_buffer, "2DX3 Final Project Code %d\r\n", mynumber);
	UART_printf(printf_buffer);

	// Read and store the model ID and module type from the sensor
	status = VL53L1_RdByte(dev, 0x010F, &byteData);  // Read Model ID (expected 0xEA)
	myByteArray[0] = byteData;

	status = VL53L1_RdByte(dev, 0x0110, &byteData);  // Read Module Type (expected 0xCC)
	myByteArray[1] = byteData;

	// Read word data and sensor ID
	status = VL53L1_RdWord(dev, 0x010F, &wordData);
	status = VL53L1X_GetSensorId(dev, &wordData);

	// Print sensor details
	sprintf(printf_buffer, "Model_ID=0x%x , Module_Type=0x%x\r\n", myByteArray[0], myByteArray[1]);
	UART_printf(printf_buffer);

	// Wait for the sensor to boot up
	while (sensorState == 0) {
		status = VL53L1X_BootState(dev, &sensorState);  // Check boot status
		SysTick_Wait10ms(1);  // Small delay
	}

	FlashAllLEDs();  // Indicate boot completion

	// Clear any existing interrupts before starting
	status = VL53L1X_ClearInterrupt(dev);

	// Initialize the sensor with default settings
	status = VL53L1X_SensorInit(dev);
	Status_Check("SensorInit", status);

	// Start the ranging process
	status = VL53L1X_StartRanging(dev);
	Status_Check("StartRanging", status);
}

// Global variables for Lidar data
uint16_t Distance;
uint8_t dataReady;
uint8_t RangeStatus;
uint16_t SignalRate;
uint16_t AmbientRate;
uint16_t SpadNum;

void outputLidarData(int stepCounter, double xDistMM, double degPerStep) {
	// Wait until data is ready
	while (dataReady == 0) {
		status = VL53L1X_CheckForDataReady(dev, &dataReady);
	}

	dataReady = 0;  // Reset flag

	// Read ranging data from the sensor
	status = VL53L1X_GetRangeStatus(dev, &RangeStatus);
	status = VL53L1X_GetDistance(dev, &Distance);

	// Clear the interrupt to enable the next reading
	status = VL53L1X_ClearInterrupt(dev);

	// Print the Lidar data (x position, distance, angle)
	sprintf(printf_buffer, "%f, %u, %f\r\n", xDistMM, Distance, stepCounter * degPerStep);
	UART_printf(printf_buffer);
}

/**
 * Initializes Port G for Lidar XSHUT (shutdown/reset) control.
 */
void PortG_Init(void) {
	// Enable clock for Port G
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R6;  
	while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R6) == 0) {};  // Wait for clock stabilization

	GPIO_PORTG_DIR_R &= 0x00;  // Set PG0 as input (High-Z)
	GPIO_PORTG_AFSEL_R &= ~0x01;  // Disable alternate functions on PG0
	GPIO_PORTG_DEN_R |= 0x01;  // Enable digital I/O on PG0
	GPIO_PORTG_AMSEL_R &= ~0x01;  // Disable analog functionality on PG0
}

/**
 * Performs a hardware reset on the VL53L1X sensor using the XSHUT pin (PG0).
 */
void VL53L1X_XSHUT(void) {
	GPIO_PORTG_DIR_R |= 0x01;  // Set PG0 as output
	GPIO_PORTG_DATA_R &= 0b11111110;  // Set PG0 LOW (sensor shutdown)

	FlashAllLEDs();  // Indicate reset with LEDs
	SysTick_Wait10ms(10);  // Wait for 100ms

	GPIO_PORTG_DIR_R &= ~0x01;  // Set PG0 back to input (High-Z)
}


