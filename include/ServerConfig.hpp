/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 12:41:07 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/18 17:53:02 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>

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
    std::string host;
    int port;
    std::string serverName;

    std::map<int, std::string> defaultErrorPages;

    int clientBodyLimit;

    std::vector<Route> routes;

public:
    ServerConfig(const std::string &);
    ~ServerConfig();
};

#endif