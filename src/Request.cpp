/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:04:14 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/18 01:08:33 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"

Request::Request() {
    _contentLen = 0;
    _content = "";
    _contentType = "";
    _boundary = "";
}

Request::Request(std::string location, std::string method): _method(method), _location(location) {
    _contentLen = 0;
    _content = "";
    _contentType = "";
    _boundary = "";
}

Request::~Request() {}

void Request::setContent(std::string body)
{
    this->_content = body;
    this->_contentLen = body.size();
}

void Request::setContentType(std::string type)
{
    this->_contentType = type;
}

void Request::setBoundary(std::string boundary)
{
    this->_boundary = boundary;
}

std::string Request::getMethod() {return this->_method;}

std::string Request::getLocation() {return this->_location;}

std::string Request::getContent() {return this->_content;}

size_t Request::getContentLen() {return this->_contentLen;}

std::string Request::getBoundary() {return this->_boundary;}

std::string Request::getContentType() {return this->_contentType;}
