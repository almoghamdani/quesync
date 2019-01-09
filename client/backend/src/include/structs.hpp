#define MAX_DATA_SIZE 100

// This enum will hold all the different request types
enum request_t {
    LOGIN_REQUEST,
    REGISTER_REQUEST
};

// This struct will hold the base packet type
typedef struct packet_t {
    enum request_t type;
    const char data[MAX_DATA_SIZE];
} packet_t;