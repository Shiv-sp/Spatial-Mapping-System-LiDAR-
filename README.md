**Spatial Mapping System (LiDAR)**
This project is a 3D spatial mapping system that uses a VL53L1X distance sensor and a TI MSP432E401Y microcontroller to scan and map the environment. The sensor collects distance data, which is sent to a computer using UART. A Python script then turns this data into a 3D visual using PyVista.

**Key Features**
- Uses a Time-of-Flight (ToF) sensor for accurate distance measurements

- Runs on the MSP432E401Y microcontroller with code written in C and Assembly

- Sends data to a computer using UART

- Displays the environment in 3D using Python and PyVista
