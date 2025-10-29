#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sys/select.h>
#include <ctime>  // 用于时间戳
using namespace std;

vector<int> clients;       // 存放所有客户端的 socket
vector<string> client_names; // 存放所有客户端的用户名
fd_set readfds;            // 用于 select() 的文件描述符集

// 获取当前时间的字符串
string current_time() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return string(buf);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind"); return 1;
    }

    listen(server_fd, 5);
    cout << "✅ Server listening on port 8080..." << endl;

    int max_fd = server_fd;
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        for (int client_fd : clients) {
            FD_SET(client_fd, &readfds);
            if (client_fd > max_fd) max_fd = client_fd;
        }

        int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            perror("select");
            break;
        }

        // 处理新连接
        if (FD_ISSET(server_fd, &readfds)) {
            sockaddr_in client{};
            socklen_t len = sizeof(client);
            int client_fd = accept(server_fd, (sockaddr*)&client, &len);
            if (client_fd < 0) continue;

            // 让新客户端输入用户名
            char name[1024];
            memset(name, 0, sizeof(name));
            string welcome = "Enter your username: ";
            write(client_fd, welcome.c_str(), welcome.size());
            read(client_fd, name, sizeof(name));
            string client_name(name);

            clients.push_back(client_fd);
            client_names.push_back(client_name);

            cout << "🎯 New client " << client_name << " connected!" << endl;

            // 通知所有客户端新用户加入
            string msg = client_name + " has joined the chat!\n";
            for (int fd : clients) {
                if (fd != client_fd) {
                    write(fd, msg.c_str(), msg.size());
                }
            }
        }

        // 处理已有客户端消息
        for (auto it = clients.begin(); it != clients.end();) {
            int client_fd = *it;
            if (FD_ISSET(client_fd, &readfds)) {
                char buffer[1024] = {0};
                int n = read(client_fd, buffer, sizeof(buffer));

                if (n <= 0) {
                    cout << "👋 Client " << client_names[it - clients.begin()] << " disconnected." << endl;
                    string msg = client_names[it - clients.begin()] + " has left the chat.\n";
                    for (int fd : clients) {
                        if (fd != client_fd) {
                            write(fd, msg.c_str(), msg.size());
                        }
                    }

                    close(client_fd);
                    it = clients.erase(it);
                    client_names.erase(client_names.begin() + (it - clients.begin()));
                    continue;
                }

                // 添加时间戳和用户名
                string timestamp = "[" + current_time() + "] ";
                string message = timestamp + client_names[it - clients.begin()] + ": " + string(buffer);
                cout << message << endl;

                // 广播消息给所有客户端
                for (int fd : clients) {
                    if (fd != client_fd) {
                        write(fd, message.c_str(), message.size());
                    }
                }
            }
            ++it;
        }
    }

    close(server_fd);
    return 0;
}

