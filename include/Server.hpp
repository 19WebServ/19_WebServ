/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhouyet <jhouyet@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 08:00:25 by jhouyet           #+#    #+#             */
/*   Updated: 2024/12/11 08:06:28 by jhouyet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

class Server {
public:
    Server(const std::string &configFile);
    void run();
private:
    std::string _configFile;
};

#endif // SERVER_HPP