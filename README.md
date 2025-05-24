**Spatial Mapping System (LiDAR)**


An embedded spatial mapping system that leverages the VL53L1X Time-of-Flight (ToF) sensor for precise 3D environment scanning. Built using the TI MSP432E401Y microcontroller, the system captures spatial data via I2C, transmits it over UART, and reconstructs it using Python with PyVista for 3D visualization.

Features
Real-time distance measurement using VL53L1X ToF sensor

Embedded firmware development in C and Assembly

I2C communication for sensor interfacing

UART communication for data transmission to host PC

3D environment reconstruction using Python, PyVista, and NumPy

Tech Stack
Embedded C, Assembly

TI MSP432E401Y, VL53L1X

Keil uVision (firmware development)

Python, PyVista, NumPy

AutoCAD (for structural components or scanning environment setup)

Applications
Spatial mapping and object detection

Robotics and autonomous navigation

3D modeling and scanning

