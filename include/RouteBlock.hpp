/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteBlock.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdecleir <vdecleir@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:47:45 by vdecleir          #+#    #+#             */
/*   Updated: 2025/01/21 16:49:25 by vdecleir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTEBLOCK_HPP
# define ROUTEBLOCK_HPP

# include <iostream>
# include <map>

struct RouteSettings {
    std::vector<std::string> methods;
    std::string root;
    bool directoryListing;
    std::string index;
    std::map<std::string, std::string> redirect;
    std::string cgi;
};

RouteSettings extractLocationSettings(std::string);
void extractRouteRoot(RouteSettings &, std::string);
void extractRouteIndex(RouteSettings &, std::string);
void extractRouteAutoindex(RouteSettings &, std::string);
void extractRouteMethod(RouteSettings &, std::string);
void extractRouteCgi(RouteSettings &, std::string);
void extractRouteRedir(RouteSettings &, std::string);
void initRouteBlock(RouteSettings &routeBlock);

#endif