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
    RpcClient(const char* host, const uint16_t port);
    ~RpcClient();
    
    void sendRequest(unsigned char *request, std::size_t request_size, unsigned char *response, std::size_t response_size, std::size_t& offset);

private:
    int sockfd_;
};

} // namespace rpc

#endif // RPC_HPP
