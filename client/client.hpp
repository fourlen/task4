#pragma once

#include <string>

class Client {
public:
    void execute(char* argv[]);
    std::string getFormattedTimestamp();

private:
    std::string GetCurrentTimestamp();
};
