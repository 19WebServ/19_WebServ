#include <iostream>
#include "../include/Server.hpp"

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv) {
    std::string configFile = "default.conf";
    if (argc > 1) {
        configFile = argv[1];
    }

    std::cout << "Starting Webserv with configuration file: " << configFile << std::endl;

    try {
        Server server(configFile);
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}