#pragma once
# include <cstdlib>
# include <iostream>

class Client
{
    public:
        Client(int ServerFd, int indexServerSock, int port, size_t bodySize);
        ~Client();

        int     getServerFd();
        size_t  getIndexServerFd();
        int     getClientFd();
        size_t  getIndexClientFd();
        size_t  getMaxBodySize();
        int     getPort();

        void    setIp(std::string ip);
        void    setClientFd(int clientFd);
        void    setIndexClientFd(size_t index);


    private:
        int             _serverFd;
        size_t          _indexServerFd;
        int             _clientFd;
        size_t          _indexClientFd;
        std::string     _ip;
        size_t          _maxBodySize;
        int             _port;
};