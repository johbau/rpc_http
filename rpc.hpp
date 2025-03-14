#ifndef RPC_HPP
#define RPC_HPP

#include <flatbuffers/flatbuffers.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum class EnumType {
    HELLO,
    GOODBYE
};

struct RPCRequest {
    EnumType type;
    std::string payload;
};

struct RPCResponse {
    std::vector<std::string> results;
};

// Encode RPC request using flatbuffers
flatbuffers::FlatBufferBuilder encodeRequest(const RPCRequest& req);

// Decode RPC response from flatbuffer data
RPCResponse decodeResponse(const uint8_t* data, size_t len);

// Send RPC request and get response
RPCResponse rpc_send(int sockfd, const RPCRequest& req);

#endif // RPC_HPP
