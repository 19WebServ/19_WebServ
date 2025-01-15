/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 17:44:04 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/15 18:21:47 by vdecleir         ###   ########.fr       */
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

// Check read permission for a file
bool Utils::hasReadPermission(const char* filePath)
{
    if (access(filePath, R_OK) == 0)
        return true;
    else
        return false;
}

// Check execute permission for a file
bool Utils::hasExecutePermission(const char* filePath)
{
    if (access(filePath, X_OK) == 0)
        return true;
    else
        return false;
}

// Check write permission for a file
bool Utils::hasWritePermission(const char* filePath)
{
    if (access(filePath, W_OK) == 0)
        return true;
    else
        return false;
}

// Check necessary permissions (read + execute) for the root directory
bool Utils::hasRootDirectoryAccess(const char* dirPath)
{
    if (access(dirPath, R_OK | X_OK) == 0)
        return true;
    else
        return false;
}

std::string Utils::readFile(char *filename) 
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return "";

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, ' ');
    if (!file.read(&buffer[0], size))
        return "";
    file.close();
    return buffer;
}