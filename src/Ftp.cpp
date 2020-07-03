#include "Ftp.h"

// Constructor
Ftp::Ftp()
{
    dataPort = -1;
    commandSD = -1;
    dataSD = -1;
}

// Destructor
Ftp::~Ftp()
{
    // close the socket if open
    this->closeSocket(this->commandSD);
    this->closeSocket(this->dataSD);
}

// execute "open ip port" command
void Ftp::ExecuteOpen(string ip, int port)
{

    Socket *sock = new Socket(port);
    this->serverName = ip;
    this->commandSD = sock->getClientSocket((char *)ip.c_str());
    cout << "Connected to " << this->serverName << " (" << sock->getHostIp() << ")." << endl;
    this->rcvReply();
    cout << this->reply;

    // then login
    this->login();
}

// login procidure - only after connecting to the ftp server
void Ftp::login()
{
    // User name
    string userInput;
    string userString(getlogin());
    cout << "Name (" << this->serverName << ":" << userString << "): ";
    getline(cin, userInput);
    userInput = "USER " + userInput;
    sendCommand(userInput);
    this->rcvReply();
    cout << this->reply;

    // Password
    cout << "Enter password: ";
    getline(cin, userInput);
    userInput = "PASS " + userInput;
    sendCommand(userInput);
    this->rcvReply();
    cout << this->reply;
    if (tokenizer.tokenize(this->reply.c_str(), ' ').at(0) == "501")
    {
        cout << "Login failed." << endl;
    }
    else
    {
        this->rcvReply();
        cout << this->reply;
    }

    // system information
    sendCommand("SYST");
    this->rcvReply();
    cout << this->reply;
    cout << "Using binary mode to transfer files." << endl;
}

// execute cd command
void Ftp::ExecuteCd(string dir)
{
    this->sendCommand("CWD " + dir);
    this->rcvReply();
    cout << this->reply;
}

// execute ls command
void Ftp::ExecuteLs(string path)
{
    // try to get data connection, retrun if not
    this->dataSD = this->openDataSocket();
    if (this->dataSD == -1)
    {
        return;
    }
    
    // send ls command
    this->sendCommand("LIST " + path);
    int statusCode = this->rcvReply();
    //cout << this->reply;

    // if the status code = 150 read from the data connection
    if (statusCode == 150)
    {
        cout << "Does it get here?" << endl;
        int fk = fork();

        if (fk < 0)
        {
            cerr << "Fork error.";
        }
        else if (fk == 0)
        {
            // child process reading from data connection
            this->rcvData();
            cout << this->data;

            // finish child process
            exit(EXIT_SUCCESS);
        }
        else
        {
            // wait for child to finish reading
            wait(NULL);

            // data transfer complete message
            this->rcvReply();
            cout << this->reply;
        }
    }    
    // close the data connection
    this->closeSocket(this->dataSD);
    this->dataSD = -1;
}

// execute get file command
void Ftp::ExecuteGet(string remote, string local)
{
    cout << "local: " << local << " remote: " << remote << endl;
    
    // try to get data connection, return if not
    this->dataSD = this->openDataSocket();
    if (this->dataSD == -1)
    {
        return;
    }

    // enter binary mode
    this->sendCommand("TYPE I");
    this->rcvReply();
    cout << this->reply;

    // the get command
    this->sendCommand("RETR " + remote);
    int statusCode = this->rcvReply();
    cout << this->reply;

    // if status code = 150 read read, otherwise close data connection
    if (statusCode == 150)
    {
        int fk = fork();
        if (fk < 0)
        {
            cerr << "Forking error.";
        }
        else if (fk == 0)
        {
            // start timer, recieve file, stop timer
            timer.Start();
            int totalData = this->rcvFile(local);
            float lapSec = timer.End() / 1000000.0;

            // file transfer complete message and report
            this->rcvReply();
            cout << this->reply;
            if (totalData > 0)
            {
                long throughput = totalData / (lapSec * 1024);
                string report = "" + to_string(totalData) +
                                " bytes received in " + to_string(lapSec) +
                                " secs (" + to_string(throughput) +
                                " Kbytes/sec)";
                cout << report << endl;
            }

            // finish child process
            exit(EXIT_SUCCESS);
        }
        else
        {
            // parent waiting for child to finish reading
            wait(NULL);
        }
    }

    // close the data connection
    this->closeSocket(this->dataSD);
    this->dataSD = -1;
}

