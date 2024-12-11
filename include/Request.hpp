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