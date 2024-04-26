#pragma once

#include <string_view>

class Server {

public:
    void handleConnections(const std::string& port);

private:
    void saveOutput(std::string_view data);
    void processClient(int client_socket);
};
