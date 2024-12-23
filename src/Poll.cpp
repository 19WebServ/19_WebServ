#include "../include/Poll.hpp"

/**
 * @brief Create a new customer and add it to the fd table to monitor
 * 
 * @param Socket_instance
 * 
 * @return none
 */
void    new_client_connection(Socket &server)
{
    int client_sock = server.acceptConnection();
    if (client_sock >= 0) 
    {
        std::cout << "New client connected!" << std::endl;
        struct pollfd client_pfd;
        client_pfd.fd = client_sock;
        client_pfd.events = POLLIN | POLLOUT;
        client_pfd.revents = 0;
        server._poll_fds.push_back(client_pfd);
    }
}

/**
 * @brief Check if customer i is disconnected, if so close his fd
 * 
 * @param Socket_instance, int 
 * 
 * @return int
 */
int    disconnection_check(Socket &server, int i)
{
    char check_buffer;
    int res = recv(server._poll_fds[i].fd, &check_buffer, 1, MSG_PEEK | MSG_DONTWAIT);
    if (res == 0 || (res < 0 && errno != EWOULDBLOCK && errno != EAGAIN)) 
    {
        std::cout << "Client disconnected." << std::endl;
        close(server._poll_fds[i].fd);
        server._poll_fds.erase(server._poll_fds.begin() + i);
        --i;
    }
    return i;
}