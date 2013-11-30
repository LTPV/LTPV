# sudo ldconfig /usr/local/cuda/lib64
NVCC=/usr/local/cuda/bin/nvcc
all:
	make clean
	g++ -g -I /usr/local/cuda/include/ -Wall -pedantic callKernels.cpp main.cpp `pkg-config --libs opencv` -lOpenCL -o main
clean:
	#rm -f ~/.nv/ComputeCache -R
	rm -f Lenna_out.jpg
	rm -f main
	rm -f main.o
	rm -f kernel.o
