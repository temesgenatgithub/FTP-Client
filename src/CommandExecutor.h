#ifndef _COMMANDEXECUTOR_H_
#define _COMMANDEXECUTOR_H_

#include <map>
#include <vector>
#include <iterator>

#include "Ftp.h"
#include "Tokenizer.h"

using namespace std;

class CommandExecutor
{
public:
	CommandExecutor();
	~CommandExecutor();
	void Execute(string command);
private:
	int commandMapper(string command);
	void executeOpen(vector<string> commandV);
	void executeCd(vector<string> commandV);
	void executeGet(vector<string> commandV);
	void executePut(vector<string> commandV);
	void executeLs(vector<string> commandV);
	void executeClose(vector<string> commandV);
	void executeQuit(vector<string> commandV);
	
	Ftp ftp;
	Tokenizer tokenizer;
	map<string, int> commandMap;
};
#endif