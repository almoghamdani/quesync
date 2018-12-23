import socket
import sys

HOST = "127.0.0.1"
PORT = 55556

connected = []

# Create a UDP socket
voice_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
voice_socket.bind((HOST, PORT))

while True:
    try:   
        # Get from a client voice chat info
        data, addr = voice_socket.recvfrom(5000)
    except KeyboardInterrupt:
        break
    except:
        continue

    # If the sender address isn't a connected client in the server, save it as one
    if addr not in connected:
        connected.append(addr)

    # Send the voice chat to info to each connected client
    for connected_client in connected:
        if connected_client != addr:
            voice_socket.sendto(connected_client, data)