#include "../include/Socket.hpp"
#include <iostream>
#include <cstring>

int init_server(ServerSocket &server) 
{
    if (server.bindSocket() < 0 || server.listenSocket(10) < 0)
        return 1;
    std::cout << "Server is listening on port 8080...\n" << std::endl;
    return 0;
}

void handle_client(int client_sock, ServerSocket &server) 
{
    char buffer[1024] = {0};

    int bytes_received = server.receiveData(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_received > 0) 
    {
        buffer[bytes_received] = '\0';
        std::cout << "Received from client: " << buffer << std::endl; 
    }
    else if (bytes_received == 0)
        std::cout << "Client disconnected." << std::endl; 
    else
        std::cerr << "Failed to receive data from client." << std::endl;

    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 15\r\n"
        "\r\n"
        "Hello, client!\n";

    int bytes_sent = server.sendData(client_sock, response, strlen(response));
    if (bytes_sent > 0)
        std::cout << "Response sent to client: HTTP/1.1 200 OK" << std::endl;
    else
        std::cerr << "Failed to send response to client." << std::endl;
    close(client_sock);
    std::cout << "Client socket closed." << std::endl;
}

int main() 
{
    ServerSocket server(8080);

    if (init_server(server))
        return -1;
    while (true) {
        int client_sock = server.acceptConnection();
        if (client_sock < 0) 
        {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        std::cout << "New client connected!" << std::endl;
        handle_client(client_sock, server);
    }
    server.closeSocket();
    return 0;
}