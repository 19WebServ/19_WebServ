#pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>

// class Socket 
// {
//     public:
//         Socket(int port, unsigned int ip);
//         ~Socket();

//         int     createSocket();
//         int     bindSocket();
//         int     listenSocket(int backlog = 5); //backlog -> nombre deconnexion en attente pouvant etre géré
//         int     acceptConnection();
//         void    closeSocket();

//         int     sendData(int target_sock, const char *data, unsigned int len);
//         int     receiveData(int target_sock, char *buffer, unsigned int len);

//         std::string getClientIP(struct sockaddr_in *client_addr);

//     private:
//         int _server_sock;
//         struct sockaddr_in _server_addr;
// };

class Socket 
{
    public:
        Socket();
        virtual ~Socket();
        int     sendData(int target_sock, const char *data, unsigned int len);
        int     receiveData(int target_sock, char *buffer, unsigned int len);
        void    closeSocket();
    
    protected:
        int _socket_fd;
        sockaddr_in _addr;
};

class ServerSocket: public Socket
{
    public:
        ServerSocket(int port);
        int     bindSocket();
        int     listenSocket(int backlog = 5); //backlog -> nombre deconnexion en attente pouvant etre géré
        int     acceptConnection();
        std::string getClientIP(struct sockaddr_in *client_addr);

};

class ClientSocket : public Socket
{
    public:
        ClientSocket(const std::string &host, int port);
        void connectToServer();
};
