/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:01 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/06 12:10:26 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string &configFile)
{
    std::string configStr = "";
    
    fileInStr(configFile, configStr);
    while (!configStr.empty())
        splitStr(configStr);
    for (size_t i(0); i < _serversConfigStr.size(); i++)
        parseServer(_serversConfigStr[i]);
    
}

ConfigFile::~ConfigFile()
{
}

// Read all the file and put it in one string.
void ConfigFile::fileInStr(const std::string &configFile, std::string &configStr)
{
    std::string line;
    std::ifstream file(configFile.c_str());
    if (!file)
        throw std::runtime_error("Could not open the config file.");
    while (getline(file, line)) {
        line = cleanLine(line);
        if (!line.empty())
            configStr += (line + " ");
    }
    file.close();
}

// Get rid of the whitespaces and the comments.
std::string ConfigFile::cleanLine(std::string line)
{
    size_t beg = 0;
    while (isspace(line[beg]))
        beg++;
    size_t end = line.find('#');
    if (end == std::string::npos)
        end = line.size();
    while (isspace(line[end - 1]))
        end--;
    if (end <= beg)
        return "";
    return line.substr(beg, end - beg);
}

// Split the string by server and check for brackets, after this we are left with one vector of strings, each string is all the settings for one server.
void ConfigFile::splitStr(std::string &configStr)
{
    std::istringstream line;
    std::string word;
    std::string tempStr = "";
    int brackets(0);

    line.str(configStr);
    getline(line, word, ' ');
    if (word != "server")
        throw std::runtime_error("Config file");
    getline(line, word, ' ');
    if (word != "{")
        throw std::runtime_error("Must open brackets after declaring the server.");
    else
        brackets++;
    while (getline(line, word, ' ')) {
        if (word == "{")
            brackets++;
        else if (word == "}")
            brackets--;
        if (brackets == 0)
            break;
        tempStr += (word + " ");
    }
    if (word != "}")
        throw std::runtime_error("Missing bracket(s)");
    else {
        configStr = configStr.substr(tempStr.size() + static_cast<std::string>("server ").size() + 4);
        _serversConfigStr.push_back(tempStr);
    }
}

// Check keywords for each line and send it to the appropriate function to exctract the relevant infos.
void ConfigFile::parseServer(std::string serverStr)
{
    std::string keywords[7] = {"listen", "server_name", "error_page", "client_max_body_size", "root", "index", "location"};
    std::string word;
    std::string settings;
    std::istringstream line;
    size_t index;
    ServerConfig server;
    
    line.str(serverStr);
    while (getline(line, word, ' ')) {
        index = 0;
        for (; index < sizeof(keywords) / sizeof(keywords[0]); index++) {
            if (keywords[index] == word)
                break;
        }
        switch (index)
        {
        case 0:
            /* ports */
            settings = trimLine(serverStr, ';');
            server.extractPort(settings);
            break;
        case 1:
            /* server_name */
            settings = trimLine(serverStr, ';');
            server.extractServerName(settings);
            break;
        case 2:
            /* error_page */
            settings = trimLine(serverStr, ';');
            server.extractErrorPage(settings);
            break;
        case 3:
            /* max size */
            settings = trimLine(serverStr, ';');
            server.extractMaxBodySize(settings);
            break;
        case 4:
            /* root */
            settings = trimLine(serverStr, ';');
            server.extractRooT(settings);
            break;
        case 5:
            /* index */
            settings = trimLine(serverStr, ';');
            server.extractIndex(settings);
            break;
        case 6:
            /* routes */
            settings = trimLine(serverStr, '}');
            server.extractLacoation(settings);
            break;
        
        default:
            throw std::runtime_error(word + ": unknown keyword");
            break;
        }
        std::cout << settings << std::endl;
        line.str(serverStr);
    }
    _servers.push_back(server);
}


// Returns the one setting line and delete it from the server string.
std::string ConfigFile::trimLine(std::string &serverStr, char delim)
{
    std::string settings;
    std::istringstream ss;
    
    ss.str(serverStr);
    getline(ss, settings, delim);
    serverStr = serverStr.substr(settings.size() + 2);
    return settings;
}

std::ostream& operator<<(std::ostream& os, const ConfigFile& obj) {
    for (size_t i(0); i < obj._servers.size(); i++) {
        os << "----- SERRVER " << i << "-----\n" << std::endl;
        os << obj._servers[i] << "\n" << std::endl;
    }
    return os;
}