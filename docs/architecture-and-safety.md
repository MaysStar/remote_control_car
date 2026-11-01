# Architecture and safety migration

## Verified baseline

Two ESP-IDF/PlatformIO firmware projects (vehicle ESP32-S3 and controller ESP32), a Python log collector, and a FastAPI/Three.js visualization service share this repository. BSP currently contains both hardware and network services; OSAL exposes BSP types; main creates runtime tasks. This is a migration baseline, not a claim that layering is complete.

## First implementation: command safety boundary

The vehicle starts stopped. Only complete non-retained MQTT messages on exact direction/speed topics are parsed. Direction accepts one digit 0–4; speed accepts 1–3 decimal digits in 0–100 and caps demand at 90. Invalid known commands, fragments, retained data, disconnects and MQTT errors clear direction and speed. Unknown complete topics are ignored.

Direction and speed have independent freshness timestamps: after 500 ms without either valid field, the next motor snapshot clears demand. Motor polling is 50 ms, so normal scheduling adds up to one cycle. This is a software bound, not a hard real-time guarantee. Zero speed bypasses PID and resets its state. Controller refreshes speed and direction every 100 ms so receiver restart does not preserve obsolete throttle. Both firmware images should be updated together.

Host tests cover parser boundaries, initialization, timeout, clearing stale throttle, disconnect stop state and tick wraparound. Mutex stubs are single-threaded: they do not validate FreeRTOS scheduling/concurrency. Firmware compilation and physical motor-stop latency still require ESP-IDF and hardware verification.

## Target boundaries and next review units

1. BSP: pins, PWM, ADC/buttons, I2C/IMU only; explicit driver contracts and error propagation.
2. Platform services: FreeRTOS, clock, Wi-Fi, MQTT, HTTP, OTA and logging adapters. OSAL must stop exporting BSP data types.
3. Domain: transport-independent command, motion/safety state machine and pose types, with host tests.
4. Application: composition root owns initialization, tasks and adapter wiring. A single motor owner applies safety decisions.
5. Replace separate legacy topics with a versioned atomic command envelope, session/sequence validation, expiry and explicit arming. Update both endpoints in one reviewed change.
6. Add sensor freshness/validity, latched faults, OTA motion interlock and health-gated watchdog.
7. Audit TLS verification, broker ACLs/identity, OTA signature/rollback policy and Python HTTP/WebSocket authentication, payload limits and rate limits.

The legacy MQTT protocol has no application authentication or replay protection. Retained rejection and timeouts do not solve broker compromise, replay, stalled motor tasks or sensor staleness. No production security or hardware safety certification is claimed.

## Hardware acceptance before driving

Lift wheels off the ground. Confirm boot stop, zero throttle with heading error, controller/broker disconnect, malformed/retained payload rejection, timeout after paused publishing, and reconnect without stale throttle. Record measured latency and firmware versions. Then test OTA and fault handling after their interlocks are implemented.

## IMU freshness gate

Motor control now requires a finite position sample younger than 100 ms. Boot, stale samples and non-finite samples force zero motor demand on the next control cycle. Timestamp and sample are read under the same mutex. Valid samples restore eligibility automatically; this is not a latched fault or explicit re-arm mechanism. Host tests cover boundary expiry, NaN/infinity, missing samples and tick wraparound. Hardware stop timing remains unverified.

## Partial command assembly correction

A motor/telemetry read between the direction and speed MQTT messages previously
cleared the first field, potentially preventing startup under unlucky task timing.
Fresh partial commands now remain pending while the returned motor demand stays
STOP. Either expired field still clears the whole command. Host regression tests
cover both arrival orders and partial expiry. This fixes the legacy transport
without pretending that two MQTT topics form an atomic protocol.

## OTA motion interlock and failure handling

An OTA request latches motion inhibition until reboot. Pending commands are erased
and new direction/speed commands are ignored. The motor task writes zero PWM,
resets its PID and acknowledges the stop. OTA waits at most 500 ms for this
acknowledgement before opening the network connection. A missing acknowledgement
prevents the update. This acknowledges a software PWM write, not measured wheel
standstill: there is no physical feedback or hardware validation in these tests.
A request racing an active motor iteration is acknowledged on the next iteration;
the OTA transfer cannot start while that earlier iteration can still drive PWM.

Only one update attempt is made per boot. Failure leaves motion inhibited and
requires an intentional reboot to recover. MQTT reconnect and stop commands do
not clear the latch. Successful completion reboots into the updated image.

OTA requires HTTPS, certificate bundle validation and hostname validation, with
streaming HTTP APIs that never follow redirects. Only HTTP 200 with a positive
Content-Length fitting the inactive partition is accepted; redirect responses,
partial responses, missing lengths and oversized images are rejected before flash
erase. Servers must serve firmware directly with Content-Length; chunked/unknown
length downloads are deliberately unsupported.
Download errors and incomplete images abort the session; esp_ota_end validates
the image before esp_ota_set_boot_partition selects it for the next boot. Each begun session is cleaned
up exactly once. Progress no longer divides by an unknown or zero image length.
A 120-second deadline is checked between blocking network operations (15-second
network timeout); it is not a hard real-time deadline.

Host tests compile the actual OTA control flow with simulated ESP-IDF networking
to exercise missing stop acknowledgement, tick wraparound, invalid URLs, begin,
network, flash-write and image-validation errors, redirects, invalid sizes,
incomplete images, timeout and success. They do not
exercise TLS on a device, flash writes, motor electronics, signed firmware,
secure boot or rollback protection. TLS validation alone does not authenticate
the firmware publisher independently of the update server.
