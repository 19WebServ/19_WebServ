/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 12:42:41 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/18 12:44:33 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
    _clientBodyLimit = 1000000;
    _root = "/var/www/html";
    _index = "index.html";
}

ServerConfig::~ServerConfig()
{
}

// Check if the number is in  the right range of availables ports and push baack in the vector.
void ServerConfig::extractPort(std::string setting)
{
    std::istringstream ss;
    std::string word;
    float nb;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing port.");
    if (Utils::areOnlyDigits(word)) {
        nb = atof(word.c_str());
        if (nb < 1024 || nb > 65535)
            throw std::runtime_error(word + " : invalid port.");
        else
            _port = static_cast<int>(nb);
    }
    else
        throw std::runtime_error(word + " : invalid port.");
}

// Check if the name is valid before pushing it to the _serveName vector.
void ServerConfig::extractServerName(std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    while (getline(ss, word, ' '))
        _serverName.push_back(word);
}

void ServerConfig::extractErrorPage(std::string setting)
{
    std::istringstream ss;
    std::string word;
    float error;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing error page.");
    if (!Utils::areOnlyDigits(word) || atof(word.c_str()) < 400 || atof(word.c_str()) > 599)
        throw std::runtime_error(word + " : invalid error status.");
    else {
        error = atof(word.c_str());
        getline(ss, word, ' ');
        if (word.empty())
            throw std::runtime_error("missing error page.");
        if (!ss.eof())
            throw std::runtime_error("More than 1 error page.");
        if (Utils::isPath(word))
            _errorPages[error] = word;
        else
            _errorPages[error] = "/" + word;
    }
}

void ServerConfig::extractMaxBodySize(std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing client max body size.");
    if (Utils::areOnlyDigits(word))
        _clientBodyLimit = atof(word.c_str());
    else
        throw std::runtime_error(word + " : invalid maximum body size");
}

void ServerConfig::extractRoot(std::string setting)
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
    if (word[word.size() - 1] == '/')
        _root = word.substr(0, word.size() - 1);
    else
        _root = word;
}

void ServerConfig::extractIndex(std::string setting)
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
        _index = word;
    else
        _index = "/" + word;
}

void ServerConfig::extractLocation(std::string setting)
{
    RouteSettings locationSettings = {};
    std::istringstream ss;
    std::string word;
    std::string locationSettingsStr;
    std::string route;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing route.");
    if (!Utils::isPath(word))
        throw std::runtime_error("invalid route.");
    route = word;
    getline(ss, word, ' ');
    if (word != "{")
        throw std::runtime_error("missing open bracket after route declaration.");
    locationSettingsStr = setting.substr(route.size() + 12);
    _routes[route] = extractLocationSettings(locationSettingsStr);
}

void ServerConfig::checkIfValidPath()
{
    if (!Utils::isFile(_root + _index))
        throw std::runtime_error("index is not a file.");
    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); it++) {
        if (!Utils::isFile(_root + it->second))
            throw std::runtime_error("error page " + it->second + "is not a file.");
    }
    for (std::map<std::string, RouteSettings>::const_iterator it = _routes.begin(); it != _routes.end(); it++) {
        if (!Utils::isFile(it->second.root + it->second.index))
            throw std::runtime_error("index " + it->second.index + " in loaction block " + it->first + " is not a file.");
        if (!Utils::isFile(it->second.root + it->second.cgi))
            throw std::runtime_error("CGI " + it->second.cgi + " in loaction block " + it->first + " is not a file.");
    }
}

std::ostream& operator<<(std::ostream& os, const ServerConfig& obj) {
    os << "Listening to port : ";
    os << obj._port << std::endl;
    os << "Server name(s) : ";
    for (size_t i(0); i < obj._serverName.size(); i++)
        os << obj._serverName[i] << " ";
    os << "\nError pages : " << std::endl;
    for (std::map<int, std::string>::const_iterator it = obj._errorPages.begin(); it != obj._errorPages.end(); it++) {
        os << it->first << "     " << it->second << std::endl;
    }
    os << "Max body size : " << obj._clientBodyLimit << std::endl;
    os << "Root : " << obj._root << std::endl;
    os << "Index : " << obj._index << std::endl;
    for (std::map<std::string, RouteSettings>::const_iterator it = obj._routes.begin(); it != obj._routes.end(); it++) {
        os << "Route : " << it->first << std::endl << "{" << std::endl;
        os << "Methods : ";
        for (size_t i(0); i < it->second.methods.size(); i++)
            os << it->second.methods[i] << " ";
        os << std::endl;
        os << "Root : " << it->second.root << std::endl;
        os << "Index : " << it->second.index << std::endl;
        os << "Dir listing : " << it->second.directoryListing << std::endl;
        os << "Redir : " << it->second.redirect << std::endl;
        os << "CGI : " << it->second.cgi << std::endl << "}" << std::endl; 
    }
    return os;
}