import socket
import sys

HOST = "0.0.0.0"
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

    print(addr)

    # If the sender address isn't a connected client in the server, save it as one
    if b"THIS_IS_PORT_MESSAGE" in data and addr not in connected:
        connected.append(addr)

    # Send the voice chat to info to each connected client
    for connected_client in connected:
        if connected_client[0] != addr[0]:
            voice_socket.sendto(data, connected_client)