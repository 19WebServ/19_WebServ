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
    _host = "loaclhost";
    _clientBodyLimit = 1000;
}

ServerConfig::ServerConfig(const ServerConfig &copy)
{
    this->_port = copy._port;
    this->_host = copy._host;
    this->_serverName = copy._serverName;
    this->_errorPages = copy._errorPages;
    this->_clientBodyLimit = copy._clientBodyLimit;
    this->_root = copy._root;
    this->_index = copy._index;
    this->_routes = copy._routes;
}

ServerConfig::~ServerConfig() {}

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
    if (!ss.eof())
        throw std::runtime_error("More than 1 port on the line.");
    if (Utils::areOnlyDigits(word)) {
        nb = atof(word.c_str());
        if (word.size() > 5 || nb < 1024 || nb > 65535)
            throw std::runtime_error(word + " : invalid port.");
        else
            _port.push_back(static_cast<int>(nb));
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
    if (Utils::areOnlyDigits(word)) {
        if (word.size() < 10 && atof(word.c_str()) <= 2000000000)
            _clientBodyLimit = atof(word.c_str());
        else if (atof(word.c_str()) < 30)
            _clientBodyLimit = 30;
        else
            _clientBodyLimit = 2000000000;
    }
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
    else if (!Utils::hasRootDirectoryAccess(word.c_str()))
        throw std::runtime_error("Root directory does not have necessary access.");
    if (word[0] == '/')
        word = word.erase(0, 1);
    if (word[word.size() - 1] == '/')
        _root = word.substr(0, word.size() - 1);
    else
        _root = word;
}

void    ServerConfig::extractHost(std::string setting)
{
    std::istringstream ss;
    std::string word;
    std::string ip;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (word.empty())
        throw std::runtime_error("missing host.");
    if (!ss.eof())
        throw std::runtime_error("More than 1 host.");
    else if (word == "localhost")
        _host = word;
    else {
        ip = word;
        ss.clear();
        ss.str(ip);
        for (int i(0); i < 4; i++) {
            getline(ss, word, '.');
            if (!Utils::areOnlyDigits(word) || word.size() > 3 || word.size() < 1 || atoi(word.c_str()) < 0 || atoi(word.c_str()) > 255)
                throw std::runtime_error("Invalid host");
        }
        if (!ss.eof())
            throw std::runtime_error("Invalid host");
        _host = ip;
    }
}

// void ServerConfig::extractIndex(std::string setting)
// {
//     std::istringstream ss;
//     std::string word;

//     ss.str(setting);
//     getline(ss, word, ' ');
//     getline(ss, word, ' ');
//     if (word.empty())
//         throw std::runtime_error("missing index page.");
//     if (!ss.eof())
//         throw std::runtime_error("More than 1 error page.");
//     if (Utils::isPath(word))
//         _index = word;
//     else
//         _index = "/" + word;
// }

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
    size_t i = 12;
    if (route[0] == '/') {
        route = route.erase(0, 1);
        i++;
    }
    getline(ss, word, ' ');
    if (word != "{")
        throw std::runtime_error("missing open bracket after route declaration.");
    locationSettingsStr = setting.substr(route.size() + i);
    _routes[route] = extractLocationSettings(locationSettingsStr);
}

void ServerConfig::checkMissigValues()
{
    if (_port.empty())
        throw std::runtime_error("missing port.");
    // if (_serverName.empty())
    //     throw std::runtime_error("missing a server name.");
    if (_root.empty())
        throw std::runtime_error("missing a root directory.");
    for (std::map<std::string, RouteSettings>::iterator it = _routes.begin(); it != _routes.end(); it++) {
        if (it->second.root.empty())
            it->second.root = _root;
    }
}

