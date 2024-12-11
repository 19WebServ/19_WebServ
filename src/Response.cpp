/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhouyet <jhouyet@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 08:00:17 by jhouyet           #+#    #+#             */
/*   Updated: 2024/12/11 08:26:22 by jhouyet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"
#include <iostream>

/**
 * @brief Construct a new Response:: Response object
 * 
 */
Response::Response() {
    std::cout << "Response handler initialized." << std::endl;
}

/**
 * @brief Generate a response
 * 
 * @return std::string 
 */
std::string Response::generate() {
    std::cout << "Generating response..." << std::endl;
    return "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello, World!";
}