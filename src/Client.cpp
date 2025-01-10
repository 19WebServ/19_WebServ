#include "../include/Client.hpp"

Client::Client(int serverFd, int indexServerSock, int port, size_t bodySize) : _serverFd(serverFd), _indexServerFd(indexServerSock), _maxBodySize(bodySize), _port(port)
{
    std::cout << "BODY SIZE = " << this->_maxBodySize << std::endl;
    
}

Client::~Client()
{

}

void Client::setIp(std::string ip)
{
    this->_ip = ip;
}

void Client::setClientFd(int clientFd)
{
    this->_clientFd = clientFd;
}

void Client::setIndexClientFd(size_t index)
{
    this->_indexClientFd = index;
}

int Client::getServerFd()
{
    return this->_serverFd;
}

int Client::getClientFd()
{
    return this->_clientFd;
}

size_t Client::getIndexServerFd()
{
    return this->_indexServerFd;
}
size_t Client::getIndexClientFd()
{
    return this->_indexClientFd;
}

size_t Client::getMaxBodySize()
{
    return this->_maxBodySize;
}

int Client::getPort()
{
    return this->_port;
}