#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(){
    int sock =socket(AF_INET,SOCK_STREAM,0);
    if(sock==-1){
        perror("socket");
        return 1;

    }


    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET,"192.168.177.129",&server.sin_addr);

    if(connect(sock,(sockaddr*)&server,sizeof(server))==-1){
        perror("connect");
        return 1;
    }

    std::cout<<"Connected to server!"<<std::endl;

    //循环聊天
    char buffer[1024];
    while(true){
        std::cout<<"You: ";
        std::string msg;
        std::getline(std::cin,msg);

        write(sock,msg.c_str(),msg.size());
        if(msg=="exit")break;

        memset(buffer,0,sizeof(buffer));
        read(sock,buffer,sizeof(buffer));
        std::cout<<"Server: "<<buffer<<std::endl;

    }

    close(sock);
    return 0;
}