
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
using namespace std;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) == -1) { perror("bind"); return 1; }
    listen(server_fd, 5);
    cout << "Server listening on port 8080..." << endl;

    sockaddr_in client{};
    socklen_t len = sizeof(client);
    int client_fd = accept(server_fd, (sockaddr*)&client, &len);
    if (client_fd == -1) { perror("accept"); return 1; }

    char buffer[1024] = {0};
    read(client_fd, buffer, sizeof(buffer));
    cout << "Client says: " << buffer << endl;

    string msg = "Hello from server";
    write(client_fd, msg.c_str(), msg.size());

    close(client_fd);
    close(server_fd);
    return 0;
}
