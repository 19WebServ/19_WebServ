/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 17:32:17 by vdecleir          #+#    #+#             */
/*   Updated: 2025/02/27 14:51:14 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cstdlib>
# include <iostream>
# include <sstream>
# include <string>
# include <dirent.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <fcntl.h>
# include "./ServerConfig.hpp"
# include "./Request.hpp"
# include "./Utils.hpp"

class Client
{
    public:
        Client(int ServerFd, int indexServerSock, int port, ServerConfig server);
        Client();
        Client(const Client &rhs);
        ~Client();

        /* ---GETTERS--- */
        int         getServerFd();
        size_t      getIndexServerFd();
        int         getClientFd();
        size_t      getIndexClientFd();
        size_t      getMaxBodySize();
        int         getPort();
        size_t      getTimeout();
        size_t      getTimeLastRequest();
        std::string getIp();
        Request     getRequest();
        std::string getResponse();
        size_t      getResponseLen();
        size_t      getTotalSent();
        // std::string getRequestStr();

        /* ---SETTERS--- */
        void    setIp(std::string ip);
        void    setClientFd(int clientFd);
        void    setIndexClientFd(size_t index);
        void    setTimeout(size_t time = 100);
        void    setTimeLastRequest();
        void    setTotalSent(int);
        // void    setRequest(std::string str);

        /* ---REQUEST--- */
        void    parseRequest(std::string);

        /* ---RESPONSE--- */
        void sendResponse();
        void handleErrorResponse(std::string);
        
    private:
        /* ---REQUEST--- */
        void    createRequest(std::string, std::string, std::string, std::string);
        
        /* ---RESPONSE--- */
        std::string respondToGet();
        std::string respondToPost();
        std::string respondToDelete();

        void        postContent();
        std::string makeRedirection(std::string, std::string);
        std::string listDir(std::string);
        std::string displayList(std::vector<std::string>);
        std::string executeCGI(const std::string& scriptPath, std::string query);

        /* ---ATTRIBUTES---*/
        ServerConfig    _server;
        Request         _request;
        int             _serverFd;
        size_t          _indexServerFd;
        int             _clientFd;
        size_t          _indexClientFd;
        std::string     _ip;
        size_t          _maxBodySize;
        int             _port;
        size_t          _timeLastRequest;
        size_t          _timeout;
        std::string     _response;
        size_t          _responseLen;
        size_t          _totalSent;
        
        
};

#endif