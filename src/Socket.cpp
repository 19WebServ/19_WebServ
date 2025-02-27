#include "../include/Socket.hpp"
#include <poll.h>
#include <fcntl.h>
#include <fstream>

Socket* Socket::_instance = NULL;

Socket::Socket(const std::vector<int> &ports, const std::vector<ServerConfig> &servers) 
    : _ports(ports), _servers(servers) 
{
    for (size_t i = 0; i < _servers.size(); i++) // Parcourir chaque serveur
    {
        const std::vector<int> &serverPorts = _servers[i].getPort(); // Récupérer les ports du serveur

        for (size_t j = 0; j < serverPorts.size(); j++) // Parcourir chaque port du serveur
        {
            int serverSock = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSock < 0)
            {
                std::cerr << "❌ Erreur : Échec de création du socket pour le serveur " 
                          << i << " sur le port " << serverPorts[j] << std::endl;
                closeFds(this->_serverSocks);
                return;
            }

            int opt = 1;
            if (setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            {
                std::cerr << "❌ Erreur : Impossible de configurer le socket en réutilisation d'adresse" << std::endl;
                closeFds(this->_serverSocks);
                return;
            }

            sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(serverPorts[j]); // Associer le bon port
            serverAddr.sin_addr.s_addr = INADDR_ANY; // Accepter toutes les connexions entrantes

            if (bind(serverSock, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
            {
                std::cerr << "❌ Erreur : Impossible de binder le serveur " << i 
                          << " au port " << serverPorts[j] << std::endl;
                closeFds(this->_serverSocks);
                return;
            }

            if (listen(serverSock, SOMAXCONN) < 0)
            {
                std::cerr << "❌ Erreur : Impossible d'écouter sur le serveur " << i 
                          << " au port " << serverPorts[j] << std::endl;
                closeFds(this->_serverSocks);
                return;
            }

            if (fcntl(serverSock, F_SETFL, O_NONBLOCK) < 0)
            {
                std::cerr << "❌ Erreur : Impossible d'activer le mode non-bloquant" << std::endl;
                closeFds(this->_serverSocks);
                return;
            }

            struct pollfd serverPollFd;
            serverPollFd.fd = serverSock;
            serverPollFd.events = POLLIN;

            this->_poll_fds.push_back(serverPollFd);
            this->_serverSocks.push_back(serverSock);

            std::cout << "✅ Serveur " << i << " écoutant sur le port " << serverPorts[j] << std::endl;
        }
    }

    this->_instance = this;
    std::signal(SIGINT, Socket::signalHandler);
}

Socket::Socket(const Socket &rhs) {
    *this = rhs;
}

Socket::~Socket()
{
    closeFds(this->_serverSocks);
}

void Socket::launchServer()
{
    while (true)
    {
        int event_count = poll(this->_poll_fds.data(), this->_poll_fds.size(), 1000);
        if (event_count < 0)
        {
            std::cerr << "Error: Poll failed" << std::endl;
            closeFds(this->_serverSocks);
            return;
        }
        for (size_t i = 0; i < this->_poll_fds.size(); i++)
        {
            int fd = this->_poll_fds[i].fd;
            if (this->_poll_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                std::cout << "POLLHUP detected" << std::endl;
                close(fd);
                for (size_t j = 0; j < this->_clients.size(); j++)
                {
                    if (this->_clients[j].getClientFd() == fd)
                    {
                        this->_clients.erase(this->_clients.begin() + j);
                        break;
                    }
                }
                this->_poll_fds.erase(this->_poll_fds.begin() + i);
                i--;
                continue;
            }
            if (this->_poll_fds[i].revents & POLLIN)
            {
                bool newConnection = false;
                for (size_t j = 0; j < this->_serverSocks.size(); j++)
                {
                    if (fd == this->_serverSocks[j])
                    {
                        acceptConnection(this->_serverSocks[j], i);
                        newConnection = true;
                        break;
                    }
                }
                if (!newConnection)
                {
                    for (size_t k = 0; k < this->_clients.size(); k++)
                    {
                        if (this->_clients[k].getClientFd() == fd)
                        {
                            handleClient(fd, this->_clients[k]);
                            if (this->_clients[k].getRequest().getIfComplete())
                                this->_poll_fds[i].events |= POLLOUT;
                            break;
                        }
                    }
                }
            }
            if (this->_poll_fds[i].revents & POLLOUT)
            {
                for (size_t k = 0; k < this->_clients.size(); k++)
                {
                    if (this->_clients[k].getClientFd() == fd)
                    {
                        processingResponse(fd, this->_clients[k]);
                        if (this->_clients[k].getTotalSent() == this->_clients[k].getResponseLen())
                        {
                            this->_clients[k].setTotalSent(0);
                            this->_poll_fds[i].events &= ~POLLOUT;
                        }
                        break;
                    }

                }
            }
        }
        for (size_t k = 0; k < this->_clients.size(); k++)
        {
            if (Utils::getTime() - this->_clients[k].getTimeLastRequest() >= this->_clients[k].getTimeout())
            {
                int fd = this->_clients[k].getClientFd();

                close(fd);
                std::cout << "Client erase timeout" << std::endl;
                this->_clients.erase(this->_clients.begin() + k);
                for (size_t i = 0; i < this->_poll_fds.size(); i++)
                {
                    if (this->_poll_fds[i].fd == fd)
                    {
                        this->_poll_fds.erase(this->_poll_fds.begin() + i);
                        break;
                    }
                }
                k--;
            }
        }
    }
}

void Socket::signalHandler(int signum)
{
    if (Socket::getInstance())
    {
        Socket::getInstance()->closeFds(Socket::getInstance()->_serverSocks);
    }
    exit(signum);
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
    std::cout << "\nAll server sockets closed" << std::endl;
}

void Socket::acceptConnection(int serverSock, int i) 
{
    int port = this->getPort(i);
    ServerConfig temp;
    size_t j = 0;
    for (;j < this->_servers.size(); j++)
    {
        for (size_t k = 0; k < this->_servers[j].getPort().size(); k++)
        {
            if (this->_servers[j].getPort()[k] == port)
            {
                temp = this->_servers[j];
                break;
            }
        }
    }
    Client client(serverSock, i, port, temp);
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

void    Socket::handleClient(int &clientFd, Client &client)
{
    client.setTimeLastRequest();
    std::string request;
    int bytes_receiv = this->receiveData(clientFd, request);
    if (bytes_receiv > 0)
    {
        if (this->processingRequest(request, client))
            return ;
    }
    else  
    {
        if (bytes_receiv == 0) 
            std::cout << "Client disconnected" << std::endl;
        else if (bytes_receiv < 0)
            std::cerr << "Failed to receive data from client" << std::endl;
        close(clientFd);
        for (size_t k = 0; k < this->_clients.size(); k++)
        {
            if (this->_clients[k].getClientFd() == clientFd)
            {
                this->_clients.erase(this->_clients.begin() + k);
                break;
            }
        }
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

int Socket::processingRequest(std::string requestStr, Client &client)
{
    try {
        client.parseRequest(requestStr);
        if (client.getRequest().getIfComplete() == true)
            client.sendResponse();

    }
    catch(const std::exception& e) {
        client.handleErrorResponse(e.what());
    }
    return 0;
}

int Socket::processingResponse(int clientFd, Client &client)
{
    size_t temp;

    temp = client.getTotalSent() + this->sendData(clientFd, client.getResponse().substr(client.getTotalSent()));
    client.setTotalSent(temp);
    return 0;
}

int Socket::sendData(int target_sock, std::string toSend)
{
    char data[2048];
    if (target_sock < 0)
    {
        std::cerr<<"Error\nInvalide target_sock"<<std::endl;
        return -1;
    }
    int res = 0;
    unsigned long int i = 0;
    for (; i < toSend.size() && i < sizeof(data); i++)
        data[i] = toSend[i];

    res = send(target_sock, data, i, 0);
    if (res == -1)
        std::cerr<< "Error\nFailed to send data" << std::endl;
    return res;
}

int Socket::receiveData(int target_sock, std::string &request)
{
    char buffer[2048];
    if (target_sock < 0)
    {
        std::cerr<<"Error\nInvalide target_sock"<<std::endl;
        return -1;
    }
    int res;
    res = recv(target_sock, buffer, sizeof(buffer) - 1, 0);
    if (res <= 0)
        return 0;
    buffer[res] = '\0';
    request.clear();
    for (int i(0); i < res; i++)
        request += buffer[i];
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


/* ---GETTERS--- */

Socket* Socket::getInstance() {return _instance;}

int Socket::getPort(int i) {return this->_ports[i];}

ServerConfig Socket::getServer(int index) {return this->_servers[index];}
