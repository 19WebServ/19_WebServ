#include "../include/Client.hpp"

Client::Client(int serverFd, int indexServerSock, int port, ServerConfig server) : _server(server), _serverFd(serverFd), _indexServerFd(indexServerSock), _port(port)
{
    this->_maxBodySize = this->_server.getBodySize();
}

Client::Client()
{}


Client::~Client()
{}

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
    std::cout << "HOLA" << std::endl;
    // std::cout << request << std::endl;
    std::cout << "ICI" << std::endl;
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
        size_t pos;
    
        ss.str(requestStr);
        while (getline(ss, line)) {
            pos = line.find("boundary=");
            if (pos != std::string::npos)
                temp.setBoundary(line.substr(pos + 9));
            if (!line.empty() && line[line.size() - 1] == '\r')
                line = line.substr(0, line.size() - 1);
            std::cout << line << std::endl;
            if (line.empty())
                break;
        }
        while (getline(ss, line)) {
            body += (line + '\n');
        }
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
        /* POST */
        response = this->respondToPost();
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
    std::string response;
    std::string locationRoot = _server.getLocationRoot(_request.getLocation());
    std::string locationIndex = _server.getLocationIndex(_request.getLocation());
    std::string path = locationRoot + locationIndex;
    // std::cout << "Received from client "<< _ip << std::endl;
    // std::cout <<"chemin : "<< locationRoot + locationIndex << std::endl;
    if (!_server.getLocationRedirect(_request.getLocation()).empty())
        response = makeRedirection(_server.getLocationRedirect(_request.getLocation()).begin()->first, _server.getLocationRedirect(_request.getLocation()).begin()->second);
    else {
        if (!Utils::isFile(path))
            throw std::runtime_error("404 Not Found");
        else if (!Utils::hasReadPermission((path).c_str()))
            throw std::runtime_error("403 Forbidden");
        if (path.size() > 3 && (path.substr(path.size() - 3) == ".py" || path.substr(path.size() - 3) == ".pl"))
            return executeCGI(path);
        
        // std::cout << "Location Index: " << locationIndex << " Location Root: " << std::endl; 
        htmlContent = Utils::readFile(path);
        if (htmlContent.empty())
            throw std::runtime_error("Failed to read html file.");
        std::ostringstream oss;
        oss << htmlContent.size();
        std::string contentLength = oss.str();
        response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + contentLength + "\r\n"
            "Connection: keep-alive\r\n"
            "Keep-Alive: timeout=10000\r\n"
            "\r\n" +
            htmlContent;
    }
    return response;
}

std::string Client::makeRedirection(std::string statusCode, std::string redirection)
{
    std::string response = 
        "HTTP/1.1 " + statusCode + " Found\r\n"
        "Location: " + redirection + "\r\n"
        "Content-Length: 0\r\n"
        "Connection: keep-alive\r\n"
        "Keep-Alive: timeout=10000\r\n"
        "\r\n";
    return response;
}

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

std::string Client::executeCGI(const std::string& scriptPath) {
    int pipefd[2];
    if (pipe(pipefd) == -1)
        throw std::runtime_error("500 Internal Server Error: Pipe failed");

    pid_t pid = fork();
    if (pid < 0)
        throw std::runtime_error("500 Internal Server Error: Fork failed");

    if (pid == 0) {
        close(pipefd[0]);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char* argv[] = {(char*)scriptPath.c_str(), NULL};
        char* envp[] = {NULL};

        execve(argv[0], argv, envp);
        perror("execve");
        exit(1);
    } 

    close(pipefd[1]);
    char buffer[getMaxBodySize()];
    std::string output;
    size_t bytesRead;

    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }
    close(pipefd[0]);
    int status;
    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        throw std::runtime_error("500 Internal Server Error: CGI script failed");

    std::ostringstream oss;
    oss << output.size();
    std::string contentLength = oss.str();

    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: " + contentLength + "\r\n"
           "Connection: keep-alive\r\n"
           "Keep-Alive: timeout=10000\r\n"
           "\r\n" +
           output;
}

std::string Client::respondToPost()
{
    std::string response;
    postContent();
    response =
        "HTTP/1.1 201 Created\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 19\r\n"
        "Connection: close\r\n"
        "\r\n"
        "File upload success";
    return response;
}

void Client::postContent()
{
    std::string delimiter = "--" + _request.getBoundary();
    size_t start = 0, end = 0;
    std::string body = _request.getContent();
    start = body.find(delimiter, end);
    if (start == std::string::npos)
        throw std::runtime_error("Boundary not found in body.");
    while (start != std::string::npos) {
        start += delimiter.size() + 1;
        end = body.find(delimiter, start);
        std::string part = body.substr(start, end - start);
        if (part.find("Content-Disposition: form-data;") != std::string::npos) {
            size_t filenamePos = part.find("filename=");
            if (filenamePos != std::string::npos) {
                size_t nameStart = part.find("\"", filenamePos) + 1;
                size_t nameEnd = part.find("\"", nameStart);
                std::string filename = part.substr(nameStart, nameEnd - nameStart);

                size_t dataStart = part.find("\r\n\r\n", nameEnd) + 4;
                std::string fileData = part.substr(dataStart, part.size() - dataStart - delimiter.size() - 7);
                Utils::saveFile(filename, fileData);
            }
        }
        start = body.find(delimiter, end);
    }
}

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
            htmlContent = error;

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