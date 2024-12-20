#include "../include/Socket.hpp"
#include <poll.h>

Socket::Socket(int port = 8080, unsigned int ip = INADDR_ANY)
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

// int Socket::createSocket()
// {
//     this->_server_sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (!this->_server_sock)
//         std::cerr << "Error\nSocket creation failure" << std::endl;
//     return this->_server_sock;
// }

int Socket::createSocket()
{
    this->_server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_server_sock < 0) {
        std::cerr << "Error\nSocket creation failure" << std::endl;
        return -1;
    }

    // Permet de réutiliser l'adresse immédiatement après la fermeture
    int opt = 1;
    if (setsockopt(this->_server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error\nFailed to set socket options" << std::endl;
        return -1;
    }
    int current_opt;
    socklen_t opt_len = sizeof(current_opt);
    if (getsockopt(this->_server_sock, SOL_SOCKET, SO_REUSEADDR, &current_opt, &opt_len) == 0) {
        std::cout << "SO_REUSEADDR option is set to: " << current_opt << std::endl;
    } else {
        std::cerr << "Failed to get SO_REUSEADDR option." << std::endl;
    }
    return this->_server_sock;
}

int Socket::bindSocket()
{
    if (bind(this->_server_sock, reinterpret_cast<struct sockaddr*>(&this->_server_addr), sizeof(this->_server_addr)) < 0) 
    {
        std::cerr << "Error\nSocket binding failure" << std::endl;
        return -1;
    }
    return 0;
}

int Socket::listenSocket(int backlog)
{
    if (listen(this->_server_sock, backlog) < 0) 
    {
        std::cerr << "Error\nSocket wiretap failure" << std::endl;
        return -1;
    }
    this->_pfd.fd = this->getSocketFD();
    this->_pfd.events = POLLIN;
    this->_pfd.revents = 0;
    this->_poll_fds.push_back(this->_pfd);
    return 0;
}

int Socket::acceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(this->_server_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
    if (client_sock >= 0) 
    {
        std::string clientIP = getClientIP(&client_addr);
        std::cout << "Client connected: " << clientIP << std::endl;

        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(client_sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
            std::cerr << "Error: failed to retrieve socket error status for client." << std::endl;
        else if (error != 0)
        {
            std::cerr << "Client socket has an error: " << strerror(error) << std::endl;
            close(client_sock);
            return -1;
        }
    } 
    else 
        std::cerr << "Error\nFailed to accept client connection." << std::endl;
    return client_sock;
}

void Socket::closeSocket() {
    if (this->_server_sock >= 0)
    {
        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(this->_server_sock, SOL_SOCKET, SO_ERROR, &error, &len) == 0 && error != 0) 
            std::cerr << "Socket error before closing: " << strerror(error) << std::endl;
        close(this->_server_sock);
    }
}
/**
 * @brief Construct a new Response:: Response object
 * 
 * @param target, data, data_len
 * 
 * @return nbr_bits or -1
 */
int Socket::sendData(int target_sock, const char *data, unsigned int len)
{
    if (target_sock < 0)
    {
        std::cerr<<"Error\nInvalide target_sock"<<std::endl;
        return -1;
    }
    int res = send(target_sock, data, len, 0);
    if (res == -1)
        std::cerr<< "Error\nFailed to send data" << std::endl;
    return res;
}

/**
 * @brief Construct a new Response:: Response object
 * 
 * @param target, data, data_len
 * 
 * @return nbr_bits or -1
 */
int Socket::receiveData(int target_sock, char *buffer, unsigned int len)
{
    if (target_sock < 0)
    {
        std::cerr<<"Error\nInvalide target_sock"<<std::endl;
        return -1;
    }
    int res = recv(target_sock, buffer, len, 0);
    if (res < 0)
        std::cerr << "Error\nFailed to receive data." << std::endl;
    return res;
}

std::string Socket::getClientIP(struct sockaddr_in *client_addr)
{
    unsigned char *ip = reinterpret_cast<unsigned char *>(&(client_addr->sin_addr.s_addr));
    std::stringstream ss;
    ss << static_cast<int>(ip[0]) << "."
       << static_cast<int>(ip[1]) << "."
       << static_cast<int>(ip[2]) << "."
       << static_cast<int>(ip[3]);
    return ss.str();
}

int Socket::getSocketFD() const {
    return this->_server_sock;
}

int Socket::server_poll()
{
    int event_count = poll(this->_poll_fds.data(), this->_poll_fds.size(), -1);
    if (event_count < 0)
    {
        std::cerr << "Error\n Poll failed" << std::endl;
    }
    return event_count;
}
