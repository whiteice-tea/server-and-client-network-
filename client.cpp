#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket error");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect error");
        return 1;
    }

    std::string msg = "Hello from client";
    write(sock, msg.c_str(), msg.size());

    char buffer[1024] = {0};
    ssize_t bytes = read(sock, buffer, sizeof(buffer));
    std::cout << "Server replied: " << buffer << std::endl;

    close(sock);
    return 0;
}
