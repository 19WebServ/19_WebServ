#include "../include/Socket.hpp"
#include <poll.h>

Socket::Socket(int port = 8080, unsigned int ip = INADDR_ANY) : _port(port)
{
    this->_server_sock = -1;
    memset(&this->_server_addr, 0, sizeof(this->_server_addr));
    this->_server_addr.sin_family = AF_INET;
    this->_server_addr.sin_port = htons(_port);
    this->_server_addr.sin_addr.s_addr = htonl(ip);
}

Socket::~Socket()
{
    this->closeSocket();
}
/**
 * @brief Create a new Socket | set Socket_option |  make addresses reusable
 * 
 * @param none
 * 
 * @return int 
 */
int Socket::createSocket()
{
    this->_server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_server_sock < 0) 
    {
        std::cerr << "Error\nSocket creation failure" << std::endl;
        return -1;
    }
    int opt = 1;
    if (setsockopt(this->_server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        std::cerr << "Error\nFailed to set socket options" << std::endl;
        return -1;
    }
    int current_opt;
    socklen_t opt_len = sizeof(current_opt);
    if (getsockopt(this->_server_sock, SOL_SOCKET, SO_REUSEADDR, &current_opt, &opt_len) == 0)
        std::cout << "SO_REUSEADDR option is set to: " << current_opt << std::endl;
    else
        std::cerr << "Failed to get SO_REUSEADDR option." << std::endl;
    return this->_server_sock;
}

/**
 * @brief assigns address to socket
 * 
 * @param none
 * 
 * @return int 
 */
int Socket::bindSocket()
{
    if (bind(this->_server_sock, reinterpret_cast<struct sockaddr*>(&this->_server_addr), sizeof(this->_server_addr)) < 0) 
    {
        std::cerr << "Error\nSocket binding failure" << std::endl;
        return -1;
    }
    return 0;
}

/**
 * @brief set a socket fd waiting for a connection | fd table set-up for poll function
 * 
 * @param none
 * 
 * @return int 
 */
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
 * @brief send a new response
 * 
 * @param target, data, data_len
 * 
 * @return int
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
 * @brief receive a new call
 * 
 * @param target, data, data_len
 * 
 * @return int 
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

int Socket::getPort()
{
    return this->_port;
}


int Socket::getSocketFD() const 
{
    return this->_server_sock;
}

/**
 * @brief use the poll function
 * 
 * @param none
 * 
 * @return int
 */
int Socket::server_poll()
{
    int event_count = poll(this->_poll_fds.data(), this->_poll_fds.size(), -1);
    if (event_count < 0)
        std::cerr << "Error\n Poll failed" << std::endl;
    return event_count;
}

/**
 * @brief server initialization
 * 
 * @param Socket_instance
 * 
 * @return int 
 */
int init_server(Socket &server) 
{
    if (server.createSocket() < 0 || server.bindSocket() < 0 || server.listenSocket() < 0) 
        return 1;
    std::cout << "Server is listening on port " << server.getPort() << "\n"<< std::endl;
    return 0;
}
