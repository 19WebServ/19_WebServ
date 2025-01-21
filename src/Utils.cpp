/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 17:44:04 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/21 17:42:04 by vdecleir         ###   ########.fr       */
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

std::string Utils::readFile(std::string filename) 
{
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
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

std::string Utils::intToStr(int len)
{
    std::ostringstream oss;
    oss << len;
    return oss.str();
}

// std::string Utils::generateErrorPage(std::string error)
// {
//     std::string buffer;
//     buffer =    "<!DOCTYPE html>\n"
//                 "<html>\n"
//                 "<head>\n"
//                 "    <title>" + error + "</title>\n"
//                 "</head>\n"
//                 "<body>\n"
//                 "    <p><b>" + error + "</b></p>\n"
//                 "</body>\n"
//                 "</html>\n";
//     return buffer;
// }

void Utils::saveFile(const std::string& filename, const std::string& fileData) {
    std::ofstream outFile(("./document/" + filename).c_str(), std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing.");
    }
    outFile.write(fileData.c_str(), fileData.size());
    outFile.close();
    std::cout << "File saved: " << filename << std::endl;
}

size_t Utils::getTime()
{
    std::time_t currentTime = std::time(NULL);
    size_t res = 0;
    if (currentTime != static_cast<std::time_t>(-1))
    {
        res = static_cast<size_t>(currentTime);
    }
    return res;
}