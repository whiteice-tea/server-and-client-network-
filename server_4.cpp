//多人聊天室
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sys/select.h>

int main(){
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == -1){
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_fd,(sockaddr*)&addr,sizeof(addr))==-1){
        perror("bind");
        return 1;
    }
    
    listen(server_fd,5);
    std::cout<<"Server listening on port 8080..."<<std::endl;

    std::vector<int> clients;
    fd_set readfds;
    int max_fd = server_fd;

    while(true){
        FD_ZERO(&readfds);
        FD_SET(server_fd,&readfds);

        for(int client_fd : clients){
            FD_SET(client_fd,&readfds);
            if(client_fd>max_fd) max_fd = client_fd;
        }

        int activity = select(max_fd+1,&readfds,nullptr,nullptr,nullptr);
        if(activity<0){
            perror("select");
            break;
        }

        // 有新的客户端连进来
        if (FD_ISSET(server_fd, &readfds)) {
            sockaddr_in client{};
            socklen_t len = sizeof(client);
            int client_fd = accept(server_fd, (sockaddr*)&client, &len);
            if (client_fd < 0) continue;

            clients.push_back(client_fd);
            std::cout << "🎯 New client connected! (Total: " << clients.size() << ")" << std::endl;

            std::string welcome = "Welcome! There are now " + std::to_string(clients.size()) + " people online.\n";
            write(client_fd, welcome.c_str(), welcome.size());
        }

        // 检查所有客户端是否有消息
        for (auto it = clients.begin(); it != clients.end();) {
            int client_fd = *it;
            if (FD_ISSET(client_fd, &readfds)) {
                char buffer[1024] = {0};
                int n = read(client_fd, buffer, sizeof(buffer));

                if (n <= 0) {
                    std::cout << "👋 Client disconnected." << std::endl;
                    close(client_fd);
                    it = clients.erase(it);
                    continue;
                }

                std::cout << "📨 Message: " << buffer << std::endl;

                // 广播给其他客户端
                std::string msg = "🗣 Someone: " + std::string(buffer);
                for (int fd : clients) {
                    if (fd != client_fd)
                        write(fd, msg.c_str(), msg.size());
                }
            }
            ++it;
        }
    }
    close(server_fd);
    return 0;

}