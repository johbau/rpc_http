#ifndef RPC_HPP
#define RPC_HPP

#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib> // for std::exit

#define MAX_SIZE 16384

namespace rpc {

class RpcClient {
public:
    struct ConnectionParams {
        const char* host;
        uint16_t port;
    };

    RpcClient(const ConnectionParams& params);
    ~RpcClient();
    
    void sendRequest(unsigned char *request, std::size_t request_size, unsigned char *response, std::size_t response_size, std::size_t& offset);

private:
    int sockfd_;
    
    void createSocketConnection(const ConnectionParams& params);
};

} // namespace rpc

#endif // RPC_HPP