// execute put file command
void Ftp::ExecutePut(string local, string remote)
{
    cout << "local: " << local << " remote: " << remote << endl;

    // open the file in read mode
    fstream file;
    file.open(local, ios::in | ios::binary);
    if (!file)
    {
        cout << "local: " << local << ": No such file or directory" << endl;
        return;
    }

    // try to get data connection, return if not
    this->dataSD = this->openDataSocket();
    if (this->dataSD == -1)
    {
        return;
    }

    // entering binary mode
    this->sendCommand("TYPE I");
    this->rcvReply();
    cout << this->reply;

    // the put command
    this->sendCommand("STOR " + remote);
    int statusCode = this->rcvReply();
    cout << this->reply;

    // check the status code of the reply = "150"
    if (statusCode == 150)
    {
        // preparation
        file.seekg(0, ios::beg);
        char buffer[8192];
        int totalByteSent = 0;
        int bytesRead = 0;

        // start the timer
        timer.Start();

        // data transfer throught the data connection
        do
        {
            file.read(buffer, 8192);
            bytesRead = file.gcount();
            if (bytesRead > 0)
            {
                write(this->dataSD, buffer, bytesRead);
                totalByteSent += bytesRead;
            }
        } while (bytesRead > 0);

        // stop timer, transfer complet
        float lapSec = timer.End() / 1000000.0;

        // close the data connection
        this->closeSocket(this->dataSD);
        this->dataSD = -1;

        // the transfer complete reply and report
        this->rcvReply();
        cout << this->reply;
        if (totalByteSent > 0)
        {
            long throughput = totalByteSent / (lapSec * 1024);
            string report = "" + to_string(totalByteSent) +
                            " bytes sent in " + to_string(lapSec) +
                            " secs (" + to_string(throughput) +
                            " Kbytes/sec)";
            cout << report << endl;
        }
    }
    else
    {
        // close the data connection
        this->closeSocket(this->dataSD);
        this->dataSD = -1;
    }
}

// puts server reply on reply member variable and returns the status code
int Ftp::rcvReply()
{
    char msg[8192];
    bzero(msg, sizeof(msg));
    int bytesRead = read(this->commandSD, msg, sizeof(msg));
    this->reply = msg;

    // returning the status code
    return stoi(tokenizer.tokenize(msg, ' ').at(0));
}

// receives data and puts on data(memeber variable) & returns bytes received
int Ftp::rcvData()
{
    this->data.clear();
    char buffer[8192];
    struct pollfd pfd[1];
    pfd[0].fd = this->dataSD;
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;

    int dataSize = 0;
    int bytesRead = 1; // to make it go through the loop once
    while ((poll(pfd, 1, 1000) > 0) && bytesRead > 0)
    {
        bzero(buffer, sizeof(buffer));
        bytesRead = read(this->dataSD, buffer, sizeof(buffer));
        this->data.append(buffer);
        dataSize += bytesRead;
    }
    return dataSize;
}

// receive file, return the bytes received, helper function for get command
int Ftp::rcvFile(string local)
{
    // open or create file locally with permission mode rw-r--r--
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    int fileFD = open(local.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);

    char buffer[8192];
    struct pollfd pfd[1];
    pfd[0].fd = this->dataSD;
    pfd[0].events = POLLIN;
    pfd[0].revents = 0;
    int totalData = 0;
    int bytesRead = 1; // so it goes through the while loop at least once
    while ((poll(pfd, 1, 1000) > 0) && bytesRead > 0)
    {
        bzero(buffer, sizeof(buffer));
        bytesRead = read(this->dataSD, buffer, sizeof(buffer));
        lseek(fileFD, 0, SEEK_END);
        write(fileFD, buffer, bytesRead);
        totalData += bytesRead;
    }
    // close the local file
    close(fileFD);

    return totalData;
}

// send command to the server
int Ftp::sendCommand(string command)
{
    command += "\r\n";
    char msg[command.length()];
    bzero(msg, sizeof(msg));
    strcpy(msg, command.c_str());
    return write(this->commandSD, msg, sizeof(msg));
}

// opens data socket and returns the file descriptor
int Ftp::openDataSocket()
{
    if (!this->Active())
    {
        cerr << "No command connection" << endl;
    }

    this->sendCommand("PASV");
    int statusCode = this->rcvReply();
    cout << this->reply;

    // exit if coudnt connect
    if (statusCode == 530)
    {
        cout << "Passive mode refused." << endl;
        return -1;
    }
    
    // cleaning the response to get the ip and port
    vector<string> response = tokenizer.tokenize(this->reply.c_str(), ' ');
    string ipPort = response.back();
    ipPort.erase(remove(ipPort.begin(), ipPort.end(), ' '), ipPort.end());
    ipPort.erase(remove(ipPort.begin(), ipPort.end(), ')'), ipPort.end());
    ipPort.erase(remove(ipPort.begin(), ipPort.end(), '('), ipPort.end());

    response = tokenizer.tokenize(ipPort.c_str(), ',');

    // the port part
    int port = 256 * stoi(response.at(4)) + stoi(response.back());

    // the ip part
    string dataIp = "";
    for (int i = 0; i < 3; i++)
    {
        dataIp += (response.at(i) + ".");
    }
    dataIp += response.at(3);

    Socket *sock = new Socket(port);
    return sock->getClientSocket((char *)dataIp.c_str());
}

// close socket if connected
void Ftp::closeSocket(int socketSD)
{
    if (socketSD != -1)
    {
        close(socketSD);
    }
}

// returns the status of command socket connection
bool Ftp::Active()
{
    return this->commandSD != -1;
}

// quit ftp
void Ftp::ExecuteQuit()
{
    if (this->Active())
    {
        this->sendCommand("QUIT");
        this->rcvReply();
        cout << this->reply;
        this->commandSD = -1;
    }
}

// retrive the server name
string Ftp::getServerName(){
    return this->serverName;
}