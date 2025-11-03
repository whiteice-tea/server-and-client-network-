// #include <iostream>
// #include <fstream>
// #include <cstdio>
// #include <cstring>
// #include <cstdlib>
// #include <unistd.h>
// #include <netdb.h>
// #include <signal.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// using namespace std;

// class ctcpserver         // TCP通讯的服务端类。
// {
// private:
//   int    m_listenfd;        // 监听的socket，-1表示未初始化。
//   int    m_clientfd;        // 客户端连上来的socket，-1表示客户端未连接。
//   string m_clientip;        // 客户端字符串格式的IP。
//   unsigned short m_port;    // 服务端用于通讯的端口。
// public:
//   ctcpserver():m_listenfd(-1),m_clientfd(-1) {}

//   // 初始化服务端用于监听的socket。
//   bool initserver(const unsigned short in_port)
//   {
//     // 第1步：创建服务端的socket。
//     if ( (m_listenfd=socket(AF_INET,SOCK_STREAM,0))==-1) return false;

//     m_port=in_port;

//     // 第2步：把服务端用于通信的IP和端口绑定到socket上。
//     struct sockaddr_in servaddr;                // 用于存放协议、端口和IP地址的结构体。
//     memset(&servaddr,0,sizeof(servaddr));
//     servaddr.sin_family=AF_INET;                // ①协议族，固定填AF_INET。
//     servaddr.sin_port=htons(m_port);            // ②指定服务端的通信端口。
//     servaddr.sin_addr.s_addr=htonl(INADDR_ANY); // ③如果操作系统有多个IP，全部的IP都可以用于通讯。

//     // 绑定服务端的IP和端口（为socket分配IP和端口）。
//     if (bind(m_listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1)
//     {
//       close(m_listenfd); m_listenfd=-1; return false;
//     }

//     // 第3步：把socket设置为可连接（监听）的状态。
//     if (listen(m_listenfd,5) == -1 )
//     {
//       close(m_listenfd); m_listenfd=-1; return false;
//     }

//     return true;
//   }

//   // 受理客户端的连接（从已连接的客户端中取出一个客户端），
//   // 如果没有已连接的客户端，accept()函数将阻塞等待。
//   bool accept()
//   {
//     struct sockaddr_in caddr;        // 客户端的地址信息。
//     socklen_t addrlen=sizeof(caddr); // struct sockaddr_in的大小。
//     if ((m_clientfd=::accept(m_listenfd,(struct sockaddr *)&caddr,&addrlen))==-1) return false;

//     m_clientip=inet_ntoa(caddr.sin_addr);  // 把客户端的地址从大端序转换成字符串。

//     return true;
//   }

//   // 获取客户端的IP(字符串格式)。
//   const string & clientip() const
//   {
//     return m_clientip;
//   }

//   // 向对端发送报文，成功返回true，失败返回false。
//   bool send(const string &buffer)
//   {
//     if (m_clientfd==-1) return false;

//     if ( (::send(m_clientfd,buffer.data(),buffer.size(),0))<=0) return false;

//     return true;
//   }

//   // 接收对端的报文（字符串），成功返回true，失败返回false。
//   // buffer-存放接收到的报文的内容，maxlen-本次接收报文的最大长度。
//   bool recv(string &buffer,const size_t maxlen)
//   {
//     buffer.clear();         // 清空容器。
//     buffer.resize(maxlen);  // 设置容器的大小为maxlen。
//     int readn=::recv(m_clientfd,&buffer[0],buffer.size(),0);  // 直接操作buffer的内存。
//     if (readn<=0) { buffer.clear(); return false; }
//     buffer.resize(readn);   // 重置buffer的实际大小。

//     return true;
//   }

//   // 接收客户端的报文（二进制数据），成功返回true，失败返回false。
//   // buffer-存放接收到的报文的内容，size-本次接收报文的最大长度。
//   bool recv(void *buffer,const size_t size)
//   {
//     if (::recv(m_clientfd,buffer,size,0)<=0) return false;

//     return true;
//   }

//   // 关闭监听的socket。
//   bool closelisten()
//   {
//     if (m_listenfd==-1) return false;

//     ::close(m_listenfd);
//     m_listenfd=-1;
//     return true;
//   }

//   // 关闭客户端连上来的socket。
//   bool closeclient()
//   {
//     if (m_clientfd==-1) return false;

//     ::close(m_clientfd);
//     m_clientfd=-1;
//     return true;
//   }

