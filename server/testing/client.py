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
sock.send(b'QUESYNC|001|{ "username": "almogh23", "password": "123456789" }|')
print(sock.recv(512).decode())

sock.send(b'QUESYNC|007|{ "userId": "79bbcce2-01f4-4e12-b88d-c3f985d960f5" }|')
#sock.send(b'QUESYNC|005|{ "recipientId": "5464a7ba-4417-4a21-895d-0902bf4f9343" }|')

print(sock.recv(512).decode())