# Remote Control Car

A distributed ESP-IDF robotics project built around two microcontrollers:

- **ESP32-S3 vehicle node** — motor control, IMU processing, OTA, diagnostics, Wi-Fi/MQTT communication, and telemetry.
- **ESP32 remote-control node** — user input acquisition and command publishing.
- **Host-side tools** — UDP log receiver and a FastAPI/Three.js telemetry visualization service.

The project is currently being refactored from a prototype-oriented BSP/OSAL/APP layout into a clearer layered architecture that can support deterministic control, stronger diagnostics, additional sensors, mapping, and future autonomous navigation without coupling those features directly to ESP-IDF peripherals.

> The current firmware behavior is intentionally preserved while the architecture is migrated incrementally through reviewable pull requests.

## Current capabilities

- Four-motor drive using ESP32-S3 MCPWM
- Direction and speed commands over MQTT
- MPU6050-based roll, pitch, and yaw estimation
- Heading correction with a PID controller
- Wi-Fi connectivity and SNTP time synchronization
- HTTPS OTA update support
- UDP remote logging
- FastAPI + WebSocket telemetry backend
- Three.js browser visualization
- Separate ESP32 remote-control firmware

## Repository layout

```text
remote_control_car/
├── remote_car/                       # ESP32-S3 vehicle firmware
│   ├── app/                          # Application logic and FreeRTOS tasks
│   ├── bsp/                          # Hardware/platform-facing implementations
│   ├── osal/                         # Synchronization wrappers
│   └── src/                          # app_main composition root
├── remote_car_user_control/          # ESP32 handheld controller firmware
├── remote_car_logs_server/           # UDP log receiver
├── remote_car_visualization_server/  # FastAPI + Three.js telemetry visualization
└── docs/                             # Architecture and development documentation
```

The current layout is the **migration source**, not the final architecture. See [Architecture](docs/architecture.md) for the target dependency model and migration rules.

## Architecture direction

The refactor separates the project into five responsibilities:

```text
Application / runtime orchestration
              |
              v
Domain services and control logic
              |
              v
Abstract interfaces / platform services
              |
              v
Hardware drivers
              |
              v
ESP-IDF + physical hardware
```

The most important rule is that high-level vehicle behavior must not depend directly on ESP-IDF driver details. Hardware failures should be reported as errors/state changes instead of crashing unrelated application logic.

See:

- [docs/architecture.md](docs/architecture.md) — current state, target architecture, dependency rules
- [docs/development.md](docs/development.md) — build and development workflow
- [docs/roadmap.md](docs/roadmap.md) — staged refactor plan and future autonomy work

## Build the vehicle firmware

Requirements:

- PlatformIO
- ESP-IDF toolchain provided through the PlatformIO `espressif32` platform
- ESP32-S3 DevKitM-1 compatible target

```bash
cd remote_car
pio run
```

Upload and monitor:

```bash
pio run -t upload
pio device monitor
```

## Build the remote-control firmware

```bash
cd remote_car_user_control
pio run
```

## Development model

Changes are intentionally split into small pull requests. Architectural changes should avoid mixing unrelated behavior changes into the same PR.

Typical sequence:

```text
architecture baseline
        ↓
driver hardening
        ↓
motor-control API
        ↓
communication services
        ↓
runtime + safety supervisor
        ↓
pose / odometry
        ↓
mapping and autonomous navigation
```

## Planned direction

The long-term target is an ESP32-S3 autonomous rover platform with:

- manual control
- assisted/safe control
- deterministic motor supervision
- wheel odometry
- improved localization
- distance/LiDAR integration
- occupancy-grid mapping
- path planning
- browser-based live map and telemetry
- autonomous exploration

These features will be added only after the control and hardware layers have stable interfaces.

## License

See [LICENSE.txt](LICENSE.txt).