//   // 接收文件内容。
//   bool recvfile(const string &filename, const size_t filesize)
//   {
//     // 确保路径正确拼接
//     string filePath = filename;
//     if (filePath.back() == '/') {
//         filePath.pop_back();  // 去除路径末尾的斜杠
//     }

//     ofstream fout;
//     fout.open(filePath, ios::binary);
//     if (!fout.is_open()) {
//         cout << "Failed to open file: " << filePath << endl;
//         return false;
//     }

//     cout << "Saving file to: " << filePath << endl;

//     int totalbytes = 0;
//     int onread = 0;
//     char buffer[4096];

//     while (true) {
//         if (filesize - totalbytes > 4096) onread = 4096;
//         else onread = filesize - totalbytes;

//         if (recv(buffer, onread) == false) return false;
//         fout.write(buffer, onread);
//         totalbytes += onread;

//         if (totalbytes == filesize) break;
//     }

//     return true;
//   }

//  ~ctcpserver() { closelisten(); closeclient(); }
// };

// ctcpserver tcpserver;

// void FathEXIT(int sig);  // 父进程的信号处理函数。
// void ChldEXIT(int sig);  // 子进程的信号处理函数。

// int main(int argc,char *argv[])
// {
//   if (argc!=3)
//   {
//     cout << "Using:./demo12 通讯端口 文件存放的目录\n";
//     cout << "Example:./demo12 5005 /tmp\n\n";
//     cout << "注意：运行服务端程序的Linux系统的防火墙必须要开通5005端口。\n";
//     cout << "      如果是云服务器，还要开通云平台的访问策略。\n\n";
//     return -1;
//   }

//   // 忽略全部的信号，不希望被打扰。顺便解决了僵尸进程的问题。
//   for (int ii=1;ii<=64;ii++) signal(ii,SIG_IGN);

//   // 设置信号,在shell状态下可用 "kill 进程号" 或 "Ctrl+c" 正常终止些进程
//   // 但请不要用 "kill -9 +进程号" 强行终止
//   signal(SIGTERM,FathEXIT); signal(SIGINT,FathEXIT);  // SIGTERM 15 SIGINT 2

//   if (tcpserver.initserver(atoi(argv[1]))==false) // 初始化服务端用于监听的socket。
//   {
//     perror("initserver()"); return -1;
//   }

//   while (true)
//   {
//     // 受理客户端的连接（从已连接的客户端中取出一个客户端），
//     // 如果没有已连接的客户端，accept()函数将阻塞等待。
//     if (tcpserver.accept()==false)
//     {
//       perror("accept()"); return -1;
//     }

//     int pid=fork();
//     if (pid==-1) { perror("fork()"); return -1; }  // 系统资源不足。
//     if (pid>  0)
//     { // 父进程。
//       tcpserver.closeclient();  // 父进程关闭客户端连接的socket。
//       continue;                 // 父进程返回到循环开始的位置，继续受理客户端的连接。
//     }

//     tcpserver.closelisten();    // 子进程关闭监听的socket。

//     // 子进程需要重新设置信号。
//     signal(SIGTERM,ChldEXIT);   // 子进程的退出函数与父进程不一样。
//     signal(SIGINT ,SIG_IGN);    // 子进程不需要捕获SIGINT信号。

//     // 子进程负责与客户端进行通讯。
//     cout << "客户端已连接(" << tcpserver.clientip() << ")。\n";

//     // 以下是接收文件的流程。
//     // 1）接收文件名和文件大小信息。
//     // 定义文件信息的结构体。
//     struct st_fileinfo{
//       char filename[256];  // 文件名。
//       int  filesize;       // 文件大小。
//     }fileinfo;
//     memset(&fileinfo,0,sizeof(fileinfo));
//     // 用结构体存放接收报文的内容。
//     if (tcpserver.recv(&fileinfo,sizeof(fileinfo))==false) { perror("recv()"); return -1; }
//     cout << "文件信息结构体" << fileinfo.filename << "(" << fileinfo.filesize <<")。"<< endl;

//     // 2）给客户端回复确认报文，表示客户端可以发送文件了。
//     if (tcpserver.send("ok")==false)  { perror("send"); break; }

//     // 3）接收文件内容。  string   char * + const char * + char *
//     if (tcpserver.recvfile(string(argv[2])+"/"+fileinfo.filename,fileinfo.filesize)==false)
//     {
//       cout << "接收文件内容失败。\n"; return -1;
//     }

//     cout << "接收文件内容成功。\n";

//     // 4）给客户端回复确认报文，表示文件已接收成功。
//     tcpserver.send("ok");

