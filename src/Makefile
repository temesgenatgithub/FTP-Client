ftp: Ftp.o CommandExecutor.o
	g++ Ftp.o Socket.o Tokenizer.o Timer.o CommandExecutor.o Driver.cpp -o ftp

Ftp.o: Ftp.cpp Ftp.h Socket.o Tokenizer.o Timer.o
	g++ -c Ftp.h Ftp.cpp

Socket.o: Socket.h Socket.cpp
	g++ -c Socket.h Socket.cpp 

Tokenizer.o: Tokenizer.h Tokenizer.cpp
	g++ -c Tokenizer.h Tokenizer.cpp

Timer.o: Timer.h Timer.cpp
	g++ -c Timer.h Timer.cpp

CommandExecutor.o: CommandExecutor.h CommandExecutor.cpp
	g++ -c CommandExecutor.h CommandExecutor.cpp

clean:
	rm *.o ftp