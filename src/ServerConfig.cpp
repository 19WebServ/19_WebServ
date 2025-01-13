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
    _port = 8080;
    _clientBodyLimit = 1000;
    _root = "";
    _index = "";
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
    else if (!Utils::hasRootDirectoryAccess(word.c_str()))
        throw std::runtime_error("Root directory does not have necessary access.");
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

void ServerConfig::checkMissigValues()
{
    if (_serverName.empty())
        throw std::runtime_error("missing a server name.");
    if (_root.empty())
        throw std::runtime_error("missing a root directory.");
    if (_index.empty())
        throw std::runtime_error("missing index page.");
    for (std::map<std::string, RouteSettings>::iterator it = _routes.begin(); it != _routes.end(); it++) {
        if (it->second.root.empty())
            it->second.root = _root;
        if (it->second.index.empty())
            throw std::runtime_error("missing index page in " + it->first + " location block.");
    }
}

// Check if all the paths are valid and if we have the right permissions
void ServerConfig::checkIfValidPath()
{
    if (!Utils::isFile(_root + _index))
        throw std::runtime_error("index " + _root + _index + " is not a file.");
    else if (!Utils::hasReadPermission((_root + _index).c_str()))
        throw std::runtime_error("index " + _root + _index + " file doesn't have read permission.");
    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); it++) {
        if (!Utils::isFile(_root + it->second))
            throw std::runtime_error("error page " + it->second + "is not a file.");
        else if (!Utils::hasReadPermission((_root + it->second).c_str()))
            throw std::runtime_error("error page " + it->second + "doesn't have read permission.");
    }
    for (std::map<std::string, RouteSettings>::const_iterator it = _routes.begin(); it != _routes.end(); it++) {
        if (!Utils::isFile(it->second.root + it->second.index))
            throw std::runtime_error("index " + it->second.root + it->second.index + " in loaction block " + it->first + " is not a file.");
        else if (!Utils::hasReadPermission((it->second.root + it->second.index).c_str()))
            throw std::runtime_error("index " + it->second.root + it->second.index + " in loaction block " + it->first + " doesn't have read permission.");
        if (!it->second.cgi.empty() && !Utils::isFile(it->second.root + it->second.cgi))
            throw std::runtime_error("CGI " + it->second.root + it->second.cgi + " in loaction block " + it->first + " is not a file.");
        else if (!it->second.cgi.empty() && !Utils::hasReadPermission((it->second.root + it->second.cgi).c_str()))
            throw std::runtime_error("CGI " + it->second.root + it->second.cgi + " in loaction block " + it->first + " doesn't have read permission.");
        else if (!it->second.cgi.empty() && !Utils::hasExecutePermission((it->second.root + it->second.cgi).c_str()))
            throw std::runtime_error("CGI " + it->second.root + it->second.cgi + " in loaction block " + it->first + " doesn't have execute permission.");
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



// ------------------------GETTERS----------------------- //

int ServerConfig::getPort()
{
    return this->_port;
}

size_t ServerConfig::getBodySize()
{
    return this->_clientBodyLimit;
}

std::vector<std::string> ServerConfig::getServerNames()
{
    return _serverName;
}

std::string ServerConfig::getErrorPage(int errorNo)
{
    std::map<int,std::string>::iterator it = _errorPages.find(errorNo);;

    if (it != _errorPages.end())
        return it->second;
    else
        return "";
}

std::string ServerConfig::getRoot()
{
    return _root;
}

std::string ServerConfig::getIndex()
{
    return _index;
}

bool ServerConfig::getLocationAllowedMethods(std::string location, std::string method)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end()) {
        std::vector<std::string>::iterator it2 = std::find(it->second.methods.begin(), it->second.methods.end(), method);
        if (it2 != it->second.methods.end())
            return true;
        else
            return false;
    }
    else
        return false;
}

std::string ServerConfig::getLocationRoot(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.root;
    else
        return "";
}

bool ServerConfig::getLocationDirectoryListing(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.directoryListing;
    else
        return "";
}

std::string ServerConfig::getLocationIndex(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.index;
    else
        return "";
}

std::string ServerConfig::getLocationRedirect(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.redirect;
    else
        return "";
}

std::string ServerConfig::getLocationCgi(std::string location)
{
    std::map<std::string, RouteSettings>::iterator it = _routes.find(location);
    if (it != _routes.end())
        return it->second.cgi;
    else
        return "";
}