//     return 0;  // 子进程一定要退出，否则又会回到accept()函数的位置。
//   }
// }

// // 父进程的信号处理函数。
// void FathEXIT(int sig)
// {
//   // 以下代码是为了防止信号处理函数在执行的过程中再次被信号中断。
//   signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

//   cout << "父进程退出，sig=" << sig << endl;

//   kill(0,SIGTERM);     // 向全部的子进程发送15的信号，通知它们退出。

//   // 在这里增加释放资源的代码（全局的资源）。
//   tcpserver.closelisten();       // 父进程关闭监听的socket。

//   exit(0);
// }

// // 子进程的信号处理函数。
// void ChldEXIT(int sig)
// {
//   // 以下代码是为了防止信号处理函数在执行的过程中再次被信号中断。
//   signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

//   cout << "子进程" << getpid() << "退出，sig=" << sig << endl;

//   // 在这里增加释放资源的代码（只释放子进程的资源）。
//   tcpserver.closeclient();       // 子进程关闭客户端连上来的socket。

//   exit(0);
// }

// #include <arpa/inet.h>
// #include <unistd.h>
// #include <iostream>
// #include <fstream>
// #include <cstring>
// #include <sys/stat.h>

// using namespace std;

// #define PORT 8080

// // 接收文件
// void recv_file(int client_sock) {
//     char buffer[1024];
//     int bytes_received;

//     // === Step 1. 接收文件名长度 ===
//     int name_len;
//     bytes_received = read(client_sock, &name_len, sizeof(name_len));
//     if (bytes_received <= 0) {
//         cout << "Failed to read filename length" << endl;
//         return;
//     }

//     // === Step 2. 接收文件名 ===
// char filename_buf[256];
// bytes_received = read(client_sock, filename_buf, name_len);
// if (bytes_received <= 0) {
//     cout << "Failed to read filename" << endl;
//     return;
// }
// filename_buf[name_len] = '\0';
// string filename = filename_buf;

// // ✅ 只保留文件名部分
// size_t pos = filename.find_last_of('/');
// if (pos != string::npos) {
//     filename = filename.substr(pos + 1);
// }
//     // === Step 3. 接收文件大小 ===
//     long long file_size = 0;
//     bytes_received = read(client_sock, &file_size, sizeof(file_size));
//     if (bytes_received <= 0) {
//         cout << "Failed to read file size" << endl;
//         return;
//     }

//     // === Step 4. 打开输出文件 ===
//     string save_path = "/home/whiteicetea/download_client/" + filename;
//     ofstream outfile(save_path, ios::binary);
//     if (!outfile) {
//         cout << "Error opening file for writing: " << save_path << endl;
//         return;
//     }

//     // === Step 5. 循环接收文件内容 ===
//     long long total_received = 0;
//     while (total_received < file_size &&
//            (bytes_received = read(client_sock, buffer, sizeof(buffer))) > 0) {
//         outfile.write(buffer, bytes_received);
//         total_received += bytes_received;
//     }

//     outfile.close();

//     if (total_received == file_size) {
//         cout << "✅ File received successfully: " << save_path
//              << " (" << file_size << " bytes)" << endl;
//     } else {
//         cout << "⚠️ Incomplete file received (" << total_received << "/" << file_size << ")" << endl;
//     }
// }

// int main() {
//     int server_sock, client_sock;
//     sockaddr_in server{}, client{};
//     socklen_t addr_len = sizeof(client);

//     // === Step 1. 创建 socket ===
//     if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//         perror("socket");
//         return 1;
//     }

//     server.sin_family = AF_INET;
//     server.sin_port = htons(PORT);
//     server.sin_addr.s_addr = INADDR_ANY;

//     // === Step 2. 绑定端口 ===
//     if (bind(server_sock, (sockaddr*)&server, sizeof(server)) == -1) {
//         perror("bind");
//         close(server_sock);
//         return 1;
//     }

//     // === Step 3. 开始监听 ===
//     if (listen(server_sock, 5) == -1) {
//         perror("listen");
//         close(server_sock);
//         return 1;
//     }

//     cout << "Server listening on port " << PORT << "..." << endl;

//     // === Step 4. 等待客户端连接 ===
//     while ((client_sock = accept(server_sock, (sockaddr*)&client, &addr_len)) != -1) {
//         cout << "Client connected!" << endl;
//         recv_file(client_sock);
//         close(client_sock);
//         cout << "Client disconnected.\n" << endl;
//     }

//     close(server_sock);
//     return 0;
// }

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <endian.h>

