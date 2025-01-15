/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:04:14 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/15 18:03:43 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"

Request::Request() {
    _contentLen = 0;
    _content = "";
    _contentType = "";
}

Request::Request(std::string location, std::string method): _method(method), _location(location) {
    _contentLen = 0;
    _content = "";
    _contentType = "";
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

std::string Request::getMethod() {return this->_method;}

std::string Request::getLocation() {return this->_location;}

std::string Request::getContent() {return this->_content;}

size_t Request::getContentLen() {return this->_contentLen;}

std::string Request::getContentType() {return this->_contentType;}
