# IoT MQTT Remote Controlled Vehicle (ESP-IDF)

A distributed dual-MCU robotic system featuring an **ESP32-S3** mobile chassis controlled asynchronously via MQTT by a separate **ESP32** remote control unit. Built using the **ESP-IDF** framework with a clean, decoupled 3-level software architecture (BSP, OSAL, APP).

**Remote car system: hardware, real-time visualization, and user remote control unit**
<img width="1280" height="814" alt="top_car_and_romote_photo" src="https://github.com/user-attachments/assets/6e994290-5a0b-4c58-9713-1d8ee2496997" />
<img width="964" height="900" alt="side_car_and_romote_photo" src="https://github.com/user-attachments/assets/e2bdc93d-d40f-45c7-82f5-a8849794c62d" />
<img width="1833" height="897" alt="car_visualization" src="https://github.com/user-attachments/assets/d9bd3b79-fdc5-473a-99d9-c40914a3b464" />

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
```
**Video demonstration of remote car**
https://github.com/user-attachments/assets/7dcfcbba-0e0f-4bac-97c0-e9eeb21efebd
