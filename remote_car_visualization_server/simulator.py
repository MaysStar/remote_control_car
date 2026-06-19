import time
import math
import requests

# Target URL for the cloud/local FastAPI telemetry endpoint
TARGET_URL = "http://127.0.0.1:8000/api/telemetry"


def run_telemetry_simulator():
    """Generates continuous mock Euler angles and transmits them via HTTP POST."""
    print(f"Telemetry simulator initialized. Sending data to: {TARGET_URL}")
    time_step = 0.0

    while True:
        # Simulate realistic mechanical movements within designated physical boundaries
        roll = math.sin(time_step) * 85.0       # Dynamic range constrained to [-85, 85]
        pitch = math.cos(time_step) * 85.0      # Dynamic range constrained to [-85, 85]
        yaw = math.sin(time_step * 0.5) * 180.0 # Dynamic full-circle rotation [-180, 180]

        # Construct the standardized payload expected by the Pydantic backend model
        payload = {
            "roll": round(roll, 2),
            "pitch": round(pitch, 2),
            "yaw": round(yaw, 2)
        }

        try:
            # Fire the HTTP POST request with the JSON payload
            response = requests.post(TARGET_URL, json=payload, timeout=1.0)
            
            if response.status_code == 200:
                print(f"Telemetry pushed successfully: {payload}")
            else:
                print(f"Server rejection. Status code received: {response.status_code}")
                
        except requests.exceptions.ConnectionError:
            print("Target server is currently offline. Retrying stream...")

        # Progress time and throttle generation rate to ~20Hz (every 50ms)
        time_step += 0.05
        time_sleep = time.sleep(0.05)


if __name__ == "__main__":
    run_telemetry_simulator()
