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

class Socket 
{
    public:
        Socket(int port, unsigned int ip);
        ~Socket();

        int     createSocket();
        int     bindSocket();
        int     listenSocket(int backlog = 5); //backlog -> nombre deconnexion en attente pouvant etre géré
        int     acceptConnection();
        void    closeSocket();

        int     getPort();
        int     sendData(int target_sock, const char *data, unsigned int len);
        int     receiveData(int target_sock, char *buffer, unsigned int len);
        int     getSocketFD() const;

        std::string getClientIP(struct sockaddr_in *client_addr);

        int server_poll();
        std::vector<pollfd> _poll_fds;


    private:
        int _server_sock;
        const int _port;
        struct sockaddr_in _server_addr;

        /*gestion poll*/
        struct pollfd _pfd;
};

