#include <iostream>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>
#include <map>
#include <string>

struct client_struct
{
    int sock = 0;
    char addr[18] = {0};
};

std::map<std::string, client_struct> clients;

int create_server()
{
    struct sockaddr_rc loc_addr = {0};
    int server_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (server_sock < 0)
    {
        perror("socket");
        return 1;
    }

    loc_addr.rc_family = AF_BLUETOOTH;
    str2ba("14:13:33:F8:91:F8", &loc_addr.rc_bdaddr);
    loc_addr.rc_channel = (uint8_t)1;

    if (bind(server_sock, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    listen(server_sock, 1);

    std::cout << "Ожидание подключения...\n";

    return server_sock;
}

void reading(client_struct client)
{
    char buf[1024] = {0};

    while (true)
    {
        int bytes_read = read(client.sock, buf, sizeof(buf));

        if (bytes_read > 0)
        {
            std::cout << client.addr << ": " << buf << std::endl;
            memset(buf, 0, sizeof(buf));
        }
        else
        {
            std::cout << "Клиент отключился: " << client.addr << std::endl;
            close(client.sock);
            clients.erase(client.addr);
            return;
        }
    }
}

void accepting(int server_sock)
{
    while (true)
    {
        client_struct result;
        struct sockaddr_rc rem_addr = {0};
        socklen_t opt = sizeof(rem_addr);

        result.sock = accept(server_sock, (struct sockaddr *)&rem_addr, &opt);
    
        if (result.sock < 0)
        {
            perror("accept");
            return;
        }

        ba2str(&rem_addr.rc_bdaddr, result.addr);
        std::cout << "Клиент подключился: " << result.addr << std::endl;

        clients[result.addr] = result;

        static const char* message = "Ты лох и жизнь твоя лох!";
        write(result.sock, message, strlen(message));
        std::thread read_thread{reading, result};
        read_thread.detach();
    }
}

int main()
{
    int server_sock = create_server();

    std::thread accepting_thread{accepting, server_sock};

    accepting_thread.join();
    close(server_sock);

    return 0;
}
