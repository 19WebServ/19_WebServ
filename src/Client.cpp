#include "../include/Client.hpp"

Client::Client(int serverFd, int indexServerSock, int port, ServerConfig server) : _server(server), _serverFd(serverFd), _indexServerFd(indexServerSock), _port(port)
{
    this->_maxBodySize = this->_server.getBodySize();
    this->_totalSent = 0;
    // std::cerr << "client cree" << std::endl;
}

Client::Client(const Client &rhs) {
    *this = rhs;
    // std::cout << "Client copy construct" << std::endl;
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
    // std::cout << request << std::endl;
    if (_request.getIfComplete() == false) {
        std::string totalRequest = _request.getContent() + request;
        _request.setContent(totalRequest);
        // std::cout << "Total request size :" << totalRequest.size() << std::endl;
        // std::cout << "Needed content len :" << _request.getContentLen() << std::endl;
        if (totalRequest.size() - 1 == _request.getContentLen()) {
            std::cout << "\n REQUEST COMPLETED\n" << std::endl;
            _request.setComplete(true);
        }
    }
    else if (_request.getIfComplete() == true && request.find("HTTP/1.1") == std::string::npos)
        throw std::runtime_error("400 Bad Request");
    else {
        // std::cout << "\n" << request << std::endl;
        getline(ss, method, ' ');
        getline(ss, path, ' ');
        ss.clear();
        ss.str(path);
        getline(ss, location, '/'); // Voir comment je dois gerer ici -> differencier une requete d'une page direct d'une location vide
        if (!ss.eof() && location.empty())
            getline(ss, location, '/');
        if (!path.empty())
            path = path.substr(location.size() + 1);
        path = path.erase(0, 1);
        for (size_t i(0); i < _server.getLocationAllowedMethods(location).size(); i++) {
            if (method == _server.getLocationAllowedMethods(location)[i]) {
                createRequest(request, location, method, path);
                allowed = true;
            }
        }
        if (allowed == false)
            throw std::runtime_error("405 Method Not Allowed");
    }
}

// cree un objet requete avec les infos importantes du header et, si Post, recupere le body
void Client::createRequest(std::string requestStr, std::string location, std::string method, std::string path)
{
    Request temp(location, path, method);

    std::istringstream ss;
    std::string line;
    std::string body;
    size_t pos;

    ss.str(requestStr);
    while (getline(ss, line)) {
        pos = line.find("boundary=");
        if (pos != std::string::npos) {
            temp.setBoundary(line.substr(pos + 9));
            // std::cout << "boundary: " << temp.getBoundary() << std::endl;
        }
        pos = line.find("Content-Length: ");
        if (pos != std::string::npos)
            temp.setContentLen(atoi(line.substr(pos + 16).c_str()));
        if (!line.empty() && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
        // std::cout << line << std::endl;
        if (line.empty())
            break;
    }
    while (getline(ss, line)) {
        body += (line + '\n');
    }
    // std::cout << "body size: " << body.size() << std::endl;
    // std::cout << "content len: " << temp.getContentLen() << std::endl;

    if (temp.getContentLen() == body.size())
        temp.setComplete(true);
    else
        temp.setComplete(false);
    if (body.size() > _maxBodySize)
        throw std::runtime_error("413 Content Too Large");
    temp.setContent(body);
    this->_request = temp;
}


/* ---RESPONSE--- */

// dispatch la requete a la methode correspondante
void    Client::sendResponse()
{
    size_t index(0);
    std::string methods[3] = {"GET", "POST", "DELETE"};
    std::string response;

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
        response = this->respondToDelete();
        break;
        
    default:
        break;
    }
    _response = response;
    _responseLen = _response.size();
}

