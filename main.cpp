#include "rpc.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <flatbuffers/flatbuffers.h>

int main() {
    try {
        // Create socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        
        // Connect to server (replace with actual server IP and port)
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
        
        if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            throw std::runtime_error("Failed to connect to server");
        }
        
        // Create request
        RPCRequest req;
        req.type = EnumType::HELLO;
        req.payload = "World";
        
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
