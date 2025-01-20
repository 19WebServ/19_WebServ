#pragma once
# include <cstdlib>
# include <iostream>
# include <ctime>
# include <sstream>
# include <string>
# include "./ServerConfig.hpp"
# include "./Request.hpp"
# include "./Utils.hpp"

class Client
{
    public:
        Client(int ServerFd, int indexServerSock, int port, ServerConfig server);
        Client();
        ~Client();

        int         getServerFd();
        size_t      getIndexServerFd();
        int         getClientFd();
        size_t      getIndexClientFd();
        size_t      getMaxBodySize();
        int         getPort();
        size_t      getTimeout();
        size_t      getTimeLastRequest();
        std::string getIp();
        // std::string getRequestStr();

        void    setIp(std::string ip);
        void    setClientFd(int clientFd);
        void    setIndexClientFd(size_t index);
        void    setTimeout(size_t time = 100);
        void    setTimeLastRequest();
        // void    setRequest(std::string str);

        void    parseRequest(std::string);
        void    setRequest(std::string, std::string, std::string);
        std::string    sendResponse();

        std::string respondToGet();
        std::string respondToPost();
        std::string respondToDelete();
        std::string handleErrorResponse(std::string);
        std::string makeRedirection(std::string, std::string);
        void        postContent();
        std::string displayDirList(std::string);
        std::string executeCGI(const std::string& scriptPath);


    private:
        ServerConfig    _server;
        Request         _request;
        int             _serverFd;
        size_t          _indexServerFd;
        int             _clientFd;
        size_t          _indexClientFd;
        std::string     _ip;
        size_t          _maxBodySize;
        int             _port;
        size_t          _timeLastRequest;
        size_t          _timeout;

};