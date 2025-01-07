#include "../include/Poll.hpp"
#include "../include/Handle_client.hpp"
#include <vector>

#define MAX_CLIENTS 100
#include "../include/ConfigFile.hpp"

/**
 * @brief request client
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv) {
    std::string file = "./config/example.conf";
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
    server.closeSocket();
    return 0;
}

