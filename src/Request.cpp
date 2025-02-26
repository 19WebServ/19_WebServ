/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:04:14 by vdecleir          #+#    #+#             */
/*   Updated: 2025/02/26 11:48:56 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"

Request::Request() {
    _contentLen = 0;
    _complete = true;
}

Request::Request(std::string location, std::string path, std::string method): _method(method), _location(location), _path(path) {
    _contentLen = 0;
    _complete = true;
}

Request::~Request() {}

Request &Request::operator=(const Request &copy)
{
    if (this != &copy) {
        _method = copy._method;
        _location = copy._location;
        _path = copy._path;
        _content = copy._content;
        _contentLen = copy._contentLen;
        _boundary = copy._boundary;
        _complete = copy._complete;
    }
    return *this;
}

/* ---GETTERS--- */

std::string Request::getMethod() {return this->_method;}

std::string Request::getLocation() {return this->_location;}

std::string Request::getPath() {return this->_path;}

std::string Request::getContent() {return this->_content;}

size_t Request::getContentLen() {return this->_contentLen;}

std::string Request::getBoundary() {return this->_boundary;}

bool Request::getIfComplete() {return this->_complete;}

std::string Request::getQuery() const {
    size_t pos = _path.find('?');
    if (pos != std::string::npos)
        return _path.substr(pos + 1);
    return "";
}


/* --- SETTERS--- */

void Request::setContent(std::string body) {this->_content = body;}

void Request::setContentLen(int len) {this->_contentLen = len;}

void Request::setBoundary(std::string boundary) {this->_boundary = boundary;}

void Request::setComplete(bool complete) {this->_complete = complete;}
