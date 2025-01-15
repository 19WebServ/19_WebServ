#pragma once
# include <cstdlib>
# include <iostream>
# include <ctime>

class Client
{
    public:
        Client(int ServerFd, int indexServerSock, int port, size_t bodySize);
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
        void    setRequest(std::string str);


    private:
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