#include "socket_client.hpp"
#include "rpc.hpp"
#include <iostream>
#include <flatbuffers/flatbuffers.h>

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

FlatBufferBuilder encodeRequest(const RPCRequest& req) {
    FlatBufferBuilder builder(1024);
    
    // Create string
    auto payload = builder.CreateString(req.payload);
    
    // Create request
    auto request = CreateRPCRequest(builder, static_cast<int>(req.type), payload);
    
    // Finalize and get data
    builder.Finish(request);
    return builder;
}

RPCResponse decodeResponse(const uint8_t* data, size_t len) {
    RPCResponse response;
    const char* error = verifyRPCResponse(data, len);
    if (error) {
        throw std::runtime_error(std::string("Flatbuffers error: ") + error);
    }
    
    const auto& resp = GetRPCResponse(data);
    for (auto it = resp.results()->begin(); it != resp.results()->end(); ++it) {
        response.results.push_back((*it)->str());
    }
    return response;
}

int main() {
    try {
        // Create and encode request
        RPCRequest req;
        req.type = EnumType::HELLO;
        req.payload = "World";
        auto builder = encodeRequest(req);
        const uint8_t* data = builder.GetBufferPointer();
        size_t len = builder.GetSize();
        
        // Send request and get response
        RPCResponse resp = rpc_send(sockfd, req);
        
        // Print response
        std::cout << "Received " << resp.results.size() << " strings:" << std::endl;
        for (const auto& str : resp.results) {
            std::cout << str << std::endl;
        }
        
        close(sockfd);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
