import socket

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 61110)
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

sock.send(b"QUESYNC|000|almogh52|123456789|")

print(sock.recv(512).decode())