#include "CommandExecutor.h"

int main(int argc, char* argv[])
{
    CommandExecutor cmd;
    string command = "";
    if (argc == 2)
    {
        command = argv[1];
        command = "open " + command + " 21";
    }
    
    do
    {
        // execute command
        if (command != "")
        {
            cmd.Execute(command);
        }

        // prompt for command
        cout << "ftp>";
        getline(cin, command, '\n');
    } while (command != "quit" && command != "exit");

    // Execute "quit" command
    cmd.Execute("quit");

    return 0;
}