// Check if all the paths are valid and if we have the right permissions
void ServerConfig::checkIfValidPath()
{
    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); it++) {
        if (!Utils::isFile(_root + it->second))
            throw std::runtime_error("error page " + it->second + "is not a file.");
        else if (!Utils::hasReadPermission((_root + it->second).c_str()))
            throw std::runtime_error("error page " + it->second + "doesn't have read permission.");
    }
    for (std::map<std::string, RouteSettings>::const_iterator it = _routes.begin(); it != _routes.end(); it++) {
        if (!it->second.cgi.empty() && !Utils::isFile(it->second.root + it->second.cgi))
            throw std::runtime_error("CGI " + it->second.root + it->second.cgi + " in loaction block " + it->first + " is not a file.");
        else if (!it->second.cgi.empty() && !Utils::hasReadPermission((it->second.root + it->second.cgi).c_str()))
            throw std::runtime_error("CGI " + it->second.root + it->second.cgi + " in loaction block " + it->first + " doesn't have read permission.");
        else if (!it->second.cgi.empty() && !Utils::hasExecutePermission((it->second.root + it->second.cgi).c_str()))
            throw std::runtime_error("CGI " + it->second.root + it->second.cgi + " in loaction block " + it->first + " doesn't have execute permission.");
    }
}

// std::ostream& operator<<(std::ostream& os, const ServerConfig& obj) {
//     os << "Listening to port : ";
//     os << obj._port << std::endl;
//     os << "Server name(s) : ";
//     for (size_t i(0); i < obj._serverName.size(); i++)
//         os << obj._serverName[i] << " ";
//     os << "\nError pages : " << std::endl;
//     for (std::map<int, std::string>::const_iterator it = obj._errorPages.begin(); it != obj._errorPages.end(); it++) {
//         os << it->first << "     " << it->second << std::endl;
//     }
//     os << "Max body size : " << obj._clientBodyLimit << std::endl;
//     os << "Root : " << obj._root << std::endl;
//     os << "Index : " << obj._index << std::endl;
//     for (std::map<std::string, RouteSettings>::const_iterator it = obj._routes.begin(); it != obj._routes.end(); it++) {
//         os << "Route : " << it->first << std::endl << "{" << std::endl;
//         os << "Methods : ";
//         for (size_t i(0); i < it->second.methods.size(); i++)
//             os << it->second.methods[i] << " ";
//         os << std::endl;
//         os << "Root : " << it->second.root << std::endl;
//         os << "Index : " << it->second.index << std::endl;
//         os << "Dir listing : " << it->second.directoryListing << std::endl;
//         os << "Redir : " << it->second.redirect.begin()->second << std::endl;
//         os << "CGI : " << it->second.cgi << std::endl << "}" << std::endl; 
//     }
//     return os;
// }


/* ---GETTERS--- */

std::vector<int> ServerConfig::getPort() {return this->_port;}

size_t ServerConfig::getBodySize() {return this->_clientBodyLimit;}

std::vector<std::string> ServerConfig::getServerNames() {return _serverName;}

std::string ServerConfig::getErrorPage(int errorNo)
{
    std::map<int,std::string>::iterator it = _errorPages.find(errorNo);;

    if (it != _errorPages.end())
        return it->second;
    else
        return "";
}

std::string ServerConfig::getRoot() {return _root;}

std::string ServerConfig::getHost() {return _host;}

std::string ServerConfig::getIndex() {return _index;}

std::vector<std::string> ServerConfig::getLocationAllowedMethods(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.methods;
    else {
        std::vector<std::string> method;
        method.push_back("GET");
        return method;
    }
}

// bool ServerConfig::isAllowed(std::string location, std::string method)
// {
//     std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
//     if (it != _routes.end()) {
//         std::vector<std::string>::iterator it2 = std::find(it->second.methods.begin(), it->second.methods.end(), method);
//         if (it2 != it->second.methods.end())
//             return true;
//         else
//             return false;
//     }
//     else
//         throw std::runtime_error("404 Not Found");
// }

std::string ServerConfig::getLocationRoot(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.root;
    else {
        if (location[0] == '/')
            location = location.substr(1);
        return location;
    }
}

bool ServerConfig::getLocationDirectoryListing(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.directoryListing;
    else
        return false;;
}

std::string ServerConfig::getLocationIndex(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.index;
    else
        return "";
}

std::map<std::string, std::string> ServerConfig::getLocationRedirect(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.redirect;
    else {
        std::map<std::string, std::string> emptyMap;
        return emptyMap;
    }
}

std::string ServerConfig::getLocationCgi(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.cgi;
    else
        throw std::runtime_error("404 Not Found");
}
