#include "include/socket-manager.hpp"

using std::cout;
using std::endl;

udp::socket SocketManager::CreateUDPSocket()
{
    // Create a IPv4 UDP socket with a random port (it is specified by the 0 in the port parameter)
    return udp::socket(udp::v4(), 0);
}

udp::resolver::results_type SocketManager::ConnectToUDPServer(const char *ipAddress, int port)
{
    udp::resolver resolver(io_context);

    // Try to resolve the server's entry and get the endpoints of the server
    udp::resolver::results_type endpoints = resolver.resolve(udp::v4(), ipAddress, std::to_string(port).c_str());

    return endpoints;
}

/*static uv_loop_t *eventLoop;
static bool run = false;

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) 
{
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

void runLoop(uv_loop_t *loop)
{
    // Run the loop infinity times until the thread is closed
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // If the loop has active handles, active it
        if (run)
        {
            // Run the event loop in it's default mode
            uv_run(loop, UV_RUN_ONCE);

            // Reset the boolean since the loop has finished
            run = false;
        }
    }
}

void on_tcp_connect (uv_connect_t* req, int status)
{
    // Print successful connection to the server
    cout << "Successfully connected to the server!" << endl;
}

void SocketManager::InitSocketManager()
{
    int socketError = 0;
    uv_thread_t loopThread;

    // Setting the event loop as the default event loop and starting it
    eventLoop = uv_default_loop();
    cout << "Event loop Initialized!" << endl;

    // Create a thread that will run the event loop
    uv_thread_create(&loopThread, (uv_thread_cb)runLoop, eventLoop);
}

void SocketManager::InitReadFunction(uv_udp_t *socket, uv_udp_recv_cb readFunction)
{
    int socketError = 0;

    // Start receiving on the socket and setting the callback function as the read function
    if((socketError = uv_udp_recv_start(socket, alloc_buffer, readFunction)))
    {
        throw SocketError("Unable to open the stream for receiving!", socketError);
    }
    cout << "Receiving on socket enabled!" << endl;

    // Start the event loop
    run = true;
}

void SocketManager::CreateUDPSocket(uv_udp_t *socket)
{
    int socketError = 0;
    struct sockaddr_in addr;

    // Initialize the send socket
    cout << "Initializing UDP socket.." << endl;
    if((socketError = uv_udp_init(eventLoop, socket))) {
        throw SocketError("Unable to initiate UDP Send socket!", socketError);
    }
    cout << "UDP Socket successfully initialized!" << endl;

    // Bind the socket to a random port (port 0 generates a random port)
    uv_ip4_addr("0.0.0.0", 0, (sockaddr_in *)&addr);
    uv_udp_bind(socket, (const struct sockaddr *)&addr, 0);
}

void SocketManager::CreateTCPSocket(uv_tcp_t *socket, const char *serverIP, const int port)
{
    uv_connect_t* connect;
    int socketError = 0;
    struct sockaddr_in server_addr;

    // Initialize the TCP socket
    cout << "Initializing TCP socket.." << endl;
    if((socketError = uv_tcp_init(eventLoop, socket))) {
        throw SocketError("Unable to initiate TCP socket!", socketError);
    }
    cout << "TCP Socket successfully initialized!" << endl;

    // Allocate memory for the connect request type
    connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));

    // Set dest server IP and port
    uv_ip4_addr(serverIP, port, &server_addr);

    // Connect to the server
    if ((socketError = uv_tcp_connect(connect, socket, (const struct sockaddr *)&server_addr, on_tcp_connect)))
    {
        throw SocketError("Unable to connect to the server!", socketError);
    }

    // Start the event loop
    run = true;
}

void SocketManager::Send(uv_tcp_t *socket, void *data, unsigned int nbytes)
{
    uv_write_t *write_req;
    uv_buf_t *buf;

    // Allocate memory for the write req and the buffer
    write_req = (uv_write_t *)malloc(sizeof(uv_write_t));
    buf = (uv_buf_t *)malloc(sizeof(uv_buf_t));

    // Allocate buffer's base to hold the data
    alloc_buffer(NULL, nbytes, buf);

    // Copy all data to buffer
    memcpy(buf->base, data, nbytes);

    // Write to the TCP socket
    uv_write(write_req, (uv_stream_t *)socket, buf, 1, NULL);
}*/