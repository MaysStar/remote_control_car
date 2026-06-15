import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Log server start: {UDP_PORT}... press Ctrl+C for exit")

while True:
    try:
        data, addr = sock.recvfrom(2048)  # wait for the packet 
        log_text = data.decode('utf-8', errors='ignore')
        print(f"[{addr[0]}]: {log_text}", end='')  # preint in console
    except KeyboardInterrupt:
        print("\n Stop server.")
        break

sock.close()