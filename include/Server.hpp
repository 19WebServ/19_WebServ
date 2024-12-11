#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>

class Server {
public:
    Server(const std::string &configFile);
    void run();
private:
    std::string _configFile;
};

#endif // SERVER_HPP