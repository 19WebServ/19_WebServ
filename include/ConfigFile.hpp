/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 15:00:04 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/06 18:04:26 by vdecleir         ###   ########.fr       */
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
private:
    //-----ATTRIBUTES-----//
    std::vector<ServerConfig> _servers;

    //-----METHODS-----//
    void fileInStr(const std::string &, std::string &);
    std::string cleanLine(std::string);
    void splitStr(std::string &, std::vector<std::string> &);
    void parseServer(std::string);
    std::string trimLine(std::string &, char);
    
public:
    ConfigFile(const std::string &);
    ~ConfigFile();

    std::vector<ServerConfig> getServer();
    
    friend std::ostream& operator<<(std::ostream& os, const ConfigFile& obj);
};


#endif
