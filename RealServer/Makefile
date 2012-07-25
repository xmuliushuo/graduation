# Makefile

RealServer: main.o ConnPool.o realserver.o utils.o
	g++ main.o ConnPool.o realserver.o utils.o -lpthread -lmysqlclient -o RealServer
main.o: main.cc realserver.h main.h ConnPool.h
	g++ -c -g main.cc
ConnPool.o: ConnPool.cc ConnPool.h main.h
	g++ -c -g ConnPool.cc
realserver.o: realserver.cc realserver.h main.h ConnPool.h
	g++ -c -g realserver.cc
utils.o: utils.cc main.h
	g++ -c -g utils.cc
clean:
	rm *.o
