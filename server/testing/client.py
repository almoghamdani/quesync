import socket
import random
import string
import json

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

sock.send(b'QUESYNC|007|{ "userId": "530e9407-7cfd-4be4-9cc6-70ab38e6310c" }|')
res = sock.recv(512).decode()
print(res[12:-2])
res = json.loads(res[12:-2])
channelId = res["id"]
print(('QUESYNC|009|{ "amount": 300, "offset": 0, "channelId": "' + channelId + '" }|'))
sock.send(('QUESYNC|009|{ "amount": 100, "offset": 0, "channelId": "' + "49dc2dce-509d-46af-a563-05bef5ddcc67" + '" }|').encode())
#sock.send(('QUESYNC|008|{ "content": "TESTING", "channelId": "' + res["id"] + '" }|').encode())
#sock.send(b'QUESYNC|005|{ "recipientId": "5464a7ba-4417-4a21-895d-0902bf4f9343" }|')

print(sock.recv(2048).decode())