package main

import (
	"log"
	"net/http"
	"bufio"
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
	buf := make([]byte, 1024)
	n, err := reader.Read(buf)
	if err != nil {
		log.Printf("Error reading request body: %v", err)
		http.Error(w, "Failed to read request body", http.StatusInternalServerError)
		return
	}

	// Set response headers
	w.Header().Set("Content-Type", "application/octet-stream")

	// Write the response body
	if _, err := w.Write(buf[:n]); err != nil {
		log.Printf("Error writing response: %v", err)
		http.Error(w, "Failed to write response", http.StatusInternalServerError)
		return
	}
}

func main() {
	log.Println("Starting RPC server on :7777")
	
	// Create a new HTTP server
	srv := &http.Server{
		Addr:    ":7777",
		Handler: nil,
	}
	
	// Register the handler for /RPC endpoint
	http.HandleFunc("/RPC", rpcHandler)
	
	// Start the server
	if err := srv.ListenAndServe(); err != nil {
		log.Fatalf("Failed to start server: %v", err)
	}
}
