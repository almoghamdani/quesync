#include "include/socket-manager.hpp"

using std::cout;
using std::endl;

static uv_loop_t *eventLoop;

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
        // If the loop has active handles, active it
        if (uv_loop_alive(loop))
        {
            // Run the event loop in it's default mode
            uv_run(loop, UV_RUN_ONCE);
        }
    }
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