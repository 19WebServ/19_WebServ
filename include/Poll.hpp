#pragma once
#include "Socket.hpp"

void    new_client_connection(Socket &server);
int    disconnection_check(Socket &server, int i);