import time
import math
import requests

# Target URL for the cloud/local FastAPI telemetry endpoint
TARGET_URL = "https://remotecontrolcar-production.up.railway.app/api/telemetry"

def run_telemetry_simulator():
    """Generates continuous mock Euler angles and transmits them using HTTP Keep-Alive session."""
    print(f"Telemetry simulator initialized. Streaming to: {TARGET_URL}")
    time_step = 0.0

    # Initialize a persistent session to reuse the same TCP/TLS connection
    with requests.Session() as session:
        # Enforce HTTP Keep-Alive header to eliminate handshake latency
        session.headers.update({"Connection": "keep-alive"})

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
                # session.post keeps the connection warm, skipping handshakes
                response = session.post(TARGET_URL, json=payload, timeout=0.5)
                
                if response.status_code == 200:
                    print(f"Telemetry pushed successfully: {payload}")
                else:
                    print(f"Server rejection. Status code received: {response.status_code}")
                    
            except requests.exceptions.RequestException as e:
                print(f"Network lag or error: {e}. Retrying stream...")
                time.sleep(1.0) # Wait a bit before flooding again on error

            # Progress time and throttle generation rate
            time_step += 0.05
            time.sleep(0.04) # Throttled slightly to compensate for transatlantic physics

if __name__ == "__main__":
    run_telemetry_simulator()