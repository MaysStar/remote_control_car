import os
import asyncio
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse
from pydantic import BaseModel
from .config.settings import ALPHA

app = FastAPI()

# Data structure for incoming hardware telemetry validation
class Telemetry(BaseModel):
    roll: float
    pitch: float
    yaw: float

# In-memory storage for smoothed telemetry data
# Constraints: roll [-85, 85], pitch [-85, 85], yaw [-180, 180]
current_data = {'roll': 0.0, 'pitch': 0.0, 'yaw': 0.0}


@app.post('/api/telemetry')
async def receive_telemetry(data: Telemetry):
    """Accepts raw telemetry from the car via HTTP POST and applies an EMA filter."""
    current_data['roll'] = ALPHA * data.roll + (1 - ALPHA) * current_data['roll']
    current_data['pitch'] = ALPHA * data.pitch + (1 - ALPHA) * current_data['pitch']
    current_data['yaw'] = ALPHA * data.yaw + (1 - ALPHA) * current_data['yaw']
    
    # Send a confirmation response back to the micro-controller
    return {"status": "success"}


@app.websocket('/ws/telemetry')
async def websocket_endpoint(websocket: WebSocket):
    """Streams the filtered telemetry data to the 3D frontend at ~50Hz."""
    await websocket.accept()
    try:
        while True:
            await websocket.send_json(current_data)
            await asyncio.sleep(0.02)
    except WebSocketDisconnect:
        print('Client disconnected from telemetry stream')


@app.get('/', response_class=HTMLResponse)
def read_root():
    """Serves the primary 3D wireframe visualization webpage using a foolproof absolute path."""
    # Get the exact folder where main.py lives (backend/)
    current_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Build a solid path to the index.html
    path = os.path.join(current_dir, '..', 'frontend', 'index.html')
    
    with open(path, 'r', encoding='utf-8') as f:
        return f.read()