/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 12:41:07 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/14 14:24:19 by vdecleir         ###   ########.fr       */
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


struct RouteSettings {
    std::vector<std::string> methods;
    std::string root;
    bool directoryListing;
    std::string index;
    std::string redirect;
    std::string cgi;
};


class ServerConfig
{
private:
    int _port;
    std::vector<std::string> _serverName;
    std::map<int, std::string> _errorPages;
    size_t _clientBodyLimit;
    std::string _root;
    std::string _index;
    std::map<std::string, RouteSettings> _routes;

public:
    ServerConfig();
    ~ServerConfig();

    void extractPort(std::string);
    void extractServerName(std::string);
    void extractErrorPage(std::string);
    void extractMaxBodySize(std::string);
    void extractRoot(std::string);
    void extractIndex(std::string);
    void extractLocation(std::string);
    void checkIfValidPath();
    void checkMissigValues();

    int                         getPort();
    size_t                      getBodySize();
    std::vector<std::string>    getServerNames();
    std::string                 getErrorPage(int);
    std::string                 getRoot();
    std::string                 getIndex();
    bool                        getLocationAllowedMethods(std::string, std::string);
    std::string                 getLocationRoot(std::string);
    bool                        getLocationDirectoryListing(std::string);
    std::string                 getLocationIndex(std::string);
    std::string                 getLocationRedirect(std::string);
    std::string                 getLocationCgi(std::string);

    friend std::ostream& operator<<(std::ostream& os, const ServerConfig& obj);
};

RouteSettings extractLocationSettings(std::string);
void extractRouteRoot(RouteSettings &, std::string);
void extractRouteIndex(RouteSettings &, std::string);
void extractRouteAutoindex(RouteSettings &, std::string);
void extractRouteMethod(RouteSettings &, std::string);
void extractRouteCgi(RouteSettings &, std::string);
void extractRouteRedir(RouteSettings &, std::string);

#endif