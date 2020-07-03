#ifndef _FTP_H_
#define _FTP_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <fstream>

#include "Socket.h"
#include "Tokenizer.h"
#include "Timer.h"

#define COMMANDPORT 21;
using namespace std;

class Ftp
{
    public:
        Ftp();
        ~Ftp();
        void ExecuteOpen(string ip, int port);
        void ExecuteCd(string dir);
        void ExecuteGet(string remote, string local);
        void ExecutePut(string local, string remote);
        void ExecuteLs(string path);
        void ExecuteQuit();
        string getServerName();
        bool Active();

    private:
        int openDataSocket();
        void login();
        void closeSocket(int socketSD);
        int rcvReply();
        int rcvData();
        int rcvFile(string localFilename);
        int sendCommand(string command);
        
        string serverName;
        string reply;
        string data;

        int dataPort;

        int commandSD;
        int dataSD;

        Tokenizer tokenizer;
        Timer timer;
};

#endif