/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 12:41:07 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/06 18:23:15 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>
# include <sstream>
# include <cstdlib>
# include <sys/stat.h>

struct Route {
    std::string path;
    std::vector<std::string> methods;
    std::string root;
    bool directoryListing;
    std::string defaultFile;
    std::string redirectTo;
    std::string cgiHandler;
    std::vector<std::string> cgiExtensions;
    std::string uploadDir;
};


class ServerConfig
{
private:
    std::vector<int> _port;
    std::vector<std::string> _serverName;
    std::map<int, std::string> _errorPages;
    size_t _clientBodyLimit;
    std::string _root;
    std::string _index;
    std::vector<Route> _routes;

public:
    ServerConfig();
    ~ServerConfig();

    void extractPort(std::string);
    void extractServerName(std::string);
    void extractErrorPage(std::string);
    void extractMaxBodySize(std::string);
    void extractRoot(std::string);
    void extractIndex(std::string);
    // void extractLacoation(std::string);

    std::vector<int>    getPorts();
    size_t              getBodySize();

    friend std::ostream& operator<<(std::ostream& os, const ServerConfig& obj);
};

bool areOnlyDigits(std::string);
// bool isValidServerName(std::string);

#endif
