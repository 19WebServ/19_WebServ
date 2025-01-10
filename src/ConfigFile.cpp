/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:01 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/10 19:01:37 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string &configFile)
{
    std::string configStr = "";
    std::vector<std::string> blockStrs;
    
    fileInStr(configFile, configStr);
    while (!configStr.empty())
        splitStr(configStr, blockStrs);
    for (size_t i(0); i < blockStrs.size(); i++)
        parseServer(blockStrs[i]);
    
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
        if (!line.empty()) {
            std::istringstream ss;
            std::string word;
            ss.str(line);
            getline(ss, word, ' ');
            if ((word == "location" || word == "server") && line[line.size() - 1] != '{')
                throw std::runtime_error(line + " : missing open bracket.");
            else if (line.size() == 1 && line[0] == '}')
                ;
            else if (line[line.size() - 1] != ';' && (word != "location" && word != "server"))
                throw std::runtime_error(line + " : missing semicolon.");
            configStr += (line + " ");
        }
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
    while (end >= 1 && isspace(line[end - 1]))
        end--;
    if (end <= beg)
        return "";
    return line.substr(beg, end - beg);
}

// Split the string by server and check for brackets, after this we are left with one vector of strings, each string is all the settings for one server.
void ConfigFile::splitStr(std::string &configStr, std::vector<std::string> &blockStrs)
{
    std::istringstream line;
    std::string word;
    std::string tempStr = "";
    int brackets(0);

    line.str(configStr);
    getline(line, word, ' ');
    if (word != "server")
        throw std::runtime_error("Missing 'server' keyword or problem with brackets.");
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
        blockStrs.push_back(tempStr);
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
            settings = Utils::trimLine(serverStr, ';');
            server.extractPort(settings);
            break;
        case 1:
            /* server_name */
            settings = Utils::trimLine(serverStr, ';');
            server.extractServerName(settings);
            break;
        case 2:
            /* error_page */
            settings = Utils::trimLine(serverStr, ';');
            server.extractErrorPage(settings);
            break;
        case 3:
            /* max size */
            settings = Utils::trimLine(serverStr, ';');
            server.extractMaxBodySize(settings);
            break;
        case 4:
            /* root */
            settings = Utils::trimLine(serverStr, ';');
            server.extractRoot(settings);
            break;
        case 5:
            /* index */
            settings = Utils::trimLine(serverStr, ';');
            server.extractIndex(settings);
            break;
        case 6:
            /* routes */
            settings = Utils::trimLine(serverStr, '}');
            server.extractLocation(settings);
            break;
        
        default:
            throw std::runtime_error(word + ": unknown keyword");
            break;
        }
        // std::cout << settings << std::endl;
        line.str(serverStr);
    }
    server.checkIfValidPath();
    // server.fillMissigValues();
    _servers.push_back(server);
}

std::ostream& operator<<(std::ostream& os, const ConfigFile& obj) {
    for (size_t i(0); i < obj._servers.size(); i++) {
        os << "----- SERVER " << i << "-----\n" << std::endl;
        os << obj._servers[i] << "\n" << std::endl;
    }
    return os;
}