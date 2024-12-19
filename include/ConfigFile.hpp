/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:04 by vdecleir          #+#    #+#             */
/*   Updated: 2024/12/19 18:59:05 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include <exception>
# include "./ServerConfig.hpp"

class ConfigFile
{
private:
    //-----ATTRIBUTES-----//
    std::vector<ServerConfig> _servers;
    std::vector<std::vector<std::string> > _serversConfigVector;

    //-----METHODS-----//
    void fileInVect(const std::string &, std::vector<std::string> &);
    std::string cleanLine(std::string);
    void splitServer(std::vector<std::string> &);
    void parseVect();
    
public:
    ConfigFile(const std::string &);
    ~ConfigFile();
};




#endif