#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "server.hpp"

void Server::saveOutput(std::string_view messageData) {
    std::ofstream logFile("out", std::ios::app);
    logFile << messageData << '\n';
}

void Server::processClient(int clientSocket) {
    char receivedData[1024] = {0};

    if (recv(clientSocket, receivedData, 1024, 0) < 0) {
        std::cerr << "Failed to receive data.\n";
        close(clientSocket);
        return;
    }

    saveOutput(receivedData);
    close(clientSocket);
}

void Server::handleConnections(const std::string& portString) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1)
        throw std::runtime_error("Failed to create socket.");

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("Failed to set socket options.");

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(std::stoi(portString));

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
        throw std::runtime_error("Failed to bind socket.");

    while (true) {
        if (listen(serverSocket, 10) < 0)
            throw std::runtime_error("Failed listening on socket.");

        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);

        if (clientSocket < 0)
            throw std::runtime_error("Socket accept failed.");

        std::thread clientThread([this, clientSocket]() { 
            this->processClient(clientSocket); 
        });

        clientThread.detach();
    }

    close(serverSocket);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        throw std::runtime_error("wrong args count");
    }

    Server server;

    server.handleConnections(argv[1]);
}
