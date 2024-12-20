/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:01 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/20 16:58:49 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string &configFile)
{
    std::string configStr = "";
    
    fileInStr(configFile, configStr);
    while (!configStr.empty())
        SplitStr(configStr);
    for (size_t i(0); i < _serversConfigStr.size(); i++)
        std::cout << _serversConfigStr[i] << "\n" << std::endl;
    parseVect();
}

ConfigFile::~ConfigFile()
{
}

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

void ConfigFile::SplitStr(std::string &configStr)
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

void ConfigFile::parseVect()
{
    std::string keywords[7] = {"listen", "server_name", "error_page", "client_max_body_size", "root", "index", "location"};
    std::string word;
    std::istringstream line;
    int index;
    
    line.str(_serversConfigStr[0]);
    for (size_t i(0); i < _serversConfigStr.size(); i++) {
        while (getline(line, word, ' ')) {
            index = 0;
            for (; index < 8; index++) {
                if (keywords[index] == word)
                    break;
            }
            switch (index)
            {
            case 0:
                /* code for port */
                break;
            case 1:
                /* code for server_name */
                break;
            case 2:
                /* code error_page */
                break;
            case 3:
                /* code max size */
                break;
            case 4:
                /* code root */
                break;
            case 5:
                /* code index */
                break;
            case 6:
                /* code location */
                break;
            
            default:
                throw std::runtime_error(word + ": unknown keyword");
                break;
            line.str(_serversConfigStr[i]);
            }  
        }
    }
}
