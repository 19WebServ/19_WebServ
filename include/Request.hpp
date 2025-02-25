/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 14:46:03 by vdecleir          #+#    #+#             */
/*   Updated: 2025/02/25 19:08:50 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>

class Request
{
public:
    Request();
    Request(std::string, std::string, std::string);
    ~Request();

    /* ---GETTERS--- */
    std::string getMethod();
    std::string getLocation();
    std::string getPath();
    std::string getContent();
    std::string getBoundary();
    size_t      getContentLen();
    bool        getIfComplete();
    std::string getQuery() const;

    /* --- SETTERS--- */
    void setLocation(std::string);
    void setPath(std::string);
    void setMethos(std::string);
    void setContent(std::string);
    void setBoundary(std::string);
    void setContentLen(int);
    void setComplete(bool);
    
private:
    /* ---ATTRIBUTES--- */
    std::string _method;
    std::string _location;
    std::string _path;
    std::string _header;
    std::string _content;
    size_t      _contentLen;
    std::string _boundary;
    bool        _complete;
};

#endif