static const int PORT = 8080;
static const char *SAVE_DIR = "/home/whiteicetea/download_client";

// 64位大小端转换函数
static uint64_t htonll(uint64_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return ((uint64_t)htonl((uint32_t)(x & 0xFFFFFFFFULL)) << 32) | htonl((uint32_t)(x >> 32));
#else
    return x;
#endif
}
static uint64_t ntohll(uint64_t x)
{
    return htonll(x);
}

ssize_t readn(int fd, void *buf, size_t n)
{
    size_t left = n;
    char *p = (char *)buf;
    while (left > 0)
    {
        ssize_t r = ::read(fd, p, left);
        if (r == 0)
            return n - left;
        if (r < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        p += r;
        left -= r;
    }
    return n;
}

ssize_t writen(int fd, const void *buf, size_t n)
{
    size_t left = n;
    const char *p = (const char *)buf;
    while (left > 0)
    {
        ssize_t r = ::write(fd, p, left);
        if (r <= 0)
        {
            if (r < 0 && errno == EINTR)
                continue;
            return -1;
        }
        p += r;
        left -= r;
    }
    return n;
}

static std::string basename_only(const std::string &path)
{
    size_t pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? path : path.substr(pos + 1);
}

void ensure_save_dir()
{
    struct stat st{};
    if (stat(SAVE_DIR, &st) == -1)
    {
        if (mkdir(SAVE_DIR, 0755) == -1)
        {
            perror("mkdir SAVE_DIR");
            exit(1);
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        std::cerr << "SAVE_DIR exists but is not a directory\n";
        exit(1);
    }
}

bool handle_one_client(int cfd)
{
    // 1) 读文件名长度
    uint32_t name_len_net = 0;
    if (readn(cfd, &name_len_net, sizeof(name_len_net)) != sizeof(name_len_net))
    {
        std::cerr << "Failed to read filename length\n";
        return false;
    }

    uint32_t name_len = ntohl(name_len_net);
    if (name_len == 0 || name_len > 1024)
    {
        std::cerr << "Bad filename length: " << name_len << "\n";
        return false;
    }
    // 2) 文件名
    std::string fname(name_len, '\0');
    if (readn(cfd, &fname[0], name_len) != (ssize_t)name_len)
    {
        std::cerr << "Failed to read filename\n";
        return false;
    }
    std::string base = basename_only(fname);

    // 3) 文件大小
    uint64_t fsize_net = 0;
    if (readn(cfd, &fsize_net, sizeof(fsize_net)) != (ssize_t)sizeof(fsize_net))
    {
        std::cerr << "Failed to read file size\n";
        return false;
    }
    uint64_t fsize = ntohll(fsize_net);

    // 4) 打开输出文件
    ensure_save_dir();
    std::string save_path = std::string(SAVE_DIR) + "/" + base;
    std::ofstream out(save_path, std::ios::binary);
    if (!out)
    {
        std::cerr << "Error opening file for writing: " << save_path << "\n";
        return false;
    }

    // 5) 循环接收文件内容
    const size_t BUF_SZ = 64 * 1024;
    static char buf[BUF_SZ];
    uint64_t received = 0;
    while (received < fsize)
    {
        size_t need = (size_t)std::min<uint64_t>(BUF_SZ, fsize - received);
        ssize_t n = readn(cfd, buf, need);
        if (n <= 0)
        {
            std::cerr << "Connection closed or read error while receiving body\n";
            return false;
        }
        out.write(buf, n);
        received += static_cast<uint64_t>(n);
    }
    out.close();

    std::cout << "✅ Saved: " << save_path << " (" << fsize << " bytes)\n";
    return true;
}

int main()
{
    // 1) 监听 socket
    int lfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return 1;
    }

    int on = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        perror("setsockopt");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY; // 绑定所有本机地址

    if (bind(lfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1)
    {
        perror("bind");
        return 1;
    }
    if (listen(lfd, 16) == -1)
    {
        perror("listen");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << " ...\n";

    // 2) 循环接入并处理
    while (true)
    {
        sockaddr_in cli{};
        socklen_t len = sizeof(cli);
        int cfd = accept(lfd, reinterpret_cast<sockaddr *>(&cli), &len);
        if (cfd == -1)
        {
            perror("accept");
            continue;
        }

        char ip[64] = {0};
        inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
        std::cout << "Client connected: " << ip << ":" << ntohs(cli.sin_port) << "\n";

        (void)handle_one_client(cfd);
        ::close(cfd);

        std::cout << "Client disconnected.\n";
    }

    ::close(lfd);
    return 0;
}