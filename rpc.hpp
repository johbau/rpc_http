#ifndef RPC_HPP
#define RPC_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Encode RPC request using flatbuffers
flatbuffers::FlatBufferBuilder encodeRequest(const RPCRequest& req);

// Send RPC request and get response
RPCResponse rpc_send(int sockfd, const RPCRequest& req);

// Send RPC request and get response
RPCResponse rpc_send(int sockfd, const RPCRequest& req);

#endif // RPC_HPP
