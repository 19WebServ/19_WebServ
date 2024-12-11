#include <iostream>
#include <cstring>           // Pour memset
#include <sys/socket.h>      // Pour les sockets
#include <netinet/in.h>      // Pour sockaddr_in
#include <unistd.h>          // Pour close()

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
