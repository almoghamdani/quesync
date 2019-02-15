#define MAX_DATA_SIZE 100

// This enum will hold all the different request types
enum quesync_request_t {
    LOGIN_REQUEST,
    REGISTER_REQUEST
};

// This struct will hold the base packet type
typedef struct quesync_packet_t {
    enum quesync_request_t type;
    char data[MAX_DATA_SIZE];
} quesync_packet_t;