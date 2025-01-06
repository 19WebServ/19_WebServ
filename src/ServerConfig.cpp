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
    while (getline(ss, word, ' ')) {
        if (areOnlyDigits(word)) {
            nb = atof(word.c_str());
            if (nb < 1 || nb > 65535)
                throw std::runtime_error(word + " : invalid port.");
            else
                _port.push_back(static_cast<int>(nb));
        }
        else
            throw std::runtime_error(word + " : invalid port.");
    }
}

// Check if the name is valid before pushing it to the _serveName vector.
void ServerConfig::extractServerName(std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    while (getline(ss, word, ' ')) {
        if (isValidServerName(word))
            _serverName.push_back(word);
        else
            throw std::runtime_error(word + " : invalid server name.");
    }
}

void ServerConfig::extractErrorPage(std::string setting)
{
    std::istringstream ss;
    std::string word;
    float error;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (!areOnlyDigits(word) || atof(word.c_str()) < 400 || atof(word.c_str()) > 599)
        throw std::runtime_error(word + " : invalid server name.");
    else {
        error = atof(word.c_str());
        getline(ss, word, ' ');
        if (!ss.eof())
            throw std::runtime_error("More than 1 error page.");
        // tester la page d'erreur
        _errorPages[error] = word;
    }
}

void ServerConfig::extractMaxBodySize(std::string setting)
{
    std::istringstream ss;
    std::string word;

    ss.str(setting);
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    if (areOnlyDigits(word))
        _clientBodyLimit = atof(word.c_str());
    else
        throw std::runtime_error(word + " : invalid maximum body size");
}

/* -----UTILS----- */

// Check if the string passed as parameter is constitued only by digits.
bool areOnlyDigits(std::string nb)
{
    for (int i(0); nb[i]; i++) {
        if (!isdigit(nb[i]))
            return false;
    }
    return true;
}

// Check if the name passed as parameter is valid or not (can only caintain letters, numbers or '-').
bool isValidServerName(std::string name)
{
    for (int i(0); name[i]; i++) {
        if ((name[i] < 'a' || name[i] > 'z') && !isdigit(name[i]) && name[i] != '-')
            return false;
    }
    // Checks en plus a faire : verifier aue le - n'est pas au debut ni a la fin et prendre en compte le www. et le .com
    return true;
}

std::ostream& operator<<(std::ostream& os, const ServerConfig& obj) {
    os << "Listening to ports : ";
    for (size_t i(0); i < obj._port.size(); i++)
        os << obj._port[i] << " ";
    os << "\nServer name(s) : ";
    for (size_t i(0); i < obj._serverName.size(); i++)
        os << obj._serverName[i] << " ";
    os << "\nError pages : " << std::endl;
    for (std::map<int, std::string>::const_iterator it = obj._errorPages.begin(); it != obj._errorPages.end(); it++) {
        os << it->first << "     " << it->second << std::endl;
    }
    os << "Max body size : " << obj._clientBodyLimit << std::endl;
    os << "Root : " << obj._root << std::endl;
    os << "Index : " << obj._index << std::endl;
    
    return os;
}