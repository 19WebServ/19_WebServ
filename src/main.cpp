#include "../include/Socket.hpp"
#include <poll.h>
#include <vector>
#include <iostream>
#include <fstream>

#define MAX_CLIENTS 100

std::string readFile(const char *filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return "";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, ' ');
    if (!file.read(&buffer[0], size)) {
        return "";
    }

    file.close();
    return buffer;
}

int init_server(Socket &server) {
    if (server.createSocket() < 0 || server.bindSocket() < 0 || server.listenSocket() < 0) 
        return 1;
    std::cout << "Server is listening on port 8080...\n" << std::endl;
    return 0;
}

void handle_client(int client_sock, Socket &server) {
    char buffer[1024] = {0};
    int bytes_receiv = server.receiveData(client_sock, buffer, sizeof(buffer));
    if (bytes_receiv > 0) {
        buffer[bytes_receiv] = '\0';
        std::cout << "Received from client:\n" << buffer << std::endl;

        // Lire le fichier index.html
        std::string htmlContent = readFile("pages_html/index.html");
        if (htmlContent.empty()) {
            std::cerr << "Failed to read index.html." << std::endl;
            return;
        }

        // Conversion de la taille en string pour C++98
        std::ostringstream oss;
        oss << htmlContent.size();
        std::string contentLength = oss.str();

        // Construire la réponse HTTP
        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + contentLength + "\r\n"
            "\r\n" +
            htmlContent;

        // Envoyer la réponse au client
        int bytes_sent = server.sendData(client_sock, response.c_str(), response.size());
        if (bytes_sent > 0)
            std::cout << "Response sent to client: HTTP/1.1 200 OK" << std::endl;
        else
            std::cerr << "Failed to send response to client." << std::endl;
    } else if (bytes_receiv == 0) {
        std::cout << "Client disconnected." << std::endl;
    } else {
        std::cerr << "Failed to receive data from client." << std::endl;
    }

    close(client_sock);
    std::cout << "Client socket closed." << std::endl;
}



void check_fd(Socket &server)
{
    for (size_t i = 0; i < server._poll_fds.size(); ++i) 
    {
        if (server._poll_fds[i].revents & POLLIN) {
            if (server._poll_fds[i].fd == server.getSocketFD())
            {
                // Nouvelle connexion
                int client_sock = server.acceptConnection();
                if (client_sock >= 0) 
                {
                    std::cout << "New client connected!" << std::endl;
                    struct pollfd client_pfd;
                    client_pfd.fd = client_sock;
                    client_pfd.events = POLLIN;
                    client_pfd.revents = 0;
                    server._poll_fds.push_back(client_pfd);
                }
            } 
            else 
            {
                // Données reçues d'un client existant
                handle_client(server._poll_fds[i].fd, server);
                server._poll_fds.erase(server._poll_fds.begin() + i); // Supprime le client déconnecté
                --i;
            }
        }
    }
}


int main()
{
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
