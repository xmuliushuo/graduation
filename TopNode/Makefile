# Makefile

LoadBalancer: main.o ConnPool.o node.o topnode.o utils.o
	g++ main.o ConnPool.o node.o topnode.o utils.o -lpthread -lmysqlclient -o LoadBalancer
main.o: main.cc main.h topnode.h node.h ConnPool.h
	g++ -c -g main.cc
ConnPool.o: ConnPool.cc ConnPool.h main.h
	g++ -c -g ConnPool.cc
node.o: node.cc node.h main.h
	g++ -c -g node.cc
topnode.o: topnode.cc topnode.h main.h node.h ConnPool.h
	g++ -c -g topnode.cc
utils.o: utils.cc main.h
	g++ -c -g utils.cc
clean:
	rm *.o
