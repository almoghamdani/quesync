import socket
from threading import Thread

"""This is the main server class"""
class Server(object):
    SERVER_LISTEN_PORT = "0.0.0.0"
    SERVER_MAIN_PORT = 61110
    BUFFER_SIZE = 1024

    """This function initiates the server object"""
    def __init__(self):
        # Create a listening port of TCP/IP type
        self.listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Bind the listen socket to listen to all traffic on 61110 port
        self.listen_socket.bind((self.SERVER_LISTEN_PORT, self.SERVER_MAIN_PORT))

        # Init the list of connected clients
        self.connected_clients = []

    """This function starts the server by listening to incoming clients"""
    def start(self):
        # Start listening to incoming connections with 1 client in queue
        self.listen_socket.listen(1)
        
        # Main loop of the server
        while True:
            # Accept of a connection of a client and print it
            conn_socket, addr = self.listen_socket.accept()
            print(f"Incoming connection from {addr[0]}")

            # If the client is already connected, ignore his connection
            if addr[0] in self.connected_clients:
                # Close socket and continue with the loop
                conn_socket.close()
                continue

            # Add the client to the connected clients
            self.connected_clients.append(addr[0])

            # Create a thread that will handle the client
            Thread(target=self.handle_client, args=(conn_socket, addr))

    def handle_client(self, conn_socket, addr):
        # Main loop of handling each client
        while True:
            # Get data from the client
            data = conn_socket.recv(self.BUFFER_SIZE)