// Reponse dans le cas d'une requete GET
std::string Client::respondToGet()
{
    std::string htmlContent;
    std::string response;
    std::string locationBlock = _request.getLocation();
    std::string locationRoot = _server.getLocationRoot(locationBlock);
    std::string locationIndex;
    std::string query;
    bool download = false;

    if (_request.getPath().empty())
        locationIndex = _server.getLocationIndex(locationBlock);
    else
        locationIndex = _request.getPath();
    std::string path = locationRoot + "/" + locationIndex;
    if (!_server.getLocationRedirect(locationBlock).empty())
        response = makeRedirection(_server.getLocationRedirect(locationBlock).begin()->first, _server.getLocationRedirect(locationBlock).begin()->second);
    else {
        if (path.find("?") != std::string::npos) 
        {
            if (path.find("download=true"))
                download = true;
            std::istringstream ss;
            ss.str(path);
            getline(ss, path, '?');
            getline(ss, query, '\0');
        }
        // std::cout << path << std::endl;
        // std::cout << query << std::endl;
        // std::cout<< "Root : "<< locationRoot << std::endl;
        // std::cout<< "Index : "<< locationIndex << std::endl;
        if (path.size() > 3 && (path.substr(path.size() - 3) == ".py" || path.substr(path.size() - 3) == ".pl"))
            return executeCGI(path.substr(0, path.size()), query);
        if (Utils::isFile(path)) {
            if (!Utils::hasReadPermission((path).c_str()))
                throw std::runtime_error("403 Forbidden");
            htmlContent = Utils::readFile(path);
            if (htmlContent.empty())
                throw std::runtime_error("Failed to read html file.");
        }
        else if (Utils::isDir(path)) {
            if (locationIndex.empty()) {
                if (_server.getLocationDirectoryListing(locationBlock))
                    htmlContent = listDir(_server.getLocationRoot(locationBlock));
                else
                    throw std::runtime_error("403 Forbidden");
            }
            else {
                response = makeRedirection("301", _server.getLocationIndex(locationBlock));
                return response;
            }
        }
        else
            throw std::runtime_error("404 Not Found");
        // std::cout << "response size : " << htmlContent.size() << std::endl;
        if (htmlContent.size() > _maxBodySize)
            throw std::runtime_error("413 Content Too Large");
        std::string type = Utils::findType(locationIndex);
        size_t fileNamePos = path.find_last_of('/');
        response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: " + type + "\r\n"
            "Content-Length: " + Utils::intToStr(htmlContent.size()) + "\r\n";
        if (download == false)
            response += "Content-Disposition: inline;\r\n";
        else
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
    std::string locationBlock = _request.getLocation();
    std::string locationRoot = _server.getLocationRoot(locationBlock);
    std::string locationIndex = _request.getPath();
    std::string path = locationRoot + "/" + locationIndex;
    if (path.size() > 3 && (path.substr(path.size() - 3) == ".py" || path.substr(path.size() - 3) == ".pl" || path.substr(path.size() - 4) == ".cgi")) {
        return executeCGI(path, "");
    }
    std::string response;
    postContent();
    response =
        "HTTP/1.1 201 Created\r\n"
        "Content-Type: text/plain\r\n"
        "Refresh: 0\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";
    return response;
}

std::string Client::respondToDelete()
{
    std::string response;
    std::string toDelete;
    std::string baseDir;
    std::string completePath = _server.getLocationRoot(_request.getLocation()) + _request.getPath();
    if (completePath[completePath.size() - 1] == '/')
        completePath = completePath.substr(0, completePath.size() - 1);
    size_t pos = completePath.find_last_of('/');
    if (pos == std::string::npos)
        toDelete = completePath;
    else {
        toDelete = completePath.substr(pos + 1);
        baseDir = completePath.substr(0, pos);
    }
    if (Utils::isDeletable(baseDir, toDelete)) {
        if (std::remove((completePath).c_str()))
            throw std::runtime_error("Error while deleting the file.");
        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: " + Utils::findType(toDelete) + "\r\n"
            "Content-Length: 31\r\n"
            "\r\n"
            "Ressource deleted successfully\n";
    }
    return response;
}

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
                if (!filename.empty())
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
        htmlContent += "\t\t<p><a href=\"http://" + _server.getHost() + ":" + Utils::intToStr(_port) + location + "/" + listing[i] + "?download=true\">" + listing[i] + "</a></p>\n";
    htmlContent +="\
    </p>\n\
    </body>\n\
    </html>\n";
    return htmlContent;
}

std::string Client::executeCGI(const std::string& scriptPath, std::string query) 
{
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

        std::string method = _request.getMethod();
        std::string queryString = query.empty() ? _request.getQuery() : query;
        std::string body = _request.getContent();
        if (!body.empty()) body.substr(0, body.size() - 1);
        std::string contentLength = Utils::intToStr(body.size());

        std::vector<std::string> envVars;
        envVars.push_back("REQUEST_METHOD=" + method);
        if (queryString.empty())
            queryString = body;
        envVars.push_back("QUERY_STRING=" + queryString);
        envVars.push_back("CONTENT_LENGTH=" + contentLength);
        envVars.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
        envVars.push_back("SCRIPT_FILENAME=" + scriptPath);

        std::vector<char*> envp;
        for (size_t i = 0; i < envVars.size(); ++i)
            envp.push_back(const_cast<char*>(envVars[i].c_str()));
        envp.push_back(NULL);

        std::cerr << "=== ENVIRONMENT VARIABLES ===" << std::endl;
        for (size_t i = 0; envp[i] != NULL; i++) {
            std::cerr << envp[i] << std::endl;
        }
        std::cerr << "=============================" << std::endl;

        char *argv[] = {(char *)scriptPath.c_str(), NULL};
        std::cerr << "HOOOOO" << std::endl;
        execve(argv[0], argv, &envp[0]);
        std::cerr << "AAAAAA" << std::endl;
        perror("execve");
        exit(1);
    }
    close(pipefd[1]);
    char buffer[4096];
    std::string output;
    ssize_t bytesRead;

    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) 
    {
        buffer[bytesRead] = '\0';
        output += buffer;
    }
    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        throw std::runtime_error("500 Internal Server Error: CGI script failed");
    // std::cout << "OLAAAA" << std::endl;
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: " + Utils::intToStr(output.size()) + "\r\n"
           "Connection: keep-alive\r\n\r\n" +
           output;
}

// Repond en cas d'erreur, avec la page html correspondante ou une page par defaut si aucune n'a eete specifiee
void Client::handleErrorResponse(std::string error)
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
        if (!errorPage.empty()) {
            htmlContent = Utils::readFile(root + errorPage);
            if (htmlContent.size() > _maxBodySize)
                htmlContent = error;
        }
        else
            htmlContent = error;
        if (htmlContent.empty())
            throw std::runtime_error("Failed to read html file.");
        if (htmlContent.size() > _maxBodySize)
            throw std::runtime_error("413 Content Too Large");
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
    _response = response;
    _responseLen = _response.size();
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

Request Client::getRequest() {return this->_request;}

std::string Client::getResponse() {return this->_response;}

size_t Client::getResponseLen() {return this->_responseLen;}

size_t Client::getTotalSent() {return this->_totalSent;}

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

void Client::setTotalSent(int totalSent) {this->_totalSent = totalSent;}