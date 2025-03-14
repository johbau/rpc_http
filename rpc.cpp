#include "socket_client.hpp"
#include "rpc.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>

void SocketClient::createSocketConnection(const ConnectionParams& params) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params.port);
    inet_pton(AF_INET, params.host, &server_addr.sin_addr);
    
    if (connect(sockfd_, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        throw std::runtime_error("Failed to connect to server");
    }
}

SocketClient::SocketClient(const ConnectionParams& params) {
    createSocketConnection(params);
}

SocketClient::~SocketClient() {
    close(sockfd_);
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
