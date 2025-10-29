#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd==-1){
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8080);
    addr.sin_addr.s_addr=INADDR_ANY;


    if(bind(server_fd,(sockaddr*)&addr,sizeof(addr))==-1){
        perror("bind");
        return 1;
    }

    listen(server_fd,5);
    std::cout<<"Server listening on port 8080..."<<std::endl;

    sockaddr_in client{};
    socklen_t len =sizeof(client);
    int client_fd=accept(server_fd,(sockaddr*)&client,&len);
    if(client_fd==-1){
        perror("accept");
        return 1;

    }

    std::cout<<"Client connect!"<<std::endl;

    //循环通信
    char buffer[1024];
    while(true){
        memset(buffer,0,sizeof(buffer));
        int n =read(client_fd,buffer,sizeof(buffer));
        if(n<=0)break;//客户端断开

        std::cout<<"Client says:"<<buffer<<std::endl;

        if(strcmp(buffer,"exit")==0){
            std::cout<<"Client exited."<<std::endl;
            break;
        }

        std::string msg = std::string("OK:")+buffer;
        write(client_fd,msg.c_str(),msg.size());
    }

    close(client_fd);
    close(server_fd);
    return 0;

}