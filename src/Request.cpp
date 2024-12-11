/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhouyet <jhouyet@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 08:00:05 by jhouyet           #+#    #+#             */
/*   Updated: 2024/12/11 08:41:14 by jhouyet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"
#include <iostream>

/**
 * @brief Construct a new Request:: Request object
 * 
 */
Request::Request() {
    std::cout << "Request handler initialized." << std::endl;
}

/**
 * @brief Parse a request
 * 
 * @param rawRequest 
 */
void Request::parse(const std::string &rawRequest) {
    std::cout << "Parsing request: " << rawRequest << std::endl;
    // To be implemented: Parse method, path, and HTTP version
}