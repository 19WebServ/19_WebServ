#include <iostream>
#include "../include/ConfigFile.hpp"
#include "../include/Socket.hpp"

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
// int main(int argc, char **argv) {
//     std::string file = "./config/example.conf";
//     if (argc == 2) {
//         file = argv[1];
//     }

//     // std::cout << "Starting Webserv with configuration file: " << file << std::endl;

//     try {
//         ConfigFile configFile(file);
//         // Server server(file);
//         // server.run();
//         std::vector<ServerConfig> server = configFile.getServer();
//         std::vector<int>ports;
//         for (size_t i = 0; i < server.size(); i++)
//         {
//             ports
//         }
//         std::cout<< "\n\n TOUT EST GOOD \n" << std::endl;
//     } catch (const std::exception &e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }

int main(int argc, char **argv) {
    std::string file = "./config/example.conf";

    if (argc > 2) {
        std::cerr << "Too many arguments for program launch." << std::endl;
        return 1;
    }
    else if (argc == 2)
        file = argv[1];
    try {
        // Charger le fichier de configuration
        ConfigFile configFile(file);
        std::vector<ServerConfig> servers = configFile.getServer();
        // Récupérer tous les ports
        std::vector<int> allPorts;
        for (size_t i = 0; i < servers.size(); i++) {
            for (size_t j = 0; j < servers[i].getPort().size(); j++) {
                int serverPort = servers[i].getPort()[j];
                if (std::find(allPorts.begin(), allPorts.end(), serverPort) == allPorts.end())
                    allPorts.push_back(serverPort);
                else
                    throw std::runtime_error("Same port for multiple servers.");
            }
        }

        // Afficher les ports récupérés pour vérification
        std::cout << "Ports configurés : ";
        for (size_t i = 0; i < allPorts.size(); i++) {
            std::cout << allPorts[i] << (i < allPorts.size() - 1 ? ", " : "\n");
        }

        // Lancer les serveurs avec la classe Socket
        Socket socketServer(allPorts, servers);
        socketServer.launchServer();

    } catch (const std::exception &e) {
        std::cerr << "Error : " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

