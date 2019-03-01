import socket
import random
import string

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 61110)
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

idk = ''.join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(10))
#sock.send(('QUESYNC|002|{ "username": "almogh23", "email": "' + idk + '@gmail.com", "nickname": "sdsd", "password": "123456789" }|').encode())
#sock.send(b'QUESYNC|003|{ "nickname": "s", "tag": 9591 }|')
sock.send(b'QUESYNC|001|{ "username": "almogh52", "password": "123456789" }|')
print(sock.recv(512).decode())

sock.send(b'QUESYNC|004|{ "recipient_id": "530e9407-7c-4be4-9cc6-70ab38e6310c" }|')

print(sock.recv(512).decode())