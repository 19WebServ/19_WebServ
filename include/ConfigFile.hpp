/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:04 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/18 17:54:09 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <iostream>
# include <vector>
# include <fstream>
# include "./ServerConfig.hpp"

class ProblemOccured : public std::exception {
private:
    std::string message;

public:
    explicit ProblemOccured(const std::string& error) : message(error) {}
    virtual const char* what() const throw() {
        return message.c_str();
    }
};


class ConfigFile
{
private:
    std::vector<ServerConfig> _servers;
    
    std::string skipWhitespaces(std::string);
    
public:
    ConfigFile(const std::string &);
    ~ConfigFile();
};




#endif