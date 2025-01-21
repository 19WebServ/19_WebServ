/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 17:46:22 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/21 17:41:59 by vdecleir         ###   ########.fr       */
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

namespace Utils
{
    std::string trimLine(std::string &, char);
    bool areOnlyDigits(std::string);
    bool isDir(std::string);
    bool isFile(std::string);
    bool isPath(std::string);
    bool hasReadPermission(const char* filePath);
    bool hasExecutePermission(const char* filePath);
    bool hasWritePermission(const char* filePath);
    bool hasRootDirectoryAccess(const char* dirPath);
    std::string readFile(std::string filename);
    // std::string generateErrorPage(std::string);
    std::string intToStr(int);
    void saveFile(const std::string& filename, const std::string& fileData);
    size_t getTime();
}

#endif