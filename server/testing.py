import socket
import sys
import opuslib

HOST = "127.0.0.1"
PORT = 55556

# Create a UDP socket
listen_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
listen_socket.bind((HOST, PORT))

decoder = opuslib.Decoder(48000, 2)

while 1:
    data, addr = listen_socket.recvfrom(5000)
    print(data)
    print(decoder.decode(data, 2880, False))
    #print(data)