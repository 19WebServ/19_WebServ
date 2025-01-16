#include "../include/Client.hpp"

Client::Client(int serverFd, int indexServerSock, int port, ServerConfig server) : _server(server), _serverFd(serverFd), _indexServerFd(indexServerSock), _port(port)
{
    this->_maxBodySize = this->_server.getBodySize();
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

void Client::setTimeout(size_t time)
{
    this->_timeout = time;
}


void Client::setTimeLastRequest()
{
    std::time_t currentTime = std::time(NULL);
    if (currentTime != static_cast<std::time_t>(-1))
    {
        this->_timeLastRequest = static_cast<size_t>(currentTime);
    }
    else
    {
        std::cerr << "Error:\nFailed to set time of request" << std::endl;
        this->_timeLastRequest = 0;
    }
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

size_t Client::getTimeout()
{
    return this->_timeout;
}

size_t Client::getTimeLastRequest()
{
    return this->_timeLastRequest;
}

std::string Client::getIp()
{
    return this->_ip;
}

void    Client::parseRequest(std::string request)
{
    std::istringstream ss;
    ss.str(request);
    std::string method;
    std::string location;
    bool allowed = false;

    getline(ss, method, ' ');
    getline(ss, location, ' ');
    for (size_t i(0); i < _server.getLocationAllowedMethods(location).size(); i++) {
        if (method == _server.getLocationAllowedMethods(location)[i]) {
            setRequest(request, location, method);
            allowed = true;
        }
    }
    if (allowed == false)
        throw std::runtime_error("405" + method);
}

void Client::setRequest(std::string requestStr, std::string location, std::string method)
{
    Request temp(location, method);

    if (method == "POST") {
        std::istringstream ss;
        std::string line;
        std::string body = "";
    
        ss.str(requestStr);
        while (getline(ss, line)) {
            if (!line.empty() && line[line.size() - 1] == '\r')
                line.substr(0, line.size() - 2);
            if (line.empty())
                break;
        }
        while (getline(ss, line))
            body += (line + '\n');
        temp.setContent(body);
    }
    this->_request = temp;
}

std::string    Client::sendResponse()
{
    int index(0);
    std::string methods[3] = {"GET", "POST", "DELETE"};
    std::string htmlContent;
    for (; index < 3; index++)
    {
        index = 0;
        for (; index < sizeof(methods) / sizeof(methods[0]); index++) {
            if (methods[index] == this->_request.getMethod())
                break;
        }
        switch (index)
        {
        case 0:
            /* GET */
            htmlContent = this->respondToGet();
            break;
        case 1:
            /* POST */
            htmlContent = this->respondToPost();
            break;
        case 2:
            /* DELETE */
            htmlContent = this->respondToDelete();
            break;
        
        default:
            throw std::runtime_error("Should not go here");
            break;
        }
    }
    return htmlContent;
}

std::string Client::respondToGet()
{
    
}

std::string Client::respondToPost()
{
    
}

std::string Client::respondToDelete()
{
    
}