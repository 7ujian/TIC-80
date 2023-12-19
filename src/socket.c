#include "api.h"

#include "stdio.h"
#include "socket.h"

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <errno.h>
#endif

s32 tic_api_startnet(tic_mem* tic)
{
    printf("tic_api_lobby\n");

#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return -1;
    }
#endif
    return 0;
}

s32 tic_api_socket(tic_mem* tic)
{
    printf("tic_api_socket\n");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {

#ifdef WIN32
        printf("tic_api_socket: failed to create socket err=%d\n", WSAGetLastError());
#else
        printf("tic_api_socket: failed to create socket err=%d\n", errno);
#endif

        return -1;
    }

#ifdef WIN32
    unsigned long arg = 1;
    if(ioctlsocket(sock, FIONBIO, &arg)!=NO_ERROR)
    {
        printf("tic_api_socket: ioctlsocket failed\n");
        closesocket(sock);
        return -1;
    }
#endif
    return sock;
}


s32 tic_api_bind(tic_mem* tic, s32 sock, const char* addr, u16 port)
{
    printf("tic_api_bind: %s %d\n", addr, port);
    struct sockaddr_in saddr;
    
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(addr);

    int ret = bind(sock, &saddr, sizeof(saddr));
    if (ret == -1)
    {
        printf("tic_api_bind: bind failed\n");
    }

    return ret;
}

s32 tic_api_listen(tic_mem* tic, s32 sock, s32 backlog)
{
    return listen(sock, backlog);
}

s32 tic_api_accept(tic_mem* tic, s32 sock)
{
    return accept(sock, NULL, NULL);
}

s32 tic_api_connect(tic_mem* tic, s32 sock, const char* addr, u16 port)
{
    struct sockaddr_in saddr;
    
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(addr);

    s32 ret = connect(sock, &saddr, sizeof(saddr));
    if (ret !=0) {
#ifdef WIN32
        int err = WSAGetLastError();

        if (err == WSAEWOULDBLOCK)
            return 0;
        else
        {
            printf("tic_api_connect: connect failed %s:%d err=%d\n", addr,port, WSAGetLastError());
            return ret;
        } 
#else
            printf("tic_api_connect: connect failed %s:%d err=%d\n", addr,port, errno);
#endif 
    }

    return 0;
}

s32 tic_api_closesocket(tic_mem* tic, s32 sock)
{
    // TODO: 是否需要先Shutdown
    shutdown(sock, 0);
#ifdef WIN32
    return closesocket(sock);
#else
    return close(sock);
#endif
}


s32 tic_api_recv(tic_mem* tic, s32 sock)
{
    s32 n= recv(sock, tic->recv_buff, TIC_RECV_BUFF_SIZE, 0);
    
    return n;
}

s32 tic_api_send(tic_mem* tic, s32 sock, const char* data, s32 len)
{
    s32 ret= send(sock, data, len, 0);

    return ret;
}

s32 tic_api_gsopt(tic_mem* tic, s32 sock, int* error)
{
    int opt;
    int optLen=sizeof(int);

    s32 ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&opt, &optLen);

    (*error) = opt;
    if (ret < 0)
    {
#ifdef WIN32
        printf("tic_api_gsopt: opt=%d err=%d\n", opt, WSAGetLastError());
#else
        printf("tic_api_gsopt: opt=%d err=%d\n", opt, errno);
#endif
    }

    return ret;
}



