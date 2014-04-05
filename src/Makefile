all:
	g++ -W -Wall -Wextra -std=c++11 -g -ggdb -shared -o libLTPV.so -fPIC libLTPV.cc -lOpenCL -I/usr/local/cuda/include/
clean:
	rm -f libLTPV.so
checklib:
	clang++ -W -Wall -Wextra -std=c++11 -g -ggdb -o main -fPIC libLTPV.cc -lOpenCL -I/usr/local/cuda/include/ -ldl
