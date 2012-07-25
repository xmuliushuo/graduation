# Makefile

Client: main.o utils.o
	g++ main.o utils.o -lpthread -lmysqlclient -o Client
main.o: main.cc main.h
	g++ -c -g main.cc
utils.o: utils.cc main.h
	g++ -c -g utils.cc
clean:
	rm *.o
