/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:01 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/18 18:02:36 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigFile.hpp"

ConfigFile::ConfigFile(const std::string &configFile)
{
    std::string line;
    std::string fileStr = "";
    std::ifstream file(configFile);
    if (!file)
        throw ProblemOccured("Config file could not open.");
    while (getline(file, line)) {
        line = skipWhitespaces(line);
        fileStr += line;
    }
}

ConfigFile::~ConfigFile()
{
}

std::string ConfigFile::skipWhitespaces(std::string line)
{
    int i = 0;
    while (isspace(line[i]))
        i++;
    return &line[i];
}
