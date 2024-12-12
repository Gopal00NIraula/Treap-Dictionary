treap_dictionary: main.o
	g++ -o project4 main.o --std=c++17

main.o: main.cpp treapDictionary.h
	g++ -c --std=c++17 main.cpp

clean:
	rm -f *.o project4
