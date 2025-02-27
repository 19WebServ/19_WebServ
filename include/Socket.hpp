/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:45:59 by vdecleir          #+#    #+#             */
/*   Updated: 2025/02/27 15:30:08 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <csignal>
#include <iostream>
#include <vector>
#include <poll.h>
#include "ConfigFile.hpp"
#include "Client.hpp"

class Socket 
{
    public:
        Socket(const std::vector<int> &ports, const std::vector<ServerConfig> &server);
        Socket(const Socket &rhs);
        ~Socket();

        void            launchServer();
        static void     signalHandler(int signum);

        /* ---GETTERS--- */
        int             getPort(int index);
        ServerConfig    getServer(int index);
        static Socket*  getInstance();

    private:
        /* ---METHODS--- */
        void        closeFds(std::vector<int>serverSocks);
        void        acceptConnection(int serverSock, int i);
        void        handleClient(int &clientFd, Client &client);
        std::string getClientIP(struct sockaddr_in *client_addr);
        int         processingRequest(std::string request, Client &client);
        int         processingResponse(int clientFd, Client &client);
        int         sendData(int target_sock, std::string toSend);
        int         receiveData(int target_sock, std::string &request);

        /* ---ATTRIBUTES--- */
        std::vector<int> _ports;
        std::vector<int> _serverSocks;
        std::vector<pollfd> _poll_fds;
        std::vector<ServerConfig> _servers;
        std::vector<Client> _clients;
        static Socket*     _instance;
};

#endif