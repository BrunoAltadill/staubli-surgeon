#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>
#include <vector>

using namespace std;

class TCPClient {
    public:
        TCPClient();
        TCPClient(string address, int port);
        bool sendData(string data);
        string receiveData(int size = 4096);
        string read();
        void exit();
    private:
        int sock;
        string address;
        int port;
        struct sockaddr_in server;
};

#endif
