#include "Server.hpp"

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <thread>

Server::Server(const std::string& serverAddress)
    : serverAddress_(serverAddress), serverSocket_(-1), running_(false) {}

Server::~Server() {
    stop();
}

void Server::start() {
    struct sockaddr_rc localAddress = {0};
    serverSocket_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (serverSocket_ < 0)
    {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }

    localAddress.rc_family = AF_BLUETOOTH;
    str2ba(serverAddress_.c_str(), &localAddress.rc_bdaddr);
    localAddress.rc_channel = static_cast<uint8_t>(1);

    if (bind(serverSocket_, reinterpret_cast<struct sockaddr *>(&localAddress), sizeof(localAddress)) < 0)
    {
        perror("bind");
        throw std::runtime_error("Failed to bind socket");
    }

    listen(serverSocket_, 1);

    std::cout << "Ожидание подключения...\n";
    running_ = true;

    acceptClients();
}

void Server::stop() {
    if (running_) {
        running_ = false;
        close(serverSocket_);
        serverSocket_ = -1;
        std::cout << "Сервер остановлен." << std::endl;
    }
}

void Server::acceptClients() {
    while (running_) {
        ClientInfo client;
        struct sockaddr_rc remoteAddress = {0};
        socklen_t opt = sizeof(remoteAddress);

        client.sock = accept(serverSocket_, reinterpret_cast<struct sockaddr *>(&remoteAddress), &opt);
        
        if (client.sock < 0) {
            perror("accept");
            continue;
        }

        ba2str(&remoteAddress.rc_bdaddr, client.addr);
        std::cout << "Клиент подключился: " << client.addr << std::endl;

        std::lock_guard<std::mutex> clientsLock(clientsMutex_);

        clients_[client.addr] = client;

        static const char* message = "Успешно подключён к серверу\n";
        write(client.sock, message, strlen(message));

        std::thread readThread(&Server::readClientData, this);
        readThread.detach();
    }
}

void Server::readClientData() {
    char buf[1024] = {0};

    while (running_) {
        for (const auto& [addr, client] : clients_) {
            memset(buf, 0, sizeof(buf));
            int bytesRead = read(client.sock, buf, sizeof(buf));

            if (bytesRead > 0) {
                std::cout << addr << ": " << buf << std::endl;
            } else {
                std::cout << "Клиент отключился: " << addr << std::endl;
                std::lock_guard<std::mutex> clientsLock(clientsMutex_);
                close(client.sock);
                clients_.erase(addr);
            }
        }
    }
}