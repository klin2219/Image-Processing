make:
	g++ -std=c++11 -o project2.out src/*.cpp

clean:
	del project2.out
	del *.o
