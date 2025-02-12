/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 17:46:22 by vdecleir          #+#    #+#             */
/*   Updated: 2025/02/08 19:09:31 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_CPP
# define UTILS_CPP

# include <iostream>
# include <sys/stat.h>
# include <sstream>
# include <unistd.h>
# include <fstream>
# include <vector>
# include <map>
# include <ctime>
# include "./ServerConfig.hpp"

class ServerConfig;

namespace Utils
{
    std::string trimLine(std::string &, char);
    bool areOnlyDigits(std::string);
    bool isDir(std::string);
    bool isFile(std::string);
    bool isPath(std::string);
    bool hasReadPermission(const char*);
    bool hasExecutePermission(const char*);
    bool hasWritePermission(const char*);
    bool hasRootDirectoryAccess(const char*);
    std::string readFile(std::string);
    // std::string generateErrorPage(std::string);
    std::string intToStr(int);
    bool samePorts(std::vector<ServerConfig>);
    void saveFile(const std::string&, const std::string&);
    size_t getTime();
    std::string findType(std::string);
    bool isDeletable(std::string, std::string);
}

#endif