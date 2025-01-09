#pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <poll.h>
#include <cerrno>

class Socket 
{
    public:
        Socket(const std::vector<int> &ports);
        ~Socket();

        void        launchServer();

    private:
        std::vector<int> _ports;
        std::vector<int> _serverSocks;
        std::vector<pollfd> _poll_fds;
        

        void        closeFds(std::vector<int>serverSocks);
        void        acceptConnection(int serverSock);
        void        handleClient(int clientFd);
        std::string getClientIP(struct sockaddr_in *client_addr);
        std::string readFile(const char *filename); 
        int         processingRequest(char *buffer, int bytes_receive, int client);
        int         sendData(int target_sock, const char *data, unsigned int len);
        int         receiveData(int target_sock, char *buffer, unsigned int len);
};

