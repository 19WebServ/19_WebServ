/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhouyet <jhouyet@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 07:59:57 by jhouyet           #+#    #+#             */
/*   Updated: 2024/12/11 10:49:58 by jhouyet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cstdio>

/**
 * @brief Construct a new Server:: Server object
 * 
 * @param configFile 
 */
Server::Server(const std::string &configFile) : _configFile(configFile) {
    std::cout << "Server initialized with config file: " << _configFile << std::endl;
}

/**
 * @brief Run the server
 * 
 */
void Server::run() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    const int PORT = 8080; // Port to listen on

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Define the address and port
    memset(&address, 0, sizeof(address)); // Clear the structure
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    address.sin_port = htons(PORT); // Convert port to network byte order

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is running on localhost:" << PORT << std::endl;

    while (true) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue; // Continue to accept new connections even if there's an error
        }
        std::cout << "New connection accepted." << std::endl;

        // Prepare the response
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: 12\r\n"; // Length of "Hello World"
        response += "\r\n"; // End of headers
        response += "Hello World"; // The body of the response

        // Send the response
        send(new_socket, response.c_str(), response.size(), 0);
        std::cout << "Response sent." << std::endl;

        // Shutdown the socket for sending and receiving
        shutdown(new_socket, SHUT_RDWR);
        // Close the connection
        close(new_socket);
    }
}