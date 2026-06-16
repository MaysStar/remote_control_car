import socket
import time
import math
from config.settings import IP_ADDRESS, PORT


udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

t = 0.0

while True:
    x = math.sin(t) * 90
    y = math.sin(t) * 2
    z = math.cos(t) * 90

    cords = f'x:{x:.4f},y:{y:.4f},z:{z:.4f}'
    packet_bytes = cords.encode('utf-8')
    
    udp_socket.sendto(packet_bytes, (IP_ADDRESS, PORT))

    t += 0.005
    time.sleep(0.016)

    