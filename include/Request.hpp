/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhouyet <jhouyet@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 08:00:33 by jhouyet           #+#    #+#             */
/*   Updated: 2024/12/11 08:02:54 by jhouyet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

class Request {
public:
    Request();
    void parse(const std::string &rawRequest);
private:
    std::string _method;
    std::string _path;
    std::string _version;
};

#endif // REQUEST_HPP