/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 14:46:03 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/18 01:08:02 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>

class Request
{
public:
    Request();
    Request(std::string, std::string);
    ~Request();

    void setContent(std::string);
    void setContentType(std::string);
    void setBoundary(std::string);
    std::string getMethod();
    std::string getLocation();
    std::string getContent();
    std::string getBoundary();
    size_t      getContentLen();
    std::string getContentType();
    
private:
    std::string _method;
    std::string _location;
    std::string _content;
    size_t      _contentLen;
    std::string _contentType;
    std::string _boundary;
};

#endif