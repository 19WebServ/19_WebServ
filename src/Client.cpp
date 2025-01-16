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

    if (request.find("GET /favicon.ico") != std::string::npos)
        return ;
    std::cout << request << std::endl;
    getline(ss, method, ' ');
    getline(ss, location, ' ');
    for (size_t i(0); i < _server.getLocationAllowedMethods(location).size(); i++) {
        if (method == _server.getLocationAllowedMethods(location)[i]) {
            setRequest(request, location, method);
            allowed = true;
        }
    }
    if (allowed == false)
        throw std::runtime_error("405 Method Not Allowed");
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
    size_t index(0);
    std::string methods[3] = {"GET", "POST", "DELETE"};
    std::string response = "";

    index = 0;
    for (; index < sizeof(methods) / sizeof(methods[0]); index++) {
        if (methods[index] == this->_request.getMethod())
            break;
    }
    switch (index)
    {
    case 0:
        /* GET */
        response = this->respondToGet();
        break;
    case 1:
    //     /* POST */
    //     response = this->respondToPost();
        break;
    case 2:
    //     /* DELETE */
    //     response = this->respondToDelete();
        break;
        
    default:
        break;
    }
    return response;
}

std::string Client::respondToGet()
{
    std::string htmlContent;
    std::string locationRoot = _server.getLocationRoot(_request.getLocation());
    std::string locationIndex = _server.getLocationIndex(_request.getLocation());
    // std::cout << "Received from client "<< _ip << std::endl;
    if (!Utils::isFile(locationRoot + locationIndex))
        throw std::runtime_error("404 Not Found");
    else if (!Utils::hasReadPermission((locationRoot + locationIndex).c_str()))
        throw std::runtime_error("403 Forbidden");
    htmlContent = Utils::readFile(locationRoot + locationIndex);
    if (htmlContent.empty())
        throw std::runtime_error("Failed to read html file.");
    std::ostringstream oss;
    oss << htmlContent.size();
    std::string contentLength = oss.str();
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + contentLength + "\r\n"
        "Connection: keep-alive\r\n"
        "Keep-Alive: timeout=10000\r\n"
        "\r\n" +
        htmlContent;
    return response;
}

// std::string Client::respondToPost()
// {
    
// }

// std::string Client::respondToDelete()
// {
    
// }

std::string Client::handleErrorResponse(std::string error)
{
    std::string word;
    std::string response;
    std::istringstream ss;
    std::string errorPage;
    std::string root;
    std::string htmlContent;

    ss.str(error);
    getline(ss, word, ' ');
    if (Utils::areOnlyDigits(word)) {
        errorPage = _server.getErrorPage(atoi(word.c_str()));
        root = _server.getRoot();
        if (!errorPage.empty())
            htmlContent = Utils::readFile(root + errorPage);
        else
            htmlContent = Utils::generateErrorPage(error);

        if (htmlContent.empty())
            throw std::runtime_error("Failed to read html file.");
        std::ostringstream oss;
        oss << htmlContent.size();
        std::string contentLength = oss.str();

        response =
        "HTTP/1.1 " + error + "\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: " + contentLength + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        htmlContent;
    }
    else {
        std::cerr << "Error: " << error << std::endl;
        std::exit(1); // JE SAIS PAS QUOI METTRE ICI
    }
    return response;
}