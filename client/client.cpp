#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "Client.hpp"

std::string Client::getFormattedTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto timeInSec = std::chrono::system_clock::to_time_t(now);
    auto timeInms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto currentTime = *std::localtime(&timeInSec);

    char formattedTime[100];
    std::strftime(formattedTime, sizeof(formattedTime), "[%Y-%m-%d %H:%M:%S", &currentTime);
    std::snprintf(formattedTime + std::strlen(formattedTime), sizeof(formattedTime) - std::strlen(formattedTime), ".%03ld]", timeInms.count() % 1000);

    return formattedTime;
}

void Client::execute(char* arguments[]) {
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(std::stoi(arguments[2]));
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    while (true) {
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

        if (clientSocket == -1)
            throw std::runtime_error("socket() failure");

        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
            throw std::runtime_error("connect() failure");

        std::string message = getFormattedTimestamp() + " " + arguments[1];

        send(clientSocket, message.data(), message.size(), 0);
        close(clientSocket);

        std::this_thread::sleep_for(std::chrono::seconds(std::stoi(arguments[3])));
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        throw std::runtime_error("wrong args count");
    }

    Client clientInstance;

    clientInstance.execute(argv);
}
