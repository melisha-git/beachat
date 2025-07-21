#include <iostream>
#include "Server.hpp"

int main(int argc, char *argv[])
{
    std::string serverAddress;
    
    if (argc > 1)
    {
        serverAddress = argv[1];
        std::cout << "Использование адреса сервера: " << serverAddress << std::endl;
    }
    else
    {
        std::cerr << "Не указан адрес сервера. Используйте: " << argv[0] << " <server_address>\n";
        return 1;
    }

    Server server(serverAddress);

    try {
        server.start();
    } catch (const std::exception& e) {
        server.stop();
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
