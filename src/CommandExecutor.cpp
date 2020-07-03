#include "CommandExecutor.h"

// constructor
CommandExecutor::CommandExecutor()
{
    commandMap.insert(pair<string, int>("open", 1));
    commandMap.insert(pair<string, int>("cd", 2));
    commandMap.insert(pair<string, int>("get", 3));
    commandMap.insert(pair<string, int>("put", 4));
    commandMap.insert(pair<string, int>("ls", 5));
    commandMap.insert(pair<string, int>("close", 6));
    commandMap.insert(pair<string, int>("quit", 7));
}

// destructor
CommandExecutor::~CommandExecutor()
{
}

// returns -1 if invalid command
int CommandExecutor::commandMapper(string command)
{
    map<string, int>::iterator it;
    it = this->commandMap.find(command);
    if (it == commandMap.end())
    {
        return -1;
    }
    return it->second;
}

// commandExecutor function
void CommandExecutor::Execute(string command)
{
    vector<string> commandV = tokenizer.tokenize(command.c_str(), ' ');
    int commandCode = commandMapper(commandV.at(0));
    switch (commandCode)
    {
    case 1:
        this->executeOpen(commandV);
        break;
    case 2:
        this->executeCd(commandV);
        break;
    case 3:
        this->executeGet(commandV);
        break;
    case 4:
        this->executePut(commandV);
        break;
    case 5:
        this->executeLs(commandV);
        break;
    case 6:
        this->executeClose(commandV);
        break;
    case 7:
        this->executeQuit(commandV);
        break;
    case -1:
        cout << "?Invalid command" << endl;
        break;
    }
}

// open command
void CommandExecutor::executeOpen(vector<string> commandV)
{
    // check if already open
    if (ftp.Active())
    {
        cout << "Already connected to " << ftp.getServerName() << ", use close first." << endl;
        return;
    }

    if (commandV.size() == 1)
    {
        string ip;
        cout << "(to)";
        getline(cin, ip, '\n');
        if (ip == "")
        {
            cout << "usage: open host-name [port]" << endl;
        }
        else
        {
            Execute("open " + ip);
        }
    }
    else if (commandV.size() == 2)
    {
        ftp.ExecuteOpen(commandV.at(1), 21);
    }
    else
    {
        ftp.ExecuteOpen(commandV.at(1), stoi(commandV.at(2)));
    }
}

// cd command
void CommandExecutor::executeCd(vector<string> commandV)
{
    // check if there is connection yet
    if (!ftp.Active())
    {
        cout << "Not connected." << endl;
        return;
    }
    
    if (commandV.size() == 1)
    {
        string dir;
        cout << "(remote-directory) ";
        getline(cin, dir, '\n');
        if (dir == "")
        {
            cout << "usage: cd remote-directory" << endl;
        }
        else
        {
            ftp.ExecuteCd(dir);
        }
    }
    else
    {
        ftp.ExecuteCd(commandV.at(1));
    }
}

// get file
void CommandExecutor::executeGet(vector<string> commandV)
{
    // check if there is connection yet
    if (!ftp.Active())
    {
        cout << "Not connected." << endl;
        return;
    }

    if (commandV.size() == 1)
    {
        string remote;
        cout << "(remote-file) ";
        getline(cin, remote, '\n');
        if (remote == "")
        {
            cout << "usage: get remote-file [ local-file ]" << endl;
        }
        else
        {
            vector<string> remoteV = tokenizer.tokenize((char *)remote.c_str(), ' ');
            if (remoteV.size() >= 2)
            {
                ftp.ExecuteGet(remoteV.at(0), remoteV.at(1));
            }
            else
            {
                string local;
                cout << "(local-file) ";
                getline(cin, local, '\n');
                if (local == "")
                {
                    cout << "usage: get remote-file [ local-file ]" << endl;
                }
                else
                {
                    ftp.ExecuteGet(remote, local);
                }
            }
        }
    }
    else
    {
        string remoteFile = "";
        string localFile = commandV.at(1);
        if (commandV.size() >= 3)
        {
            localFile = commandV.at(2);
        }
        remoteFile = commandV.at(1);
        ftp.ExecuteGet(remoteFile, localFile);
    }
}

// put file
void CommandExecutor::executePut(vector<string> commandV)
{
    // check if there is connection yet
    if (!ftp.Active())
    {
        cout << "Not connected." << endl;
        return;
    }

    if (commandV.size() == 1)
    {
        string local;
        cout << "(local-file) ";
        getline(cin, local, '\n');
        if (local == "")
        {
            cout << "usage: put local-file [ remote-file ]" << endl;
        }
        else
        {
            vector<string> localV = tokenizer.tokenize(local.c_str(), ' ');
            if (localV.size() >= 2)
            {
                ftp.ExecutePut(localV.at(0), localV.at(1));
            }
            else
            {
                string remote;
                cout << "(remote-file) ";
                getline(cin, remote, '\n');
                if (remote == "")
                {
                    cout << "usage: put local-file [ remote-file ]" << endl;
                }
                else
                {
                    ftp.ExecutePut(local, remote);
                }
            }
        }
    }
    else
    {
        string localFile = "";
        string remoteFile = commandV.at(1);
        if (commandV.size() >= 3)
        {
            remoteFile = commandV.at(2);
        }
        localFile = commandV.at(1);
        ftp.ExecutePut(localFile, remoteFile);
    }
}

// ls command
void CommandExecutor::executeLs(vector<string> commandV)
{
    // check if there is connection yet
    if (!ftp.Active())
    {
        cout << "Not connected." << endl;
        return;
    }

    string path = "";
    if (commandV.size() > 1)
    {
        path = commandV.at(1);
    }

    ftp.ExecuteLs(path);
}

// close command
void CommandExecutor::executeClose(vector<string> commandV)
{
    // check if there is connection yet
    if (!ftp.Active())
    {
        cout << "Not connected." << endl;
        return;
    }
    ftp.ExecuteQuit();
}

// quit command
void CommandExecutor::executeQuit(vector<string> commandV)
{
    ftp.ExecuteQuit();
}
