#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>

int main(){
    //1，创建socket
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd==-1){
        perror("socket");
        return 1;
    }

    //2，绑定ip和端口
    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8080);
    addr.sin_addr.s_addr=INADDR_ANY;

    if(bind(server_fd,(sockaddr*)&addr,sizeof(addr))==-1){
        perror("bind");
        return 1;
    }

    //3，监听
    listen(server_fd,5);
    std::cout<<"Server is listening on port 8080..."<<std::endl;

    //4，等待客户端链接
    sockaddr_in client{};
    socklen_t len =sizeof(client);
    int client_fd=accept(server_fd,(sockaddr*)&client,&len);
    if(client_fd==-1){
        perror("accept");
        return 1;
    }

    std::cout<<"Client connected."<<std::endl;


    //5,接受消息
    char buffer[1024]={0};
    read(client_fd,buffer,sizeof(buffer));
    std::cout<<"Client says: "<<buffer<<std::endl;

    //6, 回复客户端
    std::string msg="hello from server!";
    write(client_fd,msg.c_str(),msg.size());

    //7，关闭链接
    close(client_fd);
    close(server_fd);
    return 0;


}