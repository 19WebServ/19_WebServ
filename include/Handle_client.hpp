#pragma once
#include "Socket.hpp"
#include <fstream>



int processing_request(char *buffer, int bytes_receiv, Socket &server, int client);