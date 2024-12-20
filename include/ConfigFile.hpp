/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:04 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/20 15:20:24 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include <exception>
// # include "./ServerConfig.hpp"

class ConfigFile
{
private:
    //-----ATTRIBUTES-----//
    // std::vector<ServerConfig> _servers;
    std::vector<std::string> _serversConfigStr;

    //-----METHODS-----//
    void fileInStr(const std::string &, std::string &);
    std::string cleanLine(std::string);
    void SplitStr(std::string &);
    void parseVect();
    
public:
    ConfigFile(const std::string &);
    ~ConfigFile();
};




#endif