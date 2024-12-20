/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:01 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/19 20:03:37 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string &configFile)
{
    std::vector<std::string> configVector;
    
    fileInVect(configFile, configVector);
    if (configFile.size() < 1)
        throw std::runtime_error("Empty config file.");
    splitServer(configVector);
    parseVect();
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::fileInVect(const std::string &configFile, std::vector<std::string> &configVector)
{
    std::string line;
    std::ifstream file(configFile.c_str());
    if (!file)
        throw std::runtime_error("Could not open the config file.");
    while (getline(file, line)) {
        line = cleanLine(line);
        if (!line.empty())
            configVector.push_back(line);
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

void ConfigFile::splitServer(std::vector<std::string> &configFile)
{
    std::vector<std::string> tempVect;
    std::istringstream line;
    std::string word;
    int brackets(0);
    
    line.str(configFile[0]);
    getline(line, word, ' ');
    while (word == "server") {
        
    }
    if (!configFile.empty())
        throw std::runtime_error("Wrong syntax: " + configFile[0]);
}

// void ConfigFile::parseVect()
// {
//     std::string keywords[7] = {"listen", "server_name", "error_page", "client_max_body_size", "root", "index", "location"};
//     std::string word;
//     std::istringstream line;
//     int index;
    
//     for (int i(0); i < _configVector.size(); i++) {
//         line.str(_configVector[i]);
//         std::getline(line, word, ' ');
//         index = 0;
//         for (; index < 8; index++) {
//             if (keywords[index] == word)
//                 break;
//         }
//         switch (index)
//         {
//         case 0:
//             /* code for port */
//             break;
//         case 1:
//             /* code for server_name */
//             break;
//         case 2:
//             /* code error_page */
//             break;
//         case 3:
//             /* code max size */
//             break;
//         case 4:
//             /* code root */
//             break;
//         case 5:
//             /* code index */
//             break;
//         case 6:
//             /* code location */
//             break;
        
//         default:
//             throw std::runtime_error(word + ": unknown keyword");
//             break;
//         }
//     }
// }
