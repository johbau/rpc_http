#ifndef SOCKET_CLIENT_HPP
#define SOCKET_CLIENT_HPP

#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib> // for std::exit

class SocketClient {
public:
    struct ConnectionParams {
        const char* host;
        uint16_t port;
    };

    SocketClient(const ConnectionParams& params);
    ~SocketClient();
    
    int getSocket() const { return sockfd_; }
    
private:
    int sockfd_;
    
    void createSocketConnection(const ConnectionParams& params);
};

#endif // SOCKET_CLIENT_HPP
