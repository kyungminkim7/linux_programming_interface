#include <limits.h>
#include <stddef.h>

#define SERVER_KEY 0x1aaaaaa1

struct RequestMsg {
    long type;
    int clientId;
    char pathname[PATH_MAX];
};

#define REQUEST_MSG_DATA_SIZE \
    (offsetof(struct RequestMsg, pathname) - \
     offsetof(struct RequestMsg, clientId) + PATH_MAX)

#define RESPONSE_MSG_DATA_SIZE 8192

struct ResponseMsg {
    long type;
    char data[RESPONSE_MSG_DATA_SIZE];
};

#define RESPONSE_MSG_FAILURE 1
#define RESPONSE_MSG_DATA 2
#define RESPONSE_MSG_END 3
