#include "../include/Socket.hpp"

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
    } 
    else
        std::cerr << "Error\nFailed to accept client connection." << std::endl;
    return client_sock;
}

void Socket::closeSocket() {
    if (this->_server_sock >= 0)
        close(this->_server_sock);
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
