#include "../include/Socket.hpp"
#include <poll.h>
#include <fcntl.h>
#include <fstream>

Socket::Socket(const std::vector<int> &ports, const std::vector<ServerConfig> &server) : _ports(ports) , _servers(server)
{
    for (size_t i = 0; i < this->_ports.size(); i ++)
    {
        int serverSock = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSock < 0)
        {
            std::cerr<< "Error\n!! Socket creation failure !!" << std::endl;
            closeFds(this->_serverSocks);
        }
        int opt = 1;
        if (setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
        {
            std::cerr << "Error\nFailed to set socket options" << std::endl;
            closeFds(this->_serverSocks);
        }
        int current_opt;
        socklen_t opt_len = sizeof(current_opt);
        if (getsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &current_opt, &opt_len) == 0)
            std::cout << "SO_REUSEADDR option is set to: " << current_opt << std::endl;
        else
            std::cerr << "Failed to get SO_REUSEADDR option." << std::endl;
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(this->_ports[i]);
        serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
        if (bind(serverSock, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) 
        {
            std::cerr << "Error\nSocket binding failure" << std::endl;
            closeFds(this->_serverSocks);
        }
        if (listen(serverSock, SOMAXCONN) < 0) 
        {
            std::cerr << "Error\nSocket wiretap failure" << std::endl;
            closeFds(this->_serverSocks);
        }
        if (fcntl(serverSock, F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << "Error\nFailed non-blocking mode" << std::endl;
            closeFds(this->_serverSocks);
        }
        struct pollfd serverPollFd;
        serverPollFd.fd = serverSock;
        serverPollFd.events = POLLIN;

        this->_poll_fds.push_back(serverPollFd);
        this->_serverSocks.push_back(serverSock);
    }
}

Socket::~Socket()
{
    closeFds(this->_serverSocks);
}

void Socket::closeFds(std::vector<int>serverSocks)
{
    for (size_t i = 0; i < serverSocks.size(); i++)
    {
        if (serverSocks[i] >= 0)
        {
            int error = 0;
            socklen_t len = sizeof(error);
            if (getsockopt(serverSocks[i], SOL_SOCKET, SO_ERROR, &error, &len) == 0 && error != 0)
                std::cerr << "Socket error before closing: " << strerror(error) << std::endl;
            close(serverSocks[i]);
        }
    }
    std::cerr<< "All server sockets closed" << std::endl;
}

void Socket::launchServer()
{
    while (1)
    {
        int event_count = poll(this->_poll_fds.data(), this->_poll_fds.size(), 1000);
        if (event_count < 0)
        {
            std::cerr << "Error\n Poll failed" << std::endl;
            closeFds(this->_serverSocks);
        }
        for (size_t i = 0; i < this->_poll_fds.size(); i++)
        {

            if (this->_poll_fds[i].revents & POLLIN)
            {
                bool newConnection = false;
                for (size_t j = 0; j < this->_serverSocks.size(); j++)
                {
                    if (this->_poll_fds[i].fd == this->_serverSocks[j])
                    {

                        acceptConnection(this->_serverSocks[j], i);
                        newConnection = true;
                        break;
                    }
                }
                if (newConnection == false)
                {
                    size_t k = 0;
                    for (; k < this->_clients.size(); k++)
                    {
                        if (this->_clients[k].getClientFd() == this->_poll_fds[i].fd)
                        {
                            handleClient(this->_poll_fds[i].fd, this->_clients[k]);
                            break;
                        }
                    }
                }
            }
            size_t k = 0;
            for (; k < this->_clients.size(); k++)
            {
                if (getTime() - this->_clients[k].getTimeLastRequest() >= this->_clients[k].getTimeout())
                {
                    std::cout << "Client "<<this->_clients[k].getIp() <<" Disconnected after '"<< this->_clients[k].getTimeout() << "' secondes of inactivity" << std::endl;
                    close(this->_clients[k].getClientFd());
                    this->_clients.erase(this->_clients.begin() + k);
                    break;
                }
            }
        }
        
    }
}

void    Socket::handleClient(int &clientFd, Client &client)
{
    size_t i = client.getMaxBodySize();
    client.setTimeLastRequest();
    char buffer[i];
    int bytes_receiv = this->receiveData(clientFd, buffer, sizeof(buffer));
    if (bytes_receiv > 0)
    {
        if (this->processingRequest(buffer, bytes_receiv, clientFd, client))
            return ;
    }
    if (bytes_receiv <= 0) 
    {
        if (bytes_receiv == 0) 
            std::cout << "Client disconnected" << std::endl;
        else if (errno != EWOULDBLOCK && errno != EAGAIN)
            std::cerr << "Failed to receive data from client" << std::endl;
        for (size_t k = 0; k < this->_clients.size(); k++)
        {
            if (this->_clients[k].getClientFd() == clientFd)
            {
                this->_clients.erase(this->_clients.begin() + k);
                break;
            }
        }
        close(clientFd);
        for (size_t k = 0; k < this->_poll_fds.size(); ++k)
        {
            if (this->_poll_fds[k].fd == clientFd)
            {
                this->_poll_fds.erase(this->_poll_fds.begin() + k);
                break;
            }
        }
    }
}

int Socket::processingRequest(char *buffer, int bytes_receive, int clientFd, Client client)
{
    buffer[bytes_receive] = '\0';
    std::string request(buffer);
    std::string htmlContent;
    try {
        client.parseRequest(request);
        htmlContent = client.sendResponse();
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    // if (request.find("GET /favicon.ico") != std::string::npos)
    //     return 0;
    // if (request.find("GET /televers.html") != std::string::npos)
    //     htmlContent = readFile("pages_html/televers.html");
    // else if (request.find("GET /contact.html") != std::string::npos)
    //     htmlContent = readFile("pages_html/contact.html");
    // else if (request.find("GET /service.html") != std::string::npos)
    //     htmlContent = readFile("pages_html/service.html");
    // else
    //     htmlContent = readFile("pages_html/index.html");

    // std::cout << "Received from client "<< client.getIp() << ":\n" << request << std::endl;

    // if (htmlContent.empty()) 
    // {
    //     std::cerr << "Failed to read index.html." << std::endl;
    //     return 1;
    // }

    // std::ostringstream oss;
    // oss << htmlContent.size();
    // std::string contentLength = oss.str();

    // std::string response = 
    //     "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/html\r\n"
    //     "Content-Length: " + contentLength + "\r\n"
    //     "Connection: keep-alive\r\n"
    //     "Keep-Alive: timeout=10000\r\n"
    //     "\r\n" +
    //     htmlContent;

    // int total_sent = 0;
    // while (total_sent < (int)response.size())
    // {
    //     int bytes_sent = this->sendData(clientFd, response.c_str() + total_sent, response.size() - total_sent);
    //     if (bytes_sent > 0)
    //         total_sent += bytes_sent;
    //     else if (errno != EWOULDBLOCK && errno != EAGAIN)
    //     {
    //         std::cerr << "Failed to send response to client." << std::endl;
    //         break;
    //     }
    // }
    return 0;
}

void Socket::acceptConnection(int serverSock, int i) 
{
    Client client(serverSock, i, this->getPort(i),  this->getServer(i));
    client.setTimeLastRequest();
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(serverSock, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
    if (client_sock >= 0) 
    {
       if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << "Error\nFailed non-blocking mode" << std::endl;
            closeFds(this->_serverSocks);
        }
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
        }
        struct pollfd clientFd;

        clientFd.fd = client_sock;
        clientFd.events = POLLIN;
        this->_poll_fds.push_back(clientFd);
        client.setClientFd(client_sock);
        client.setIndexClientFd(this->_poll_fds.size() - 1);
        client.setIp(getClientIP(&client_addr));
        client.setTimeout();
        this->_clients.push_back(client);
    }
    else 
        std::cerr << "Error\nFailed to accept client connection." << std::endl;
}

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

int Socket::receiveData(int target_sock, char *buffer, unsigned int len)
{
    if (target_sock < 0)
    {
        std::cerr<<"Error\nInvalide target_sock"<<std::endl;
        return -1;
    }
    int receiv = 0;
    int res = 0;
    while (true)
    {
        res = receiv;
        receiv = recv(target_sock, buffer, len, 0);
        if (receiv <= 0)
            break;
    }
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

int Socket::getPort(int i)
{
    return this->_ports[i];
}

ServerConfig Socket::getServer(int index)
{
    return this->_servers[index];
}

size_t getTime()
{
    std::time_t currentTime = std::time(NULL);
    size_t res = 0;
    if (currentTime != static_cast<std::time_t>(-1))
    {
        res = static_cast<size_t>(currentTime);
    }
    return res;
}