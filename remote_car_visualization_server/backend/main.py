from contextlib import asynccontextmanager
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
import asyncio, socket
from fastapi.responses import HTMLResponse
import os
from config.settings import PORT, IP_ADDRESS, ALPHA



current_data = {'x': 0.0, 'y': 0.0, 'z': 0.0}

@asynccontextmanager
async def lifespan(app: FastAPI):
    asyncio.create_task(udp_listener())
    yield

app = FastAPI(lifespan=lifespan)


async def udp_listener():

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)    
    udp_socket.bind((IP_ADDRESS, PORT))
    udp_socket.setblocking(False)

    while True:
        try:
            packet, addr = udp_socket.recvfrom(1024)
            data_str = packet.decode('utf-8')
            parts = data_str.split(',')

            for part in parts:
                if ':' in part:
                    key, val = part.split(':', 1)
                    key = key.strip()
                    
                    if key in current_data:
                        try:
                            current_data[key] = ALPHA * float(val.strip()) + (1 - ALPHA) * current_data[key]
                            
                        except ValueError:
                            pass

            await asyncio.sleep(0.001)
        
        except BlockingIOError:
            await asyncio.sleep(0.01)
            continue
    
@app.websocket('/ws/telemetry')
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()

    try:
        while True:
            await websocket.send_json(current_data)
            await asyncio.sleep(0.02)

    except WebSocketDisconnect:
        print('User left')
        


@app.get('/api/telemetry')
def get_telemetry():
    return current_data 


@app.get('/', response_class=HTMLResponse)
def read_root():
    path = os.path.join('..', 'frontend', 'index.html')
    with open(path, 'r', encoding='utf-8') as f:
        return f.read()