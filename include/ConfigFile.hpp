/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:04 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/27 12:26:36 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <iostream>
# include <vector>
# include <fstream>
# include <exception>
# include "./ServerConfig.hpp"

class ConfigFile
{
public:
    ConfigFile(const std::string &);
    ~ConfigFile();

    std::vector<ServerConfig> getServer();
    
    // friend std::ostream& operator<<(std::ostream& os, const ConfigFile& obj);
    
private:
    /* ---METHODS--- */
    void fileInStr(const std::string &, std::string &);
    std::string cleanLine(std::string);
    void splitStr(std::string &, std::vector<std::string> &);
    void parseServer(std::string);
    
    /* --- ATTRIBUTES ---*/
    std::vector<ServerConfig> _servers;
};


#endif
