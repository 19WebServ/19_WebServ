#include "../include/Poll.hpp"
#include "../include/Handle_client.hpp"
#include <vector>

#define MAX_CLIENTS 100

/**
 * @brief request client
 * 
 * @param  client_sock, Socket_instance
 * 
 * @return none
 */
void handle_client_nonblocking(int client_sock, Socket &server) {
    char buffer[1024];
    int bytes_receiv = server.receiveData(client_sock, buffer, sizeof(buffer));
    if (bytes_receiv > 0) 
    {
        if (processing_request(buffer, bytes_receiv, server, client_sock))
            return;
    }
    else if (bytes_receiv == 0)
        std::cout << "Client disconnected." << std::endl;
    else if (errno != EWOULDBLOCK && errno != EAGAIN)
        std::cerr << "Failed to receive data from client." << std::endl;
}

/**
 * @brief fd management for poll function
 * 
 * @param Socket_instance
 * 
 * @return none
 */
void check_fd(Socket &server) {
    for (size_t i = 0; i < server._poll_fds.size(); ++i) 
    {
        if (server._poll_fds[i].revents & POLLIN) 
        {
            if (server._poll_fds[i].fd == server.getSocketFD())
                new_client_connection(server);
            else 
            {
                handle_client_nonblocking(server._poll_fds[i].fd, server);
                i = disconnection_check(server, i);
            }
        }
    }
}

int main() {
    Socket server(8080, 0);
    if (init_server(server))
        return -1;
    while (true)
    {
        int event_count = server.server_poll();
        if (event_count < 0)
            break;
        check_fd(server);
    }
    server.closeSocket();
    return 0;
}
