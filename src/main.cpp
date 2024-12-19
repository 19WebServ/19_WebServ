#include <iostream>
#include "../include/Server.hpp"
#include "../include/ConfigFile.hpp"

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv) {
    std::string file = "default.conf";
    if (argc == 2) {
        file = argv[1];
    }

    // std::cout << "Starting Webserv with configuration file: " << file << std::endl;

    try {
        ConfigFile configFile(file);
        // Server server(file);
        // server.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}