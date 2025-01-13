/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 17:46:22 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/13 13:41:18 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_CPP
# define UTILS_CPP

# include <iostream>
# include <sys/stat.h>
# include <sstream>
# include <unistd.h>

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
}

#endif