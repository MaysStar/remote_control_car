# IoT MQTT Remote Controlled Vehicle (ESP-IDF)

A distributed dual-MCU robotic system featuring an **ESP32-S3** mobile chassis controlled asynchronously via MQTT by a separate **ESP32** remote control unit. Built using the **ESP-IDF** framework with a clean, decoupled 3-level software architecture (BSP, OSAL, APP).

## System Architecture

The project implements a modular architecture to separate low-level hardware components from the high-level application logic.

## Project Structure

```text
remote_car/
├── app/               # Application Level (Business logic, MQTT parser, PID)
│   ├── app_header/
│   └── app_src/
├── bsp/               # Board Support Package (Hardware drivers: PWM, I2C, NVS)
│   ├── bsp_header/
│   └── bsp_src/
├── osal/              # Operating System Abstraction Layer (FreeRTOS wrappers)
│   ├── osal_header/
│   └── osal_src/
├── src/
│   ├── CMakeLists.txt # Advanced CMake build system rules
│   └── main.c         # System entry point (app_main)
└── platformio.ini     # Project configuration file
