#include "../include/Socket.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <cstdlib>

Socket::Socket() : _socket_fd(-1) 
{
    memset(&_addr, 0, sizeof(_addr));
}

Socket::~Socket() 
{
    closeSocket();
}

int Socket::sendData(int target_sock, const char *data, unsigned int len) 
{
    int bytes_sent = send(target_sock, data, len, 0);
    if (bytes_sent < 0)
        perror("send failed");
    return bytes_sent;
}

int Socket::receiveData(int target_sock, char *buffer, unsigned int len)
{
    int bytes_received = recv(target_sock, buffer, len, 0);
    if (bytes_received < 0)
        perror("recv failed");
    return bytes_received;
}

void Socket::closeSocket() 
{
    if (_socket_fd >= 0) 
    {
        if (close(_socket_fd) < 0)
            perror("Socket close failed");
        _socket_fd = -1;
    }
}

/*------ Server Socket ------*/


ServerSocket::ServerSocket(int port) 
{
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd < 0) 
    {
        perror("Socket creation failed");
        exit(-1);
    }

    int opt = 1;
    if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        perror("setsockopt failed");
        exit(-1);
    }

    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(port);
}

int ServerSocket::bindSocket() 
{
    if (bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) 
    {
        perror("bind failed");
        return -1;
    }
    return 0;
}

int ServerSocket::listenSocket(int backlog) 
{
    if (listen(_socket_fd, backlog) < 0)
    {
        perror("listen failed");
        return -1;
    }
    return 0;
}

std::string ServerSocket::getClientIP(struct sockaddr_in *client_addr)
{
    unsigned char *ip = reinterpret_cast<unsigned char *>(&(client_addr->sin_addr.s_addr));
    
    std::stringstream ss;
    ss << static_cast<int>(ip[0]) << "."
       << static_cast<int>(ip[1]) << "."
       << static_cast<int>(ip[2]) << "."
       << static_cast<int>(ip[3]);
    
    return ss.str();  
}

int ServerSocket::acceptConnection() 
{
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_sock = accept(_socket_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_sock < 0)
        perror("accept failed");
    else
    {
        std::string clien_Ip = getClientIP(&client_addr);
        std::cout << "Client connected: " << clien_Ip << std::endl;
    }
    return client_sock;
}

/*------ Client Socket ------*/

ClientSocket::ClientSocket(const std::string &host, int port) 
{
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd < 0) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::stringstream port_stream;
    port_stream << port;
    std::string port_str = port_stream.str();

    int ret = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res);
    if (ret != 0) 
    {
        std::cerr << "getaddrinfo failed: " << gai_strerror(ret) << std::endl;
        exit(EXIT_FAILURE);
    }

    memcpy(&_addr, res->ai_addr, sizeof(sockaddr_in));

    freeaddrinfo(res);
}

void ClientSocket::connectToServer() 
{
    if (connect(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) 
    {
        perror("Connection failed");
        exit(-1);
    }
    std::cout << "Connected to server." << std::endl;
}