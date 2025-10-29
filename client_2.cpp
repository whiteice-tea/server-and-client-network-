#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(){

    //1,创建socket

    int sock =socket(AF_INET,SOCK_STREAM,0);
    if(sock==-1){
        perror("socket");
        return 1;
    }

    //2,链接服务器
    sockaddr_in server{};
    server.sin_family =AF_INET;
    server.sin_port=htons(8080);
    inet_pton(AF_INET,"192.168.177.129",&server.sin_addr);

    if(connect(sock,(sockaddr*)&server,sizeof(server))==-1){
        perror("connect");
        return 1;
    }

    std::cout<<"Connected to server!"<<std::endl;

    //3，发送消息

    std::string msg="hello from client";
    write(sock,msg.c_str(),msg.size());

    //4,接受服务器回复
    char buffer[1024]={0};
    read(sock,buffer,sizeof(buffer));
    std::cout<<"Server replied:"<<buffer<<std::endl;

    //5,关闭链接
    close(sock);
    return 0;
}