#pragma once

#include <string>
#include <map>
#include <mutex>
#include <atomic>

#include "../Utils/ClientInfo.hpp"

class Server {
public:
    Server(const std::string& serverAddress);
    ~Server();

    void start();
    void stop();
private:
    void acceptClients();
    void readClientData();

    std::string serverAddress_;
    int serverSocket_;
    std::atomic_bool running_;
    std::map<std::string, ClientInfo> clients_;
    std::mutex clientsMutex_;
};