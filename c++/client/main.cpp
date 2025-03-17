#include "rpc.hpp"
#include "../hello_generated.h"
#include "flatbuffers/flatbuffers.h"
#include <string>
#include <vector>
#include <iostream>

using namespace rpc;
using namespace Hello;
using namespace flatbuffers;

int main() {
    // Create an RPC client
    std::cerr << "Create RpcClient" << std::endl;
    RpcClient client("127.0.0.1", 7777);

    // Create a FlatBuffer request
    std::cerr << "Create flatbuffers builder" << std::endl;
    FlatBufferBuilder fbb;

    // Create a HelloRequest with GREETING type and message
    std::cerr << "Create request" << std::endl;
    RequestType request_type = RequestType_GREETING;
    auto message = fbb.CreateString("Hello, world!");
    auto request = CreateHelloRequest(fbb, request_type, message);
    fbb.Finish(request);

    // Send request and get response
    std::cerr << "Make RPC call" << std::endl;
    const std::size_t size = MAX_SIZE;
    unsigned char response[size] = {0};
    std:size_t offset = 0;
    try {
        client.sendRequest(fbb.GetBufferPointer(), fbb.GetSize(), response, size, offset);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Verify the response
    std::cerr << "Verify response" << std::endl;
    auto verifier = Verifier(response + offset, size - offset);
    if (VerifyHelloResponseBuffer(verifier)) {
        auto helloResponse = GetHelloResponse(response + offset);
        auto result = helloResponse->result();
        std::cerr << "Received response with " << result->size() << " strings." << std::endl;
    } else {
        std::cerr << "No valid response received." << std::endl;
    }

    return 0;
}
