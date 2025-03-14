package main

import (
	"fmt"
	"os"
	"net/http"
	"bufio"

	"github.com/google/flatbuffers/go"
	"github.com/johbau/rpc_http/go/Hello"
)

func rpcHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Only POST requests are allowed", http.StatusMethodNotAllowed)
		return
	}

	contentType := r.Header.Get("Content-Type")
	if contentType != "application/octet-stream" {
		http.Error(w, "Only application/octet-stream content type is allowed", http.StatusUnsupportedMediaType)
		return
	}

	// Read the request body
	reader := bufio.NewReader(r.Body)
	buf := make([]byte, 16384)
	_, err := reader.Read(buf)
	if err != nil {
		fmt.Printf("Error reading request body: %v\n", err)
		http.Error(w, "Failed to read request body", http.StatusInternalServerError)
		return
	}

    // Process the request using FlatBuffers schema
    request := Hello.GetRootAsHelloRequest(buf, 0)
    if request == nil {
        fmt.Printf("Failed to parse request\n")
        return
    }

    requestType := request.RequestType()
    switch requestType {
    case Hello.RequestTypeGREETING:
        fmt.Printf("request GREETING\n")
    case Hello.RequestTypeGOODBYE:
        fmt.Printf("request GOODBYE\n")
    default:
        fmt.Printf("unknown request\n")
    }
    message := string(request.Message())
    fmt.Printf("message: %s\n", message)

    // Create a buffer for FlatBuffers
    builder := flatbuffers.NewBuilder(16384)

    // Create the response using the Flatbuffers schema
    result1 := builder.CreateString("Hello client")
    Hello.HelloResponseStartResultVector(builder, 1)
    builder.PrependUOffsetT(result1)
    result := builder.EndVector(1)
    Hello.HelloResponseStart(builder)
    Hello.HelloResponseAddResult(builder, result)
    response := Hello.HelloResponseEnd(builder)
    builder.Finish(response)

	// Set response headers
	w.Header().Set("Content-Type", "application/octet-stream")

	// Write the response body
	if _, err := w.Write(builder.FinishedBytes()); err != nil {
		fmt.Printf("Error writing response: %v\n", err)
		http.Error(w, "Failed to write response", http.StatusInternalServerError)
		return
	}
}

func main() {
	fmt.Println("Starting RPC server on :7777")
	
	// Create a new HTTP server
	srv := &http.Server{
		Addr:    ":7777",
		Handler: nil,
	}
	
	// Register the handler for /RPC endpoint
	http.HandleFunc("/RPC", rpcHandler)
	
	// Start the server
	if err := srv.ListenAndServe(); err != nil {
		fmt.Printf("Failed to start server: %v\n", err)
	    os.Exit(1)
	}
}
