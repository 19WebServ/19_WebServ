#include "../include/Socket.hpp"
#include <poll.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#define MAX_CLIENTS 100

/**
 * @brief read a html file
 * 
 * @param filename
 * 
 * @return std::string buffer
 */
std::string readFile(const char *filename) 
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return "";

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, ' ');
    if (!file.read(&buffer[0], size))
        return "";
    file.close();
    return buffer;
}

/**
 * @brief server initialization
 * 
 * @param Socket_instance
 * 
 * @return int 
 */
int init_server(Socket &server) 
{
    if (server.createSocket() < 0 || server.bindSocket() < 0 || server.listenSocket() < 0) 
        return 1;
    std::cout << "Server is listening on port " << server.getPort() << "\n"<< std::endl;
    return 0;
}

// Traitement d'un client non bloquant
void handle_client_nonblocking(int client_sock, Socket &server) {
    char buffer[1024];
    int bytes_receiv = server.receiveData(client_sock, buffer, sizeof(buffer));
    if (bytes_receiv > 0) 
    {
        buffer[bytes_receiv] = '\0';
        std::cout << "Received from client:\n" << buffer << std::endl;

        // Lire le fichier index.html
        std::string htmlContent = readFile("pages_html/index.html");
        if (htmlContent.empty()) 
        {
            std::cerr << "Failed to read index.html." << std::endl;
            return;
        }

        std::ostringstream oss;
        oss << htmlContent.size();
        std::string contentLength = oss.str();

        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + contentLength + "\r\n"
            "\r\n" +
            htmlContent;

        // Envoi non bloquant
        int total_sent = 0;
        while (total_sent < (int)response.size())
        {
            int bytes_sent = server.sendData(client_sock, response.c_str() + total_sent, response.size() - total_sent);
            if (bytes_sent > 0)
                total_sent += bytes_sent;
            else if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                std::cerr << "Failed to send response to client." << std::endl;
                break;
            }
        }
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
            {
                // Nouvelle connexion
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
            else 
            {
                handle_client_nonblocking(server._poll_fds[i].fd, server);

                // Vérification de la déconnexion client
                char check_buffer;
                int res = recv(server._poll_fds[i].fd, &check_buffer, 1, MSG_PEEK | MSG_DONTWAIT);
                if (res == 0 || (res < 0 && errno != EWOULDBLOCK && errno != EAGAIN)) 
                {
                    std::cout << "Client disconnected." << std::endl;
                    close(server._poll_fds[i].fd);
                    server._poll_fds.erase(server._poll_fds.begin() + i);
                    --i;
                }
            }
        }
    }
}

// Fonction principale
int main() {
    Socket server(8080, 0);

    // Initialisation du serveur
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
