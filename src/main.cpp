#include "Socket.hpp"


int init_server(Socket server)
{
    if (server.createSocket() < 0 || server.bindSocket() < 0 || server.listenSocket() < 0) 
        return 1;
    std::cout << "Server is listening port 8080..." << std::endl;
    return 0;
}

int main() 
{
    Socket server(8080, 0);
    if (init_server(server))
        return -1;

    server.closeSocket();
    return 0;
}