#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <thread>
using namespace std;

// 接收线程
void recv_msg(int sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = read(sock, buffer, sizeof(buffer));
        if (n <= 0) break;
        cout << buffer << endl;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) { perror("socket"); return 1; }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "192.168.177.129", &server.sin_addr);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) == -1) {
        perror("connect");
        return 1;
    }

    cout << "🚀 Connected to server!" << endl;

    // 接收消息的线程
    thread recv_thread(recv_msg, sock);
    recv_thread.detach();

    // 输入用户名
    string username;
    cout << "Enter your username: ";
    getline(cin, username);

    // 发送用户名
    write(sock, username.c_str(), username.size());

    // 发送消息
    string msg;
    while (true) {
        cout << "You: ";
        getline(cin, msg);
        if (msg == "exit") break;
        write(sock, msg.c_str(), msg.size());
    }

    close(sock);
    return 0;
}