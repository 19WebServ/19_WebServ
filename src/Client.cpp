#include "../include/Client.hpp"

Client::Client(int serverFd, int indexServerSock, int port, ServerConfig server) : _server(server), _serverFd(serverFd), _indexServerFd(indexServerSock), _port(port)
{
    this->_maxBodySize = this->_server.getBodySize();
}

Client::Client(){}

Client::~Client(){}


/* ---REQUEST--- */

// Recupère la methode et la location et accepte la requete si la methode est autorisee
void    Client::parseRequest(std::string request)
{
    std::istringstream ss;
    ss.str(request);
    std::string method;
    std::string path;
    std::string location;
    bool allowed = false;

    if (request.find("GET /favicon.ico") != std::string::npos)
        return ;
    std::cout << "\n" << request << std::endl;
    getline(ss, method, ' ');
    getline(ss, path, ' ');
    ss.clear();
    ss.str(path);
    getline(ss, location, '/');                          // Voir comment je dois gerer ici -> differencier une requete d'une page direct d'une location vide
    if (!ss.eof() && location.empty())
        getline(ss, location, '/');
    location = "/" + location;
    path = path.substr(location.size());
    // if (method == "POST" && path.empty())
    //     return ;
    for (size_t i(0); i < _server.getLocationAllowedMethods(location).size(); i++) {
        if (method == _server.getLocationAllowedMethods(location)[i]) {
            createRequest(request, location, method, path);
            allowed = true;
        }
    }
    if (allowed == false)
        throw std::runtime_error("405 Method Not Allowed");
}

// cree un objet requete avec les infos importantes du header et, si Post, recupere le body
void Client::createRequest(std::string requestStr, std::string location, std::string method, std::string path)
{
    Request temp(location, path, method);

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


/* ---RESPONSE--- */

// dispatch la requete a la methode correspondante
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

// Reponse dans le cas d'une requete GET
std::string Client::respondToGet()
{
    std::string htmlContent;
    std::string response;
    std::string locationBlock = _request.getLocation();
    std::string locationRoot = _server.getLocationRoot(locationBlock);
    std::string locationIndex;

    if (_request.getPath().empty())
        locationIndex = _server.getLocationIndex(locationBlock);
    else
        locationIndex = _request.getPath();
    std::string path = locationRoot + locationIndex;
    std::cout << "\n\n" << path << std::endl << std::endl;
    if (!_server.getLocationRedirect(locationBlock).empty())
        response = makeRedirection(_server.getLocationRedirect(locationBlock).begin()->first, _server.getLocationRedirect(locationBlock).begin()->second);
    else {
        if (path.size() > 3 && (path.substr(path.size() - 4) == ".py?" || path.substr(path.size() - 4) == ".pl?"))
            return executeCGI(path.substr(0, path.size() - 1));
        else if (Utils::isFile(path)) {
            if (!Utils::hasReadPermission((path).c_str()))
                throw std::runtime_error("403 Forbidden");
            htmlContent = Utils::readFile(path);
            if (htmlContent.empty())
                throw std::runtime_error("Failed to read html file.");
        }
        else if (locationIndex.empty()) {
            if (_server.getLocationDirectoryListing(locationBlock))
                htmlContent = listDir(_server.getLocationRoot(locationBlock));
            else
                throw std::runtime_error("403 Forbidden");
        }
        else {
            std::cout << "okkkkkkkk\n\n";
            response = makeRedirection("301", _server.getLocationIndex(locationBlock));
            return response;
        }
        std::string type = Utils::findType(locationIndex);
        size_t fileNamePos = locationIndex.find_last_of('/', 0);
        response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: " + type + "\r\n"
            "Content-Length: " + Utils::intToStr(htmlContent.size()) + "\r\n";
        if (type.find("image") != std::string::npos || type.find("pdf") != std::string::npos)
            response += "Content-Disposition: attachment; filename=\"" + path.substr(fileNamePos) + "\"\r\n";
        response +=
            "Connection: keep-alive\r\n"
            "Keep-Alive: timeout=10000\r\n"
            "\r\n" +
            htmlContent;
    }
    return response;
}

// Reponse dans le cas d'une requete POST
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

// std::string Client::respondToDelete()
// {
    
// }

// Scinde le body grace au delimteur et envoie les infos necessaires a la fonction saveFile
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

// reponse s'il s'agit d'une redirection
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

std::string Client::listDir(std::string root)
{
    std::vector<std::string> listing;
    std::string htmlContent;
    DIR* dir = opendir(root.c_str());
    if (dir == NULL)
        std::runtime_error("Could not open directory for listing.");
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        if (name == "." || name == "..")
            continue;
        listing.push_back(name);
    }
    closedir(dir);
    htmlContent = Client::displayList(listing);
    return htmlContent;
}

std::string Client::displayList(std::vector<std::string> listing)
{
    std::string location = _request.getLocation();
    std::string htmlContent;
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<title>" + location + "</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>INDEX</h1>\n"
    "<p>\n";

    if (location[0] != '/')
        location = "/" + location;
    for (size_t i(0); i < listing.size(); i++)
        htmlContent += "\t\t<p><a href=\"http://" + _server.getHost() + ":" + Utils::intToStr(_server.getPort()) + location + "/" + listing[i] + "\">" + listing[i] + "</a></p>\n";
    htmlContent +="\
    </p>\n\
    </body>\n\
    </html>\n";
    return htmlContent;
}

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

    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: " + Utils::intToStr(output.size()) + "\r\n"
           "Connection: keep-alive\r\n"
           "Keep-Alive: timeout=10000\r\n"
           "\r\n" +
           output;
}

// Repond en cas d'erreur, avec la page html correspondante ou une page par defaut si aucune n'a eete specifiee
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

        response =
        "HTTP/1.1 " + error + "\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: " + Utils::intToStr(htmlContent.size()) + "\r\n"
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


/* ---GETTERS--- */

int Client::getServerFd() {return this->_serverFd;}

int Client::getClientFd() {return this->_clientFd;}

size_t Client::getIndexServerFd() {return this->_indexServerFd;}

size_t Client::getIndexClientFd() {return this->_indexClientFd;}

size_t Client::getMaxBodySize() {return this->_maxBodySize;}

int Client::getPort() {return this->_port;}

size_t Client::getTimeout() {return this->_timeout;}

size_t Client::getTimeLastRequest() {return this->_timeLastRequest;}

std::string Client::getIp() {return this->_ip;}


/* ---SETTERS--- */

void Client::setIp(std::string ip) {this->_ip = ip;}

void Client::setClientFd(int clientFd) {this->_clientFd = clientFd;}

void Client::setIndexClientFd(size_t index) {this->_indexClientFd = index;}

void Client::setTimeout(size_t time) {this->_timeout = time;}

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