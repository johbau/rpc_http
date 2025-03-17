#include "rpc.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace rpc;

RpcClient::RpcClient(const char* host, const uint16_t port) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &server_addr.sin_addr);

    if (connect(sockfd_, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        throw std::runtime_error("Failed to connect to server");
    }
}

RpcClient::~RpcClient() {
    close(sockfd_);
}

void dumpData(const char *buffer, int length) {
    for (int i = 0; i < length; i++) std::cout << buffer[i];
}

void RpcClient::sendRequest(unsigned char *request, std::size_t request_size, unsigned char *response, std::size_t response_size, std::size_t& offset) {
    // Create HTTP POST request
    std::string http_req = "POST /RPC HTTP/1.1\r\n";
    http_req += "Host: localhost\r\n";
    http_req += "Content-Type: application/octet-stream\r\n";
    http_req += "Content-Length: " + std::to_string(request_size) + "\r\n\r\n";
    
    // Send request
    send(sockfd_, http_req.c_str(), http_req.length(), 0);
    dumpData(http_req.c_str(), http_req.length());
    send(sockfd_, reinterpret_cast<const char*>(request), request_size, 0);
    dumpData(reinterpret_cast<const char*>(request), request_size);

    // Receive response
    int bytes_received = recv(sockfd_, response, response_size, 0);
    if (bytes_received < 0) {
        throw std::runtime_error("Failed to receive response");
    }
    std::cerr << "sendRequest() ";
    std::cerr << "bytes_received=" << bytes_received << std::endl;
    dumpData(reinterpret_cast<const char*>(response), bytes_received);

    // Parse HTTP response and extract body
    std::string header(reinterpret_cast<const char*>(response), bytes_received);
    std::cerr << "sendRequest() ";
    std::cerr << "received header:" << std::endl;
    std::cerr << header << std::endl;
    offset = header.find("\r\n\r\n") + 4;
    if (offset == std::string::npos) {
        throw std::runtime_error("Invalid HTTP response");
    }

    std::cerr << "sendRequest() ";
    std::cerr << "request_size=" << request_size << " ";
    std::cerr << "response_size=" << response_size << " ";
    std::cerr << "offset=" << offset << std::endl;
}
