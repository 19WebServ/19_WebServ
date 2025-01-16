/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteBlock.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 19:23:03 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/16 13:14:13 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

RouteSettings extractLocationSettings(std::string configStr)
{
    RouteSettings routeBlock;
    std::istringstream ss;
    std::string word;
    size_t index;
    std::string settings;
    std::string keywords[6] = {"root", "index", "autoindex", "allowed_methods", "cgi_param", "return"};
    
    initRouteBlock(routeBlock);
    ss.str(configStr);
    while (getline(ss, word, ' ')) {
        index = 0;
        for (; index < sizeof(keywords) / sizeof(keywords[0]); index++) {
            if (keywords[index] == word)
                break;
        }
        settings = Utils::trimLine(configStr, ';');
        switch (index)
        {
        case 0:
            extractRouteRoot(routeBlock, settings);
            break;
        case 1:
            /* index */
            extractRouteIndex(routeBlock, settings);
            break;
        case 2:
            /* autoindex */
            extractRouteAutoindex(routeBlock, settings);
            break;
        case 3:
            /* methods */
            extractRouteMethod(routeBlock, settings);
            break;
        case 4:
            /* cgi */
            extractRouteCgi(routeBlock, settings);
            break;
        case 5:
            /* return */
            extractRouteRedir(routeBlock, settings);
            break;
        
        default:
            throw std::runtime_error(word + ": unknown keyword");
            break;
        }
        ss.str(configStr);
    }
    return routeBlock;
}

void initRouteBlock(RouteSettings &routeBlock)
{
    routeBlock.methods.push_back("GET");
    routeBlock.root = "";
    routeBlock.directoryListing = false;
    routeBlock.index = "";
    routeBlock.redirect = "";
    routeBlock.cgi = "";
}

void extractRouteRoot(RouteSettings &route, std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing root path.");
    if (!ss.eof())
        throw std::runtime_error("More than 1 root.");
    if (!Utils::isDir(word))
        throw std::runtime_error("Invalid root.");
    else if (!Utils::hasRootDirectoryAccess(word.c_str()))
        throw std::runtime_error("Root directory does not have necessary access.");
    if (word[word.size() - 1] == '/')
        route.root = word.substr(0, word.size() - 1);
    else
        route.root = word;
}

void extractRouteIndex(RouteSettings &route, std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing index page.");
    if (!ss.eof())
        throw std::runtime_error("More than 1 error page.");
    if (Utils::isPath(word))
        route.index = word;
    else
        route.index = "/" + word;
}

void extractRouteAutoindex(RouteSettings &route, std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing directory listening parameter.");
    if (!ss.eof())
        throw std::runtime_error("Too much arguments for directory listing.");
    if (word == "on")
        route.directoryListing = true;
    else if (word == "off")
        route.directoryListing = false;
    else
        throw std::runtime_error(word + "invalid parameter for directory listing.");
}

void extractRouteMethod(RouteSettings &route, std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    while (getline(ss, word, ' ')) {
        if (word == "GET" || word == "POST" || word == "DELETE") {
            std::vector<std::string>::iterator it;
            it = std::find(route.methods.begin(), route.methods.end(), word);
            if (it == route.methods.end())
                route.methods.push_back(word);
        }
        else
            throw std::runtime_error(word + " : unknown method.");
    }
    if (route.methods.empty())
        throw std::runtime_error("no allowed method precised.");
}

void extractRouteCgi(RouteSettings &route, std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing CGI script name.");
    if (!ss.eof())
        throw std::runtime_error("More than 1 CGI script name.");
    if (Utils::isPath(word))
        route.cgi = word;
    else
        route.cgi = "/" + word;
}

void extractRouteRedir(RouteSettings &route, std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word != "301")
        throw std::runtime_error("status code is not '301'");
    getline(ss, word, ' ');
    if (!ss.eof())
        throw std::runtime_error("Too much redirections.");
    else
        route.redirect = word; 
}
