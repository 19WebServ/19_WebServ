/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 17:44:04 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/09 19:08:57 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"

// Returns the one setting line and delete it from the server string.
std::string Utils::trimLine(std::string &serverStr, char delim)
{
    std::string settings;
    std::istringstream ss;
    
    ss.str(serverStr);
    getline(ss, settings, delim);
    serverStr = serverStr.substr(settings.size() + 2);
    return settings;
}

// Check if the string passed as parameter is constitued only by digits.
bool Utils::areOnlyDigits(std::string nb)
{
    for (int i(0); nb[i]; i++) {
        if (!isdigit(nb[i]))
            return false;
    }
    return true;
}

bool Utils::isDir(std::string path)
{
    struct stat sb;
 
    if (stat(path.c_str(), &sb) == 0)
        return true;
    else
        return false;
}

bool Utils::isFile(std::string path)
{
    struct stat sb;
 
    if (stat(path.c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR))
        return true;
    else
        return false;
}

bool Utils::isPath(std::string path)
{
    if (path.size() > 0) {
        if (path[0] == '/')
            return true;
    }
    return false;
}