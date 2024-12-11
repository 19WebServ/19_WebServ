#include "Socket.hpp"

Socket::Socket(int port, unsigned int ip = INADDR_ANY)
{
    this->_server_sock = -1;
    memset(&this->_server_addr, 0, sizeof(this->_server_addr));
    this->_server_addr.sin_family = AF_INET;
    this->_server_addr.sin_port = htons(port);          // Port d'écoute
    this->_server_addr.sin_addr.s_addr = htonl(ip);
}

Socket::~Socket()
{
    this->closeSocket();
}

int Socket::createSocket()
{
    this->_server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (!this->_server_sock)
        std::cerr << "Error\n Socket creation failure" << std::endl;
    return this->_server_sock;
}

int Socket::bindSocket()
{
    if (bind(this->_server_sock, reinterpret_cast<struct sockaddr*>(&this->_server_addr), sizeof(this->_server_addr)) < 0) 
    {
        std::cerr << "Error\n Socket binding failure" << std::endl;
        return -1;
    }
    return 0;
}

int Socket::listenSocket(int backlog)
{
    if (listen(this->_server_sock, backlog) < 0) 
    {
        std::cerr << "Error\n socket wiretap failure" << std::endl;
        return -1;
    }
    return 0;
}

int Socket::acceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    return accept(this->_server_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
}

void Socket::sendResponse(int client_sock, const std::string& response) {
    send(client_sock, response.c_str(), response.length(), 0);
}

void Socket::closeSocket() {
    if (this->_server_sock >= 0) {
        close(this->_server_sock);
    }
}
