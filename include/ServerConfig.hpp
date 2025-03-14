/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 12:41:07 by vdecleir          #+#    #+#             */
/*   Updated: 2025/02/18 13:39:42 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>
# include <sstream>
# include <cstdlib>
# include "./Utils.hpp"
# include <algorithm>
# include "./RouteBlock.hpp"

class ServerConfig
{
public:
    ServerConfig();
    ServerConfig(const ServerConfig &);
    ~ServerConfig();

    /* ---GETTERS--- */
    std::vector<int>    getPort();
    size_t              getBodySize();
    std::vector<std::string> getServerNames();
    std::string getErrorPage(int);
    std::string getRoot();
    std::string getHost();
    std::string getIndex();
    std::vector<std::string> getLocationAllowedMethods(std::string);
    // bool isAllowed(std::string, std::string);
    std::string getLocationRoot(std::string);
    bool getLocationDirectoryListing(std::string);
    std::string getLocationIndex(std::string);
    std::map<std::string, std::string> getLocationRedirect(std::string);
    std::string getLocationCgi(std::string);

    // friend std::ostream& operator<<(std::ostream& os, const ServerConfig& obj);

    /* --- PARSING--- */
    void extractPort(std::string);
    void extractHost(std::string);
    void extractServerName(std::string);
    void extractErrorPage(std::string);
    void extractMaxBodySize(std::string);
    void extractRoot(std::string);
    // void extractIndex(std::string);
    void extractLocation(std::string);
    void checkIfValidPath();
    void checkMissigValues();
    
private:
    /* ---ATTRIBUTES--- */
    std::vector<int> _port;
    std::string _host;
    std::vector<std::string> _serverName;
    std::map<int, std::string> _errorPages;
    size_t _clientBodyLimit;
    std::string _root;
    std::string _index;
    std::map<std::string, RouteSettings> _routes;

};

#endif