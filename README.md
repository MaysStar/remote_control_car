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
remote_control_car/
│
├── remote_car/                        #  Embedded C Firmware (ESP32 - Vehicle Unit)
│   ├── app/                           # Application Level (Business logic, control loops, PID)
│   ├── bsp/                           # Board Support Package (Hardware drivers: PWM, I2C, Motors)
│   ├── osal/                          # Operating System Abstraction Layer (FreeRTOS wrappers)
│   ├── src/main.c                     # Vehicle system entry point (app_main)
│   └── platformio.ini                 # PlatformIO configuration for the vehicle board
│
├── remote_car_user_control/           #  Embedded C Firmware (ESP32 - Remote Controller Unit)
│   ├── app/                           # Application Level (Joystick parsing, command generation)
│   ├── bsp/                           # Board Support Package (Buttons, ADC for sticks, radio TX)
│   ├── lib/                           # External or shared C/C++ libraries
│   ├── osal/                          # Operating System Abstraction Layer
│   ├── src/main.c                     # Controller system entry point
│   ├── partitions.csv                 # Custom flash memory partitioning map for ESP32
│   ├── sdkconfig.esp32dev             # ESP-IDF SDK configuration variables
│   └── platformio.ini                 # PlatformIO configuration for the remote board
│
├── remote_car_logs_server/            #  Diagnostic Logging Microservice
│   └── remote_logs_server.py          # Standalone Python server for capturing and storing system events
│
└── remote_car_visualization_server/   #  Cloud Telemetry & 3D Web Dashboard
    ├── backend/main.py                # FastAPI HTTP ingestion endpoint and WebSocket broadcast
    ├── config/settings.py             # Cloud/Local environment variables and constants (e.g., ALPHA)
    ├── frontend/index.html            # Procedural Three.js 3D vehicle model and auto-reconnecting WS client
    ├── simulator.py                   # Continuous mock data generator for isolated UI testing
    └── requirements.txt               # Production Python dependencies (FastAPI, Uvicorn, Pydantic)
```
**Video demonstration of remote car**
https://github.com/user-attachments/assets/7dcfcbba-0e0f-4bac-97c0-e9eeb21efebd
