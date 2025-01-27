/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:04:14 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/27 09:57:32 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"

Request::Request() {
    _contentLen = 0;
}

Request::Request(std::string location, std::string path, std::string method): _method(method), _location(location), _path(path) {
    _contentLen = 0;
}

Request::~Request() {}


/* ---GETTERS--- */

std::string Request::getMethod() {return this->_method;}

std::string Request::getLocation() {return this->_location;}

std::string Request::getPath() {return this->_path;}

std::string Request::getContent() {return this->_content;}

size_t Request::getContentLen() {return this->_contentLen;}

std::string Request::getBoundary() {return this->_boundary;}


/* --- SETTERS--- */

void Request::setContent(std::string body)
{
    this->_content = body;
    this->_contentLen = body.size();
}

void Request::setBoundary(std::string boundary){ this->_boundary = boundary;}


