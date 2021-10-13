main:
	g++ -std=c++2a  main.cpp Searcher.cpp IndexMaker.cpp -fsanitize=undefined -Wall -o main

test:
	g++ -std=c++2a  main.cpp Searcher.cpp IndexMaker.cpp -O1 -Wall -o test
	./test test

clean:
	rm -f *.o main clean
