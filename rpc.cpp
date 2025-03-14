#include "rpc.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <flatbuffers/flatbuffers.h>

using namespace flatbuffers;

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

RPCResponse rpc_send(int sockfd, const RPCRequest& req) {
    // Encode request
    auto builder = encodeRequest(req);
    const uint8_t* data = builder.GetBufferPointer();
    size_t len = builder.GetSize();
    
    // Create HTTP POST request
    std::string http_req = "POST /RPC HTTP/1.1\r\n";
    http_req += "Host: localhost\r\n";
    http_req += "Content-Type: application/octet-stream\r\n";
    http_req += "Content-Length: " + std::to_string(len) + "\r\n\r\n";
    
    // Send request
    send(sockfd, http_req.c_str(), http_req.length(), 0);
    send(sockfd, reinterpret_cast<const char*>(data), len, 0);
    
    // Receive response
    char buffer[1024];
    int bytes_received = recv(sockfd, buffer, 1024, 0);
    if (bytes_received < 0) {
        throw std::runtime_error("Failed to receive response");
    }
    
    // Parse HTTP response and extract body
    std::string response(buffer, bytes_received);
    size_t body_start = response.find("\r\n\r\n") + 4;
    if (body_start == std::string::npos) {
        throw std::runtime_error("Invalid HTTP response");
    }
    
    return decodeResponse(reinterpret_cast<const uint8_t&>(response[body_start]),
                          response.length() - body_start);
}
