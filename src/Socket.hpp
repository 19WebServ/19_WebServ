#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Socket 
{
    public:
        Socket(int port, unsigned int ip);
        ~Socket();

        int     createSocket();
        int     bindSocket();
        int     listenSocket(int backlog = 5);
        int     acceptConnection();
        void    sendResponse(int client_sock, const std::string& response);
        void    closeSocket();

    private:
        int _server_sock;
        struct sockaddr_in _server_addr;
};
