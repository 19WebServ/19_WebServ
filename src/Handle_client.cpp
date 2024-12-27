#include "../include/Handle_client.hpp"

/**
 * @brief read a html file
 * 
 * @param filename
 * 
 * @return std::string buffer
 */
std::string readFile(const char *filename) 
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return "";

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, ' ');
    if (!file.read(&buffer[0], size))
        return "";
    file.close();
    return buffer;
}

/**
 * @brief Processing client request 
 * 
 * @param buffer, bytes_receive, Socket_instance, client_socket
 * 
 * @return int
 */
int processing_request(char *buffer, int bytes_receiv, Socket &server, int client)
{
    buffer[bytes_receiv] = '\0';

    std::string request(buffer);
    if (request.find("GET /favicon.ico") != std::string::npos) 
    {
        std::cout << "[INFO] Favicon request ignored.\n";
        return 0;
    }
    std::string htmlContent;
    if (request.find("GET /televers.html") != std::string::npos)
        htmlContent = readFile("pages_html/televers.html");
    else if (request.find("GET /contact.html") != std::string::npos)
        htmlContent = readFile("pages_html/contact.html");
    else if (request.find("GET /service.html") != std::string::npos)
        htmlContent = readFile("pages_html/service.html");
    else
        htmlContent = readFile("pages_html/index.html");

    std::cout << "Received from client:\n" << buffer << std::endl;

    if (htmlContent.empty()) 
    {
        std::cerr << "Failed to read index.html." << std::endl;
        return 1;
    }

    std::ostringstream oss;
    oss << htmlContent.size();
    std::string contentLength = oss.str();

    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + contentLength + "\r\n"
        "\r\n" +
        htmlContent;

    int total_sent = 0;
    while (total_sent < (int)response.size())
    {
        int bytes_sent = server.sendData(client, response.c_str() + total_sent, response.size() - total_sent);
        if (bytes_sent > 0)
            total_sent += bytes_sent;
        else if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            std::cerr << "Failed to send response to client." << std::endl;
            break;
        }
    }
    return 0;
}



