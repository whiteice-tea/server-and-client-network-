#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) { perror("socket"); return 1; }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "192.168.177.129", &server.sin_addr); // Server IP

    if (connect(sock, (sockaddr*)&server, sizeof(server)) == -1) { perror("connect"); return 1; }

    string msg = "Hello from client";
    write(sock, msg.c_str(), msg.size());

    char buffer[1024] = {0};
    read(sock, buffer, sizeof(buffer));
    cout << "Server replied: " << buffer << endl;

    close(sock);
    return 0;
}

